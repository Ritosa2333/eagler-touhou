cmake_minimum_required(VERSION 3.20)
project(th12_runtime LANGUAGES CXX)

file(GLOB TH12_GAME_SOURCES CONFIGURE_DEPENDS src/game/*.cpp)
add_library(th12_runtime STATIC ${TH12_GAME_SOURCES} src/wasm/Exports.cpp)
target_compile_features(th12_runtime PUBLIC cxx_std_20)
target_include_directories(th12_runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
set_target_properties(th12_runtime PROPERTIES CXX_EXTENSIONS OFF)
include(CTest)
if(BUILD_TESTING AND NOT EMSCRIPTEN)
  add_executable(th12_native_smoke tests/native/runtime-smoke.cpp)
  target_link_libraries(th12_native_smoke PRIVATE th12_runtime)
  add_test(NAME th12_native_smoke COMMAND th12_native_smoke)
endif()

# Native hosts can link the same logical core. The browser build uses
# scripts/build.mjs for its pinned Emscripten version and stable export list.
