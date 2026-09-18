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

add_executable(eagler_common_rollback_journal_test
    rollback-journal-test.cpp
)
target_link_libraries(eagler_common_rollback_journal_test PRIVATE eagler::rollback)
add_test(NAME eagler-common.rollback-journal COMMAND eagler_common_rollback_journal_test)

add_executable(eagler_common_snapshot_policy_test
    snapshot-policy-test.cpp
)
target_link_libraries(eagler_common_snapshot_policy_test PRIVATE eagler::rollback)
add_test(NAME eagler-common.snapshot-policy COMMAND eagler_common_snapshot_policy_test)

add_executable(eagler_common_sparse_pool_test
    sparse-pool-test.cpp
)
target_link_libraries(eagler_common_sparse_pool_test PRIVATE eagler::rollback)
add_test(NAME eagler-common.sparse-pool COMMAND eagler_common_sparse_pool_test)

add_executable(eagler_common_partitioned_pool_test
    partitioned-pool-journal-test.cpp
)
target_link_libraries(eagler_common_partitioned_pool_test PRIVATE eagler::netplay_headers)
add_test(NAME eagler-common.partitioned-pool COMMAND eagler_common_partitioned_pool_test)
