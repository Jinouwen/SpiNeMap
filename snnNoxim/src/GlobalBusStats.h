#ifndef __GLOBAL_BUS_STATS_H__
#define __GLOBAL_BUS_STATS_H__

#include <iostream>
#include <fstream>
#include <cmath>

#include "NoS.h"

using namespace std;

class GlobalBusStats{
public:
    GlobalBusStats(const NoS *_nos);

    int getMaxDelay(int _node_id);
    int getMinDelay(int _node_id);
    double getAverageDelay(int _node_id);
    int getMaxIsiDistortion(int _node_id);
    int getMinIsiDistortion(int _node_id);
    double getAverageIsiDistortion(int _node_id);
    int getSpikingCount(int _node_id);
    int getReceivingCount(int _node_id);
    int getDisorderCount(int _node_id);
    int getMaxIntraHop(int _node_id);
    int getMinIntraHop(int _node_id);
    double getAverageIntraHop(int _node_id);

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

    double getFanOutRate();

    void dumpSpikingLogOut(int _node_id, ostream & out);
    void dumpSpikingLogOut(ostream & out);

    void dumpReceivingLogOut(int _node_id, ostream & out);
    void dumpReceivingLogOut(ostream & out);

    void showStats(ostream & out);

private:
    const NoS *nos;
    int node_number;
};

#endif
