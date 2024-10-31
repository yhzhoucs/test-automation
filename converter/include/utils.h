#ifndef ADAPTER_UTILS_H_
#define ADAPTER_UTILS_H_

#include <gtool/graph.h>

using Node = int;

gtool::Graph<Node> from_simulator(std::string const &row_ptr_path, std::string const &column_path);
gtool::Graph<Node> from_edgelist(std::string const &edgelist_path, bool symmetrize);
gtool::Graph<Node> from_ligra(std::string const &ligra_graph_path, bool symmetrize);

void to_simulator(gtool::Graph<Node> const &g, std::string const &row_ptr_path, std::string const &column_path);
void to_edgelist(gtool::Graph<Node> const &g, std::string const &edgelist_path);
void to_ligra(gtool::Graph<Node> const &g, std::string const &ligra_graph_path);

#endif // ADAPTER_UTILS_H_