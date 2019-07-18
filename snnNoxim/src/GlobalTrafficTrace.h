/*
 * GlobalTrafficTrace.h
 *
 *  Created on: Mar 22, 2016
 *      Author: khanh
 */

#ifndef __NOXIMGLOBALTRAFFICTRACE_H_
#define __NOXIMGLOBALTRAFFICTRACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "DataStructs.h"
#include "NeuralNetworkParser.h"

using namespace std;

#define UNUSED  0
#define IN_USE  1
#define USED    2

// Structure used to store information into the trace
struct TraceCommunication {
  int time;
  int group_src;
  int group_dst;
  int src;          // ID of the source node (PE)
  int no_dst;
//  int no_flit;
  int trace_usage;
  int dest_sent;
  vector <int> dst_set;          // ID of the destination nodes (PE)
};

typedef vector < TraceCommunication > TraceTable;

class GlobalTrafficTrace {

  public:

    GlobalTrafficTrace();

    /**
     * Load traffic trace from a file.
     * @param fname The traffic trace file name
     * @return True if the traffic file is successfully loaded, False otherwise
     */
    bool load(const char *fname);

    /**
     *
     * @param node_id
     * @param now
     * @param[out] dest_id
     * @return
     */
    bool canShot(const int node_id, const int now, int * dest_id);

    // Returns the number of occurrences of source src_id in the traffic
    // trace
    int occurrencesAsSource(const int src_id);
    TraceCommunication * arbitrate(vector<TraceCommunication*> traces);
    bool isValid();

    NeuralNetworkParser * ptr_nn_parser;

  private:
    bool valid;
    // a map from a node_id to its traffic trace
    map <int, TraceTable> traffic_trace;
};


#endif /* __NOXIMGLOBALTRAFFICTRACE_H_ */
