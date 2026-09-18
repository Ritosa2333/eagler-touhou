cmake_minimum_required(VERSION 3.20)

project(eagler-common VERSION 0.6.0 LANGUAGES CXX)

# Consumers such as Replay metadata readers need the shared protocol types even
# in ordinary (non-netplay) builds. Keep that dependency header-only so title
# builds do not accidentally pull transport/core implementation objects into
# their normal runtime.
add_library(eagler_common_netplay_headers INTERFACE)
add_library(eagler::netplay_headers ALIAS eagler_common_netplay_headers)

target_include_directories(eagler_common_netplay_headers INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)

target_compile_features(eagler_common_netplay_headers INTERFACE cxx_std_17)

# Generic rollback storage is a separate component so future titles and tools
# can consume the validated storage layer without pulling Protocol/Core or any
# transport implementation into the target.
add_library(eagler_common_rollback INTERFACE)
add_library(eagler::rollback ALIAS eagler_common_rollback)

target_sources(eagler_common_rollback INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/netplay/RollbackJournal.cpp>"
)

target_link_libraries(eagler_common_rollback INTERFACE
    eagler::netplay_headers
)

# Browser peer networking is deliberately separate from netplay_base. Titles
# that use another transport can keep Protocol/Core without inheriting WebRTC
# or relay implementation sources. The consumer provides only the stable
# <netplay/NetplayTransportConfig.hpp> tag seam used for channel/global names.
add_library(eagler_common_browser_peer_transport INTERFACE)
add_library(eagler::browser_peer_transport ALIAS eagler_common_browser_peer_transport)

target_sources(eagler_common_browser_peer_transport INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/netplay/BrowserPeerTransport.cpp>"
)

target_link_libraries(eagler_common_browser_peer_transport INTERFACE
    eagler::netplay_headers
)

# Logical input capture/replay is independent of transport and Protocol/Core.
# Keep it as its own component so title multiplayer gameplay can consume the
# validated input ownership layer without pulling the full netplay runtime.
# The consumer provides only <netplay/NetplayInputConfig.hpp>, which commits
# synchronized logical button lanes into title-owned gameplay state.
add_library(eagler_common_netplay_input INTERFACE)
add_library(eagler::netplay_input ALIAS eagler_common_netplay_input)

target_sources(eagler_common_netplay_input INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/netplay/NetplayInput.cpp>"
)

target_link_libraries(eagler_common_netplay_input INTERFACE
    eagler::netplay_headers
)

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

target_link_libraries(eagler_common_netplay_base INTERFACE
    eagler::rollback
)

option(EAGLER_COMMON_BUILD_TESTS "Build eagler-common standalone tests" ${PROJECT_IS_TOP_LEVEL})
if(EAGLER_COMMON_BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()
