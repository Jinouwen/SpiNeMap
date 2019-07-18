/*
* NeuralNetworkParser.cpp
*
* Created on: Jun 3, 2016
*    Author: khanh
*
* Modified on March 24, 2017
*    Author: Yuefeng Wu
*/

#include "NeuralNetworkParser.h"

NeuralNetworkParser::NeuralNetworkParser()
{
    nodemap_valid = false;
    connection_valid = false;

}

bool NeuralNetworkParser::loadNodeMap(const char * fname)
{
    // Open file
    ifstream fin(fname, ios::in);
    if (!fin)
    return false;
    cout << fname << endl;
    // Initialize variables
    neuron_table.clear();
    // Cycle reading file
    while (!fin.eof())
    {
        char line[128];
        if (!fin.getline(line, sizeof(line) - 1))
        break;
        if ((line[0] != '\0') && (line[0] != '%')) {
            int group_id, layer_id, neuron_id, node_id;
            int params;

            if (GlobalParams::cluster_traffic == true)
            {
                params = sscanf(line, "%d %d", &group_id, &node_id);
                if (params == 3)
                {
                    NeuronGroup ng(group_id); //use 0 as dummy value for neuron_id
                    neuron_table[ng] = node_id;
                }
            }
            else
            {
                params = sscanf(line, "%d %d", &group_id, &node_id);
                if (params == 2)
                { 
                    //NeuronPair np(layer_id, neuron_id);
                    //group_table[np] = group_id;
                    
                    //ab3586
                    
                    // Neuron Group is the class that contains the information about the 
                    // TODO  : Remove the Neuron Group class and its objects. Seems like a waste of memory. Unless a relation between the group and
                    // its neurons needs to be established.  
                    NeuronGroup ng(group_id);
                    neuron_table[ng] = node_id;

                    //cout << "GroupID: " << group_id << " NodeId: " << node_id <<endl;
                }
            }
        }
    }
    nodemap_valid = true;
    return true;
}

bool NeuralNetworkParser::loadConnection(const char * fname)
{
    int numberofGroups = 0;
    // Open file
    ifstream fin(fname, ios::in);
    if (!fin)
    return false;

    // Initialize variables
    dest_table.clear();

    cout << "Connection file is : " << fname << endl;

    // read number of neuron groups
    /*while (!fin.eof())
    {
        char line[1024];
        fin.getline(line, sizeof(line) - 1);
        if ((line[0] != '\0') && (line[0] != '%')) {
            int params = sscanf(line, "%d", &numberofGroups);
            if (params == 1)
            break;
        }
    }*/

    // Cycle reading file
    while (!fin.eof())
    {
        int group_id, layer_id, neuron_id, dst_number;
        if (!(fin >> group_id >> dst_number)){
            break;
        }

        if (GlobalParams::output_mode == DEBUG_MODE){
            //cout << "\nNeuron: " << neuron_id << " in  Layer " << layer_id << " has " << dst_number << " destionations." << endl;
        }
        
        //ab3586 : The number of groups are not equal to the number of layers
        //assert(layer_id <= group_number);
        //NeuronPair np(layer_id, neuron_id);
        //group_id = group_table[np];

        NeuronGroup ng_src(group_id);

        int layer_id_dst, group_id_dst;
        
        if (GlobalParams::output_mode == DEBUG_MODE){
            //cout << "Destination neurons are in Layer " << layer_id_dst << endl;
        }

        for (int i = 0; i < dst_number; i++){
            int dst;
            fin >> dst;
    
            //ab3586: find the group that the destination neuron belongs to and create a NeuronGroup.     
            //NeuronPair np(layer_id_dst, dst);
            //group_id_dst = group_table[np];
            
            NeuronGroup ng_dst(dst);
            
            if (find(GlobalParams::rout_only_nodes.begin(), GlobalParams::rout_only_nodes.end(), neuron_table[ng_dst]) == GlobalParams::rout_only_nodes.end()){
                if (neuron_table[ng_src] != neuron_table[ng_dst]){
                    if (GlobalParams::output_mode == DEBUG_MODE){
                        cout << dst << '\t';
                    }
                    dest_table[ng_src].push_back(neuron_table[ng_dst]);
                }
            }
        }

        if (GlobalParams::output_mode == DEBUG_MODE){
            cout << endl;
        }

        if (dest_table[ng_src].size() > 0)
        {
            // Erase duplicated destinations
            sort(dest_table[ng_src].begin(), dest_table[ng_src].end());
            dest_table[ng_src].erase(unique(dest_table[ng_src].begin(), dest_table[ng_src].end()), dest_table[ng_src].end());
        }

        // NeuronGroup ng_src(group_id, neuron_id);
        // fin.getline(line, sizeof(line) - 1);
        // if ((line[0] != '\0') && (line[0] != '%')) {
        //     int group_id, neuron_id, dst_number;
        //
        //
        //     int params = sscanf(line, "%d %d %d", &group_id, &neuron_id, &dst_number);
        //     if (params == 3)
        //     {
        //         NeuronGroup ng_src(group_id, neuron_id);
        //         int group_dst_id;
        //         //                node_id = neuron_table[ng_src];
        //         //                printf("Synapses %d %d %d: ", group_id, neuron_id, dst_number);
        //         if (dst_number > 0)
        //         {
        //             fin.getline(line, sizeof(line) - 1);
        //             while ((line[0] == '\0') || (line[0] == '%'))
        //             fin.getline(line, sizeof(line) - 1);
        //             stringstream ss(line);
        //             ss >> group_dst_id;
        //             for (int i = 0; i < dst_number; i++)
        //             {
        //                 int dst;
        //                 ss >> dst;
        //                 NeuronGroup ng_dst(group_dst_id, dst);
        //                 if (find(GlobalParams::rout_only_nodes.begin(), GlobalParams::rout_only_nodes.end(), neuron_table[ng_dst]) ==
        //                 GlobalParams::rout_only_nodes.end())
        //                 if (neuron_table[ng_src] != neuron_table[ng_dst])
        //                 dest_table[ng_src].push_back(neuron_table[ng_dst]);
        //             }
        //             if (dest_table[ng_src].size() > 0)
        //             {
        //                 // Erase duplicated destinations
        //                 sort(dest_table[ng_src].begin(), dest_table[ng_src].end());
        //                 dest_table[ng_src].erase(unique(dest_table[ng_src].begin(), dest_table[ng_src].end()), dest_table[ng_src].end());
        //             }
        //         }
        //
        //     }
        // }
    }

    connection_valid = true;
    return true;
}

vector <int> NeuralNetworkParser::getDests(NeuronGroup neuron_group)
{
    assert(connection_valid);
    return dest_table[neuron_group];
}

int NeuralNetworkParser::getNodeId(NeuronGroup neuron_group)
{
    assert(nodemap_valid);
    if (neuron_table.find(neuron_group) != neuron_table.end())
    return neuron_table[neuron_group];
    else
    return -1;
}


int NeuralNetworkParser::getGroupId(int layerId, int neuronId)
{
    int group_src = 0;
    //NeuronPair np(layerId, neuronId); 
    return 0;
    //return group_src = group_table[np];

}
