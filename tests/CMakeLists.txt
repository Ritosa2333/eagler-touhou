add_executable(eagler_common_netplay_base_test
    netplay-base-test.cpp
)

target_include_directories(eagler_common_netplay_base_test PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/include"
)

target_link_libraries(eagler_common_netplay_base_test PRIVATE
    eagler::netplay_base
)

add_test(
    NAME eagler-common.netplay-base
    COMMAND eagler_common_netplay_base_test
)

add_executable(eagler_common_netplay_headers_test
    netplay-headers-test.cpp
)

target_link_libraries(eagler_common_netplay_headers_test PRIVATE
    eagler::netplay_headers
)

add_test(
    NAME eagler-common.netplay-headers
    COMMAND eagler_common_netplay_headers_test
)
