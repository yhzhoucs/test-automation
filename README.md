# 全自动生成/测试图工具

## 准备

- make
- git
- g++
- python

## 自动搜寻图步骤

### 0. 拉取本仓库

```bash
git clone https://github.com/yhzhoucs/test-automation.git
```

### 1. 运行准备脚本

```bash
chmod +x prepare.sh
./prepare.sh
```

这个脚本会拉取 ligra 的代码，执行 patch 修改它，最后编译出可执行文件

可以修改 prepare.sh 脚本中的 `BUILD_THREADS_NUM` 来控制编译的线程数

### 2. 修改生成/测试的配置

进 scripts/autorun.py 中修改最上方的配置

### 3. 执行脚本，等待输出

```bash
python scripts/autorun.py
```

进 logs 目录查看执行结果

## 生成图并测试步骤

```bash
# 查看命令使用方法
python scripts/gen.py -h

# 生成一个 rmat18 图，其中 a,b,c 分别为 0.77, 0.04, 0.02
# 输出到 temp/my-rmat-18 文件，使用 10 个并发线程
python scripts/gen.py 18 0.77 0.04 0.02 -o temp/my-rmat-18 -t 10

# 使用默认位置（输出到 ./rMat_18），默认线程（1个）
python scripts/gen.py 18 0.77 0.04 0.02
```

# 转换图格式工具

位于 converter 目录下，可实现 模拟器格式、边表格式、 Ligra 格式 三者之间的相互转化

## 准备

- cmake 3.20.0 及以上
- ninja
- g++

## 转换图格式步骤

### 1. 写你的 CMakeUserPresets.json

可参考：

```json
{
    "version": 2,
    "configurePresets": [
        {
            "name": "x64-debug",
            "displayName": "x64 Debug",
            "description": "debug build for x64 arch",
            "inherits": "ci-ninja",
            "architecture": {
                "value": "x64",
                "strategy": "external"
            },
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Debug",
                "CMAKE_C_COMPILER": "$env{HOME}/local/bin/gcc",
                "CMAKE_CXX_COMPILER": "$env{HOME}/local/bin/g++"
            }
        }
    ],
    "buildPresets": [
        {
            "name": "converter",
            "configurePreset": "x64-debug",
            "targets": ["converter"]
        }
    ]
}
```

**P.S.** 如果系统里装有 gcc/g++ ，那么将下面两行删去后一般就能正常运行

```
"CMAKE_C_COMPILER": "$env{HOME}/local/bin/gcc",
"CMAKE_CXX_COMPILER": "$env{HOME}/local/bin/g++"
```

**P.S.** 如果不想用 Ninja 建构，你也可以指定如 Unix Makefiles 之类的构建器

```json
{
    "name": "x64-debug",
    "displayName": "x64 Debug",
    "description": "debug build for x64 arch",
    "inherits": "ci-base",
    "generator": "Unix Makefiles",
    "architecture": {
        "value": "x64",
        "strategy": "external"
    },
    "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Debug",
    }
}
```

### 2. 编译项目

```bash
# 在 convert 目录下
cmake --list-presets

# 查看 build preset
cmake --list-presets build

# 配置 cmake
cmake --preset <preset-name>

# 编译
cmake --build --preset <preset-name>

# 如果是用上面提供的 CMakeUserPresets.json
# 那么运行的命令依次为
cmake --preset x64-debug
cmake --build --preset converter
```

### 3. 执行转换

执行 converter 并输入参数，格式如下：

```
converter <mode> [-s] -i <file1[,file2]> -o <file1[,file2]>
converter <mode> [-s] -i <file1> [-i file2] -o <file1> [-o file2]
converter <mode> [-s] --input <file1[,file2]> --output <file1[,file2]>
```

mode 为转换模式，有下面几个选择
- `e2l` ：边表到 Ligra
- `e2s` ：边表到模拟器，此时 -o 要指定两个文件名，前一个是偏移，后一个是边
- `l2e` ：Ligra 到边表
- `l2s` ：Ligra 到模拟器，同样 -o 要指定两个文件名
- `s2e` ：模拟器到边表，此时 -i 要指定两个文件名，前一个是偏移，后一个是边
- `s2l` ：模拟器到 Ligra ，同样 -i 要指定两个文件名

如果指定 `-s` 那么对于每条边，添加它的反向边

```bash
# 查看使用指南
converter --help

# 下面是几个示例
# 将 Ligra 格式转化为模拟器格式
# -o 后面的两个文件名间不能有空格，用逗号隔开
./converter l2s -i ./dataset/ligra.txt -o ./output/row_ptr,./output/column 
# 等效于
./converter l2s -i ./dataset/ligra.txt -o ./output/row_ptr -o ./output/column

# 将边表格式转化为 Ligra 格式
./converter e2l -i ./dataset/soc-Slashdot0811.txt -o ./output/ligra.txt
```