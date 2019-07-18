/*
* Noxim - the NoC Simulator
*
* (C) 2005-2015 by the University of Catania
* For the complete list of authors refer to file ../doc/AUTHORS.txt
* For the license applied to these sources refer to file ../doc/LICENSE.txt
*
* This file contains the implementation of the statistics
*/

#include "Stats.h"
#include "Utils.h"
#include <iostream>


// TODO: nan in averageDelay
double Stats::max_delay = -1.0;
Flit Stats::max_delay_flit;

Flit Stats::getMaxDelayFlit()
{
    return max_delay_flit;
}

void Stats::configure(const int node_id, const double _warm_up_time)
{
    id = node_id;
    warm_up_time = _warm_up_time;


    //    max_delay = -1.0;
}

void Stats::receivedFlit(const double arrival_time,
    const Flit & flit)
    {
        if (arrival_time - GlobalParams::reset_time < warm_up_time)
        return;

        double receiving_time =  arrival_time - warm_up_time;

        if (GlobalParams::output_mode != EX_STAT_MODE){
            int i = searchCommHistory(flit.src_id);

            if (i == -1) {
                // first flit received from a given source
                // initialize CommHist structure
                CommHistory ch;

                ch.src_id = flit.src_id;
                ch.total_received_flits = 0;
                ch.disorder_flit_count = 0;
                ch.last_received_flit_time = 0 - warm_up_time;
                ch.last_received_flit_generated_time = 0;
                ch.interarrival_distortion = 0;
                chist.push_back(ch);

                i = chist.size() - 1;
            }

            if (flit.flit_type == FLIT_TYPE_HEAD)
            {
                chist[i].delays.push_back(arrival_time - flit.timestamp);
                chist[i].network_delays.push_back(arrival_time - flit.sent_time);
                chist[i].hop_counts.push_back(flit.hop_no);
                if (arrival_time - flit.timestamp > max_delay)
                {
                    max_delay = arrival_time - flit.timestamp;
                    max_delay_flit = flit;
                }
            }



            if (flit.timestamp > chist[i].last_received_flit_generated_time){
                chist[i].interarrival_distortion = (receiving_time - chist[i].last_received_flit_time) - (flit.timestamp - chist[i].last_received_flit_generated_time);
                chist[i].last_received_flit_generated_time = flit.timestamp;
            } else {
                chist[i].disorder_flit_count++;
            }

            chist[i].total_received_flits++;
            chist[i].last_received_flit_time = receiving_time;

            // cout<<"Node: "<<id<<" received a flit at "<<receiving_time<<", generated at "<<flit.timestamp<<" from Node: "<<chist[i].src_id<<endl;
            // cout << "Current disorder count is " <<chist[i].disorder_flit_count<< ". Interarrival Distortion is " << chist[i].interarrival_distortion <<'\n';
            // cout << "The slacking of the flit is " << (receiving_time - flit.deadline) << '\n';
            // cout << endl;
        }

        else{
            /* ab3586 */
            cout<<"R,"
                <<id<<","
                <<receiving_time<<","
                <<flit.src_id<<","
                <<flit.timestamp<<","
                <<flit.hop_no<<","
                <<getDistance(flit.src_id, id)
                <<endl; 

        }

        // csv_stat.open("stat.csv",ios::out|ios::app);
        // csv_stat<<chist[i].src_id<<","
        //                <<id<<","
        //                <<flit.timestamp<<","
        //                <<receiving_time<<","
        //                <<chist[i].interarrival_distortion<<","
        //                <<(receiving_time - flit.deadline)<<","
        //                <<chist[i].disorder_flit_count<<","
        //                <<getDistance(chist[i].src_id, id)<<","
        //                <<endl;
        // csv_stat.close();

    }

    int Stats::getDisorderFlitCount (const int src_id){

        int i = searchCommHistory(src_id);

        return chist[i].disorder_flit_count;
    }

    int Stats::getDisorderFlitCount(){

        int sum = 0;

        for (unsigned int i = 0; i < chist.size(); i++) {

            sum = sum + chist[i].disorder_flit_count;

        }

        return sum;
    }

    double Stats::getAverageDelay(const int src_id)
    {
        double sum = 0.0;

        int i = searchCommHistory(src_id);

        assert(i >= 0);

        for (unsigned int j = 0; j < chist[i].delays.size(); j++)
        sum += chist[i].delays[j];

        return sum / (double) chist[i].delays.size();
    }

    double Stats::getAverageDelay()
    {
        double avg = 0.0;

        for (unsigned int k = 0; k < chist.size(); k++) {
            unsigned int samples = chist[k].delays.size();
            if (samples)
            avg += (double) samples *getAverageDelay(chist[k].src_id);
        }

        return avg / (double) getReceivedPackets();
    }

    double Stats::getMaxDelay(const int src_id)
    {
        double maxd = -1.0;

        int i = searchCommHistory(src_id);

        assert(i >= 0);

        for (unsigned int j = 0; j < chist[i].delays.size(); j++)
        if (chist[i].delays[j] > maxd) {
            maxd = chist[i].delays[j];
        }
        return maxd;
    }

    double Stats::getMaxDelay()
    {

        double maxd = -1.0;

        for (unsigned int k = 0; k < chist.size(); k++) {
            unsigned int samples = chist[k].delays.size();
            if (samples) {
                double m = getMaxDelay(chist[k].src_id);
                if (m > maxd)
                maxd = m;
            }
        }

        return maxd;
    }

    double Stats::getAverageNetworkDelay(const int src_id)
    {
        double sum = 0.0;

        int i = searchCommHistory(src_id);

        assert(i >= 0);

        for (unsigned int j = 0; j < chist[i].network_delays.size(); j++)
        sum += chist[i].network_delays[j];

        return sum / (double) chist[i].network_delays.size();
    }

    double Stats::getAverageNetworkDelay()
    {
        double avg = 0.0;

        for (unsigned int k = 0; k < chist.size(); k++) {
            unsigned int samples = chist[k].network_delays.size();
            if (samples)
            avg += (double) samples *getAverageNetworkDelay(chist[k].src_id);
        }

        return avg / (double) getReceivedPackets();
    }

    double Stats::getMaxNetworkDelay(const int src_id)
    {
        double maxd = -1.0;

        int i = searchCommHistory(src_id);

        assert(i >= 0);

        for (unsigned int j = 0; j < chist[i].network_delays.size(); j++)
        if (chist[i].network_delays[j] > maxd) {
            maxd = chist[i].network_delays[j];
        }
        return maxd;
    }

    double Stats::getMaxNetworkDelay()
    {

        double maxd = -1.0;

        for (unsigned int k = 0; k < chist.size(); k++) {
            unsigned int samples = chist[k].network_delays.size();
            if (samples) {
                double m = getMaxNetworkDelay(chist[k].src_id);
                if (m > maxd)
                maxd = m;
            }
        }

        return maxd;
    }

    double Stats::getAverageHopCount(const int src_id)
    {
        double sum = 0.0;

        int i = searchCommHistory(src_id);

        assert(i >= 0);

        for (unsigned int j = 0; j < chist[i].hop_counts.size(); j++)
        sum += chist[i].hop_counts[j];

        return sum / (double) chist[i].hop_counts.size();
    }

    double Stats::getAverageHopCount()
    {
        double avg = 0.0;

        for (unsigned int k = 0; k < chist.size(); k++) {
            unsigned int samples = chist[k].hop_counts.size();
            if (samples)
            avg += (double) samples *getAverageHopCount(chist[k].src_id);
        }

        return avg / (double) getReceivedPackets();
    }

    double Stats::getMaxHopCount(const int src_id)
    {
        double maxd = -1.0;

        int i = searchCommHistory(src_id);

        assert(i >= 0);

        for (unsigned int j = 0; j < chist[i].hop_counts.size(); j++)
        if (chist[i].hop_counts[j] > maxd) {
            maxd = chist[i].hop_counts[j];
        }
        return maxd;
    }

    double Stats::getMaxHopCount()
    {

        double maxd = -1.0;

        for (unsigned int k = 0; k < chist.size(); k++) {
            unsigned int samples = chist[k].hop_counts.size();
            if (samples) {
                double m = getMaxHopCount(chist[k].src_id);
                if (m > maxd)
                maxd = m;
            }
        }

        return maxd;
    }

    double Stats::getAverageThroughput(const int src_id)
    {
        int i = searchCommHistory(src_id);

        assert(i >= 0);

        // not using GlobalParams::simulation_time since
        // the value must takes into account the invokation time
        // (when called before simulation ended, e.g. turi signal)
        int current_sim_cycles = sc_time_stamp().to_double()/GlobalParams::clock_period_ps - warm_up_time - GlobalParams::reset_time;

        if (chist[i].total_received_flits == 0)
        return -1.0;
        else
        return (double) chist[i].total_received_flits / current_sim_cycles;
        //(double) chist[i].last_received_flit_time;
    }

    double Stats::getAverageThroughput()
    {
        double sum = 0.0;

        for (unsigned int k = 0; k < chist.size(); k++) {
            double avg = getAverageThroughput(chist[k].src_id);
            if (avg > 0.0)
            sum += avg;
        }

        return sum;
    }

    unsigned int Stats::getReceivedPackets()
    {
        int n = 0;

        for (unsigned int i = 0; i < chist.size(); i++)
        n += chist[i].delays.size();

        return n;
    }

    unsigned int Stats::getReceivedFlits()
    {
        int n = 0;

        for (unsigned int i = 0; i < chist.size(); i++)
        n += chist[i].total_received_flits;

        return n;
    }

    unsigned int Stats::getTotalCommunications()
    {
        return chist.size();
    }

    double Stats::getCommunicationEnergy(int src_id, int dst_id)
    {
        // NOT YET IMPLEMENTED
        // Assumptions: minimal path routing, constant packet size
        /*
        Coord src_coord = id2Coord(src_id);
        Coord dst_coord = id2Coord(dst_id);

        int hops =
        abs(src_coord.x - dst_coord.x) + abs(src_coord.y - dst_coord.y);

        double energy =
        hops * (power.getPwrArbitration() + power.getPwrCrossbar() +
        power.getPwrBuffering() *
        (GlobalParams::min_packet_size +
        GlobalParams::max_packet_size) / 2 +
        power.getPwrRouting() + power.getPwrSelection()
    );

    return energy;
    */
    return -1.0;
}

int Stats::searchCommHistory(int src_id)
{
    for (unsigned int i = 0; i < chist.size(); i++)
    if (chist[i].src_id == src_id)
    return i;

    return -1;
}

void Stats::showStats(int curr_node, std::ostream & out, bool header)
{
    if (header) {
        out << "%"
        << setw(5) << "src"
        << setw(5) << "dst"
        << setw(10) << "delay avg"
        << setw(10) << "delay max"
        << setw(15) << "throughput"
        << setw(13) << "energy"
        << setw(12) << "received" << setw(12) << "received" << setw(12) << "disorder" << endl;
        out << "%"
        << setw(5) << ""
        << setw(5) << ""
        << setw(10) << "cycles"
        << setw(10) << "cycles"
        << setw(15) << "flits/cycle"
        << setw(13) << "Joule"
        << setw(12) << "packets" << setw(12) << "flits" << setw(12) << "flits" << endl;
    }
    for (unsigned int i = 0; i < chist.size(); i++) {
        out << " "
        << setw(5) << chist[i].src_id
        << setw(5) << curr_node
        << setw(10) << getAverageDelay(chist[i].src_id)
        << setw(10) << getMaxDelay(chist[i].src_id)
        << setw(15) << getAverageThroughput(chist[i].src_id)
        << setw(13) << getCommunicationEnergy(chist[i].src_id,
            curr_node)
            << setw(12) << chist[i].delays.size()
            << setw(12) << chist[i].total_received_flits
            << setw(12) << chist[i].disorder_flit_count << endl;
        }

        out << "% Aggregated average delay (cycles): " << getAverageDelay() <<
        endl;
        out << "% Aggregated average throughput (flits/cycle): " <<
        getAverageThroughput() << endl;
    }
