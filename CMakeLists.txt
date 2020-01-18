cmake_minimum_required(VERSION 3.14)
project(node_cpp_addon)
set(CMAKE_CXX_STANDARD 14)
# .node-gyp
include_directories(C:\\Users\\20311\\.node-gyp\\11.13.0\\include\\node)
AUX_SOURCE_DIRECTORY(. CUR_DIR)
add_subdirectory(./lib)
add_executable(node_cpp_addon ${CUR_DIR})
