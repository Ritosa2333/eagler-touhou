cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0091 NEW)
project(th20_music_parser_oracle LANGUAGES CXX)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
add_executable(th20_music_parser_cpu_compare
 ../music_parser_cpu_compare.cpp ../music_parser.cpp ../music_reader.cpp
 ../../stone_menu/cursor.cpp ../../archive/archive.cpp ../../archive/resource_manager.cpp
 ../../runtime_core/runtime_core.cpp ../../platform_services/services.cpp
 ../../core_scheduler/scheduler.cpp ../../platform_services/configuration.cpp ../../platform_services/clock.cpp
 ../../../src/binary.cpp ../../../src/pe.cpp)
target_include_directories(th20_music_parser_cpu_compare PRIVATE ../../../native_recovered ../../../include ../../core_scheduler "${CMAKE_CURRENT_BINARY_DIR}")
target_compile_features(th20_music_parser_cpu_compare PRIVATE cxx_std_20)
target_compile_options(th20_music_parser_cpu_compare PRIVATE /W4 /utf-8 /fp:strict /arch:SSE2 /Gy)
target_link_options(th20_music_parser_cpu_compare PRIVATE /BASE:0x10000000 /DYNAMICBASE:NO /OPT:REF)
target_link_libraries(th20_music_parser_cpu_compare PRIVATE bcrypt user32 winmm)
target_compile_definitions(th20_music_parser_cpu_compare PRIVATE TH20_NATIVE_CORE_SHA256="unused" TH20_MUSIC_INTEGRATION_ORACLE=1)
get_target_property(validation_sources th20_music_parser_cpu_compare SOURCES)
list(APPEND validation_sources CMakeLists.txt ../music.hpp ../music_data.hpp ../title.hpp ../../stone_menu/cursor.hpp ../../archive/archive.hpp ../../archive/resource_manager.hpp ../../runtime_core/runtime_core.hpp ../../platform_services/services.hpp ../../../tests/native_cpu_compare.cpp)
set(hash_header "#pragma once\nstruct BoundSourceHash {const char* path;const char* hash;};\ninline constexpr BoundSourceHash bound_source_hashes[]{\n")
foreach(path IN LISTS validation_sources)
 get_filename_component(absolute "${path}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
 file(RELATIVE_PATH relative "${CMAKE_CURRENT_SOURCE_DIR}/.." "${absolute}")
 file(SHA256 "${absolute}" source_sha)
 set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${absolute}")
 string(APPEND hash_header "{\"${relative}\",\"${source_sha}\"},\n")
endforeach()
string(APPEND hash_header "};\n")
file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/music_parser_source_hashes.hpp" CONTENT "${hash_header}")
