cmake_minimum_required(VERSION 3.28)
project(advent_of_code LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_BUILD_TYPE "Debug")
# set(CMAKE_BUILD_TYPE "Release")

add_executable(day1   day1.c)
add_executable(day2   day2.c)
add_executable(day3   day3.c)
add_executable(day4   day4.c)
add_executable(day5   day5.c)
add_executable(day6   day6.c)
add_executable(day7   day7.c)
add_executable(day8   day8.c)
add_executable(day9   day9.c)
add_executable(day10 day10.c)
add_executable(day11 day11.c)

target_link_libraries(day7  m)
target_link_libraries(day11 m)
