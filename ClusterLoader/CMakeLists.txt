cmake_minimum_required(VERSION 3.17)

set(CLUSTERLOADER "ClusterLoader")

PROJECT(${CLUSTERLOADER})

set(CMAKE_AUTOMOC ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /DWIN32 /EHsc /MP /permissive- /Zc:__cplusplus")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

find_package(Qt6 COMPONENTS Widgets WebEngineWidgets REQUIRED)

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

add_library(${CLUSTERLOADER} SHARED ${CLUSTERLOADER_SOURCES} ${COMMON_SOURCES})

target_include_directories(${CLUSTERLOADER} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../Common/")
target_include_directories(${CLUSTERLOADER} PRIVATE "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/")

target_compile_features(${CLUSTERLOADER} PRIVATE cxx_std_17)

set(MV_LINK_PATH "${MV_INSTALL_DIR}/$<CONFIGURATION>/lib")
set(PLUGIN_LINK_PATH "${MV_INSTALL_DIR}/$<CONFIGURATION>/$<IF:$<CXX_COMPILER_ID:MSVC>,lib,Plugins>")
set(MV_LINK_SUFFIX $<IF:$<CXX_COMPILER_ID:MSVC>,${CMAKE_LINK_LIBRARY_SUFFIX},${CMAKE_SHARED_LIBRARY_SUFFIX}>)

set(MV_LINK_LIBRARY "${MV_LINK_PATH}/${CMAKE_SHARED_LIBRARY_PREFIX}MV_Public${MV_LINK_SUFFIX}")
set(POINTDATA_LINK_LIBRARY "${PLUGIN_LINK_PATH}/${CMAKE_SHARED_LIBRARY_PREFIX}PointData${MV_LINK_SUFFIX}") 
set(CLUSTERDATA_LINK_LIBRARY "${PLUGIN_LINK_PATH}/${CMAKE_SHARED_LIBRARY_PREFIX}ClusterData${MV_LINK_SUFFIX}") 

target_link_libraries(${CLUSTERLOADER} PRIVATE Qt6::Widgets)
target_link_libraries(${CLUSTERLOADER} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${CLUSTERLOADER} PRIVATE "${MV_LINK_LIBRARY}")
target_link_libraries(${CLUSTERLOADER} PRIVATE "${POINTDATA_LINK_LIBRARY}")
target_link_libraries(${CLUSTERLOADER} PRIVATE "${CLUSTERDATA_LINK_LIBRARY}")

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
        --prefix ${MV_INSTALL_DIR}/$<CONFIGURATION>
)

# Automatically set the debug environment (command + working directory) for MSVC
if(MSVC)
    set_property(TARGET ${CLUSTERLOADER} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY  $<IF:$<CONFIG:DEBUG>,${MV_INSTALL_DIR}/debug,${MV_INSTALL_DIR}/release>)
    set_property(TARGET ${CLUSTERLOADER} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,"${MV_INSTALL_DIR}/debug/ManiVault Studio.exe","${MV_INSTALL_DIR}/release/ManiVault Studio.exe">)
endif()
