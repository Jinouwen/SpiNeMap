#include "Segment.h"

void Segment::module_initialisation() {

	switches = new Switch*[number_of_switches];

	for (int i = 0; i < number_of_switches; i++) {
        char module_name[10];
        memset(module_name,0,10);
        sprintf(module_name,"SW%d",i);
		switches[i] = new Switch(module_name, i);
	}
	if (GlobalParams::output_mode == DEBUG_MODE) {
		cout << "Switches in Segment "<< id << " are initialised." << endl;
	}

}

void Segment::connection_configuration() {

	// Configuring interconnections between switches

	for (int i = 0; i < number_of_switches - 1; i++) {
		switches[i]->left_in(switch_left_in[i]);
		switches[i]->left_out(switch_left_out[i]);
		//switches[i]->mid_in(switch_from_module[i]);
		switches[i]->mid_out(switch_to_module[i]);
		switches[i]->right_in(switch_left_out[i + 1]);
		switches[i]->right_out(switch_left_in[i + 1]);
	}
	switches[number_of_switches - 1]->left_in(switch_left_in[number_of_switches - 1]);
	switches[number_of_switches - 1]->left_out(switch_left_out[number_of_switches - 1]);
	//switches[number_of_switches - 1]->mid_in(switch_from_module[number_of_switches - 1]);
	switches[number_of_switches - 1]->mid_out(switch_to_module[number_of_switches - 1]);
	switches[number_of_switches - 1]->right_in(right_edge_in);
	switches[number_of_switches - 1]->right_out(right_edge_out);

	if (GlobalParams::output_mode == DEBUG_MODE) {
		cout << "Connections in Segment "<< id << " are established." << endl;
	}

}
