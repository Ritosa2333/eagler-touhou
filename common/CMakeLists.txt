cmake_minimum_required(VERSION 3.20)

project(eagler-common VERSION 0.16.0 LANGUAGES CXX)

# Purple THPrac tools are source-level and opt-in. No ImGui/SDL dependency is
# introduced into netplay or standalone non-UI users.
add_library(eagler_common_thprac INTERFACE)
add_library(eagler::thprac ALIAS eagler_common_thprac)
target_include_directories(eagler_common_thprac INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>")
target_compile_features(eagler_common_thprac INTERFACE cxx_std_17)

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

# File codec only: input types, not a title layout or a transport dependency.
add_library(eagler_common_input_replay INTERFACE)
add_library(eagler::input_replay ALIAS eagler_common_input_replay)
target_sources(eagler_common_input_replay INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/netplay/InputReplay.cpp>"
)
target_link_libraries(eagler_common_input_replay INTERFACE eagler::netplay_headers)

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

option(EAGLER_COMMON_NATIVE_WEBRTC "Build the native WebRTC peer transport" OFF)
option(EAGLER_COMMON_NATIVE_ROOM "Build native signaling for the shared room service" OFF)
option(EAGLER_COMMON_NATIVE_UDP "Build the native UDP peer transport independently" OFF)
if(EAGLER_COMMON_NATIVE_UDP OR (EAGLER_COMMON_NATIVE_WEBRTC AND EAGLER_COMMON_NATIVE_ROOM))
    include(FetchContent)
    FetchContent_Declare(eagler_enet
        URL https://codeload.github.com/lsalzman/enet/tar.gz/refs/tags/v1.3.18
        URL_HASH SHA256=28603c895f9ed24a846478180ee72c7376b39b4bb1287b73877e5eae7d96b0dd)
    FetchContent_MakeAvailable(eagler_enet)
    add_library(eagler_common_native_udp_transport STATIC src/netplay/NativeUdpTransport.cpp)
    add_library(eagler::native_udp_transport ALIAS eagler_common_native_udp_transport)
    target_include_directories(eagler_common_native_udp_transport SYSTEM PRIVATE "${eagler_enet_SOURCE_DIR}/include")
    target_link_libraries(eagler_common_native_udp_transport PUBLIC eagler::netplay_headers PRIVATE enet)
    if(WIN32)
        target_link_libraries(eagler_common_native_udp_transport PRIVATE ws2_32 winmm)
    endif()
    if(MSVC)
        target_compile_options(eagler_common_native_udp_transport PRIVATE /utf-8)
    endif()
endif()

if(EAGLER_COMMON_NATIVE_WEBRTC)
    if(NOT TARGET LibDataChannel::LibDataChannel)
        find_package(LibDataChannel CONFIG REQUIRED)
    endif()
    add_library(eagler_common_native_peer_transport STATIC src/netplay/NativePeerTransport.cpp)
    add_library(eagler::native_peer_transport ALIAS eagler_common_native_peer_transport)
    target_link_libraries(eagler_common_native_peer_transport
        PUBLIC eagler::netplay_headers LibDataChannel::LibDataChannel)
    if(EAGLER_COMMON_NATIVE_ROOM)
        if(NOT TARGET nlohmann_json::nlohmann_json)
            find_package(nlohmann_json CONFIG REQUIRED)
        endif()
        target_sources(eagler_common_native_peer_transport PRIVATE
            src/netplay/NativeRoomTransport.cpp src/netplay/NativeAdaptiveRoute.cpp)
        target_link_libraries(eagler_common_native_peer_transport PRIVATE nlohmann_json::nlohmann_json eagler::native_udp_transport)
        if(WIN32)
            target_sources(eagler_common_native_peer_transport PRIVATE src/netplay/DesktopSwarmTurn.cpp)
            target_link_libraries(eagler_common_native_peer_transport PRIVATE crypt32 winhttp)
        endif()
    endif()
endif()

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

add_library(eagler_common_session_channel INTERFACE)
add_library(eagler::session_channel ALIAS eagler_common_session_channel)
target_sources(eagler_common_session_channel INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/netplay/SessionChannel.cpp>"
)
target_link_libraries(eagler_common_session_channel INTERFACE eagler::netplay_base)

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
