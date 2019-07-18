#pragma once
#ifndef __SWITCH_H__
#define __SWITCH_H__

#define LEFT_MASK 0b100
#define MID_MASK 0b010
#define RIGHT_MASK 0b001

#include <systemc.h>
#include "DataStructs.h"
#include "Utils.h"

SC_MODULE(Switch) {

	SC_HAS_PROCESS(Switch);

    sc_in <Spike> left_in, mid_in, right_in;
	sc_out <Spike> left_out, mid_out, right_out;
	sc_signal <Spike> left_out_signal, mid_out_signal, right_out_signal;

	int input_enable;
	int output_enable;
	int id;

	void updatePort();

	int getSwitchId();

	// Constructor
	Switch(sc_module_name module_name, int _id): sc_module(module_name) {

		id = _id;
		output_enable = 0b000;
		input_enable = 0b000;

		SC_METHOD(updatePort);
        sensitive << left_in << mid_in << right_in;

	}
};

#endif
