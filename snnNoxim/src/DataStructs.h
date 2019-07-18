/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the top-level of Noxim
 */

#ifndef _DATASTRUCS_H__
#define _DATASTRUCS_H__

#include <systemc.h>
#include "GlobalParams.h"

// Coord -- XY coordinates type of the Tile inside the Mesh
class Coord {
  public:
    int x;			// X coordinate
    int y;			// Y coordinate

    inline bool operator ==(const Coord & coord) const {
	return (coord.x == x && coord.y == y);
}};

// FlitType -- Flit type enumeration
enum FlitType {
    FLIT_TYPE_HEAD, FLIT_TYPE_BODY, FLIT_TYPE_TAIL
};


// Payload -- Payload definition
struct Payload {
    sc_uint<32> data;	// Bus for the data to be exchanged

    inline bool operator ==(const Payload & payload) const {
	return (payload.data == data);
}};

// Packet -- Packet definition
struct Packet {
    int src_id;
    int dst_id;
    double timestamp;		// SC timestamp at packet generation
    int size;
    int flit_left;		// Number of remaining flits inside the packet
    int relative_deadline;
    bool use_low_voltage_path;

    // Constructors
    Packet() { }

    Packet(const int s, const int d, const double ts, const int sz) {
	make(s, d, ts, sz);
    }

    void make(const int s, const int d, const double ts, const int sz) {
	src_id = s;
	dst_id = d;
	timestamp = ts;
	size = sz;
	flit_left = sz;
	use_low_voltage_path = false;
    }

    void setDeadline(int deadline) {

        relative_deadline = deadline;

    }
};

// RouteData -- data required to perform routing
struct RouteData {
    int current_id;
    int src_id;
    int dst_id;
    int dir_in;			// direction from which the packet comes from
};

struct ChannelStatus {
    int free_slots;		// occupied buffer slots
    bool available;		//
    inline bool operator ==(const ChannelStatus & bs) const {
	return (free_slots == bs.free_slots && available == bs.available);
    };
};

// NoP_data -- NoP Data definition
struct NoP_data {
    int sender_id;
    ChannelStatus channel_status_neighbor[DIRECTIONS];

    inline bool operator ==(const NoP_data & nop_data) const {
	return (sender_id == nop_data.sender_id &&
		nop_data.channel_status_neighbor[0] ==
		channel_status_neighbor[0]
		&& nop_data.channel_status_neighbor[1] ==
		channel_status_neighbor[1]
		&& nop_data.channel_status_neighbor[2] ==
		channel_status_neighbor[2]
		&& nop_data.channel_status_neighbor[3] ==
		channel_status_neighbor[3]);
    };
};

// Flit -- Flit definition
struct Flit {
    int src_id;
    int dst_id;
    FlitType flit_type;	// The flit type (FLIT_TYPE_HEAD, FLIT_TYPE_BODY, FLIT_TYPE_TAIL)
    int sequence_no;		// The sequence number of the flit inside the packet
    int sequence_length;    // The length of the packet that consists this flit
    int r_time;         // Time that the flit has stayed in a router
    int deadline;      // Attention: this argument stores the absolute deadline of this flit, which equals to F + timestamp
    vector <pair<int, double>> route_time; // Pair of router ID - time received at that router for logging
    Payload payload;	// Optional payload
    double timestamp;	// Time-stamp at packet generation
    double sent_time;   // Time-stamp at flit transmission to router
    double recei_time;  // Time-stamp at flit reception from router
    int hop_no;			// Current number of hops from source to destination
    bool use_low_voltage_path;

    inline bool operator ==(const Flit & flit) const {
	return (flit.src_id == src_id && flit.dst_id == dst_id
		&& flit.flit_type == flit_type
		&& flit.sequence_no == sequence_no
		&& flit.sequence_length == sequence_length
		&& flit.payload == payload && flit.timestamp == timestamp
		&& flit.hop_no == hop_no
		&& flit.use_low_voltage_path == use_low_voltage_path);
}};

typedef struct
{
    string label;
    double value;
} PowerBreakdownEntry;


enum
{
    BUFFER_PUSH_PWR_D,
    BUFFER_POP_PWR_D,
    BUFFER_FRONT_PWR_D,
    BUFFER_TO_TILE_PUSH_PWR_D,
    BUFFER_TO_TILE_POP_PWR_D,
    BUFFER_TO_TILE_FRONT_PWR_D,
    BUFFER_FROM_TILE_PUSH_PWR_D,
    BUFFER_FROM_TILE_FRONT_PWR_D,
    BUFFER_FROM_TILE_POP_PWR_D,
    ANTENNA_BUFFER_PUSH_PWR_D,
    ANTENNA_BUFFER_POP_PWR_D,
    ANTENNA_BUFFER_FRONT_PWR_D,
    ROUTING_PWR_D,
    SELECTION_PWR_D,
    CROSSBAR_PWR_D,
    LINK_R2R_PWR_D,
    LINK_R2H_PWR_D,
    NI_PWR_D,
    WIRELESS_TX,
    WIRELESS_DYNAMIC_RX_PWR,
    WIRELESS_SNOOPING,
    NO_BREAKDOWN_ENTRIES_D
};

enum
{
    TRANSCEIVER_RX_PWR_BIASING,
    TRANSCEIVER_TX_PWR_BIASING,
    BUFFER_ROUTER_PWR_S,
    BUFFER_TO_TILE_PWR_S,
    BUFFER_FROM_TILE_PWR_S,
    ANTENNA_BUFFER_PWR_S,
    LINK_R2H_PWR_S,
    ROUTING_PWR_S,
    SELECTION_PWR_S,
    CROSSBAR_PWR_S,
    NI_PWR_S,
    TRANSCEIVER_RX_PWR_S,
    TRANSCEIVER_TX_PWR_S,
    NO_BREAKDOWN_ENTRIES_S
};

typedef struct
{
    int size;
    PowerBreakdownEntry breakdown[NO_BREAKDOWN_ENTRIES_D+NO_BREAKDOWN_ENTRIES_S];
} PowerBreakdown;

//Spike -- Spike defination
struct Spike {

	int src_id;
	int spike_id;
    double spiking_time;
    int intra_seg_hop_num;
    int inter_seg_hop_num;
	std::string payload;

    inline bool operator ==(const Spike & spike) const{
        return (spike.src_id == src_id &&
				spike.spike_id == spike_id &&
                spike.spiking_time == spiking_time);
    }
};

struct MasterCommItem {
    int spike_id;
    double spiking_time;
};

struct SlaveCommItem {
    int src_id;
    int spike_id;
    double spiking_time;
    double receiving_time;
    int intra_seg_hop_num;
    int inter_seg_hop_num;
    int delay;
    int isi_distortion;
};

#endif
