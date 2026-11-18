include(FetchContent)
include("${CMAKE_CURRENT_LIST_DIR}/OverlayWebP.cmake")
set(BUILD_SHARED_LIBS OFF)
set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
FetchContent_Declare(th09_freetype
    URL https://codeload.github.com/freetype/freetype/tar.gz/refs/tags/VER-2-14-3
    URL_HASH SHA256=dc49de6b01a266eef4876a4dd34d9842c475d3e28ff2eff63bd2fb760ab56261)
FetchContent_MakeAvailable(th09_freetype)
if(NOT TARGET Freetype::Freetype)
    add_library(Freetype::Freetype ALIAS freetype)
endif()
set(RMLUI_SAMPLES OFF)
set(RMLUI_LUA_BINDINGS OFF)
FetchContent_Declare(th09_rmlui
    URL https://codeload.github.com/mikke89/RmlUi/tar.gz/refs/tags/6.3
    URL_HASH SHA256=d977298bb6147610e5984d5db85ddf284020d655a8713913f6982074f1dbdede)
FetchContent_MakeAvailable(th09_rmlui)
set(HTTPLIB_USE_OPENSSL_IF_AVAILABLE OFF CACHE BOOL "" FORCE)
set(HTTPLIB_USE_BROTLI_IF_AVAILABLE OFF CACHE BOOL "" FORCE)
set(HTTPLIB_USE_ZLIB_IF_AVAILABLE OFF CACHE BOOL "" FORCE)
set(HTTPLIB_USE_ZSTD_IF_AVAILABLE OFF CACHE BOOL "" FORCE)
FetchContent_Declare(th09_httplib
    URL https://codeload.github.com/yhirose/cpp-httplib/tar.gz/refs/tags/v0.27.0
    URL_HASH SHA256=cc57615af359efda816122dcfca37bcbb9f1591396f50a1fd1ad70bbe6050581)
FetchContent_MakeAvailable(th09_httplib)

find_program(TH09_NODE_EXECUTABLE NAMES node node.exe REQUIRED)
set(quick_chat_contract "${EAGLER_COMMON_ROOT}/contracts/multiplayer-quick-chat.mjs")
set(quick_chat_header "${CMAKE_CURRENT_BINARY_DIR}/RmlQuickChat.hpp")
add_custom_command(OUTPUT "${quick_chat_header}"
    COMMAND "${TH09_NODE_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/desktop/generate-rml-quick-chat.mjs"
        "${quick_chat_contract}" "${quick_chat_header}"
    DEPENDS "${quick_chat_contract}" desktop/generate-rml-quick-chat.mjs VERBATIM)
target_sources(th09 PRIVATE desktop/RmlOverlay.cpp "${quick_chat_header}"
    "${EAGLER_COMMON_ROOT}/src/netplay/NativeLobby.cpp"
    "${th09_rmlui_SOURCE_DIR}/Backends/RmlUi_Platform_SDL.cpp")
target_include_directories(th09 PRIVATE "${th09_rmlui_SOURCE_DIR}/Backends" "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(th09 PRIVATE RmlUi::Core httplib::httplib webpdecoder Crypt32)
target_compile_definitions(th09 PRIVATE TH09_RMLUI_OVERLAY=1 TH09_NATIVE_OVERLAY=1 RMLUI_SDL_VERSION_MAJOR=3 _WIN32_WINNT=0x0A00)
option(TH09_RMLUI_TEST "Enable unattended native overlay validation" OFF)
if(TH09_RMLUI_TEST)
    target_compile_definitions(th09 PRIVATE TH09_RMLUI_TEST=1)
endif()
add_custom_command(TARGET th09 POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${th09_rmlui_SOURCE_DIR}/LICENSE.txt" "$<TARGET_FILE_DIR:th09>/LICENSE-RmlUi.txt"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${th09_freetype_SOURCE_DIR}/docs/FTL.TXT" "$<TARGET_FILE_DIR:th09>/LICENSE-FreeType.txt"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${th09_httplib_SOURCE_DIR}/LICENSE" "$<TARGET_FILE_DIR:th09>/LICENSE-httplib.txt"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${th09_webp_SOURCE_DIR}/COPYING" "$<TARGET_FILE_DIR:th09>/LICENSE-libwebp.txt"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${th09_webp_SOURCE_DIR}/PATENTS" "$<TARGET_FILE_DIR:th09>/PATENTS-libwebp.txt"
    VERBATIM)
