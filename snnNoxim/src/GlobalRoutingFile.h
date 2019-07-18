/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 */

#ifndef __NOXIMGLOBALROUTINGFILE_H__
#define __NOXIMGLOBALROUTINGFILE_H__

#define COLUMN_AOC 22

#include <vector>
#include <map>
#include <set>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <fstream>
#include "DataStructs.h"
#include "GlobalRoutingTable.h"
using namespace std;

struct AdOutTime{
    AdmissibleOutputs ao;
    int time_on;
    int time_off;
};

typedef vector <AdOutTime> RouteTime;

// Map a destination to a set of admissible outputs
typedef map < int, RouteTime > RoutingFileLink;

// Map an input link to its routing table
typedef map < LinkId, RoutingFileLink > RoutingFileNode;

// Map a node of the network to its routing table
typedef map < int, RoutingFileNode > RoutingFileNoC;

class GlobalRoutingFile {

  public:

    GlobalRoutingFile();

    // Load routing table from file. Returns true if ok, false otherwise
    bool load(const char *fname);

    RoutingFileNode getNodeRoutingFile(const int node_id);

    bool isValid() {
	return valid;
  }
    private:

    RoutingFileNoC rt_noc;
    bool valid;

};

#endif
