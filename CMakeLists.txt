cmake_minimum_required(VERSION 3.20)

project(th07 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(TH_EXTERNAL_ASSETS "Do not bundle assets (on platforms that bundle them) and use the user/executable directories instead" OFF)
option(TH_WEB_SHARED_FONT "Let the Web host install runtime fonts instead of embedding them" OFF)
option(TH_ENABLE_THPRAC "Build the archived thprac integration" OFF)
option(TH_ENABLE_THCRAP "Build the archived thcrap localization integration" OFF)
option(TH_ENABLE_NETPLAY "Build the isolated TH07 rollback-netplay core" OFF)
option(TH_ENABLE_MULTIPLAYER_GAMEPLAY "Build TH07 multiplayer gameplay without requiring a transport" OFF)
option(TH_DEV_TOOLS "Enable developer-only tools such as F5 fast-forward" OFF)
set(THPRAC_IMGUI_ROOT "${CMAKE_SOURCE_DIR}/../thprac-reallyportable/thprac/src/3rdParties/ImGui" CACHE PATH "Dear ImGui 1.82 source used by the desktop thprac integration")
set(TH_WEB_MUSIC "BASE" CACHE STRING "Bundled Web music payload: BASE, MIDI, WAV, or OGG")
set_property(CACHE TH_WEB_MUSIC PROPERTY STRINGS BASE MIDI WAV OGG)
set(TH07_ASSET_ROOT "${CMAKE_SOURCE_DIR}/assets" CACHE PATH "TH07 original asset directory")
set(TH07_OGG_ROOT "${CMAKE_SOURCE_DIR}/assets-ogg" CACHE PATH "TH07 generated OGG asset directory")
set(TH_UNIFONT_FILE "${CMAKE_SOURCE_DIR}/../dependencies/unifont-15.1.05/unifont-15.1.05.otf" CACHE FILEPATH "GNU Unifont OTF used by localized text and thprac")
if((TH_ENABLE_THCRAP OR TH_ENABLE_THPRAC) AND NOT EXISTS "${TH_UNIFONT_FILE}")
    message(FATAL_ERROR "GNU Unifont is required: ${TH_UNIFONT_FILE}")
endif()

set(SOURCES
    src/AnmManager.cpp
    src/AnmVm.cpp
    src/AsciiManager.cpp
    src/BombData.cpp
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
    src/Gui.cpp
    src/ItemManager.cpp
    src/MainMenu.cpp
    src/MidiOutput.cpp
    src/MusicRoom.cpp
    src/Player.cpp
    src/ReplayExtension.cpp
    src/ReplayManager.cpp
    src/ResultScreen.cpp
    src/Rng.cpp
    src/ScreenEffect.cpp
    src/SoundPlayer.cpp
    src/Stage.cpp
    src/Supervisor.cpp
    src/TextHelper.cpp
    src/Touch.cpp
    src/main.cpp
    src/graphics/Gles.cpp
    src/pbg4/Lzss.cpp
    src/pbg4/Pbg4Archive.cpp
    src/pbg4/Pbg4File.cpp
    src/sj2utf8/sj2utf8.cpp
    src/utils.cpp
)

if(TH_ENABLE_THPRAC)
    list(APPEND SOURCES src/PracticeRuntime.cpp)
    if(NOT EXISTS "${THPRAC_IMGUI_ROOT}/imgui.cpp" OR NOT EXISTS "${THPRAC_IMGUI_ROOT}/imgui.h")
        message(FATAL_ERROR "TH_ENABLE_THPRAC requires Dear ImGui sources at ${THPRAC_IMGUI_ROOT}")
    endif()
    list(APPEND SOURCES
        src/ThpracImGui.cpp
        "${THPRAC_IMGUI_ROOT}/imgui.cpp"
        "${THPRAC_IMGUI_ROOT}/imgui_draw.cpp"
        "${THPRAC_IMGUI_ROOT}/imgui_freetype.cpp"
        "${THPRAC_IMGUI_ROOT}/imgui_tables.cpp"
        "${THPRAC_IMGUI_ROOT}/imgui_widgets.cpp"
    )
else()
    list(APPEND SOURCES src/PracticeRuntimeStub.cpp)
endif()

if(TH_ENABLE_THCRAP)
    list(APPEND SOURCES src/Localization.cpp)
else()
    list(APPEND SOURCES src/LocalizationStub.cpp)
endif()

if(TH_ENABLE_NETPLAY OR TH_ENABLE_MULTIPLAYER_GAMEPLAY)
    list(APPEND SOURCES
        src/multiplayer/GameplaySession.cpp
        src/MultiplayerResources.cpp
    )
endif()

set(EAGLER_COMMON_ROOT "${CMAKE_SOURCE_DIR}/third_party/eagler-common"
    CACHE PATH "Pinned eagler-common checkout; override explicitly for development only")
if(NOT EXISTS "${EAGLER_COMMON_ROOT}/cmake/EaglerCommon.cmake")
    message(FATAL_ERROR "TH07 requires the pinned eagler-common checkout at ${EAGLER_COMMON_ROOT}")
endif()
include("${EAGLER_COMMON_ROOT}/cmake/EaglerCommon.cmake")

if(TH_ENABLE_NETPLAY)
    list(APPEND SOURCES
        src/netplay/NetplaySideEffects.cpp
        src/netplay/Th07CanonicalHash.cpp
        src/netplay/Th07DeterminismProbe.cpp
        src/netplay/Th07LanCoreProbe.cpp
        src/netplay/Th07LanStageProbe.cpp
        src/netplay/Th07RollbackProbe.cpp
        src/netplay/Th07RollbackState.cpp
    )
endif()

if(EMSCRIPTEN)
    list(APPEND SOURCES src/midi/MidiWeb.cpp)
elseif(WIN32)
    list(APPEND SOURCES src/midi/MidiWin32.cpp)
elseif(APPLE)
    list(APPEND SOURCES src/midi/MidiCoreAudio.cpp)
    set(TH07_MIDI_COREAUDIO ON)
elseif(UNIX)
    find_package(ALSA QUIET)
    if(ALSA_FOUND)
        list(APPEND SOURCES src/midi/MidiAlsa.cpp)
        set(TH07_MIDI_ALSA ON)
    else()
        list(APPEND SOURCES src/midi/MidiDefault.cpp)
    endif()
else()
    list(APPEND SOURCES src/midi/MidiDefault.cpp)
endif()

if(ANDROID)
    add_library(main SHARED ${SOURCES})
    set(TH_EXEC_NAME "main")
else()
    add_executable(th07 ${SOURCES})
    set(TH_EXEC_NAME "th07")
endif()
if(NOT EMSCRIPTEN AND NOT ANDROID AND NOT APPLE)
    add_custom_command(TARGET ${TH_EXEC_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${TH07_ASSET_ROOT}/msgothic.ttc" "$<TARGET_FILE_DIR:${TH_EXEC_NAME}>/msgothic.ttc"
        VERBATIM)
    if(TH_ENABLE_THCRAP OR TH_ENABLE_THPRAC)
        add_custom_command(TARGET ${TH_EXEC_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${TH_UNIFONT_FILE}" "$<TARGET_FILE_DIR:${TH_EXEC_NAME}>/unifont.otf"
            VERBATIM)
    endif()
endif()

target_include_directories(${TH_EXEC_NAME} PRIVATE src src/pbg4)
if(TH_ENABLE_NETPLAY)
    eagler_common_link_netplay_base(${TH_EXEC_NAME})
    eagler_common_link_browser_peer_transport(${TH_EXEC_NAME})
    eagler_common_link_netplay_input(${TH_EXEC_NAME})
else()
    eagler_common_link_netplay_headers(${TH_EXEC_NAME})
endif()
if(TH_ENABLE_THPRAC)
    target_compile_definitions(${TH_EXEC_NAME} PRIVATE TH_ENABLE_THPRAC)
    target_compile_definitions(${TH_EXEC_NAME} PRIVATE IMGUI_DISABLE_WIN32_FUNCTIONS)
    target_include_directories(${TH_EXEC_NAME} PRIVATE "${THPRAC_IMGUI_ROOT}")
endif()
if(TH_ENABLE_THCRAP)
    target_compile_definitions(${TH_EXEC_NAME} PRIVATE TH_ENABLE_THCRAP)
endif()
if(TH_ENABLE_NETPLAY OR TH_ENABLE_MULTIPLAYER_GAMEPLAY)
    target_compile_definitions(${TH_EXEC_NAME} PRIVATE TH_ENABLE_MULTIPLAYER_GAMEPLAY)
endif()
if(TH_ENABLE_NETPLAY)
    target_compile_definitions(${TH_EXEC_NAME} PRIVATE TH_ENABLE_NETPLAY)
endif()
if(TH_DEV_TOOLS)
    target_compile_definitions(${TH_EXEC_NAME} PRIVATE TH_DEV_TOOLS)
endif()

# Optional, generic out-of-tree runtime extension. Ordinary builds leave this
# empty and retain no dependency on the extension implementation.
set(TH_RUNTIME_EXTENSION_CMAKE "" CACHE FILEPATH "Optional runtime extension CMake integration")
if(TH_ENABLE_THPRAC AND TH_RUNTIME_EXTENSION_CMAKE)
    include("${TH_RUNTIME_EXTENSION_CMAKE}")
    thprac_portable_attach(${TH_EXEC_NAME} th07 "${CMAKE_CURRENT_SOURCE_DIR}/src")
endif()

if(TH_EXTERNAL_ASSETS)
    target_compile_definitions(${TH_EXEC_NAME} PRIVATE TH_EXTERNAL_ASSETS)
endif()

if(EMSCRIPTEN)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

    target_compile_options(th07 PRIVATE
        "-O2"
    )
    target_link_options(th07 PRIVATE
        "-sALLOW_MEMORY_GROWTH"
        "-sMAX_WEBGL_VERSION=2"
        "-sMIN_SAFARI_VERSION=140100"
        "-sENVIRONMENT=web"
        "-O2"
        "-sINVOKE_RUN=0"
        "-sEXPORTED_RUNTIME_METHODS=['callMain']"
        "-lidbfs.js"
        "--shell-file"
        "${CMAKE_SOURCE_DIR}/resources/shell.html"
    )
    if(TH_ENABLE_NETPLAY)
        target_link_options(th07 PRIVATE "-lwebsocket")
    endif()
    set_property(TARGET th07 APPEND PROPERTY LINK_DEPENDS "${CMAKE_SOURCE_DIR}/resources/shell.html")
    if(NOT TH_EXTERNAL_ASSETS)
        set(TH07_WEB_ASSET_STAGE "${CMAKE_BINARY_DIR}/web-assets-${TH_WEB_MUSIC}")
        file(REMOVE_RECURSE "${TH07_WEB_ASSET_STAGE}")
        file(MAKE_DIRECTORY "${TH07_WEB_ASSET_STAGE}")
        set(TH07_WEB_BASE_ASSETS th07.dat)
        foreach(asset IN LISTS TH07_WEB_BASE_ASSETS)
            if(NOT EXISTS "${TH07_ASSET_ROOT}/${asset}")
                message(FATAL_ERROR "Missing Web asset: ${TH07_ASSET_ROOT}/${asset}")
            endif()
            file(COPY_FILE "${TH07_ASSET_ROOT}/${asset}" "${TH07_WEB_ASSET_STAGE}/${asset}")
        endforeach()
        if(NOT TH_WEB_SHARED_FONT)
            file(COPY_FILE "${TH07_ASSET_ROOT}/msgothic.ttc" "${TH07_WEB_ASSET_STAGE}/msgothic.ttc")
            if(TH_ENABLE_THCRAP OR TH_ENABLE_THPRAC)
                file(COPY_FILE "${TH_UNIFONT_FILE}" "${TH07_WEB_ASSET_STAGE}/unifont.otf")
            endif()
        endif()
        if(TH_WEB_MUSIC STREQUAL "WAV")
            file(COPY_FILE "${TH07_ASSET_ROOT}/thbgm.dat" "${TH07_WEB_ASSET_STAGE}/thbgm.dat")
        elseif(TH_WEB_MUSIC STREQUAL "OGG")
            if(NOT EXISTS "${TH07_OGG_ROOT}/bgm-ogg")
                message(FATAL_ERROR "TH_WEB_MUSIC=OGG requires generated ${TH07_OGG_ROOT}/bgm-ogg")
            endif()
            file(COPY "${TH07_OGG_ROOT}/bgm-ogg" DESTINATION "${TH07_WEB_ASSET_STAGE}")
        elseif(TH_WEB_MUSIC STREQUAL "MIDI" OR TH_WEB_MUSIC STREQUAL "BASE")
            # The original MIDI is in th07.dat. BASE leaves streamed BGM to
            # the Web host, which installs WAV/OGG resources before callMain.
        else()
            message(FATAL_ERROR "Unsupported TH_WEB_MUSIC=${TH_WEB_MUSIC}; expected BASE, MIDI, WAV, or OGG")
        endif()
        target_link_options(th07 PRIVATE "--preload-file" "${TH07_WEB_ASSET_STAGE}@/")
    endif()
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

if(TH07_MIDI_ALSA)
    target_compile_definitions(${TH_EXEC_NAME} PRIVATE LIBASOUND_MIDI_SUPPORT)
    target_link_libraries(${TH_EXEC_NAME} PRIVATE ALSA::ALSA)
elseif(TH07_MIDI_COREAUDIO)
    target_compile_definitions(${TH_EXEC_NAME} PRIVATE COREAUDIO_MIDI_SUPPORT)
    target_link_libraries(${TH_EXEC_NAME} PRIVATE
        "-framework AudioToolbox"
        "-framework AudioUnit"
    )
endif()

if(NOT ANDROID AND NOT EMSCRIPTEN AND NOT APPLE AND NOT WIN32)
    find_package(OpenGL COMPONENTS GLES3 REQUIRED)
endif()

if(APPLE)
    enable_language(OBJC)
    enable_language(OBJCXX)

    set_source_files_properties(src/SoundPlayer.cpp PROPERTIES LANGUAGE OBJCXX)
    set(APP_RESOURCES
        "${CMAKE_SOURCE_DIR}/assets/th07.dat"
        "${CMAKE_SOURCE_DIR}/assets/thbgm.dat"
        "${CMAKE_SOURCE_DIR}/assets/msgothic.ttc"
    )
    if(TH_ENABLE_THCRAP OR TH_ENABLE_THPRAC)
        list(APPEND APP_RESOURCES "${TH_UNIFONT_FILE}")
    endif()

if(NOT TH_EXTERNAL_ASSETS)
    target_sources(${TH_EXEC_NAME} PRIVATE ${APP_RESOURCES})
    set_source_files_properties(${APP_RESOURCES} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )
endif()

if (IOS OR NOT TH_EXTERNAL_ASSETS)
    set_target_properties(${TH_EXEC_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "東方妖々夢"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.zun.th07"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.0"
    )
endif()

    if(IOS)
        set(BUILD_SHARED_LIBS OFF)
        set_target_properties(${TH_EXEC_NAME} PROPERTIES
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/iOS/Info.plist
        )
        target_link_libraries(${TH_EXEC_NAME} PRIVATE
            "-framework OpenGLES"
            "-framework AudioToolbox"
            "-framework AVFoundation"
            "-framework CoreAudio"
            "-framework CoreGraphics"
            "-framework QuartzCore"
            "-framework UIKit"
            "-framework Foundation"
        )
    else()
        target_compile_definitions(${TH_EXEC_NAME} PRIVATE USING_GL)
        set_target_properties(${TH_EXEC_NAME} PROPERTIES
            XCODE_ATTRIBUTE_INFOPLIST_KEY_CFBundleDisplayName "東方妖々夢　～ Perfect Cherry Blossom"
        )
        find_package(OpenGL REQUIRED)
        target_link_libraries(${TH_EXEC_NAME} PRIVATE
            OpenGL::GL
            "-framework AudioToolbox"
            "-framework CoreAudio"
            "-framework CoreVideo"
            "-framework Cocoa"
            "-framework IOKit"
            "-framework ForceFeedback"
            "-framework Carbon"
        )
    endif()
endif()

if(EMSCRIPTEN)
    # emscripten has no sdl3_image port, so it has to use a vendored sdl
    set(USE_VENDORED_SDL ON)
else()
    find_package(SDL3 QUIET)
    find_package(SDL3_ttf QUIET)
    find_package(SDL3_image QUIET)

    if(NOT SDL3_FOUND OR NOT SDL3_ttf_FOUND OR NOT SDL3_image_FOUND)
        message(STATUS "system SDL3 libraries not found. using vendored sdl3")
        set(USE_VENDORED_SDL ON)
    else()
        set(USE_VENDORED_SDL OFF)
    endif()
endif()

if(USE_VENDORED_SDL)
    set(SDLIMAGE_BACKEND_STB ON)
    set(SDLIMAGE_BACKEND_IMAGEIO OFF)

    set(SDLIMAGE_ANI OFF)
    set(SDLIMAGE_AVIF OFF)
    set(SDLIMAGE_BMP OFF)
    set(SDLIMAGE_GIF OFF)
    set(SDLIMAGE_JPG ON) # really, we only need jpg
    set(SDLIMAGE_JXL OFF)
    set(SDLIMAGE_LBM OFF)
    set(SDLIMAGE_PCX OFF)
    set(SDLIMAGE_PNG ON) # thcrap localization supplies PNG text and title assets
    set(SDLIMAGE_PNM OFF)
    set(SDLIMAGE_QOI OFF)
    set(SDLIMAGE_SVG OFF)
    set(SDLIMAGE_TGA OFF)
    set(SDLIMAGE_TIF OFF)
    set(SDLIMAGE_WEBP OFF)
    set(SDLIMAGE_XCF OFF)
    set(SDLIMAGE_XPM OFF)
    set(SDLIMAGE_XV OFF)

    set(SDLTTF_VENDORED ON)
    set(SDLTTF_HARFBUZZ OFF)

    add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
    add_subdirectory(vendored/SDL_image EXCLUDE_FROM_ALL)
    add_subdirectory(vendored/SDL_ttf EXCLUDE_FROM_ALL)
endif()

if(TH_ENABLE_THPRAC AND USE_VENDORED_SDL)
    if(NOT TARGET Freetype::Freetype)
        message(FATAL_ERROR "TH_ENABLE_THPRAC requires the vendored Freetype target for ImGui's pinned FreeType atlas builder")
    endif()
    target_include_directories(${TH_EXEC_NAME} PRIVATE
        $<TARGET_PROPERTY:Freetype::Freetype,INTERFACE_INCLUDE_DIRECTORIES>
    )
    target_link_libraries(${TH_EXEC_NAME} PRIVATE Freetype::Freetype)
endif()

target_link_libraries(${TH_EXEC_NAME} PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
)

if(ANDROID)
    target_link_libraries(${TH_EXEC_NAME} PRIVATE GLESv3)
elseif(WIN32 AND NOT EMSCRIPTEN)
    set(TH_MESA_ROOT "${CMAKE_SOURCE_DIR}/../dependencies")
    target_include_directories(${TH_EXEC_NAME} PRIVATE "${TH_MESA_ROOT}/mesa-devel/include")
    target_link_libraries(${TH_EXEC_NAME} PRIVATE
        "${TH_MESA_ROOT}/mesa-devel/lib/x64/libEGL.dll.a"
        "${TH_MESA_ROOT}/mesa-devel/lib/x64/libGLESv2.dll.a"
        winmm
    )
elseif(NOT EMSCRIPTEN AND NOT APPLE)
    target_link_libraries(${TH_EXEC_NAME} PRIVATE OpenGL::GLES3)
endif()
