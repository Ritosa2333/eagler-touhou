from __future__ import annotations

import os
import socket
import subprocess
import sys
import time
from pathlib import Path

from playwright.sync_api import sync_playwright


ROOT = Path(__file__).resolve().parents[1]
WORKSPACE = ROOT.parent
RELAY_ROOT = WORKSPACE / "th07-eagler" / "tools" / "netplay"


def free_port() -> int:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.bind(("127.0.0.1", 0))
        return int(sock.getsockname()[1])


def wait_http(url: str, timeout: float = 10.0) -> None:
    import urllib.request

    deadline = time.time() + timeout
    while time.time() < deadline:
        try:
            with urllib.request.urlopen(url, timeout=0.5) as response:
                if response.status < 400:
                    return
        except Exception:
            time.sleep(0.1)
    raise RuntimeError(f"HTTP server did not start: {url}")


def wait_relay(process: subprocess.Popen[str], timeout: float = 10.0) -> None:
    assert process.stdout is not None
    deadline = time.time() + timeout
    while time.time() < deadline:
        line = process.stdout.readline()
        if line:
            print(f"RELAY {line.rstrip()}")
            if "LAN relay listening" in line:
                return
        elif process.poll() is not None:
            raise RuntimeError(f"relay exited early: {process.returncode}")
        else:
            time.sleep(0.05)
    raise RuntimeError("relay did not start")


def runtime_snapshot(page, target_frame: int = 300):
    return page.evaluate(
        """target => {
          const runtime = document.getElementById('runtime')?.contentWindow;
          const key = String(target);
          return {
            launched: !!globalThis.__th06SmokeLaunched,
            hostFailure: String(globalThis.__th06SmokeFailure || ''),
            active: !!runtime?.__eaglerNetplayLanActive,
            frame: Number(runtime?.__eaglerNetplayLanFrame || 0),
            confirmed: Number(runtime?.__eaglerNetplayLanConfirmed ?? -1),
            rollback: Number(runtime?.__eaglerNetplayLanRollback ?? 0),
            resimulated: Number(runtime?.__eaglerNetplayLanResimulated ?? 0),
            hashTarget: String(runtime?.__eaglerNetplayLanHashes?.[key] || ''),
            canonicalTarget: runtime?.__eaglerNetplayLanCanonical?.[key] || null,
            canonicalDetail: runtime?.__eaglerNetplayLanCanonicalDetail?.[key] || null,
            player0Detail: runtime?.__eaglerNetplayLanPlayer0Detail || null,
            localDebugTarget: String(runtime?.__eaglerNetplayLanLocalDebugHashes?.[key] || ''),
            transport: String(runtime?.__eaglerNetplayTransport || ''),
            path: String(runtime?.__eaglerNetplayPath || ''),
            failed: !!runtime?.__eaglerNetplayFailed,
            error: String(runtime?.__eaglerNetplayError || ''),
            build: String(runtime?.__eaglerNetplayRuntimeBuild || ''),
            generation: Number(runtime?.__eaglerNetplayLanGeneration || 0),
            pauseState: Array.isArray(runtime?.__eaglerNetplayPauseState)
              ? [...runtime.__eaglerNetplayPauseState]
              : [],
            playerStates: Array.isArray(runtime?.__eaglerNetplayPlayerStates)
              ? [...runtime.__eaglerNetplayPlayerStates]
              : [],
            contributionKills: Array.isArray(runtime?.__eaglerNetplayContributionKills)
              ? [...runtime.__eaglerNetplayContributionKills]
              : [],
            contributionDamage: Array.isArray(runtime?.__eaglerNetplayContributionDamage)
              ? [...runtime.__eaglerNetplayContributionDamage]
              : [],
            highestStage: Number(runtime?.__eaglerNetplayLanHighestStage || 0),
            stageTransition: !!runtime?.__eaglerNetplayLanStageTransitionObserved,
            singleElimination: !!runtime?.__eaglerNetplaySingleEliminationObserved,
            coopRevivable: !!runtime?.__eaglerNetplayCoopRevivableObserved,
            coopRevive: !!runtime?.__eaglerNetplayCoopReviveObserved,
            coopPowerTransfer: !!runtime?.__eaglerNetplayCoopPowerTransferObserved,
            teamWipe: !!runtime?.__eaglerNetplayTeamWipeObserved,
            teamWipeGrace: !!runtime?.__eaglerNetplayTeamWipeGraceObserved,
            teamWipeRetry: !!runtime?.__eaglerNetplayTeamWipeRetryObserved,
            teamWipeStartFrame: Number(runtime?.__eaglerNetplayTeamWipeStartFrame ?? -1),
            teamWipeRetryFrame: Number(runtime?.__eaglerNetplayTeamWipeRetryFrame ?? -1),
            retryContinue: !!runtime?.__eaglerNetplayRetryContinueObserved,
            retryResourcesReset: !!runtime?.__eaglerNetplayRetryResourcesResetObserved,
            retryContinueFrame: Number(runtime?.__eaglerNetplayRetryContinueFrame ?? -1),
            secondTeamWipeGrace: !!runtime?.__eaglerNetplaySecondTeamWipeGraceObserved,
            secondTeamWipeRetryFrame: Number(runtime?.__eaglerNetplaySecondTeamWipeRetryFrame ?? -1),
            secondRetryContinue: !!runtime?.__eaglerNetplaySecondRetryContinueObserved,
            secondRetryContinueFrame: Number(runtime?.__eaglerNetplaySecondRetryContinueFrame ?? -1),
            retryResetCount: Number(runtime?.__eaglerNetplayRetryResetCount || 0),
            retryResetVerifiedCount: Number(runtime?.__eaglerNetplayRetryResetVerifiedCount || 0),
            endingEntered: !!runtime?.__eaglerNetplayEndingEntered,
            endingCompleted: !!runtime?.__eaglerNetplayEndingCompleted,
            resultEntered: !!runtime?.__eaglerNetplayResultEntered,
            resultCompleted: !!runtime?.__eaglerNetplayResultCompleted,
            resultState: Number(runtime?.__eaglerNetplayResultState ?? -1),
            replaySaved: !!runtime?.__eaglerNetplayReplaySaved,
            replaySavedPath: String(runtime?.__eaglerNetplayReplaySavedPath || ''),
            replaySavedStoragePath: String(runtime?.__eaglerNetplayReplaySavedStoragePath || ''),
            replayPlaybackObserved: !!runtime?.__eaglerNetplayReplayPlaybackObserved,
            replayPlaybackFrame: Number(runtime?.__eaglerNetplayReplayPlaybackFrame ?? -1),
            replayPlaybackPlayerCount: Number(runtime?.__eaglerNetplayReplayPlaybackPlayerCount || 0),
            replayPlaybackIndependentInputs: !!runtime?.__eaglerNetplayReplayPlaybackIndependentInputs,
            replayPlaybackInputs: [
              Number(runtime?.__eaglerNetplayReplayPlaybackInput0 || 0),
              Number(runtime?.__eaglerNetplayReplayPlaybackInput1 || 0),
              Number(runtime?.__eaglerNetplayReplayPlaybackInput2 || 0),
            ],
            replayPlaybackStage: Number(runtime?.__eaglerNetplayReplayPlaybackStage || 0),
            replayExpectedFrames: Number(runtime?.__eaglerNetplayReplayExpectedFrames || 0),
            replayComparedFrames: Number(runtime?.__eaglerNetplayReplayComparedFrames || 0),
            replayInputMismatch: !!runtime?.__eaglerNetplayReplayInputMismatch,
            replayInputCoverage: Number(runtime?.__eaglerNetplayReplayInputCoverage || 0),
            receiveBacklog: Math.max(0,
              Number(runtime?.__th06PeerTransport?.received?.length || 0) -
              Number(runtime?.__th06PeerTransport?.receivedHead || 0)),
            pendingSignals: Number(runtime?.__th06PeerTransport?.pendingSignals?.length || 0),
            sendBuffered: (() => {
              const transport = runtime?.__th06PeerTransport;
              if (!transport) return 0;
              let total = Number(transport.relay?.bufferedAmount || 0);
              for (const peer of transport.peers?.values?.() || []) {
                total += Number(peer.controlDc?.bufferedAmount || 0);
                total += Number(peer.inputDc?.bufferedAmount || 0);
              }
              return total;
            })(),
            denseRollback: !!runtime?.__eaglerNetplayDenseRollback,
            maxSnapshotBytes: Number(runtime?.__eaglerNetplayMaxSnapshotBytes || 0),
            maxSnapshotBlocks: Number(runtime?.__eaglerNetplayMaxSnapshotBlocks || 0),
            transientDisconnectStarted: !!runtime?.__th06PeerTransport?.__smokeTransientDisconnect?.started,
            transientDisconnectRecovered: !!runtime?.__th06PeerTransport?.__smokeTransientDisconnect?.recovered,
            transientDisconnectRestartRequests: Number(runtime?.__th06PeerTransport?.__smokeTransientDisconnect?.restartRequests || 0),
            transientDisconnectStartFrame: Number(runtime?.__th06PeerTransport?.__smokeTransientDisconnect?.startFrame ?? -1),
            transientDisconnectRecoveredFrame: Number(runtime?.__th06PeerTransport?.__smokeTransientDisconnect?.recoveredFrame ?? -1),
            transientDisconnectError: String(runtime?.__th06PeerTransport?.__smokeTransientDisconnect?.error || ''),
          };
        }""",
        target_frame,
    )


def pulse_runtime_key(page, key: str, code: str, hold: float = 0.05) -> None:
    page.evaluate(
        """({ key, code }) => {
          const runtime = document.getElementById('runtime')?.contentWindow;
          runtime?.dispatchEvent(new KeyboardEvent('keydown', {
            key, code, bubbles: true, cancelable: true,
          }));
        }""",
        {"key": key, "code": code},
    )
    time.sleep(hold)
    page.evaluate(
        """({ key, code }) => {
          const runtime = document.getElementById('runtime')?.contentWindow;
          runtime?.dispatchEvent(new KeyboardEvent('keyup', {
            key, code, bubbles: true, cancelable: true,
          }));
        }""",
        {"key": key, "code": code},
    )


def run_smoke(
    player_count: int,
    force_relay: bool,
    *,
    target_frame: int = 300,
    dynamic_input: bool = False,
    dynamic_input_player: int = -1,
    touch_input: bool = False,
    touch_input_player: int = -1,
    relay_delay_ms: int = 0,
    relay_jitter_ms: int = 0,
    relay_drop_every: int = 0,
    relay_drop_first_input_per_edge: bool = False,
    relay_drop_input_latest_from: int = -1,
    relay_drop_input_latest_to: int = -1,
    route_skew_player: int = -1,
    route_skew_ms: int = 0,
    require_rollback: bool = False,
    rollback_audit: bool = False,
    dense_rollback_profile: bool = False,
    scripted_input: bool = False,
    scripted_stress_input: bool = False,
    require_contribution_stats: bool = False,
    stage_transition: bool = False,
    elimination_cycle: bool = False,
    coop_transfer_cycle: bool = False,
    pause_cycle: bool = False,
    restart_cycle: bool = False,
    retry_continue_cycle: bool = False,
    rtc_recovery_cycle: bool = False,
    transient_disconnect_cycle: bool = False,
    quit_cycle: bool = False,
    ending_cycle: bool = False,
    replay_playback_cycle: bool = False,
    replay_playback_target: int = 300,
    cpu_throttle_rate: int = 1,
    report_backlog: bool = False,
    loadout_profile: str = "default",
) -> None:
    if player_count not in (2, 3):
        raise ValueError("player_count must be 2 or 3")
    http_port = free_port()
    relay_port = free_port()
    while relay_port == http_port:
        relay_port = free_port()
    room = f"th06-smoke-{player_count}p-{'relay' if force_relay else 'rtc'}-{int(time.time() * 1000)}"
    env = os.environ.copy()
    env.update({
        "TH07_RELAY_HOST": "127.0.0.1",
        "TH07_RELAY_PORT": str(relay_port),
        "TH07_RTC_TIMEOUT_MS": "4500",
        "TH07_STUN_URLS": "",
        "TH07_RELAY_DELAY_MS": str(relay_delay_ms),
        "TH07_RELAY_JITTER_MS": str(relay_jitter_ms),
        "TH07_RELAY_DROP_EVERY": str(relay_drop_every),
        "TH07_RELAY_DROP_FIRST_INPUT_PER_EDGE": "1" if relay_drop_first_input_per_edge else "0",
        "TH07_RELAY_DROP_INPUT_LATEST_FROM": str(relay_drop_input_latest_from),
        "TH07_RELAY_DROP_INPUT_LATEST_TO": str(relay_drop_input_latest_to),
        "TH07_TEST_ROUTE_SKEW_PLAYER": str(route_skew_player),
        "TH07_TEST_ROUTE_SKEW_MS": str(route_skew_ms),
    })
    http = subprocess.Popen(
        [sys.executable, "-m", "http.server", str(http_port), "--bind", "127.0.0.1"],
        cwd=ROOT,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        text=True,
    )
    relay = subprocess.Popen(
        ["node", "lan-relay.cjs"],
        cwd=RELAY_ROOT,
        env=env,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        bufsize=1,
    )
    browsers = []
    try:
        wait_http(f"http://127.0.0.1:{http_port}/tests/netplay-browser-host.html")
        wait_relay(relay)
        with sync_playwright() as pw:
            for _ in range(player_count):
                browsers.append(pw.chromium.launch(
                    headless=True,
                    args=[
                        "--autoplay-policy=no-user-gesture-required",
                        "--disable-background-timer-throttling",
                        "--disable-backgrounding-occluded-windows",
                        "--disable-renderer-backgrounding",
                    ],
                ))
            pages = []
            cdp_sessions = []
            failures = [""] * player_count
            for index, browser in enumerate(browsers):
                context = browser.new_context(viewport={"width": 960, "height": 720})
                if force_relay:
                    context.add_init_script("delete globalThis.RTCPeerConnection")
                page = context.new_page()
                page.on("pageerror", lambda error, i=index: failures.__setitem__(i, f"pageerror: {error}"))
                page.on("console", lambda message, i=index: (
                    print(f"P{i + 1} {message.text}")
                    if "netplay" in message.text.lower() or message.type == "error" else None
                ))
                if index == player_count - 1 and cpu_throttle_rate > 1:
                    cdp = context.new_cdp_session(page)
                    cdp.send("Emulation.setCPUThrottlingRate", {"rate": cpu_throttle_rate})
                    cdp_sessions.append(cdp)
                pages.append(page)

            for index, page in enumerate(pages):
                relay_url = f"ws://127.0.0.1:{relay_port}/?room={room}&player={index}"
                use_dynamic_input = dynamic_input and (
                    dynamic_input_player < 0 or index == dynamic_input_player
                )
                use_touch_input = touch_input and (
                    touch_input_player < 0 or index == touch_input_player
                )
                input_mode = "dynamic" if use_dynamic_input else "touch" if use_touch_input else "none"
                host_url = (
                    f"http://127.0.0.1:{http_port}/tests/netplay-browser-host.html"
                    f"?player={index}&players={player_count}&relay={relay_url}"
                    f"&frames={target_frame}"
                    f"&input={input_mode}"
                    f"&scripted={'1' if scripted_input else '0'}"
                    f"&stress={'1' if scripted_stress_input else '0'}"
                    f"&audit={'1' if rollback_audit else '0'}"
                    f"&dense={'1' if dense_rollback_profile else '0'}"
                    f"&stage={'1' if stage_transition else '0'}"
                    f"&eliminate={'1' if elimination_cycle else '0'}"
                    f"&coop={'1' if coop_transfer_cycle else '0'}"
                    f"&pause={'1' if pause_cycle else '0'}"
                    f"&restart={'1' if restart_cycle else '0'}"
                    f"&retry={'1' if retry_continue_cycle else '0'}"
                    f"&quit={'1' if quit_cycle else '0'}"
                    f"&ending={'1' if ending_cycle else '0'}"
                    f"&replayplay={'1' if replay_playback_cycle else '0'}"
                    f"&dev={'1' if replay_playback_cycle else '0'}"
                    f"&loadouts={loadout_profile}"
                )
                page.goto(host_url, wait_until="load", timeout=30_000)

            test_timeout = max(
                60.0,
                165.0 if ending_cycle else 0.0,
                120.0 if target_frame > 300 else 60.0,
                (target_frame / 60.0) * 1.6 + 30.0,
            )
            # Keep every individual gate below the project's three-minute
            # ceiling, including browser teardown and failure reporting.
            deadline = time.time() + min(test_timeout, 165.0)
            snapshots = None
            pause_seen = [False] * player_count
            resumed_seen = [False] * player_count
            generation_seen = [0] * player_count
            active_seen = [False] * player_count
            ending_last_pulse = [0.0] * player_count
            result_last_state = [-1] * player_count
            result_state_since = [0.0] * player_count
            result_last_pulse = [0.0] * player_count
            replay_name_pulses = [0] * player_count
            peak_receive_backlog = [0] * player_count
            peak_pending_signals = [0] * player_count
            peak_send_buffered = [0] * player_count
            replay_viewer_armed = [False] * player_count
            replay_menu_last_pulse = [0.0] * player_count
            rtc_recovery_triggered = False
            rtc_recovery_completed = False
            transient_disconnect_triggered = False
            transient_disconnect_completed = False
            while time.time() < deadline:
                if any(failures):
                    break
                snapshots = [runtime_snapshot(page, target_frame) for page in pages]
                for index, value in enumerate(snapshots):
                    if any(value["pauseState"]):
                        pause_seen[index] = True
                    elif pause_seen[index]:
                        resumed_seen[index] = True
                    generation_seen[index] = max(generation_seen[index], value["generation"])
                    active_seen[index] = active_seen[index] or value["active"]
                    peak_receive_backlog[index] = max(peak_receive_backlog[index], value["receiveBacklog"])
                    peak_pending_signals[index] = max(peak_pending_signals[index], value["pendingSignals"])
                    peak_send_buffered[index] = max(peak_send_buffered[index], value["sendBuffered"])
                if ending_cycle:
                    now = time.time()
                    for index, (page, value) in enumerate(zip(pages, snapshots)):
                        if value["endingEntered"] and not value["endingCompleted"]:
                            if now - ending_last_pulse[index] >= 0.15:
                                pulse_runtime_key(page, "z", "KeyZ")
                                ending_last_pulse[index] = time.time()
                            continue
                        if replay_playback_cycle and value["replaySaved"] and not replay_viewer_armed[index]:
                            # Arm Replay Viewer only after SaveReplay succeeded.
                            # ResultScreen remains in EXITING for 60 frames, so
                            # the next real MainMenu::RegisterChain observes the
                            # option without disturbing the initial LAN launch.
                            replay_viewer_armed[index] = bool(page.evaluate(
                                """() => {
                                  const runtime = document.getElementById('runtime')?.contentWindow;
                                  if (!runtime?.Module?.eaglerOptions) return false;
                                  runtime.Module.eaglerOptions.replayViewer = true;
                                  return true;
                                }"""
                            ))
                        if (
                            replay_playback_cycle and value["resultCompleted"] and
                            value["replaySaved"] and not value["replayPlaybackObserved"]
                        ):
                            # replayViewer returns this same runtime to the real
                            # MainMenu Replay list. Periodic clean Select edges
                            # choose slot 1, then its first recorded stage; any
                            # later Z edges are replay-capture input and cannot
                            # alter the playback payload.
                            if now - replay_menu_last_pulse[index] >= 1.0:
                                pulse_runtime_key(page, "z", "KeyZ")
                                replay_menu_last_pulse[index] = time.time()
                            continue
                        if not value["resultEntered"] or value["replaySaved"]:
                            continue
                        state = value["resultState"]
                        if state != result_last_state[index]:
                            result_last_state[index] = state
                            result_state_since[index] = now
                            result_last_pulse[index] = 0.0
                            if state != 13:
                                replay_name_pulses[index] = 0
                        elapsed = now - result_state_since[index]
                        if state == 9 and elapsed >= 0.75 and result_last_pulse[index] == 0.0:
                            # High-score keyboard -> real Stats screen.
                            pulse_runtime_key(page, "Escape", "Escape")
                            result_last_pulse[index] = time.time()
                        elif state == 15 and elapsed >= 1.75 and result_last_pulse[index] == 0.0:
                            # Stats -> Stats-to-save transition.
                            pulse_runtime_key(page, "z", "KeyZ")
                            result_last_pulse[index] = time.time()
                        elif state == 10 and elapsed >= 0.6 and result_last_pulse[index] == 0.0:
                            # SAVE REPLAY? defaults to Yes.
                            pulse_runtime_key(page, "z", "KeyZ")
                            result_last_pulse[index] = time.time()
                        elif state in (12, 14) and elapsed >= 0.5 and result_last_pulse[index] == 0.0:
                            # Select slot 1; if a stale slot exists, accept overwrite.
                            pulse_runtime_key(page, "z", "KeyZ")
                            result_last_pulse[index] = time.time()
                        elif state == 13 and elapsed >= 0.6 and replay_name_pulses[index] < 9:
                            if now - result_last_pulse[index] >= 0.15:
                                # Eight characters advance the cursor to END;
                                # the ninth confirmation calls ReplayManager::SaveReplay.
                                pulse_runtime_key(page, "z", "KeyZ")
                                replay_name_pulses[index] += 1
                                result_last_pulse[index] = time.time()
                if replay_playback_cycle and not ending_cycle:
                    now = time.time()
                    for index, (page, value) in enumerate(zip(pages, snapshots)):
                        if value["replaySaved"] and not replay_viewer_armed[index]:
                            replay_viewer_armed[index] = bool(page.evaluate(
                                """() => {
                                  const runtime = document.getElementById('runtime')?.contentWindow;
                                  if (!runtime?.Module?.eaglerOptions) return false;
                                  runtime.Module.eaglerOptions.replayViewer = true;
                                  return true;
                                }"""
                            ))
                        if value["replaySaved"] and not value["replayPlaybackObserved"]:
                            if now - replay_menu_last_pulse[index] >= 1.0:
                                pulse_runtime_key(page, "z", "KeyZ")
                                replay_menu_last_pulse[index] = now
                if (
                    rtc_recovery_cycle and not force_relay and
                    not rtc_recovery_triggered and
                    all(value["transport"] == "rtc" and value["frame"] >= 300 for value in snapshots)
                ):
                    rtc_recovery_triggered = bool(pages[0].evaluate(
                        """() => {
                          const runtime = document.getElementById('runtime')?.contentWindow;
                          const transport = runtime?.__th06PeerTransport;
                          if (!transport || transport.route !== 'rtc') return false;
                          const peerIds = [...transport.peers.keys()].sort((a, b) => a - b);
                          if (!peerIds.length || typeof transport.restartPeerIce !== 'function') return false;
                          if (!transport.__smokeOriginalRestartPeerIce) {
                            transport.__smokeOriginalRestartPeerIce = transport.restartPeerIce.bind(transport);
                            transport.restartPeerIce = async peerId => {
                              transport.__smokeIceRestartCalls = Number(transport.__smokeIceRestartCalls || 0) + 1;
                              return transport.__smokeOriginalRestartPeerIce(peerId);
                            };
                          }
                          transport.__smokeSignalBefore = transport.signal;
                          try { transport.signal?.close(4000, 'smoke signaling reconnect'); } catch {}
                          transport.restartPeerIce(peerIds[0]).catch(error => {
                            transport.__smokeRecoveryError = String(error);
                          });
                          return true;
                        }"""
                    ))
                if rtc_recovery_triggered and not rtc_recovery_completed:
                    rtc_recovery_completed = bool(pages[0].evaluate(
                        """() => {
                          const runtime = document.getElementById('runtime')?.contentWindow;
                          const transport = runtime?.__th06PeerTransport;
                          if (!transport || transport.failed || transport.__smokeRecoveryError) return false;
                          return Number(transport.__smokeIceRestartCalls || 0) >= 1 &&
                            !!transport.signal &&
                            transport.signal !== transport.__smokeSignalBefore &&
                            transport.signal.readyState === WebSocket.OPEN;
                        }"""
                    ))
                if (
                    transient_disconnect_cycle and not force_relay and
                    not transient_disconnect_triggered and
                    all(value["transport"] == "rtc" and value["frame"] >= 120 for value in snapshots)
                ):
                    injected = []
                    for page in pages:
                        injected.append(bool(page.evaluate(
                            """() => {
                              const runtime = document.getElementById('runtime')?.contentWindow;
                              const transport = runtime?.__th06PeerTransport;
                              if (!transport || transport.route !== 'rtc') return false;
                              const peerId = [...transport.peers.keys()].sort((a, b) => a - b)[0];
                              const peer = transport.peers.get(peerId);
                              if (!peer || typeof transport.schedulePeerRecovery !== 'function') return false;
                              try {
                                if (!transport.__smokeOriginalRequestPeerIceRestart) {
                                  transport.__smokeOriginalRequestPeerIceRestart = transport.requestPeerIceRestart.bind(transport);
                                  transport.requestPeerIceRestart = id => {
                                    transport.__smokeTransientRestartCalls = Number(transport.__smokeTransientRestartCalls || 0) + 1;
                                    return transport.__smokeOriginalRequestPeerIceRestart(id);
                                  };
                                }
                                transport.__smokeTransientRestartCalls = 0;
                                transport.__smokeTransientRestored = false;
                                transport.__smokeTransientError = '';
                                transport.__smokeTransientPeerId = peerId;
                                transport.__smokeTransientStartFrame = Number(runtime.__eaglerNetplayLanFrame || 0);
                                Object.defineProperty(peer.pc, 'connectionState', {
                                  configurable: true,
                                  get: () => transport.__smokeTransientActive ? 'disconnected' : 'connected',
                                });
                                Object.defineProperty(peer.pc, 'iceConnectionState', {
                                  configurable: true,
                                  get: () => transport.__smokeTransientActive ? 'disconnected' : 'connected',
                                });
                                transport.__smokeTransientActive = true;
                                transport.schedulePeerRecovery(peerId).catch(error => {
                                  transport.__smokeTransientError = String(error);
                                });
                                setTimeout(() => {
                                  transport.__smokeTransientActive = false;
                                  try { delete peer.pc.connectionState; } catch {}
                                  try { delete peer.pc.iceConnectionState; } catch {}
                                  transport.__smokeTransientRestored = true;
                                }, 3200);
                                return true;
                              } catch (error) {
                                transport.__smokeTransientError = String(error);
                                return false;
                              }
                            }"""
                        )))
                    transient_disconnect_triggered = all(injected)
                if transient_disconnect_triggered and not transient_disconnect_completed:
                    checks = [page.evaluate(
                        """() => {
                          const runtime = document.getElementById('runtime')?.contentWindow;
                          const transport = runtime?.__th06PeerTransport;
                          if (!transport || transport.failed || transport.__smokeTransientError) return false;
                          const peer = transport.peers.get(transport.__smokeTransientPeerId);
                          const start = Number(transport.__smokeTransientStartFrame || 0);
                          return !!transport.__smokeTransientRestored &&
                            Number(transport.__smokeTransientRestartCalls || 0) === 0 &&
                            !peer?.recoveryTimer &&
                            Number(runtime.__eaglerNetplayLanFrame || 0) >= start + 180;
                        }"""
                    ) for page in pages]
                    transient_disconnect_completed = all(bool(value) for value in checks)
                if any(value["hostFailure"] or value["failed"] for value in snapshots):
                    break
                if quit_cycle and all(active_seen) and all(
                    value["generation"] >= 1 and not value["active"] for value in snapshots
                ):
                    break
                if (elimination_cycle or retry_continue_cycle) and all(
                    active_seen[index] and value["generation"] >= 1 and not value["active"] and
                    value["resultEntered"]
                    for index, value in enumerate(snapshots)
                ):
                    break
                if ending_cycle and all(
                    active_seen[index] and value["generation"] >= 1 and not value["active"] and
                    value["endingEntered"] and value["endingCompleted"] and
                    value["resultEntered"] and value["resultCompleted"] and
                    value["replaySaved"] and
                    (
                        not replay_playback_cycle or
                        (value["replayPlaybackObserved"] and value["replayPlaybackFrame"] >= 300 and
                         value["replayPlaybackPlayerCount"] == player_count and
                         value["replayPlaybackIndependentInputs"] and
                         value["replayPlaybackStage"] == 6)
                    )
                    for index, value in enumerate(snapshots)
                ):
                    break
                if replay_playback_cycle and not ending_cycle and all(
                    active_seen[index] and value["generation"] >= 1 and not value["active"] and
                    value["replaySaved"] and value["replayPlaybackObserved"] and
                    value["replayPlaybackFrame"] >= replay_playback_target and
                    value["replayPlaybackPlayerCount"] == player_count and
                    value["replayPlaybackIndependentInputs"] and
                    value["replayPlaybackStage"] == 1
                    for index, value in enumerate(snapshots)
                ):
                    break
                if not ending_cycle and not replay_playback_cycle and all(
                    value["launched"] and value["active"] and
                    value["frame"] >= target_frame and
                    value["confirmed"] >= target_frame - 1 and value["hashTarget"]
                    for value in snapshots
                ):
                    break
                time.sleep(0.1)

            snapshots = snapshots or [runtime_snapshot(page, target_frame) for page in pages]
            if any(failures):
                raise RuntimeError(f"page failure: {failures}")
            if any(value["hostFailure"] or value["failed"] for value in snapshots):
                raise RuntimeError(f"runtime failure: {snapshots}")
            if report_backlog:
                print(
                    "TH06 browser netplay backlog: "
                    f"recv={'/'.join(str(v) for v in peak_receive_backlog)} "
                    f"signal={'/'.join(str(v) for v in peak_pending_signals)} "
                    f"send={'/'.join(str(v) for v in peak_send_buffered)} "
                    f"finalRecv={'/'.join(str(value['receiveBacklog']) for value in snapshots)}"
                )
            if quit_cycle:
                if not all(active_seen) or not all(
                    value["generation"] >= 1 and not value["active"] for value in snapshots
                ):
                    raise RuntimeError(
                        f"gameplay session did not retire after synchronized Quit: "
                        f"active_seen={active_seen} generation={generation_seen} snapshots={snapshots}"
                    )
                print(
                    f"TH06 browser netplay retire smoke: PASS players={player_count} "
                    f"generation={'/'.join(str(value['generation']) for value in snapshots)} "
                    f"rollback={'/'.join(str(value['rollback']) for value in snapshots)}"
                )
                return
            if elimination_cycle or retry_continue_cycle:
                no_continue_complete = all(
                    active_seen[index] and value["generation"] >= 1 and not value["active"] and
                    value["singleElimination"] and value["teamWipe"] and
                    value["teamWipeGrace"] and value["teamWipeRetry"] and
                    value["teamWipeRetryFrame"] - value["teamWipeStartFrame"] == 180 and
                    value["resultEntered"] and not value["retryContinue"] and
                    not value["retryResourcesReset"] and
                    (len(value["pauseState"]) <= 1 or value["pauseState"][1] == 0)
                    for index, value in enumerate(snapshots)
                )
                if not no_continue_complete:
                    raise RuntimeError(
                        f"multiplayer no-Continue lifecycle did not complete: "
                        f"active_seen={active_seen} generation={generation_seen} snapshots={snapshots}"
                    )
                print(
                    f"TH06 browser multiplayer no-Continue smoke: PASS players={player_count} "
                    f"resultFrame={'/'.join(str(value['teamWipeRetryFrame']) for value in snapshots)} "
                    f"generation={'/'.join(str(value['generation']) for value in snapshots)}"
                )
                return
            if ending_cycle:
                lifecycle_complete = all(
                    active_seen[index] and value["generation"] >= 1 and not value["active"] and
                    value["endingEntered"] and value["endingCompleted"] and
                    value["resultEntered"] and value["resultCompleted"] and
                    value["replaySaved"] and value["replaySavedPath"].endswith(".rpyx") and
                    value["replaySavedStoragePath"].startswith("/savesth06-multiplayer/replay/") and
                    value["replaySavedStoragePath"].endswith(".rpyx")
                    for index, value in enumerate(snapshots)
                )
                if not lifecycle_complete:
                    raise RuntimeError(
                        f"Ending/Result/Replay lifecycle did not complete: "
                        f"active_seen={active_seen} generation={generation_seen} snapshots={snapshots}"
                    )
                if replay_playback_cycle:
                    if not all(
                        value["replayPlaybackObserved"] and value["replayPlaybackFrame"] >= 300 and
                        value["replayPlaybackPlayerCount"] == player_count and
                        value["replayPlaybackIndependentInputs"] and
                        value["replayPlaybackStage"] == 6
                        for value in snapshots
                    ):
                        raise RuntimeError(f"multiplayer replay playback did not reproduce lanes: {snapshots}")
                    print(
                        f"TH06 browser netplay Replay playback smoke: PASS players={player_count} "
                        f"frame={'/'.join(str(value['replayPlaybackFrame']) for value in snapshots)} "
                        f"inputs={'/'.join(str(value['replayPlaybackInputs']) for value in snapshots)} "
                        f"replay={'/'.join(value['replaySavedPath'] for value in snapshots)} "
                        f"storage={'/'.join(value['replaySavedStoragePath'] for value in snapshots)}"
                    )
                    return
                print(
                    f"TH06 browser netplay Ending/Result smoke: PASS players={player_count} "
                    f"generation={'/'.join(str(value['generation']) for value in snapshots)} "
                    f"rollback={'/'.join(str(value['rollback']) for value in snapshots)} "
                    f"replay={'/'.join(value['replaySavedPath'] for value in snapshots)}"
                )
                return
            if replay_playback_cycle:
                if not all(
                    active_seen[index] and value["generation"] >= 1 and not value["active"] and
                    value["replaySaved"] and value["replaySavedPath"].endswith(".rpyx") and
                    value["replaySavedStoragePath"].startswith("/savesth06-multiplayer/replay/") and
                    not value["resultEntered"] and
                    value["replayPlaybackObserved"] and
                    value["replayPlaybackFrame"] >= replay_playback_target and
                    value["replayPlaybackPlayerCount"] == player_count and
                    value["replayPlaybackIndependentInputs"] and value["replayPlaybackStage"] == 1 and
                    value["replayExpectedFrames"] == target_frame and
                    value["replayInputCoverage"] == 31 and
                    value["replayComparedFrames"] >= replay_playback_target and
                    not value["replayInputMismatch"]
                    for index, value in enumerate(snapshots)
                ):
                    raise RuntimeError(f"short multiplayer Replay save/playback did not complete: {snapshots}")
                print(
                    f"TH06 short browser Replay playback: PASS players={player_count} "
                    f"frame={'/'.join(str(value['replayPlaybackFrame']) for value in snapshots)} "
                    f"exact={'/'.join(str(value['replayComparedFrames']) for value in snapshots)} "
                    f"inputs={'/'.join(str(value['replayPlaybackInputs']) for value in snapshots)} "
                    f"replay={'/'.join(value['replaySavedPath'] for value in snapshots)}"
                )
                return
            if not all(
                value["frame"] >= target_frame and
                value["confirmed"] >= target_frame - 1 and value["hashTarget"]
                for value in snapshots
            ):
                raise RuntimeError(f"telemetry timeout: {snapshots}")
            hashes = {value["hashTarget"] for value in snapshots}
            if len(hashes) != 1:
                raise RuntimeError(f"frame {target_frame} deterministic hash mismatch: {snapshots}")
            if force_relay and any(value["transport"] != "relay" for value in snapshots):
                raise RuntimeError(f"expected WS relay fallback: {snapshots}")
            if not force_relay and any(value["transport"] != "rtc" for value in snapshots):
                raise RuntimeError(f"expected RTC mesh: {snapshots}")
            if any(value["build"] != "th06mp-20260831-abi6-pause-restart" for value in snapshots):
                raise RuntimeError(f"wrong runtime build marker: {snapshots}")
            if require_rollback and sum(value["rollback"] for value in snapshots) <= 0:
                raise RuntimeError(f"expected rollback but saw none: {snapshots}")
            if dense_rollback_profile:
                dense_details = [
                    (value["maxSnapshotBytes"], value["maxSnapshotBlocks"])
                    for value in snapshots
                ]
                if any(
                    not value["denseRollback"] or
                    value["maxSnapshotBytes"] < 2_566_088 or
                    value["maxSnapshotBlocks"] < 1410 or
                    value["maxSnapshotBytes"] > 4 * 1024 * 1024 or
                    value["maxSnapshotBlocks"] > 2048
                    for value in snapshots
                ):
                    raise RuntimeError(
                        f"dense rollback profile missed full capacity or mobile budget: {dense_details}"
                    )
                print(
                    "TH06 browser netplay dense rollback: "
                    f"bytes={'/'.join(str(value['maxSnapshotBytes']) for value in snapshots)} "
                    f"blocks={'/'.join(str(value['maxSnapshotBlocks']) for value in snapshots)}"
                )
            if stage_transition and any(
                value["highestStage"] < 2 or not value["stageTransition"]
                for value in snapshots
            ):
                raise RuntimeError(f"stage transition not observed: {snapshots}")
            if elimination_cycle and any(
                not value["singleElimination"] or not value["teamWipe"] or
                not value["teamWipeGrace"] or not value["teamWipeRetry"] or
                value["teamWipeRetryFrame"] - value["teamWipeStartFrame"] != 180 or
                any(state == 4 for state in value["playerStates"][:player_count]) or
                (len(value["pauseState"]) > 1 and value["pauseState"][1] != 0)
                for value in snapshots
            ):
                raise RuntimeError(f"elimination/retry lifecycle not completed: {snapshots}")
            if coop_transfer_cycle and any(
                not value["coopRevivable"] or not value["coopRevive"] or
                not value["coopPowerTransfer"]
                for value in snapshots
            ):
                raise RuntimeError(f"cooperative transfer lifecycle not completed: {snapshots}")
            if require_contribution_stats:
                contribution_pairs = {
                    (tuple(value["contributionKills"][:player_count]),
                     tuple(value["contributionDamage"][:player_count]))
                    for value in snapshots
                }
                if len(contribution_pairs) != 1:
                    raise RuntimeError(f"contribution counters diverged between peers: {snapshots}")
                kills, damage = next(iter(contribution_pairs))
                if len(kills) != player_count or len(damage) != player_count or sum(damage) <= 0:
                    raise RuntimeError(f"contribution counters did not observe real damage: {snapshots}")
            if pause_cycle and (not all(pause_seen) or not all(resumed_seen)):
                raise RuntimeError(
                    f"pause lifecycle not completed: pause={pause_seen} resume={resumed_seen} snapshots={snapshots}"
                )
            if restart_cycle and any(value["generation"] < 1 for value in snapshots):
                raise RuntimeError(
                    f"Pause R did not create a new gameplay generation: "
                    f"generation_seen={generation_seen} snapshots={snapshots}"
                )
            if rtc_recovery_cycle and (not rtc_recovery_triggered or not rtc_recovery_completed):
                raise RuntimeError(
                    f"RTC ICE/signaling recovery lifecycle not completed: "
                    f"triggered={rtc_recovery_triggered} completed={rtc_recovery_completed} snapshots={snapshots}"
                )
            if transient_disconnect_cycle and (
                not transient_disconnect_triggered or not transient_disconnect_completed
            ):
                details = [page.evaluate(
                    """() => {
                      const runtime = document.getElementById('runtime')?.contentWindow;
                      const transport = runtime?.__th06PeerTransport;
                      return {
                        calls: Number(transport?.__smokeTransientRestartCalls || 0),
                        restored: !!transport?.__smokeTransientRestored,
                        error: String(transport?.__smokeTransientError || ''),
                      };
                    }"""
                ) for page in pages]
                raise RuntimeError(
                    f"transient disconnected state caused false ICE restart or did not settle: {details}"
                )
            print(
                f"TH06 browser netplay smoke: PASS players={player_count} "
                f"transport={'relay' if force_relay else 'rtc'} frame={target_frame} "
                f"hash={next(iter(hashes))} "
                f"frames={'/'.join(str(value['frame']) for value in snapshots)} "
                f"confirmed={'/'.join(str(value['confirmed']) for value in snapshots)} "
                f"rollback={'/'.join(str(value['rollback']) for value in snapshots)} "
                f"contribution={'/'.join(str(value['contributionKills'][:player_count]) + ':' + str(value['contributionDamage'][:player_count]) for value in snapshots)}"
            )
    finally:
        for browser in browsers:
            try:
                browser.close()
            except Exception:
                pass
        for process in (relay, http):
            if process.poll() is None:
                process.terminate()
        for process in (relay, http):
            try:
                process.wait(timeout=3)
            except subprocess.TimeoutExpired:
                process.kill()


if __name__ == "__main__":
    mode = sys.argv[1] if len(sys.argv) > 1 else "relay2"
    if mode == "relay2":
        run_smoke(2, True)
    elif mode == "rtc2":
        run_smoke(2, False)
    elif mode == "rtcmove2":
        run_smoke(
            2,
            False,
            target_frame=600,
            dynamic_input=True,
            rollback_audit=True,
        )
    elif mode == "rtctouch2":
        run_smoke(
            2,
            False,
            target_frame=600,
            touch_input=True,
            touch_input_player=0,
            rollback_audit=True,
        )
    elif mode == "routeskew2":
        run_smoke(2, False, route_skew_player=1, route_skew_ms=300)
    elif mode == "relay3":
        run_smoke(3, True)
    elif mode == "contribution3":
        run_smoke(
            3,
            True,
            target_frame=600,
            scripted_input=True,
            require_contribution_stats=True,
        )
    elif mode == "rtc3":
        run_smoke(3, False)
    elif mode == "routeskew3":
        run_smoke(3, False, route_skew_player=2, route_skew_ms=300)
    elif mode == "rollback2":
        run_smoke(
            2,
            True,
            target_frame=600,
            dynamic_input=True,
            relay_delay_ms=55,
            relay_jitter_ms=10,
            require_rollback=True,
            rollback_audit=True,
        )
    elif mode == "singletouch2":
        run_smoke(
            2,
            True,
            target_frame=600,
            touch_input=True,
            touch_input_player=0,
            relay_delay_ms=55,
            relay_jitter_ms=10,
            require_rollback=True,
            rollback_audit=True,
        )
    elif mode == "singlemove2":
        run_smoke(
            2,
            True,
            target_frame=600,
            dynamic_input=True,
            dynamic_input_player=0,
            relay_delay_ms=55,
            relay_jitter_ms=10,
            require_rollback=True,
            rollback_audit=True,
        )
    elif mode == "scripted2":
        run_smoke(
            2,
            True,
            target_frame=600,
            relay_delay_ms=55,
            relay_jitter_ms=10,
            require_rollback=True,
            rollback_audit=True,
            scripted_input=True,
        )
    elif mode == "backlog2":
        run_smoke(
            2,
            True,
            target_frame=900,
            relay_delay_ms=20,
            relay_jitter_ms=15,
            relay_drop_every=7,
            require_rollback=True,
            scripted_stress_input=True,
            cpu_throttle_rate=4,
            report_backlog=True,
        )
    elif mode == "backlog3":
        run_smoke(
            3,
            True,
            target_frame=900,
            relay_delay_ms=20,
            relay_jitter_ms=15,
            relay_drop_every=7,
            require_rollback=True,
            scripted_stress_input=True,
            cpu_throttle_rate=3,
            report_backlog=True,
        )
    elif mode == "dense2":
        run_smoke(
            2,
            True,
            target_frame=300,
            relay_delay_ms=20,
            relay_jitter_ms=15,
            relay_drop_every=7,
            require_rollback=True,
            scripted_stress_input=True,
            dense_rollback_profile=True,
            cpu_throttle_rate=4,
        )
    elif mode == "dense3":
        run_smoke(
            3,
            True,
            target_frame=300,
            relay_delay_ms=20,
            relay_jitter_ms=15,
            relay_drop_every=7,
            require_rollback=True,
            scripted_stress_input=True,
            dense_rollback_profile=True,
            cpu_throttle_rate=3,
        )
    elif mode == "stress2":
        run_smoke(
            2,
            True,
            target_frame=600,
            relay_delay_ms=55,
            relay_jitter_ms=10,
            require_rollback=True,
            rollback_audit=True,
            scripted_stress_input=True,
            require_contribution_stats=True,
        )
    elif mode == "frame0drop2":
        run_smoke(
            2,
            True,
            target_frame=300,
            relay_drop_first_input_per_edge=True,
            scripted_input=True,
        )
    elif mode == "frame0drop3":
        run_smoke(
            3,
            True,
            target_frame=300,
            relay_drop_first_input_per_edge=True,
            scripted_input=True,
        )
    elif mode == "predlimit2":
        run_smoke(
            2,
            True,
            target_frame=300,
            relay_drop_input_latest_from=120,
            relay_drop_input_latest_to=132,
            require_rollback=True,
            scripted_input=True,
        )
    elif mode == "predlimit3":
        run_smoke(
            3,
            True,
            target_frame=300,
            relay_drop_input_latest_from=120,
            relay_drop_input_latest_to=132,
            require_rollback=True,
            scripted_input=True,
        )
    elif mode == "loadout2":
        run_smoke(
            2,
            True,
            target_frame=600,
            relay_delay_ms=55,
            relay_jitter_ms=10,
            require_rollback=True,
            rollback_audit=True,
            scripted_stress_input=True,
            loadout_profile="alternate",
        )
    elif mode == "elimination2":
        run_smoke(
            2,
            True,
            target_frame=1200,
            relay_delay_ms=55,
            relay_jitter_ms=10,
            require_rollback=True,
            rollback_audit=True,
            scripted_stress_input=True,
            elimination_cycle=True,
        )
    elif mode == "elimination3":
        run_smoke(
            3,
            True,
            target_frame=1200,
            relay_delay_ms=55,
            relay_jitter_ms=10,
            require_rollback=True,
            rollback_audit=True,
            scripted_stress_input=True,
            elimination_cycle=True,
        )
    elif mode == "coop2":
        run_smoke(
            2,
            True,
            target_frame=600,
            relay_delay_ms=55,
            relay_jitter_ms=10,
            require_rollback=True,
            rollback_audit=True,
            coop_transfer_cycle=True,
        )
    elif mode == "pause2":
        run_smoke(
            2,
            True,
            target_frame=900,
            rollback_audit=True,
            pause_cycle=True,
        )
    elif mode == "restart2":
        run_smoke(
            2,
            True,
            target_frame=900,
            rollback_audit=True,
            restart_cycle=True,
        )
    elif mode == "restart3":
        run_smoke(
            3,
            True,
            target_frame=900,
            relay_delay_ms=55,
            relay_jitter_ms=10,
            require_rollback=True,
            rollback_audit=True,
            restart_cycle=True,
        )
    elif mode == "pause3":
        run_smoke(
            3,
            True,
            target_frame=900,
            relay_delay_ms=55,
            relay_jitter_ms=10,
            require_rollback=True,
            rollback_audit=True,
            scripted_stress_input=True,
            pause_cycle=True,
        )
    elif mode == "loss2":
        run_smoke(
            2,
            True,
            target_frame=900,
            relay_delay_ms=55,
            relay_jitter_ms=10,
            relay_drop_every=7,
            require_rollback=True,
            rollback_audit=True,
            scripted_stress_input=True,
        )
    elif mode == "loss3":
        run_smoke(
            3,
            True,
            target_frame=900,
            relay_delay_ms=55,
            relay_jitter_ms=10,
            relay_drop_every=7,
            require_rollback=True,
            rollback_audit=True,
            scripted_stress_input=True,
        )
    elif mode == "retry2":
        run_smoke(
            2,
            True,
            target_frame=1200,
            relay_delay_ms=55,
            relay_jitter_ms=10,
            require_rollback=True,
            rollback_audit=True,
            scripted_stress_input=True,
            retry_continue_cycle=True,
        )
    elif mode == "retry3":
        run_smoke(
            3,
            True,
            target_frame=1200,
            relay_delay_ms=55,
            relay_jitter_ms=10,
            require_rollback=True,
            rollback_audit=True,
            scripted_stress_input=True,
            retry_continue_cycle=True,
        )
    elif mode == "recovery2":
        run_smoke(
            2,
            False,
            target_frame=900,
            require_rollback=True,
            rollback_audit=True,
            scripted_stress_input=True,
            rtc_recovery_cycle=True,
        )
    elif mode == "transient2":
        run_smoke(
            2,
            False,
            target_frame=600,
            scripted_input=True,
            transient_disconnect_cycle=True,
        )
    elif mode == "transient3":
        run_smoke(
            3,
            False,
            target_frame=600,
            scripted_input=True,
            transient_disconnect_cycle=True,
        )
    elif mode == "recovery3":
        run_smoke(
            3,
            False,
            target_frame=900,
            require_rollback=True,
            rollback_audit=True,
            scripted_stress_input=True,
            rtc_recovery_cycle=True,
        )
    elif mode == "quit2":
        run_smoke(
            2,
            True,
            target_frame=900,
            relay_delay_ms=55,
            relay_jitter_ms=10,
            rollback_audit=True,
            scripted_stress_input=True,
            quit_cycle=True,
        )
    elif mode == "quit3":
        run_smoke(
            3,
            True,
            target_frame=900,
            relay_delay_ms=55,
            relay_jitter_ms=10,
            rollback_audit=True,
            scripted_stress_input=True,
            quit_cycle=True,
        )
    elif mode == "ending2":
        run_smoke(
            2,
            True,
            target_frame=1200,
            ending_cycle=True,
        )
    elif mode == "ending3":
        run_smoke(
            3,
            True,
            target_frame=1200,
            ending_cycle=True,
        )
    elif mode == "replay2":
        run_smoke(
            2,
            True,
            target_frame=600,
            replay_playback_cycle=True,
        )
    elif mode == "replay3":
        run_smoke(
            3,
            True,
            target_frame=180,
            replay_playback_cycle=True,
            replay_playback_target=60,
        )
    elif mode == "replayrollback2":
        run_smoke(
            2,
            True,
            target_frame=600,
            relay_delay_ms=55,
            relay_jitter_ms=10,
            require_rollback=True,
            rollback_audit=True,
            scripted_stress_input=True,
            replay_playback_cycle=True,
            replay_playback_target=300,
        )
    else:
        raise SystemExit("usage: netplay-browser-smoke.py [relay2|rtc2|rtcmove2|rtctouch2|routeskew2|relay3|contribution3|rtc3|routeskew3|rollback2|singlemove2|singletouch2|scripted2|stress2|elimination2|elimination3|coop2|pause2|pause3|restart2|restart3|loss2|loss3|frame0drop2|frame0drop3|predlimit2|predlimit3|backlog2|backlog3|loadout2|retry2|retry3|recovery2|recovery3|transient2|transient3|quit2|quit3|ending2|ending3|replay2|replay3|replayrollback2]")
