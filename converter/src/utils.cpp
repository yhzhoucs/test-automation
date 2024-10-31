#include "utils.h"

#include "gtool/builder.h"
#include <vector>
#include <fstream>
#include <iterator>
#include <ranges>
#include <filesystem>
#include <cassert>

gtool::Graph<Node> from_simulator(std::string const &row_ptr_path, std::string const &column_path) {
    std::vector<int> row_ptr;
    row_ptr.reserve(10000);

    std::ifstream in(row_ptr_path, std::ios::in);
    std::copy(std::istream_iterator<int>(in), std::istream_iterator<int>(), std::back_inserter(row_ptr));
    in.close();

    std::vector<int> column;
    column.reserve(row_ptr.back());

    in.open(column_path, std::ios::in);
    std::copy(std::istream_iterator<int>(in), std::istream_iterator<int>(), std::back_inserter(column));

    std::ofstream out("temp.txt", std::ios::out | std::ios::trunc);
    out << "# vertex_number edge_number directed_edge_number" << std::endl;
    out << "# " << row_ptr.size() - 1 << " " << row_ptr.back() << std::endl;
    for (auto u : std::views::iota(0ul, row_ptr.size() - 1)) {
        auto from = row_ptr[u];
        auto to = row_ptr[u + 1];
        for ( ; from < to; ++from) {
            out << u << " " << column[from] << "\n";
        }
    }
    out.flush();
    out.close();

    gtool::Builder<Node> builder("temp.txt", true, false);
    gtool::Graph<Node> graph;
    {
        auto raw = builder.build_csr();
        graph = gtool::simplify_graph(raw);
    }

    namespace fs = std::filesystem;
    fs::remove("temp.txt");

    return graph;
}

gtool::Graph<Node> from_edgelist(std::string const &edgelist_path, bool symmetrize) {
    gtool::Builder<Node> builder(edgelist_path, true, symmetrize);
    return builder.build_csr();
}

gtool::Graph<Node> from_ligra(std::string const &ligra_graph_path, bool symmetrize) {
    std::vector<int> row_ptr;
    row_ptr.reserve(10000);

    std::ifstream in(ligra_graph_path, std::ios::in);
    std::string line;
    std::getline(in, line);
    assert(line == "AdjacencyGraph");

    long long vn{}, en{};
    in >> vn >> en;

    for (Node tmp{}; auto n : std::views::iota(0, vn)) {
        in >> tmp;
        row_ptr.emplace_back(tmp);
    }
    row_ptr.emplace_back(en);

    std::vector<int> column;
    column.reserve(en);

    for (Node tmp{}; auto n : std::views::iota(0, en)) {
        in >> tmp;
        column.emplace_back(tmp);
    }

    std::ofstream out("temp.txt", std::ios::out | std::ios::trunc);
    out << "# vertex_number edge_number directed_edge_number" << std::endl;
    out << "# " << row_ptr.size() - 1 << " " << row_ptr.back() << std::endl;
    for (auto u : std::views::iota(0ul, row_ptr.size() - 1)) {
        auto from = row_ptr[u];
        auto to = row_ptr[u + 1];
        for ( ; from < to; ++from) {
            out << u << " " << column[from] << "\n";
        }
    }
    out.flush();
    out.close();

    gtool::Builder<Node> builder("temp.txt", true, symmetrize);
    gtool::Graph<Node> graph;
    {
        auto raw = builder.build_csr();
        graph = gtool::simplify_graph(raw);
    }

    namespace fs = std::filesystem;
    fs::remove("temp.txt");

    return graph;
}

void to_simulator(gtool::Graph<Node> const &g, std::string const &row_ptr_path, std::string const &column_path) {
    std::ofstream out(row_ptr_path, std::ios::out | std::ios::trunc);

    int carry{};
    for (auto u : std::views::iota(0ul, g.get_vertex_number())) {
        out << carry << "\n";
        carry += g.out_degree(u);
    }
    out << carry << "\n";
    out.flush();
    out.close();

    out.open(column_path, std::ios::out | std::ios::trunc);
    for (auto u : std::views::iota(0ul, g.get_vertex_number())) {
        for (auto const &v : g.out_neighbors(u)) {
            out << v << "\n";
        }
    }
    out.flush();
    out.close();
}

void to_edgelist(gtool::Graph<Node> const &g, std::string const &edgelist_path) {
    std::ofstream out(edgelist_path, std::ios::out | std::ios::trunc);
    out << "# vertex_number edge_number " << std::endl;
    out << "# " << g.get_vertex_number() << " " << g.get_edge_number() << std::endl;

    for (auto u : std::views::iota(0ul, g.get_vertex_number())) {
        for (auto const &v : g.out_neighbors(u)) {
            out << u << " " << v << "\n";
        }
    }
    out.flush();
    out.close();
}

void to_ligra(gtool::Graph<Node> const &g, std::string const &ligra_graph_path) {
    std::ofstream out(ligra_graph_path, std::ios::out | std::ios::trunc);

    // ligra 文件格式
    // AdjacencyGraph
    // <n>
    // <m>
    // <o0>
    // <o1>
    // ...
    // <o(n-1)>
    // <e0>
    // <e1>
    // ...
    // <e(m-1)>

    out << "AdjacencyGraph\n";
    out << g.get_vertex_number() << "\n";
    out << g.get_edge_number() << "\n";

    auto offset = std::make_unique<int[]>(g.get_vertex_number());
    for (auto carry{0}; auto u : std::views::iota(0ull, g.get_vertex_number())) {
        offset[u] = carry;
        carry += g.out_degree(u);
    }

    std::copy(offset.get(), offset.get() + g.get_vertex_number(), std::ostream_iterator<int>(out, "\n"));
    out.flush();

    for (auto u : std::views::iota(0ull, g.get_vertex_number())) {
        for (auto v : g.out_neighbors(u)) {
            out << v << "\n";
        }
    }

    out.close();
}