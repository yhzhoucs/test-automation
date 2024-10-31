import subprocess
import random
import re
import os

# ============================== Configuration Start ==============================

# 2^{rmat} 次方个点
rmat = 19

# 搜索的轮次
max_iterations = 100

# 并发执行的线程数量，建议小于等于 2/3 服务器总核数
threads_num = 20

# =============================== Configuration End ===============================

# 添加线程数限制
myenv = os.environ.copy()
myenv['OMP_NUM_THREADS'] = f'{threads_num}'

vertex_number = 2**rmat
ratios = []  # 用于存储比值和对应的随机值

current_dir = os.path.dirname(os.path.abspath(__file__))
project_root = os.path.abspath(os.path.join(current_dir, '..'))
ligra_root = os.path.join(project_root, 'ligra')
rmat_gen_exe = os.path.join(ligra_root, 'build', 'utils', 'rMatGraph')
temp_directory = os.path.join(project_root, 'temp')
pagerank_exe = os.path.join(ligra_root, 'build', 'apps', 'PageRank')
pagerankdelta_exe = os.path.join(ligra_root, 'build', 'apps', 'PageRankDelta')
log_file_path = os.path.join(project_root, 'logs', f'search_rmat{rmat}.log')

try:
    log_file = open(log_file_path, 'w')
except OSError as e:
    print('Error during open log file:', e)


def log(s):
    global log_file
    log_file.write(s + '\n')
    log_file.flush()

def logf(formatter, *args, **kwargs):
    global log_file
    log_file.write(formatter.format(*args, **kwargs))
    log_file.write('\n')
    log_file.flush()

log(f'rmat {rmat} vertex number {vertex_number}')

# 创建临时文件存放目录
os.makedirs(temp_directory, exist_ok=True)
current_temp_file = os.path.join(temp_directory, f'rMat_{rmat}')

for i in range(max_iterations):
    # 生成随机小数
    a = random.uniform(0.01, 0.9)
    b = random.uniform(0.01, 0.9 - a)
    c = 0.9 - a - b

    # 构建命令生成图
    graph_command = f'{rmat_gen_exe} -s -a {a:.2f} -b {b:.2f} -c {c:.2f} {vertex_number} {current_temp_file}'

    # 打印并执行生成图的命令
    logf('Iteration {}: {}', i+1, graph_command)
    try:
        subprocess.run(graph_command, env=myenv, shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print('Error during graph generation:', e)
        continue

    # 运行 PageRank 算法
    pagerank_command = f'{pagerank_exe} -s -rounds 1 {current_temp_file}'
    try:
        result = subprocess.run(pagerank_command, env=myenv, shell=True, check=True, capture_output=True, text=True)
        output = result.stdout
    except subprocess.CalledProcessError as e:
        print('Error during PageRank execution:', e)
        continue

    temp_log_lst = []

    # 提取 Total Edge Visited
    pagerank_edge_visited_match = re.search(r'Total Edge Visted:\s*(\d+)', output)
    if pagerank_edge_visited_match:
        total_edge_visited_pagerank = int(pagerank_edge_visited_match.group(1))
        temp_log_lst.append(f'{total_edge_visited_pagerank}')
    else:
        print('Failed to extract values from PageRank output.')
        continue

    # 运行 PageRankDelta 算法
    pagerank_delta_command = f'{pagerankdelta_exe} -s -rounds 1 {current_temp_file}'
    try:
        result = subprocess.run(pagerank_delta_command, env=myenv, shell=True, check=True, capture_output=True, text=True)
        output = result.stdout
    except subprocess.CalledProcessError as e:
        print("Error during PageRankDelta execution:", e)
        continue

    # 提取 Total Edge Visited
    pagerank_delta_edge_visited_match = re.search(r'Total Edge Visted:\s*(\d+)', output)
    if pagerank_delta_edge_visited_match:
        total_edge_visited_pagerank_delta = int(pagerank_delta_edge_visited_match.group(1))
        temp_log_lst.append(f'{total_edge_visited_pagerank_delta}')
    else:
        print("Failed to extract values from PageRankDelta output.")
        continue

    # 计算比值
    if total_edge_visited_pagerank_delta > 0:
        ratio = total_edge_visited_pagerank / total_edge_visited_pagerank_delta
        temp_log_lst.append(f'{ratio:.2f}')
        ratios.append((ratio, a, b, c))
    else:
        print("Total Edge Visited (PageRankDelta) is zero, cannot calculate ratio.")

    # 输出此前结果到 log
    log(" ".join(temp_log_lst))


# 输出比值最大的前 20 种情况
top_ratios = sorted(ratios, key=lambda x: x[0], reverse=True)[:20]
log("\nTop 20")
for r, a, b, c in top_ratios:
    log(f"Ratio: {r:.2f}, {a:.2f}, {b:.2f}, {c:.2f}")

# 关闭日志文件
log_file.close()