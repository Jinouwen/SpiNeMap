#include <string>

namespace EXT
{
class Argument
{
  public:
    static const unsigned INVALID = (unsigned) - 1;

  protected:
    std::string connection_file = "N/A";
    std::string spike_file = "N/A";
    std::string unrolled_ir_output = "N/A";
    std::string unrolled_parent_output = "N/A"; // Parent of each unrolled neurons

    std::string cluster_ir_output = "N/A";
    std::string cluster_stats = "N/A";

    unsigned unrolled_fanin = INVALID;
    unsigned crossbar_size = INVALID;

    std::string clustering_algo = "N/A";

  public:
    Argument(int argc, char **argv);

    auto getFanin() 
    {
        bool unroll_en = (unrolled_fanin != INVALID) ? true : false; 
        return std::make_pair(unroll_en, unrolled_fanin); 
    }
    auto getCrossbarSize()
    {
        bool valid = (crossbar_size != INVALID) ? true : false;
        return std::make_pair(valid, crossbar_size);
    }
    std::pair<bool,std::string&> getClusteringAlgo()
    {
        bool en = (clustering_algo != "N/A") ? true : false;
        return std::pair<bool,std::string&>(en, clustering_algo);
    }

    std::string& getConnFile() // This file is required
    { 
        return connection_file;
    }
    std::string& getSpikeFile() // This file is required
    { 
        return spike_file; 
    }

    std::pair<bool,std::string&> getUnrolledIROutputFile()
    {
        bool valid = (unrolled_ir_output != "N/A") ? true : false; 
        return std::pair<bool,std::string&>(valid,unrolled_ir_output);
    }
    std::pair<bool,std::string&> getUnrolledParentOutputFile()
    {
        bool valid = (unrolled_parent_output != "N/A") ? true : false;
        return std::pair<bool,std::string&>(valid,unrolled_parent_output);
    }
    std::pair<bool,std::string&> getClusterIROutputFile()
    {
        bool valid = (cluster_ir_output != "N/A") ? true : false;
        return std::pair<bool,std::string&>(valid,cluster_ir_output);
    }
    std::pair<bool,std::string&> getClusterStatsFile()
    {
        bool valid = (cluster_stats != "N/A") ? true : false;
        return std::pair<bool,std::string&>(valid,cluster_stats);
    }
};
}
