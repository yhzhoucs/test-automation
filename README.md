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

### 1. 进入 converter/src/main.cpp 目录下修改代码

一般来说，只要写两行代码
- 从文件中读取图： from_xxx 函数
- 把图写入另一种格式的文件： to_xxx 函数

### 2. 写你的 CMakeUserPresets.json

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

### 3. 编译项目

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

### 4. 执行转换

```bash
./out/path/to/your/executable

# 如果是上面的 CMakeUserPresets.json
# 那么执行
./out/build/x64-debug/converter
```