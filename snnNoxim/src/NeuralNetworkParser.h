/*
 * NeuralNetworkParser.h
 *
 *  Created on: Jun 3, 2016
 *      Author: khanh
 */

#ifndef NOXIM_NEURALNETWORKPARSER_H_
#define NOXIM_NEURALNETWORKPARSER_H_

#include "GlobalParams.h"
#include "systemc.h"
#include <assert.h>
#include <vector>
#include <map>

using namespace std;

// pair of layerID - neuron ID
//typedef pair<int, int> NeuronPair;
typedef int NeuronID;
typedef int GroupID;
typedef int NodeID;
//map to connect a neuron's layer and ID to a virtual GroupID. 
//typedef map <NeuronID, GroupID> NeuronGroupMap;
//typedef vector<NeuronGroup> NeuronGroupMap;

typedef vector<int> Destinations;

class NeuronGroup
{
    public:
    int groupID;
    //int layerID;
    int neuronID;
    
    //constructor 
    NeuronGroup(int group)
        :groupID(group)
        {}

    bool operator<(const NeuronGroup &right) const
    {
        //if (groupID == right.groupID)
        //{
            //if(layerID == right.layerID)
            //{
                //return neuronID < right.neuronID;
            //}
            //else
            //{
            //    return layerID < layerID;
            //}
        //}
        //else
       // {
            return groupID < right.groupID;
        //}
    }

    bool operator==(const NeuronGroup &right) const
    {
        //if(groupID == right.groupID && layerID == right.layerID && neuronID == right.neuronID)
        if(groupID == right.groupID)// && neuronID == right.neuronID)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    
    private: 
    Destinations destinationGroups;
    void updateNeuronGroup(int neuronNumber);
    //void updateDestinationGroups(int destinationGroupID);
};


// map a neuron group to a network node
typedef map <NeuronGroup, NodeID> NeuronNodeMap;


class NeuralNetworkParser
{
public:
    NeuralNetworkParser();
    bool loadNodeMap(const char * fname);
    bool loadConnection(const char * fname);
    int getNodeId(NeuronGroup neuron_group);
    vector <int> getDests(NeuronGroup neuron_group);
    bool isNodeMapValid();
    bool isConnectionValid();
    int getGroupId(int layerId, int neuronId);


private:
    bool nodemap_valid;
    bool connection_valid;
    // Maps the NeuronGroup object to a nodeID (strict one-one mapping. )
    NeuronNodeMap neuron_table;
    // Contains the individual group IDs in this NN architecture
    //NeuronGroupMap group_table;
    //maps a Neuron Group to a Node ID 
    map <NeuronGroup, Destinations> dest_table;
};



#endif /* NOXIM_NEURALNETWORKPARSER_H_ */
