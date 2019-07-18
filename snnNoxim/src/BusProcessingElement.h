#pragma once
#ifndef __BUS_MODULE_H__
#define __BUS_MODULE_H__

#define MASTER_MASK 0x02
#define SLAVE_MASK 0x01

#define BLOCKED_MODE 0x00
#define SLAVE_MODE 0x01
#define MASTER_MODE 0x02
#define HYBRID_MODE 0x03


#include "GlobalParams.h"
#include "DataStructs.h"
#include "Utils.h"
#include "BusStats.h"
#include <assert.h>
#include <deque>
#include <vector>
#include <stdlib.h>
#include <systemc.h>
#include <string.h>
#include <fstream>
#include <iostream>

SC_MODULE(BusProcessingElement) {

    SC_HAS_PROCESS(BusProcessingElement);

    // Port defination
    sc_in_clk clk;
    sc_in<Spike>* in;
    sc_out<Spike> out;
	sc_signal<Spike> empty_signal, output_signal;

    // Variables
    int id;
    int spike_time_point; // For demo only
    int counter;
	int number_of_input;
	int spike_counter;
    bool spike_finished = false;
    std::deque<int> spiking_time_list;

    // Logging
    // std::vector<MasterCommItem> spiking_history;
    // std::vector<SlaveCommItem> receiving_history;
    // Statistic
    BusStats stats;

	void loadTraffic();
	void generateSpike();
    void processSpike();
	void processClock();

    BusProcessingElement(sc_module_name module_name, int _module_id,int _number_of_inputs)
        : sc_module(module_name) {

        id = _module_id;
		in = new sc_in<Spike>[_number_of_inputs];
		number_of_input = _number_of_inputs;
        stats.configure(id);
		loadTraffic();

		if (!spiking_time_list.empty()) {
			spike_time_point = spiking_time_list.front();
			counter = 0;
			spike_counter = 0;
			spiking_time_list.pop_front();
		}
		else {
			counter = -1;
			spike_time_point = 0;
		}


        SC_METHOD(processClock);
        sensitive << clk.pos();

    }
};

#endif // !__PROCESSING_ELEMENT_DEMO_H__
