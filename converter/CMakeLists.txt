cmake_minimum_required(VERSION 3.20.0)

project(converter
    VERSION 0.0.1
    DESCRIPTION "Tools for converting graph format"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# fetch dependency
cmake_policy(SET CMP0135 NEW)
include(FetchContent)
FetchContent_Declare(
        gtool
        URL https://github.com/yhzhoucs/graph-tool/releases/download/v0.1.2/gtool-0.1.2-Linux.tar.gz
        URL_HASH MD5=577484751E337C16A50C228EBDD32EC1
)
FetchContent_MakeAvailable(gtool)

set(SOURCES src/utils.cpp)

add_executable(converter src/main.cpp ${SOURCES})
target_include_directories(converter PRIVATE include)

# add my graph processing library
find_package(gtool CONFIG REQUIRED PATHS ${gtool_SOURCE_DIR})
if (gtool_FOUND)
    message(STATUS "Critical library `gtool` found.")
    target_link_libraries(converter PRIVATE gtool::gtool)
else ()
    message(FATAL_ERROR "The essential library `gtool` is not found.")
endif ()

target_compile_definitions(converter PRIVATE
    PROJECT_ROOT_PATH="${PROJECT_SOURCE_DIR}")