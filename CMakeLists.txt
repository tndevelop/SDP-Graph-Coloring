cmake_minimum_required(VERSION 3.19)
project(repo)

set(CMAKE_CXX_STANDARD 11)


add_executable(repo main.cpp util.h util.cpp algorithmJP.cpp algorithmJP.h algorithmMIS.cpp algorithmMIS.h algorithmGreedy.h algorithmGreedy.cpp algorithmSDL.h algorithmSDL.cpp)