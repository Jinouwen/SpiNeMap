#include <boost/program_options.hpp>
#include <iostream>

#include "util/args.hh"

namespace EXT
{
Argument::Argument(int argc, char **argv)
{
    namespace po = boost::program_options;
    po::options_description desc("Options");
    desc.add_options()
        ("help", "Print help messages")
        ("conn-file", po::value<std::string>(&connection_file)->required(),
                 "Connection file")
        ("spike-file", po::value<std::string>(&spike_file)->required(),
                 "Spike file")
        ("unroll-fanin", po::value<unsigned>(&unrolled_fanin),
                   "Max fanin to unroll")
        ("cluster-crossbar-size", po::value<unsigned>(&crossbar_size),
                   "Crossbar size")
        ("clustering-algo", po::value<std::string>(&clustering_algo),
            "Clustering algorithms: random, min-clusters, min-comm")
        ("unroll-ir-out", po::value<std::string>(&unrolled_ir_output),
                   "Unrolled IR output file")
        ("unroll-parent-out", po::value<std::string>(&unrolled_parent_output),
                   "Unrolled parent neurons output file")
        ("cluster-stats", po::value<std::string>(&cluster_stats),
                   "Clustering stats")
        ("cluster-ir-out", po::value<std::string>(&cluster_ir_output),
                   "Clustering IR output file");

    po::variables_map vm;

    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vm); // can throw 

        if (vm.count("help"))
        {
            std::cout << "NCC extension.\n"
                      << desc << "\n";
            exit(0);
        }

        po::notify(vm);
    }
    catch(po::error& e)
    {
        std::cerr << "ERROR: " << e.what() << "\n\n";
        std::cerr << desc << "\n";
        exit(0);
    }
}
}
