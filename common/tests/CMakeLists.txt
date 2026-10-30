# Assertions are executable test contracts, including fixture setup. Release
# builds must not compile them out and then mutate uninitialized test buffers.
# Keep this option scoped to test targets; production consumers are unchanged.
if(MSVC)
    add_compile_options(/UNDEBUG)
else()
    add_compile_options(-UNDEBUG)
endif()

add_executable(eagler_common_thprac_test thprac-test.cpp)
target_link_libraries(eagler_common_thprac_test PRIVATE eagler::thprac)
add_test(NAME eagler-common.thprac COMMAND eagler_common_thprac_test)

add_executable(eagler_common_thprac_pointer_test thprac-pointer-test.cpp)
target_link_libraries(eagler_common_thprac_pointer_test PRIVATE eagler::thprac)
add_test(NAME eagler-common.thprac-pointer COMMAND eagler_common_thprac_pointer_test)

find_package(Freetype QUIET)
if(Freetype_FOUND)
    add_executable(eagler_common_thprac_keyboard_test thprac-keyboard-test.cpp
        ../third_party/imgui/imgui.cpp ../third_party/imgui/imgui_draw.cpp
        ../third_party/imgui/imgui_widgets.cpp ../third_party/imgui/imgui_tables.cpp
        ../third_party/imgui/imgui_freetype.cpp)
    target_include_directories(eagler_common_thprac_keyboard_test PRIVATE ../third_party/imgui)
    target_link_libraries(eagler_common_thprac_keyboard_test PRIVATE eagler::thprac Freetype::Freetype)
    add_test(NAME eagler-common.thprac-keyboard COMMAND eagler_common_thprac_keyboard_test)
endif()

add_executable(eagler_common_assertions_enabled_test assertions-enabled-test.cpp)
add_test(NAME eagler-common.assertions-enabled COMMAND eagler_common_assertions_enabled_test)

add_executable(eagler_common_adonis_timing_test adonis-timing-test.cpp)
target_include_directories(eagler_common_adonis_timing_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/include")
target_link_libraries(eagler_common_adonis_timing_test PRIVATE eagler::netplay_base)
add_test(NAME eagler-common.adonis-timing COMMAND eagler_common_adonis_timing_test)

add_executable(eagler_common_spectator_stage_test spectator-stage-test.cpp)
target_include_directories(eagler_common_spectator_stage_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/include")
target_link_libraries(eagler_common_spectator_stage_test PRIVATE eagler::netplay_base)
add_test(NAME eagler-common.spectator-stage COMMAND eagler_common_spectator_stage_test)

add_executable(eagler_common_spectator_stage_publisher_boundary_test
    spectator-stage-publisher-boundary-test.cpp
)
target_include_directories(eagler_common_spectator_stage_publisher_boundary_test PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/include"
)
target_link_libraries(eagler_common_spectator_stage_publisher_boundary_test PRIVATE
    eagler::netplay_base
)
add_test(NAME eagler-common.spectator-stage-publisher-boundary
    COMMAND eagler_common_spectator_stage_publisher_boundary_test
)

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

add_executable(eagler_common_input_replay_test input-replay-test.cpp)
target_include_directories(eagler_common_input_replay_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/include")
target_link_libraries(eagler_common_input_replay_test PRIVATE eagler::input_replay eagler::netplay_base)
add_test(NAME eagler-common.input-replay COMMAND eagler_common_input_replay_test)

add_executable(eagler_common_simulation_frontier_test simulation-frontier-test.cpp)
target_include_directories(eagler_common_simulation_frontier_test PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/include")
target_link_libraries(eagler_common_simulation_frontier_test PRIVATE eagler::netplay_base)
add_test(NAME eagler-common.simulation-frontier COMMAND eagler_common_simulation_frontier_test)

add_executable(eagler_common_session_channel_test session-channel-test.cpp)
target_include_directories(eagler_common_session_channel_test PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/include")
target_link_libraries(eagler_common_session_channel_test PRIVATE eagler::session_channel)
add_test(NAME eagler-common.session-channel COMMAND eagler_common_session_channel_test)

add_executable(eagler_common_calibration_retirement_test calibration-retirement-test.cpp)
target_include_directories(eagler_common_calibration_retirement_test PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/include")
target_link_libraries(eagler_common_calibration_retirement_test PRIVATE eagler::session_channel)
add_test(NAME eagler-common.calibration-retirement COMMAND eagler_common_calibration_retirement_test)

add_executable(eagler_common_packet_transaction_test packet-transaction-test.cpp)
target_include_directories(eagler_common_packet_transaction_test PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/include")
target_link_libraries(eagler_common_packet_transaction_test PRIVATE eagler::netplay_base)
add_test(NAME eagler-common.packet-transaction COMMAND eagler_common_packet_transaction_test)

add_executable(eagler_common_confirmation_window_test confirmation-window-test.cpp)
target_include_directories(eagler_common_confirmation_window_test PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/include")
target_link_libraries(eagler_common_confirmation_window_test PRIVATE eagler::netplay_base)
add_test(NAME eagler-common.confirmation-window COMMAND eagler_common_confirmation_window_test)

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

add_executable(eagler_common_session_pacing_test session-pacing-test.cpp)
target_include_directories(eagler_common_session_pacing_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/include")
target_link_libraries(eagler_common_session_pacing_test PRIVATE eagler::netplay_headers)
add_test(NAME eagler-common.session-pacing COMMAND eagler_common_session_pacing_test)
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
add_executable(eagler_common_adonis_startup_test adonis-startup-test.cpp ../src/netplay/NetplayProtocol.cpp)
target_include_directories(eagler_common_adonis_startup_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/include")
target_link_libraries(eagler_common_adonis_startup_test PRIVATE eagler::netplay_headers)
add_test(NAME eagler-common.adonis-startup COMMAND eagler_common_adonis_startup_test)
if(EAGLER_COMMON_NODE_EXECUTABLE)
    file(GLOB_RECURSE EAGLER_COMMON_NODE_TESTS CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.test.mjs"
        "${CMAKE_CURRENT_SOURCE_DIR}/*-test.mjs"
        "${CMAKE_CURRENT_SOURCE_DIR}/*-test.cjs"
        "${CMAKE_CURRENT_SOURCE_DIR}/../browser/*.test.mjs"
        "${CMAKE_CURRENT_SOURCE_DIR}/../testkit/*.test.mjs")
    foreach(node_test IN LISTS EAGLER_COMMON_NODE_TESTS)
        if(node_test MATCHES "/fixtures/")
            continue()
        endif()
        file(RELATIVE_PATH node_test_name "${CMAKE_CURRENT_SOURCE_DIR}/.." "${node_test}")
        string(REGEX REPLACE "(\\.test|-test)\\.(mjs|cjs)$" "" node_test_name "${node_test_name}")
        string(REPLACE "/" "." node_test_name "${node_test_name}")
        add_test(NAME "eagler-common.node.${node_test_name}"
            COMMAND ${EAGLER_COMMON_NODE_EXECUTABLE} --test "${node_test}")
    endforeach()
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../ports/th09/web/tests/server/replay-player.test.mjs")
        add_test(NAME eagler-common.versus-replay
            COMMAND ${EAGLER_COMMON_NODE_EXECUTABLE} --test ${CMAKE_CURRENT_SOURCE_DIR}/../../ports/th09/web/tests/server/replay-player.test.mjs)
    endif()
    add_test(NAME eagler-common.adaptive-peer-route-generated
        COMMAND ${EAGLER_COMMON_NODE_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../tools/sync-adaptive-peer-route.mjs --check)
endif()

add_executable(eagler_common_rollback_readonly_batch_test rollback-readonly-batch-test.cpp)
target_link_libraries(eagler_common_rollback_readonly_batch_test PRIVATE eagler::rollback)
add_test(NAME eagler-common.rollback-readonly-batch COMMAND eagler_common_rollback_readonly_batch_test)

add_executable(eagler_common_rollback_capture_window_test rollback-capture-window-test.cpp)
target_link_libraries(eagler_common_rollback_capture_window_test PRIVATE eagler::rollback)
add_test(NAME eagler-common.rollback-capture-window COMMAND eagler_common_rollback_capture_window_test)

add_executable(eagler_common_confirmed_input_clock_test confirmed-input-clock-test.cpp)
target_link_libraries(eagler_common_confirmed_input_clock_test PRIVATE eagler::netplay_base)
target_include_directories(eagler_common_confirmed_input_clock_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/include")
add_test(NAME eagler-common.confirmed-input-clock COMMAND eagler_common_confirmed_input_clock_test)

add_executable(eagler_common_cooperative_rules_test cooperative-rules-test.cpp)
add_executable(eagler_common_cooperative_menu_input_test cooperative-menu-input-test.cpp)
target_link_libraries(eagler_common_cooperative_menu_input_test PRIVATE eagler::netplay_headers)
add_test(NAME eagler-common.cooperative-menu-input COMMAND eagler_common_cooperative_menu_input_test)
target_link_libraries(eagler_common_cooperative_rules_test PRIVATE eagler::netplay_headers)
add_test(NAME eagler-common.cooperative-rules COMMAND eagler_common_cooperative_rules_test)
