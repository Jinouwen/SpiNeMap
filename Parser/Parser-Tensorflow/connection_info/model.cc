#include "model.h"

// boost library to parse json architecture file
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include <sstream>

// #include "protobuf/proto_graph/graph.pb.h"

#include <boost/filesystem.hpp>

namespace NCC
{
namespace NCC_FrontEnd
{
void Model::Architecture::connector()
{
    int prev = 0;
    for (int i = 0; i < layers.size() - 1; i++)
    {
        if (layers[i + 1].layer_type == Layer::Layer_Type::Conv2D)
        {
            if (layers[i + 1].padding_type == Layer::Padding_Type::same)
            {
                std::cout << "\nconv2d_pad " << prev << " " << i + 1 << "\n";
                connToConvPadding(prev, i + 1);
            }
            else
            {
                std::cout << "\nconv2d " << prev << " " << i + 1 << "\n";
                connToConv(prev, i + 1);
            }
            std::cout << "conv2d done\n";
            prev = i + 1;
        }
        /*
	else if (layers[i + 1].layer_type == Layer::Layer_Type::Activation)
        {
            connToAct(prev, i + 1); prev++;
        }
        else if (layers[i + 1].layer_type == Layer::Layer_Type::BatchNormalization)
        {
            connToNorm(prev, i + 1); prev++;
        }
        else if (layers[i + 1].layer_type == Layer::Layer_Type::Dropout)
        {
            connToDrop(prev, i + 1); prev++;
        }
        */
	else if (layers[i + 1].layer_type == Layer::Layer_Type::MaxPooling2D || 
            layers[i + 1].layer_type == Layer::Layer_Type::AveragePooling2D)
        {
            connToPool(prev, i + 1); prev = i + 1;
        }
	else if (layers[i + 1].layer_type == Layer::Layer_Type::Flatten)
        {
            connToFlat(prev, i + 1); prev = i + 1;
        }
	else if (layers[i + 1].layer_type == Layer::Layer_Type::Dense)
        {
            connToDense(prev, i + 1); prev = i + 1;
        }
        // else
        // {
        //     std::cerr << "Error: unsupported connection type. \n";
        //     exit(0);
        // }

        /*
        auto name = layers[i].name;
        auto type = layers[i].layer_type;

        std::cout << "Layer name: " << name << "; ";
        if (type == Layer::Layer_Type::Input) 
        { std::cout << "Layer type: Input"; }
        else if (type == Layer::Layer_Type::Conv2D) 
        { std::cout << "Layer type: Conv2D"; }
        else if (type == Layer::Layer_Type::Activation) 
        { std::cout << "Layer type: Activation"; }
        else if (type == Layer::Layer_Type::BatchNormalization) 
        { std::cout << "Layer type: BatchNormalization"; }
        else if (type == Layer::Layer_Type::Dropout) 
        { std::cout << "Layer type: Dropout"; }
        else if (type == Layer::Layer_Type::MaxPooling2D) 
        { std::cout << "Layer type: MaxPooling2D"; }
        else if (type == Layer::Layer_Type::AveragePooling2D) 
        { std::cout << "Layer type: AveragePooling2D"; }
        else if (type == Layer::Layer_Type::Flatten) 
        { std::cout << "Layer type: Flatten"; }
        else if (type == Layer::Layer_Type::Dense) 
        { std::cout << "Layer type: Dense"; }
        else { std::cerr << "Error: unsupported layer type\n"; exit(0); }
        std::cout << "\n";

        auto &output_dims = layers[i].output_dims;
        std::cout << "Output shape: ";
        for (auto dim : output_dims) { std::cout << dim << " "; }
        std::cout << "\n\n";
        */
    }
}

void Model::Architecture::connToConv(unsigned cur_layer_id, 
                                     unsigned next_layer_id)
{
    auto &cur_neurons_dims = layers[cur_layer_id].output_dims;
    auto &cur_neurons_ids = layers[cur_layer_id].output_neuron_ids;

    auto &conv_kernel_dims = layers[next_layer_id].w_dims;
    auto &conv_kernel_weights = layers[next_layer_id].weights;
    auto &conv_strides = layers[next_layer_id].strides;
    auto &conv_output_dims = layers[next_layer_id].output_dims;
    auto &conv_output_neuron_ids = layers[next_layer_id].output_neuron_ids;

    // Important. We need to re-organize the conv kernel to be more memory-friendly
    // Original layout: row->col->dep->filter
    // New layer: filter->dep->row->col
    unsigned row_limit = conv_kernel_dims[0];
    unsigned col_limit = conv_kernel_dims[1];
    unsigned dep_limit = conv_kernel_dims[2];
    unsigned filter_limit = conv_kernel_dims[3];

    std::vector<float> conv_kernel_weights_format(filter_limit * 
                                                  dep_limit * 
                                                  row_limit * 
                                                  col_limit, 0.0);

    for (unsigned row = 0; row < row_limit; row++)
    {
        for (unsigned col = 0; col < col_limit; col++)
        {
            for (unsigned dep = 0; dep < dep_limit; dep++)
            {
                for (unsigned filter = 0; filter < filter_limit; filter++)
                {
                    conv_kernel_weights_format[
                        filter * dep_limit * row_limit * col_limit + 
                        dep * row_limit * col_limit +
                        row * col_limit +
                        col] = 
                    conv_kernel_weights[
                        row * col_limit * dep_limit * filter_limit +
                        col * dep_limit * filter_limit +
                        dep * filter_limit +
                        filter];
                }
            }
        }
    }

    /*
    for (unsigned row = 0; row < row_limit; row++)
    {
        for (unsigned col = 0; col < col_limit; col++)
        {
            for (unsigned dep = 0; dep < dep_limit; dep++)
            {
                for (unsigned filter = 0; filter < filter_limit; filter++)
                {
                    std::cout << conv_kernel_weights[
                        row * col_limit * dep_limit * filter_limit +
                        col * dep_limit * filter_limit +
                        dep * filter_limit +
                        filter] << " ";
                }
                std::cout << "\n";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }
    std::cout << "-------------------------\n";
    for (unsigned filter = 0; filter < filter_limit; filter++)
    {
        for (unsigned dep = 0; dep < dep_limit; dep++)
        {
            for (unsigned row = 0; row < row_limit; row++)
            {
                for (unsigned col = 0; col < col_limit; col++) 
                {
                    std::cout << conv_kernel_weights_format[
                        filter * dep_limit * row_limit * col_limit + 
                        dep * row_limit * col_limit + 
                        row * col_limit +
                        col] << " ";
                }
                std::cout << "\n";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    exit(0);
    */

    uint64_t conv_neuron_id_track = 
        cur_neurons_ids[cur_neurons_ids.size() - 1] + 1;
    // std::cout << conv_neuron_id_track << "\n";

    unsigned conv_output_dims_x = 0;
    unsigned conv_output_dims_y = 0;
    // For each filter
    for (unsigned filter = 0; filter < conv_kernel_dims[3]; filter++)
    {
        conv_output_dims_x = 0;
        for (unsigned row = conv_kernel_dims[0] - 1; 
            row < cur_neurons_dims[0]; 
            row += conv_strides[0])
        {
            conv_output_dims_x++;

            conv_output_dims_y = 0;
            for (unsigned col = conv_kernel_dims[1] - 1; 
                col < cur_neurons_dims[1]; 
                col += conv_strides[1])
            {
                conv_output_dims_y++;

                // All neurons inside the current kernel
                unsigned starting_row = row + 1 - conv_kernel_dims[0];
                unsigned ending_row = row;
                unsigned starting_col = col + 1 - conv_kernel_dims[1];
                unsigned ending_col = col;

                // std::cout << starting_row << " " 
                //     << ending_row << " " 
                //     << starting_col << " " 
                //     << ending_col << "\n";
                for (unsigned k = 0; k < cur_neurons_dims[2]; k++)
                {
                    for (unsigned i = starting_row; i <= ending_row; i++)
                    {
                        for (unsigned j = starting_col; j <= ending_col; j++)
                        {
                            uint64_t cur_neuron_id = 
                                cur_neurons_ids[
                                k * cur_neurons_dims[0] * cur_neurons_dims[1] +
                                i * cur_neurons_dims[1] + j];

                            float weight = 
                                conv_kernel_weights_format[
                                    filter * conv_kernel_dims[2] * 
                                    conv_kernel_dims[0] * 
                                    conv_kernel_dims[1] +


                                    k * conv_kernel_dims[0] * 
	                            conv_kernel_dims[1] +

                                    (i - starting_row) * conv_kernel_dims[1] +
                                    (j - starting_col)];
                            // conn_txt << cur_neuron_id << " " 
			    //     << conv_neuron_id_track << " " 
                            //     << weight << "\n";
                            
                            // Record the connection information
                            if (auto iter = connections.find(cur_neuron_id);
                                     iter != connections.end())
                            {
                                (*iter).second.out_neurons_ids.push_back(
                                    conv_neuron_id_track);
                                (*iter).second.weights.push_back(weight);
                            }
                            else
                            {
                                connections.insert({cur_neuron_id, 
                                    {conv_neuron_id_track, weight}});
                            }
                            
                            // std::cout << cur_neuron_id << " ";
                        }
                    }
                }
                // std::cout << "-> " << conv_neuron_id_track << "\n";
                conv_output_neuron_ids.push_back(conv_neuron_id_track);
                conv_neuron_id_track++;
                // std::cout << "\n";
            }
        }
    }
    // std::cout << "\n";
    conv_output_dims.push_back(conv_output_dims_x);
    conv_output_dims.push_back(conv_output_dims_y);
    conv_output_dims.push_back(conv_kernel_dims[3]);
}

void Model::Architecture::connToConvPadding(unsigned cur_layer_id, unsigned next_layer_id)
{
    auto &ori_neurons_dims = layers[cur_layer_id].output_dims;
    auto &ori_neurons_ids = layers[cur_layer_id].output_neuron_ids;

    auto &conv_kernel_dims = layers[next_layer_id].w_dims;
    auto &conv_kernel_weights = layers[next_layer_id].weights;
    auto &conv_strides = layers[next_layer_id].strides;
    auto &conv_output_dims = layers[next_layer_id].output_dims;
    auto &conv_output_neuron_ids = layers[next_layer_id].output_neuron_ids;

    // Important. We need to re-organize the conv kernel to be more memory-friendly
    // Original layout: row->col->dep->filter
    // New layer: filter->dep->row->col
    unsigned row_limit = conv_kernel_dims[0];
    unsigned col_limit = conv_kernel_dims[1];
    unsigned dep_limit = conv_kernel_dims[2];
    unsigned filter_limit = conv_kernel_dims[3];

    std::vector<float> conv_kernel_weights_format(filter_limit * 
                                                  dep_limit * 
                                                  row_limit * 
                                                  col_limit, 0.0);

    for (unsigned row = 0; row < row_limit; row++)
    {
        for (unsigned col = 0; col < col_limit; col++)
        {
            for (unsigned dep = 0; dep < dep_limit; dep++)
            {
                for (unsigned filter = 0; filter < filter_limit; filter++)
                {
                    conv_kernel_weights_format[
                        filter * dep_limit * row_limit * col_limit +
                        dep * row_limit * col_limit +
                        row * col_limit + col] =

                    conv_kernel_weights[
                        row * col_limit * dep_limit * filter_limit +
                        col * dep_limit * filter_limit +
                        dep * filter_limit +
                        filter];
                }
            }
        }
    }

    // Determine the number of paddings
    auto padding_to_row = 
        ((ori_neurons_dims[0] - 1) * 
          conv_strides[0] - ori_neurons_dims[0] + 
          conv_kernel_dims[0]) / 2;

    auto padding_to_col = 
        ((ori_neurons_dims[1] - 1) * 
          conv_strides[1] - ori_neurons_dims[1] + 
          conv_kernel_dims[1]) / 2;

    auto final_row_size = ori_neurons_dims[0] + 2 * padding_to_row;
    auto final_col_size = ori_neurons_dims[1] + 2 * padding_to_col;
    auto final_dep_size = ori_neurons_dims[2];

    std::vector<unsigned> final_neurons_dims{final_row_size, 
                                             final_col_size, 
                                             final_dep_size};

    std::vector<uint64_t> final_neurons_ids(final_row_size * 
                                            final_col_size * 
                                            final_dep_size, 0);
    std::vector<bool> final_neurons_ids_valid(final_row_size * 
                                              final_col_size * 
                                              final_dep_size, 0);
    for (unsigned dep = 0; dep < ori_neurons_dims[2]; dep++)
    {
        for (unsigned row = 0; row < ori_neurons_dims[0]; row++)
        {
            for (unsigned col = 0; col < ori_neurons_dims[1]; col++)
            {
                final_neurons_ids[
                    dep * final_row_size * final_col_size + 
                    (row + padding_to_row) * final_col_size + 
                    (col + padding_to_col)] = 

                ori_neurons_ids[
                    dep * ori_neurons_dims[0] * ori_neurons_dims[1] +
                    row * ori_neurons_dims[1] +
                    col];

                final_neurons_ids_valid[
                    dep * final_row_size * final_col_size + 
                    (row + padding_to_row) * final_col_size + 
                    (col + padding_to_col)] = 1;
            }
        }
    }

    uint64_t conv_neuron_id_track = 
        ori_neurons_ids[ori_neurons_ids.size() - 1] + 1;

    unsigned conv_output_dims_x = 0;
    unsigned conv_output_dims_y = 0;
    // For each filter
    for (unsigned filter = 0; 
         filter < conv_kernel_dims[3]; 
         filter++)
    {
        conv_output_dims_x = 0;
        for (unsigned row = conv_kernel_dims[0] - 1; 
             row < final_neurons_dims[0]; 
             row += conv_strides[0])
        {
            conv_output_dims_x++;

            conv_output_dims_y = 0;
            for (unsigned col = conv_kernel_dims[1] - 1; 
                 col < final_neurons_dims[1]; 
                 col += conv_strides[1])
            {
                conv_output_dims_y++;

                // All neurons inside the current kernel
                unsigned starting_row = row + 1 - conv_kernel_dims[0];
                unsigned ending_row = row;
                unsigned starting_col = col + 1 - conv_kernel_dims[1];
                unsigned ending_col = col;

                for (unsigned k = 0; k < final_neurons_dims[2]; k++)
                {
                    for (unsigned i = starting_row; i <= ending_row; i++)
                    {
                        for (unsigned j = starting_col; j <= ending_col; j++)
                        {
                            
                            if (final_neurons_ids_valid[
                                k * final_neurons_dims[0] * 
                                    final_neurons_dims[1] +
                                i * final_neurons_dims[1] + j])
                            {
                                uint64_t cur_neuron_id = final_neurons_ids[
                                    k * final_neurons_dims[0] * 
                                        final_neurons_dims[1] +
                                    i * final_neurons_dims[1] + j];

                                float weight =conv_kernel_weights_format[
                                    filter * conv_kernel_dims[2] * 
                                        conv_kernel_dims[0] * 
                                        conv_kernel_dims[1] +
                                    k * conv_kernel_dims[0] * 
                                        conv_kernel_dims[1] +
                                    (i - starting_row) * conv_kernel_dims[1] +
                                    (j - starting_col)];
                                
                                // Record the connection information
                                if (auto iter = connections.find(cur_neuron_id);
                                         iter != connections.end())
                                {
                                    (*iter).second.out_neurons_ids.push_back(
                                        conv_neuron_id_track);
                                    (*iter).second.weights.push_back(weight);
                                }
                                else
                                {
                                    connections.insert({cur_neuron_id, 
                                                       {conv_neuron_id_track, 
                                                        weight}});
                                }
                            }
                        }
                    }
                }
                conv_output_neuron_ids.push_back(conv_neuron_id_track);
                conv_neuron_id_track++;
            }
        }
    }
    conv_output_dims.push_back(conv_output_dims_x);
    conv_output_dims.push_back(conv_output_dims_y);
    conv_output_dims.push_back(conv_kernel_dims[3]);
}

void Model::Architecture::connToAct(unsigned cur_layer_id, unsigned next_layer_id)
{

}
        
void Model::Architecture::connToNorm(unsigned cur_layer_id, unsigned next_layer_id)
{

}

void Model::Architecture::connToDrop(unsigned cur_layer_id, unsigned next_layer_id)
{

}

void Model::Architecture::connToPool(unsigned cur_layer_id, unsigned next_layer_id)
{
    auto &cur_neurons_dims = layers[cur_layer_id].output_dims;
    auto &cur_neurons_ids = layers[cur_layer_id].output_neuron_ids;

    auto &pool_kernel_dims = layers[next_layer_id].w_dims;
    auto &pool_strides = layers[next_layer_id].strides;
    auto &pool_output_dims = layers[next_layer_id].output_dims;
    auto &pool_output_neuron_ids = layers[next_layer_id].output_neuron_ids;

    uint64_t pool_neuron_id_track = 
        cur_neurons_ids[cur_neurons_ids.size() - 1] + 1;

    unsigned pool_output_dims_x = 0;
    unsigned pool_output_dims_y = 0;

    pool_kernel_dims.push_back(cur_neurons_dims[2]);

    // For each filter
    for (unsigned filter = 0; 
         filter < pool_kernel_dims[3]; 
         filter++)
    {
        pool_output_dims_x = 0;
        for (unsigned row = pool_kernel_dims[0] - 1; 
             row < cur_neurons_dims[0]; 
             row += pool_strides[0])
        {
            pool_output_dims_x++;

            pool_output_dims_y = 0;
            for (unsigned col = pool_kernel_dims[1] - 1; 
                 col < cur_neurons_dims[1]; 
                 col += pool_strides[1])
            {
                pool_output_dims_y++;

                // All neurons inside the current kernel
                unsigned starting_row = row + 1 - pool_kernel_dims[0];
                unsigned ending_row = row;
                unsigned starting_col = col + 1 - pool_kernel_dims[1];
                unsigned ending_col = col;

                for (unsigned i = starting_row; i <= ending_row; i++)
                {
                    for (unsigned j = starting_col; j <= ending_col; j++)
                    {
                        uint64_t cur_neuron_id = 
                            cur_neurons_ids[filter * cur_neurons_dims[0] * 
                                cur_neurons_dims[1] +
                            i * cur_neurons_dims[1] + j];
                        
                        // Record the connection information
                        if (auto iter = connections.find(cur_neuron_id);
                                 iter != connections.end())
                        {
                            (*iter).second.out_neurons_ids.push_back(
                                pool_neuron_id_track);
                            (*iter).second.weights.push_back(-1);
                        }
                        else
                        {
                            connections.insert({cur_neuron_id, 
                                               {pool_neuron_id_track, 
                                                -1}});
                        }
                    }
                }
                pool_output_neuron_ids.push_back(pool_neuron_id_track);
                pool_neuron_id_track++;
            }
        }
    }

    pool_output_dims.push_back(pool_output_dims_x);
    pool_output_dims.push_back(pool_output_dims_y);
    pool_output_dims.push_back(pool_kernel_dims[3]);
}

void Model::Architecture::connToFlat(unsigned cur_layer_id, unsigned next_layer_id)
{
    auto &cur_neurons_dims = layers[cur_layer_id].output_dims;
    auto &cur_neurons_ids = layers[cur_layer_id].output_neuron_ids;

    auto &output_dims = layers[next_layer_id].output_dims;
    auto &output_neuron_ids = layers[next_layer_id].output_neuron_ids;

    uint64_t out_neuron_id_track = cur_neurons_ids[cur_neurons_ids.size() - 1] + 1;

    uint64_t data_dim = 1;
    for (auto dim : cur_neurons_dims) { data_dim *= dim; }

    output_dims.push_back(data_dim);
    output_dims.push_back(1);
    output_dims.push_back(1);

    for (uint64_t i = 0; i < data_dim; i++)
    {
        uint64_t cur_neuron_id = cur_neurons_ids[i];
        
        if (auto iter = connections.find(cur_neuron_id);
               iter != connections.end())
        {
            (*iter).second.out_neurons_ids.push_back(out_neuron_id_track);
            (*iter).second.weights.push_back(-1);
        }
        else
        {
            connections.insert({cur_neuron_id, {out_neuron_id_track, -1}});
        }
        
        output_neuron_ids.push_back(out_neuron_id_track);
        out_neuron_id_track++;
    }
}

void Model::Architecture::connToDense(unsigned cur_layer_id, unsigned next_layer_id)
{
    auto &cur_neurons_dims = layers[cur_layer_id].output_dims;
    auto &cur_neurons_ids = layers[cur_layer_id].output_neuron_ids;

    auto &dense_dims = layers[next_layer_id].w_dims;
    auto &dense_weights = layers[next_layer_id].weights;
    auto &output_dims = layers[next_layer_id].output_dims;
    auto &output_neuron_ids = layers[next_layer_id].output_neuron_ids;
    
    if(cur_layer_id ==0)
    { 
        cur_neurons_ids.push_back(0); 
    }
    uint64_t out_neuron_id_track = cur_neurons_ids[cur_neurons_ids.size() - 1] + 1;

    uint64_t data_dim = 1;
    for (auto dim : cur_neurons_dims) { data_dim *= dim; }

    output_dims.push_back(dense_dims[1]);
    output_dims.push_back(1);
    output_dims.push_back(1);
    
    for (unsigned i = 0; i < dense_dims[1]; i++)
    {
        for (unsigned j = 0; j < dense_dims[0]; j++)
        {
            uint64_t cur_neuron_id = cur_neurons_ids[j];
            float weight = dense_weights[j * dense_dims[1] + i];
            
            if (auto iter = connections.find(cur_neuron_id);
                   iter != connections.end())
            {
                (*iter).second.out_neurons_ids.push_back(out_neuron_id_track);
                (*iter).second.weights.push_back(weight);
            }
            else    
            {
                connections.insert({cur_neuron_id, {out_neuron_id_track, weight}});
            }
        }
        output_neuron_ids.push_back(out_neuron_id_track);
        out_neuron_id_track++;
    }
}

void Model::Architecture::setOutRoot(std::string &out_root)
{
    std::string conns_out_txt = out_root + "connection_info.txt";
    conns_output.open(conns_out_txt);

    std::string weights_out_txt = out_root + "weight_info.txt";
    weights_output.open(weights_out_txt);
}

void Model::Architecture::layerOutput()
{

}

// TODO, we may need to break down the protobuf into multiple smaller files.
void Model::Architecture::printConns(std::string &out_root)
{
    // Txt record
    std::string conns_out_txt = out_root + "connection_info.txt";
    std::ofstream conns_out(conns_out_txt);

    std::string weights_out_txt = out_root + "weight_info.txt";
    std::ofstream weights_out(weights_out_txt);

    for (int i = 0; i < layers.size() - 1; i++)
    {
        auto &output_neurons = layers[i].output_neuron_ids;

        for (auto neuron : output_neurons)
        {            
            auto iter = connections.find(neuron);
            if (iter == connections.end()) { continue; }

            auto &out_neurons_ids = (*iter).second.out_neurons_ids;
            auto &weights = (*iter).second.weights;

            weights_out << neuron << " ";
            conns_out << neuron << " ";
            for (unsigned j = 0; j < out_neurons_ids.size(); j++)
            {
                weights_out << weights[j] << " ";
                conns_out << out_neurons_ids[j] << " ";
            }
            weights_out << "\n";
            conns_out << "\n";
        }
    }
    weights_out.close();
    conns_out.close();

    /*
    // Protobuf record
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    NCC_Graph_Proto::Graph graph;
    NCC_Graph_Proto::Node *node;

    boost::filesystem::path graph_root = out_root + ".graph";
    if (boost::filesystem::exists(graph_root))
    {
        std::cerr << "Error: graph already directory exists. \n";
        exit(0);
    }
    boost::filesystem::create_directory(graph_root);

    unsigned num_in_neurons_per_sub_graph = 1000;
    unsigned sub_graph_idx = 0;
    boost::filesystem::path sub_graph = std::to_string(sub_graph_idx) + ".graph";
    boost::filesystem::path sub_graph_full = graph_root / sub_graph;

    unsigned neurons_count = 0;
    for (int i = 0; i < layers.size() - 1; i++)
    {
        auto &output_neurons = layers[i].output_neuron_ids;

        for (auto neuron : output_neurons)
        {
            auto iter = connections.find(neuron);
            if (iter == connections.end()) { continue; }
            node = graph.add_nodes();
            node->set_id(neuron);
            node->set_type(NCC_Graph_Proto::Node::IO); 

            auto &out_neurons_ids = (*iter).second.out_neurons_ids;
            auto &weights = (*iter).second.weights;

            for (unsigned j = 0; j < out_neurons_ids.size(); j++)
            {
                node->add_adjs(out_neurons_ids[j]);
                node->add_weights(weights[j]);
            }
            
            if (++neurons_count >= num_in_neurons_per_sub_graph)
            {
                neurons_count = 0;
                std::ofstream out(sub_graph_full.string());
                if (!graph.SerializeToOstream(&out))
                {
                    std::cerr << "Failed to graph." << std::endl;
                    exit(0);
                }
                out.close();

                // Start a new message
                sub_graph_idx++;
                sub_graph = std::to_string(sub_graph_idx) + ".graph";
                sub_graph_full = graph_root / sub_graph;
                graph.Clear();
            }
        }
    }
    
    if (neurons_count)
    {
        std::ofstream out(sub_graph_full.string());
        if (!graph.SerializeToOstream(&out))
        {
            std::cerr << "Failed to graph." << std::endl;
            exit(0);
        }
        out.close();
    }
    
    google::protobuf::ShutdownProtobufLibrary();
    */
}

void Model::loadArch(std::string &arch_file)
{
    try
    {
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(arch_file, pt);

        unsigned layer_counter = 0;
        // Iterate through the layers
        BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("config.layers"))
        {
            // We need to construct the input layer first
            // Sometimes, input layer is not explicitly specified. When the input layer is explicitly specified, 
            // we will change its name later.
            if (layer_counter == 0)
            {
                std::vector<std::string> input_shape;
                std::vector<unsigned> output_dims;
                for (boost::property_tree::ptree::value_type &cell : v.second.get_child("config.batch_input_shape"))
                {
                    input_shape.push_back(cell.second.get_value<std::string>());
                }

                input_shape.erase(input_shape.begin());
                for (auto dim : input_shape) { output_dims.push_back(stoll(dim)); }

                std::string name = "input";
                Layer::Layer_Type layer_type = Layer::Layer_Type::Input;
                arch.addLayer(name, layer_type);
                arch.getLayer(name).setOutputDim(output_dims);

                auto &out_neuro_ids = arch.getLayer(name).output_neuron_ids;
                for (int k = 0; k < output_dims[2]; k++)
                {
                    for (int i = 0; i < output_dims[0]; i++)
                    {
                        for (int j = 0; j < output_dims[1]; j++)
                        {
                            out_neuro_ids.push_back(k * output_dims[0] * output_dims[1] + 
                                                    i * output_dims[1] + j);
                        }
                    }
                }

                layer_counter++;
            }

            std::string class_name = v.second.get<std::string>("class_name");
            std::string name = v.second.get<std::string>("config.name");

            Layer::Layer_Type layer_type = Layer::Layer_Type::MAX;
            if (class_name == "InputLayer") { layer_type = Layer::Layer_Type::Input; }
            else if (class_name == "Conv2D") { layer_type = Layer::Layer_Type::Conv2D; }
            else if (class_name == "Activation") { layer_type = Layer::Layer_Type::Activation; }
            else if (class_name == "BatchNormalization") {layer_type = Layer::Layer_Type::BatchNormalization; }
            else if (class_name == "Dropout") { layer_type = Layer::Layer_Type::Dropout; }
            else if (class_name == "MaxPooling2D") { layer_type = Layer::Layer_Type::MaxPooling2D; }
            else if (class_name == "AveragePooling2D") { layer_type = Layer::Layer_Type::AveragePooling2D; }
            else if (class_name == "Flatten") { layer_type = Layer::Layer_Type::Flatten; }
            else if (class_name == "Dense") { layer_type = Layer::Layer_Type::Dense; }
            // else { std::cerr << "Error: Unsupported layer type.\n"; exit(0); }

            if (class_name != "InputLayer")
            {
                arch.addLayer(name, layer_type);
            }
            else if (class_name == "InputLayer")
            {
                // The input layer is explicitly specified, we need to change its name here.
                std::string default_name = "input";
                arch.getLayer(default_name).name = name; // When input is explicitly mentioned.
            }

            if (class_name == "Conv2D" || class_name == "MaxPooling2D" || class_name == "AveragePooling2D")
            {
                // get padding type
                std::string padding_type = v.second.get<std::string>("config.padding");
                if (padding_type == "same")
                {
                    arch.getLayer(name).padding_type = Layer::Padding_Type::same;
                }

                // get strides information
                std::vector<std::string> strides_str;
                std::vector<unsigned> strides;
                for (boost::property_tree::ptree::value_type &cell : v.second.get_child("config.strides"))
                {
                    strides_str.push_back(cell.second.get_value<std::string>());
                }
  
                for (auto stride : strides_str) { strides.push_back(stoll(stride)); }
                arch.getLayer(name).setStrides(strides);
            }

            if (class_name == "MaxPooling2D" || class_name == "AveragePooling2D")
            {
                // We need pool_size since Conv2D's kernel size can be extracted from h5 file
                std::vector<std::string> pool_size_str;
                auto &pool_size = arch.getLayer(name).w_dims;
                for (boost::property_tree::ptree::value_type &cell : v.second.get_child("config.pool_size"))
                {
                    pool_size_str.push_back(cell.second.get_value<std::string>());
                }

                for (auto size : pool_size_str) { pool_size.push_back(stoll(size)); }
                pool_size.push_back(1); // depth is 1
            }

            // TODO, more information to extract, such as activation method...

            layer_counter++;
        }
    }
    catch (std::exception const& e)
    {
        std::cerr << e.what() << std::endl;
        exit(0);
    }
}

void Model::loadWeights(std::string &weight_file)
{
    // Example on parsing H5 format
    hid_t file;
    hid_t gid; // group id
    herr_t status;

    // char model_path[MAX_NAME];

    // Open h5 model
    file = H5Fopen(weight_file.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
    gid = H5Gopen(file, "/", H5P_DEFAULT); // open root
    scanGroup(gid);

    status = H5Fclose(file);
}

void Model::scanGroup(hid_t gid)
{
    ssize_t len;
    hsize_t nobj;
    herr_t err;
    int otype;
    hid_t grpid, dsid;
    char group_name[MAX_NAME];
    char memb_name[MAX_NAME];
    char ds_name[MAX_NAME];

    // Get number of objects in the group
    len = H5Iget_name(gid, group_name, MAX_NAME);
    err = H5Gget_num_objs(gid, &nobj);

    // Iterate over every object in the group
    for (int i = 0; i < nobj; i++)
    {
        // Get object type
        len = H5Gget_objname_by_idx(gid, (hsize_t)i, memb_name, (size_t)MAX_NAME);
        otype = H5Gget_objtype_by_idx(gid, (size_t)i);

        switch (otype)
        {
            // If it's a group, recurse over it
        case H5G_GROUP:
            grpid = H5Gopen(gid, memb_name, H5P_DEFAULT);
            scanGroup(grpid);
            H5Gclose(grpid);
            break;
            // If it's a dataset, that means group has a bias and kernel dataset
        case H5G_DATASET:
            dsid = H5Dopen(gid, memb_name, H5P_DEFAULT);
            H5Iget_name(dsid, ds_name, MAX_NAME);
            // std::cout << ds_name << "\n";
            extrWeights(dsid);
            break;
        default:
            break;
        }
    }
}

void Model::extrWeights(hid_t id)
{
    hid_t datatype_id, space_id;
    herr_t status;
    hsize_t size;
    char ds_name[MAX_NAME];

    H5Iget_name(id, ds_name, MAX_NAME);
    space_id = H5Dget_space(id);
    datatype_id = H5Dget_type(id);

    // Get dataset dimensions to create buffer of same size
    const int ndims = H5Sget_simple_extent_ndims(space_id);
    hsize_t dims[ndims];
    H5Sget_simple_extent_dims(space_id, dims, NULL);

    // Calculating total 1D size
    unsigned data_size = 1;
    for (int i = 0; i < ndims; i++) { data_size *= dims[i]; }
    float *rdata = (float *)malloc(data_size * sizeof(float));
    status = H5Dread(id, datatype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);
    
    // Add information to the corres. layer
    std::stringstream full_name(ds_name);
    std::vector <std::string> tokens;
    std::string intermediate;

    while(getline(full_name, intermediate, '/'))
    {
        tokens.push_back(intermediate);
    }
    // The secondary last element indicates the layer name
    // TODO, I'm not sure if this is always true. Need to do more research
    if (tokens[tokens.size() - 1].find("kernel") != std::string::npos)
    {
        Layer &layer = arch.getLayer(tokens[tokens.size() - 2]);
        std::vector<unsigned> dims_vec(dims, dims + ndims);
        std::vector<float> rdata_vec(rdata, rdata + data_size);

        layer.setWeights(dims_vec, rdata_vec);
    }
    else if (tokens[tokens.size() - 1].find("bias") != std::string::npos)
    {
        Layer &layer = arch.getLayer(tokens[tokens.size() - 2]);
        std::vector<unsigned> dims_vec(dims, dims + ndims);
        std::vector<float> rdata_vec(rdata, rdata + data_size);

        layer.setBiases(dims_vec, rdata_vec);
    }
    else if (tokens[tokens.size() - 1].find("beta") != std::string::npos)
    {
        Layer &layer = arch.getLayer(tokens[tokens.size() - 2]);
        std::vector<unsigned> dims_vec(dims, dims + ndims);
        std::vector<float> rdata_vec(rdata, rdata + data_size);

        layer.setBeta(dims_vec, rdata_vec);
    }
    else if (tokens[tokens.size() - 1].find("gamma") != std::string::npos)
    {
        Layer &layer = arch.getLayer(tokens[tokens.size() - 2]);
        std::vector<unsigned> dims_vec(dims, dims + ndims);
        std::vector<float> rdata_vec(rdata, rdata + data_size);

        layer.setGamma(dims_vec, rdata_vec);
    }
    else if (tokens[tokens.size() - 1].find("moving_mean") != std::string::npos)
    {
        Layer &layer = arch.getLayer(tokens[tokens.size() - 2]);
        std::vector<unsigned> dims_vec(dims, dims + ndims);
        std::vector<float> rdata_vec(rdata, rdata + data_size);

        layer.setMovingMean(dims_vec, rdata_vec);
    }
    else if (tokens[tokens.size() - 1].find("moving_variance") != std::string::npos)
    {
        Layer &layer = arch.getLayer(tokens[tokens.size() - 2]);
        std::vector<unsigned> dims_vec(dims, dims + ndims);
        std::vector<float> rdata_vec(rdata, rdata + data_size);

        layer.setMovingVariance(dims_vec, rdata_vec);
    }
    free(rdata);
}
}
}
