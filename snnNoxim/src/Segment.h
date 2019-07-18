#ifndef __SEGMENT_H__
#define __SEGMENT_H__

#include <systemc.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <string>
#include <deque>

#include "BusProcessingElement.h"
#include "Switch.h"
#include "DataStructs.h"
#include "Utils.h"

#define FIRST_LAYER 0
#define MIDDLE_LAYER 1
#define LAST_LAYER 2
#define ONE_LAYER 3

SC_MODULE(Segment) {

	// Defining clock

	//sc_in_clk clk;

	// Defining variables
	int id;
	//unsigned char type;
	//int number_of_pes;
	int number_of_switches;
	//int number_of_masters;
	//int number_of_slaves;

	// Defining configuration for segment
	//struct SegmentParameter
	//{
	//	int number_of_pes;
	//	int number_of_masters;
	//	int *output_enable_list;

	//	// Spiking time of a specific node
	//	std::deque <int>* spiking_time_list;

	//	inline void allocate_memory() {
	//		assert(number_of_pes > number_of_masters);
	//		output_enable_list = new int[number_of_pes];
	//		spiking_time_list = new std::deque<int>[number_of_masters];
	//	}

	//};

	//SegmentParameter parameter;
	//std::deque<int> empty_deque;

	//Defining dynamic numbers of signals connecting to the switches
	sc_signal<Spike> right_edge_in, right_edge_out;
	sc_signal<Spike> *switch_left_in;
	sc_signal<Spike> *switch_left_out;
	sc_signal<Spike> *switch_from_module;
	sc_signal<Spike> *switch_to_module;

	//Defining processing elements instances
	//BusProcessingElement **pes;

	//Defining switches
	Switch **switches;

	//Defining functions
	void load_configuration();
	void module_initialisation();
	void connection_configuration();

	Segment(sc_module_name module_name, int _segment_id, int _number_of_switches):sc_module(module_name) {

		id = _segment_id;
		//type = segment_type;

		//load_configuration();

		//number_of_pes = parameter.number_of_pes;
		number_of_switches = _number_of_switches;
		//number_of_masters = parameter.number_of_masters;
		//number_of_slaves = number_of_pes-number_of_masters;

		//Defining dynamic numbers of signals connecting to the switches
		switch_left_in = new sc_signal<Spike>[number_of_switches];
		switch_left_out = new sc_signal<Spike>[number_of_switches];
		switch_from_module = new sc_signal<Spike>[number_of_switches];
		switch_to_module = new sc_signal<Spike>[number_of_switches];

		module_initialisation();
		connection_configuration();
	}


};

#endif // !__SEGMENT_H__
