# SDL/GLES host for the current game's th12_runtime target. No second game core.
include_guard(DIRECTORY)
set(TH12_SDL_DIR "${CMAKE_CURRENT_LIST_DIR}")
option(TH12_FETCH_SDL_DEPS "Build pinned SDL3 and FreeType when not installed" OFF)
option(TH12_SWITCH_SMALL_BLOCK_POOL "Use the Switch host's small allocation pool" OFF)
option(TH12_STATIC_RUNTIME "Link the MinGW runtime into the desktop executable" ON)

if(NOT EMSCRIPTEN)
  find_package(Threads REQUIRED)
endif()
find_package(Python3 REQUIRED COMPONENTS Interpreter)

if(NINTENDO_SWITCH)
  add_library(th12_sdl_dependencies INTERFACE)
  target_include_directories(th12_sdl_dependencies INTERFACE
    "$ENV{DEVKITPRO}/portlibs/switch/include"
    "$ENV{DEVKITPRO}/portlibs/switch/include/SDL3"
    "$ENV{DEVKITPRO}/portlibs/switch/include/freetype2"
    "$ENV{DEVKITPRO}/libnx/include")
  target_link_directories(th12_sdl_dependencies INTERFACE "$ENV{DEVKITPRO}/portlibs/switch/lib" "$ENV{DEVKITPRO}/libnx/lib")
  target_link_libraries(th12_sdl_dependencies INTERFACE SDL3 freetype harfbuzz png bz2 z EGL glapi drm_nouveau GLESv2 nx m)
  target_compile_definitions(th12_runtime PUBLIC __SWITCH__ _DEFAULT_SOURCE)
else()
  find_package(SDL3 CONFIG QUIET)
  find_package(Freetype QUIET)
  if(TH12_FETCH_SDL_DEPS)
    include(FetchContent)
    if(NOT TARGET SDL3::SDL3 AND NOT TARGET SDL3::SDL3-static)
      set(SDL_SHARED OFF CACHE BOOL "" FORCE)
      set(SDL_STATIC ON CACHE BOOL "" FORCE)
      set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
      set(SDL_TESTS OFF CACHE BOOL "" FORCE)
      set(SDL_INSTALL OFF CACHE BOOL "" FORCE)
      FetchContent_Declare(th12_sdl3
        URL https://codeload.github.com/libsdl-org/SDL/tar.gz/683181b47cfabd293e3ea409f838915b8297a4fd
        URL_HASH SHA256=f5b3a3b27ae296da479ab4642c456ff620d268e65d642bdfc934aa210107593f
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
      FetchContent_MakeAvailable(th12_sdl3)
    endif()
    if(NOT TARGET Freetype::Freetype AND NOT TARGET freetype)
      set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
      set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
      set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
      set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
      set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)
      FetchContent_Declare(th12_freetype
        URL https://codeload.github.com/freetype/freetype/tar.gz/526ec5c47b9ebccc4754c85ac0c0cdf7c85a5e9b
        URL_HASH SHA256=d24f53b60034dc5c48e90d122d432a8f04a05b9e0a98c8d8e49b4a759207ce2e
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
      FetchContent_MakeAvailable(th12_freetype)
    endif()
  endif()
  if(TARGET SDL3::SDL3-static)
    set(TH12_SDL_TARGET SDL3::SDL3-static)
  elseif(TARGET SDL3::SDL3)
    set(TH12_SDL_TARGET SDL3::SDL3)
  else()
    message(FATAL_ERROR "Install SDL3 development files, or set TH12_FETCH_SDL_DEPS=ON")
  endif()
  if(TARGET Freetype::Freetype)
    set(TH12_FREETYPE_TARGET Freetype::Freetype)
  elseif(TARGET freetype)
    set(TH12_FREETYPE_TARGET freetype)
  else()
    message(FATAL_ERROR "Install FreeType, or set TH12_FETCH_SDL_DEPS=ON")
  endif()
  add_library(th12_sdl_dependencies INTERFACE)
  target_link_libraries(th12_sdl_dependencies INTERFACE ${TH12_SDL_TARGET} ${TH12_FREETYPE_TARGET})
endif()

set(TH12_CP932_TABLE "${CMAKE_CURRENT_BINARY_DIR}/generated/cp932_table.inc")
add_custom_command(OUTPUT "${TH12_CP932_TABLE}"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/generated"
  COMMAND ${Python3_EXECUTABLE} "${TH12_SDL_DIR}/scripts/gen-cp932.py" "${TH12_CP932_TABLE}"
  DEPENDS "${TH12_SDL_DIR}/scripts/gen-cp932.py" VERBATIM)
file(GLOB TH12_SDL_SOURCES CONFIGURE_DEPENDS "${TH12_SDL_DIR}/*.cpp")
list(REMOVE_ITEM TH12_SDL_SOURCES "${TH12_SDL_DIR}/main.cpp" "${TH12_SDL_DIR}/Memory.cpp")
add_library(th12_sdl_host STATIC ${TH12_SDL_SOURCES} "${TH12_CP932_TABLE}")
target_include_directories(th12_sdl_host PUBLIC "${TH12_SDL_DIR}" PRIVATE
  "${CMAKE_CURRENT_BINARY_DIR}/generated" "${TH12_SDL_DIR}/third_party/khronos" "${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb_vorbis")

target_link_libraries(th12_sdl_host PUBLIC th12_runtime th12_sdl_dependencies)
if(NOT EMSCRIPTEN)
  target_link_libraries(th12_sdl_host PUBLIC Threads::Threads)
endif()
target_compile_definitions(th12_runtime PUBLIC TH12_SDL_HOST=1)
if(MSVC)
  target_compile_options(th12_runtime PUBLIC /utf-8 /fp:strict)
  target_compile_options(th12_sdl_host PRIVATE /utf-8 /fp:strict)
else()
  target_compile_options(th12_runtime PUBLIC -ffp-contract=off -fsigned-char -fno-strict-aliasing)
  target_compile_options(th12_sdl_host PRIVATE -ffp-contract=off -fsigned-char -fno-strict-aliasing)
endif()

# MP uses the same native presentation/resource library with its own core.
# Its network session remains owned by the existing multiplayer host.
if(NOT TH12_MULTIPLAYER)
  add_executable(touhou12 "${TH12_SDL_DIR}/main.cpp")
  target_link_libraries(touhou12 PRIVATE th12_sdl_host)
  if(WIN32)
    target_link_libraries(touhou12 PRIVATE shell32)
  endif()
  if(MINGW AND TH12_STATIC_RUNTIME)
    target_link_options(touhou12 PRIVATE -static)
  endif()
  if(NINTENDO_SWITCH AND TH12_SWITCH_SMALL_BLOCK_POOL)
    target_sources(touhou12 PRIVATE "${TH12_SDL_DIR}/Memory.cpp")
  endif()
  if(EMSCRIPTEN)
    target_compile_options(th12_runtime PUBLIC -fexceptions)
    set_target_properties(touhou12 PROPERTIES SUFFIX ".mjs")
    target_link_options(touhou12 PRIVATE
      "-sMODULARIZE=1" "-sEXPORT_ES6=1" "-sEXPORT_NAME=createTH12SDL"
      "-sENVIRONMENT=web" "-sALLOW_MEMORY_GROWTH=1" "-sINITIAL_MEMORY=134217728"
      "-sMAX_WEBGL_VERSION=2" "-sMIN_WEBGL_VERSION=2" "-sINVOKE_RUN=0"
      "-sFORCE_FILESYSTEM=1" "-sEXPORTED_RUNTIME_METHODS=['callMain','FS','UTF8ToString','stringToUTF8']"
      "-sDISABLE_EXCEPTION_CATCHING=0"
      "-sEXPORTED_FUNCTIONS=['_main','_malloc','_free','_th12_host_key','_th12_host_controls','_th12_host_state','_th12_host_stop','_th12_host_audio','_th12_host_options','_th12_host_playback','_th12_host_flush','_th12_host_pointer','_th12_host_wheel','_th12_host_refresh_music','_th12_host_clear','_th12_host_mobile']")
  endif()
  install(TARGETS touhou12 RUNTIME DESTINATION .)
endif()
