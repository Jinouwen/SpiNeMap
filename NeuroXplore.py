from __future__ import print_function
import re
import math
import string
import operator
import time
import sys
import random as rn
import os
import getopt
import subprocess

#####################################################################################################
# CONFIG Parameters:
#####################################################################################################
SimulationTool='CARLsim'
ApplicationName='Smooth'
ApplicationLocation='/home/disco-toolchain/Documents/toolchain/SpiNeMap/test_apps/image_processing/'

#####################################################################################################
# CLUSTERING
#####################################################################################################
ClusterSize=128
UnrollFanin=2
ClusteringAlgorithm='min-clusters'

#####################################################################################################
# Mapping
#####################################################################################################
NetworkType='MESH'
Xdim=0
Ydim=0
FlitSize=16
RoutingAlgorithm='XY'
#####################################################################################################

localPath = os.path.dirname(os.path.abspath(__file__))

def testFormat(argv):
    try:
        opts, args = getopt.getopt(sys.argv[1:], "c:hv", ["--version", "--help"])
    except getopt.GetoptError, err:
        # print help information and exit:
        print(err) # will print something like "option -a not recognized"
        sys.exit(-1)
    
    if len(opts) == 0:
        print("Usage: python2 NeuroXplore.py -h")
        sys.exit(2)
    for flag, value in opts:
        if flag in ("-h", "--help"):
            sys.stdout = sys.stderr
            print("""Usage: python2 NeuroXplore.py [-c|-v|-h]
            -v, --version :  SpiNeMap Version
            -h, --help: Help
            -c: Config File""")
            sys.exit(2)     
        elif flag in ("-c"):
            configFile = value
            return configFile
        elif flag in ("-v", "--version"):
            print("NeuroXplore v1.0.0")
            sys.exit(2)
        else:
            assert False, "Unhandled option"
            sys.exit(-1)

def printLogs(handler):
    for line in handler.stdout:
        print(line)

def printSimLogs(handler):
    i = 0
    for line in handler.stdout:
        if i ==1:
            print(line)
        i = 1
def parseConfigFile(lines):

    for line in lines: 
        # Check to see if comment. 
        if line[0] not in ['#']:
            line = line.split(':')
            if len(line) > 1:
                keyword = line[0].strip()
                value = line[1].strip()
                if keyword == 'SimulationTool':
                    SimulationTool = value
                elif keyword == 'Simulation':
                    Simulation = value
                elif keyword == 'ApplicationName':
                    ApplicationName = value
                elif keyword == 'ApplicationLocation':
                    ApplicationLocation= value
                elif keyword == 'Clustering':
                    Clustering = value
                elif keyword == 'ClusterSize':
                    ClusterSize = int(value)
                elif keyword == 'UnrollFanin':
                    UnrollFanin = int(value)
                elif keyword == 'ClusteringAlgorithm':
                    ClusteringAlgorithm = value
                elif keyword == 'Mapping':
                    Mapping = value
                elif keyword == 'NetworkType':
                    NetworkType = value
                elif keyword == 'Xdim':
                    Xdim = int(value)
                elif keyword == 'Ydim':
                    Ydim = int(value)
                elif keyword == 'FlitSize':
                    FlitSize = int(value)
                elif keyword == 'RoutingAlgorithm':
                    RoutingAlgorithm = value
                else:
                    print("[ERROR]: Config value %s is incorrect" % keyword)
    return SimulationTool, Simulation, ApplicationName, ApplicationLocation, Clustering, ClusterSize, UnrollFanin, ClusteringAlgorithm, Mapping, NetworkType, Xdim, Ydim, FlitSize, RoutingAlgorithm
    

if __name__ == "__main__":
    configFile = testFormat(sys.argv)
    cfp = open(configFile, 'r')

    lines = cfp.readlines()

    SimulationTool, Simulation, ApplicationName, ApplicationLocation, Clustering, ClusterSize, UnrollFanin, ClusteringAlgorithm, Mapping,  NetworkType, Xdim, Ydim, FlitSize, RoutingAlgorithm = parseConfigFile(lines)
    
    
    if SimulationTool == 'CARLsim': 

        # Compile the CARLsim application:
        CARLsimCompile = subprocess.Popen(['make', 'nocuda'], cwd=ApplicationLocation, stdin =subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True, bufsize=0)
        CARLsimCompile.wait()
        printLogs(CARLsimCompile)
        print("[INFO]: CARLsim Application Compiled.")
        
        # Execute the CARLsim application:
        CARLsimExecute = subprocess.Popen(['./smooth'], cwd=ApplicationLocation, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True, bufsize=0)
        #CARLsimExecute.wait()
        
        print("[INFO]: CARLsim Application Executed.")
        
        # Execute the CARLsim application:
        #CARLlogClean = subprocess.Popen(['perl', 'edit_carllog.pm', '/home/disco-toolchain/Documents/toolchain/SpiNeMap/test_apps/image_processing/results/carlsim.log', './tmp_out/'], cwd="/home/disco-toolchain/Documents/toolchain/SpiNeMap/CARLsim-Noxim-Parser/", stdin =subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True, bufsize=0)
        #CARLsimExecute.wait()
        print("[INFO]: CARLsim application log cleaned.")
        

        # Execute parser to extract Connection and Spike information:
        ExtractInfo = subprocess.Popen(['python2', 'log_parser.py', 'tmp_out/carl.log', 'tmp_out/'], cwd=localPath+"/Parser/Parser-CARLsim/", stdin =subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True, bufsize=0)
        ExtractInfo.wait()

        print("[INFO]: CARLsim application log parsed. Connection and Spike information extracted.")
        
        if Clustering == 'True':
            # Format the Connection Info and Spike Info to support Clustering:
            FormatInfo = subprocess.Popen(['python2', 'format.py', 'tmp_out/', '../'], cwd=localPath+"/Parser/Parser-CARLsim/", stdin =subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True, bufsize=0)
            print("[INFO]: Connection and Spike Information reformated.")
            #FormatInfo.wait()
        
            # Cluster neurons and synapses of application:
            Clustering = subprocess.Popen(['./clustering', '--conn-file', '../CARLsim-Noxim-Parser/input_next_layer/connection_info.txt', '--spike-file', '../CARLsim-Noxim-Parser/input_next_layer/spike_info.txt', '--unroll-fanin', str(UnrollFanin), '--cluster-crossbar-size', str(ClusterSize), '--clustering-algo', ClusteringAlgorithm, '--cluster-stats', 'input_next_layer.txt', '--cluster-ir-out', 'cluster.ir'], cwd="./SNNPartitioning/", stdin =subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True, bufsize=0)
            #CARLsimExecute.wait()

        print("[INFO]: Model Unrolling and Clustering completed.")
    
        if Mapping == 'True':
            # place the clusters on to the nodes of Noxim using PSO:
            Placement = subprocess.Popen(['python', 'placer.py', '../../SNNPartitioning/input_next_layer.txt', './'], cwd=localPath+"/SNNPlacer/pso", stdin =subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True, bufsize=0)
            # wait for the completion of the application execution. (Generate the carlsim.log) 
            print("[INFO]: Partition information Read.")
            print("[INFO]: Traffic File Created.")
            print("[INFO]: Cluster to Cluster connection table generated.")
            print("[INFO]: Node to node mapping table generated.")
            print("[INFO]: Noxim configuration file generated.")

            Placement.wait()
    
            # Simulate application on Noxim:
            NoximExecute = subprocess.Popen(['./noxim'], cwd="./snnNoxim/bin", stdin =subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True, bufsize=0)
            # wait for the completion of the application execution. (Generate the carlsim.log) 
            #CARLsimExecute.wait()
           
            print("[INFO]: Application Executed on Noxim.")


######################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################

                                                                            # Tensorflow
            
######################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################

    elif SimulationTool == 'tensorflow':
        # Execute the CARLsim application:
        TensorflowExecute = subprocess.Popen(['snntoolbox', '-t', ApplicationLocation+'config'], cwd=ApplicationLocation, stdin =subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True, bufsize=0)
        TensorflowExecute.wait()
        printSimLogs(TensorflowExecute)
        
        SpikeInfo = subprocess.Popen(['python3', 'extract_spike_times.py', ApplicationLocation, '../output/'], cwd='Parser/Parser-Tensorflow/spike_info', stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True, bufsize=0)
        SpikeInfo.wait()

        
        print("[INFO]: SpikeInfo extracted.")
        
        
        ConnectionInfo = subprocess.Popen(['./ncc', ApplicationLocation+ApplicationName+'.json', ApplicationLocation+ApplicationName+'.h5'], cwd="./Parser/Parser-Tensorflow/connection_info", stdin =subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True, bufsize=0)
        ConnectionInfo.wait()
        print("[INFO]: ConnectionInfo extracted.")


        # copy connection info to parser location
        #CopyInfo = subprocess.Popen(['cp', './Parser/Parser-Tensorflow/connection_info/example/'+ApplicationName+'.connection_info.txt', './Parser/Parser-Tensorflow/connection_info.txt'], cwd='./', stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True, bufsize=0)
        #CopyInfo.wait()
        
        formatInfo = subprocess.Popen(['python3', 'parser.py', './output/', './output/'], cwd='Parser/Parser-Tensorflow', stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True, bufsize=0)
        #formatInfo.wait()

        if Clustering == 'True':
            # Cluster neurons and synapses of application:
            Clustering = subprocess.Popen(['./ext', '--conn-file', '../Parser/Parser-Tensorflow/output/connection_info.txt', '--spike-file', '../Parser/Parser-Tensorflow/output/spike_info.txt', '--unroll-fanin', str(UnrollFanin), '--cluster-crossbar-size', str(ClusterSize), '--clustering-algo', ClusteringAlgorithm, '--cluster-stats', 'input_next_layer.txt', '--cluster-ir-out', 'cluster.ir'], cwd="./SNNPartitioning/", stdin =subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True, bufsize=0)
            #CARLsimExecute.wait()

        print("[INFO]: Model Unrolling and Clustering completed.")
        
        if Mapping == 'True':
            # place the clusters on to the nodes of Noxim using PSO:
            Placement = subprocess.Popen(['python', 'placer.py', '../../SNNPartitioning/input_next_layer.txt', './', str(Xdim), str(Ydim)], cwd=localPath+"/SNNPlacer/pso", stdin =subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True, bufsize=0)
            
            
            # wait for the completion of the application execution. (Generate the carlsim.log) 
            print("[INFO]: Partition information Read.")
            print("[INFO]: Traffic File Created.")
            print("[INFO]: Cluster to Cluster connection table generated.")
            print("[INFO]: Node to node mapping table generated.")
            print("[INFO]: Noxim configuration file generated.")

            Placement.wait()
    
            # Simulate application on Noxim:
            NoximExecute = subprocess.Popen(['./noxim', '>>', ApplicationName+'.out'], cwd="./snnNoxim/bin", stdin =subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True, bufsize=100)
            (output, err) = NoximExecute.communicate()
            print("[INFO]: Application Executed on Noxim.")
            print(output)
            # wait for the completion of the application execution. (Generate the carlsim.log) 
            #CARLsimExecute.wait()
           
