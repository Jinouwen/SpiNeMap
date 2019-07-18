/*
 * GlobalSelectionTable.cpp
 *
 *  Created on: May 26, 2016
 *      Author: khanh
 */
#include "GlobalSelectionTable.h"
#include "GlobalRoutingTable.h"
#include <systemc.h>

GlobalSelectionTable::GlobalSelectionTable()
{
}

bool GlobalSelectionTable::load(const char * fname)
{
    // Open file
    ifstream fin(fname, ios::in);
    if (!fin)
        return false;

    // Initialize variables
    sl_table.clear();

    while (!fin.eof())
    {
        char line[512];
        fin.getline(line, sizeof(line) - 1);
        if ((line[0] != '\0') && (line[0] != '%')) {
            int time_on, time_off, node_id, in_src, dst_id, out_dst;

            int params = sscanf(line, "%d %d %d %d %d %d", &node_id, &in_src, &dst_id, &out_dst, &time_on, &time_off);
            if (params >= 4)
            {
                // Create a sl_port from the parameters read on the line
                PortSelection sl_port;

                // Mandatory fields
                sl_port.node_id = node_id;
                sl_port.in_src = in_src;
                sl_port.dst_id = dst_id;
                sl_port.out_dst = out_dst;

                if (params >= 5)
                    sl_port.time_on = GlobalParams::reset_time + GlobalParams::stats_warm_up_time + time_on;
                else
                    sl_port.time_on = GlobalParams::reset_time + GlobalParams::stats_warm_up_time;
                if (params == 6)
                    sl_port.time_off = GlobalParams::reset_time + GlobalParams::stats_warm_up_time + time_off;
                else
                    sl_port.time_off = GlobalParams::reset_time + GlobalParams::simulation_time;

                // Add this sl_port to the vector of sl_ports
                sl_table.push_back(sl_port);
            }
//            printf("Sel: %d %d %d %d %d %d \n", node_id, in_src, dst_id, out_dst, time_on, time_off);
        }
    }

    return true;
}

int GlobalSelectionTable::getSelectionPort(const RouteData & route_data)
{
    int output = NOT_VALID;
    LinkId link_in = direction2ILinkId(route_data.current_id, route_data.dir_in);
    double now = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
    for (unsigned int i = 0; i < sl_table.size(); i++)
    {
        if ((sl_table[i].dst_id == route_data.dst_id)
                && (sl_table[i].node_id == route_data.current_id)
                && (sl_table[i].in_src == link_in.first))
        {
            if ((now >= sl_table[i].time_on) && (now <= sl_table[i].time_off))
            {
                LinkId link_out(route_data.current_id, sl_table[i].out_dst);
                output = oLinkId2Direction(link_out);
                break;
            }
            else if (now > sl_table[i].time_off)
                sl_table.erase(sl_table.begin()+i);
        }
    }

    return output;
}


