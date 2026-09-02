from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def text(path: str) -> str:
    return (ROOT / path).read_text(encoding="utf-8")


extension = text("src/ReplayExtension.cpp")
manager = text("src/ReplayManager.cpp")
main_menu = text("src/MainMenu.cpp")
game_manager = text("src/GameManager.cpp")
player = text("src/Player.cpp")
touch = text("src/Touch.cpp")
result_screen = text("src/ResultScreen.cpp")
practice = text("src/PracticeRuntime.cpp")
eagler_options = text("src/EaglerOptions.hpp")
shell = text("resources/shell.html")

# EAGX format identity must stay compatible with the existing replay contract.
assert "constexpr u32 VERSION = 1;" in extension
assert "constexpr u32 DETERMINISM_ABI = 1;" in extension
assert "constexpr u32 FLAG_MULTIPLAYER_INPUT = 32;" in extension
assert "constexpr u32 FLAG_MULTIPLAYER_STAGE_RESOURCES = 64;" in extension
assert "constexpr u32 FLAG_MULTIPLAYER_STAGE_CONTRIBUTIONS = 128;" in extension
assert "constexpr std::size_t HEADER_SIZE = 96;" in extension
assert "constexpr std::size_t MP_HEADER_SIZE = 160;" in extension

# The multiplayer sidecar stores room metadata/loadouts and all stable input
# lanes rather than only the local controller approximation.
assert "BeginMultiplayerRecording" in manager
assert "ReplayExtension::RecordMultiplayerFrame" in manager
assert "Netplay::Input::PlayerInputOverride" in manager
assert "MP_PLAYER_COUNT_OFFSET" in extension
assert "MP_DIFFICULTY_OFFSET" in extension
assert "MP_GAMEPLAY_ABI_OFFSET" in extension
assert "MP_LOADOUT_OFFSET" in extension
assert "MP_LOCAL_PLAYER_OFFSET" in extension
assert "config.localPlayer = MultiplayerGameplay::GetLocalPlayerSlot();" in manager
assert "config.gameplayAbi = TH06_MULTI_GAMEPLAY_ABI;" in manager
assert "mpConfig.gameplayAbi = TH06_MULTI_GAMEPLAY_ABI;" in extension
assert "loadedConfig.gameplayAbi == TH06_MULTI_GAMEPLAY_ABI" in extension
assert "legacyConfig.gameplayAbi == 1" in extension
assert "replayConfig.gameplayAbi != TH06_MULTI_GAMEPLAY_ABI" in main_menu
assert "CaptureMultiplayerStageResources" in manager
assert "CaptureMultiplayerStageContributions" in manager
assert "contributions[playerId].enemiesDefeated = GetPlayerEnemiesDefeated(playerId)" in manager
assert "contributions[playerId].damageDealt = GetPlayerDamageDealt(playerId)" in manager
assert "GetPlayerLives(playerId)" in manager
assert "GetPlayerBombs(playerId)" in manager
assert "GetPlayerPower(playerId)" in manager

# Rollback resimulation reproduces game state but must never append the same
# logical replay frame twice.
record_update = manager.split("ChainCallbackResult ReplayManager::OnUpdate(ReplayManager *mgr)", 1)[1].split(
    "ChainCallbackResult ReplayManager::OnUpdateDemoLowPrio", 1
)[0]
assert "Netplay::SideEffects::IsSpeculative()" in record_update
assert record_update.index("Netplay::SideEffects::IsSpeculative()") < record_update.index(
    "ReplayExtension::RecordMultiplayerFrame"
)

# Replay selection restores multiplayer session metadata before GameManager
# creates gameplay objects.
assert "ReplayExtension::GetMultiplayerPlaybackConfig(&replayConfig)" in main_menu
assert "MultiplayerGameplay::Configure(session)" in main_menu
assert "session.localPlayer = replayConfig.localPlayer;" in main_menu
assert "session.showContributionStats = replayConfig.showContributionStats;" in main_menu
assert "session.players[playerId].character = replayConfig.characters[playerId]" in main_menu
assert "session.players[playerId].shot = replayConfig.shots[playerId]" in main_menu

# Unlike the historical TH07 replay bug, TH06 GameManager explicitly registers
# every active guest whenever the restored gameplay session is multiplayer.
assert "if (MultiplayerGameplay::IsMultiplayer())" in game_manager
assert "for (u8 playerId = 1; playerId < MultiplayerGameplay::GetPlayerCount(); ++playerId)" in game_manager
assert "Player::RegisterChain(playerId)" in game_manager
player_register = player.split("ZunResult Player::RegisterChain(u8 unk)", 1)[1].split(
    "ZunResult Player::AddedCallback", 1
)[0]
assert "!g_GameManager.isInReplay" not in player_register

# Playback restores every synchronized lane and therefore drives the Player
# objects created above.
demo = manager.split("ChainCallbackResult ReplayManager::OnUpdateDemoHighPrio(ReplayManager *mgr)", 1)[1].split(
    "ChainCallbackResult ReplayManager::OnDraw", 1
)[0]
assert "ReplayExtension::GetMultiplayerPlaybackFrame" in demo
assert "Netplay::Input::SetPlayerInputOverrides" in demo
assert "combinedButtons" in demo

# Multiplayer Replay analog movement must consume those per-player overrides
# while GameManager is in Replay mode. Ordinary Replay's local touch sidecar
# deliberately returns false for multiplayer files, so gating ReplayJoystick /
# ReplayDirectTouch behind !replayPlayback would silently freeze touch-driven
# guests even though their FrameInput lanes were stored correctly.
player_inputs = player.split("ZunResult Player::HandlePlayerInputs()", 1)[1].split(
    "void Player::DrawBullets", 1
)[0]
assert "replayPlayback && ReplayExtension::MultiplayerPlaybackActive() &&\n             Netplay::Input::ReplayJoystick" in player_inputs
assert "replayPlayback && ReplayExtension::MultiplayerPlaybackActive() &&\n              (sampledNetplayTouch = Netplay::Input::ReplayDirectTouch" in player_inputs

# ResultScreen uses the normal replay save entry for multiplayer too; the
# ReplayManager appends the already-recorded EAGX sidecar rather than writing a
# P1-only special file or suppressing the prompt.
assert "ReplayManager::SaveReplay(replayPath, this->replayName);" in result_screen
save = manager.split("void ReplayManager::SaveReplay(const char *replayPath, char *replayName)", 1)[1]
assert "ReplayExtension::AppendRecording(actualReplayPath.c_str())" in save
save_before_extension = save.split("ReplayExtension::AppendRecording(actualReplayPath.c_str())", 1)[0]
assert "MultiplayerGameplay::IsMultiplayer()" not in save_before_extension

# EAGX v1 has one established body order shared with TH07 and LoadPlayback:
# ordinary input, touch events, direct-touch/state, then multiplayer lanes.
# MP-only recordings can hide an ordering bug because the touch sections are
# empty, so pin the combined MP + local-touch layout explicitly.
append = extension.split("bool AppendRecording(const char *path)", 1)[1].split(
    "void ClearPlayback()", 1
)[0]
load = extension.split("bool LoadPlayback(const u8 *bytes, std::size_t size)", 1)[1].split(
    "void SetPlaybackFrame", 1
)[0]
assert append.index("g_RecordTouchEvents[stage].data()") < append.index(
    "g_RecordDirectTouch[stage].data()"
) < append.index("g_RecordMultiplayerInputs[stage].data()")
assert append.index("g_RecordMultiplayerInputs[stage].data()") < append.index(
    "g_RecordMultiplayerStageResources[stage][player]"
 ) < append.index(
    "g_RecordMultiplayerStageContributions[stage][player]"
)
assert load.index("g_PlaybackTouchEvents[stage].data()") < load.index(
    "g_PlaybackDirectTouch[stage].data()"
) < load.index("g_PlaybackMultiplayerInputs[stage].data()")
assert load.index("g_PlaybackMultiplayerInputs[stage].data()") < load.index(
    "g_PlaybackMultiplayerStageResources[stage][player]"
 ) < load.index(
    "g_PlaybackMultiplayerStageContributions[stage][player]"
)

# Direct Stage2+ MP playback must restore guest stage-start resources after the
# vanilla replay loader has restored P1 and after guest sidecars are seeded,
# but before Player objects are registered. Old v1 replays without the optional
# resource feature flag retain the Stage1-compatible P1-clone fallback.
game_added = game_manager.split("ZunResult GameManager::AddedCallback", 1)[1].split(
    "ChainCallbackResult GameManager::OnUpdate", 1
)[0]
assert "ResetMultiplayerPlayerResources();" in game_added
assert "ReplayExtension::GetMultiplayerPlaybackStageResources" in game_added
assert game_added.index("ResetMultiplayerPlayerResources();") < game_added.index(
    "ReplayExtension::GetMultiplayerPlaybackStageResources"
) < game_added.index("Player::RegisterChain(0)")
assert "SetPlayerLives(playerId, resources.lives);" in game_added
assert "SetPlayerBombs(playerId, resources.bombs);" in game_added
assert "SetPlayerPower(playerId, resources.power);" in game_added
assert "ReplayExtension::GetMultiplayerPlaybackStageContributions" in game_added
assert "g_MultiplayerContributionStats[playerId].enemiesDefeated" in game_added
assert "g_MultiplayerContributionStats[playerId].damageDealt" in game_added

# Fixed-tick touch replay keeps the already-validated state-machine rules:
# only DOWN creates a playback finger and orphan motion/up cannot synthesize one.
touch_replay = touch.split("void Touch::ApplyReplayTouchEvent", 1)[1]
assert "TOUCH_ACTION_DOWN" in touch_replay
assert "FindReplayPlaybackFinger" in touch_replay
assert "AcquireReplayPlaybackFinger" in touch_replay
assert touch_replay.index("TOUCH_ACTION_DOWN") < touch_replay.index("AcquireReplayPlaybackFinger")
assert touch_replay.count("if (!slot || !slot->active)\n            return;") >= 2

# Practice/thprac restart reuses GAMEMANAGER_REINIT and therefore cannot rely
# on fresh GameManager/ReplayManager registration to clear replay visualization.
# The restart helper must clear both live/recording touch ownership and the
# separate playback-finger table so a two-finger replay cannot leave ghost
# crosses in the next attempt.
restart_reset = practice.split("static void ResetTouchReplayForPracticeRestart()", 1)[1].split("#endif", 1)[0]
assert "Touch::CancelTouches();" in restart_reset
assert "Touch::ResetReplayRecordingState();" in restart_reset
assert "Touch::ResetReplayTouch();" in restart_reset
assert "ReplayExtension::ResetRecording();" in restart_reset

# Save/config/Replay ownership follows the downloaded binary variant. The MP
# build never falls back to the ordinary IDBFS root merely because it is not in
# a room, while both variants continue to consume the same read-only package,
# font and music URLs.
storage = eagler_options.split("inline bool MultiplayerStorageEnabled()", 1)[1].split(
    "} // namespace EaglerOptions", 1
)[0]
assert "defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY)" in storage
assert "return true;" in storage
assert "netplayMode" not in storage
assert "replayViewer" not in storage
assert 'params.get("runtimeVariant") === "multiplayer" ? "/savesth06-multiplayer" : "/savesth06"' in shell
assert "/assets-multiplayer/" not in shell
assert "/bgm-multiplayer/" not in shell

# Room results must not mutate ordinary score/high-score progression. There are
# two independent barriers: the MP binary owns a different writable root, and
# ResultScreen suppresses persistent result writes only while an actual
# multiplayer gameplay session is active. Ordinary builds compile this helper
# to false and therefore retain the vanilla score.dat path.
persistence_guard = result_screen.split("bool ShouldSkipPersistentResultWrite()", 1)[1].split(
    "}", 1
)[0]
assert "#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY" in persistence_guard
assert "MultiplayerGameplay::IsMultiplayer()" in persistence_guard
assert "return false;" in result_screen.split("bool ShouldSkipPersistentResultWrite()", 1)[1].split(
    "void DrawResultShotTypeText", 1
)[0]
write_score = result_screen.split("void ResultScreen::WriteScore(ResultScreen *resultScreen)", 1)[1].split(
    "i32 ResultScreen::LinkScoreEx", 1
)[0]
assert "if (ShouldSkipPersistentResultWrite())" in write_score
assert write_score.index("if (ShouldSkipPersistentResultWrite())") < write_score.index(
    'FileSystem::WriteDataToFile("score.dat"'
)
result_exit = result_screen.split("ChainCallbackResult ResultScreen::OnUpdate", 1)[1]
assert "if (!ShouldSkipPersistentResultWrite() &&" in result_exit

# The executable Touch self-test also exercises two playback fingers, restart
# clearing, and rejects stale MOTION/UP events after the reset boundary.
touch_selftest = touch.split("bool Touch::DebugStateSelfTest()", 1)[1].split("#endif", 1)[0]
assert "twoReplayFingersVisible" in touch_selftest
assert "replayRestartClearsBoth" in touch_selftest
assert "staleReplayEventsStayCleared" in touch_selftest

print("TH06 multiplayer replay contract: PASS")
