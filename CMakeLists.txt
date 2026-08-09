cmake_minimum_required(VERSION 3.20)

project(th06 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(TH_EXTERNAL_ASSETS "Do not bundle original game assets" OFF)

# Keep this list explicit so source additions are visible in upstream review.
set(TH06_SOURCES
    src/AnmManager.cpp
    src/AsciiManager.cpp
    src/BombData.cpp
    src/BulletData.cpp
    src/BulletManager.cpp
    src/Chain.cpp
    src/Controller.cpp
    src/EclManager.cpp
    src/EffectManager.cpp
    src/Ending.cpp
    src/EnemyEclInstr.cpp
    src/EnemyManager.cpp
    src/FileSystem.cpp
    src/GameErrorContext.cpp
    src/GameManager.cpp
    src/GameWindow.cpp
    src/graphics/Gles.cpp
    src/Gui.cpp
    src/ItemManager.cpp
    src/MainMenu.cpp
    src/MidiOutput.cpp
    src/midi/MidiDefault.cpp
    src/MusicRoom.cpp
    src/Player.cpp
    src/ReplayManager.cpp
    src/ResultScreen.cpp
    src/Rng.cpp
    src/ScreenEffect.cpp
    src/SoundPlayer.cpp
    src/Stage.cpp
    src/Supervisor.cpp
    src/TextHelper.cpp
    src/ZunMath.cpp
    src/ZunTimer.cpp
    src/main.cpp
    src/pbg3/FileAbstraction.cpp
    src/pbg3/IPbg3Parser.cpp
    src/pbg3/Pbg3Archive.cpp
    src/pbg3/Pbg3Parser.cpp
    src/thirdparty/sjis_converter.cpp
    src/utils.cpp
)

# GLES and the SDL3 callback lifecycle are introduced during the source
# migration. Keeping legacy renderers out of this target prevents accidentally
# treating the old WebGL experiment as the new architecture.
add_executable(th06 ${TH06_SOURCES})
target_include_directories(th06 PRIVATE src src/pbg3)

if(TH_EXTERNAL_ASSETS)
    target_compile_definitions(th06 PRIVATE TH_EXTERNAL_ASSETS)
endif()

if(EMSCRIPTEN)
    if(NOT TH_EXTERNAL_ASSETS AND NOT EXISTS "${CMAKE_SOURCE_DIR}/assets")
        message(FATAL_ERROR "assets/ is required for a bundled local Web build; use -DTH_EXTERNAL_ASSETS=ON for source-only builds")
    endif()

    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    target_compile_options(th06 PRIVATE -O2)
    target_link_options(th06 PRIVATE
        -O2
        -sALLOW_MEMORY_GROWTH
        -sMAX_WEBGL_VERSION=2
        -lidbfs.js
        --use-preload-cache
        --shell-file "${CMAKE_SOURCE_DIR}/resources/shell.html"
    )
    if(NOT TH_EXTERNAL_ASSETS)
        target_link_options(th06 PRIVATE --preload-file "${CMAKE_SOURCE_DIR}/assets@/")
    endif()
endif()

# Emscripten has no maintained SDL3_image port, matching th07 reallyportable we
# vendor the complete SDL3 stack for browser builds and use it as a fallback on
# desktop platforms.
if(EMSCRIPTEN)
    set(TH_USE_VENDORED_SDL ON)
else()
    find_package(SDL3 QUIET)
    find_package(SDL3_image QUIET)
    find_package(SDL3_ttf QUIET)
    if(NOT SDL3_FOUND OR NOT SDL3_image_FOUND OR NOT SDL3_ttf_FOUND)
        set(TH_USE_VENDORED_SDL ON)
    endif()
endif()

if(TH_USE_VENDORED_SDL)
    set(SDLIMAGE_BACKEND_STB ON)
    set(SDLIMAGE_BACKEND_IMAGEIO OFF)
    set(SDLIMAGE_JPG ON)
    set(SDLIMAGE_PNG ON)
    set(SDLTTF_VENDORED ON)
    set(SDLTTF_HARFBUZZ OFF)
    add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
    add_subdirectory(vendored/SDL_image EXCLUDE_FROM_ALL)
    add_subdirectory(vendored/SDL_ttf EXCLUDE_FROM_ALL)
endif()

target_link_libraries(th06 PRIVATE SDL3::SDL3 SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf)
