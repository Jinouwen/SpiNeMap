#include "GlobalBusStats.h"

GlobalBusStats::GlobalBusStats(const NoS *_nos) {
    nos = _nos;
    node_number = nos->number_of_pes;
}

int GlobalBusStats::getMaxDelay(int _node_id) {
    return nos->pes[_node_id]->stats.getMaxDelay();
}

int GlobalBusStats::getMaxDelay() {
    int max_delay = 0;
    for (int i = 0; i < node_number; i++) {
        if (getMaxDelay(i) > max_delay) {
            max_delay = getMaxDelay(i);
        }
    }
    return max_delay;
}

int GlobalBusStats::getMinDelay(int _node_id) {
    return nos->pes[_node_id]->stats.getMinDelay();
}

int GlobalBusStats::getMinDelay() {
    int min_delay = 100;
    for (int i = 0; i < node_number; i++) {
        if (getMinDelay(i) < min_delay) {
            min_delay = getMinDelay(i);
        }
    }
    return min_delay;
}

double GlobalBusStats::getAverageDelay(int _node_id) {
    return nos->pes[_node_id]->stats.getAverageDelay();
}

double GlobalBusStats::getAverageDelay() {
    double avg_delay = 0;
    for (int i = 0; i < node_number; i++) {
        avg_delay = avg_delay + getAverageDelay(i);
    }
    return avg_delay / node_number;
}

int GlobalBusStats::getMaxIsiDistortion(int _node_id) {
    return nos->pes[_node_id]->stats.getMaxIsiDistortion();
}

int GlobalBusStats::getMaxIsiDistortion() {
    int max_isi_distortion = 0;
    for (int i = 0; i < node_number; i++) {
        if (abs(getMaxIsiDistortion(i)) > abs(max_isi_distortion)) {
            max_isi_distortion = getMaxIsiDistortion(i);
        }
    }
    return max_isi_distortion;
}

int GlobalBusStats::getMinIsiDistortion(int _node_id) {
    return nos->pes[_node_id]->stats.getMinIsiDistortion();
}

int GlobalBusStats::getMinIsiDistortion() {
    int min_isi_distortion = 100;
    for (int i = 0; i < node_number; i++) {
        if (abs(getMinIsiDistortion(i)) < abs(min_isi_distortion)) {
            min_isi_distortion = getMinIsiDistortion(i);
        }
    }
    return min_isi_distortion;
}

double GlobalBusStats::getAverageIsiDistortion(int _node_id) {
    return nos->pes[_node_id]->stats.getAverageIsiDistortion();
}

double GlobalBusStats::getAverageIsiDistortion() {
    double avg_isi_distortion = 0;
    for (int i = 0; i < node_number; i++) {
        avg_isi_distortion = avg_isi_distortion + getAverageIsiDistortion(i);
    }
    return avg_isi_distortion / node_number;
}

int GlobalBusStats::getMaxIntraHop(int _node_id) {
    return nos->pes[_node_id]->stats.getMaxIntraHop();
}

int GlobalBusStats::getMaxIntraHop() {
    int max_intra_hop = 0;
    for (int i = 0; i < node_number; i++) {
        if (getMaxIntraHop(i) > max_intra_hop) {
            max_intra_hop = getMaxIntraHop(i);
        }
    }
    return max_intra_hop;
}

int GlobalBusStats::getMinIntraHop(int _node_id) {
    return nos->pes[_node_id]->stats.getMinIntraHop();
}

int GlobalBusStats::getMinIntraHop() {
    int min_intra_hop = 10000;
    for (int i = 0; i < node_number; i++) {
        if (getMinIntraHop(i) < min_intra_hop) {
            min_intra_hop = getMinIntraHop(i);
        }
    }
    return min_intra_hop;
}

double GlobalBusStats::getAverageIntraHop(int _node_id) {
    return nos->pes[_node_id]->stats.getAverageIntraHop();
}

double GlobalBusStats::getAverageIntraHop() {
    double avg_intra_hop = 0;
    for (int i = 0; i < node_number; i++) {
        avg_intra_hop = avg_intra_hop + getAverageIntraHop(i);
    }
    return avg_intra_hop / node_number;
}

int GlobalBusStats::getSpikingCount(int _node_id) {
    return nos->pes[_node_id]->stats.getSpikingCount();
}

int GlobalBusStats::getSpikingCount() {
    int count = 0;
    for (int i = 0; i < node_number; i++) {
        count = count + getSpikingCount(i);
    }
    return count;
}

int GlobalBusStats::getReceivingCount(int _node_id) {
    return nos->pes[_node_id]->stats.getReceivingCount();
}

int GlobalBusStats::getReceivingCount() {
    int count = 0;
    for (int i = 0; i < node_number; i++) {
        count = count + getReceivingCount(i);
    }
    return count;
}

int GlobalBusStats::getDisorderCount(int _node_id) {
    return nos->pes[_node_id]->stats.getDisorderCount();
}

int GlobalBusStats::getDisorderCount() {
    int count = 0;
    for (int i = 0; i < node_number; i++) {
        count = count + getDisorderCount(i);
    }
    return count;
}

double GlobalBusStats::getFanOutRate() {
    return getReceivingCount() * 1.0 / getSpikingCount();
}

void GlobalBusStats::dumpSpikingLogOut(int _node_id, ostream &out) {
    for (MasterCommItem item : nos->pes[_node_id]->stats.spiking_history) {
        out << _node_id << "," << item.spike_id << "," << item.spiking_time << endl;
    }
}

void GlobalBusStats::dumpSpikingLogOut(ostream &out) {
    for (int i = 0; i < node_number; i++) {
        dumpSpikingLogOut(i, out);
    }
}

void GlobalBusStats::dumpReceivingLogOut(int _node_id, ostream &out) {
    for (SlaveCommItem item : nos->pes[_node_id]->stats.receiving_history) {
        out << _node_id << "," << item.src_id << "," << item.spike_id << "," << item.spiking_time << "," << item.receiving_time << "," << item.intra_seg_hop_num << "," << item.inter_seg_hop_num << "," << item.delay << "," << item.isi_distortion << endl;
    }
}

void GlobalBusStats::dumpReceivingLogOut(ostream &out) {
    for (int i = 0; i < node_number; i++) {
        dumpReceivingLogOut(i, out);
    }
}

void GlobalBusStats::showStats(ostream &out) {
    out << "% Total generated spikes: " << getSpikingCount() << endl;
    out << "% Total received spikes: " << getReceivingCount() << endl;
    out << "% Total disorder: " << getDisorderCount() << endl;
    out << "% Latency: Max: " << getMaxDelay() << " Min: " << getMinDelay() << " Avg.: " << getAverageDelay() << endl;
    out << "% ISI distortion: Max: " << getMaxIsiDistortion() << " Min: " << getMinIsiDistortion() << " Avg.: " << getAverageIsiDistortion() << endl;
    out << "% Intra-segment hops: Max: " << getMaxIntraHop() << " Min: " << getMinIntraHop() << " Avg.: " << getAverageIntraHop() << endl;
    out << "% Fan-out ratio: " << getFanOutRate() << endl;
}
