/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2015 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the command line parser
 */

#include "GlobalParams.h"

int GlobalParams::verbose_mode;
int GlobalParams::output_mode;
bool GlobalParams::log_mode;
string GlobalParams::log_filename;
int GlobalParams::trace_mode;
string GlobalParams::trace_filename;
string GlobalParams::network_type;
vector<int> GlobalParams::rout_only_nodes;
int GlobalParams::dim_x;
int GlobalParams::dim_y;
double GlobalParams::r2r_link_length;
double GlobalParams::r2h_link_length;
int GlobalParams::router_cycle;
int GlobalParams::buffer_depth;
int GlobalParams::flit_size;
int GlobalParams::min_packet_size;
int GlobalParams::max_packet_size;
string GlobalParams::routing_algorithm;
string GlobalParams::routing_filename;
string GlobalParams::selection_strategy;
//string GlobalParams::selection_filename;
double GlobalParams::packet_injection_rate;
double GlobalParams::probability_of_retransmission;
double GlobalParams::locality;
string GlobalParams::traffic_distribution;
string GlobalParams::traffic_filename;
//string GlobalParams::traffic_trace_filename;
bool GlobalParams::cluster_traffic; // indicate whether the input traffic is between clusters or neurons
string GlobalParams::neuron_node_filename;
string GlobalParams::neural_network_filename;
string GlobalParams::config_filename;
string GlobalParams::power_config_filename;
int GlobalParams::spike_step; // the distance between consecutive spikes in term of clock cycles
int GlobalParams::clock_period_ps;
int GlobalParams::simulation_time;
int GlobalParams::reset_time;
int GlobalParams::stats_warm_up_time;
int GlobalParams::rnd_generator_seed;
bool GlobalParams::detailed;
double GlobalParams::dyad_threshold;
unsigned int GlobalParams::max_volume_to_be_drained;
vector <pair <int, double> > GlobalParams::hotspots;
bool GlobalParams::show_buffer_stats;
bool GlobalParams::use_winoc;
bool GlobalParams::use_powermanager;
ChannelConfig GlobalParams::default_channel_configuration;
map<int, ChannelConfig> GlobalParams::channel_configuration;
HubConfig GlobalParams::default_hub_configuration;
map<int, HubConfig> GlobalParams::hub_configuration;
map<int, int> GlobalParams::hub_for_tile;
PowerConfig GlobalParams::power_configuration;
