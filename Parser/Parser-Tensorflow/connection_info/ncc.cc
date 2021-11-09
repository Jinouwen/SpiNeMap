#include <stdlib.h>

#include "model.h"
// #include "snn_converter/ann_to_snn.h"

int main(int argc, char *argv[])
{
    std::vector<std::string> args(argv, argv+argc);
    if ((argc > 4) || (argc < 2))
    {
        std::cout << "Incorrect Usage\n";
        std::cout << "Do this:\n";
        std::cout << "./ncc <arch_file> <weight_file>\n";
        std::cout << "or this:\n";
        std::cout << "./ncc <arch_file>\n";
        return 1;
    }
    std::string arch_file(argv[1]);
    std::string weight_file;

    if (argc == 4) {
        weight_file = args[4];
    } else {
        weight_file = "";
    }


    // long duration = 50;
    // char *duration_env = getenv("NCC_DURATION");
    // if (duration_env != NULL)
    // {
    //     duration = atoi(duration_env);
    // }

    // long num_to_test = 100;
    // char *num_to_test_env = getenv("NCC_NUM_TEST");
    // if (num_to_test_env != NULL)
    // {
    //     num_to_test = atoi(num_to_test_env);
    // }

    // long batch_size = 50;
    // char *batch_size_env = getenv("NCC_BATCH_SIZE");
    // if (batch_size_env != NULL)
    // {
    //     batch_size = atoi(batch_size_env);
    // }

    /*
    convert_ann_to_snn(arch_file.c_str(), model_name.c_str(), duration, num_to_test, 0);
    */
    NCC::NCC_FrontEnd::Model model(arch_file, weight_file);
    if (args[2] == "--layer") {
        //model.connectLayers();
        std::cout << "outputting layer connection stats\n";
        std::string out_root = arch_file.substr(0, arch_file.find(".json"));
        std::string outputIRFile = out_root + ".layer_depth.txt";
        model.printLayerConns(out_root);
        model.outputLayerDepthIR(outputIRFile);
        std::pair<uint64_t, uint64_t> irr_metric = model.getIrregularMetric();
        uint64_t metric = std::get<0>(irr_metric);
        uint64_t num_connections = std::get<1>(irr_metric);
        std::cout << metric << ", " << num_connections << ", " 
                  << (float)metric/(float)num_connections << std::endl;

    } 
    // else if (args[2] == "--neuron") {
    //     model.connector();
    //     std::cout << "\n";
    //     model.printLayers();
    //     std::cout << "\n";
    //     std::string out_root = arch_file.substr(0, arch_file.find(".json"));
    //     model.printConns(out_root);
    // }
}
