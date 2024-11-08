#include "utils.h"
#include <gtool/graph.h>
#include <cxxopts.hpp>
#include <filesystem>
#include <iostream>

constexpr char valid_opts[4] = "els";

bool is_valid_mode(std::string const &mode) {
    return mode.size() == 3
        && mode[1] == '2'
        && (std::find(std::begin(valid_opts), std::end(valid_opts), mode[0]) != std::end(valid_opts))
        && (std::find(std::begin(valid_opts), std::end(valid_opts), mode[2]) != std::end(valid_opts));
}

int main(int argc, char *argv[]) {
    namespace fs = std::filesystem;

    // edgelist, ligra, simulator

    cxxopts::Options options("converter", "A simple graph format converter");
    options.add_options()
        ("mode",
            "Converting mode, one of e2l,e2s,l2e,l2s,s2e,s2l (e: edgelist, l: ligra, s: simulator)",
            cxxopts::value<std::string>())
        ("s,symmetrize", "Symmetrize edgelist")
        ("i,input", "Input file path(s)", cxxopts::value<std::vector<std::string>>())
        ("o,output", "Output file path(s)", cxxopts::value<std::vector<std::string>>())
        ("h,help", "Print usage");
    
    options.parse_positional({"mode"});
    auto result = options.parse(argc, argv);

    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        exit(0);
    }

    if (!(result.count("mode") && result.count("input") && result.count("output"))) {
        std::cout << "Invalid inputs" << std::endl;
        std::cout << options.help() << std::endl;
        exit(1);
    }
    
    std::string mode = result["mode"].as<std::string>();
    if (!is_valid_mode(mode)) {
        std::cout << "Invalid input" << std::endl;
        std::cout << options.help() << std::endl;
        exit(1);
    }

    std::vector<std::string> input_paths = result["input"].as<std::vector<std::string>>();
    if ((mode[0] == 's' && input_paths.size() != 2)
            || (mode[0] != 's' && input_paths.size() != 1)) {
        std::cout << "Invalid input" << std::endl;
        std::cout << options.help() << std::endl;
        exit(1);
    }
    for (auto const &path : input_paths) {
        if (!fs::exists(path)) {
            std::cout << "File not exists" << std::endl;
            exit(1);
        }
    }
    std::vector<std::string> output_paths = result["output"].as<std::vector<std::string>>();
    if ((mode[2] == 's' && output_paths.size() != 2)
            || (mode[2] != 's' && output_paths.size() != 1)) {
        std::cout << "Invalid output" << std::endl;
        std::cout << options.help() << std::endl;
        exit(1);
    }
    for (auto const &path : output_paths) {
        if (!fs::path(path).parent_path().empty() && !fs::exists(fs::path(path).parent_path())) {
            fs::create_directories(fs::path(path).parent_path());
        }
    }

    gtool::Graph<Node> graph;
    bool symmetrize = result.count("symmetrize") > 0;

    switch (mode[0]) {
    case 'e':
        graph = from_edgelist(input_paths[0], symmetrize);
        break;
    case 'l':
        graph = from_ligra(input_paths[0], symmetrize);
        break;
    case 's':
        graph = from_simulator(input_paths[0], input_paths[1]);
        break;
    default:
        ;
    }

    switch(mode[2]) {
    case 'e':
        to_edgelist(graph, output_paths[0]);
        break;
    case 'l':
        to_ligra(graph, output_paths[0]);
        break;
    case 's':
        to_simulator(graph, output_paths[0], output_paths[1]);
        break;
    default:
        ;
    }

    return 0;
}