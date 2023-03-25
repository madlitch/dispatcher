cmake_minimum_required(VERSION 3.21)
project(Lab_4 C)

set(CMAKE_C_STANDARD 99)

add_executable(hostd hostd.c utility.c queue.c queue.h)
