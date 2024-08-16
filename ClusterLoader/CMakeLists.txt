cmake_minimum_required(VERSION 3.17)

# -----------------------------------------------------------------------------
# Cluster Loader Plugin
# -----------------------------------------------------------------------------
set(CLUSTERLOADER "ClusterLoader")
PROJECT(${CLUSTERLOADER})

# -----------------------------------------------------------------------------
# CMake Options
# -----------------------------------------------------------------------------
set(CMAKE_AUTOMOC ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /DWIN32 /EHsc /MP /permissive- /Zc:__cplusplus")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------
find_package(Qt6 COMPONENTS Widgets WebEngineWidgets REQUIRED)

find_package(ManiVault COMPONENTS Core PointData ClusterData CONFIG)

# -----------------------------------------------------------------------------
# Source files
# -----------------------------------------------------------------------------
set(CLUSTERLOADER_SOURCES
    src/ClusterLoader.h
    src/ClusterLoader.cpp
    src/ClusterLoader.json
)

set(COMMON_SOURCES
    ../Common/ClusterUtils.h
)

source_group(Plugin FILES ${CLUSTERLOADER_SOURCES})
source_group(Common FILES ${COMMON_SOURCES})

# -----------------------------------------------------------------------------
# CMake Target
# -----------------------------------------------------------------------------
add_library(${CLUSTERLOADER} SHARED ${CLUSTERLOADER_SOURCES} ${COMMON_SOURCES})

# -----------------------------------------------------------------------------
# Target include directories
# -----------------------------------------------------------------------------
target_include_directories(${CLUSTERLOADER} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../Common/")
target_include_directories(${CLUSTERLOADER} PRIVATE "${ManiVault_INCLUDE_DIR}")

# -----------------------------------------------------------------------------
# Target properties
# -----------------------------------------------------------------------------
target_compile_features(${CLUSTERLOADER} PRIVATE cxx_std_17)

# -----------------------------------------------------------------------------
# Target library linking
# -----------------------------------------------------------------------------
target_link_libraries(${CLUSTERLOADER} PRIVATE Qt6::Widgets)
target_link_libraries(${CLUSTERLOADER} PRIVATE Qt6::WebEngineWidgets)

target_link_libraries(${CLUSTERLOADER} PRIVATE ManiVault::Core)
target_link_libraries(${CLUSTERLOADER} PRIVATE ManiVault::PointData)
target_link_libraries(${CLUSTERLOADER} PRIVATE ManiVault::ClusterData)

# -----------------------------------------------------------------------------
# Target installation
# -----------------------------------------------------------------------------
install(TARGETS ${CLUSTERLOADER}
    RUNTIME DESTINATION Plugins COMPONENT PLUGIN # Windows .dll
    LIBRARY DESTINATION Plugins COMPONENT PLUGIN # Linux/Mac .so
)

add_custom_command(TARGET ${CLUSTERLOADER} POST_BUILD
    DEPENDS ClusterLoader
    COMMAND "${CMAKE_COMMAND}"
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --component PLUGIN
        --prefix ${ManiVault_INSTALL_DIR}/$<CONFIGURATION>
)

# -----------------------------------------------------------------------------
# Miscellaneous
# -----------------------------------------------------------------------------
# Automatically set the debug environment (command + working directory) for MSVC
if(MSVC)
    set_property(TARGET ${CLUSTERLOADER} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY  $<IF:$<CONFIG:DEBUG>,${ManiVault_INSTALL_DIR}/debug,${ManiVault_INSTALL_DIR}/release>)
    set_property(TARGET ${CLUSTERLOADER} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,"${ManiVault_INSTALL_DIR}/debug/ManiVault Studio.exe","${ManiVault_INSTALL_DIR}/release/ManiVault Studio.exe">)
endif()
