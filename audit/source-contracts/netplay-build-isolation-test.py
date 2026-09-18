from pathlib import Path

root = Path(__file__).resolve().parents[2]
cmake = (root / "CMakeLists.txt").read_text(encoding="utf-8")

assert 'option(TH_ENABLE_NETPLAY' in cmake
assert 'option(TH_ENABLE_MULTIPLAYER_GAMEPLAY' in cmake
assert 'if(TH_ENABLE_NETPLAY OR TH_ENABLE_MULTIPLAYER_GAMEPLAY)' in cmake
assert 'src/multiplayer/GameplaySession.cpp' in cmake
assert 'src/netplay/NetplayInput.cpp' in cmake
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
):
    assert shared_source not in cmake
assert 'eagler_common_link_netplay_base(${TH_EXEC_NAME})' in cmake
assert 'eagler_common_link_browser_peer_transport(${TH_EXEC_NAME})' in cmake
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
    'src/netplay/WebSocketTransport.cpp',
    'include/eagler/netplay/NetplayCore.hpp',
    'include/eagler/netplay/NetplayProtocol.hpp',
    'include/eagler/netplay/NetplaySession.hpp',
    'include/eagler/netplay/RollbackJournal.hpp',
    'include/eagler/netplay/SnapshotPolicy.hpp',
    'include/eagler/netplay/SparsePoolCapture.hpp',
    'include/eagler/netplay/PartitionedPoolJournal.hpp',
    'include/eagler/netplay/BrowserPeerTransport.hpp',
    'include/eagler/netplay/WebSocketTransport.hpp',
):
    assert (common / source).is_file(), source
for stem in ('RollbackJournal', 'SnapshotPolicy', 'SparsePoolCapture', 'PartitionedPoolJournal'):
    shim = (root / 'src' / 'netplay' / f'{stem}.hpp').read_text(encoding='utf-8')
    assert f'#include <eagler/netplay/{stem}.hpp>' in shim
    assert 'implementation authority lives in eagler-common' in shim
peer_header = (root / 'src/netplay/BrowserPeerTransport.hpp').read_text(encoding='utf-8')
peer_source = (root / 'src/netplay/BrowserPeerTransport.cpp').read_text(encoding='utf-8')
transport_config = (root / 'src/netplay/NetplayTransportConfig.hpp').read_text(encoding='utf-8')
assert '#include <eagler/netplay/BrowserPeerTransport.hpp>' in peer_header
assert 'implementation authority lives in eagler-common' in peer_header
assert 'CMake compiles the eagler-common implementation source' in peer_source
assert not [line for line in peer_source.splitlines()
            if line.strip() and not line.lstrip().startswith('//')]
assert 'Tag[] = "th06"' in transport_config
assert 'target_compile_definitions(${TH_EXEC_NAME} PRIVATE TH_ENABLE_NETPLAY)' in cmake
assert 'target_link_options(th06 PRIVATE -lwebsocket)' in cmake

# The rejected flat multiplayer line must not reappear as an option, source or
# runtime switch.  Normal builds keep both new feature switches OFF.
for rejected in ('TH_ENABLE_MULTIPLAYER"', 'MultiplayerRuntime.cpp', 'multiplayerEnabled'):
    assert rejected not in cmake

print("TH06 ordinary/netplay build isolation: PASS")
