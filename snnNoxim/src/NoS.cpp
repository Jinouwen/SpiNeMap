#include "NoS.h"

void NoS::removeComment(istream &in) {
	while (in.peek() == '\n' || in.peek() == '\t' || in.peek() == '#') {
		in.ignore(10240, '\n');
	}
}

void NoS::reloadSwitchConfiguration() {
    ifstream switch_reload;
    switch_reload.open("reload.conf", ios::in);

    if (switch_reload.is_open()){
        removeComment(switch_reload);
        int bus_id, switch_id, input_conf, output_conf;
        while (switch_reload.good()) {
            switch_reload>>bus_id >> switch_id >> input_conf >> output_conf;
            segments[bus_id] -> switches[switch_id] -> input_enable = input_conf;
            segments[bus_id] -> switches[switch_id] -> output_enable = output_conf;
            removeComment(switch_reload);
        }
    }
}

void NoS::loadNodeConfiguration() {
	ifstream pe_conf;
	pe_conf.open("pe.conf", ios::in);

	// Fail to open the file, exit!
	if (!pe_conf.is_open()) {
		cout << "Cannot open the PE configuration file, exit!\n";
		assert(false);
	}

	removeComment(pe_conf);

	// Begin to load PE configrations
	int pe_id;
	int number_of_input;

	pe_conf >> number_of_pes;
	pes = new BusProcessingElement*[number_of_pes];
    if (GlobalParams::output_mode != EX_STAT_MODE){
	    cout << number_of_pes << " PEs are defined. \n";
    }
    else{
        cout << "PE:"<<number_of_pes<<endl;
    }
	removeComment(pe_conf);

	for (int i = 0; i < number_of_pes; i++) {
		pe_conf >> pe_id >> number_of_input;
		if (pe_id < number_of_pes) {
			// Initialise PE instance
			char module_name[10];
			memset(module_name, 0, 10);
			sprintf(module_name, "PE%d", pe_id);
			pes[pe_id] = new BusProcessingElement(module_name, pe_id, number_of_input);
			pes[pe_id]->clk(clk);

			if (GlobalParams::output_mode == DEBUG_MODE){
				cout << "PE " << pe_id << " has " << number_of_input << " input ports.\n";
			}

			removeComment(pe_conf);
		}
		else {
			cout << "Errors in seting PEs, exit!\n";
			assert(false);
		}
	}

	pe_conf.close();
	if(GlobalParams::output_mode != EX_STAT_MODE){
		cout << "PE configuration loaded and instances are initialised!\n\n";
	}
}

void NoS::loadSegmentConfiguration() {
	ifstream segment_conf;
	segment_conf.open("segment.conf", ios::in);

	// Tackle the condition failing to load the configuration
	if (!segment_conf.is_open()) {
		cout << "Cannot open the segment configuration file, exit!\n";
		assert(false);
	}

	removeComment(segment_conf);

	// Begin to load segment configurations
	int number_of_switches;
	int segment_id, switch_id;
	int switch_input, switch_output;

	// Initialise segment instance array
	segment_conf >> number_of_segments;
	segments = new Segment*[number_of_segments];
    if (GlobalParams::output_mode != EX_STAT_MODE){
        cout << number_of_segments << " segments are defined. \n";
    }
    else{
        cout << "SEG:"<<number_of_segments<<endl;
    }


	for (int i = 0; i < number_of_segments; i++) {
		removeComment(segment_conf);
		segment_conf >> segment_id >> number_of_switches;
		if (segment_id < number_of_segments && segment_id==i) {

			// Initialise segment instances
			char module_name[10];
			memset(module_name, 0, 10);
			sprintf(module_name, "SGM%d", segment_id);
			segments[segment_id] = new Segment(module_name, segment_id, number_of_switches);
			if (GlobalParams::output_mode == DEBUG_MODE){
				cout << "Segment " << segment_id << " has " << number_of_switches << " switches.\n";
			}

			for (int j = 0; j < number_of_switches; j++) {
				removeComment(segment_conf);
				segment_conf >> switch_id >> switch_input >> switch_output;
				if (switch_id < number_of_switches && switch_input < 8 && switch_output < 8 && switch_id == j) {

					// Configure switch input and output
					segments[segment_id]->switches[switch_id]->input_enable = switch_input;
					segments[segment_id]->switches[switch_id]->output_enable = switch_output;
					if (GlobalParams::output_mode == DEBUG_MODE){
						cout << "Switch " << segment_id << ":" << switch_id << " is set to ";
						cout << switch_input << ":" << switch_output << ".\n";
					}

				}
				else {
					cout << "Swtich configuration error, exit!\n";
					assert(false);
				}
			}
		}
		else {
			cout << "Segment configuration error, exit!\n";
			assert(false);
		}
	}

	segment_conf.close();
	if(GlobalParams::output_mode != EX_STAT_MODE){
		cout << "Segment configuration loaded!\n\n";
	}
}

void NoS::loadMasterConnection() {
	ifstream master_connection_conf;
	master_connection_conf.open("master_connection.conf", ios::in);

	if (!master_connection_conf.is_open()) {
		cout << "Cannot open the master configuration file, exit!\n";
		assert(false);
	}

	int pe_id, segment_id, switch_id;
    removeComment(master_connection_conf);

	while (master_connection_conf.good()) {
		master_connection_conf >> pe_id >> segment_id >> switch_id;

		// Chech validity
		if (pe_id < number_of_pes
			&& segment_id < number_of_segments
			&& switch_id < segments[segment_id]->number_of_switches) {

			// Build connections!
            segments[segment_id]->switches[switch_id]->mid_in.bind(pes[pe_id]->output_signal);
			//pes[pe_id]->out.bind(segments[segment_id]->switch_from_module[switch_id]);
			if (GlobalParams::output_mode == DEBUG_MODE){
				cout << "Node " << pe_id << " is connected to Switch " << segment_id << ":" << switch_id << " as a MASTER.\n";
			}
		}
		else {
			cout << "Bad connection encountered, exit!\n";
			assert(false);
		}
        removeComment(master_connection_conf);
	}

	master_connection_conf.close();
	if(GlobalParams::output_mode != EX_STAT_MODE){
		cout << "Master-to-segment configuration loaded!\n\n";
	}
}

void NoS::loadSlaveConnection() {
	ifstream slave_connection_conf;
	slave_connection_conf.open("slave_connection.conf", ios::in);

	if (!slave_connection_conf.is_open()) {
		cout << "Cannot open the slave configuration file, exit!\n";
		assert(false);
	}

	int segment_id, switch_id, pe_id, port_id;
    removeComment(slave_connection_conf);

	while (slave_connection_conf.good()) {
		slave_connection_conf >> segment_id >> switch_id >> pe_id >> port_id;

		// Chech validity
		if (segment_id < number_of_segments
			&& pe_id < number_of_pes
			&& switch_id < segments[segment_id]->number_of_switches
			&& port_id < pes[pe_id]->number_of_input) {

			// Build connection
            segments[segment_id]->switches[switch_id]->mid_in.bind(segments[segment_id]->switch_from_module[switch_id]);
			pes[pe_id]->in[port_id].bind(segments[segment_id]->switch_to_module[switch_id]);

			if (GlobalParams::output_mode == DEBUG_MODE){
				cout << "Node " << pe_id << ":" << port_id << " is connected to Switch " << segment_id << ":" << switch_id << " as a SLAVE.\n";
			}
		}
        else {
            cout << "Bad connection encountered, exit!\n";
            assert(false);
        }
        removeComment(slave_connection_conf);
	}

	slave_connection_conf.close();

	if(GlobalParams::output_mode != EX_STAT_MODE){
		cout << "Slave-to-segment configuration loaded!\n\n";
	}
}


void NoS::buildNetwork() {
	loadNodeConfiguration();
	loadSegmentConfiguration();
	loadMasterConnection();
	loadSlaveConnection();
}
