from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def text(path: str) -> str:
    return (ROOT / path).read_text(encoding="utf-8")


cmake = text("CMakeLists.txt")
driver = text("src/netplay/Th06LanStageProbe.cpp")
player = text("src/Player.cpp")
supervisor = text("src/Supervisor.cpp")
window = text("src/GameWindow.cpp")
main = text("src/main.cpp")
rollback = text("src/netplay/Th06RollbackState.cpp")

# Production driver belongs only to the isolated netplay binary.
netplay_block = cmake.split("if(TH_ENABLE_NETPLAY)", 1)[1].split("endif()", 1)[0]
assert "src/netplay/Th06LanStageProbe.cpp" in netplay_block

# Frame zero is a hard real-input barrier. Prediction is allowed only later.
assert "g_SimFrame == 0 && ConfirmedThroughAllRemotes() == INVALID_FRAME" in driver
assert "const FrameDecision decision = g_Core.PrepareFrame(g_SimFrame);" in driver
assert driver.index("g_SimFrame == 0 && ConfirmedThroughAllRemotes() == INVALID_FRAME") < driver.index(
    "const FrameDecision decision = g_Core.PrepareFrame(g_SimFrame);"
)

# Retry/stall retransmits the already scheduled logical sample instead of
# touching keyboard/controller/touch a second time.
assert "bool SendLocalFrame(std::uint32_t frame)" in driver
assert "const FrameInput input = CaptureLocalInput(frame);" in driver
assert "bool SendScheduledLocalFrame(std::uint32_t frame)" in driver
retry_body = driver.split("bool SendScheduledLocalFrame(std::uint32_t frame)\n{", 1)[1].split(
    "bool SendTailKeepalive()", 1
)[0]
assert "CaptureLocalInput" not in retry_body
assert "g_Core.BuildInputPacket" in retry_body

# A stalled network gate owns no simulation time. GameWindow must keep a
# bounded backlog and only subtract targetDt after LastTickAdvanced().
assert "Netplay::Th06LanStageProbe::LastTickAdvanced()" in window
assert "maxNetplayCatchupTicks = 6" in window
stall = window.split("if (Netplay::Th06LanStageProbe::Requested())", 2)[2]
assert "if (!lastSimulationTickAdvanced)" in stall
assert stall.index("if (!lastSimulationTickAdvanced)") < stall.index("this->accumulator -= targetDt;")

# Chain::RunCalcChain() returns 0/-1 for exit and otherwise the number of
# callbacks/jobs updated this tick. Positive values such as 4 or 5 are not
# ChainCallbackResult EXIT enums and must never be interpreted as such by the
# production driver or GameWindow boundary.
assert "CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS" not in driver
assert "CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR" not in driver
assert "res == CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS" not in window
assert "res == CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR" not in window
assert "if (res == 0)" in window
assert "if (res == -1)" in window

# Chain::RunCalcChain() returns 0/-1 for terminal success/error and otherwise
# returns the number of jobs updated. Positive counts such as 4 or 5 are not
# ChainCallbackResult enums and must never be interpreted as exits by the
# production driver/window boundary.
assert "CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS" not in driver
assert "CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR" not in driver
assert "if (res == 0)" in window
assert "if (res == -1)" in window

# Chain::RunCalcChain() normalizes callback EXIT_SUCCESS/EXIT_ERROR to 0/-1;
# every positive return is merely the number of jobs updated this tick. In
# particular 4/5 are perfectly valid normal-frame values and must never be
# reinterpreted as ChainCallbackResult enum values at the driver/window edge.
assert "CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS" not in driver
assert "CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR" not in driver
assert "res == 0" in window
assert "res == -1" in window
assert "res == CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS" not in window
assert "res == CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR" not in window

# Chain::RunCalcChain() returns 0/-1 for real exits and otherwise a positive
# updated-job count. Values 4 and 5 are therefore perfectly valid normal-frame
# counts and must never be interpreted as ChainCallbackResult enum values at
# the production-driver/GameWindow boundary.
assert "CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS" not in driver
assert "CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR" not in driver
assert "CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS" not in window
assert "CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR" not in window
assert "if (result == 0 || result == -1)" in driver
assert "return -1;" in driver

# Browser transport starts during title/menu ticks; gameplay dispatch waits
# until the selected RTC/WS route is actually open.
assert "StartProductionTransportEarly()" in driver
assert "Netplay::Th06LanStageProbe::TransportReady()" in main
assert "Module.eaglerOptions?.netplayMode === 'lan'" in main

# The room, not each machine's persistent Options file, owns gameplay startup
# resources.  Phone/desktop users may have different configured starting
# lives/bombs; LAN dispatch must normalize those before GameManager copies them
# into defaultConfig (which is also used by later death/respawn handling).
netplay_start = main.split("static ChainCallbackResult StartNetplayGame(MainMenu *menu)", 1)[1].split(
    "static void ResumeAudioForActiveWindow()", 1
)[0]
assert "g_GameManager.livesRemaining = 2;" in netplay_start
assert "g_GameManager.bombsRemaining = 3;" in netplay_start
assert "g_GameManager.livesRemaining = g_Supervisor.cfg.lifeCount;" not in netplay_start
assert "g_GameManager.bombsRemaining = g_Supervisor.cfg.bombCount;" not in netplay_start

# Once a LAN run has been requested, TH06 still needs a few vanilla calc ticks
# to construct Stage/Player/ReplayManager before frame zero. Those bootstrap
# ticks must be input-neutral: allowing the local keyboard/touch to move its
# own Player before HELLO/READY makes each peer enter rollback from a different
# world state. Keep initialization running, but install zero logical lanes.
bootstrap_gate = driver.split("bool InitialNetplayBootstrapInProgress()", 1)[1].split(
    "bool SessionStillOwnsStageState()", 1
)[0]
assert "g_Supervisor.wantedState == SUPERVISOR_STATE_GAMEMANAGER" in bootstrap_gate
assert "g_Supervisor.wantedState == SUPERVISOR_STATE_GAMEMANAGER_REINIT" in bootstrap_gate
assert "g_Supervisor.curState == SUPERVISOR_STATE_GAMEMANAGER" not in bootstrap_gate
assert "g_GameManager.currentStage >= 1" not in bootstrap_gate
bootstrap = driver.split("if (!g_Initialized && !EligibleForInitialNetplayStart())", 1)[1].split(
    "if (!g_Initialized)", 1
)[0]
assert "std::array<FrameInput, TH06_MULTI_MAX_PLAYERS> neutralInputs{};" in bootstrap
assert "Input::SetReplayOverride(0);" in bootstrap
assert "Input::SetPlayerInputOverrides(neutralInputs.data(), g_PlayerCount);" in bootstrap
assert "Input::ClearPlayerButtonOverrides();" in bootstrap
assert "Input::ClearReplayOverride();" in bootstrap
assert "if (!InitialNetplayBootstrapInProgress())" in bootstrap
postgame = bootstrap.split("if (!InitialNetplayBootstrapInProgress())", 1)[1].split(
    "std::array<FrameInput, TH06_MULTI_MAX_PLAYERS> neutralInputs{};", 1
)[0]
assert "return g_Chain.RunCalcChain();" in postgame

# Replay stream cursors are committed output, not rewindable gameplay state.
# A rollback resimulation keeps ReplayManager at the forward-pass cursor and
# overwrites only the EAGX multiplayer frame bound to the corrected logical
# frame.
assert "TouchObject(g_ReplayManager)" not in rollback
assert "struct ReplayFrameBinding" in driver
assert "slot.replayFrame = g_ReplayManager->frameId" in driver
assert "ReplayExtension::RecordMultiplayerFrame(" in driver
assert "replayBinding.replayFrame" in driver
assert "decision.inputs.data()" in driver
neutral_bootstrap = bootstrap.split(
    "std::array<FrameInput, TH06_MULTI_MAX_PLAYERS> neutralInputs{};", 1
)[1]
assert neutral_bootstrap.index("Input::SetPlayerInputOverrides") < neutral_bootstrap.index(
    "g_Chain.RunCalcChain()"
)

# The same ownership rule applies to mobile wheel/touch movement. When a
# synchronized player lane is installed, raw machine-local touch must not be a
# fallback source; the synchronized ReplayJoystick/ReplayDirectTouch sample is
# authoritative.
joystick_path = player.split("Netplay::Input::ReplayJoystick", 1)[1].split(
    "ReplayExtension::CaptureJoystick", 1
)[0]
assert "!Netplay::Input::PlayerButtonOverridesActive()" in joystick_path
assert "allowRawTouchFallback" in joystick_path
direct_touch_path = player.split("Netplay::Input::ReplayDirectTouch", 1)[1].split(
    "ReplayExtension::CaptureDirectTouch", 1
)[0]
assert "!Netplay::Input::PlayerButtonOverridesActive()" in direct_touch_path
assert "allowRawTouchFallback" in direct_touch_path
assert "canSampleRawTouch && !MultiplayerGameplay::IsMultiplayer()" in player

# Touch-use history affects TH06's death/deathbomb timing. In multiplayer it
# must come from the synchronized per-player FrameInput lane, never from this
# machine's local Touch singleton.
die_body = player.split("void Player::Die()", 1)[1]
assert "this->respawnTimer = 6 + (PlayerUsedTouch(this) ? Touch::DEATHBOMB_TOLERANCE : 0);" in die_body
assert "allowRawTouchFallback" in joystick_path
assert "allowRawTouchFallback" in direct_touch_path
assert "canSampleRawTouch && !MultiplayerGameplay::IsMultiplayer()" in player

# Once the room has configured multiplayer gameplay, no gap without a netplay
# override may mirror this machine's keyboard/controller into P1's gameplay
# lane.  Raw g_CurFrameInput stays intact for local Result/Menu UI.
assert "const bool multiplayerGameplay = MultiplayerGameplay::IsMultiplayer();" in supervisor
assert "!multiplayerGameplay && playerId == 0 ? g_CurFrameInput : 0" in supervisor

# TH06 room loadouts are deliberately narrower than TH07: only Reimu/Marisa
# and shot A/B, with exactly 2 or 3 stable participants.
assert "return value === 3 ? 3 : 2;" in main
assert "value >= 0 && value <= 1 ? value : $1" in main
assert "value >= 0 && value <= 1 ? value : 0" in main
assert "MultiplayerGameplay::Configure(gameplaySession)" in main

# Chain::RunCalcChain() normalizes callback exit enums internally and returns
# 0 on success-exit, -1 on error-exit, otherwise a positive updated-job count.
# Positive counts can legitimately equal 4/5, so the netplay driver/window
# boundary must never reinterpret those integers as ChainCallbackResult enums.
assert "CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS" not in driver
assert "CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR" not in driver
assert "res == CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS" not in window
assert "res == CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR" not in window
assert "if (res == 0)" in window
assert "if (res == -1)" in window

print("TH06 production netplay driver contract: PASS")
