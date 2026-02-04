cmake_minimum_required(VERSION 3.22)

# -----------------------------------------------------------------------------
# ClusterIO plugins
# -----------------------------------------------------------------------------
set(PROJECT "ClusterIO")
PROJECT(${PROJECT}
        LANGUAGES CXX
)

add_definitions(-DQT_MESSAGELOGCONTEXT)

# -----------------------------------------------------------------------------
# Plugins
# -----------------------------------------------------------------------------
add_subdirectory(ClusterLoaderBin)
add_subdirectory(ClusterLoaderJson)
add_subdirectory(ClusterExporterBin)
add_subdirectory(ClusterExporterJson)