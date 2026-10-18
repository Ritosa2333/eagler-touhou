cmake_minimum_required(VERSION 3.15)
project(asset_extract_index C)
set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)
get_filename_component(ASSET_PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
add_executable(asset_extract_index ../asset_extract_index.c)
target_include_directories(asset_extract_index PRIVATE
    "${ASSET_PROJECT_ROOT}/third_party/thtk"
    "${ASSET_PROJECT_ROOT}/third_party/thtk/build")
target_compile_definitions(asset_extract_index PRIVATE _CRT_SECURE_NO_WARNINGS THTK_STATIC_DEFINE)
target_link_libraries(asset_extract_index PRIVATE "${ASSET_PROJECT_ROOT}/third_party/thtk/build/thtk/Release/thtk.lib")
