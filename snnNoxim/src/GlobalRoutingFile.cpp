/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2015 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the global routing table
 */

#include "GlobalRoutingFile.h"
using namespace std;

GlobalRoutingFile::GlobalRoutingFile()
{
    valid = false;
}

bool GlobalRoutingFile::load(const char *fname)
{
    ifstream fin(fname, ios::in);

    if (!fin)
    {
        std::cout << !fin << endl;
        return false;
    }

    rt_noc.clear();

    while (!fin.eof()) {
        char line[512];
        fin.getline(line, sizeof(line) - 1);
        if ((line[0] != '\0') && (line[0] != '%')) {
            int time_on, time_off, node_id, in_src, dst_id, out_dst;

            int params = sscanf(line, "%d %d %d %d %d %d", &node_id, &in_src, &dst_id, &out_dst, &time_on, &time_off);
            if (params >= 4)
            {

                // Mandatory fields
                LinkId lin(in_src, node_id);
                LinkId lout(node_id, out_dst);
                AdmissibleOutputs ao;
                ao.insert(lout);


                if (params >= 5)
                    time_on = GlobalParams::reset_time + GlobalParams::stats_warm_up_time
                            + time_on*(PS_PER_MS/GlobalParams::clock_period_ps);
                else
                    time_on = GlobalParams::reset_time + GlobalParams::stats_warm_up_time;
                if (params == 6)
                    time_off = GlobalParams::reset_time + GlobalParams::stats_warm_up_time
                            + time_off*(PS_PER_MS/GlobalParams::clock_period_ps);
                else
                    time_off = GlobalParams::reset_time + GlobalParams::simulation_time;

                AdOutTime aot = {ao, time_on, time_off};
                rt_noc[node_id][lin][dst_id].push_back(aot);
            }
        }

		}


    valid = true;

    return true;
}

RoutingFileNode GlobalRoutingFile::
getNodeRoutingFile(const int node_id)
{
    return rt_noc[node_id];
}
