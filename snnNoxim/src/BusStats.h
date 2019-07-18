#ifndef __BUS_STATS_H__
#define __BUS_STATS_H__

#include <cmath>
#include <iostream>
#include <vector>

#include "DataStructs.h"
#include "GlobalParams.h"

using namespace std;

class BusStats {
  public:
    // Logging
    vector<MasterCommItem> spiking_history;
    vector<SlaveCommItem> receiving_history;

    BusStats() {}
    void configure(int _node_id);
    void generateSpike(Spike spike);
    void receiveSpike(Spike spike);
    int getMaxDelay();
    int getMinDelay();
    double getAverageDelay();
    int getMaxIsiDistortion();
    int getMinIsiDistortion();
    double getAverageIsiDistortion();
    int getSpikingCount();
    int getReceivingCount();
    int getDisorderCount();
    int getMaxIntraHop();
    int getMinIntraHop();
    double getAverageIntraHop();
    // int getMaxInterHop();
    // int getMinInterHop();
    // double getAverageInterHop();

  private:
    int node_id;

    double last_received_spike_time;
    double last_generated_spike_time;
    int disorder_spike_count;

    // int max_delay;
    // int min_delay;
    // int sum_delay;
    //
    // int max_isi_distortion;
    // int min_isi_distortion;
    // int sum_isi_distortion;
};

#endif
