#include <stdlib.h>

#include "model.h"
// #include "snn_converter/ann_to_snn.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "Incorrect Usage\n";
        std::cout << "./ncc <arch_file> <weight_file>\n";
        return 1;
    }
    std::string arch_file(argv[1]);
    std::string weight_file(argv[2]);

    /*
    long duration = 50;
    char *duration_env = getenv("NCC_DURATION");
    if (duration_env != NULL)
    {
        duration = atoi(duration_env);
    }

    long num_to_test = 100;
    char *num_to_test_env = getenv("NCC_NUM_TEST");
    if (num_to_test_env != NULL)
    {
        num_to_test = atoi(num_to_test_env);
    }

    long batch_size = 50;
    char *batch_size_env = getenv("NCC_BATCH_SIZE");
    if (batch_size_env != NULL)
    {
        batch_size = atoi(batch_size_env);
    }

    convert_ann_to_snn(arch_file.c_str(), model_name.c_str(), duration, num_to_test, 0);
    */
    NCC::NCC_FrontEnd::Model model(arch_file, weight_file);
    model.connector();
    // std::cout << "\n";
    // model.printLayers();
    // std::cout << "\n";
    std::string out_root = "../output/";//arch_file.substr(0, arch_file.find(".json"));
    model.printConns(out_root);
}
