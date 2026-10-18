#!/usr/bin/env python3
"""Focused contracts for mature sbrik rollback behavior on the Web host."""
from __future__ import annotations

import pathlib
import re
import subprocess


ROOT = pathlib.Path(__file__).resolve().parents[2]
COMMON = ROOT / "third_party" / "eagler-common"
CMAKE = (ROOT / "CMakeLists.txt").read_text(encoding="utf-8")
CORE_H = (COMMON / "include/eagler/netplay/NetplayCore.hpp").read_text(encoding="utf-8")
CORE = (COMMON / "src/netplay/NetplayCore.cpp").read_text(encoding="utf-8")
PROTOCOL = (COMMON / "include/eagler/netplay/NetplayProtocol.hpp").read_text(encoding="utf-8")
PROTOCOL_CONFIG = (ROOT / "src/netplay/NetplayProtocolConfig.hpp").read_text(encoding="utf-8")
DRIVER = (ROOT / "src/netplay/Th07LanStageProbe.cpp").read_text(encoding="utf-8")
PLAYER = (ROOT / "src/Player.cpp").read_text(encoding="utf-8")
ROLLBACK_H = (ROOT / "src/netplay/Th07RollbackState.hpp").read_text(encoding="utf-8")
ROLLBACK = (ROOT / "src/netplay/Th07RollbackState.cpp").read_text(encoding="utf-8")
JOURNAL_H = (COMMON / "include/eagler/netplay/RollbackJournal.hpp").read_text(encoding="utf-8")
JOURNAL = (COMMON / "src/netplay/RollbackJournal.cpp").read_text(encoding="utf-8")
FRAME_BUDGET = (COMMON / "include/eagler/netplay/FrameBudget.hpp").read_text(encoding="utf-8")
INPUT_REPAIR = (COMMON / "include/eagler/netplay/InputRepairBudget.hpp").read_text(encoding="utf-8")
RTC_IMPAIRMENT = (COMMON / "testkit/rtc-input-impairment.cjs").read_text(encoding="utf-8")
SIDE_EFFECTS = (ROOT / "src/netplay/NetplaySideEffects.cpp").read_text(encoding="utf-8")
WINDOW = (ROOT / "src/GameWindow.cpp").read_text(encoding="utf-8")
TRANSPORT = (COMMON / "src/netplay/WebSocketTransport.cpp").read_text(encoding="utf-8")
PEER_TRANSPORT = (COMMON / "src/netplay/BrowserPeerTransport.cpp").read_text(encoding="utf-8")
PEER_TRANSPORT_H = (COMMON / "include/eagler/netplay/BrowserPeerTransport.hpp").read_text(encoding="utf-8")
TRANSPORT_CONFIG = (ROOT / "src/netplay/NetplayTransportConfig.hpp").read_text(encoding="utf-8")
BASE = "5b9ebe892914ff5666ef68c0cd02719dde7d4ee9"
FINAL = "022c533"


def require(name: str, condition: bool) -> None:
    if not condition:
        raise AssertionError(name)
    print(f"PASS: {name}")


def git(*args: str) -> str:
    return subprocess.check_output(["git", *args], cwd=ROOT).decode(
        "utf-8", errors="replace"
    ).replace("\r\n", "\n")


def main() -> None:
    require(
        "ordinary and netplay builds consume separate common CMake surfaces",
        "eagler_common_link_netplay_headers(${TH_EXEC_NAME})" in CMAKE
        and "eagler_common_link_netplay_base(${TH_EXEC_NAME})" in CMAKE
        and "eagler_common_link_browser_peer_transport(${TH_EXEC_NAME})" in CMAKE,
    )
    require(
        "browser peer transport implementation authority lives in eagler-common",
        "src/netplay/BrowserPeerTransport.cpp" not in CMAKE
        and "class BrowserPeerTransport" in PEER_TRANSPORT_H
        and 'Tag[] = "th07"' in TRANSPORT_CONFIG,
    )
    require(
        "rollback journal implementation authority lives in eagler-common",
        "src/netplay/RollbackJournal.cpp" not in CMAKE
        and "std::vector<Block> blocks" in JOURNAL_H,
    )
    require(
        "title-local common authority shims are retired",
        all(not (ROOT / "src/netplay" / retired).exists() for retired in (
            "BrowserPeerTransport.hpp", "BrowserPeerTransport.cpp",
            "NetplayCore.hpp", "NetplayCore.cpp",
            "NetplayInput.hpp", "NetplayInput.cpp",
            "NetplayProtocol.hpp", "NetplayProtocol.cpp",
            "NetplaySession.hpp", "NetplaySession.cpp",
            "RollbackJournal.hpp", "RollbackJournal.cpp",
            "WebSocketTransport.hpp", "WebSocketTransport.cpp",
        )),
    )
    frozen = git(
        "diff", "--unified=0", f"{BASE}..{FINAL}", "--", "src/th07/Netplay.cpp"
    )
    require(
        "retry or new-run gameplay starts a fresh rollback session generation",
        "RetireGameplaySession()" in DRIVER
        and "++g_SessionGeneration" in DRIVER
        and "CurrentSessionId()" in DRIVER
        and "packet.sessionId != CurrentSessionId()" in DRIVER,
    )
    require(
        "upstream Netplay.cpp is one large new-file inventory hunk",
        len(re.findall(r"^@@ ", frozen, re.MULTILINE)) == 1
        and frozen.count("\n+") > 14000,
    )
    shell = CORE + PROTOCOL + DRIVER + ROLLBACK
    require(
        "browser gameplay ABI rejects runtimes without shared difficulty and Ending semantics",
        "constexpr std::uint32_t GAMEPLAY_ABI = TH07_MULTI_NETPLAY_ABI;" in DRIVER,
    )
    require(
        "portable rollback path excludes WinSock threads LowLatency and UDP",
        not any(token in shell for token in ("WinSock", "SOCKET", "CreateThread", "LowLatency", "sockaddr", "sendto(")),
    )
    require(
        "input redundancy matches final upstream thirty-two-frame tail",
        "MAX_REDUNDANT_INPUTS = 32" in PROTOCOL
        and "packet.inputCount < MAX_REDUNDANT_INPUTS" in CORE,
    )
    require(
        "reliable repair uses shared bounded primitives without a title-local compatibility shim",
        '#include <eagler/netplay/InputRepairBudget.hpp>' in DRIVER
        and "StalledMs = 80" in INPUT_REPAIR
        and "RetryMs = 80" in INPUT_REPAIR
        and "SendRepairTo" in PEER_TRANSPORT
        and "inputRepairSent" in PEER_TRANSPORT,
    )
    require(
        "RTC repair impairment is shared and title labels are injected by the harness",
        'inputLabel = options.inputLabel ?? "eagler-input"' in RTC_IMPAIRMENT
        and 'controlLabel = options.controlLabel ?? "eagler-control"' in RTC_IMPAIRMENT,
    )
    require(
        "TH07 protocol capability remains title-owned while codec/core are shared",
        "MagicGame = '7'" in PROTOCOL_CONFIG
        and "MaxAnalogMode = 4" in PROTOCOL_CONFIG,
    )
    require(
        "prediction retains only TH07 held controls",
        "coreConfig.predictableButtons" in DRIVER
        and all(token in DRIVER for token in ("TH_BUTTON_DIRECTION", "TH_BUTTON_FOCUS", "TH_BUTTON_SHOOT", "TH_BUTTON_SKIP"))
        and "predicted.buttons &= config_.predictableButtons" in CORE,
    )
    require(
        "keyboard direction prediction has an independent three-frame horizon",
        "coreConfig.directionButtons = TH_BUTTON_DIRECTION" in DRIVER
        and "coreConfig.maxDirectionPredictionFrames = 3" in DRIVER
        and "distance > config_.maxDirectionPredictionFrames" in CORE,
    )
    require(
        "touch prediction keeps one delta but never repeats touch Bomb",
        "predicted.touchBomb = false" in CORE
        and "AnalogMode::DirectTouch && distance > 1" in CORE
        and "predicted.x = 0.0f" in CORE
        and "predicted.y = 0.0f" in CORE,
    )
    require(
        "frame zero waits for exact remote input after the session barrier",
        "g_SimFrame == 0 && g_Core.ConfirmedThroughAllRemotes() == INVALID_FRAME" in DRIVER
        and "g_Session.CanStart()" in DRIVER,
    )
    bootstrap = DRIVER.split(
        "if (!g_Initialized && !EligibleForInitialNetplayStart())", 1
    )[1].split("if (!g_Initialized)", 1)[0]
    require(
        "pre-frame-zero bootstrap runs with neutral synchronized input lanes",
        "std::array<FrameInput, TH07_MULTI_MAX_PLAYERS> neutralInputs{};" in bootstrap
        and "Input::SetReplayOverride(0);" in bootstrap
        and "Input::SetPlayerInputOverrides(neutralInputs.data(), g_PlayerCount);" in bootstrap
        and bootstrap.split("std::array<FrameInput, TH07_MULTI_MAX_PLAYERS> neutralInputs{};", 1)[1].index(
            "Input::SetPlayerInputOverrides"
        )
        < bootstrap.split("std::array<FrameInput, TH07_MULTI_MAX_PLAYERS> neutralInputs{};", 1)[1].index(
            "g_Chain.RunCalcChain()"
        )
        and "Input::ClearPlayerButtonOverrides();" in bootstrap
        and "Input::ClearReplayOverride();" in bootstrap,
    )
    require(
        "retired post-game scenes regain vanilla UI input",
        "if (!InitialNetplayBootstrapInProgress())" in bootstrap
        and "return g_Chain.RunCalcChain();" in bootstrap.split(
            "if (!InitialNetplayBootstrapInProgress())", 1
        )[1].split("std::array<FrameInput, TH07_MULTI_MAX_PLAYERS> neutralInputs{}", 1)[0],
    )
    require(
        "frame-zero baseline clears local raw and held-input history",
        "g_CurFrameRawInput = 0;" in DRIVER
        and "g_LastFrameRawInput = 0;" in DRIVER
        and "g_IsEighthFrameOfHeldInput = 0;" in DRIVER
        and "g_NumOfFramesInputsWereHeld = 0;" in DRIVER
        and "g_CurFrameGameInputs[playerId] = 0;" in DRIVER
        and "g_LastFrameGameInputs[playerId] = 0;" in DRIVER,
    )
    joystick_path = PLAYER.split("Netplay::Input::ReplayJoystick", 1)[1].split(
        "ReplayExtension::CaptureJoystick", 1
    )[0]
    direct_touch_path = PLAYER.split("Netplay::Input::ReplayDirectTouch", 1)[1].split(
        "ReplayExtension::CaptureDirectTouch", 1
    )[0]
    require(
        "synchronized touch lanes block machine-local joystick and direct-touch fallback",
        "!Netplay::Input::PlayerButtonOverridesActive()" in joystick_path
        and "!Netplay::Input::PlayerButtonOverridesActive()" in direct_touch_path,
    )
    scheduled_sender = re.search(
        r"bool SendScheduledLocalFrame\(std::uint32_t frame\)\s*\{(?P<body>.*?)\n\}",
        DRIVER,
        re.DOTALL,
    )
    require(
        "stalled current input is retried without resampling keyboard controller or touch",
        scheduled_sender is not None
        and "CaptureLocalInput" not in scheduled_sender.group("body")
        and "g_Core.BuildInputPacket" in scheduled_sender.group("body")
        and "localPresent && (g_DriverTicks % 3u) == 0u" in DRIVER
        and "g_Core.HasLocalCapture(g_SimFrame)" in DRIVER
        and "SendScheduledLocalFrame(g_Core.LocalFrameForCapture(g_SimFrame))" in DRIVER,
    )
    require(
        "production input packets carry peer-relative ACK and timing state",
        "for (std::uint8_t peer = 0; peer < g_PlayerCount; ++peer)" in scheduled_sender.group("body")
        and "TransportSendTo(peer" in scheduled_sender.group("body")
        and "g_LastRemoteSenderFrame[peer]" in scheduled_sender.group("body")
        and "packet.ackFrame = INVALID_FRAME" not in scheduled_sender.group("body").split("if (ProductionLanMode())", 1)[1].split("return true;", 1)[0],
    )
    require(
        "multi-peer time sync keeps per-endpoint windows and uses the largest lead",
        "g_PeerTimeSync[packet.senderPlayer]" in DRIVER
        and "g_PeerTimeSync[player].averageLead > recommendedLead" in DRIVER
        and "return ProductionLanMode() && !g_DisableTimeSyncPacing" in DRIVER
        and "? g_SimulationIntervalScale : 1.0;" in DRIVER,
    )
    require(
        "pause retry and transition UI wait for confirmed input",
        "SharedUiNeedsConfirmedInputs()" in DRIVER
        and "g_Core.ConfirmedThroughAllRemotes() < g_SimFrame" in DRIVER,
    )
    require(
        "RunCalcChain positive job counts are never confused with callback exit enums",
        "CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS" not in DRIVER
        and "CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR" not in DRIVER
        and "res == CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS" not in WINDOW
        and "res == CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR" not in WINDOW
        and "if (res == 0)" in WINDOW
        and "if (res == -1)" in WINDOW,
    )
    require(
        "calc-chain return values use 0/-1 exits rather than ChainCallbackResult enum numbers",
        "CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS" not in DRIVER
        and "CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR" not in DRIVER
        and "res == CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS" not in WINDOW
        and "res == CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR" not in WINDOW
        and "if (res == 0)" in WINDOW
        and "if (res == -1)" in WINDOW,
    )
    require(
        "driver respects Chain::RunCalcChain integer return contract",
        "CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS" not in DRIVER
        and "CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR" not in DRIVER
        and "if (res == 0)" in WINDOW
        and "if (res == -1)" in WINDOW,
    )
    require(
        "driver preserves Chain::RunCalcChain integer return contract",
        "CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS" not in DRIVER
        and "CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR" not in DRIVER
        and "res == 0" in WINDOW
        and "res == -1" in WINDOW
        and "res == CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS" not in WINDOW
        and "res == CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR" not in WINDOW,
    )
    require(
        "driver preserves Chain RunCalcChain integer exit contract",
        "CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS" not in DRIVER
        and "CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR" not in DRIVER
        and "CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS" not in WINDOW
        and "CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR" not in WINDOW
        and "if (result == 0 || result == -1)" in DRIVER
        and "return -1;" in DRIVER,
    )
    require(
        "Extra Phantasm and Ending remain inside the synchronized session",
        "stage == 1 || stage == 7 || stage == 8" in DRIVER
        and "g_GameManager.currentStage <= 8" in DRIVER
        and "const bool endingScene = g_Supervisor.curState == 9" in DRIVER,
    )
    require(
        "Ending input is confirmed and shared while local skip history is normalized",
        "g_Supervisor.curState != 2 || g_Supervisor.wantedState != 2" in DRIVER
        and "Input::SetReplayOverride(CombinedButtons(decision))" in DRIVER
        and "NormalizeMultiplayerEndingSkipHistory();" in DRIVER
        and "difficultyClearedWithRetries[difficulty] = 99" in DRIVER
        and "difficultyClearedWithoutRetries[difficulty] = 99" in DRIVER,
    )
    require(
        "rollback journal invalidates history on stage change",
        "if (g_HistoryStage != stage)" in ROLLBACK
        and "ResetHistoryForStage(stage)" in ROLLBACK,
    )
    require(
        "rollback captures deterministic managers lanes and interpolation endpoints",
        all(
            token in ROLLBACK
            for token in (
                "TouchObject(&g_Rng)",
                "TouchObject(&g_CurFrameGameInputs)",
                "TouchObject(&g_LastFrameGameInputs)",
                "TouchObject(&g_AnmManager->prevShakeOffset)",
                "CapturePlayer(&g_Players[playerId])",
                "TouchEffect(&g_EffectManager.effects[i])",
            )
        ),
    )
    require(
        "rollback does not rewind committed replay output cursors",
        "TouchObject(g_ReplayManager)" not in ROLLBACK
        and "TouchObject(&g_ReplayManager->rngSeed)" in ROLLBACK
        and "TouchObject(&g_ReplayManager->replayEventFlags)" in ROLLBACK,
    )
    require(
        "rollback resimulation overwrites the mapped multiplayer Replay frame",
        "struct ReplayFrameBinding" in DRIVER
        and "slot.replayFrame = g_ReplayManager->frameId" in DRIVER
        and "if (!resimulation)" in DRIVER
        and "ReplayExtension::RecordMultiplayerFrame(" in DRIVER
        and "replayBinding.replayFrame" in DRIVER
        and "decision.inputs.data()" in DRIVER,
    )
    require(
        "three-player BombEffect rollback capacity matches final upstream",
        "stateConfig.maxBombEffectsPerFrame = 1024" in DRIVER,
    )
    require(
        "dense rollback overlap lookup remains logarithmic",
        "std::vector<Block> blocks" in JOURNAL_H
        and "std::uint32_t root = NO_BLOCK" in JOURNAL_H
        and "std::uint32_t ancestors[64]" in JOURNAL
        and "RebalanceBlock" in JOURNAL
        and "RotateLeft" in JOURNAL
        and "RotateRight" in JOURNAL,
    )
    require(
        "evicted rollback byte buffers are reused",
        "std::vector<FrameRecord> frames_" in JOURNAL_H
        and "if (frameCount_ == frames_.size())" in JOURNAL
        and "firstFrame_ = (firstFrame_ + 1) % frames_.size()" in JOURNAL
        and "EnsureByteCapacity" in JOURNAL,
    )
    require(
        "default production rollback keeps first-write state across two logical frames and replays from checkpoint start",
        "std::size_t checkpointLogicalFrames = 2;" in ROLLBACK_H
        and "stateConfig.checkpointLogicalFrames = 2;" in DRIVER
        and "(g_FramesInCheckpoint + 1) % g_Config.checkpointLogicalFrames" in ROLLBACK
        and "g_Journal.BeginFrame(frame, true)" in ROLLBACK
        and "Th07Rollback::RestoreTo(rollback, &replayFrom)" in DRIVER
        and "frame = replayFrom" in DRIVER
        and "snapshotInterval" not in DRIVER + ROLLBACK
        and "FindRestoreFrame" not in DRIVER + ROLLBACK,
    )
    require(
        "unavailable old snapshot is abandoned while journal corruption remains fatal",
        "rollback correction abandoned" in DRIVER
        and "if (Th07Rollback::Failed())" in DRIVER
        and "g_Core.ClearRollbackRequest()" in DRIVER,
    )
    require(
        "resimulation suppresses side effects and restores normal forward mode",
        "SideEffects::SetSpeculative(resimulation)" in DRIVER
        and "SideEffects::SetSpeculative(false)" in DRIVER
        and "g_Speculative" in SIDE_EFFECTS,
    )
    require(
        "network stalls keep bounded presentation catch-up",
        "MaxCatchupTicks = 6" in FRAME_BUDGET
        and "static_cast<i32>(Netplay::FrameBudget::MaxCatchupTicks)" in WINDOW
        and "Netplay::FrameBudget::CanStartTick(" in WINDOW
        and "SDL_GetTicksNS() - catchupStartNs" in WINDOW
        and "completedTicks == 0 || elapsedNs < CatchupBudgetNs" in FRAME_BUDGET
        and "lastSimulationTickAdvanced" in WINDOW,
    )
    require(
        "clean remote WebSocket close is surfaced instead of freezing silently",
        "if (!self->closing_ && !self->failed_)" in TRANSPORT
        and "remote WebSocket closed" in TRANSPORT
        and "RemoteInputsTimedOut()" in DRIVER
        and 'Fail("remote input timeout")' in DRIVER,
    )
    require(
        "reliable session control is rate-limited during slow-peer restart",
        "SESSION_RETRY_TICKS = 15" in DRIVER
        and "g_LastHelloSendTick" in DRIVER
        and "g_LastReadySendTick" in DRIVER,
    )
    require(
        "browser peer receive queue dequeues without Array.shift quadratic backlog",
        "receivedHead" in PEER_TRANSPORT
        and "state.receivedHead = head + 1" in PEER_TRANSPORT
        and "state.received?.shift()" not in PEER_TRANSPORT,
    )
    require(
        "hash resync and automatic repair are absent from rollback core",
        not any(token in CORE + ROLLBACK for token in ("ApplyRngResync", "SendPendingResync", "compatibilityHash", "automatic repair")),
    )


if __name__ == "__main__":
    main()
