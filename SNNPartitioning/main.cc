#include "util/args.hh"
#include "unroll/unroll.hh"

typedef EXT::Argument Argument;
typedef EXT::Unrolling::Model Model;

int main(int argc, char **argv)
{
    Argument args(argc, argv);

    Model model(args.getConnFile(),
                args.getSpikeFile());

    if (auto [enable, fanin] = args.getFanin(); enable)
    {
        model.setFanin(fanin);
        model.unroll();
    }
    else
    {
        std::cerr << "---------------------------------------\n";
        std::cerr << "Warning: fanin is not provided.\n";
        std::cerr << "Warning: model is not unrolled.\n";
    }

    if (auto [enable, algo] = args.getClusteringAlgo(); enable)
    {
        if (auto [valid, crossbar_size] = args.getCrossbarSize(); valid)
        {
            model.clustering(algo,crossbar_size);
        }
        else
        {
            std::cerr << "---------------------------------------\n";
            std::cerr << "Warning: crossbar size is not provided.\n";
            std::cerr << "Warning: clustering is disabled.\n";
        }
    }
    else
    {
        std::cerr << "---------------------------------------\n";
        std::cerr << "Warning: clustering algorithm is not provided.\n";
        std::cerr << "Warning: clustering is disabled.\n";
    }

    if (auto [valid, unrolled_ir] = args.getUnrolledIROutputFile(); valid)
    {
        model.outputUnrolledIR(unrolled_ir);
    }
    if (auto [valid, parent_neu_out] = args.getUnrolledParentOutputFile(); valid)
    {
        model.parentNeuronOutput(parent_neu_out);
    }
    if (auto [valid, cluster_ir] = args.getClusterIROutputFile(); valid)
    {
        model.printClusterIR(cluster_ir);
    }
    if (auto [valid, cluster_stats] = args.getClusterStatsFile(); valid)
    {
        // std::cout << cluster_stats << "\n";
        model.printClusterStats(cluster_stats);
    }
    
    return 0;
}
