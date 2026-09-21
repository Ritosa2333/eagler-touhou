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

add_executable(eagler_common_browser_peer_transport_test
    browser-peer-transport-test.cpp
)
target_include_directories(eagler_common_browser_peer_transport_test PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/include"
)
target_link_libraries(eagler_common_browser_peer_transport_test PRIVATE
    eagler::browser_peer_transport
)
add_test(
    NAME eagler-common.browser-peer-transport
    COMMAND eagler_common_browser_peer_transport_test
)

add_executable(eagler_common_netplay_input_test
    netplay-input-test.cpp
)
target_include_directories(eagler_common_netplay_input_test PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/include"
)
target_compile_definitions(eagler_common_netplay_input_test PRIVATE
    TH_ENABLE_MULTIPLAYER_GAMEPLAY
)
target_link_libraries(eagler_common_netplay_input_test PRIVATE
    eagler::netplay_input
)
add_test(
    NAME eagler-common.netplay-input
    COMMAND eagler_common_netplay_input_test
)

add_executable(eagler_common_direct_touch_pipeline_test
    direct-touch-pipeline-test.cpp
)
target_include_directories(eagler_common_direct_touch_pipeline_test PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/include"
)
target_link_libraries(eagler_common_direct_touch_pipeline_test PRIVATE
    eagler::netplay_base
    eagler::netplay_input
)
add_test(
    NAME eagler-common.direct-touch-pipeline
    COMMAND eagler_common_direct_touch_pipeline_test
)

add_executable(eagler_common_direct_touch_equivalence_test
    direct-touch-equivalence-test.cpp
)
target_link_libraries(eagler_common_direct_touch_equivalence_test PRIVATE
    eagler::netplay_headers
)
add_test(
    NAME eagler-common.direct-touch-equivalence
    COMMAND eagler_common_direct_touch_equivalence_test
)

add_executable(eagler_common_frame_advantage_window_test
    frame-advantage-window-test.cpp
)
target_link_libraries(eagler_common_frame_advantage_window_test PRIVATE
    eagler::netplay_headers
)
add_test(
    NAME eagler-common.frame-advantage-window
    COMMAND eagler_common_frame_advantage_window_test
)

add_executable(eagler_common_frame_budget_test
    frame-budget-test.cpp
)
target_link_libraries(eagler_common_frame_budget_test PRIVATE
    eagler::netplay_headers
)
add_test(
    NAME eagler-common.frame-budget
    COMMAND eagler_common_frame_budget_test
)

add_executable(eagler_common_frame_pacing_policy_test
    frame-pacing-policy-test.cpp
)
target_link_libraries(eagler_common_frame_pacing_policy_test PRIVATE
    eagler::netplay_headers
)
add_test(
    NAME eagler-common.frame-pacing-policy
    COMMAND eagler_common_frame_pacing_policy_test
)

add_executable(eagler_common_confirmed_input_watchdog_test
    confirmed-input-watchdog-test.cpp
)
target_link_libraries(eagler_common_confirmed_input_watchdog_test PRIVATE
    eagler::netplay_headers
)
add_test(
    NAME eagler-common.confirmed-input-watchdog
    COMMAND eagler_common_confirmed_input_watchdog_test
)

add_executable(eagler_common_rollback_replay_budget_test
    rollback-replay-budget-test.cpp
)
target_link_libraries(eagler_common_rollback_replay_budget_test PRIVATE
    eagler::netplay_headers
)
add_test(
    NAME eagler-common.rollback-replay-budget
    COMMAND eagler_common_rollback_replay_budget_test
)

add_executable(eagler_common_input_repair_budget_test
    input-repair-budget-test.cpp
)
target_link_libraries(eagler_common_input_repair_budget_test PRIVATE
    eagler::netplay_headers
)
add_test(
    NAME eagler-common.input-repair-budget
    COMMAND eagler_common_input_repair_budget_test
)

add_test(
    NAME eagler-common.input-repair-source-contract
    COMMAND ${CMAKE_COMMAND}
        -DSOURCE=${CMAKE_CURRENT_SOURCE_DIR}/../src/netplay/BrowserPeerTransport.cpp
        -P ${CMAKE_CURRENT_SOURCE_DIR}/browser-peer-repair-source-test.cmake
)

find_program(EAGLER_COMMON_NODE_EXECUTABLE NAMES node node.exe)
if(EAGLER_COMMON_NODE_EXECUTABLE)
    add_test(
        NAME eagler-common.rtc-input-impairment
        COMMAND ${EAGLER_COMMON_NODE_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/rtc-input-impairment-test.cjs
    )
    add_test(
        NAME eagler-common.replay-verifier
        COMMAND ${EAGLER_COMMON_NODE_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/../testkit/replay-verifier/replay-verifier.test.mjs
    )
endif()
