/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2015 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementaton of the global statistics
 */

 #include "GlobalStats.h"
 using namespace std;

 GlobalStats::GlobalStats(const NoC * _noc)
 {
     noc = _noc;

     #ifdef TESTING
     drained_total = 0;
     #endif
 }

 double GlobalStats::getAverageDelay()
 {
     unsigned int total_packets = 0;
     double avg_delay = 0.0;

     for (int y = 0; y < GlobalParams::dim_y; y++)
     for (int x = 0; x < GlobalParams::dim_x; x++) {
         unsigned int received_packets =
         noc->t[x][y]->r->stats.getReceivedPackets();

         if (received_packets) {
             avg_delay +=
             received_packets *
             noc->t[x][y]->r->stats.getAverageDelay();
             total_packets += received_packets;
         }
     }

     avg_delay /= (double) total_packets;

     return avg_delay;
 }



 double GlobalStats::getAverageDelay(const int src_id,
     const int dst_id)
     {
         Tile *tile = noc->searchNode(dst_id);

         assert(tile != NULL);

         return tile->r->stats.getAverageDelay(src_id);
     }



     double GlobalStats::getMaxDelay()
     {
         double maxd = -1.0;

         for (int y = 0; y < GlobalParams::dim_y; y++)
         for (int x = 0; x < GlobalParams::dim_x; x++) {
             Coord coord;
             coord.x = x;
             coord.y = y;
             int node_id = coord2Id(coord);
             double d = getMaxDelay(node_id);
             if (d > maxd)
             maxd = d;
         }

         return maxd;
     }



     double GlobalStats::getMaxDelay(const int node_id)
     {
         Coord coord = id2Coord(node_id);

         unsigned int received_packets =
         noc->t[coord.x][coord.y]->r->stats.getReceivedPackets();

         if (received_packets)
         return noc->t[coord.x][coord.y]->r->stats.getMaxDelay();
         else
         return -1.0;
     }



     double GlobalStats::getMaxDelay(const int src_id, const int dst_id)
     {
         Tile *tile = noc->searchNode(dst_id);

         assert(tile != NULL);

         return tile->r->stats.getMaxDelay(src_id);
     }



     int GlobalStats::getDisorderFlitCount(){

         int disorder_count = 0;

         for (int y = 0; y < GlobalParams::dim_y; y++){
             for (int x = 0; x < GlobalParams::dim_x; x++) {
                 Coord coord;
                 coord.x = x;
                 coord.y = y;
                 int node_id = coord2Id(coord);
                 disorder_count = disorder_count + getDisorderFlitCount(node_id);
             }
         }

         return disorder_count;
     }



     int GlobalStats::getDisorderFlitCount(const int node_id){

         Coord coord = id2Coord(node_id);

         unsigned int received_packets =
         noc->t[coord.x][coord.y]->r->stats.getReceivedPackets();

         if (received_packets)
         return noc->t[coord.x][coord.y]->r->stats.getDisorderFlitCount();
         else
         return 0;
     }

     vector < vector < double > > GlobalStats::getMaxDelayMtx()
     {
         vector < vector < double > > mtx;

         mtx.resize(GlobalParams::dim_y);
         for (int y = 0; y < GlobalParams::dim_y; y++)
         mtx[y].resize(GlobalParams::dim_x);

         for (int y = 0; y < GlobalParams::dim_y; y++)
         for (int x = 0; x < GlobalParams::dim_x; x++) {
             Coord coord;
             coord.x = x;
             coord.y = y;
             int id = coord2Id(coord);
             mtx[y][x] = getMaxDelay(id);
         }

         return mtx;
     }

     double GlobalStats::getAverageNetworkDelay()
     {
         unsigned int total_packets = 0;
         double avg_delay = 0.0;

         for (int y = 0; y < GlobalParams::dim_y; y++)
         for (int x = 0; x < GlobalParams::dim_x; x++) {
             unsigned int received_packets =
             noc->t[x][y]->r->stats.getReceivedPackets();

             if (received_packets) {
                 avg_delay +=
                 received_packets *
                 noc->t[x][y]->r->stats.getAverageNetworkDelay();
                 total_packets += received_packets;
             }
         }

         avg_delay /= (double) total_packets;

         return avg_delay;
     }

     double GlobalStats::getAverageNetworkDelay(const int src_id,
         const int dst_id)
         {
             Tile *tile = noc->searchNode(dst_id);

             assert(tile != NULL);

             return tile->r->stats.getAverageNetworkDelay(src_id);
         }

         double GlobalStats::getMaxNetworkDelay()
         {
             double maxd = -1.0;

             for (int y = 0; y < GlobalParams::dim_y; y++)
             for (int x = 0; x < GlobalParams::dim_x; x++) {
                 Coord coord;
                 coord.x = x;
                 coord.y = y;
                 int node_id = coord2Id(coord);
                 double d = getMaxNetworkDelay(node_id);
                 if (d > maxd)
                 maxd = d;
             }

             return maxd;
         }

         double GlobalStats::getMaxNetworkDelay(const int node_id)
         {
             Coord coord = id2Coord(node_id);

             unsigned int received_packets =
             noc->t[coord.x][coord.y]->r->stats.getReceivedPackets();

             if (received_packets)
             return noc->t[coord.x][coord.y]->r->stats.getMaxNetworkDelay();
             else
             return -1.0;
         }

         double GlobalStats::getMaxNetworkDelay(const int src_id, const int dst_id)
         {
             Tile *tile = noc->searchNode(dst_id);

             assert(tile != NULL);

             return tile->r->stats.getMaxNetworkDelay(src_id);
         }

         vector < vector < double > > GlobalStats::getMaxNetworkDelayMtx()
         {
             vector < vector < double > > mtx;

             mtx.resize(GlobalParams::dim_y);
             for (int y = 0; y < GlobalParams::dim_y; y++)
             mtx[y].resize(GlobalParams::dim_x);

             for (int y = 0; y < GlobalParams::dim_y; y++)
             for (int x = 0; x < GlobalParams::dim_x; x++) {
                 Coord coord;
                 coord.x = x;
                 coord.y = y;
                 int id = coord2Id(coord);
                 mtx[y][x] = getMaxNetworkDelay(id);
             }

             return mtx;
         }

         double GlobalStats::getAverageHopCount()
         {
             unsigned int total_packets = 0;
             double avg_delay = 0.0;

             for (int y = 0; y < GlobalParams::dim_y; y++)
             for (int x = 0; x < GlobalParams::dim_x; x++) {
                 unsigned int received_packets =
                 noc->t[x][y]->r->stats.getReceivedPackets();

                 if (received_packets) {
                     avg_delay +=
                     received_packets *
                     noc->t[x][y]->r->stats.getAverageHopCount();
                     total_packets += received_packets;
                 }
             }

             avg_delay /= (double) total_packets;

             return avg_delay;
         }

         double GlobalStats::getAverageHopCount(const int src_id,
             const int dst_id)
             {
                 Tile *tile = noc->searchNode(dst_id);

                 assert(tile != NULL);

                 return tile->r->stats.getAverageHopCount(src_id);
             }

             double GlobalStats::getMaxHopCount()
             {
                 double maxd = -1.0;

                 for (int y = 0; y < GlobalParams::dim_y; y++)
                 for (int x = 0; x < GlobalParams::dim_x; x++) {
                     Coord coord;
                     coord.x = x;
                     coord.y = y;
                     int node_id = coord2Id(coord);
                     double d = getMaxHopCount(node_id);
                     if (d > maxd)
                     maxd = d;
                 }

                 return maxd;
             }

             double GlobalStats::getMaxHopCount(const int node_id)
             {
                 Coord coord = id2Coord(node_id);

                 unsigned int received_packets =
                 noc->t[coord.x][coord.y]->r->stats.getReceivedPackets();

                 if (received_packets)
                 return noc->t[coord.x][coord.y]->r->stats.getMaxHopCount();
                 else
                 return -1.0;
             }

             double GlobalStats::getMaxHopCount(const int src_id, const int dst_id)
             {
                 Tile *tile = noc->searchNode(dst_id);

                 assert(tile != NULL);

                 return tile->r->stats.getMaxHopCount(src_id);
             }

             double GlobalStats::getAverageThroughput(const int src_id, const int dst_id)
             {
                 Tile *tile = noc->searchNode(dst_id);

                 assert(tile != NULL);

                 return tile->r->stats.getAverageThroughput(src_id);
             }

             /*
             double GlobalStats::getAverageThroughput()
             {
             unsigned int total_comms = 0;
             double avg_throughput = 0.0;

             for (int y = 0; y < GlobalParams::dim_y; y++)
             for (int x = 0; x < GlobalParams::dim_x; x++) {
             unsigned int ncomms =
             noc->t[x][y]->r->stats.getTotalCommunications();

             if (ncomms) {
             avg_throughput +=
             ncomms * noc->t[x][y]->r->stats.getAverageThroughput();
             total_comms += ncomms;
         }
     }

     avg_throughput /= (double) total_comms;

     return avg_throughput;
 }
 */

 double GlobalStats::getAggregatedThroughput()
 {
     int total_cycles = GlobalParams::simulation_time - GlobalParams::stats_warm_up_time;

     return (double)getReceivedFlits()/(double)(total_cycles);
 }

 unsigned int GlobalStats::getReceivedPackets()
 {
     unsigned int n = 0;

     for (int y = 0; y < GlobalParams::dim_y; y++)
     for (int x = 0; x < GlobalParams::dim_x; x++)
     n += noc->t[x][y]->r->stats.getReceivedPackets();

     return n;
 }

 unsigned int GlobalStats::getReceivedFlits()
 {
     unsigned int n = 0;

     for (int y = 0; y < GlobalParams::dim_y; y++)
     for (int x = 0; x < GlobalParams::dim_x; x++) {
         n += noc->t[x][y]->r->stats.getReceivedFlits();
         #ifdef TESTING
         drained_total += noc->t[x][y]->r->local_drained;
         #endif
     }

     return n;
 }

 double GlobalStats::getThroughput()
 {

     int number_of_ip = GlobalParams::dim_x * GlobalParams::dim_y;

     return (double)getAggregatedThroughput()/(double)(number_of_ip);
 }

 // Only accounting IP that received at least one flit
 double GlobalStats::getActiveThroughput()
 {
     int total_cycles =
     GlobalParams::simulation_time -
     GlobalParams::stats_warm_up_time;


     unsigned int n = 0;
     unsigned int trf = 0;
     for (int y = 0; y < GlobalParams::dim_y; y++)
     for (int x = 0; x < GlobalParams::dim_x; x++) {
         unsigned int rf = noc->t[x][y]->r->stats.getReceivedFlits();

         if (rf != 0)
         n++;

         trf += rf;
     }

     return (double) trf / (double) (total_cycles * n);

 }

 vector < vector < unsigned long > > GlobalStats::getRoutedFlitsMtx()
 {

     vector < vector < unsigned long > > mtx;

     mtx.resize(GlobalParams::dim_y);
     for (int y = 0; y < GlobalParams::dim_y; y++)
     mtx[y].resize(GlobalParams::dim_x);

     for (int y = 0; y < GlobalParams::dim_y; y++)
     for (int x = 0; x < GlobalParams::dim_x; x++)
     mtx[y][x] = noc->t[x][y]->r->getRoutedFlits();

     return mtx;
 }

 vector < vector < unsigned long > > GlobalStats::getReceivedFlitsMtx()
 {

     vector < vector < unsigned long > > mtx;

     mtx.resize(GlobalParams::dim_y);
     for (int y = 0; y < GlobalParams::dim_y; y++)
     mtx[y].resize(GlobalParams::dim_x);

     for (int y = 0; y < GlobalParams::dim_y; y++)
     for (int x = 0; x < GlobalParams::dim_x; x++)
     mtx[y][x] = noc->t[x][y]->r->stats.getReceivedFlits();

     return mtx;
 }


 unsigned int GlobalStats::getWirelessPackets()
 {
     unsigned int packets = 0;

     // Wireless noc
     for (map<int, HubConfig>::iterator it = GlobalParams::hub_configuration.begin();
     it != GlobalParams::hub_configuration.end();
     ++it)
     {
         int hub_id = it->first;

         map<int,Hub*>::const_iterator i = noc->hub.find(hub_id);
         Hub * h = i->second;

         packets+= h->wireless_communications_counter;
     }
     return packets;
 }

 double GlobalStats::getDynamicPower()
 {
     double power = 0.0;

     // Electric noc
     for (int y = 0; y < GlobalParams::dim_y; y++){

        for (int x = 0; x < GlobalParams::dim_x; x++){
            power += noc->t[x][y]->r->power.getDynamicPower();
            //ab3586
            cout << "NoC:["<<x<<","<<y<< "]-> Power consumption = " << noc->t[x][y]->r->power.getDynamicPower() << endl; 
        }
     }

     // Wireless noc
     for (map<int, HubConfig>::iterator it = GlobalParams::hub_configuration.begin();
     it != GlobalParams::hub_configuration.end();
     ++it)
     {
         int hub_id = it->first;

         map<int,Hub*>::const_iterator i = noc->hub.find(hub_id);
         Hub * h = i->second;

         power+= h->power.getDynamicPower();
     }
     return power;
 }

 double GlobalStats::getStaticPower()
 {
     double power = 0.0;

     for (int y = 0; y < GlobalParams::dim_y; y++)
     for (int x = 0; x < GlobalParams::dim_x; x++)
     power += noc->t[x][y]->r->power.getStaticPower();

     // Wireless noc
     for (map<int, HubConfig>::iterator it = GlobalParams::hub_configuration.begin();
     it != GlobalParams::hub_configuration.end();
     ++it)
     {
         int hub_id = it->first;

         map<int,Hub*>::const_iterator i = noc->hub.find(hub_id);
         Hub * h = i->second;

         power+= h->power.getStaticPower();
     }
     return power;
 }

 void GlobalStats::showStats(std::ostream & out, bool detailed)
 {
     if (detailed) {
         out << endl << "detailed = [" << endl;
         for (int y = 0; y < GlobalParams::dim_y; y++)
         for (int x = 0; x < GlobalParams::dim_x; x++)
         noc->t[x][y]->r->stats.showStats(y * GlobalParams:: dim_x + x, out, true);
         out << "];" << endl;

         // show MaxDelay matrix
         vector < vector < double > > md_mtx = getMaxDelayMtx();

         out << endl << "max_delay = [" << endl;
         for (unsigned int y = 0; y < md_mtx.size(); y++) {
             out << "   ";
             for (unsigned int x = 0; x < md_mtx[y].size(); x++)
             out << setw(6) << md_mtx[y][x];
             out << endl;
         }
         out << "];" << endl;

         vector < vector < double > > mnd_mtx = getMaxNetworkDelayMtx();

         out << endl << "max__network_delay = [" << endl;
         for (unsigned int y = 0; y < mnd_mtx.size(); y++) {
             out << "   ";
             for (unsigned int x = 0; x < mnd_mtx[y].size(); x++)
             out << setw(6) << mnd_mtx[y][x];
             out << endl;
         }
         out << "];" << endl;

         // show RoutedFlits matrix
         vector < vector < unsigned long > > rf_mtx = getRoutedFlitsMtx();

         out << endl << "routed_flits = [" << endl;
         for (unsigned int y = 0; y < rf_mtx.size(); y++) {
             out << "   ";
             for (unsigned int x = 0; x < rf_mtx[y].size(); x++)
             out << setw(10) << rf_mtx[y][x];
             out << endl;
         }
         out << "];" << endl;

         // show ReceivedFlits matrix
         vector < vector < unsigned long > > ref_mtx = getReceivedFlitsMtx();

         out << endl << "received_flits = [" << endl;
         for (unsigned int y = 0; y < ref_mtx.size(); y++) {
             out << "   ";
             for (unsigned int x = 0; x < ref_mtx[y].size(); x++)
             out << setw(10) << ref_mtx[y][x];
             out << endl;
         }
         out << "];" << endl;

         showPowerBreakDown(out);
         showPowerManagerStats(out);
     }

     int total_cycles = GlobalParams::simulation_time - GlobalParams::stats_warm_up_time;
     out << "% Total received packets: " << getReceivedPackets() << endl;
     out << "% Total received flits: " << getReceivedFlits() << endl;
     out << "% Received/Ideal flits Ratio: " << getReceivedFlits()
     / (GlobalParams::packet_injection_rate * (GlobalParams::min_packet_size +
         GlobalParams::max_packet_size)/2 * total_cycles * GlobalParams::dim_y * GlobalParams::dim_x) << endl;
         out << "% Average wireless utilization: " << getWirelessPackets()/(double)getReceivedPackets() << endl;
         out << "% Global average delay (cycles): " << getAverageDelay() << endl;
         out << "% Max delay (cycles): " << getMaxDelay() << endl;
         out << "% Global average network delay (cycles): " << getAverageNetworkDelay() << endl;
         out << "% Max network delay (cycles): " << getMaxNetworkDelay() << endl;
         out << "% Global average hop count: " << getAverageHopCount() << endl;
         out << "% Max network hop count: " << getMaxHopCount() << endl;
         out << "% Network throughput (flits/cycle): " << getAggregatedThroughput() << endl;
         out << "% Average IP throughput (flits/cycle/IP): " << getThroughput() << endl;
         out << "% Total energy (J): " << getTotalPower() << endl;
         out << "% \tDynamic energy (J): " << getDynamicPower() << endl;
         out << "% \tStatic energy (J): " << getStaticPower() << endl;
         out << "% Max delay flit: " << Stats::getMaxDelayFlit() << endl;
         out << "% Disorder flit count: " << getDisorderFlitCount() << endl;

         
         cout << "% \tDynamic energy (J): " << getDynamicPower() << endl;
         
         if (GlobalParams::show_buffer_stats)
         showBufferStats(out);

     }

     void GlobalStats::updatePowerBreakDown(map<string,double> &dst,PowerBreakdown* src)
     {
         for (int i=0;i!=src->size;i++)
         {
             dst[src->breakdown[i].label]+=src->breakdown[i].value;
         }
     }

     void GlobalStats::showPowerManagerStats(std::ostream & out)
     {
         std::streamsize p = out.precision();
         int total_cycles = sc_time_stamp().to_double() / GlobalParams::clock_period_ps - GlobalParams::reset_time;

         out.precision(4);

         out << "powermanager_stats_tx = [" << endl;
         out << "%\tFraction of: TX Transceiver off (TTXoff), AntennaBufferTX off (ABTXoff) " << endl;
         out << "%\tHUB\tTTXoff\tABTXoff\t" << endl;

         for (map<int, HubConfig>::iterator it = GlobalParams::hub_configuration.begin();
         it != GlobalParams::hub_configuration.end();
         ++it)
         {
             int hub_id = it->first;

             map<int,Hub*>::const_iterator i = noc->hub.find(hub_id);
             Hub * h = i->second;

             out << "\t" << hub_id << "\t" << std::fixed << (double)h->total_ttxoff_cycles/total_cycles << "\t";

             int s = 0;
             for (map<int,int>::iterator i = h->abtxoff_cycles.begin(); i!=h->abtxoff_cycles.end();i++) s+=i->second;

             out << (double)s/h->abtxoff_cycles.size()/total_cycles << endl;
         }

         out << "];" << endl;


         out << "powermanager_stats_rx = [" << endl;
         out << "%\tFraction of: RX Transceiver off (TRXoff), AntennaBufferRX off (ABRXoff), BufferToTile off (BTToff) " << endl;
         out << "%\tHUB\tTRXoff\tABRXoff\tBTToff\t" << endl;



         for (map<int, HubConfig>::iterator it = GlobalParams::hub_configuration.begin();
         it != GlobalParams::hub_configuration.end();
         ++it)
         {
             string bttoff_str;

             out.precision(4);

             int hub_id = it->first;

             map<int,Hub*>::const_iterator i = noc->hub.find(hub_id);
             Hub * h = i->second;

             out << "\t" << hub_id << "\t" << std::fixed << (double)h->total_sleep_cycles/total_cycles << "\t";

             int s = 0;
             for (map<int,int>::iterator i = h->buffer_rx_sleep_cycles.begin();
             i!=h->buffer_rx_sleep_cycles.end();i++)
             s+=i->second;

             out << (double)s/h->buffer_rx_sleep_cycles.size()/total_cycles << "\t";

             s = 0;
             for (map<int,int>::iterator i = h->buffer_to_tile_poweroff_cycles.begin();
             i!=h->buffer_to_tile_poweroff_cycles.end();i++)
             {
                 double bttoff_fraction = i->second/(double)total_cycles;
                 s+=i->second;
                 if (bttoff_fraction<0.25)
                 bttoff_str+=" ";
                 else if (bttoff_fraction<0.5)
                 bttoff_str+=".";
                 else if (bttoff_fraction<0.75)
                 bttoff_str+="o";
                 else if (bttoff_fraction<0.90)
                 bttoff_str+="O";
                 else
                 bttoff_str+="0";


             }
             out << (double)s/h->buffer_to_tile_poweroff_cycles.size()/total_cycles << "\t" << bttoff_str << endl;
         }

         out << "];" << endl;

         out.unsetf(std::ios::fixed);

         out.precision(p);

     }

     void GlobalStats::showPowerBreakDown(std::ostream & out)
     {
         map<string,double> power_dynamic;
         map<string,double> power_static;
         
         cout << " Power stats have been output from here. 12345666 \n" << endl;

         for (int y = 0; y < GlobalParams::dim_y; y++)
         for (int x = 0; x < GlobalParams::dim_x; x++)
         {
             updatePowerBreakDown(power_dynamic, noc->t[x][y]->r->power.getDynamicPowerBreakDown());

             updatePowerBreakDown(power_static, noc->t[x][y]->r->power.getStaticPowerBreakDown());
         }

         for (map<int, HubConfig>::iterator it = GlobalParams::hub_configuration.begin();
         it != GlobalParams::hub_configuration.end();
         ++it)
         {
             int hub_id = it->first;

             map<int,Hub*>::const_iterator i = noc->hub.find(hub_id);
             Hub * h = i->second;

             updatePowerBreakDown(power_dynamic,
                 h->power.getDynamicPowerBreakDown());

                 updatePowerBreakDown(power_static,
                     h->power.getStaticPowerBreakDown());
                 }

                 printMap("power_dynamic",power_dynamic,out);
                 printMap("power_static",power_static,out);

             }



             void GlobalStats::showBufferStats(std::ostream & out)
             {
                 out << "Router id\tBuffer N\t\tBuffer E\t\tBuffer S\t\tBuffer W\t\tBuffer L" << endl;
                 out << "         \tMean\tMax\tMean\tMax\tMean\tMax\tMean\tMax\tMean\tMax" << endl;
                 for (int y = 0; y < GlobalParams::dim_y; y++)
                 for (int x = 0; x < GlobalParams::dim_x; x++)
                 {
                     out << noc->t[x][y]->r->local_id;
                     noc->t[x][y]->r->ShowBuffersStats(out);
                     out << endl;
                 }
             }
