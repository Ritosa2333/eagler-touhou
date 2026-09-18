include_guard(GLOBAL)

get_filename_component(_EAGLER_COMMON_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

if(NOT TARGET eagler_common_netplay_base)
    add_subdirectory(
        "${_EAGLER_COMMON_ROOT}"
        "${CMAKE_BINARY_DIR}/_deps/eagler-common"
        EXCLUDE_FROM_ALL
    )
endif()

function(eagler_common_link_netplay_base target)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "eagler_common_link_netplay_base: unknown target ${target}")
    endif()
    target_link_libraries(${target} PRIVATE eagler::netplay_base)
endfunction()

function(eagler_common_link_netplay_headers target)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "eagler_common_link_netplay_headers: unknown target ${target}")
    endif()
    target_link_libraries(${target} PRIVATE eagler::netplay_headers)
endfunction()

function(eagler_common_link_rollback target)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "eagler_common_link_rollback: unknown target ${target}")
    endif()
    target_link_libraries(${target} PRIVATE eagler::rollback)
endfunction()

function(eagler_common_link_browser_peer_transport target)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "eagler_common_link_browser_peer_transport: unknown target ${target}")
    endif()
    target_link_libraries(${target} PRIVATE eagler::browser_peer_transport)
endfunction()

function(eagler_common_link_netplay_input target)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "eagler_common_link_netplay_input: unknown target ${target}")
    endif()
    target_link_libraries(${target} PRIVATE eagler::netplay_input)
endfunction()
