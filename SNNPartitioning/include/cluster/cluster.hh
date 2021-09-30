#ifndef __CLUSTERING_HH__
#define __CLUSTERING_HH__

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <list>
#include <memory>
// #include <mutex>
#include <set>
// #include <shared_mutex>
#include <stack>
#include <unordered_map>
#include <vector>
#include <map>
// #include <boost/functional/hash.hpp>

#include "unroll/unroll.hh"

// Performance consideration:
// std::set should be enough since the number of elements is not significant
// However, keep this in mind when we are testing big networks in the future.
// Also, the std::unordered_set takes much space, this should also be considered

namespace EXT
{
namespace Clustering
{
typedef uint64_t UINT64;

const UINT64 INVALID_ID = (UINT64) - 1;

class Neuron_Status
{
  protected:
    UINT64 parent_id;

    boost::multiprecision::cpp_int num_spikes;

    std::set<UINT64> connected_clusters;
    std::set<UINT64> *spike_times;

  protected:
    UINT64 id;

  public:
    Neuron_Status() {}

    void setNeuronId(UINT64 _id) { id = _id; parent_id = _id; }
    UINT64 getNeuronId() { return id; }

    void setNumOfSpikes(boost::multiprecision::cpp_int _spikes) { num_spikes = _spikes; }
    boost::multiprecision::cpp_int numOfSpikes() { return num_spikes; }

    std::set<UINT64> &getConnectedClustersRef() { return connected_clusters; }
    std::set<UINT64> getConnectedClustersCopy() { return connected_clusters; }
    void addConnectedCluster(UINT64 _cluster) { connected_clusters.insert(_cluster); }

    auto getParentId() { return parent_id; }
    void setParentId(UINT64 _id) { parent_id = _id; }

    void setSpikeTimes(std::set<UINT64> *_spike_times)
    {
        spike_times = _spike_times;
    }

    auto &getSpikeTimes() { return *spike_times; }
};

class Cluster
{
  protected:
    const unsigned MIN_FANIN; 
    const unsigned CROSSBAR_SIZE;

  protected:
    UINT64 cluster_id = INVALID_ID;

    std::set<UINT64> inputs;
    std::set<UINT64> outputs;

    std::set<UINT64> connected_clusters_out;
    std::set<UINT64> connected_clusters_in;

    boost::multiprecision::cpp_int in_coming_spikes = 0;
    std::unordered_map<UINT64, boost::multiprecision::cpp_int> cluster_spikes_out_map;

    unsigned num_synapses = 0;

    // Record input->output mapping
    std::unordered_map<UINT64, std::vector<UINT64>> input_output_mappings;

  public:
    Cluster(UINT64 _id, unsigned _fanin, unsigned _crossbar_size)
        : MIN_FANIN(_fanin)
        , CROSSBAR_SIZE(_crossbar_size)
        , cluster_id(_id) {}

    UINT64 getClusterId() { return cluster_id; }

    void addInput(UINT64 _input) { inputs.insert(_input); }
    void addOutput(UINT64 _output) { outputs.insert(_output); }

    std::set<UINT64> &getInputsListRef() { return inputs; }
    std::set<UINT64> getInputsListCopy() { return inputs; }

    std::set<UINT64> &getOutputsListRef() { return outputs; }
    std::set<UINT64> getOutputsListCopy() { return outputs; }

    std::set<UINT64> &getConnectedClustersOutRef() { return connected_clusters_out; }
    std::set<UINT64> getConnectedClustersOutCopy() { return connected_clusters_out; }
    void addConnectedClusterOut(UINT64 _cluster) { connected_clusters_out.insert(_cluster); }

    std::set<UINT64> &getConnectedClustersInRef() { return connected_clusters_in; }
    std::set<UINT64> getConnectedClustersInCopy() { return connected_clusters_in; }
    void addConnectedClusterIn(UINT64 _cluster) { connected_clusters_in.insert(_cluster); }

    void addSynapse() { num_synapses++; }
    unsigned numSynapses() { return num_synapses; }

    void addNumSpikesIn(boost::multiprecision::cpp_int _spikes)
    {
        boost::multiprecision::cpp_int ori = in_coming_spikes;
        in_coming_spikes += _spikes;
        if (in_coming_spikes < ori)
        {
            std::cerr << "addNumSpikesIn: overflow detected." << std::endl;
            exit(0);
        }
    }

    void addNumSpikesOut(UINT64 cid, boost::multiprecision::cpp_int _spikes)
    {
        if (auto iter = cluster_spikes_out_map.find(cid);
                 iter != cluster_spikes_out_map.end())
        {
            boost::multiprecision::cpp_int ori = iter->second;
            iter->second += _spikes;
            if (iter->second < ori)
            {
                std::cerr << "addNumSpikesOut: overflow detected." << std::endl;
                exit(0);
            }
        }
        else
        {
            cluster_spikes_out_map.insert({cid, _spikes});
        }
    }

    boost::multiprecision::cpp_int numOfSpikesIn()
    {
        return in_coming_spikes;
    }

    boost::multiprecision::cpp_int numOfSpikesOut(UINT64 cid)
    {
        auto iter = cluster_spikes_out_map.find(cid);
        assert(iter != cluster_spikes_out_map.end());
        return iter->second;
    }

    bool isInputMapped(UINT64 _input)
    {
        return (inputs.find(_input) != inputs.end());
    }

    bool canBePacked(unsigned num_inputs)
    {
        return (((inputs.size() + num_inputs) <= CROSSBAR_SIZE) && 
                 (outputs.size() < CROSSBAR_SIZE));
    }

    unsigned getUtilization() { return (inputs.size() + outputs.size()); }
    unsigned numAvailInputPorts() { return (CROSSBAR_SIZE - inputs.size()); }

    // get IO mapping
    auto& getIOMappings() { return input_output_mappings; }
};
/*
template < typename SEQUENCE > struct SeqHash
{
    std::size_t operator() ( const SEQUENCE& seq ) const
    {
        std::size_t hash = 0 ;
        boost::hash_range( hash, seq.begin(), seq.end() ) ;
        return hash ;
    }
};
template < typename SEQUENCE, typename T >
using sequence_to_data_map = std::unordered_map< SEQUENCE, T, SeqHash<SEQUENCE> >;
*/
typedef EXT::Unrolling::Neuron Neuron;
class Clusters
{
  protected:
    const unsigned INVALID_FANIN = (unsigned) - 1;

    unsigned MIN_FANIN; 
    const unsigned CROSSBAR_SIZE;

  protected:

    std::vector<std::unique_ptr<Cluster>> clusters;
    std::vector<Cluster*> sorted_clusters;

  public:
    Clusters(unsigned _fanin, unsigned _crossbar_size)
    : CROSSBAR_SIZE(_crossbar_size)
    {
        if (_fanin == INVALID_FANIN) { MIN_FANIN = CROSSBAR_SIZE; }
        else { MIN_FANIN = _fanin; }
        assert(MIN_FANIN <= CROSSBAR_SIZE); // When the unrolling fanin larger than crossbar,
                                            // it is not supported.
    }

    void clustering(std::vector<Neuron> &snn, std::string &mode)
    {
        auto t1 = std::chrono::high_resolution_clock::now();
        if (mode == "min-clusters")
        {
            minClusters(snn);
        }
        else if (mode == "random")
        {
            random(snn);
        }
        else if (mode == "min-comm")
        {
            minComm(snn);
        }
        else
        {
            std::cerr << "---------------------------------------\n";
            std::cerr << "Error: unsupported clustering algorithm\n";
            return;
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
        std::cout << "Compilation time: " << duration << " microseconds\n";
    }

    void printClusterIR(std::string &_out)
    {
        std::fstream file;
        file.open(_out, std::fstream::out);

        for (auto &cluster : clusters)
        {
            UINT64 cid = cluster->getClusterId();

            for (auto &conn_cluster : cluster->getConnectedClustersOutRef())
            {
                file << cid << " "
                     << conn_cluster << " "
                     << cluster->numOfSpikesOut(conn_cluster) << "\n";
            }
        }
        file.close();
    }
    void printClusterStats(std::string &_out)
    {
        std::fstream file;
        file.open(_out, std::fstream::out);

        for (auto &cluster : clusters)
        {
            UINT64 cid = cluster->getClusterId();

            auto outputs = cluster->getOutputsListRef();
            std::map<UINT64, std::vector<UINT64>> cid_to_neu_id;

            for (auto &conn_cluster : cluster->getConnectedClustersOutRef())
            {
                for (auto output : outputs)
                {
                    auto &conn_clusters = neuron_status[output].getConnectedClustersRef();
                    if (auto iter = conn_clusters.find(conn_cluster);
                            iter != conn_clusters.end())
                    {
                        if (auto map_iter = cid_to_neu_id.find(conn_cluster);
                                map_iter != cid_to_neu_id.end())
                        {
                            map_iter->second.push_back(output);
                        }
                        else
                        {
                            std::vector<UINT64> tmp = {output};
                            cid_to_neu_id.insert({conn_cluster, tmp});
                        }
                    }
                }
            }

            for (auto [conn_cluster, outputs] : cid_to_neu_id)
            {
                file << cluster->getClusterId() << " "
                     << conn_cluster << " ";
                std::set<UINT64> spike_times;
                // std::vector<UINT64> spike_times_all;
                for (auto output : outputs)
                {
                    for (auto spike_time : 
                             neuron_status[output].getSpikeTimes())
                    {
                        // file << spike_time << " ";
                        spike_times.insert(spike_time);
                        // spike_times_all.push_back(spike_time);
                    }
                }
                
                // std::sort(spike_times_all.begin(), spike_times_all.end(),
                //           [](auto &left, auto&right) {return left < right; });
                for (auto spike_time : spike_times)
                {
                    file << spike_time << " ";
                }
                
                file << "\n";
            }

            /*
            auto &io_mappings = cluster->getIOMappings();
            auto &inputs = cluster->getInputsListRef();
            for (auto input : inputs)
            {
                auto output_iter = io_mappings.find(input);
                // std::cout << input << "\n";
                assert(output_iter != io_mappings.end());
                auto &outputs = output_iter->second;
                for (auto output : outputs)
                {
                    file << cluster->getClusterId() << " "
                         << input << " "
                         << output << " "
                         << neuron_status[input].numOfSpikes() << " "
                         << neuron_status[input].getParentId() << " "
                         << neuron_status[output].getParentId() << "\n";
                }
            }

            UINT64 cid = cluster->getClusterId();

            unsigned num_inputs = cluster->getInputsListRef().size();
            unsigned num_outputs = cluster->getOutputsListRef().size();
            unsigned num_synapses = cluster->numSynapses();
            unsigned num_conns = 0;
            boost::multiprecision::cpp_int total_spikes_in = cluster->numOfSpikesIn();
            boost::multiprecision::cpp_int total_spikes_out = 0;

            for (auto &conn_cluster : cluster->getConnectedClustersOutRef())
            {
                boost::multiprecision::cpp_int ori = total_spikes_out;
                num_conns++;
                total_spikes_out += cluster->numOfSpikesOut(conn_cluster);
                if (total_spikes_out < ori)
                {
                    std::cerr << "printClusterStats: overflow detected." << std::endl;
                    exit(0);
                }
            }

            file << cid << " " 
                 << num_inputs << " " 
                 << num_outputs << " " 
                 << num_synapses << " "
                 << num_conns << " "
                 << total_spikes_in << " "
                 << total_spikes_out << "\n";
            */
        }

        file.close();
    }

  protected: // Helper function
    void minClusters(std::vector<Neuron>&);
    void random(std::vector<Neuron>&);
    void minComm(std::vector<Neuron>&);

    UINT64 addCluster()
    {
        UINT64 cid = clusters.size();

        std::unique_ptr<Cluster> cluster = 
            std::make_unique<Cluster>(cid, MIN_FANIN, CROSSBAR_SIZE);

        clusters.push_back(std::move(cluster));

        sorted_clusters.push_back(clusters[cid].get());

        return cid;
    }

    std::vector<Neuron_Status> neuron_status;

    unsigned numCanBePacked(UINT64 cid,
                            std::list<UINT64> &non_unrolled_inputs);

    bool packToCluster(unsigned total_inputs_can_be_packed,
                       UINT64 cur_neuron_idx,
                       UINT64 cid,
                       std::unordered_map<UINT64, UINT64> &input_to_output_map,
                       std::list<UINT64> &non_unrolled_inputs);

  protected:
    void postClustering()
    {
        for (auto &cluster : clusters)
        {
            for (auto &output : cluster->getOutputsListRef())
            {
                for (auto &conn_cluster : neuron_status[output].getConnectedClustersRef())
                {
                    cluster->addConnectedClusterOut(conn_cluster);
                    clusters[conn_cluster]->addConnectedClusterIn(cluster->getClusterId());
                }
            }
        }
    }

    // The following codes should better be standalone.
    void connectedComponents()
    {
        postClustering();

        std::cout << "---------------------------------------\n";
        std::vector<bool> visited(clusters.size(), false);

        std::vector<std::vector<UINT64>> cc;
        for (auto cid = 0; cid < clusters.size(); cid++)
        {
            // std::vector<UINT64> neighbors;
            // for (auto id : clusters[cid]->getConnectedClustersOutRef()) 
            // {neighbors.push_back(id);}
            // for (auto id : clusters[cid]->getConnectedClustersInRef()) 
            // {neighbors.push_back(id);}

            if (visited[cid] == false)
            // if ((visited[cid] == false) && (neighbors.size() > 0))
            {
                iterativeDFS(cc, cid, visited);
            }
        }

        if (cc.size() > 1)
        {
            std::cout << "Number of disconnect graphs: " << cc.size() << "\n";

            // Connect all the disconnected graph
            std::list<UINT64> neurons_to_connect;
            for (auto &c : cc)
            {
                for (auto ele : c)
                {
                    auto &conn_clusters_out = 
                        clusters[ele]->getConnectedClustersOutRef();
                    if (conn_clusters_out.size() == 0)
                    {
                        auto &outputs = clusters[ele]->getOutputsListRef();
                        assert(outputs.size());
                        neurons_to_connect.push_back(*(outputs.begin()));
                        // std::cout << ele << "\n";
                        break;
                    }
                }
            }

            auto new_cid = INVALID_ID;
            unsigned num_new_neurons = 0;
            while (true)
            // for (auto neuron_to_connect : neurons_to_connect)
            {
                if (neurons_to_connect.size() == 0) { break; }

                // TODO, this is very dangerous, change it please.
                if (num_new_neurons % CROSSBAR_SIZE == 0)
                {
                    if (num_new_neurons > 0)
                    {
                        // Connect previous cid
                        auto &outputs = clusters[new_cid]->getOutputsListRef();
                        assert(outputs.size());
                        neurons_to_connect.push_front(*(outputs.begin()));
                    }

                    new_cid = addCluster();
                }

                auto neuron_to_connect = *(neurons_to_connect.begin());
                num_new_neurons++;

                UINT64 new_neuron_id = neuron_status.size();
                neuron_status.push_back(Neuron_Status());
                neuron_status[new_neuron_id].
                    setNumOfSpikes(neuron_status[neuron_to_connect].numOfSpikes());
                neuron_status[new_neuron_id].setNeuronId(new_neuron_id);

                // clusters[new_cid]->addSynapse();
                clusters[new_cid]->addInput(neuron_to_connect);
                neuron_status[neuron_to_connect].
                    addConnectedCluster(clusters[new_cid]->getClusterId());
                clusters[new_cid]->addOutput(new_neuron_id);

                if (auto mapping_iter = 
                    clusters[new_cid]->getIOMappings().find(neuron_to_connect);
                    mapping_iter != clusters[new_cid]->getIOMappings().end())
                {
                    (mapping_iter->second).push_back(neuron_to_connect);
                }
                else
                {
                    std::vector<UINT64> outs = {neuron_to_connect};
                    clusters[new_cid]->getIOMappings().insert({neuron_to_connect, 
                                                               outs});
                }


                neurons_to_connect.pop_front();
            }

            postClustering();
        }

        // Calculate inter-cluster spikes
        for (auto &cluster : clusters)
        {
            for (auto &input : cluster->getInputsListRef())
            {
                cluster->addNumSpikesIn(neuron_status[input].numOfSpikes());
            }

            for (auto &output : cluster->getOutputsListRef())
            {
                for (auto &conn_cluster : 
                    neuron_status[output].getConnectedClustersRef())
                {
                    cluster->addNumSpikesOut(conn_cluster, 
                        neuron_status[output].numOfSpikes());
                    // clusters[conn_cluster]->addNumSpikesIn(
                    // neuron_status[output].numOfSpikes());
                }
            }
        }
    }

    // Better to iterative here, recursive can crash when the graph is large,
    void iterativeDFS(std::vector<std::vector<UINT64>> &cc,
                      UINT64 cid,
                      std::vector<bool> &visited)
    {
        std::stack<UINT64> s;

        s.push(cid);

        std::vector<UINT64> c;
        while (true)
        {
            if (s.size() == 0) { break; }
            
            auto v = s.top();
            s.pop();
            if (visited[v]) { continue; }

            visited[v] = true;
            c.push_back(v);

            std::vector<UINT64> neighbors;
            for (auto id : clusters[v]->getConnectedClustersOutRef()) 
            {neighbors.push_back(id);}
            for (auto id : clusters[v]->getConnectedClustersInRef()) 
            {neighbors.push_back(id);}

            for (auto neighbor : neighbors)
            {
                if (!visited[neighbor]) { s.push(neighbor); }
            }
        }

	cc.push_back(c);
    }

    void debugPrint()
    {
        std::cout << "---------------------------------------\n";
        for (auto &cluster : clusters)
        {
            std::cout << "Cluster ID: " << cluster->getClusterId() << "\n";
            std::cout << "Input Neurons: ";
            for (auto &input : cluster->getInputsListRef()) 
            { std::cout << input << " "; }
            std::cout << "\nOutput Neurons: ";
            for (auto &output : cluster->getOutputsListRef()) 
            { std::cout << output << " "; }
            std::cout << "\nNumber of synapses: " << cluster->numSynapses();
            std::cout << "\nConnected Clusters: ";
            for (auto &conn_cluster : cluster->getConnectedClustersOutRef())
            {
                std::cout << conn_cluster << "(";
                std::cout << cluster->numOfSpikesOut(conn_cluster) << "), ";
            }
            std::cout << "\n\n";
        }

        /*
        for (auto i = 0; i < neuron_status.size(); i++)
        {
            std::cout << "Neuron ID: " << i << "; Clusters: ";
            for (auto &cluster : neuron_status[i].getConnectedClustersRef())
            {
                std::cout << cluster << " ";
            }
            std::cout << "\n";
        }
        */
    }
};
}
}
#endif
