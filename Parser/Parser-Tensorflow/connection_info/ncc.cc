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

    NCC::NCC_FrontEnd::Model model(arch_file, weight_file);
    model.connector();
    // std::cout << "\n";
    // model.printLayers();
    // std::cout << "\n";
    std::string out_root = arch_file.substr(0, arch_file.find(".json"));
    model.printConns(out_root);
    // model.printLayers();
}
