from pathlib import Path

root = Path(__file__).resolve().parents[1]
cmake = (root / "CMakeLists.txt").read_text(encoding="utf-8")

assert 'option(TH_ENABLE_NETPLAY' in cmake
assert 'option(TH_ENABLE_MULTIPLAYER_GAMEPLAY' in cmake
assert 'if(TH_ENABLE_NETPLAY OR TH_ENABLE_MULTIPLAYER_GAMEPLAY)' in cmake
assert 'src/multiplayer/GameplaySession.cpp' in cmake
assert 'src/netplay/NetplayInput.cpp' in cmake
assert 'if(TH_ENABLE_NETPLAY)' in cmake
for source in (
    'src/netplay/NetplayCore.cpp',
    'src/netplay/NetplayProtocol.cpp',
    'src/netplay/NetplaySession.cpp',
    'src/netplay/RollbackJournal.cpp',
    'src/netplay/NetplaySideEffects.cpp',
    'src/netplay/BrowserPeerTransport.cpp',
    'src/netplay/WebSocketTransport.cpp',
):
    assert source in cmake
assert 'target_compile_definitions(${TH_EXEC_NAME} PRIVATE TH_ENABLE_NETPLAY)' in cmake
assert 'target_link_options(th06 PRIVATE -lwebsocket)' in cmake

# The rejected flat multiplayer line must not reappear as an option, source or
# runtime switch.  Normal builds keep both new feature switches OFF.
for rejected in ('TH_ENABLE_MULTIPLAYER"', 'MultiplayerRuntime.cpp', 'multiplayerEnabled'):
    assert rejected not in cmake

print("TH06 ordinary/netplay build isolation: PASS")
