#ifndef __NOS_H__
#define __NOS_H__

#include "Segment.h"
#include "GlobalParams.h"
#include "BusProcessingElement.h"
#include <systemc.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <string.h>
#include <algorithm>
#include <assert.h>

using namespace std;

//struct NodeOnBus{
//    int segment_id;
//    int pe_id;
//};

SC_MODULE(NoS) {

    //SC_HAS_PROCESS(NoS);

	sc_in_clk clk;
	sc_in<bool> reset;


    // Signals and variables for segmented bus
    // sc_signal<Spike> **inter_segment;

    int number_of_segments;
    int number_of_pes;

    Segment **segments;
	BusProcessingElement **pes;

    //vector<pair<int, int>> segment_inter_connection_table;
    //vector<pair<NodeOnBus, NodeOnBus>> pe_inter_connection_table;

	// Methods
	void buildNetwork();
    void reloadSwitchConfiguration();

    // Constructor
    SC_CTOR(NoS) {
        buildNetwork();

        SC_METHOD(reloadSwitchConfiguration);
        sensitive << clk.neg();
    }

private:
	void removeComment(istream &in);
	void loadNodeConfiguration();
	void loadSegmentConfiguration();
	void loadMasterConnection();
	void loadSlaveConnection();
};

#endif
