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

    std::vector<std::pair<Node, Node>> el;
    el.reserve(row_ptr.back());

    for (auto u : std::views::iota(0ul, row_ptr.size() - 1)) {
        auto from = row_ptr[u];
        auto to = row_ptr[u + 1];
        for ( ; from < to; ++from) {
            el.emplace_back(u, column[from]);
        }
    }

    return gtool::build_graph_from_edgelist<Node>(el);
}

gtool::Graph<Node> from_edgelist(std::string const &edgelist_path, bool symmetrize) {
    return gtool::build_graph_from_file<Node>(edgelist_path, symmetrize);
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

    std::copy_n(std::istream_iterator<int>(in), vn, std::back_inserter(row_ptr));
    row_ptr.emplace_back(en);

    std::vector<int> column;
    column.reserve(en);

    std::copy_n(std::istream_iterator<int>(in), en, std::back_inserter(column));

    std::vector<std::pair<Node, Node>> el;
    el.reserve(row_ptr.back());

    for (auto u : std::views::iota(0ul, row_ptr.size() - 1)) {
        auto from = row_ptr[u];
        auto to = row_ptr[u + 1];
        for ( ; from < to; ++from) {
            el.emplace_back(u, column[from]);
        }
    }

    if (symmetrize) {
        return gtool::build_graph_from_edgelist<Node>(gtool::symmetrize_edgelist(el));
    } else {
        return gtool::build_graph_from_edgelist<Node>(el);
    }

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