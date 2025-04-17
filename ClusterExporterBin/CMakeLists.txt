cmake_minimum_required(VERSION 3.17)

# -----------------------------------------------------------------------------
# Cluster Writer Plugin
# -----------------------------------------------------------------------------
set(CLUSTER_EXPORTER_BIN "ClusterExporterBin")
PROJECT(${CLUSTER_EXPORTER_BIN})

# -----------------------------------------------------------------------------
# CMake Options
# -----------------------------------------------------------------------------
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

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
set(CLUSTER_EXPORTER_BIN_SOURCES
    src/ClusterExporterBin.h
    src/ClusterExporterBin.cpp
    src/ClusterExporterBin.json
)

set(COMMON_SOURCES
    ../Common/ClusterUtils.h
)

source_group(Plugin FILES ${CLUSTER_EXPORTER_BIN_SOURCES})
source_group(Common FILES ${COMMON_SOURCES})

# -----------------------------------------------------------------------------
# CMake Target
# -----------------------------------------------------------------------------
add_library(${CLUSTER_EXPORTER_BIN} SHARED ${CLUSTER_EXPORTER_BIN_SOURCES} ${COMMON_SOURCES})

# -----------------------------------------------------------------------------
# Target include directories
# -----------------------------------------------------------------------------
target_include_directories(${CLUSTER_EXPORTER_BIN} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../Common/")
target_include_directories(${CLUSTER_EXPORTER_BIN} PRIVATE "${ManiVault_INCLUDE_DIR}")

# -----------------------------------------------------------------------------
# Target properties
# -----------------------------------------------------------------------------
target_compile_features(${CLUSTER_EXPORTER_BIN} PRIVATE cxx_std_20)

# -----------------------------------------------------------------------------
# Target library linking
# -----------------------------------------------------------------------------
target_link_libraries(${CLUSTER_EXPORTER_BIN} PRIVATE Qt6::Widgets)
target_link_libraries(${CLUSTER_EXPORTER_BIN} PRIVATE Qt6::WebEngineWidgets)

target_link_libraries(${CLUSTER_EXPORTER_BIN} PRIVATE ManiVault::Core)
target_link_libraries(${CLUSTER_EXPORTER_BIN} PRIVATE ManiVault::PointData)
target_link_libraries(${CLUSTER_EXPORTER_BIN} PRIVATE ManiVault::ClusterData)

# -----------------------------------------------------------------------------
# Target installation
# -----------------------------------------------------------------------------
install(TARGETS ${CLUSTER_EXPORTER_BIN}
    RUNTIME DESTINATION Plugins COMPONENT PLUGIN # Windows .dll
    LIBRARY DESTINATION Plugins COMPONENT PLUGIN # Linux/Mac .so
)

add_custom_command(TARGET ${CLUSTER_EXPORTER_BIN} POST_BUILD
    DEPENDS ClusterExporterBin
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
    set_property(TARGET ${CLUSTER_EXPORTER_BIN} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY  $<IF:$<CONFIG:DEBUG>,${ManiVault_INSTALL_DIR}/debug,${ManiVault_INSTALL_DIR}/release>)
    set_property(TARGET ${CLUSTER_EXPORTER_BIN} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,"${ManiVault_INSTALL_DIR}/debug/ManiVault Studio.exe","${ManiVault_INSTALL_DIR}/release/ManiVault Studio.exe">)
endif()
