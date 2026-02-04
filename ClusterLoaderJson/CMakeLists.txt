cmake_minimum_required(VERSION 3.17)

# -----------------------------------------------------------------------------
# Cluster Loader Plugin
# -----------------------------------------------------------------------------
set(CLUSTER_LOADER_JSON "ClusterLoaderJson")
PROJECT(${CLUSTER_LOADER_JSON}
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
set(CLUSTER_LOADER_JSON_SOURCES
    src/ClusterLoaderJson.h
    src/ClusterLoaderJson.cpp
    PluginInfo.json
)

set(COMMON_SOURCES
    ../Common/ClusterUtils.h
	../Common/LoaderInputDialog.h
	../Common/LoaderInputDialog.cpp
)

source_group(Plugin FILES ${CLUSTER_LOADER_JSON_SOURCES})
source_group(Common FILES ${COMMON_SOURCES})

# -----------------------------------------------------------------------------
# CMake Target
# -----------------------------------------------------------------------------
add_library(${CLUSTER_LOADER_JSON} SHARED ${CLUSTER_LOADER_JSON_SOURCES} ${COMMON_SOURCES})

# -----------------------------------------------------------------------------
# Target include directories
# -----------------------------------------------------------------------------
target_include_directories(${CLUSTER_LOADER_JSON} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../Common/")
target_include_directories(${CLUSTER_LOADER_JSON} PRIVATE "${ManiVault_INCLUDE_DIR}")

# -----------------------------------------------------------------------------
# Target properties
# -----------------------------------------------------------------------------
target_compile_features(${CLUSTER_LOADER_JSON} PRIVATE cxx_std_20)

# -----------------------------------------------------------------------------
# Target library linking
# -----------------------------------------------------------------------------
target_link_libraries(${CLUSTER_LOADER_JSON} PRIVATE Qt6::Widgets)
target_link_libraries(${CLUSTER_LOADER_JSON} PRIVATE Qt6::WebEngineWidgets)

target_link_libraries(${CLUSTER_LOADER_JSON} PRIVATE ManiVault::Core)
target_link_libraries(${CLUSTER_LOADER_JSON} PRIVATE ManiVault::PointData)
target_link_libraries(${CLUSTER_LOADER_JSON} PRIVATE ManiVault::ClusterData)

# -----------------------------------------------------------------------------
# Target installation
# -----------------------------------------------------------------------------
install(TARGETS ${CLUSTER_LOADER_JSON}
    RUNTIME DESTINATION Plugins COMPONENT PLUGIN # Windows .dll
    LIBRARY DESTINATION Plugins COMPONENT PLUGIN # Linux/Mac .so
)

add_custom_command(TARGET ${CLUSTER_LOADER_JSON} POST_BUILD
    DEPENDS ClusterLoaderJson
    COMMAND "${CMAKE_COMMAND}"
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --component PLUGIN
        --prefix ${ManiVault_INSTALL_DIR}/$<CONFIGURATION>
)

mv_handle_plugin_config(${CLUSTER_LOADER_JSON})

# -----------------------------------------------------------------------------
# Miscellaneous
# -----------------------------------------------------------------------------
# Automatically set the debug environment (command + working directory) for MSVC
if(MSVC)
    set_property(TARGET ${BINLOADER} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<IF:$<CONFIG:DEBUG>,${ManiVault_INSTALL_DIR}/Debug,$<IF:$<CONFIG:RELWITHDEBINFO>,${ManiVault_INSTALL_DIR}/RelWithDebInfo,${ManiVault_INSTALL_DIR}/Release>>)
    set_property(TARGET ${BINLOADER} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,"${ManiVault_INSTALL_DIR}/Debug/ManiVault Studio.exe",$<IF:$<CONFIG:RELWITHDEBINFO>,"${ManiVault_INSTALL_DIR}/RelWithDebInfo/ManiVault Studio.exe","${ManiVault_INSTALL_DIR}/Release/ManiVault Studio.exe">>)
endif()