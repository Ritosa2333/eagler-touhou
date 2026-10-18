# Owned, source-only TH07 extension. No private workstation paths or binaries.
function(thprac_portable_attach target game game_source_dir)
    if(NOT game STREQUAL "th07")
        message(FATAL_ERROR "This extension belongs to TH07 only")
    endif()
    set(root "${CMAKE_CURRENT_FUNCTION_LIST_DIR}")
    target_sources(${target} PRIVATE
        "${root}/src/ecl_patch.cpp"
        "${root}/src/session.cpp"
        "${root}/src/th07/patches.cpp"
        "${root}/adapters/th07/adapter.cpp")
    target_include_directories(${target} PRIVATE
        "${root}/include" "${root}/generated" "${game_source_dir}")
    target_compile_definitions(${target} PRIVATE THPRAC_PORTABLE_ENABLED=1)
    if(EMSCRIPTEN)
        # EM_ASM's JS helper dependency is not discovered by newer SDKs.
        target_link_options(${target} PRIVATE "-sDEFAULT_LIBRARY_FUNCS_TO_INCLUDE=['$stringToNewUTF8']")
    endif()
endfunction()
