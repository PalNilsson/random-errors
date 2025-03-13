cmake_minimum_required(VERSION 3.10)

# 
project(WorkspaceProject)

# 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 
set(CMAKE_CXX_COMPILER "g++")

# 
add_executable(mc_dict mc_dict.cpp)
add_executable(error_code_generator error_code_generator.cpp)

# 
include_directories(single_include)

# 
# target_link_libraries(mc_dict some_library)
