/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2015 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the top-level of Noxim
 */
#if 1

#include "ConfigurationManager.h"
#include "DataStructs.h"
#include "GlobalBusStats.h"
#include "GlobalParams.h"
#include "GlobalStats.h"
#include "NoC.h"
#include "NoS.h"
#include "Segment.h"
#include "Utils.h"
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <fstream>

#include <csignal>

using namespace std;

// need to be globally visible to allow "-volume" simulation stop
unsigned int drained_volume;
NoC *n;
NoS *nos;

void signalHandler(int signum) {
    if (GlobalParams::output_mode != EX_STAT_MODE){
        cout << "\b\b  " << endl;
        cout << endl;
        cout << "Current Statistics:" << endl;
        cout << "(" << sc_time_stamp().to_double() / GlobalParams::clock_period_ps
             << " sim cycles executed)" << endl;

        if (GlobalParams::network_type == "MESH") {
            GlobalStats gs(n);
            gs.showStats(std::cout, GlobalParams::detailed);
        } else {
            GlobalBusStats gbs(nos);
            gbs.showStats(std::cout);
        }
    }
}

int sc_main(int arg_num, char *arg_vet[]) {
    signal(SIGQUIT, signalHandler);

    // TEMP
    drained_volume = 0;

    // Handle command-line arguments
    if (GlobalParams::output_mode != EX_STAT_MODE) {
        cout << endl
             << "\t\tNoxim - the NoC Simulator" << endl;
        cout << "\t\t(C) University of Catania" << endl
             << endl;
    }

    //cout << "Checkpoint 1 : NoC to be created. \n";
    configure(arg_num, arg_vet);
    
    /* 
    sc_time_unit time_unit = SC_MS;
    if (GlobalParams::clock_period_ps < 1000)
        time_unit = SC_PS;
    else if (GlobalParams::clock_period_ps < 1000000)
        {
            GlobalParams::clock_period_ps = GlobalParams::clock_period_ps/1000;
            time_unit = SC_NS;
        }
    else if (GlobalParams::clock_period_ps < 1000000000)
        {
            GlobalParams::clock_period_ps = GlobalParams::clock_period_ps/1000000;
            time_unit = SC_US;
        }
    */
    sc_time_unit time_unit = SC_PS;
    // Signals
    sc_clock clock("clock", GlobalParams::clock_period_ps, time_unit);
    // sc_clock clock("clock", 500, SC_PS);
    sc_signal<bool> reset;

    // Initialise trace
    sc_trace_file *tf = NULL;

   // cout << "Checkpoint 7" <<endl;
    if (GlobalParams::network_type == "MESH") {
        // NoC instance
        n = new NoC("NoC");
        std::cout << "Created NoC!" << endl;

        // Removing old statistics
        if (remove("stat.csv") == 0) {
            std::cout << "Removed old statistics!" << '\n';
        } else {
            std::cerr << "Statistic file doesn't exist, continue." << '\n';
        }
        n->clock(clock);
        n->reset(reset);

        // Trace signals

        if (GlobalParams::trace_mode) {
            tf = sc_create_vcd_trace_file(GlobalParams::trace_filename.c_str());
            sc_trace(tf, reset, "reset");
            sc_trace(tf, clock, "clock");

            for (int i = 0; i < GlobalParams::dim_x; i++) {
                for (int j = 0; j < GlobalParams::dim_y; j++) {
                    char label[40];

                    sprintf(label, "req(%04d)(%04d).east", i, j);
                    sc_trace(tf, n->req[i][j].east, label);
                    sprintf(label, "req(%04d)(%04d).west", i, j);
                    sc_trace(tf, n->req[i][j].west, label);
                    sprintf(label, "req(%04d)(%04d).south", i, j);
                    sc_trace(tf, n->req[i][j].south, label);
                    sprintf(label, "req(%04d)(%04d).north", i, j);
                    sc_trace(tf, n->req[i][j].north, label);

                    sprintf(label, "ack(%04d)(%04d).east", i, j);
                    sc_trace(tf, n->ack[i][j].east, label);
                    sprintf(label, "ack(%04d)(%04d).west", i, j);
                    sc_trace(tf, n->ack[i][j].west, label);
                    sprintf(label, "ack(%04d)(%04d).south", i, j);
                    sc_trace(tf, n->ack[i][j].south, label);
                    sprintf(label, "ack(%04d)(%04d).north", i, j);
                    sc_trace(tf, n->ack[i][j].north, label);
                }
            }
        }

    } else if (GlobalParams::network_type == "SEGMENT") {
        if (GlobalParams::output_mode != EX_STAT_MODE) {
            cout << "Segmented bus mode enabled!!!" << endl;
        }
        // NoC instance

        nos = new NoS("NoS");
        if (GlobalParams::output_mode != EX_STAT_MODE) {
            std::cout << "Created Network of Segments!" << endl;
        }
        nos->clk(clock);
        nos->reset(reset);
    }

    if (GlobalParams::network_type == "MESH") {
        // Reset the chip and run the simulation
        reset.write(1);
        cout << "Reset for " << (int) (GlobalParams::reset_time) << " cycles... "
             << endl;
        srand(GlobalParams::rnd_generator_seed);

        //    double r_time =
        //    GlobalParams::reset_time*GlobalParams::clock_period_ps/pow(10,(time_unit-1)*3);
        //    cout << "r_time " << r_time << endl;
        reset.write(0);
    }

    if (GlobalParams::output_mode != EX_STAT_MODE) {
        cout << " Now running for " << GlobalParams::simulation_time
             << " cycles..." << endl;
    }

    timespec start, stop, diff;
    clock_gettime(CLOCK_REALTIME, &start);
    //    gettimeofday(&start, NULL);

    //    double s_time =
    //    GlobalParams::simulation_time*(GlobalParams::clock_period_ps/pow(10,(time_unit-1)*3));
    //    cout << "s_time " << s_time << endl;

    sc_start(GlobalParams::simulation_time *
                 (GlobalParams::clock_period_ps / pow(10, (time_unit - 1) * 3)),
             time_unit);
    cout << "Runtime: " << GlobalParams::clock_period_ps / pow(10, (time_unit - 1) * 3) << GlobalParams::simulation_time *
    (GlobalParams::clock_period_ps / pow(10, (time_unit - 1) * 3)) << endl;

    //    gettimeofday(&stop, NULL);
    clock_gettime(CLOCK_REALTIME, &stop);
    diff.tv_sec = stop.tv_sec - start.tv_sec;
    diff.tv_nsec = stop.tv_nsec - start.tv_nsec;

    // Close the simulation
    if (GlobalParams::trace_mode)
        sc_close_vcd_trace_file(tf);
    if (GlobalParams::output_mode == EX_STAT_MODE) {
        
        if (GlobalParams::network_type == "SEGMENT") {
            cout << "IMEC Segmented Bus Simulation completed.\n";
            cout << " ##Designed by Yuefeng Wu @ imec NL##\n";
            cout << "Simulator is based on Noxim Architecture.\n";
        } else {
            cout << "Noxim simulation completed." << endl;
        }
        cout << sc_time_stamp().to_string() << " ( "
             << sc_time_stamp().to_double() / GlobalParams::clock_period_ps
             << " cycles) executed." << endl;
        cout << "Total simulation time: " << (long long) (diff.tv_sec * 1000L)
             << " seconds and " << (long long) (diff.tv_nsec / 1.0e6)
             << " milliseconds." << endl;

        if (GlobalParams::network_type == "MESH") {
            GlobalStats gs(n);
            
            ofstream myfile;

            myfile.open("output.txt");
            myfile << gs.getAverageNetworkDelay();
            myfile << "\n";
            myfile << gs.getDynamicPower();
            myfile.close();

            gs.showStats(std::cout, GlobalParams::detailed);
            if ((GlobalParams::max_volume_to_be_drained > 0) &&
                (sc_time_stamp().to_double() / GlobalParams::clock_period_ps -
                     GlobalParams::reset_time >=
                 GlobalParams::simulation_time)) {
                cout << endl
                     << "WARNING! the number of flits specified with -volume option" << endl
                     << "has not been reached. ( " << drained_volume << " instead of "
                     << GlobalParams::max_volume_to_be_drained << " )" << endl
                     << "You might want to try an higher value of simulation cycles" << endl
                     << "using -sim option." << endl;

#ifdef TESTING
                cout << endl
                     << " Sum of local drained flits: " << gs.drained_total << endl
                     << endl
                     << " Effective drained volume: " << drained_volume;
#endif
            }
        }

        if (GlobalParams::network_type == "SEGMENT" && GlobalParams::output_mode != EX_STAT_MODE) {
            GlobalBusStats gbs(nos);
            if (GlobalParams::output_mode == DEBUG_MODE){
                ofstream spiking_log, receiving_log;
                spiking_log.open("log.sp", ios::out);
                receiving_log.open("log.re", ios::out);
                gbs.dumpSpikingLogOut(spiking_log);
                gbs.dumpReceivingLogOut(receiving_log);
            }
            gbs.showStats(std::cout);
        }

        // Show statistics
        //   GlobalStats gs(n);
        //   gs.showStats(std::cout, GlobalParams::detailed);
        //
        //   if ((GlobalParams::max_volume_to_be_drained > 0) &&
        //       (sc_time_stamp().to_double() / GlobalParams::clock_period_ps -
        //            GlobalParams::reset_time >=
        //        GlobalParams::simulation_time)) {
        //     cout << endl
        //          << "WARNING! the number of flits specified with -volume option" << endl
        //          << "has not been reached. ( " << drained_volume << " instead of "
        //          << GlobalParams::max_volume_to_be_drained << " )" << endl
        //          << "You might want to try an higher value of simulation cycles" << endl
        //          << "using -sim option." << endl;
        //
        // #ifdef TESTING
        //     cout << endl
        //          << " Sum of local drained flits: " << gs.drained_total << endl
        //          << endl
        //          << " Effective drained volume: " << drained_volume;
        // #endif
        //   }
        //
    }
    else{
        cout<<"cycle:"<<sc_time_stamp().to_double() / GlobalParams::clock_period_ps<<endl;
    }

#ifdef DEADLOCK_AVOIDANCE
    cout << "***** WARNING: DEADLOCK_AVOIDANCE ENABLED!" << endl;
#endif
    return 0;
}
#endif
