/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2015 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the local routing table
 */

#include "LocalRoutingFile.h"

LocalRoutingFile::LocalRoutingFile()
{
}

void LocalRoutingFile::configure(GlobalRoutingFile & rfile,
				       const int _node_id)
{
    rt_node = rfile.getNodeRoutingFile(_node_id);
    node_id = _node_id;
}

AdmissibleOutputs LocalRoutingFile::
getAdmissibleOutputs(const LinkId & in_link, const int destination_id)
{
    RouteTime rt = rt_node[in_link][destination_id];
    AdmissibleOutputs ao;
    double now = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
    for (RouteTime::iterator i = rt.begin(); i < rt.end(); i++)
    {
        AdOutTime aot = *i;
        if ((now >= aot.time_on) && (now <= aot.time_off))
            ao = aot.ao;
        else if (now > aot.time_off)
        {
//            std::cout << "erase:" << std::distance(rt.begin(), i) << endl;
            rt.erase(i);
            i--;
        }
    }
    return ao;
}

AdmissibleOutputs LocalRoutingFile::
getAdmissibleOutputs(const int in_direction, const int destination_id)
{
    LinkId lid = direction2ILinkId(node_id, in_direction);

    return getAdmissibleOutputs(lid, destination_id);
}
