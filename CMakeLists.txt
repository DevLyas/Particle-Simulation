cmake_minimum_required(VERSION 3.15)
project(Particle-Simulation)

find_package(raylib REQUIRED)

add_executable(main main.c)

target_link_libraries(main raylib)
