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
add_subdirectory(ClusterLoaderBin)
add_subdirectory(ClusterLoaderJson)
add_subdirectory(ClusterExporterBin)
add_subdirectory(ClusterExporterJson)