cmake_minimum_required(VERSION 3.17)

# -----------------------------------------------------------------------------
# Cluster Writer Plugin
# -----------------------------------------------------------------------------
set(CLUSTER_EXPORTER_JSON "ClusterExporterJson")
PROJECT(${CLUSTER_EXPORTER_JSON}
        LANGUAGES CXX)

# -----------------------------------------------------------------------------
# CMake Options
# -----------------------------------------------------------------------------
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /DWIN32 /EHsc /MP /permissive- /Zc:__cplusplus")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MD")
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
set(CLUSTER_EXPORTER_JSON_SOURCES
    src/ClusterExporterJson.h
    src/ClusterExporterJson.cpp
    PluginInfo.json
)

source_group(Plugin FILES ${CLUSTER_EXPORTER_JSON_SOURCES})

# -----------------------------------------------------------------------------
# CMake Target
# -----------------------------------------------------------------------------
add_library(${CLUSTER_EXPORTER_JSON} SHARED ${CLUSTER_EXPORTER_JSON_SOURCES})

# -----------------------------------------------------------------------------
# Target include directories
# -----------------------------------------------------------------------------
target_include_directories(${CLUSTER_EXPORTER_JSON} PRIVATE "${ManiVault_INCLUDE_DIR}")

# -----------------------------------------------------------------------------
# Target properties
# -----------------------------------------------------------------------------
target_compile_features(${CLUSTER_EXPORTER_JSON} PRIVATE cxx_std_20)

# -----------------------------------------------------------------------------
# Target library linking
# -----------------------------------------------------------------------------
target_link_libraries(${CLUSTER_EXPORTER_JSON} PRIVATE Qt6::Widgets)
target_link_libraries(${CLUSTER_EXPORTER_JSON} PRIVATE Qt6::WebEngineWidgets)

target_link_libraries(${CLUSTER_EXPORTER_JSON} PRIVATE ManiVault::Core)
target_link_libraries(${CLUSTER_EXPORTER_JSON} PRIVATE ManiVault::PointData)
target_link_libraries(${CLUSTER_EXPORTER_JSON} PRIVATE ManiVault::ClusterData)

# -----------------------------------------------------------------------------
# Target installation
# -----------------------------------------------------------------------------
install(TARGETS ${CLUSTER_EXPORTER_JSON}
    RUNTIME DESTINATION Plugins COMPONENT PLUGIN # Windows .dll
    LIBRARY DESTINATION Plugins COMPONENT PLUGIN # Linux/Mac .so
)

add_custom_command(TARGET ${CLUSTER_EXPORTER_JSON} POST_BUILD
    DEPENDS ClusterExporterJson
    COMMAND "${CMAKE_COMMAND}"
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --component PLUGIN
        --prefix ${ManiVault_INSTALL_DIR}/$<CONFIGURATION>
)

mv_handle_plugin_config(${CLUSTER_EXPORTER_JSON})

# -----------------------------------------------------------------------------
# Miscellaneous
# -----------------------------------------------------------------------------
# Automatically set the debug environment (command + working directory) for MSVC
if(MSVC)
    set_property(TARGET ${BINEXPORTER} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<IF:$<CONFIG:DEBUG>,${ManiVault_INSTALL_DIR}/Debug,$<IF:$<CONFIG:RELWITHDEBINFO>,${ManiVault_INSTALL_DIR}/RelWithDebInfo,${ManiVault_INSTALL_DIR}/Release>>)
    set_property(TARGET ${BINEXPORTER} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,"${ManiVault_INSTALL_DIR}/Debug/ManiVault Studio.exe",$<IF:$<CONFIG:RELWITHDEBINFO>,"${ManiVault_INSTALL_DIR}/RelWithDebInfo/ManiVault Studio.exe","${ManiVault_INSTALL_DIR}/Release/ManiVault Studio.exe">>)
endif()
