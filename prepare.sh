#!/bin/bash

ROOT_DIR="$(pwd)"
LIGRA_DIR="$ROOT_DIR/ligra"
PATCH_FILE="$ROOT_DIR/fix.patch"

# 并发编译的线程数
BUILD_THREADS_NUM=10

# 拉取 Ligra 代码
git submodule update --init --recursive

# 应用修改
cd "$LIGRA_DIR"
git apply "$PATCH_FILE"

# 编译 Ligra
cd "apps"
make OPENMP=1 -j$BUILD_THREADS_NUM
cd "../utils"
make OPENMP=1 -j$BUILD_THREADS_NUM

# 回退目录
cd "$ROOT_DIR" || exit 1