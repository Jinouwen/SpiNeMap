#include "BusStats.h"

void BusStats::configure(int _node_id) {
    node_id = _node_id;

    last_received_spike_time = 0;
    last_generated_spike_time = 0;
    disorder_spike_count = 0;

    // max_delay = 0;
    // min_delay = 100;
    // sum_delay = 0;
    //
    // max_isi_distortion = 0;
    // min_isi_distortion = 100;
    // sum_isi_distortion = 0;
}

void BusStats::generateSpike(Spike spike) {
    MasterCommItem master_item;
    master_item.spike_id = spike.spike_id;
    master_item.spiking_time = spike.spiking_time;
    spiking_history.push_back(master_item);
}

void BusStats::receiveSpike(Spike spike) {
    SlaveCommItem slave_item;
    slave_item.src_id = spike.src_id;
    slave_item.spike_id = spike.spike_id;
    slave_item.spiking_time = spike.spiking_time;
    slave_item.intra_seg_hop_num = spike.intra_seg_hop_num;
    slave_item.inter_seg_hop_num = spike.inter_seg_hop_num;
    slave_item.receiving_time = sc_time_stamp().to_double();
    // receiving_history.push_back(slave_item);

    // Delay computation
    int delay = slave_item.receiving_time - slave_item.spiking_time;
    slave_item.delay = delay / GlobalParams::clock_period_ps;
    // if (delay > max_delay){
    //     max_delay = delay;
    // }
    // if (delay < min_delay){
    //     min_delay = delay;
    // }
    // sum_delay =  sum_delay + delay;

    // ISI computation
    int isi_distortion;
    if (last_received_spike_time == 0) {
        isi_distortion = 0;
    } else {
        isi_distortion = (slave_item.receiving_time - last_received_spike_time) - (slave_item.spiking_time - last_generated_spike_time);
    }
    slave_item.isi_distortion = isi_distortion / GlobalParams::clock_period_ps;
    // int isi = (slave_item.receiving_time - last_received_spike_time) - (slave_item.spiking_time - last_generated_spike_time);
    // if (isi > max_isi_distortion){
    //     max_isi_distortion = isi;
    // }
    // sum_isi_distortion = sum_isi_distortion + isi;

    // Spike disorder computation
    if (slave_item.spiking_time < last_generated_spike_time) {
        disorder_spike_count++;
    }

    last_received_spike_time = slave_item.receiving_time;
    last_generated_spike_time = slave_item.spiking_time;

    // Save the log item
    receiving_history.push_back(slave_item);
}

int BusStats::getMaxDelay() {
    int max_delay = 0;
    for (SlaveCommItem item : receiving_history) {
        if (item.delay > max_delay) {
            max_delay = item.delay;
        }
    }
    return max_delay;
}

int BusStats::getMinDelay() {
    int min_delay = 100;
    for (SlaveCommItem item : receiving_history) {
        if (item.delay < min_delay) {
            min_delay = item.delay;
        }
    }
    return min_delay;
}

double BusStats::getAverageDelay() {
    if (receiving_history.empty()){
        return 0;
    }
    else {
        int avg_delay = 0;
        for (SlaveCommItem item : receiving_history) {
            avg_delay = avg_delay + item.delay;
        }
        return avg_delay * 1.0 / receiving_history.size();
    }
}

int BusStats::getMaxIsiDistortion() {
    int max_isi_distortion = 0;
    for (SlaveCommItem item : receiving_history) {
        if (abs(item.isi_distortion) > abs(max_isi_distortion)) {
            max_isi_distortion = item.isi_distortion;
        }
    }
    return max_isi_distortion;
}
int BusStats::getMinIsiDistortion() {
    int min_isi_distortion = 100;
    for (SlaveCommItem item : receiving_history) {
        if (abs(item.isi_distortion) < abs(min_isi_distortion)) {
            min_isi_distortion = item.isi_distortion;
        }
    }
    return min_isi_distortion;
}
double BusStats::getAverageIsiDistortion() {
    if (receiving_history.empty()) {
        return 0;
    } else {
        int avg_isi_distortion = 0;
        for (SlaveCommItem item : receiving_history) {
            avg_isi_distortion = avg_isi_distortion + item.isi_distortion;
        }
        return avg_isi_distortion * 1.0 / receiving_history.size();
    }
}

int BusStats::getMaxIntraHop() {
    int max_intra_hop = 0;
    for (SlaveCommItem item : receiving_history) {
        if (item.intra_seg_hop_num > max_intra_hop) {
            max_intra_hop = item.intra_seg_hop_num;
        }
    }
    return max_intra_hop;
}

int BusStats::getMinIntraHop() {
    int min_intra_hop = 10000;
    for (SlaveCommItem item : receiving_history) {
        if (item.intra_seg_hop_num < min_intra_hop) {
            min_intra_hop = item.intra_seg_hop_num;
        }
    }
    return min_intra_hop;
}

double BusStats::getAverageIntraHop() {
    if (receiving_history.empty()){
        return 0;
    }
    else{
        int avg_intra_hop = 0;
        for (SlaveCommItem item : receiving_history) {
            avg_intra_hop = avg_intra_hop + item.intra_seg_hop_num;
        }
        return avg_intra_hop * 1.0 / receiving_history.size();
    }
}

int BusStats::getSpikingCount() {
    return spiking_history.size();
}
int BusStats::getReceivingCount() {
    return receiving_history.size();
}
int BusStats::getDisorderCount() {
    return disorder_spike_count;
}
