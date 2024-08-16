cmake_minimum_required(VERSION 3.17)

# -----------------------------------------------------------------------------
# ClusterIO plugins
# -----------------------------------------------------------------------------
set(PROJECT "ClusterIO")
PROJECT(${PROJECT})

add_definitions(-DQT_MESSAGELOGCONTEXT)

# -----------------------------------------------------------------------------
# Plugins
# -----------------------------------------------------------------------------
add_subdirectory(ClusterLoader)
add_subdirectory(ClusterExporter)
