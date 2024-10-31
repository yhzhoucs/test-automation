import argparse
import os
import subprocess
import re

parser = argparse.ArgumentParser(prog='graph-gen', description='生成图工具')

parser.add_argument('r', type=int, help='点数的幂 18/19/20 etc.')
parser.add_argument('a', type=float, help='a值')
parser.add_argument('b', type=float, help='b值')
parser.add_argument('c', type=float, help='c值')
parser.add_argument('-o', '--output', type=str, help='输出文件')
parser.add_argument('-t', '--threads', default=1, type=int, help='生成图的并发线程数')

args = parser.parse_args()

r = args.r
a = args.a
b = args.b
c = args.c
output_file = args.output
threads = args.threads

current_dir = os.path.dirname(os.path.abspath(__file__))
project_root = os.path.abspath(os.path.join(current_dir, '..'))
ligra_root = os.path.join(project_root, 'ligra')
rmat_gen_exe = os.path.join(ligra_root, 'build', 'utils', 'rMatGraph')

if output_file is None:
    output_file = os.path.join(project_root, f'rMat_{r}')

vertex_number = 2**r

# 添加线程数限制
myenv = os.environ.copy()
myenv['OMP_NUM_THREADS'] = f'{threads}'

graph_command = f'{rmat_gen_exe} -s -a {a:.2f} -b {b:.2f} -c {c:.2f} {vertex_number} {output_file}'
print('Executing:', graph_command)
try:
    subprocess.run(graph_command, env=myenv, shell=True, check=True)
except subprocess.CalledProcessError as e:
    print('Error during graph generation:', e)

pagerank_exe = os.path.join(ligra_root, 'build', 'apps', 'PageRank')
pagerankdelta_exe = os.path.join(ligra_root, 'build', 'apps', 'PageRankDelta')

print('Now testing...')
pagerank_command = f'{pagerank_exe} -s -rounds 1 {output_file}'
print('Executing:', pagerank_command)
try:
    result = subprocess.run(pagerank_command, env=myenv, shell=True, check=True, capture_output=True, text=True)
    output = result.stdout
except subprocess.CalledProcessError as e:
    print('Error during PageRank execution:', e)

# 提取 Total Edge Visited
pagerank_edge_visited_match = re.search(r'Total Edge Visted:\s*(\d+)', output)
if pagerank_edge_visited_match:
    total_edge_visited_pagerank = int(pagerank_edge_visited_match.group(1))
else:
    print('Failed to extract values from PageRank output.')

pagerank_delta_command = f'{pagerankdelta_exe} -s -rounds 1 {output_file}'
print('Executing:', pagerank_delta_command)
try:
    result = subprocess.run(pagerank_delta_command, env=myenv, shell=True, check=True, capture_output=True, text=True)
    output = result.stdout
except subprocess.CalledProcessError as e:
    print('Error during PageRank execution:', e)

# 提取 Total Edge Visited
pagerank_delta_edge_visited_match = re.search(r'Total Edge Visted:\s*(\d+)', output)
if pagerank_delta_edge_visited_match:
    total_edge_visited_pagerank_delta = int(pagerank_delta_edge_visited_match.group(1))
else:
    print("Failed to extract values from PageRankDelta output.")
    
print('PR: {}'.format(total_edge_visited_pagerank))
print('DPR: {}'.format(total_edge_visited_pagerank_delta))
print('RATIO: {:.2f}'.format(total_edge_visited_pagerank / total_edge_visited_pagerank_delta))
print('Testing done.')
