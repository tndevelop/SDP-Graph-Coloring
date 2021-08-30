cmake_minimum_required(VERSION 3.19)
project(repo)

set(CMAKE_CXX_STANDARD 11)

add_executable(repo main.cpp util.h util.cpp graphColoringAlgorithms.h graphColoringAlgorithms.cpp algorithmMIS.cpp algorithmMIS.h algorithmGreedy.h algorithmGreedy.cpp)