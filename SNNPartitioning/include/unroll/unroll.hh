#ifndef __UNROLL_H__
#define __UNROLL_H__

#include <boost/multiprecision/cpp_int.hpp> 

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <set>

namespace EXT
{
namespace Unrolling
{
typedef uint64_t UINT64;

const UINT64 INVALID_ID = (UINT64) - 1;

class Neuron
{
  protected:
    UINT64 neuron_id;
    std::vector<UINT64> input_neurons;
    std::vector<UINT64> output_neurons;

    boost::multiprecision::cpp_int num_spikes = 0;

    UINT64 parent = INVALID_ID; // Which neuron it unrolls from

    std::vector<UINT64> children; // All the intermediate neurons it unrolls

    std::set<UINT64> spike_times;

  public:
    Neuron() {}
    Neuron(UINT64 _id) : neuron_id(_id) {}
    Neuron(const Neuron &_copy) : neuron_id(_copy.neuron_id),
                                  input_neurons(_copy.input_neurons),
                                  output_neurons(_copy.output_neurons),
                                  num_spikes(_copy.num_spikes),
                                  spike_times(_copy.spike_times),
                                  parent(_copy.parent),
                                  children(_copy.children) {}

    void addInputNeuron(UINT64 _in_neuron)
    {
        input_neurons.push_back(_in_neuron);
    }
    void addOutputNeuron(UINT64 _out_neuron)
    {
        output_neurons.push_back(_out_neuron);
    }

    void addInputNeuronList(std::vector<UINT64> &_list)
    {
        input_neurons = _list;
    }
    void addOutputNeuronList(std::vector<UINT64> &_list)
    {
        output_neurons = _list;
    }

    void addNumSpikesFromOneInput(boost::multiprecision::cpp_int _spike)
    {
        boost::multiprecision::cpp_int ori = num_spikes; 
        num_spikes += _spike;
        if (num_spikes < ori)
	{ 
            std::cerr << "addNumSpikesFromOneInput: overflow detected." << std::endl;
            exit(0);
        }
    }

    auto& getSpikeTimes() { return spike_times; }
    void setSpikeTimes(std::set<UINT64> &_spike_times)
    {
        spike_times = _spike_times;
    }
    void setNumSpikes(boost::multiprecision::cpp_int _num_spikes)
    {
        num_spikes = _num_spikes;
    }
    void resetNumSpikes() { num_spikes = 0; }
    
    boost::multiprecision::cpp_int numOfSpikes() { return num_spikes; }

    void setNeuronId(UINT64 _id) { neuron_id = _id; }
    int getNeuronId() { return neuron_id; };

    // TODO, change this to ...Ref()
    std::vector<UINT64> &getInputNeuronList() { return input_neurons; };
    std::vector<UINT64> &getOutputNeuronList() { return output_neurons; };
    unsigned numInputNeurons() { return input_neurons.size(); }
    unsigned numOutputNeurons() { return output_neurons.size(); }

    void setParentId(UINT64 _id) { parent = _id; }
    UINT64 getParentId() { return parent; }
    bool hasParent() { return parent != INVALID_ID; }

    void addChild(UINT64 _child) { children.push_back(_child); }
    std::vector<UINT64> &getChildrenRef() { return children; }
    std::vector<UINT64> getChildrenCopy() { return children; }

    std::vector<UINT64> getInputNeuronListCopy() { return input_neurons; };
    std::vector<UINT64> getOutputNeuronListCopy() { return output_neurons; };

    void print_connections()
    {
        std::cout << "Neuron ID: " << neuron_id << "\n";
        std::cout << "Input Neuron IDs: ";
        for (auto &input : input_neurons) 
        { std::cout << input << " "; } std::cout << "\n";
        std::cout << "Output Neuron IDs: ";
        for (auto &output : output_neurons) 
        { std::cout << output << " "; } std::cout << "\n";
        std::cout << "\n";
    }
};

class Model
{
  protected:
    std::vector<Neuron> snn;
    std::vector<Neuron> usnn; // unrolled SNN

    // const unsigned INVALID_FANIN = (unsigned) - 1;

    unsigned max_fanin = (unsigned) - 1; // unrolling

  public:
    Model(const std::string&, const std::string&);
    ~Model();

    void setFanin(UINT64 _fanin) { max_fanin = _fanin; }
    void unroll();

    void outputUnrolledIR(const std::string&);

    void debugOutput(const std::string &out_name)
    {
        std::fstream file;
        file.open(out_name, std::fstream::out);

        for (auto &neuron : usnn)
        {
            file << "Neuron ID: " << neuron.getNeuronId() << "\n";

            auto &input_neurons = neuron.getInputNeuronList();
            auto &output_neurons = neuron.getOutputNeuronList();

            file << "Input Neuron IDs: ";
            for (auto &input : input_neurons) { file << input << " "; } file << "\n";
            file << "Output Neuron IDs: ";
            for (auto &output : output_neurons) { file << output << " "; } file << "\n";
            file << "\n";
        }

        file.close();
        return;
    }

    void parentNeuronOutput(const std::string &out_name)
    {
        if (usnn.size() == 0) { return; }

        std::fstream file;
        file.open(out_name, std::fstream::out);

        for (auto &neuron : usnn)
        {
            file << neuron.getNeuronId() << " ";

            auto parent = neuron.getParentId(); 

            if (neuron.getNeuronId() < snn.size()) { assert(parent == INVALID_ID); }
            else { assert(parent != INVALID_ID); }

            if (parent == INVALID_ID) { file << "-1" << "\n"; }
            else { file << parent << "\n"; }
        }

        file.close();
        return;
    }

  protected:
    UINT64 extractMaxNeuronId(const std::string&);    
    void readConnections(const std::string&);
    void readSpikes(const std::string&);

  // Clustering
  protected:
    void* clusters = nullptr;

  public:
    void clustering(std::string &mode,unsigned);
    void printClusterIR(std::string &_out);
    void printClusterStats(std::string &_out);
};
}
}

#endif
