cmake_minimum_required(VERSION 3.21)
project(th09 LANGUAGES CXX)

if(NOT WIN32 OR NOT (MSVC OR MINGW) OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "This target builds th09 for Windows x64 with MSVC or MinGW-w64. Use web/scripts/build-sdl.mjs for Web.")
endif()

find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)

set(TH_MESA_ROOT "" CACHE PATH "Directory containing mesa-devel and mesa-release")
find_path(TH09_GLES_INCLUDE GLES3/gl3.h HINTS "${TH_MESA_ROOT}/mesa-devel/include" REQUIRED)
find_library(TH09_GLES_LIBRARY NAMES GLESv2 libGLESv2.dll.a
    HINTS "${TH_MESA_ROOT}/mesa-devel/lib/x64" REQUIRED)

set(EAGLER_COMMON_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../common" CACHE PATH "eagler-common source directory")
set(TH09_LIBDATACHANNEL_SOURCE "" CACHE PATH "libdatachannel checkout for native multiplayer")
set(TH09_SWARM_TURN_HELPER "" CACHE FILEPATH "Optional Windows eagler-swarm-turn executable")
if(NOT EXISTS "${EAGLER_COMMON_ROOT}/include/eagler/netplay/SessionChannel.hpp")
    message(FATAL_ERROR "Use a checkout containing common or set EAGLER_COMMON_ROOT.")
endif()

file(GLOB TH09_SOURCES CONFIGURE_DEPENDS web/cpp/game/*.cpp web/cpp/sdl/*.cpp web/cpp/multiplayer/*.cpp)
set(NETPLAY_ROOT "${EAGLER_COMMON_ROOT}/src/netplay")
set(TH09_SOURCES_ALL
    desktop/main.cpp
    portable/sdl/Renderer.cpp
    ${TH09_SOURCES}
    "${NETPLAY_ROOT}/BrowserPeerTransport.cpp"
    "${NETPLAY_ROOT}/NetplayCore.cpp"
    "${NETPLAY_ROOT}/NetplayProtocol.cpp"
    "${NETPLAY_ROOT}/NetplaySession.cpp"
    "${NETPLAY_ROOT}/SessionChannel.cpp"
    "${NETPLAY_ROOT}/RollbackJournal.cpp"
)
add_executable(th09 ${TH09_SOURCES_ALL})
option(TH09_PROFILE_OVERLAY "Measure desktop overlay rendering in a test run" OFF)
if(TH09_PROFILE_OVERLAY)
    set_property(SOURCE portable/sdl/Renderer.cpp APPEND PROPERTY COMPILE_DEFINITIONS TH09_PROFILE_OVERLAY=1)
endif()
target_compile_features(th09 PRIVATE cxx_std_17)
target_compile_definitions(th09 PRIVATE
    NOMINMAX WIN32_LEAN_AND_MEAN TH_NATIVE_PLATFORM=1 TH_ENABLE_THCRAP=1 TH09_DEVELOPMENT_HARNESS=0 TH09_DESKTOP_MOTION_SMOOTHING=1)
target_include_directories(th09 PRIVATE
    web/cpp/sdl web/cpp/multiplayer "${EAGLER_COMMON_ROOT}/include" "${TH09_GLES_INCLUDE}")
if(MSVC)
    target_compile_options(th09 PRIVATE /utf-8 /fp:strict)
else()
    target_compile_definitions(th09 PRIVATE _WIN32_WINNT=0x0A00 NTDDI_VERSION=0x0A000000)
    target_compile_options(th09 PRIVATE -ffp-contract=off -fno-strict-aliasing)
endif()
target_link_libraries(th09 PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf "${TH09_GLES_LIBRARY}")
if(WIN32)
    target_link_libraries(th09 PRIVATE gdi32)
endif()

option(TH09_RMLUI_OVERLAY "Build the native RmlUi room overlay" ON)

if(TH09_LIBDATACHANNEL_SOURCE)
    file(STRINGS "${TH09_LIBDATACHANNEL_SOURCE}/include/rtc/configuration.hpp"
        TH09_STUN_OBSERVATIONS REGEX "^#define RTC_HAS_STUN_OBSERVATIONS 1$")
    if(NOT TH09_STUN_OBSERVATIONS)
        message(WARNING "Native th09 will build without NAT4 observations. See common/third_party/libdatachannel/README.md for the pinned patches.")
    endif()
    set(NO_MEDIA ON CACHE BOOL "Build data channels only")
    set(NO_EXAMPLES ON CACHE BOOL "Skip libdatachannel examples")
    set(NO_TESTS ON CACHE BOOL "Skip libdatachannel tests")
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static WebRTC")
    add_subdirectory("${TH09_LIBDATACHANNEL_SOURCE}" libdatachannel EXCLUDE_FROM_ALL)
    add_subdirectory("${TH09_LIBDATACHANNEL_SOURCE}/deps/json" json EXCLUDE_FROM_ALL)
    set(EAGLER_COMMON_NATIVE_WEBRTC ON)
    set(EAGLER_COMMON_NATIVE_ROOM ON)
    add_subdirectory("${EAGLER_COMMON_ROOT}" eagler-common EXCLUDE_FROM_ALL)
    target_link_libraries(th09 PRIVATE eagler::native_peer_transport nlohmann_json::nlohmann_json)
    # The standalone UDP handshake checks gameplay compatibility independently of
    # the web runtime's build ID, which the launcher supplies for lobby games.
    file(GLOB th09_udp_headers CONFIGURE_DEPENDS
        web/cpp/game/*.hpp web/cpp/multiplayer/*.hpp web/cpp/sdl/*.hpp
        "${EAGLER_COMMON_ROOT}/include/eagler/netplay/*.hpp")
    set(th09_udp_sources ${TH09_SOURCES_ALL} ${th09_udp_headers} "${NETPLAY_ROOT}/NativeUdpTransport.cpp")
    list(SORT th09_udp_sources)
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${th09_udp_sources})
    foreach(source IN LISTS th09_udp_sources)
        file(READ "${source}" contents)
        string(REPLACE "\r\n" "\n" contents "${contents}")
        string(SHA256 digest "${contents}")
        string(APPEND th09_udp_fingerprint "${digest}")
    endforeach()
    string(SHA256 th09_udp_fingerprint "${th09_udp_fingerprint}")
    string(SUBSTRING "${th09_udp_fingerprint}" 0 24 th09_udp_build)
    target_compile_definitions(th09 PRIVATE TH09_NATIVE_ROOM=1)
    set_property(SOURCE web/cpp/sdl/Application.cpp APPEND PROPERTY COMPILE_DEFINITIONS
        TH09_UDP_BUILD="${th09_udp_build}")
    get_target_property(enet_source enet SOURCE_DIR)
    add_custom_command(TARGET th09 POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${enet_source}/LICENSE" "$<TARGET_FILE_DIR:th09>/LICENSE-ENet.txt" VERBATIM)
    target_sources(th09 PRIVATE desktop/SpectatorStream.cpp)
    if(TH09_RMLUI_OVERLAY)
        include(desktop/RmlOverlay.cmake)
    else()
        include(desktop/ImGuiOverlay.cmake)
    endif()
    if(TH09_SWARM_TURN_HELPER)
        if(NOT EXISTS "${TH09_SWARM_TURN_HELPER}")
            message(FATAL_ERROR "TH09_SWARM_TURN_HELPER does not exist")
        endif()
        add_custom_command(TARGET th09 POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${TH09_SWARM_TURN_HELPER}" "$<TARGET_FILE_DIR:th09>/eagler-swarm-turn.exe")
    endif()
endif()
