#include "utils.h"
#include <gtool/graph.h>
#include <filesystem>

int main(int argc, char *argv[]) {
    namespace fs = std::filesystem;

    auto input_path = fs::path(PROJECT_ROOT_PATH)/"dataset";

    auto output_path = fs::path(PROJECT_ROOT_PATH)/"output";
    if (!fs::exists(output_path)) {
        fs::create_directory(output_path);
    }

    // 这里演示的是从 ligra 格式转换成模拟器格式
    // 假设图在 converter/dataset 目录下，名叫 ligra.txt
    auto graph = from_ligra((input_path/"ligra.txt").string(), false);
    // auto graph = from_edgelist((input_path/"soc-Slashdot0811.txt").string(), false);
    // auto graph = from_simulator((input_path/"soc-sim"/"row_ptr.txt").string(),
    //     (input_path/"soc-sim"/"column.txt").string());

    // 输出到 converter/output 目录下
    // 分别输出 row_ptr.txt 和 column.txt
    to_simulator(graph, (output_path/"row_ptr.txt").string(), 
        (output_path/"column.txt").string());
    // to_edgelist(graph, (output_path/"edgelist.txt").string());
    // to_ligra(graph, (output_path/"ligra.txt").string());

    return 0;
}