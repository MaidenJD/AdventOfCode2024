cmake_minimum_required(VERSION 3.28)
project(advent_of_code LANGUAGES C)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_C_STANDARD 23)
# set(CMAKE_C_COMPILER "/usr/bin/gcc")
# set(CMAKE_C_FLAGS_DEBUG "-g -O0")

add_executable(day1 day1.c)
add_executable(day2 day2.c)
add_executable(day3 day3.c)
add_executable(day4 day4.c)
add_executable(day5 day5.c)
add_executable(day6 day6.c)
