cmake_minimum_required(VERSION 3.20)

project(eagler-common VERSION 0.2.0 LANGUAGES CXX)

# Shared netplay sources compile inside the consumer target so they inherit the
# title's platform/toolchain settings and the small
# <netplay/NetplayProtocolConfig.hpp> wire-capability seam.
add_library(eagler_common_netplay_base INTERFACE)
add_library(eagler::netplay_base ALIAS eagler_common_netplay_base)

target_sources(eagler_common_netplay_base INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/netplay/NetplayProtocol.cpp>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/netplay/NetplayCore.cpp>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/netplay/NetplaySession.cpp>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/netplay/WebSocketTransport.cpp>"
)

target_include_directories(eagler_common_netplay_base INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)

target_compile_features(eagler_common_netplay_base INTERFACE cxx_std_17)

option(EAGLER_COMMON_BUILD_TESTS "Build eagler-common standalone tests" ${PROJECT_IS_TOP_LEVEL})
if(EAGLER_COMMON_BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()
