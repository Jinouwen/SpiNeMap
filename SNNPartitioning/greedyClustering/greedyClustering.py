from __future__ import print_function
import numpy as np
import re
import math
import string
import operator
import time


# Global variables
spikeMap = {}
connectionMap = {}
# group to neuron mapping
groupMap = {}
# group to group mapping -> Key is the source group, value is a list of destination groups.
groupGroupMap = {}
# group to spike mapping -> Map the spike times generated by a group only if the spikes are sent to another group
# internal spikes are ignored
groupSpikeMap = {}

spikeCountMap = {}

itr = 0 
numNeurons = 1000
numNeuronsPerCluster = 256
if numNeurons % numNeuronsPerCluster:
    numGroups = int(numNeurons/numNeuronsPerCluster) + 1
else:
    numGroups = int(numNeurons/numNeuronsPerCluster)

ISI = 0
spikeDisorder = 0


def numberOfSpikes(srcNeuron, destNeuron): 
    # find the number of spikes communicated between two neurons.
    # the 2 neurons cannot belong to the same crossbar. 
    if srcNeuron in connectionMap:
        spikes = 0
        for i in connectionMap[srcNeuron]:
            if int(i) == destNeuron:
                if srcNeuron in spikeMap:
                    spikes = len(spikeMap[srcNeuron])
                    break
                else:
                    spikes = 0
            else:
                spikes = 0
    else:
        spikes = 0
    
    return spikes


def generateGroupInfo(x):
    groupMap.clear()
    for groupNumber in range(numGroups): 
        itr = 0
        while itr in range(numNeurons):
            neuronNumber = itr+(groupNumber*numNeurons)
            if x[neuronNumber] == 1: 
                if groupNumber not in groupMap:
                    groupMap[groupNumber] = [itr]
                else:
                    groupMap[groupNumber].append(itr)

            itr = itr+1

    # Group -> Node mapping does not need to be generated as it is a one-one mapping at the moment
    # Generate Traffic.txt file. Contains spike time, source group ID. 
    # Generate ConnectionMap -> Source Group to Destination Group. 
    for sourceGroup in range(numGroups):
        for sourceNeuron in groupMap[sourceGroup]:
            if sourceNeuron in connectionMap:
                for destNeuron in connectionMap[sourceNeuron]:
                    if destNeuron not in groupMap[sourceGroup]:
                        
                        # find the group number that the dest neuron belongs to
                        for destGroup in groupMap:
                            if destNeuron in groupMap[destGroup]:
                                if sourceGroup not in groupGroupMap: 
                                    groupGroupMap[sourceGroup] = [destGroup]
                                else: 
                                    if destGroup not in groupGroupMap[sourceGroup]:
                                        groupGroupMap[sourceGroup].append(destGroup)
                        
                        if sourceNeuron in spikeMap:
                            if sourceGroup not in groupSpikeMap: 
                                
                                groupSpikeMap[sourceGroup] = spikeMap[sourceNeuron]
                            #else:
                                #for spike in spikeMap[sourceNeuron]:
                                groupSpikeMap[sourceGroup].extend(spikeMap[sourceNeuron])
    
    # Generating connection_table.txt and traffic.txt
    trafficFile = open("traffic1.txt", 'w')
    connectionFile = open("connection_table1.txt", 'w')

    print(groupSpikeMap)
    for group in range(numGroups):
        print(group)
        if group in groupSpikeMap:
            for spikeTime in groupSpikeMap[group]:
            # print timestamp --> source group number 
                trafficFile.write(str(spikeTime) + ' ' + str(group) + "\n")
    
    for group in range(numGroups):
        if group in groupGroupMap:
            connectionFile.write(str(group) + ' ' + str(len(groupGroupMap[group])) + "\n")
            for destGroup in groupGroupMap[group]:
                connectionFile.write(str(destGroup) + ' ')

            connectionFile.write('\n')
            
    connectionFile.close()
    trafficFile.close()
    
    # Trigger Noxim with the generated traffic and connectionTable, the node mapping is still one to one.  


def readSpikeInfo(logFileLines): 
    itr = 0
    while itr in range(0,len(logFileLines)):
        line = logFileLines[itr]
        neuronNumber_re = re.compile('[0-9]+,[0-9]+', flags=0)
        neuronNumber = str(neuronNumber_re.search(line).group()).split(',')
        neuronNumber = int(neuronNumber.pop(0))
        
        itr = itr + 1
        line = logFileLines[itr]
        spikeTimes_re = re.compile('([0-9]+ )+', flags=0)
        if(spikeTimes_re.search(line)):
            spikeTimes = spikeTimes_re.search(line).group().split(' ')
            spikeTimes.pop()
            spikeMap[neuronNumber] = [int(i) for i in spikeTimes] 
        else:
            spikeMap[neuronNumber] = []

        itr = itr + 1 

    #for neuronNumber in spikeMap: 
        #print("Neuron Number: " + str(neuronNumber) + " SpikeTimes: " + str(spikeMap[neuronNumber]) )
    

def readConnectionInfo(logFileLines):
    itr = 0 
    while itr in range(0, len(logFileLines)):
        line = logFileLines[itr]
        neuronNumber_re = re.compile('[0-9]+', flags=0)
        neuronNumber = str(neuronNumber_re.search(line).group())
        neuronNumber = int(neuronNumber)

        itr = itr + 1
        line = logFileLines[itr]
        connectedNeurons_re = re.compile('([0-9]+ )+', flags=0)
        if(connectedNeurons_re.search(line)):
            connectedNeurons = connectedNeurons_re.search(line).group().strip().split(' ')
            connectedNeurons = map(int, connectedNeurons)
            if neuronNumber in connectionMap:
                connectionMap[neuronNumber].extend(connectedNeurons)
            else:
                connectionMap[neuronNumber] = connectedNeurons
                    
        else:
            connectionMap[neuronNumber] = []

        itr = itr + 2 

    #for neuronNumber in connectionMap: 
        #print("Neuron Number: " + str(neuronNumber) + " Connections: " + str(connectionMap[neuronNumber]) )

# checks to see if two neurons are connected in a design
def isConnected(srcNeuron, destNeuron):
    if srcNeuron in connectionMap:
        if destNeuron in connectionMap[srcNeuron]:
            return 1
        else: 
            return 0
    else:
        return 0

def optimizationFunction(x, *args):
    
    #finding the total number of spikes communicated between the crossbars.
    totalSpikeCount = 0
    for srcCrossbar in range(numGroups):
        totalSpikePerCrossbar = 0
        for srcNeuron in range(numNeurons):
            spikeCount = 0
                
            if x[srcCrossbar*numNeurons + srcNeuron] == 1:
                for destCrossbar in range(numGroups):
                    if destCrossbar != srcCrossbar:
                        for destNeuron in range(numNeurons):
                            if x[destNeuron+destCrossbar*numNeurons]:
                                spikeCount = spikeCount + numberOfSpikes(srcNeuron, destNeuron)
            totalSpikePerCrossbar = totalSpikePerCrossbar + spikeCount
        totalSpikeCount = totalSpikeCount + totalSpikePerCrossbar
    return totalSpikeCount

def greedyGroupingAlgorithm(x): 


    # map to store the total number of spikes communicated.
    spikingMap = {}

    # the final locations of the neurons will be stored in a 2D list [numGroups][numNeurons]
    solution = [[] for j in range(numGroups)]
   
    #print(solution)
    # create a list of neurons that indicate the neurons that have not been placed yet. 
    allNeurons = range(numNeurons)

    # create a list to store the number of neurons communicated between different neurons in the 

    for neuron in spikeMap: 
        spikeCountMap[neuron] = len(spikeMap[neuron])

    # A tuple that stores the neurons in decending order of their activity. 
    spikeCounter = sorted(spikeCountMap.items(), key=lambda x: x[1], reverse=True)
   
    # The first neuron to be placed in the  group will be the neuron that communicates the most. 
    firstNeuron = spikeCounter[0][0]
    # all the neurons that are connected to firstNeurons must also belong to this group.
    if firstNeuron in connectionMap:
        destinationNeurons = connectionMap[firstNeuron]
    else: 
        destinationNeurons = []

    # variable indicates which group to place neurons in 
    presentGroup = 0 

    # place the first neuron and its connected neurons in Group 0.
    solution[0].append(firstNeuron)
    # Del the neuron from the neuron list - indicate that it has been placed. 
    del(allNeurons[firstNeuron])
    
    #if len(destinationNeurons):
        #for dest in destinationNeurons:
           # solution[0].append(dest)
           # del(allNeurons[dest])

    #print(solution)

    
    # Once first neurons have been added to the group - look for the neuron that communicates with them the most. 
    while (len(allNeurons)):
        #print(len(allNeurons))
        spikingMap.clear()
        bestSpike = 0 
        if( len(solution[presentGroup]) == numNeuronsPerCluster):
            presentGroup+=1
        
        for neuron in allNeurons:    
            # check to see if the group has been filled.
            spike = 0
            
            for placedNeuron in solution[presentGroup]:
                spike += numberOfSpikes(placedNeuron, neuron)
                spike += numberOfSpikes(neuron, placedNeuron)
        
            spikingMap[neuron] = spike
        
        bestSpike = sorted(spikingMap.items(), key=lambda x:x[1], reverse=True)
        bestNeuron = bestSpike[0][0]
        solution[presentGroup].append(bestNeuron)
        
        allNeurons.remove(bestNeuron)
        
       # print(allNeurons)
    for groups in range(len(solution)):
        for neurons in range(len(solution[groups])):
            neuron = solution[groups][neurons]
            x[groups*numNeurons + neuron] = 1

    #print(x)
# Check to see which of the remaining neuron communicates the most with the neurons in present group.
         


###################################################################################################################

######################################## MAIN FUNCTION ###########################################################

###################################################################################################################


logFile = open('../spike_table.txt', 'r')
logFileLine = logFile.readlines()
readSpikeInfo(logFileLine)
logFile.close()
print("Spike Data Read")

logFile = open('../connection_table.txt', 'r')
logFileLine = logFile.readlines()
readConnectionInfo(logFileLine)
logFile.close()
print("Connection Data Read")


x = [0]*numNeurons*numGroups
start_time = time.time()
greedyGroupingAlgorithm(x)
#print(x)
print(optimizationFunction(x))
print("--- %s seconds ---" % (time.time() - start_time))
generateGroupInfo(x)
#generateSpikeInfo(x)


'''
    for item in spikeCounter: 
        srcNeuron = item[0]
        if srcNeuron in connectionMap:
            destNeurons = connectionMap[srcNeuron]
            totalNeurons = len(destNeurons) + 1
        else:
            destNeurons = []
            totalNeurons = len(destNeurons) + 1
        for group in range(numGroups):
            totalNeuronsinGroup = 0
            print('here')
            for neuron in range(numNeurons):
                totalNeuronsinGroup += x[group*numNeurons+neuron]
                print(totalNeuronsinGroup) 
            if totalNeuronsinGroup <= numNeuronsPerCluster - totalNeurons:
                total = 0
                for group1 in range(numGroups):
                    total += x[srcNeuron + group1*numNeurons]
            
                if total == 0:
                    x[srcNeuron + group*numNeurons] = 1

                for neuron1 in destNeurons:
                    total = 0            
                    for group1 in range(numGroups):
                        total += x[srcNeuron + group1*numNeurons]
                
                    if total == 0:
                        x[srcNeuron + group*numNeurons] = 1
'''
