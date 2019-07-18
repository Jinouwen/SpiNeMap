/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2015 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the global traffic trace
 */

#include "GlobalTrafficTrace.h"
#include "GlobalParams.h"
#include <math.h>

GlobalTrafficTrace::GlobalTrafficTrace()
{
    valid = false;
}

/*bool GlobalTrafficTrace::load(const char *fname)
{
  // Open file
  ifstream fin(fname, ios::in);
  if (!fin)
    return false;

  // Initialize variables
  traffic_trace.clear();

  // Cycle reading file
  while (!fin.eof()) {
      char line[512];
      fin.getline(line, sizeof(line) - 1);

      if ((line[0] != '\0') && (line[0] != '%')) {
          int time, group_src, group_dst, src, no_dst;	// Mandatory

          int params = sscanf(line, "%d %d %d %d %d", &time, &group_src, &group_dst, &src, &no_dst);
          if (params == 5)
          {
              // Create a communication from the parameters read on the line
              TraceCommunication communication;

//              if (params == 5)
//                  no_flit = 1;
              // Mandatory fields
              communication.time = GlobalParams::reset_time + GlobalParams::stats_warm_up_time + time;
//              communication.no_flit = no_flit;
              communication.group_src = group_src;
              communication.group_dst = group_dst;
              communication.src = src;
              communication.no_dst = no_dst;
              communication.trace_usage = UNUSED;
              communication.dest_sent = 0;

              fin.getline(line, sizeof(line) - 1);
              while ((line[0] == '\0') || (line[0] == '%'))
                  fin.getline(line, sizeof(line) - 1);
              stringstream ss(line);
              for (int i = 0; i < no_dst; i++)
              {
                  int dst;
                  ss >> dst;
//                      printf("%d ", dst);
                  communication.dst_set.push_back(dst);
              }

              // Add this communication to the vector of communications
              traffic_trace[src].push_back(communication);
//              printf("\n");
              }
      }
  }

  valid = true;
  return true;
}*/

bool GlobalTrafficTrace::load(const char *fname)
{
    // Open file
    ifstream fin(fname, ios::in);
    if (!fin)
        return false;

    // Initialize variables
    traffic_trace.clear();

    // Cycle reading file
    while (!fin.eof()) {
        char line[512];
        fin.getline(line, sizeof(line) - 1);

        // Check whether the line is empty or is a comment line
        if ((line[0] != '\0') && (line[0] != '%')) {
            //Load cluster traffic
            if (GlobalParams::cluster_traffic == true)
            {
                int time, group_src, no_dst;  // Mandatory
                int src_id, dst_id;
                int params = sscanf(line, "%d %d %d", &time, &group_src, &no_dst);
//                printf("%d %d %d \n", time, group_src, no_dst);
                if (params == 3)
                {
                    //ab3586: find the group that the neuron has been assigned to. 
                    //NeuronPair np(layer_src, 0);
                    //group_src = ptr_nn_parser->getGroupId(layer_src, 0);
                    
                    // Create a communication from the parameters read on the line
                    TraceCommunication communication;
                    // Mandatory fields
                    communication.time = GlobalParams::reset_time + GlobalParams::stats_warm_up_time + time * GlobalParams::spike_step;
                     
                    //cout << "Spike Time: " << communication.time << endl;
                    communication.src = group_src;
                    
                    // Convert to physical node on the network
                    NeuronGroup ng_src(group_src);
                    src_id = ptr_nn_parser->getNodeId(ng_src);

                    communication.no_dst = no_dst;
                    communication.trace_usage = UNUSED;
                    communication.dest_sent = 0;
                    
                    //ab3586
                    //cout << "Destination : " << no_dst << endl;
                    fin.getline(line, sizeof(line) - 1);
                    while ((line[0] == '\0') || (line[0] == '%'))
                        fin.getline(line, sizeof(line) - 1);
                    stringstream ss(line);
                    for (int i = 0; i < no_dst; i++)
                    {
                        int group_dst;
                        ss >> group_dst;
                        
                        //ab3586
                        //group_dst = ptr_nn_parser->getGroupId(layer_src, 0); 
                        
                        // Convert to physical node on the network
                        NeuronGroup ng_dst(group_dst);
                        dst_id = ptr_nn_parser->getNodeId(ng_dst);
                        if (dst_id != src_id)
                            communication.dst_set.push_back(dst_id);
                        else
                            communication.no_dst--;
                    }

                    // Add this communication to the vector of communications
                    traffic_trace[src_id].push_back(communication);
                }
            }
            // Load neuron traffic
            else {
                int time, group_src, src, dst_id; // Mandatory

                int params = sscanf(line, "%d %d", &time, &group_src);
                //          printf("%d %d %d \n", time, group_src, src);
                if (params == 2)
                {
                    
                    // Create a communication from the parameters read on the line
                    TraceCommunication communication;

                    int node_id = ptr_nn_parser->getNodeId(group_src);
                    
                    communication.no_dst = ptr_nn_parser->getDests(group_src).size();
                    
                    // Mandatory fields
                    communication.time = GlobalParams::reset_time + GlobalParams::stats_warm_up_time + time * 100; //* GlobalParams::spike_step ;//* GlobalParams::clock_period_ps;
                   
                    //cout << "Communication Time" << communication.time << endl;
                    communication.group_src = group_src;
                    communication.src = node_id;


                    //ab3586
                    vector <int> destination_groups; 
                    destination_groups = ptr_nn_parser->getDests(group_src); 
                    std::vector <int>::iterator it = destination_groups.begin();  
                    

                    for (it; it != destination_groups.end(); it++)
                    {
                        // Convert to physical node on the network
                        NeuronGroup ng_dst(*it);
                        dst_id = ptr_nn_parser->getNodeId(ng_dst);
                        if (dst_id != node_id)
                        {
                            communication.dst_set.push_back(dst_id);
                        }
                        else
                        {
                            communication.no_dst--;
                        }
                    }


                    communication.trace_usage = UNUSED;
                    communication.dest_sent = 0;
                    NeuronGroup ng_src(group_src);
                    
                    
                    if (node_id >= 0)
                    {

                        //ab3586
                        //cout << "Source: " << group_src << " Destination : " << communication.no_dst << endl;
                        
                        if (communication.no_dst > 0)
                        {
                            traffic_trace[node_id].push_back(communication);
                        }
                    }
                }
            }
        } // end checking empty or comment lines
    } // end of file

/*
//ab3586 - Debug
    for(int z=0;z<2;z++)
    {
        //TraceTable::iterator it = traffic_trace[z].begin();

        for(int it=0; it<traffic_trace[z].size(); it++)
        {   
            cout <<"Src " << traffic_trace[z][it].src << " Group Source: " << traffic_trace[z][it].group_src << " Destination : " << traffic_trace[z][it].group_dst <<endl;
        }

    }
*/
    valid = true;
    return true;
}

bool GlobalTrafficTrace::canShot(const int node_id, const int now, int * dest_id)
{
    bool shoot = false;
    // Check cluster traffic
    if (GlobalParams::cluster_traffic == true)
    {
        for (unsigned int i = 0; i < traffic_trace[node_id].size(); i++)
        {
            TraceCommunication comm = traffic_trace[node_id][i];
                // can shoot until a spike is transmitted to all destinations
                cout << "Now " << now << " Comm Time " << comm.time << endl;
                if ((now >=  comm.time) && (now < comm.time + comm.no_dst))
                {
                    shoot = true;
                    *dest_id = comm.dst_set[now - comm.time];
                    //ab3586: find the group that the neuron has been assigned to. 
                    if (now == comm.time + comm.no_dst - 1)
                        traffic_trace[node_id].erase(traffic_trace[node_id].begin()+i);
                }
                break;
        }
    }
    // Check neuron traffic
    else
    {   

        vector<TraceCommunication*> traces;
        for (unsigned int i = 0; i < traffic_trace[node_id].size(); i++)
        {
            TraceCommunication comm = traffic_trace[node_id][i];
            // erase used trace
            if (comm.trace_usage == USED)
            {
                traffic_trace[node_id].erase(traffic_trace[node_id].begin()+i);
                i--;
            }
            // choose the trace that is in use first
            else if (comm.trace_usage == IN_USE)
            {
                shoot = true;
                //ab3586
                int group_src;
                group_src = comm.group_src;
                NeuronGroup ng(group_src);

                *dest_id = ptr_nn_parser->getDests((ng))[comm.dest_sent];
                traffic_trace[node_id][i].dest_sent++;
                if (traffic_trace[node_id][i].dest_sent >= traffic_trace[node_id][i].no_dst)
                    traffic_trace[node_id][i].trace_usage = USED;
                break;
            }
            // otherwise push it into a trace vector for arbitration
            else if ((now >=  comm.time) && (comm.trace_usage == UNUSED))
            {
                traces.push_back(&traffic_trace[node_id][i]);
            }
            //        else if (now < comm.time)
            //            break;
        }

        if (traces.size() > 0)
        {
            TraceCommunication * chosen = arbitrate(traces);

            int group_src;
            group_src = chosen->group_src;
            NeuronGroup ng(group_src);

            if (ptr_nn_parser->getDests(ng).size() > 0)
            {
                shoot = true;
                //ab3586
                //cout << "Now " << now << " Comm Time " << comm.time << endl;
                int group_src;
                group_src = chosen->group_src;
                NeuronGroup ng(group_src);
                
                *dest_id = ptr_nn_parser->getDests(ng)[chosen->dest_sent];
                
                chosen->dest_sent++;
                if (chosen->dest_sent >= chosen->no_dst)
                    chosen->trace_usage = USED;
                else
                    chosen->trace_usage = IN_USE;
            }
            else chosen->trace_usage = USED;
        }
    }
    return shoot;
}

TraceCommunication * GlobalTrafficTrace::arbitrate(vector<TraceCommunication*> traces)
{
    TraceCommunication * chosen_trace;

    // set chosen trace to be the first element
    chosen_trace = traces[0];

    for (unsigned int i = 1; i < traces.size(); i++)
    {
        // chose the trace that has the minimum time
        if (traces[i]->time < chosen_trace->time)
            chosen_trace = traces[i];
    }

    return chosen_trace;
}

bool GlobalTrafficTrace::isValid()
{
    return valid;
}

int GlobalTrafficTrace::occurrencesAsSource(const int node_id)
{
  int count = 0;

  for (unsigned int i = 0; i < traffic_trace[node_id].size(); i++)
      count += traffic_trace[node_id][i].dst_set.size();

  return count;
}
