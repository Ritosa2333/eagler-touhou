include("${CMAKE_CURRENT_LIST_DIR}/OverlayWebP.cmake")

set(imgui "${EAGLER_COMMON_ROOT}/third_party/imgui")
set(overlay_support "${CMAKE_CURRENT_BINARY_DIR}/imgui-overlay-dependencies")
file(MAKE_DIRECTORY "${overlay_support}")
function(th09_overlay_dependency name url hash)
    set(destination "${overlay_support}/${name}")
    if(EXISTS "${destination}")
        file(SHA256 "${destination}" actual)
        if(actual STREQUAL "${hash}")
            return()
        endif()
    endif()
    file(DOWNLOAD "${url}"
        "${destination}" TLS_VERIFY ON EXPECTED_HASH "SHA256=${hash}")
endfunction()
set(imgui_upstream "https://raw.githubusercontent.com/ocornut/imgui/v1.82")
th09_overlay_dependency(imstb_truetype.h "${imgui_upstream}/imstb_truetype.h" bb24084056cd1fe17078b755cbd600d23449173d6a927ad7f067d3ce6fbdd1f0)
th09_overlay_dependency(imgui_impl_opengl3.cpp "${imgui_upstream}/backends/imgui_impl_opengl3.cpp" 32281bc318a8f0cdfd7e3b3cb5ef7dee732cb7cbea24c7f213dc2711394c5ee6)
th09_overlay_dependency(imgui_impl_opengl3.h "${imgui_upstream}/backends/imgui_impl_opengl3.h" a85170bb09bbf9cc74aea4822dc80ecc50e6e35ceb902040239d0222b6f1327c)
th09_overlay_dependency(LICENSE-imgui.txt "${imgui_upstream}/LICENSE.txt" fbb51424cf27ca216445d4b472c6aac187ebd8200f7ef41bd845a7d36d54e2c4)
th09_overlay_dependency(ChillRoundGothic_Medium.ttf
    "https://cdn.jsdelivr.net/gh/Warren2060/ChillRoundGothic@53505f0818983d2fcdda00dc66e051ad13e81ffb/ttf/ChillRoundGothic_Medium.ttf"
    7aaf168681f168b9f550734d9e25058ffe73213db07d7a50152d5446dd9e7094)
set(launcher "${CMAKE_CURRENT_SOURCE_DIR}/../../launcher")
set(overlay_source "${launcher}/examples/imgui-overlay")
set(overlay_font "${overlay_support}/ChillRoundGothic_Medium.ttf")
set(overlay_font_license "${launcher}/public/assets/fonts/OFL-ChillRoundGothic.txt")
set(overlay_rtc_license "${overlay_source}/node_modules/node-datachannel/LICENSE")
set(overlay_licenses "${overlay_support}/LICENSE-imgui.txt"
    "${EAGLER_COMMON_ROOT}/include/eagler/thprac/THPRAC-LICENSE.txt" "${overlay_font_license}")
set(overlay_bridge "${CMAKE_CURRENT_BINARY_DIR}/th09-lobby.exe")
set(overlay_arch x64)
if(CMAKE_GENERATOR_PLATFORM STREQUAL "ARM64" OR CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|ARM64|arm64)$")
    set(overlay_arch arm64)
endif()
find_program(TH09_BUN_EXECUTABLE NAMES bun bun.exe HINTS "$ENV{APPDATA}/npm/node_modules/bun/bin" REQUIRED)
find_program(TH09_FFMPEG_EXECUTABLE NAMES ffmpeg REQUIRED)
set(overlay_cover "${CMAKE_CURRENT_BINARY_DIR}/th09-cover.bmp")
set(overlay_cover_source "${CMAKE_CURRENT_SOURCE_DIR}/../../launcher/app/components/launcher/book-covers/th09.webp")
file(GLOB_RECURSE overlay_contracts CONFIGURE_DEPENDS
    "${EAGLER_COMMON_ROOT}/contracts/*.mjs"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../launcher/src/launcher/*.mts")
add_custom_command(OUTPUT "${overlay_bridge}"
    COMMAND "${TH09_BUN_EXECUTABLE}" "${overlay_source}/build-bridge.mts" "${overlay_bridge}" "${overlay_arch}"
    DEPENDS "${overlay_source}/build-bridge.mts" "${overlay_source}/package-lock.json"
        "${overlay_source}/bridge.mts" "${overlay_source}/room-audio.mts"
        "${launcher}/app/services/room-music.ts" "${launcher}/app/services/music-provider.ts"
        "${launcher}/app/services/abort-signals.ts" "${launcher}/messages/zh-CN.json" ${overlay_contracts}
    VERBATIM)
add_custom_command(OUTPUT "${overlay_cover}"
    COMMAND "${TH09_FFMPEG_EXECUTABLE}" -v error -y -i "${overlay_cover_source}" -vf scale=96:-1 "${overlay_cover}"
    DEPENDS "${overlay_cover_source}" VERBATIM)
add_custom_target(th09_overlay_assets
    COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:th09>"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "${overlay_bridge}" "${overlay_cover}" "${overlay_font}" ${overlay_licenses} "$<TARGET_FILE_DIR:th09>"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${overlay_rtc_license}" "$<TARGET_FILE_DIR:th09>/LICENSE-node-datachannel.txt"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${th09_webp_SOURCE_DIR}/COPYING" "$<TARGET_FILE_DIR:th09>/LICENSE-libwebp.txt"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${th09_webp_SOURCE_DIR}/PATENTS" "$<TARGET_FILE_DIR:th09>/PATENTS-libwebp.txt"
    DEPENDS "${overlay_bridge}" "${overlay_cover}" "${overlay_font}" ${overlay_licenses} VERBATIM)
add_dependencies(th09 th09_overlay_assets)
add_library(th09_imgui STATIC
    "${imgui}/imgui.cpp" "${imgui}/imgui_draw.cpp" "${imgui}/imgui_tables.cpp" "${imgui}/imgui_widgets.cpp"
    "${overlay_support}/imgui_impl_opengl3.cpp")
target_compile_features(th09_imgui PUBLIC cxx_std_17)
target_include_directories(th09_imgui PUBLIC "${imgui}" "${overlay_support}" "${overlay_source}" PRIVATE "${TH09_GLES_INCLUDE}")
target_compile_definitions(th09_imgui PUBLIC
    IMGUI_DISABLE_INCLUDE_IMCONFIG_H IMGUI_INCLUDE_IMGUI_USER_H IMGUI_INCLUDE_IMGUI_USER_INL
    IMGUI_USER_CONFIG="overlay-imconfig.h" "ImDrawIdx=unsigned int"
    IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS NOMINMAX WIN32_LEAN_AND_MEAN
    PRIVATE IMGUI_IMPL_OPENGL_ES3)
target_link_libraries(th09_imgui PRIVATE "${TH09_GLES_LIBRARY}")
if(MSVC)
    target_compile_options(th09_imgui PRIVATE /utf-8)
endif()
target_sources(th09 PRIVATE desktop/ImGuiOverlay.cpp desktop/RoomAudio.cpp)
target_link_libraries(th09 PRIVATE th09_imgui webpdecoder Crypt32 mfuuid ole32 oleaut32)
target_compile_definitions(th09 PRIVATE TH09_NATIVE_OVERLAY=1)
