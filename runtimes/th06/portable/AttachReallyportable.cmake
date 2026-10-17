# Source-owned TH06 purple extension; no private paths or game data.
function(thprac_portable_attach target game game_source_dir)
    if(NOT game STREQUAL "th06")
        message(FATAL_ERROR "This extension belongs to TH06 only")
    endif()
    set(root "${CMAKE_CURRENT_FUNCTION_LIST_DIR}")
    target_sources(${target} PRIVATE
        "${root}/src/ecl_patch.cpp" "${root}/src/session.cpp"
        "${root}/src/th06/patches.cpp" "${root}/adapters/th06/adapter.cpp")
    target_include_directories(${target} PRIVATE
        "${root}/include" "${root}/generated" "${game_source_dir}")
    target_compile_definitions(${target} PRIVATE THPRAC_PORTABLE_ENABLED=1)
endfunction()
