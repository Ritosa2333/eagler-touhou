from pathlib import Path

root = Path(__file__).resolve().parents[2]
cmake = (root / "CMakeLists.txt").read_text(encoding="utf-8")

assert 'option(TH_ENABLE_NETPLAY' in cmake
assert 'option(TH_ENABLE_MULTIPLAYER_GAMEPLAY' in cmake
assert 'if(TH_ENABLE_NETPLAY OR TH_ENABLE_MULTIPLAYER_GAMEPLAY)' in cmake
assert 'src/multiplayer/GameplaySession.cpp' in cmake
assert 'if(TH_ENABLE_NETPLAY)' in cmake
for source in (
    'src/netplay/NetplaySideEffects.cpp',
):
    assert source in cmake
for shared_source in (
    'src/netplay/NetplayCore.cpp',
    'src/netplay/NetplayProtocol.cpp',
    'src/netplay/RollbackJournal.cpp',
    'src/netplay/BrowserPeerTransport.cpp',
    'src/netplay/NetplayInput.cpp',
):
    assert shared_source not in cmake
assert 'eagler_common_link_netplay_base(${TH_EXEC_NAME})' in cmake
assert 'eagler_common_link_browser_peer_transport(${TH_EXEC_NAME})' in cmake
assert 'eagler_common_link_netplay_input(${TH_EXEC_NAME})' in cmake
assert 'eagler_common_link_netplay_headers(${TH_EXEC_NAME})' in cmake
assert 'eagler_common_append_netplay_base_sources(TH06_SOURCES)' not in cmake
assert 'eagler_common_add_include_path(${TH_EXEC_NAME})' not in cmake
common = root / 'third_party' / 'eagler-common'
for source in (
    'src/netplay/NetplayCore.cpp',
    'src/netplay/NetplayProtocol.cpp',
    'src/netplay/NetplaySession.cpp',
    'src/netplay/RollbackJournal.cpp',
    'src/netplay/BrowserPeerTransport.cpp',
    'src/netplay/NetplayInput.cpp',
    'src/netplay/WebSocketTransport.cpp',
    'include/eagler/netplay/NetplayCore.hpp',
    'include/eagler/netplay/NetplayProtocol.hpp',
    'include/eagler/netplay/NetplaySession.hpp',
    'include/eagler/netplay/RollbackJournal.hpp',
    'include/eagler/netplay/SnapshotPolicy.hpp',
    'include/eagler/netplay/SparsePoolCapture.hpp',
    'include/eagler/netplay/PartitionedPoolJournal.hpp',
    'include/eagler/netplay/BrowserPeerTransport.hpp',
    'include/eagler/netplay/InputRepairBudget.hpp',
    'include/eagler/netplay/NetplayInput.hpp',
    'include/eagler/netplay/WebSocketTransport.hpp',
):
    assert (common / source).is_file(), source
transport_config = (root / 'src/netplay/NetplayTransportConfig.hpp').read_text(encoding='utf-8')
assert 'Tag[] = "th06"' in transport_config
input_config = (root / 'src/netplay/NetplayInputConfig.hpp').read_text(encoding='utf-8')
assert 'CommitGameInputs' in input_config
assert 'Netplay::MAX_PLAYERS' in input_config
assert 'g_LastFrameGameInputs[player] = g_CurFrameGameInputs[player]' in input_config
assert 'g_CurFrameGameInputs[player] = buttons[player]' in input_config
for retired in (
    'BrowserPeerTransport.hpp', 'BrowserPeerTransport.cpp',
    'NetplayCore.hpp', 'NetplayCore.cpp',
    'NetplayInput.hpp', 'NetplayInput.cpp',
    'NetplayProtocol.hpp', 'NetplayProtocol.cpp',
    'NetplaySession.hpp', 'NetplaySession.cpp',
    'PartitionedPoolJournal.hpp',
    'RollbackJournal.hpp', 'RollbackJournal.cpp',
    'SnapshotPolicy.hpp', 'SparsePoolCapture.hpp',
    'WebSocketTransport.hpp', 'WebSocketTransport.cpp',
):
    assert not (root / 'src' / 'netplay' / retired).exists(), retired

# Reliable input repair is a rare duplicate of the already-captured packet,
# never a second physical-input producer or a replacement for the RTC fast
# lane.  The common transport owns the control-channel send primitive; TH06
# owns only the stalled-ACK policy and product default.
driver = (root / 'src/netplay/Th06LanStageProbe.cpp').read_text(encoding='utf-8')
shell = (root / 'resources/shell.html').read_text(encoding='utf-8')
send_start = driver.index('bool SendScheduledLocalFrame(std::uint32_t frame)')
send_end = driver.index('bool SendTailKeepalive()', send_start)
send_path = driver[send_start:send_end]
assert '#include <eagler/netplay/InputRepairBudget.hpp>' in driver
assert 'g_InputRepairBudgets[peer].ShouldRepair(' in send_path
assert 'packet.firstInputFrame, packet.inputCount != 0, SDL_GetTicks()' in send_path
assert 'g_BrowserPeerTransport.SendRepairTo(peer, wire.data(), wire.size())' in send_path
assert send_path.index('TransportSendTo(peer, wire.data(), wire.size())') < send_path.index('SendRepairTo(peer, wire.data(), wire.size())')
assert 'netplayReliableInputRepair: options.netplayReliableInputRepair ?? true' in shell
assert '"netplayReliableInputRepair"' in shell
assert 'target_compile_definitions(${TH_EXEC_NAME} PRIVATE TH_ENABLE_NETPLAY)' in cmake
assert 'target_link_options(th06 PRIVATE -lwebsocket)' in cmake

# The rejected flat multiplayer line must not reappear as an option, source or
# runtime switch.  Normal builds keep both new feature switches OFF.
for rejected in ('TH_ENABLE_MULTIPLAYER"', 'MultiplayerRuntime.cpp', 'multiplayerEnabled'):
    assert rejected not in cmake

print("TH06 ordinary/netplay build isolation: PASS")
