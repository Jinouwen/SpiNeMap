#include "BusProcessingElement.h"

void BusProcessingElement::loadTraffic() {
	ifstream traffic_file;
	char file_name[20];
	memset(file_name, 0, 20);
	sprintf(file_name, "traffic/%d.tr", id);
	traffic_file.open(file_name, ios::in);

	if (traffic_file.is_open()) {
		while (traffic_file.good()) {
			int temp;
			traffic_file >> temp;
			spiking_time_list.push_back(temp);
		}

		if (GlobalParams::output_mode == DEBUG_MODE) {
			cout << "PE " << id << " spikes at:" << endl;
			for (std::deque<int>::const_iterator iter = spiking_time_list.cbegin(); iter != spiking_time_list.cend(); iter++) {
				cout << (*iter) << '\t';
			}
			cout << endl;

			cout << "PE " << id << ": traffic is loaded.\n";
		}

        out.bind(output_signal);

	}
	else
	{
		out.bind(empty_signal);
		if (GlobalParams::output_mode == DEBUG_MODE) {
			cout << "Traffic file for PE " << id << " doesn't exist!\n";
		}
	}


}

void BusProcessingElement::generateSpike() {
	if (counter >= 0) {
		counter++;
	}
	// Spike!
	if (counter == spike_time_point) {
        
	    cout << "Reached Here";
		Spike spike;
		spike.src_id = id;
		spike.spike_id = spike_counter;
		spike.spiking_time = (int)sc_time_stamp().to_double()/GlobalParams::clock_period_ps;
		spike.intra_seg_hop_num = 0;
		spike.inter_seg_hop_num = 0;

		spike_counter++;
		out = spike;

		if (spike.spiking_time!=0){
			if (GlobalParams::output_mode == DEBUG_MODE) {
				cout << "Node ";
	    		cout << id << " at time " << sc_time_stamp() << " spiked!" << endl;
			}
			else if (GlobalParams::output_mode == EX_STAT_MODE){
				cout<<"S,"<<id<<","<<sc_time_stamp().to_double()/GlobalParams::clock_period_ps<<endl;
			}
			else{
				stats.generateSpike(spike);
			}
		}

		if (!spiking_time_list.empty()) {
			spike_time_point = spiking_time_list.front();
			spiking_time_list.pop_front();
		}
		else {
			counter = -1;
		}
	}
	// Write empty spike to the out put
	else {
		Spike spike;
		spike.src_id = -1;
		out = spike;
	}
}

void BusProcessingElement::processSpike() {
    
    cout << "Reached Here";
	for (int i = 0; i < number_of_input; i++) {
		Spike spike = in[i].read();
		if ((spike.src_id) != -1 && (spike.spiking_time != 0)) {
			if (GlobalParams::output_mode == DEBUG_MODE) {
				cout << "Node ";
    			cout << id << " at time " << sc_time_stamp() << " received a spike from ";
    			cout << spike.src_id << " with " << spike.intra_seg_hop_num << " hops." << endl;
			}
			else if (GlobalParams::output_mode == EX_STAT_MODE){
				cout<<"R,"
				    <<id<<","
				    <<sc_time_stamp().to_double()/GlobalParams::clock_period_ps<<","
					<<spike.src_id<<","
					<<spike.spiking_time<<","
					<<spike.intra_seg_hop_num<<","
					<<spike.inter_seg_hop_num<<endl;
			}
			else{
				stats.receiveSpike(spike);
			}
        }
	}
}

void BusProcessingElement::processClock() {
	cout << "Reached Here";
    generateSpike();
	
    processSpike();
}
