/*
 * GlobalSelectionTable.h
 *
 *  Created on: May 26, 2016
 *      Author: khanh
 */

#ifndef NOXIMGLOBALSELECTIONTABLE_H_
#define NOXIMGLOBALSELECTIONTABLE_H_

#include "DataStructs.h"


using namespace std;


// Structure used to store selection information
struct PortSelection {
  int time_on;
  int time_off;
  int node_id;          // ID of the node
  int in_src;
  int dst_id;
  int out_dst;
};

class GlobalSelectionTable {
    public:
        GlobalSelectionTable();
        bool load(const char * fname);
        int getSelectionPort(const RouteData & route_data);

    private:

        vector <PortSelection> sl_table;
};


#endif /* NOXIMGLOBALSELECTIONTABLE_H_ */
