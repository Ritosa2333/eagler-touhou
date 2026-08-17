cmake_minimum_required(VERSION 3.20)

project(th06 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(TH_EXTERNAL_ASSETS "Do not bundle original game assets" OFF)
option(TH_DEV_TOOLS "Enable developer-only tools (e.g. F5 logic fast-forward). Keep OFF in release builds." OFF)
option(TH_WEB_SHARED_FONT "Let the Web host install runtime fonts instead of embedding them" OFF)
option(TH_ENABLE_THPRAC "Build the archived thprac integration" OFF)
option(TH_ENABLE_THCRAP "Build the archived thcrap localization integration" OFF)
set(THPRAC_IMGUI_ROOT "${CMAKE_SOURCE_DIR}/../thprac-reallyportable/thprac/src/3rdParties/ImGui" CACHE PATH "Dear ImGui 1.82 source used by the optional desktop thprac integration")
set(TH_WEB_MUSIC "BASE" CACHE STRING "Bundled Web music payload: BASE, MIDI, WAV, or OGG")
set_property(CACHE TH_WEB_MUSIC PROPERTY STRINGS BASE MIDI WAV OGG)
set(TH06_ASSET_ROOT "${CMAKE_SOURCE_DIR}/assets" CACHE PATH "TH06 original asset directory")
set(TH06_OGG_ROOT "${CMAKE_SOURCE_DIR}/assets-ogg" CACHE PATH "TH06 generated OGG asset directory")
set(TH_UNIFONT_FILE "${CMAKE_SOURCE_DIR}/../dependencies/unifont-15.1.05/unifont-15.1.05.otf" CACHE FILEPATH "GNU Unifont OTF used by localized text and thprac")
if((TH_ENABLE_THCRAP OR TH_ENABLE_THPRAC) AND NOT EXISTS "${TH_UNIFONT_FILE}")
    message(FATAL_ERROR "GNU Unifont is required: ${TH_UNIFONT_FILE}")
endif()

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
    src/RuntimeExtension.cpp
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
    src/Touch.cpp
    src/ZunMath.cpp
    src/ZunTimer.cpp
    src/main.cpp
    src/pbg3/FileAbstraction.cpp
    src/pbg3/IPbg3Parser.cpp
    src/pbg3/Pbg3Archive.cpp
    src/pbg3/Pbg3Parser.cpp
    src/thirdparty/sjis_converter.cpp
    src/thirdparty/stb_vorbis.c
    src/utils.cpp
)

if(TH_ENABLE_THPRAC)
    list(APPEND TH06_SOURCES src/PracticeRuntime.cpp)
    if(NOT EXISTS "${THPRAC_IMGUI_ROOT}/imgui.cpp" OR NOT EXISTS "${THPRAC_IMGUI_ROOT}/imgui.h")
        message(FATAL_ERROR "TH_ENABLE_THPRAC requires Dear ImGui sources at ${THPRAC_IMGUI_ROOT}")
    endif()
    # Use the exact ImGui core shipped with the pinned thprac source.  The
    # renderer/platform backends are intentionally implemented by Eagler so
    # the game remains same-process and does not inherit the old Win32/DX9
    # injection boundary.
    list(APPEND TH06_SOURCES
        src/ThpracImGui.cpp
        "${THPRAC_IMGUI_ROOT}/imgui.cpp"
        "${THPRAC_IMGUI_ROOT}/imgui_draw.cpp"
        "${THPRAC_IMGUI_ROOT}/imgui_freetype.cpp"
        "${THPRAC_IMGUI_ROOT}/imgui_tables.cpp"
        "${THPRAC_IMGUI_ROOT}/imgui_widgets.cpp"
    )
else()
    list(APPEND TH06_SOURCES src/PracticeRuntimeStub.cpp)
endif()

if(TH_ENABLE_THCRAP)
    list(APPEND TH06_SOURCES src/Localization.cpp)
else()
    list(APPEND TH06_SOURCES src/LocalizationStub.cpp)
endif()

# The MIDI backend must match the platform header selected by src/MidiOutput.hpp.
# Compiling MidiDefault.cpp on Windows would link a MidiDevice with a different
# layout than the one MidiOutput.cpp instantiates (ODR mismatch, silent no-op MIDI).
if(EMSCRIPTEN)
    list(APPEND TH06_SOURCES src/midi/MidiWeb.cpp)
elseif(WIN32)
    list(APPEND TH06_SOURCES src/midi/MidiWin32.cpp)
elseif(APPLE)
    list(APPEND TH06_SOURCES src/midi/MidiCoreAudio.cpp)
    set(TH06_MIDI_COREAUDIO ON)
elseif(UNIX AND NOT EMSCRIPTEN)
    find_package(ALSA QUIET)
    if(ALSA_FOUND)
        list(APPEND TH06_SOURCES src/midi/MidiAlsa.cpp)
        set(TH06_MIDI_ALSA ON)
    else()
        list(APPEND TH06_SOURCES src/midi/MidiDefault.cpp)
    endif()
else()
    list(APPEND TH06_SOURCES src/midi/MidiDefault.cpp)
endif()

# GLES and the SDL3 callback lifecycle are introduced during the source
# migration. Keeping legacy renderers out of this target prevents accidentally
# treating the old WebGL experiment as the new architecture.
if(ANDROID)
    add_library(main SHARED ${TH06_SOURCES})
    set(TH_EXEC_NAME main)
else()
    add_executable(th06 ${TH06_SOURCES})
    set(TH_EXEC_NAME th06)
endif()
if(NOT EMSCRIPTEN AND NOT ANDROID AND NOT APPLE)
    add_custom_command(TARGET ${TH_EXEC_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${TH06_ASSET_ROOT}/msgothic.ttc" "$<TARGET_FILE_DIR:${TH_EXEC_NAME}>/msgothic.ttc"
        VERBATIM)
    if(TH_ENABLE_THCRAP OR TH_ENABLE_THPRAC)
        add_custom_command(TARGET ${TH_EXEC_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${TH_UNIFONT_FILE}" "$<TARGET_FILE_DIR:${TH_EXEC_NAME}>/unifont.otf"
            VERBATIM)
    endif()
endif()
target_include_directories(${TH_EXEC_NAME} PRIVATE src src/pbg3)
if(TH_ENABLE_THPRAC)
    target_compile_definitions(${TH_EXEC_NAME} PRIVATE TH_ENABLE_THPRAC)
    target_compile_definitions(${TH_EXEC_NAME} PRIVATE
        IMGUI_DISABLE_WIN32_FUNCTIONS
    )
    target_include_directories(${TH_EXEC_NAME} PRIVATE "${THPRAC_IMGUI_ROOT}")
endif()
if(TH_ENABLE_THCRAP)
    target_compile_definitions(${TH_EXEC_NAME} PRIVATE TH_ENABLE_THCRAP)
endif()

set(TH_RUNTIME_EXTENSION_CMAKE "" CACHE FILEPATH "Optional runtime extension CMake integration")
if(TH_ENABLE_THPRAC AND TH_RUNTIME_EXTENSION_CMAKE)
    include("${TH_RUNTIME_EXTENSION_CMAKE}")
    thprac_portable_attach(${TH_EXEC_NAME} th06 "${CMAKE_CURRENT_SOURCE_DIR}/src")
endif()

if(TH_EXTERNAL_ASSETS)
    target_compile_definitions(${TH_EXEC_NAME} PRIVATE TH_EXTERNAL_ASSETS)
endif()

if(TH_DEV_TOOLS)
    target_compile_definitions(${TH_EXEC_NAME} PRIVATE TH_DEV_TOOLS)
endif()

if(EMSCRIPTEN)
    if(NOT TH_EXTERNAL_ASSETS AND NOT EXISTS "${TH06_ASSET_ROOT}")
        message(FATAL_ERROR "TH06_ASSET_ROOT is required for a bundled local Web build; use -DTH_EXTERNAL_ASSETS=ON for source-only builds")
    endif()

    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    target_compile_options(th06 PRIVATE -O2)
    target_link_options(th06 PRIVATE
        -O2
        -sALLOW_MEMORY_GROWTH
        -sMAX_WEBGL_VERSION=2
        -sINVOKE_RUN=0
        "-sEXPORTED_RUNTIME_METHODS=['callMain']"
        -lidbfs.js
        --use-preload-cache
        --shell-file "${CMAKE_SOURCE_DIR}/resources/shell.html"
    )
    set_property(TARGET th06 APPEND PROPERTY LINK_DEPENDS "${CMAKE_SOURCE_DIR}/resources/shell.html")
    if(NOT TH_EXTERNAL_ASSETS)
        set(TH06_WEB_ASSET_STAGE "${CMAKE_BINARY_DIR}/web-assets-${TH_WEB_MUSIC}")
        file(REMOVE_RECURSE "${TH06_WEB_ASSET_STAGE}")
        file(MAKE_DIRECTORY "${TH06_WEB_ASSET_STAGE}")
        set(TH06_WEB_BASE_ASSETS
            紅魔郷CM.DAT
            紅魔郷ED.DAT
            紅魔郷IN.DAT
            紅魔郷MD.DAT
            紅魔郷ST.DAT
            紅魔郷TL.DAT
        )
        if(EXISTS "${TH06_ASSET_ROOT}/eagler-hitbox.png")
            list(APPEND TH06_WEB_BASE_ASSETS eagler-hitbox.png)
        endif()
        foreach(asset IN LISTS TH06_WEB_BASE_ASSETS)
            if(NOT EXISTS "${TH06_ASSET_ROOT}/${asset}")
                message(FATAL_ERROR "Missing Web asset: ${TH06_ASSET_ROOT}/${asset}")
            endif()
            file(COPY_FILE "${TH06_ASSET_ROOT}/${asset}" "${TH06_WEB_ASSET_STAGE}/${asset}")
        endforeach()
        if(NOT TH_WEB_SHARED_FONT)
            file(COPY_FILE "${TH06_ASSET_ROOT}/msgothic.ttc" "${TH06_WEB_ASSET_STAGE}/msgothic.ttc")
            if(TH_ENABLE_THCRAP OR TH_ENABLE_THPRAC)
                file(COPY_FILE "${TH_UNIFONT_FILE}" "${TH06_WEB_ASSET_STAGE}/unifont.otf")
            endif()
        endif()

        if(TH_WEB_MUSIC STREQUAL "WAV")
            if(NOT EXISTS "${TH06_ASSET_ROOT}/bgm")
                message(FATAL_ERROR "TH_WEB_MUSIC=WAV requires ${TH06_ASSET_ROOT}/bgm")
            endif()
            file(COPY "${TH06_ASSET_ROOT}/bgm" DESTINATION "${TH06_WEB_ASSET_STAGE}")
        elseif(TH_WEB_MUSIC STREQUAL "OGG")
            if(NOT EXISTS "${TH06_OGG_ROOT}/bgm")
                message(FATAL_ERROR "TH_WEB_MUSIC=OGG requires generated ${TH06_OGG_ROOT}/bgm (see PORTING.md)")
            endif()
            file(COPY "${TH06_OGG_ROOT}/bgm" DESTINATION "${TH06_WEB_ASSET_STAGE}")
        elseif(TH_WEB_MUSIC STREQUAL "MIDI" OR TH_WEB_MUSIC STREQUAL "BASE")
            # MIDI is stored in the original archives. BASE intentionally ships
            # no streamed BGM; the Web host may install WAV/OGG files at run time.
        else()
            message(FATAL_ERROR "Unsupported TH_WEB_MUSIC=${TH_WEB_MUSIC}; expected BASE, MIDI, WAV, or OGG")
        endif()
        target_link_options(th06 PRIVATE --preload-file "${TH06_WEB_ASSET_STAGE}@/")
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
    set(SDLIMAGE_AVIF OFF)
    set(SDLIMAGE_BMP OFF)
    set(SDLIMAGE_GIF OFF)
    set(SDLIMAGE_JXL OFF)
    set(SDLIMAGE_LBM OFF)
    set(SDLIMAGE_PCX OFF)
    set(SDLIMAGE_PNM OFF)
    set(SDLIMAGE_QOI OFF)
    set(SDLIMAGE_SVG OFF)
    set(SDLIMAGE_TGA OFF)
    set(SDLIMAGE_TIF OFF)
    set(SDLIMAGE_WEBP OFF)
    set(SDLIMAGE_XCF OFF)
    set(SDLIMAGE_XPM OFF)
    set(SDLIMAGE_XV OFF)
    set(SDLIMAGE_JPG ON)
    set(SDLIMAGE_PNG ON)
    set(SDLTTF_VENDORED ON)
    set(SDLTTF_HARFBUZZ OFF)
    add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
    add_subdirectory(vendored/SDL_image EXCLUDE_FROM_ALL)
    add_subdirectory(vendored/SDL_ttf EXCLUDE_FROM_ALL)
endif()

if(TH_ENABLE_THPRAC AND TH_USE_VENDORED_SDL)
    if(NOT TARGET Freetype::Freetype)
        message(FATAL_ERROR "TH_ENABLE_THPRAC requires the vendored Freetype target for ImGui's pinned FreeType atlas builder")
    endif()
    # The archived ImGui configuration enables its FreeType builder. SDL_ttf
    # keeps that dependency private, so expose the same vendored target only
    # to the optional thprac build.
    target_include_directories(${TH_EXEC_NAME} PRIVATE
        $<TARGET_PROPERTY:Freetype::Freetype,INTERFACE_INCLUDE_DIRECTORIES>
    )
    target_link_libraries(${TH_EXEC_NAME} PRIVATE Freetype::Freetype)
endif()

target_link_libraries(${TH_EXEC_NAME} PRIVATE SDL3::SDL3 SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf)

if(TH06_MIDI_ALSA)
    target_compile_definitions(${TH_EXEC_NAME} PRIVATE LIBASOUND_MIDI_SUPPORT)
    target_link_libraries(${TH_EXEC_NAME} PRIVATE ALSA::ALSA)
elseif(TH06_MIDI_COREAUDIO)
    target_compile_definitions(${TH_EXEC_NAME} PRIVATE COREAUDIO_MIDI_SUPPORT)
    target_link_libraries(${TH_EXEC_NAME} PRIVATE "-framework AudioToolbox" "-framework AudioUnit")
endif()

if(APPLE)
    enable_language(OBJC)
    enable_language(OBJCXX)

    set(TH06_APP_RESOURCES
        "${CMAKE_SOURCE_DIR}/assets/msgothic.ttc"
        "${CMAKE_SOURCE_DIR}/assets/紅魔郷CM.DAT"
        "${CMAKE_SOURCE_DIR}/assets/紅魔郷ED.DAT"
        "${CMAKE_SOURCE_DIR}/assets/紅魔郷IN.DAT"
        "${CMAKE_SOURCE_DIR}/assets/紅魔郷MD.DAT"
        "${CMAKE_SOURCE_DIR}/assets/紅魔郷ST.DAT"
        "${CMAKE_SOURCE_DIR}/assets/紅魔郷TL.DAT"
    )
    if(TH_ENABLE_THCRAP OR TH_ENABLE_THPRAC)
        list(APPEND TH06_APP_RESOURCES "${TH_UNIFONT_FILE}")
    endif()
    file(GLOB TH06_BGM_RESOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/assets/bgm/*")
    if(NOT TH_EXTERNAL_ASSETS)
        target_sources(${TH_EXEC_NAME} PRIVATE ${TH06_APP_RESOURCES} ${TH06_BGM_RESOURCES})
        set_source_files_properties(${TH06_APP_RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
        set_source_files_properties(${TH06_BGM_RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/bgm")
    endif()
    if(IOS OR NOT TH_EXTERNAL_ASSETS)
        set_target_properties(${TH_EXEC_NAME} PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_BUNDLE_NAME "東方紅魔郷"
            MACOSX_BUNDLE_GUI_IDENTIFIER "com.zun.th06"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.0"
        )
    endif()
    if(IOS)
        set_target_properties(${TH_EXEC_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/ios/Info.plist")
    endif()
endif()

if(WIN32 AND NOT EMSCRIPTEN)
    set(TH_MESA_ROOT "${CMAKE_SOURCE_DIR}/../dependencies")
    target_include_directories(${TH_EXEC_NAME} PRIVATE "${TH_MESA_ROOT}/mesa-devel/include")
    target_link_libraries(${TH_EXEC_NAME} PRIVATE
        "${TH_MESA_ROOT}/mesa-devel/lib/x64/libEGL.dll.a"
        "${TH_MESA_ROOT}/mesa-devel/lib/x64/libGLESv2.dll.a"
        winmm
    )
    if(TH_ENABLE_THCRAP)
        target_link_libraries(${TH_EXEC_NAME} PRIVATE gdi32)
    endif()
elseif(ANDROID)
    target_link_libraries(${TH_EXEC_NAME} PRIVATE GLESv3)
elseif(APPLE)
    target_compile_definitions(${TH_EXEC_NAME} PRIVATE USING_GL)
    if(IOS)
        target_link_libraries(${TH_EXEC_NAME} PRIVATE
            "-framework OpenGLES" "-framework AVFoundation" "-framework CoreAudio"
            "-framework CoreGraphics" "-framework QuartzCore" "-framework UIKit"
            "-framework Foundation"
        )
    else()
        find_package(OpenGL REQUIRED)
        target_link_libraries(${TH_EXEC_NAME} PRIVATE
            OpenGL::GL
            "-framework CoreAudio" "-framework CoreVideo" "-framework Cocoa"
            "-framework IOKit" "-framework ForceFeedback" "-framework Carbon"
        )
    endif()
elseif(NOT EMSCRIPTEN)
    find_package(OpenGL COMPONENTS GLES3 REQUIRED)
    target_link_libraries(${TH_EXEC_NAME} PRIVATE OpenGL::GLES3)
endif()
