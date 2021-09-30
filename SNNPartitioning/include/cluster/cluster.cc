#include "cluster/cluster.hh"

// #include <execution>
#include <iostream>
#include <random>

namespace EXT
{
namespace Clustering
{
// The goal is to minimize the number of clusters
void Clusters::minClusters(std::vector<Neuron>& snn)
{
    std::cout << "---------------------------------------\n";
    std::cout << "Clustering Step - Minimize Clusters#\n";
    std::cout << "Minimum fanin: " << MIN_FANIN << "\n";
    std::cout << "Crossbar size: " << CROSSBAR_SIZE << "\n";
    std::cout << "---------------------------------------\n";

    // Record the clustering status for each neuron
    neuron_status.resize(snn.size());
    for (auto &neuron : snn)
    {
        UINT64 id = neuron.getNeuronId();
        boost::multiprecision::cpp_int spikes = neuron.numOfSpikes();

        neuron_status[id].setSpikeTimes(&(neuron.getSpikeTimes()));
        neuron_status[id].setNeuronId(id);
        neuron_status[id].setNumOfSpikes(spikes);
        if (neuron.hasParent()) 
        { neuron_status[id].setParentId(neuron.getParentId()); }
    }

    // debugPrint();

    for (auto cur_neuron_idx = 0;
              cur_neuron_idx < snn.size();
              cur_neuron_idx++)
    {
        if (snn[cur_neuron_idx].hasParent()) { break; }

        if (snn[cur_neuron_idx].numInputNeurons()) 
        {
            // std::cout << "\nMapping neuron id: " 
            //     << snn[cur_neuron_idx].getNeuronId() << "\n";
            // All the input neurons before unrolling
            std::list<UINT64> non_unrolled_inputs;
            // Input neuron -> Output neuron mapping
            std::unordered_map<UINT64, UINT64> input_to_output_map;

            // Case 1: the neuron is unrolled
            if (auto &children = snn[cur_neuron_idx].getChildrenRef();
                    children.size() > 0)
            {
                std::set<UINT64> intermediate_neurons;
                for (auto &child : children) { intermediate_neurons.insert(child); }
                // for (auto &inter_neuron : intermediate_neurons)
                // { std::cout << inter_neuron << " "; } std::cout << "\n";

                for (auto &inter_neuron : children)
                {
                    for (auto &raw_input : snn[inter_neuron].getInputNeuronList())
                    {
                        if (intermediate_neurons.find(raw_input) == 
                            intermediate_neurons.end())
                        {
                            non_unrolled_inputs.push_back(raw_input);
                            input_to_output_map.insert({raw_input, inter_neuron});
                        }
                    }
                }
                for (auto &raw_input : snn[cur_neuron_idx].getInputNeuronList())
                {
                    if (intermediate_neurons.find(raw_input) == intermediate_neurons.end())
                    {
                        non_unrolled_inputs.push_back(raw_input);
                        input_to_output_map.insert({raw_input, cur_neuron_idx});
                    }
		}
            }
            // Case 2: the neuron is not unrolled
            else
            {
                auto &inputs_to_pack = snn[cur_neuron_idx].getInputNeuronList();
                for (auto i = 0; i < inputs_to_pack.size(); i++)
                {
                    auto input_to_pack = inputs_to_pack[i];
                    if (i == CROSSBAR_SIZE) { break; }

                    non_unrolled_inputs.push_back(input_to_pack);
                    input_to_output_map.insert({input_to_pack,
                                                snn[cur_neuron_idx].getNeuronId()});
                }
            }

            // At this stage, you have all the neurons ready
            // std::cout << "\n";
            // for (auto &input : non_unrolled_inputs)
            // { std::cout << input << " "; } std::cout << " | ";

            std::sort(// std::execution::par,
                      sorted_clusters.begin(),
                      sorted_clusters.end(),
                      [](auto &left, auto &right)
                      {
                          return left->getUtilization() > right->getUtilization();
                      });

            for (auto &cluster : sorted_clusters)
            {
                if (non_unrolled_inputs.size() == 0) { break; }

                UINT64 cid = cluster->getClusterId();

                unsigned num_to_pack = numCanBePacked(cid,
                                                      non_unrolled_inputs);

                /* Prepare a list of input neurons that can be packed */
                std::vector<UINT64> packed_neurons;
                auto cnt = 0;
                for (auto &input : non_unrolled_inputs)
                {
                    if (cnt == num_to_pack) break;
                    packed_neurons.push_back(input);
                    cnt++;
                }

                /* Pack the input neurons to the cluster */
                bool packed = packToCluster(num_to_pack,
                                  snn[cur_neuron_idx].getNeuronId(),
                                  cid,
                                  input_to_output_map,
                                  non_unrolled_inputs);

                /* Successfully packed to the cluster */
                if (packed)
                {
                    /* Get the output neuron which the input neurons connect to */
                    UINT64 cur_output_neuron_id;
                    if (non_unrolled_inputs.size() > 0)
                    {
                        cur_output_neuron_id = *(non_unrolled_inputs.begin());
                    }
                    else
                    {
                        cur_output_neuron_id = cur_neuron_idx;
                    }

                    /* Set the IO mappings */
                    auto &io_mappings = cluster->getIOMappings();
                    for (auto input : packed_neurons)
                    {
                        if (auto mapping_iter = io_mappings.find(input);
                                mapping_iter != io_mappings.end())
                        {
                            (mapping_iter->second).push_back(cur_output_neuron_id);
                        }
                        else
                        {
                            std::vector<UINT64> outs = {cur_output_neuron_id};
                            io_mappings.insert({input, outs});
                        }
                    }
                    // std::cout << num_to_pack << ": ";
                    // for (auto input : packed_neurons) 
                    // { std::cout << input 
                    //    << "(" << neuron_status[input].getParentId() 
                    //    << ") "; }
                    // std::cout << "-> " 
                    // << cur_output_neuron_id 
                    // << "(" 
                    // << neuron_status[cur_output_neuron_id].getParentId() << ") | ";
                }
            }
            // std::cout << "Inputs: ";
            // for (auto &input : non_unrolled_inputs) { std::cout << input << " "; }
            // std::cout << "\n";

            // Need new clusters to map the rest
            while (true)
            {
                if (non_unrolled_inputs.size() == 0) { break; }

                auto cid = addCluster();
                unsigned num_to_pack = numCanBePacked(cid,
                                                      non_unrolled_inputs);

                /* Prepare a list of input neurons that can be packed */
                std::vector<UINT64> packed_neurons;
                auto cnt = 0;
                for (auto &input : non_unrolled_inputs)
                {
                    if (cnt == num_to_pack) break;
                    packed_neurons.push_back(input);
                    cnt++;
                }

                /* Pack the input neurons to the cluster */
                bool packed = packToCluster(num_to_pack,
                                  snn[cur_neuron_idx].getNeuronId(),
                                  cid,
                                  input_to_output_map,
                                  non_unrolled_inputs);

                /* Successfully packed to the cluster */
                if (packed)
                {
                    /* Get the output neuron which the input neurons connect to */
                    UINT64 cur_output_neuron_id;
                    if (non_unrolled_inputs.size() > 0)
                    {
                        cur_output_neuron_id = *(non_unrolled_inputs.begin());
                    }
                    else
                    {
                        cur_output_neuron_id = cur_neuron_idx;
                    }
		
                    /* Set the IO mappings */
                    auto &io_mappings = clusters[cid]->getIOMappings();
                    for (auto input : packed_neurons)
                    {
                        if (auto mapping_iter = io_mappings.find(input);
                                mapping_iter != io_mappings.end())
                        {
                            (mapping_iter->second).push_back(cur_output_neuron_id);
                        }
                        else
                        {
                            std::vector<UINT64> outs = {cur_output_neuron_id};
                            io_mappings.insert({input, outs});
                        }
                    }

                    // std::cout << num_to_pack << ": ";
                    // for (auto input : packed_neurons) 
                    // { std::cout << input << "(" 
                    // << neuron_status[input].getParentId() << ") "; }
                    // std::cout << "-> " 
                    // << cur_output_neuron_id 
                    // << "(" 
                    // << neuron_status[cur_output_neuron_id].getParentId() << ") | ";
                }
            }
            // std::cout << "\n";
            // std::cout << "Mapped neuron id: " << snn[cur_neuron_idx].getNeuronId() << "\n";
            // debugPrint();
        }
    }
    connectedComponents();
    // postClustering();
    //debugPrint();
}

void Clusters::minComm(std::vector<Neuron>& snn)
{
    std::cout << "---------------------------------------\n";
    std::cout << "Clustering Step - Minimize Communication\n";
    std::cout << "Minimum fanin: " << MIN_FANIN << "\n";
    std::cout << "Crossbar size: " << CROSSBAR_SIZE << "\n";
    std::cout << "---------------------------------------\n";

    // Record the clustering status for each neuron
    neuron_status.resize(snn.size());
    for (auto &neuron : snn)
    {
        UINT64 id = neuron.getNeuronId();
        boost::multiprecision::cpp_int spikes = neuron.numOfSpikes();

        neuron_status[id].setSpikeTimes(&(neuron.getSpikeTimes()));
        neuron_status[id].setNumOfSpikes(spikes);
	
        neuron_status[id].setNeuronId(id);
        neuron_status[id].setNumOfSpikes(spikes);
        if (neuron.hasParent()) { neuron_status[id].setParentId(neuron.getParentId()); }

    }

    // Sorting neurons
    std::vector<Neuron_Status> sorted_neurons;
    for (auto &neuron : snn)
    {
        Neuron_Status tmp;

        UINT64 id = neuron.getNeuronId();
        boost::multiprecision::cpp_int spikes = neuron.numOfSpikes();

        tmp.setNeuronId(id);
        tmp.setNumOfSpikes(spikes);

        sorted_neurons.push_back(tmp);
    }

    std::sort(// std::execution::par,
              sorted_neurons.begin(),
              sorted_neurons.end(),
              [](auto &left, auto &right)
              {
                  return left.numOfSpikes() > right.numOfSpikes();
              });

    static std::default_random_engine e{};
    addCluster();

    for (auto &neuron : sorted_neurons)
    {
        auto cur_neuron_idx = neuron.getNeuronId();
        // std::cout << cur_neuron_idx << "\n";

        if (snn[cur_neuron_idx].hasParent()) { continue; }

        if (snn[cur_neuron_idx].numInputNeurons()) 
        {
            // std::cout << "\nMapping neuron id: " 
            // << snn[cur_neuron_idx].getNeuronId() << "\n";
            // All the input neurons before unrolling
            std::list<UINT64> non_unrolled_inputs;
            // Input neuron -> Output neuron mapping
            std::unordered_map<UINT64, UINT64> input_to_output_map;

            // Case 1: the neuron is unrolled
            if (auto &children = snn[cur_neuron_idx].getChildrenRef();
                    children.size() > 0)
            {
                std::set<UINT64> intermediate_neurons;
                for (auto &child : children) { intermediate_neurons.insert(child); }

                for (auto &inter_neuron : children)
                {
                    for (auto &raw_input : snn[inter_neuron].getInputNeuronList())
                    {
                        if (intermediate_neurons.find(raw_input) == 
                            intermediate_neurons.end())
                        {
                            non_unrolled_inputs.push_back(raw_input);
                            input_to_output_map.insert({raw_input, inter_neuron});
                        }
                    }
                }
                for (auto &raw_input : snn[cur_neuron_idx].getInputNeuronList())
                {
                    if (intermediate_neurons.find(raw_input) == intermediate_neurons.end())
                    {
                        non_unrolled_inputs.push_back(raw_input);
                        input_to_output_map.insert({raw_input, cur_neuron_idx});
                    }
		}
            }
            // Case 2: the neuron is not unrolled
            else
            {
                auto &inputs_to_pack = snn[cur_neuron_idx].getInputNeuronList();
                for (auto i = 0; i < inputs_to_pack.size(); i++)
                {
                    auto input_to_pack = inputs_to_pack[i];
                    if (i == CROSSBAR_SIZE) { break; }

                    non_unrolled_inputs.push_back(input_to_pack);
                    input_to_output_map.insert({input_to_pack,
                                                snn[cur_neuron_idx].getNeuronId()});
                }
            }

            // At this stage, you have all the neurons ready
            // std::cout << "\n";
            // for (auto &input : non_unrolled_inputs)
            // { std::cout << input << " "; } std::cout << " | ";

            // non_unrolled_inputs.reverse();
            // for (auto check : non_unrolled_inputs) { std::cout << check << " "; }
            // std::cout << std::endl;

            while (true)
            {
                if (non_unrolled_inputs.size() == 0) { break; }
                // Choose a random cluster
                std::uniform_int_distribution<> r_index_gen(0, clusters.size() - 1);
                auto r_index = r_index_gen(e);
                unsigned num_to_pack = numCanBePacked(r_index,
                                                      non_unrolled_inputs);

                /* Prepare a list of input neurons that 
                 * can be packed to an existing cluster */
                std::vector<UINT64> packed_neurons;
                auto cnt = 0;
                for (auto &input : non_unrolled_inputs)
                {
                    if (cnt == num_to_pack) break;
                    packed_neurons.push_back(input);
                    cnt++;
                }

                /* Pack into the existing cluster */
                bool packed = packToCluster(num_to_pack,
                                  snn[cur_neuron_idx].getNeuronId(),
                                  r_index,
                                  input_to_output_map,
                                  non_unrolled_inputs);

                /* Successfully packed? */
                if (packed)
                {
                    /* Get the output neuron id which the input neurons connect to */
                    UINT64 cur_output_neuron_id;
                    if (non_unrolled_inputs.size() > 0)
                    {
                        cur_output_neuron_id = *(non_unrolled_inputs.begin());
                    }
                    else
                    {
                        cur_output_neuron_id = cur_neuron_idx;
                    }

                    /* Set the input-output mappings */
                    for (auto input : packed_neurons)
                    {
                        if (auto mapping_iter = 
                            clusters[r_index]->getIOMappings().find(input);
                            mapping_iter != clusters[r_index]->getIOMappings().end())
                        {
                            (mapping_iter->second).push_back(cur_output_neuron_id);
                        }
                        else
                        {
                            std::vector<UINT64> outs = {cur_output_neuron_id};
                            clusters[r_index]->getIOMappings().insert({input, outs});
                        }
                    }
                    // std::cout << packed_neurons.size() << ": ";
                    // for (auto input : packed_neurons) 
                    // { std::cout << input 
                    // << "(" << neuron_status[input].getParentId() << ") "; }
                    // std::cout << "-> " 
                    // << cur_output_neuron_id 
                    // << "(" 
                    // << neuron_status[cur_output_neuron_id].getParentId() << ") | ";
                }

                /* Create a new cluster it cannot be fully mapped */
                if (non_unrolled_inputs.size() == 0) { break; }

                auto cid = addCluster();
                num_to_pack = numCanBePacked(cid,
                                             non_unrolled_inputs);
                packed_neurons.clear();

                /* Prepare input neurons that can be packed into the new cluster */
                cnt = 0;
                for (auto &input : non_unrolled_inputs)
                {
                    if (cnt == num_to_pack) break;
                    packed_neurons.push_back(input);
                    cnt++;
                }

                /* Pack into the new cluster*/
                packed = packToCluster(num_to_pack,
                             snn[cur_neuron_idx].getNeuronId(),
                             cid,
                             input_to_output_map,
                             non_unrolled_inputs);

                /* Successfully packed? */
                if (packed)
                {
                    UINT64 cur_output_neuron_id;
                    if (non_unrolled_inputs.size() > 0)
                    {
                        cur_output_neuron_id = *(non_unrolled_inputs.begin());
                    }
                    else
                    {
                        cur_output_neuron_id = cur_neuron_idx;
                    }
		
                    for (auto input : packed_neurons)
                    {
                        if (auto mapping_iter = 
                            clusters[cid]->getIOMappings().find(input);
                            mapping_iter != clusters[cid]->getIOMappings().end())
                        {
                            (mapping_iter->second).push_back(cur_output_neuron_id);
                        }
                        else
                        {
                            std::vector<UINT64> outs = {cur_output_neuron_id};
                            clusters[cid]->getIOMappings().insert({input, outs});
                        }
                    }

                    // for (auto input : packed_neurons) 
                    // { std::cout << input << "(" 
                    // << neuron_status[input].getParentId() << ") "; }
                    // std::cout << "-> " 
                    // << cur_output_neuron_id 
                    // << "(" 
                    // << neuron_status[cur_output_neuron_id].getParentId() << ") | ";
                }
            }
            // std::cout << "\n";
            // std::cout << "Mapped neuron id: " 
            // << snn[cur_neuron_idx].getNeuronId() << "\n";
            // debugPrint();
        }
    }
    connectedComponents();
    // debugPrint();
}

// The goal is to minimize the number of clusters
void Clusters::random(std::vector<Neuron>& snn)
{
    std::cout << "---------------------------------------\n";
    std::cout << "Clustering Step - Random\n";
    std::cout << "Minimum fanin: " << MIN_FANIN << "\n";
    std::cout << "Crossbar size: " << CROSSBAR_SIZE << "\n";
    std::cout << "---------------------------------------\n";

    // Record the clustering status for each neuron
    neuron_status.resize(snn.size());
    for (auto &neuron : snn)
    {
        UINT64 id = neuron.getNeuronId();
        boost::multiprecision::cpp_int spikes = neuron.numOfSpikes();

        neuron_status[id].setSpikeTimes(&(neuron.getSpikeTimes()));
        neuron_status[id].setNumOfSpikes(spikes);

        neuron_status[id].setNeuronId(id);
        neuron_status[id].setNumOfSpikes(spikes);
        if (neuron.hasParent()) 
        { neuron_status[id].setParentId(neuron.getParentId()); }
    }

    static std::default_random_engine e{};
    addCluster();
    for (auto cur_neuron_idx = 0;
              cur_neuron_idx < snn.size();
              cur_neuron_idx++)
    {
        if (snn[cur_neuron_idx].hasParent()) { break; }

        if (snn[cur_neuron_idx].numInputNeurons()) 
        {
            // std::cout << "\nMapping neuron id: " 
            // << snn[cur_neuron_idx].getNeuronId() << "\n";
            // All the input neurons before unrolling
            std::list<UINT64> non_unrolled_inputs;
            // Input neuron -> Output neuron mapping
            std::unordered_map<UINT64, UINT64> input_to_output_map;

            // Case 1: the neuron is unrolled
            if (auto &children = snn[cur_neuron_idx].getChildrenRef();
                    children.size() > 0)
            {
                std::set<UINT64> intermediate_neurons;
                for (auto &child : children) { intermediate_neurons.insert(child); }

                for (auto &inter_neuron : children)
                {
                    for (auto &raw_input : snn[inter_neuron].getInputNeuronList())
                    {
                        if (intermediate_neurons.find(raw_input) == 
                            intermediate_neurons.end())
                        {
                            non_unrolled_inputs.push_back(raw_input);
                            input_to_output_map.insert({raw_input, inter_neuron});
                        }
                    }
                }
                for (auto &raw_input : snn[cur_neuron_idx].getInputNeuronList())
                {
                    if (intermediate_neurons.find(raw_input) == 
                        intermediate_neurons.end())
                    {
                        non_unrolled_inputs.push_back(raw_input);
                        input_to_output_map.insert({raw_input, cur_neuron_idx});
                    }
		}
            }
            // Case 2: the neuron is not unrolled
            else
            {
                auto &inputs_to_pack = snn[cur_neuron_idx].getInputNeuronList();
                for (auto i = 0; i < inputs_to_pack.size(); i++)
                {
                    auto input_to_pack = inputs_to_pack[i];
                    if (i == CROSSBAR_SIZE) { break; }

                    non_unrolled_inputs.push_back(input_to_pack);
                    input_to_output_map.insert({input_to_pack,
                                                snn[cur_neuron_idx].getNeuronId()});
                }
            }

            // At this stage, you have all the neurons ready
            // std::cout << "\n";
            // for (auto &input : non_unrolled_inputs)
            // { std::cout << input << " "; } std::cout << " | ";

            while (true)
            {
                if (non_unrolled_inputs.size() == 0) { break; }
                // Choose a random cluster
                std::uniform_int_distribution<> r_index_gen(0, clusters.size() - 1);
                auto r_index = r_index_gen(e);
                unsigned num_to_pack = numCanBePacked(r_index,
                                                      non_unrolled_inputs);

                /* Prepare a list of input neurons that 
                 * can be packed into an existing cluster */
                std::vector<UINT64> packed_neurons;
                auto cnt = 0;
                for (auto &input : non_unrolled_inputs)
                {
                    if (cnt == num_to_pack) break;
                    packed_neurons.push_back(input);
                    cnt++;
                }

                /* Pack into the existing cluster */
                bool packed = packToCluster(num_to_pack,
                                  snn[cur_neuron_idx].getNeuronId(),
                                  r_index,
                                  input_to_output_map,
                                  non_unrolled_inputs);

                /* Successfully packed? */
                if (packed)
                {
                    UINT64 cur_output_neuron_id;
                    if (non_unrolled_inputs.size() > 0)
                    {
                        cur_output_neuron_id = *(non_unrolled_inputs.begin());
                    }
                    else
                    {
                        cur_output_neuron_id = cur_neuron_idx;
                    }
                    for (auto input : packed_neurons)
                    {
                        if (auto mapping_iter = 
                            clusters[r_index]->getIOMappings().find(input);
                            mapping_iter != clusters[r_index]->getIOMappings().end())
                        {
                            (mapping_iter->second).push_back(cur_output_neuron_id);
                        }
                        else
                        {
                            std::vector<UINT64> outs = {cur_output_neuron_id};
                            clusters[r_index]->getIOMappings().insert({input, outs});
                        }
                    }
                    // std::cout << packed_neurons.size() << ": ";
                    // for (auto input : packed_neurons) 
                    // { std::cout << input 
                    // << "(" << neuron_status[input].getParentId() << ") "; }
                    // std::cout << "-> " 
                    // << cur_output_neuron_id 
                    // << "(" 
                    // << neuron_status[cur_output_neuron_id].getParentId() << ") | ";
                }
                // Create a new cluster it cannot be fully mapped		
                if (non_unrolled_inputs.size() == 0) { break; }

                auto cid = addCluster();
                num_to_pack = numCanBePacked(cid,
                                             non_unrolled_inputs);

                /* Prepare a list of input neurons that 
                 * can be packed into the new cluster */
                packed_neurons.clear();
                cnt = 0;
                for (auto &input : non_unrolled_inputs)
                {
                    if (cnt == num_to_pack) break;
                    packed_neurons.push_back(input);
                    cnt++;
                }

                /* Pack into the new cluster */
                packed = packToCluster(num_to_pack,
                                       snn[cur_neuron_idx].getNeuronId(),
                                       cid,
                                       input_to_output_map,
                                       non_unrolled_inputs);
                /* Successfully packed into the new cluster */
                if (packed)
                {
                    UINT64 cur_output_neuron_id;
                    if (non_unrolled_inputs.size() > 0)
                    {
                        cur_output_neuron_id = *(non_unrolled_inputs.begin());
                    }
                    else
                    {
                        cur_output_neuron_id = cur_neuron_idx;
                    }
		
                    for (auto input : packed_neurons)
                    {
                        if (auto mapping_iter = 
                            clusters[cid]->getIOMappings().find(input);
                            mapping_iter != clusters[cid]->getIOMappings().end())
                        {
                            (mapping_iter->second).push_back(cur_output_neuron_id);
                        }
                        else
                        {
                            std::vector<UINT64> outs = {cur_output_neuron_id};
                            clusters[cid]->getIOMappings().insert({input, outs});
                        }
                    }

                    // for (auto input : packed_neurons) 
                    // { std::cout << input << "(" 
                    // << neuron_status[input].getParentId() << ") "; }
                    // std::cout << "-> " 
                    // << cur_output_neuron_id 
                    // << "(" 
                    // << neuron_status[cur_output_neuron_id].getParentId() << ") | ";
                }
            }
            // std::cout << "\n\n";
            // std::cout << "Mapped neuron id: " 
            // << snn[cur_neuron_idx].getNeuronId() << "\n";
            // debugPrint();
        }
    }
    connectedComponents();
    // debugPrint();
}

unsigned Clusters::numCanBePacked(UINT64 cid,
                                  std::list<UINT64> &non_unrolled_inputs)
{
    unsigned total_inputs_can_be_packed = 0;
    unsigned new_inputs_to_be_packed = 0;
    for (auto &pending_input : non_unrolled_inputs)
    {
        if (!clusters[cid]->isInputMapped(pending_input))
        {
            new_inputs_to_be_packed++;
        }

        if (clusters[cid]->canBePacked(new_inputs_to_be_packed))
        {
            total_inputs_can_be_packed++;
        }
        else
        {
            break;
        }
    }

    return total_inputs_can_be_packed;
}

bool Clusters::packToCluster(unsigned total_inputs_can_be_packed,
                             UINT64 cur_neuron_idx, 
                             UINT64 cid,
                             std::unordered_map<UINT64, UINT64> &input_to_output_map,
                             std::list<UINT64> &non_unrolled_inputs)
{
    // It must be able to provide MIN_FANIN number of input ports
    if (total_inputs_can_be_packed >= MIN_FANIN && 
        non_unrolled_inputs.size() >= MIN_FANIN ||
        non_unrolled_inputs.size() < MIN_FANIN &&
        total_inputs_can_be_packed == non_unrolled_inputs.size())
    {
        UINT64 last_input_packed = INVALID_ID;
        unsigned cur_packed = 0;
        for (auto &input_to_pack : non_unrolled_inputs)
        {
            if (cur_packed == total_inputs_can_be_packed) { break; }

            clusters[cid]->addInput(input_to_pack);
            neuron_status[input_to_pack].
                addConnectedCluster(clusters[cid]->getClusterId());

            clusters[cid]->addSynapse();
            last_input_packed = input_to_pack;
            cur_packed++;
	}

        auto inter_neuron_iter = input_to_output_map.find(last_input_packed);
        assert(inter_neuron_iter != input_to_output_map.end());
        UINT64 inter_neuron_id = inter_neuron_iter->second;

        clusters[cid]->addOutput(inter_neuron_id);

        auto begin_to_remove = non_unrolled_inputs.begin();
        auto end_to_remove = std::next(non_unrolled_inputs.begin(), 
                                       total_inputs_can_be_packed);
        non_unrolled_inputs.erase(begin_to_remove, end_to_remove);

        if (inter_neuron_id != cur_neuron_idx)
        {
            non_unrolled_inputs.push_front(inter_neuron_id);
        }
        assert(clusters[cid]->getInputsListRef().size() <= CROSSBAR_SIZE);
        assert(clusters[cid]->getOutputsListRef().size() <= CROSSBAR_SIZE);

        return true;
    }
    return false;
}
}
}
