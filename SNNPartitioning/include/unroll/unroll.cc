#include <algorithm>
#include <boost/tokenizer.hpp>
#include <cmath>

#include "unroll/unroll.hh"

#include "cluster/cluster.hh"

namespace EXT
{
namespace Unrolling
{
typedef EXT::Clustering::Clusters Clusters;

Model::Model(const std::string& connection_file_name,
             const std::string& spike_file)
{
    UINT64 max_neuron_id = extractMaxNeuronId(connection_file_name);
    // std::cout << "Max neuron ID: " << max_neuron_id << "\n";

    // Initialize neurons with IDs 
    for (auto i = 0; i <= max_neuron_id; i++)
    {
        snn.emplace_back(i);
    }

    readSpikes(spike_file);

    readConnections(connection_file_name);

    // Initialize all the spike information
    // for (auto &neuron : snn)
    // for (auto i = 0; i < snn.size(); i++)
    // {
    //     auto &outputs = snn[i].getOutputNeuronList();
    //     for (auto &output : outputs) 
    //     {
    //         snn[output].addNumSpikesFromOneInput(snn[i].numOfSpikes());
    //     }
    // }
}

Model::~Model()
{
    Clusters* real_clusters = static_cast<Clusters*>(clusters);
    delete real_clusters;
}

UINT64 Model::extractMaxNeuronId(const std::string &file_name)
{
    std::fstream file;
    file.open(file_name, std::ios::in);

    UINT64 max_id = 0;
    std::string line;
    typedef boost::tokenizer<boost::char_separator<char>> tok_t;
    boost::char_separator<char> sep(" ", "", boost::keep_empty_tokens);
    while (std::getline(file, line))
    {
        tok_t tok(line, sep);
        for (tok_t::iterator i = tok.begin(); i != tok.end(); ++i)
        {
            if (*i == "")
            {
                continue;
            }
            UINT64 nid = std::stoull(*i);
            if (nid > max_id)
            {
                max_id = nid;
            }
        }
    }
    file.close();
    return max_id;
}

void Model::readSpikes(const std::string& spike_file)
{
    std::fstream file;
    file.open(spike_file, std::ios::in);

    std::string line;
    typedef boost::tokenizer<boost::char_separator<char>> tok_t;
    boost::char_separator<char> sep(" ", "", boost::keep_empty_tokens);

    while (std::getline(file, line))
    {
        UINT64 source_neuron;
        tok_t tok(line, sep);
        std::set<UINT64> spike_times;
        bool first = true;
        for (tok_t::iterator i = tok.begin(); i != tok.end(); ++i)
        {
            if (*i == "")
            {
                continue;
            }
            if (first)
            {
                source_neuron = std::stoull(*i);
                first = false;
                continue;
            }

            UINT64 spike = std::stoull(*i);
            spike_times.insert(spike);
        }

        // assert(source_neuron < snn.size());
        if (source_neuron < snn.size())
        {
            snn[source_neuron].setNumSpikes(spike_times.size());
            snn[source_neuron].setSpikeTimes(spike_times);
        }
    }
    file.close();
}

void Model::readConnections(const std::string &connection_file_name)
{
    std::fstream file;
    file.open(connection_file_name, std::ios::in);

    std::string line;
    typedef boost::tokenizer<boost::char_separator<char>> tok_t;
    boost::char_separator<char> sep(" ", "", boost::keep_empty_tokens);

    while (std::getline(file, line))
    {
        UINT64 source_neuron;
        tok_t tok(line, sep);
        std::set<UINT64> out_neuron_list;
        bool first = true;
        for (tok_t::iterator i = tok.begin(); i != tok.end(); ++i)
        {
            if (*i == "")
            {
                continue;
            }
            if (first)
            {
                source_neuron = std::stoull(*i);
                first = false;
                continue;
            }

            UINT64 out_neuron = std::stoull(*i);
            out_neuron_list.insert(out_neuron);
        }

        for (auto out : out_neuron_list)
        {
            snn[source_neuron].addOutputNeuron(out);
            snn[out].addInputNeuron(source_neuron);
        }
    }
    file.close();
}

// A simple proof-of-concept version of unroll
void Model::unroll()
{
    std::cout << "---------------------------------------\n";
    std::cout << "Unrolling Step\n";
    std::cout << "Maximum fanin: " << max_fanin << "\n";
    std::cout << "---------------------------------------\n";

    // Initialize the unrolled neurons
    for (auto i = 0; i < snn.size(); i++)
    {
        // emplace_back should be more space-efficient than push_back
        usnn.emplace_back(snn[i]);
    }
    
    // Look for all the neurons that have more than max_fanin number of inputs
    for (auto idx = 0; idx < snn.size(); idx++)
    {
        UINT64 prev_unrolling_neuron_id = usnn.size();
        UINT64 cur_unrolling_neuron_id = usnn.size();

        auto input_neurons_copy = usnn[idx].getInputNeuronListCopy();

        // Check if the number of inputs exceed max_fanin
        if (input_neurons_copy.size() > max_fanin)
        {
            // std::cout << "\nUnrolling neuron id: "
            //           << usnn[idx].getNeuronId() << "\n";

            // Step one, reset the output neuron of all the input neurons
            for (auto &input_neuron : input_neurons_copy)
            {
                auto &out = usnn[input_neuron].getOutputNeuronList();
                out.erase(std::remove(out.begin(), out.end(), usnn[idx].getNeuronId()));
            }

            // Step two, reset the input neurons of the currently processing neuron
            usnn[idx].getInputNeuronList().clear();
            usnn[idx].getInputNeuronList().shrink_to_fit();
            // usnn[idx].resetNumSpikes();

            // Step three, unrolling
            // The number of intermediate neurons to unroll the current neuron
            //     = ceil((#inputs - #fanin) / (#fanin - 1)) + 1
            unsigned num_inputs = input_neurons_copy.size();
            unsigned num_inter_neurons = std::ceil(((float)num_inputs - (float)max_fanin) / 
                                                   ((float)max_fanin - 1)) + 1;

            // std::vector<UINT64> new_neurons;
            for (auto inter_neu_idx = 0; inter_neu_idx < num_inter_neurons; inter_neu_idx++)
            {
                if (inter_neu_idx == 0)
                {
                    usnn.emplace_back(cur_unrolling_neuron_id);

                    boost::multiprecision::cpp_int total_spikes = 0;
                    std::set<UINT64> spike_times;

                    for (auto i = 0; i < max_fanin; i++)
                    {
                        boost::multiprecision::cpp_int ori = total_spikes;

                        usnn[input_neurons_copy[i]].getOutputNeuronList().push_back(
                            cur_unrolling_neuron_id);
                        total_spikes += usnn[input_neurons_copy[i]].numOfSpikes();
                        auto &input_spikes = usnn[input_neurons_copy[i]].getSpikeTimes();
                        for (auto spike : input_spikes) { spike_times.insert(spike); }
                        if (total_spikes < ori)
                        {
                            std::cerr << "unroll: overflow detected." << std::endl;
                            exit(0);
                        }

                        usnn[cur_unrolling_neuron_id].getInputNeuronList().push_back(
                            input_neurons_copy[i]);
                    }

                    usnn[cur_unrolling_neuron_id].setNumSpikes(total_spikes);
                    usnn[cur_unrolling_neuron_id].setSpikeTimes(spike_times);
                    usnn[cur_unrolling_neuron_id].setParentId(usnn[idx].getNeuronId());
                    usnn[idx].addChild(cur_unrolling_neuron_id);
                    cur_unrolling_neuron_id++;
                }
                else if (inter_neu_idx == num_inter_neurons - 1)
                {
                    usnn[prev_unrolling_neuron_id].getOutputNeuronList().push_back(
                        usnn[idx].getNeuronId());
                    usnn[usnn[idx].getNeuronId()].getInputNeuronList().push_back(
                        prev_unrolling_neuron_id);

                    // boost::multiprecision::cpp_int total_spikes = 
                    //     usnn[prev_unrolling_neuron_id].numOfSpikes();

                    for (auto i = max_fanin + (inter_neu_idx - 1) * (max_fanin - 1);
                              i < num_inputs;
                              i++)
                    {
                        // boost::multiprecision::cpp_int ori = total_spikes;

                        usnn[input_neurons_copy[i]].getOutputNeuronList().push_back(
                            usnn[idx].getNeuronId());
                        // total_spikes += usnn[input_neurons_copy[i]].numOfSpikes();
                        // if (total_spikes < ori)
                        // {
                            // std::cerr << "unroll: overflow detected." << std::endl;
                            // exit(0);
                        // }

                        usnn[usnn[idx].getNeuronId()].getInputNeuronList().push_back(
                            input_neurons_copy[i]);
                    }
                    // usnn[usnn[idx].getNeuronId()].setNumSpikes(total_spikes);
                }
                else
                {
                    usnn.emplace_back(cur_unrolling_neuron_id);

                    usnn[prev_unrolling_neuron_id].getOutputNeuronList().push_back(
                        cur_unrolling_neuron_id);
                    usnn[cur_unrolling_neuron_id].getInputNeuronList().push_back(
                        prev_unrolling_neuron_id);

                    boost::multiprecision::cpp_int total_spikes = 
                        usnn[prev_unrolling_neuron_id].numOfSpikes();

                    std::set<UINT64> spike_times;
                    auto &input_spikes = usnn[prev_unrolling_neuron_id].getSpikeTimes();
                    for (auto spike : input_spikes) { spike_times.insert(spike); }
                        
                    
                    for (auto i = max_fanin + (inter_neu_idx - 1) * (max_fanin - 1); 
                              i < max_fanin + inter_neu_idx * (max_fanin - 1);
                              i++)
                    {
                        boost::multiprecision::cpp_int ori = total_spikes;

                        usnn[input_neurons_copy[i]].getOutputNeuronList().push_back(
                            cur_unrolling_neuron_id);
                        total_spikes += usnn[input_neurons_copy[i]].numOfSpikes();
                        auto &input_spikes = usnn[input_neurons_copy[i]].getSpikeTimes();
                        for (auto spike : input_spikes) { spike_times.insert(spike); }
                        
                        if (total_spikes < ori)
                        {
                            std::cerr << "unroll: overflow detected." << std::endl;
                            exit(0);
                        }

                        usnn[cur_unrolling_neuron_id].getInputNeuronList().push_back(
                            input_neurons_copy[i]);
                    }

                    usnn[cur_unrolling_neuron_id].setNumSpikes(total_spikes);
                    usnn[cur_unrolling_neuron_id].setSpikeTimes(spike_times);
                    usnn[cur_unrolling_neuron_id].setParentId(usnn[idx].getNeuronId());
                    usnn[idx].addChild(cur_unrolling_neuron_id);

                    prev_unrolling_neuron_id = cur_unrolling_neuron_id;
                    cur_unrolling_neuron_id++;
                }
            }
        }
    }
    // for (auto &neuron : usnn) { neuron.print_connections(); } exit(0);
}

void Model::outputUnrolledIR(const std::string &out_name)
{
    if (usnn.size() == 0) { return; }

    std::fstream file;
    file.open(out_name, std::fstream::out);

    for (auto &neuron : usnn)
    {
        assert(neuron.getInputNeuronList().size() <= max_fanin);
        auto &output_neurons = neuron.getOutputNeuronList();
        for (auto &output : output_neurons)
        { 
            file << neuron.getNeuronId() << " ";
            file << output << " ";
            file << neuron.numOfSpikes();
            file << "\n";
        }

        // if (output_neurons.size() == 0) { std::cout << neuron.getNeuronId() << std::endl; }
    }

    file.close();
    return;
}

void Model::clustering(std::string &mode, unsigned crossbar_size)
{
    clusters = new Clusters(max_fanin, crossbar_size);
    Clusters* real_clusters = static_cast<Clusters*>(clusters);
    if (usnn.size())
    {
        real_clusters->clustering(usnn, mode);
    }
    else
    {
        real_clusters->clustering(snn, mode);
    }
}

void Model::printClusterIR(std::string &_out)
{
    if (clusters == nullptr) { return; }
    Clusters* real_clusters = static_cast<Clusters*>(clusters);
    real_clusters->printClusterIR(_out);
}

void Model::printClusterStats(std::string &_out)
{
    if (clusters == nullptr) { return; }
    Clusters* real_clusters = static_cast<Clusters*>(clusters);	
    real_clusters->printClusterStats(_out);
}
}
}
