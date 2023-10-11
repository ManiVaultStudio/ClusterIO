cmake_minimum_required(VERSION 3.17)

set(PROJECT "ClusterIO")

PROJECT(${PROJECT})

add_definitions(-DQT_MESSAGELOGCONTEXT)

# Check if the directory to the ManiVault installation has been provided
if(NOT DEFINED MV_INSTALL_DIR)
    set(MV_INSTALL_DIR "" CACHE PATH "Directory where ManiVault is installed")
    message(FATAL_ERROR "Please set MV_INSTALL_DIR to the directory where ManiVault is installed")
endif()
file(TO_CMAKE_PATH ${MV_INSTALL_DIR} MV_INSTALL_DIR)

add_subdirectory(ClusterLoader)
add_subdirectory(ClusterExporter)