cmake_minimum_required(VERSION 3.17)

set(PROJECT "ClusterIO")

PROJECT(${PROJECT})

add_definitions(-DQT_MESSAGELOGCONTEXT)

add_subdirectory(ClusterLoader)
add_subdirectory(ClusterExporter)