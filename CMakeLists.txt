# set minimum cmake version
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# project name and language
project(crb LANGUAGES CXX)

add_executable(crb src/ring_buffer.h src/main.cpp)