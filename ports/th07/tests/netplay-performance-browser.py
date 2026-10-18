#!/usr/bin/env python3
"""Delayed MP + heterogeneous CPU stress, not a real-phone FPS benchmark.

Uses existing canonical-state telemetry to reject a speedup that desynchronizes
peers. RAF cadence is measured in the game iframe with no GPU readback. Probe
startup/final waiting is excluded. Network and CPU settings are in the report.
PASS means completion and sampled deterministic-state agreement, not a phone
smoothness guarantee. CPU throttling is not a specific mobile chipset model.
"""
from __future__ import annotations

import argparse
import functools
import http.server
import json
import os
from pathlib import Path
import re
import socket
import subprocess
import tempfile
import threading
import time
import urllib.parse

from playwright.sync_api import sync_playwright

ROOT = Path(__file__).resolve().parents[1]
RELAY_ROOT = ROOT.parent / "eagler-touhou"


class QuietHandler(http.server.SimpleHTTPRequestHandler):
    def log_message(self, _format: str, *_args: object) -> None:
        pass


def free_port() -> int:
    with socket.socket() as sock:
        sock.bind(("127.0.0.1", 0))
        return int(sock.getsockname()[1])


def distribution(values: list[float]) -> dict:
    if not values:
        raise AssertionError("no in-game timing samples collected")
    ordered = sorted(values)
    return {
        "samples": len(values), "mean_ms": sum(values) / len(values),
        "p50_ms": ordered[len(ordered) // 2],
        "p95_ms": ordered[min(len(ordered) - 1, len(ordered) * 95 // 100)],
        "p99_ms": ordered[min(len(ordered) - 1, len(ordered) * 99 // 100)],
        "max_ms": ordered[-1],
        "over_25ms": sum(value > 25 for value in values),
        "over_50ms": sum(value > 50 for value in values),
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--players", type=int, choices=(2, 3), default=2)
    parser.add_argument("--frames", type=int, default=900)
    parser.add_argument("--delay-ms", type=int, default=55)
    parser.add_argument("--jitter-ms", type=int, default=10)
    parser.add_argument("--cpu-rate", type=float, default=1)
    parser.add_argument("--music", choices=("none", "ogg"), default="none")
    parser.add_argument("--require-audio", action="store_true", help="Run the actual SDL audio callback under an explicit autoplay test policy and verify nonzero output")
    parser.add_argument("--all-slow", action="store_true", help="Apply CPU throttling to every endpoint, including phone-to-phone controls")
    parser.add_argument("--browser-channel", choices=("default", "chromium", "msedge", "chrome"), default="default")
    parser.add_argument("--limit-presentation-60", action="store_true", help="Existing product display cap; does not change the fixed simulation rate")
    parser.add_argument("--auto-profile", action="store_true", help="Test the production mobile defaults instead of explicit manual tuning")
    parser.add_argument("--responsive-profile", action="store_true", help="Test real zero-delay Runtime defaults, without overriding snapshot/cadence/repair choices")
    parser.add_argument("--slow-player", type=int, default=1)
    parser.add_argument("--analog", action="store_true")
    parser.add_argument("--input-delay", type=int, choices=range(13), default=3)
    parser.add_argument("--strong-input-delay", type=int, choices=range(13), default=None,
                        help="Override input delay for every endpoint except --slow-player")
    parser.add_argument("--touch-stream", choices=("snapshot", "delta"), default="snapshot")
    parser.add_argument("--physical-touch", action="store_true", help="Inject real browser touch events and verify once-only displacement")
    parser.add_argument("--bridge-drag", action="store_true", help="Continuous gestures through the real Launcher direct-touch bridge, not synthetic FrameInput")
    parser.add_argument("--wall-clock-drag", action="store_true", help="Drive drag trajectory by elapsed time, not simulation progress")
    parser.add_argument("--input-latency", action="store_true", help="Diagnostic synthetic event-to-render decomposition; not input-to-photon")
    parser.add_argument("--direct-input-bridge", action="store_true", help="Use the same-origin synchronous input lane instead of a queued postMessage")
    parser.add_argument("--bomb-cycle", action="store_true", help="Correctness only: fire each player's real Bomb control while dragging")
    parser.add_argument("--live-bullet-audit", action="store_true", help="Correctness only: compare every live-journal restore against a full byte copy")
    parser.add_argument("--dormant-bullet-elision", action="store_true", help="Experimental: avoid saving an untouched dormant despawn VM; preserve every original update")
    parser.add_argument("--reliable-input-repair", action="store_true", help="Duplicate unacknowledged input on the reliable RTC lane after stalled ACK progress")
    parser.add_argument("--rtc-input-blackout-ms", type=float, default=0,
                        help="Drop fast-channel input sends for this duration after frame 900; reliable repairs still get the full injected latency")
    parser.add_argument("--prediction-window", type=int, choices=range(1, 13), default=12)
    parser.add_argument("--touch-delta-prediction-frames", type=int, choices=range(7), default=0)
    parser.add_argument("--touch-quantization", type=float, default=0.0)
    parser.add_argument("--touch-equivalent-absorb", action="store_true")
    parser.add_argument("--incremental-reconcile", action="store_true",
                        help="Split rollback historical replay across browser callbacks")
    parser.add_argument("--slow-frame-lag", type=int, choices=range(0, 9), default=0,
                        help="Test-only: start the slow endpoint this many simulation frames behind")
    parser.add_argument("--snapshot-policy", choices=("always", "demand", "frontier"), default="always")
    parser.add_argument("--snapshot-layout", choices=("objects", "runs"), default="objects")
    parser.add_argument("--snapshot-copy", choices=("wasm", "bulk"), default="wasm")
    parser.add_argument("--snapshot-restore", choices=("sequential", "coalesced"), default="sequential")
    parser.add_argument("--checkpoint-frames", type=int, choices=range(1, 9), default=2)
    parser.add_argument("--bullet-snapshot", choices=("journal", "compact", "live"), default="journal")
    parser.add_argument("--strong-bullet-snapshot", choices=("journal", "compact", "live"),
                        help="Reference snapshot backend on endpoints other than --slow-player")
    parser.add_argument("--rtc-input-delay-ms", type=float, default=None,
                        help="Test-only delay applied on actual RTC input sends, NOT the relay")
    parser.add_argument("--rtc-input-jitter-ms", type=float, default=10)
    parser.add_argument("--touch-workload", choices=("default", "steady", "variable", "stop", "reverse", "burst"), default="default")
    parser.add_argument("--no-rollback", action="store_true", help="Pure buffered lockstep: exact remote input only, no rollback snapshots")
    parser.add_argument("--slow-no-rollback", action="store_true",
                        help="Test-only: disable rollback only on --slow-player; other endpoints keep rollback")
    parser.add_argument("--slow-buffered-policy", action="store_true",
                        help="Use the production netplayRollbackPolicy=buffered only on --slow-player")
    parser.add_argument("--keep-rollback-snapshots", action="store_true", help="With --no-rollback, keep snapshot capture but still forbid prediction/resimulation")
    parser.add_argument("--dense-bullets", type=int, default=0, help="Test-only stationary BulletManager load (0..960)")
    parser.add_argument("--curved-bullets", action="store_true", help="Dense multi-sprite turning bullets using real command processing")
    parser.add_argument("--real-stage", type=int, choices=range(1, 7), default=None, help="Real ECL/STD workload, invulnerable probe players so deaths cannot clear the workload")
    parser.add_argument("--difficulty", type=int, choices=range(4), default=1)
    parser.add_argument("--require-rollback", action="store_true")
    parser.add_argument("--relay-diagnostics", action="store_true")
    parser.add_argument("--rtc", action="store_true", help="Use real RTC; relay delay does not affect RTC input")
    parser.add_argument("--build", default="build-web-th07-netplay")
    parser.add_argument("--timeout", type=float, default=180)
    parser.add_argument("--output", type=Path)
    parser.add_argument("--cpu-profile", type=Path, help="Diagnostic slow-endpoint sampling profile; do not use as an unprofiled performance claim")
    args = parser.parse_args()
    if args.rtc_input_blackout_ms and (not args.rtc or args.rtc_input_delay_ms is None or
            not 0 < args.rtc_input_blackout_ms <= 10000 or args.frames < 1800):
        parser.error("fast-lane blackout requires RTC delay and >=1800 frames")
    if args.bomb_cycle and (not args.bridge_drag or args.frames < 1800):
        parser.error("Bomb correctness fixture requires bridge drag and at least 1800 frames")
    if (args.wall_clock_drag or args.input_latency or args.direct_input_bridge) and not args.bridge_drag:
        parser.error("wall-clock/input-latency measurement requires --bridge-drag")
    if args.responsive_profile:
        if args.auto_profile or args.no_rollback or args.slow_no_rollback or args.slow_buffered_policy or args.strong_input_delay is not None or args.strong_bullet_snapshot is not None:
            parser.error("responsive-profile cannot be mixed with a different endpoint policy")
        if not args.bridge_drag:
            parser.error("responsive-profile requires real bridge input")
        # Expectations, not explicit low-level options sent to the Runtime.
        args.input_delay = 0
        args.snapshot_policy, args.snapshot_layout = "frontier", "runs"
        args.snapshot_copy, args.snapshot_restore = "bulk", "coalesced"
        args.checkpoint_frames, args.bullet_snapshot = 3, "live"
        # High-refresh presentation is now the default. The shared Launcher
        # "Lock to 60 FPS" preference remains the only owner of this cap.
        args.limit_presentation_60 = False
        args.reliable_input_repair = True
    if args.auto_profile:
        if not (args.bridge_drag or args.physical_touch):
            parser.error("auto-profile acceptance requires the real touch input path")
        # Expected product values, NOT forwarded as explicit overrides by host.
        args.input_delay = 6
        args.snapshot_policy = "frontier"
        args.snapshot_layout = "runs"
        args.snapshot_copy = "bulk"
        args.snapshot_restore = "coalesced"
    if args.bridge_drag and (args.physical_touch or args.analog or args.touch_workload != "default" or args.frames < 900):
        parser.error("bridge-drag requires >=900 frames, default trace and no other input fixture")
    if args.rtc_input_delay_ms is not None and (not args.rtc or
            not 0 <= args.rtc_input_delay_ms <= 10000 or not 0 <= args.rtc_input_jitter_ms <= 10000):
        parser.error("RTC input impairment requires --rtc and bounded nonnegative delays")
    if args.physical_touch and (args.players != 2 or args.analog or args.touch_workload != "default"):
        parser.error("physical-touch fixture requires 2P, default workload and no --analog")
    if (not re.fullmatch(r"[A-Za-z0-9_.-]+", args.build) or args.frames < 600
            or args.frames > 12000 or args.frames % 300 or args.cpu_rate < 1
            or not 0 <= args.slow_player < args.players
            or args.delay_ms < 0 or args.jitter_ms < 0 or args.timeout <= 0
            or not 0 <= args.touch_quantization <= 2
            or args.dense_bullets < 0 or args.dense_bullets > 960
            or (args.no_rollback and args.require_rollback)
            or (args.keep_rollback_snapshots and not args.no_rollback)):
        parser.error("invalid build/timing settings; frames must be 600..12000 in steps of 300")
    build = ROOT / args.build
    if not (build / "th07.wasm").is_file():
        parser.error(f"build missing: {build}")
    if args.analog:
        cache = build / "CMakeCache.txt"
        manifest = build / "netplay-benchmark-build.json"
        dev_tools = ("TH_DEV_TOOLS:BOOL=ON" in cache.read_text() if cache.is_file()
                     else json.loads(manifest.read_text())["dev_tools"])
        if dev_tools:
            parser.error("analog fixture requires TH_DEV_TOOLS=OFF (uses Replay input stimulus only)")

    http_port, relay_port = free_port(), free_port()
    env = {key: value for key, value in os.environ.items()
           if not key.startswith(("EAGLER_NETPLAY_", "TH07_RELAY_", "TH07_TEST_", "TH07_STUN_", "TH07_TURN_"))}
    env.update({
        "EAGLER_NETPLAY_RELAY_HOST": "127.0.0.1",
        "EAGLER_NETPLAY_RELAY_PORT": str(relay_port),
        "EAGLER_NETPLAY_RELAY_DELAY_MS": str(args.delay_ms),
        "EAGLER_NETPLAY_RELAY_JITTER_MS": str(args.jitter_ms),
        "EAGLER_NETPLAY_STUN_URLS": "",
    })
    handler = functools.partial(QuietHandler, directory=str(ROOT))
    room = f"th07mp-perf-{time.time_ns()}"
    messages: list[list[str]] = [[] for _ in range(args.players)]
    errors: list[list[str]] = [[] for _ in range(args.players)]
    report = {"settings": {key: str(value) if isinstance(value, Path) else value
                            for key, value in vars(args).items()}, "peers": []}
    with http.server.ThreadingHTTPServer(("127.0.0.1", http_port), handler) as server, tempfile.TemporaryFile(mode="w+t") as relay_log:
        thread = threading.Thread(target=server.serve_forever, daemon=True)
        thread.start()
        relay_command = ["node"]
        if args.relay_diagnostics:
            relay_command += ["--import", (ROOT / "tests/netplay-relay-timing-probe.mjs").as_uri()]
        relay = subprocess.Popen([*relay_command, str(RELAY_ROOT / "server/netplay-relay.mjs")],
                                 cwd=RELAY_ROOT, env=env, stdout=relay_log, stderr=relay_log)
        try:
            deadline = time.monotonic() + 10
            while True:
                if relay.poll() is not None or time.monotonic() >= deadline:
                    relay_log.seek(0)
                    raise RuntimeError("relay startup failed: " + relay_log.read()[-8000:])
                try:
                    with socket.create_connection(("127.0.0.1", relay_port), timeout=0.2):
                        break
                except OSError:
                    time.sleep(0.1)
            with sync_playwright() as playwright:
                browsers, pages, touch_sessions, slow_sessions = [], [], [], []
                profile_session = None
                profile_started = False
                try:
                    for player in range(args.players):
                        browser = playwright.chromium.launch(headless=True,
                            channel=None if args.browser_channel == "default" else args.browser_channel, args=[
                            "--disable-background-timer-throttling",
                            "--disable-backgrounding-occluded-windows",
                            "--disable-renderer-backgrounding",
                        ] + (["--autoplay-policy=no-user-gesture-required"] if args.require_audio else []))
                        browsers.append(browser)
                        if player == 0:
                            gpu = browser.new_browser_cdp_session().send("SystemInfo.getInfo").get("gpu", {})
                            report["browser"] = {"version": browser.version, "gpu_devices": gpu.get("devices"),
                                                 "gpu_features": gpu.get("featureStatus"), "gpu_aux": gpu.get("auxAttributes")}
                        context = browser.new_context(viewport={"width": 960, "height": 720})
                        if args.require_audio:
                            context.add_init_script((ROOT / "tests/netplay-audio-probe.cjs").read_text(encoding="utf-8"))
                        if args.input_latency:
                            context.add_init_script((ROOT / "tests/netplay-input-latency-probe.cjs").read_text(encoding="utf-8"))
                        if args.rtc_input_delay_ms is not None:
                            injector = (ROOT / "third_party/eagler-common/testkit/rtc-input-impairment.cjs").read_text(encoding="utf-8")
                            settings = json.dumps({"oneWayMs": args.rtc_input_delay_ms,
                                                   "jitterMs": args.rtc_input_jitter_ms,
                                                   "blackoutMs": args.rtc_input_blackout_ms,
                                                   "inputLabel": "th07-input",
                                                   "controlLabel": "th07-control",
                                                   "seed": 7135 + player})
                            context.add_init_script(injector +
                                "\nglobalThis.__th07InputImpairment = installRtcInputImpairment(" + settings + ");")
                        if not args.rtc:
                            context.add_init_script("delete globalThis.RTCPeerConnection")
                        context.add_init_script("""(() => {
                          if (window.parent === window) return;
                          const state = globalThis.__th07Perf = {
                            gaps: [], advanceGaps: [], firstTime: 0, lastTime: 0,
                            firstFrame: 0, lastFrame: 0
                          };
                          let last = 0, lastAdvance = 0, previousFrame = -1;
                          function measure(now) {
                            const frame = Number(globalThis.__eaglerNetplayLanFrame || 0);
                            const active = frame >= SAMPLE_START && frame < FRAME_LIMIT;
                            globalThis.__th07AudioProbe?.observe(globalThis.Module?.SDL3, active);
                            if (active) {
                              if (!state.firstTime) { state.firstTime = now; state.firstFrame = frame; }
                              if (last) state.gaps.push(now - last);
                              if (frame !== previousFrame) {
                                if (lastAdvance) state.advanceGaps.push(now - lastAdvance);
                                lastAdvance = now;
                              }
                              state.lastTime = now; state.lastFrame = frame; last = now;
                            } else { last = 0; lastAdvance = 0; }
                            previousFrame = frame;
                            requestAnimationFrame(measure);
                          }
                          requestAnimationFrame(measure);
                        })();""".replace("FRAME_LIMIT", str(args.frames)).replace("SAMPLE_START", "300" if args.dense_bullets else "120"))
                        page = context.new_page()
                        if args.physical_touch:
                            touch_session = context.new_cdp_session(page)
                            touch_session.send("Emulation.setTouchEmulationEnabled", {"enabled": True, "maxTouchPoints": 1})
                            touch_sessions.append(touch_session)
                        page.on("console", lambda message, i=player: messages[i].append(message.text))
                        page.on("pageerror", lambda error, i=player: errors[i].append(str(error)))
                        if (player == args.slow_player or args.all_slow) and args.cpu_rate != 1:
                            slow_sessions.append(context.new_cdp_session(page))
                        if player == args.slow_player and args.cpu_profile:
                            profile_session = context.new_cdp_session(page)
                        query = urllib.parse.urlencode({
                            "player": player, "players": args.players, "frames": args.frames,
                            "relay": f"ws://127.0.0.1:{relay_port}/?room={room}&player={player}",
                            "build": args.build, "analog": int(args.analog),
                            "music": args.music,
                            "inputDelay": (args.strong_input_delay if
                                args.strong_input_delay is not None and player != args.slow_player
                                else args.input_delay),
                            "rollbackPolicy": ("buffered" if
                                args.slow_buffered_policy and player == args.slow_player else "full"),
                            "touchDeltaPredictionFrames": args.touch_delta_prediction_frames,
                            "touchQuantization": args.touch_quantization,
                            "touchEquivalentAbsorb": int(args.touch_equivalent_absorb),
                            "incrementalReconcile": int(args.incremental_reconcile),
                            "initialFrameLag": args.slow_frame_lag if player == args.slow_player else 0,
                            "disableTimeSyncPacing": int(args.slow_frame_lag > 0),
                            "snapshotPolicy": args.snapshot_policy,
                            "snapshotLayout": args.snapshot_layout,
                            "snapshotCopy": args.snapshot_copy,
                            "snapshotRestore": args.snapshot_restore,
                            "checkpointFrames": args.checkpoint_frames,
                            "bulletSnapshot": (args.strong_bullet_snapshot if
                                args.strong_bullet_snapshot is not None and player != args.slow_player
                                else args.bullet_snapshot),
                            "predictionWindow": args.prediction_window,
                            "trace": args.touch_workload,
                            "noRollback": int(args.no_rollback or
                                (args.slow_no_rollback and player == args.slow_player)),
                            "keepSnapshots": int(args.keep_rollback_snapshots),
                            "denseBullets": args.dense_bullets,
                            "curvedBullets": int(args.curved_bullets),
                            "difficulty": args.difficulty,
                            "touchStream": args.touch_stream,
                            "physicalTouch": int(args.physical_touch),
                            "bridgeDrag": int(args.bridge_drag),
                            "wallClockDrag": int(args.wall_clock_drag),
                            "directInputBridge": int(args.direct_input_bridge),
                            "bombCycle": int(args.bomb_cycle),
                            "liveBulletAudit": int(args.live_bullet_audit),
                            "dormantBulletElision": int(args.dormant_bullet_elision),
                            "reliableInputRepair": int(args.reliable_input_repair),
                            "manualStart": 1,
                            "autoProfile": int(args.auto_profile),
                            "responsiveProfile": int(args.responsive_profile),
                            "cap60": int(args.limit_presentation_60),
                        })
                        if args.real_stage is not None:
                            query += "&realStage=" + str(args.real_stage)
                        page.goto(f"http://127.0.0.1:{http_port}/tests/netplay-performance-browser-host.html?{query}",
                                  wait_until="load", timeout=60000)
                        pages.append(page)
                    # Compile/load every endpoint before starting gameplay.
                    # Otherwise a cold WASM/font load can time out RTC setup or
                    # consume the gameplay deadline before a single input exists.
                    for page in pages:
                        page.wait_for_function("globalThis.__th07PerfConfigured || globalThis.__th07PerfFailure", timeout=90000)
                    for session in slow_sessions:
                        session.send("Emulation.setCPUThrottlingRate", {"rate": args.cpu_rate})
                    for page in pages:
                        page.evaluate("() => { void globalThis.__th07PerfLaunch(); }")
                    deadline = time.monotonic() + args.timeout
                    previous_frames = [-1] * args.players
                    progress_times = [time.monotonic()] * args.players
                    stalls = report.setdefault("stalls", [])
                    touch_started = touch_checked = False
                    initial_positions = []
                    next_progress = time.monotonic() + 5
                    while time.monotonic() < deadline:
                        states = [page.evaluate("""() => {
                          const runtime = document.getElementById('runtime').contentWindow;
                          return {done: !!runtime.__eaglerNetplayLanStageDone,
                            frame: runtime.__eaglerNetplayLanFrame || 0,
                            failed: !!runtime.__eaglerNetplayFailed,
                            error: globalThis.__th07PerfFailure || runtime.__eaglerNetplayError || '',
                            transport: runtime.__eaglerNetplayTransport,
                            queued: runtime.__th07PeerTransport?.received?.length,
                            queueHead: runtime.__th07PeerTransport?.receivedHead,
                            confirmed: runtime.__eaglerNetplayLanConfirmed,
                            pause: runtime.__eaglerNetplayPauseState,
                            inputReceived: runtime.__th07PeerTransport?.perf?.received,
                            inputSent: runtime.__th07PeerTransport?.perf?.sent,
                            bufferedBytes: runtime.__th07PeerTransport?.relay?.bufferedAmount,
                            lastWireFrame: runtime.__th07PeerTransport?.perf?.lastInputFrame,
                            captureFrame: runtime.__eaglerNetplayDebugCaptureFrame,
                            reconcileActive: runtime.__eaglerNetplayDebugReconcileActive,
                            reconcileNext: runtime.__eaglerNetplayDebugReconcileNext,
                            reconcileLast: runtime.__eaglerNetplayDebugReconcileLast,
                            coreLast: runtime.__eaglerNetplayDebugCoreLast,
                            rollbackFrame: runtime.__eaglerNetplayDebugRollback,
                            localConfirmed: runtime.__eaglerNetplayDebugLocalConfirmed,
                            driverTick: runtime.__eaglerNetplayDebugDriverTick,
                            reconcileYield: runtime.__eaglerNetplayDebugReconcileYield,
                            renderCallbacks: runtime.__eaglerDebugRenderCallbacks,
                            appActive: runtime.__eaglerDebugAppActive,
                            accumulator: runtime.__eaglerDebugAccumulator,
                            targetDt: runtime.__eaglerDebugTargetDt,
                            lastEventAge: runtime.__th07PeerTransport?.perf?.lastEventMs
                              ? runtime.performance.now() - runtime.__th07PeerTransport.perf.lastEventMs : 0,
                            socket: runtime.__th07PeerTransport?.relay?.readyState};
                        }""") for page in pages]
                        if any(state["failed"] or state["error"] for state in states) or any(errors):
                            raise AssertionError(f"runtime failure: {states}; errors={errors}")
                        fatals = [line for lines in messages for line in lines
                                  if "chain result=-1" in line or "netplay lan stage: FAIL" in line]
                        if fatals:
                            raise AssertionError(f"runtime exited before completion: {fatals[-5:]}")
                        if all(state["done"] for state in states):
                            break
                        if profile_session and not profile_started and states[args.slow_player]["frame"] >= 300:
                            profile_session.send("Profiler.enable")
                            profile_session.send("Profiler.setSamplingInterval", {"interval": 1000})
                            profile_session.send("Profiler.start")
                            profile_started = True
                        if args.physical_touch and not touch_started and all(state["frame"] >= 130 for state in states):
                            initial_positions = [page.evaluate("document.getElementById('runtime').contentWindow.__eaglerNetplayPerf.positions") for page in pages]
                            for i, touch_session in enumerate(touch_sessions):
                                point = {"x": 308, "y": 388, "id": 1}
                                touch_session.send("Input.dispatchTouchEvent", {"type": "touchStart", "touchPoints": [point]})
                                point["x"] += 12 if i == 0 else -14
                                touch_session.send("Input.dispatchTouchEvent", {"type": "touchMove", "touchPoints": [point]})
                            touch_started = True
                        if args.physical_touch and touch_started and not touch_checked and all(state["frame"] >= 230 for state in states):
                            observed = [page.evaluate("""() => {
                              const p = document.getElementById('runtime').contentWindow.__eaglerNetplayPerf;
                              return {positions:p.positions, x:p.capturedTouchX, y:p.capturedTouchY};
                            }""") for page in pages]
                            for peer, sample in enumerate(observed):
                                for slot in range(2):
                                    delta = sample["positions"][slot * 2] - initial_positions[peer][slot * 2]
                                    expected = observed[slot]["x"]
                                    if abs(expected) < 1 or abs(delta - expected) > 0.001:
                                        raise AssertionError(f"physical touch duplicated/lost: peer={peer} slot={slot} delta={delta} expected={expected} observed={observed}")
                            report["physical_touch"] = {"initial_positions": initial_positions, "observed": observed}
                            for touch_session in touch_sessions:
                                touch_session.send("Input.dispatchTouchEvent", {"type":"touchEnd", "touchPoints":[]})
                            touch_checked = True
                        now = time.monotonic()
                        if now >= next_progress:
                            print("PROGRESS " + json.dumps(states), flush=True)
                            next_progress = now + 5
                        for i, state in enumerate(states):
                            if state["frame"] != previous_frames[i]:
                                progress_times[i] = now
                                previous_frames[i] = state["frame"]
                            elif state["frame"] > 0 and not state["done"] and now - progress_times[i] > 0.4:
                                if len(stalls) < 150:
                                    stalls.append({"peer": i, "no_advance_ms": (now - progress_times[i]) * 1000, **state})
                        pages[0].wait_for_timeout(200)
                    else:
                        raise AssertionError(f"gameplay timeout: {states}")
                    hashes = []
                    if args.physical_touch and not touch_checked:
                        raise AssertionError("physical touch stimulus did not complete")
                    report["stalls"] = stalls
                    if args.bridge_drag:
                        report["bridge_drag"] = [page.evaluate("globalThis.__th07BridgeDrag") for page in pages]
                        for gesture in report["bridge_drag"]:
                            if (not gesture or not gesture.get("done") or gesture["events"] < 60 or
                                    gesture["pathPixels"] < 100 or gesture["maxX"] - gesture["minX"] < 20):
                                raise AssertionError(f"continuous bridge gesture did not execute: {gesture}")
                            if args.direct_input_bridge and gesture.get("directEvents") != gesture["events"]:
                                raise AssertionError("input bridge fixture silently fell back or duplicated dispatch")
                    for player, page in enumerate(pages):
                        data = page.evaluate("""() => {
                          const w = document.getElementById('runtime').contentWindow;
                          let graphics = null;
                          try {
                            const gl = w.Module.canvas.getContext('webgl2');
                            const info = gl?.getExtension('WEBGL_debug_renderer_info');
                            if (info) graphics = {vendor:gl.getParameter(info.UNMASKED_VENDOR_WEBGL),
                              renderer:gl.getParameter(info.UNMASKED_RENDERER_WEBGL)};
                          } catch (_) {}
                          return {timing: w.__th07Perf, hashes: w.__eaglerNetplayLanHashes,
                            input_latency: w.__th07InputLatencyProbe?.finish() || null,
                            live_bullet_audit_restores: w.__eaglerLiveBulletAuditRestores || 0,
                            dormant_bullet_elision: !!w.__eaglerDormantBulletElision,
                            bomb_observed_mask: w.__eaglerNetplayBombObservedMask || 0,
                            input_repairs: w.__th07PeerTransport?.inputRepairSent || 0,
                            repair_enabled: w.Module.eaglerOptions.netplayReliableInputRepair,
                            audio: w.__th07AudioProbe?.snapshot() || null,
                            environment: {userAgent:w.navigator.userAgent, graphics,
                              canvas:[w.Module.canvas.width,w.Module.canvas.height], dpr:w.devicePixelRatio,
                              concurrency:w.navigator.hardwareConcurrency},
                            canonical: w.__eaglerNetplayLanCanonical,
                            canonical_meta: w.__eaglerNetplayLanCanonicalMeta,
                            canonical_players: w.__eaglerNetplayLanCanonicalPlayers,
                            costs: w.__eaglerNetplayPerf,
                            performance_profile: w.Module.eaglerOptions.netplayPerformanceProfile,
                            cap_60: w.Module.eaglerOptions.limitPresentationTo60,
                            input_delay: w.__eaglerNetplayInputDelayFrames,
                            prediction_window: w.__eaglerNetplayMaxPredictionFrames,
                            snapshot_policy: w.__eaglerNetplaySnapshotPolicy,
                            snapshot_layout: w.__eaglerNetplaySnapshotLayout,
                            snapshot_copy: w.__eaglerNetplaySnapshotCopy,
                            snapshot_restore: w.__eaglerNetplaySnapshotRestore,
                            checkpoint_frames: w.__eaglerNetplaySnapshotCheckpointFrames,
                            bullet_snapshot: w.__eaglerNetplayBulletSnapshot,
                            touch_delta_prediction_frames: w.__eaglerNetplayTouchDeltaPredictionFrames,
                            touch_quantization: w.__eaglerNetplayTouchQuantization,
                            touch_equivalent_absorb: !!w.__eaglerNetplayTouchEquivalentAbsorb,
                            incremental_reconcile: !!w.Module.eaglerOptions.netplayIncrementalReconcile,
                            cap60: !!w.Module.eaglerOptions.limitPresentationTo60,
                            input_impairment: w.__th07InputImpairment?.stats || null,
                            no_rollback: !!w.__eaglerNetplayNoRollback,
                            keep_rollback_snapshots: !!w.__eaglerNetplayKeepRollbackSnapshots,
                            dense_bullets: w.__eaglerNetplayDenseBullets || 0,
                            curved_bullets: !!w.__eaglerNetplayDenseCurved,
                            queue_stats: w.__th07PeerTransport?.perf || null,
                            transport: w.__eaglerNetplayTransport,
                            rollback: w.__eaglerNetplayLanRollback, resimulated: w.__eaglerNetplayLanResimulated,
                            confirmed: w.__eaglerNetplayLanConfirmed};
                        }""")
                        hashes.append(data["hashes"])
                        if args.require_audio:
                            audio = data.get("audio") or {}
                            if (audio.get("blocks", 0) < 10 or audio.get("nonzeroBlocks", 0) < 10 or
                                    audio.get("runningObservations", 0) < 60 or audio.get("progressedSeconds", 0) < 5):
                                raise AssertionError(f"audio was not actually running with nonzero output: {audio}")
                        if args.music == "ogg":
                            data["ogg_streams"] = [line for line in messages[player] if 'netplay audio: OGG STREAM ' in line]
                            if not data["ogg_streams"]:
                                raise AssertionError("requested OGG never reached the game decoder")
                        if args.bomb_cycle and data.get("bomb_observed_mask") != (1 << args.players) - 1:
                            raise AssertionError(f"not all Bombs executed: {data.get('bomb_observed_mask')}")
                        if args.live_bullet_audit and data.get("bullet_snapshot") == "live" and not data.get("live_bullet_audit_restores"):
                            raise AssertionError("live Bullet byte oracle was not exercised")
                        if args.input_latency:
                            latency = data.get("input_latency") or {}
                            samples = latency.get("samples") or []
                            if len(samples) < 60 or latency.get("overflow") or latency.get("invalidClock"):
                                raise AssertionError(f"invalid or incomplete latency instrumentation: {latency}")
                            fields = ("eventToPresentMs", "deliveryMs", "samplingMs", "scheduleMs", "presentationMs")
                            latency["summary"] = {key: distribution([s[key] for s in samples]) for key in fields}
                            dense = [s for s in samples if s["bullets"] >= 500]
                            if dense:
                                latency["dense_summary"] = {key: distribution([s[key] for s in dense]) for key in fields}
                        timing = data.pop("timing")
                        for key in ("drawMs", "driverMs", "presentGapMs", "densePresentMs"):
                            samples = data.get("costs", {}).pop(key, [])
                            if samples:
                                data[key] = distribution(samples)
                        data["raf"] = distribution(timing["gaps"])
                        data["visible_advance"] = distribution(timing["advanceGaps"])
                        data["simulation_fps"] = 1000 * (timing["lastFrame"] - timing["firstFrame"]) / (timing["lastTime"] - timing["firstTime"])
                        dense_time = data.get("costs", {}).get("denseElapsedMs", 0)
                        if dense_time:
                            data["dense_simulation_fps"] = 1000 * data["costs"]["denseAdvanced"] / dense_time
                        data["pass_log"] = next((line for line in messages[player] if "netplay lan stage: PASS" in line), "")
                        if not data["pass_log"] or data["confirmed"] < args.frames - 1:
                            raise AssertionError(f"unconfirmed/incomplete run: {data}")
                        report["peers"].append(data)
                    for frame in range(300, args.frames + 1, 300):
                        values = [entry.get(str(frame)) for entry in hashes]
                        if None in values or len(set(values)) != 1:
                            raise AssertionError(f"canonical divergence at frame {frame}: {values}")
                    if args.require_rollback and not any(peer["rollback"] for peer in report["peers"]):
                        raise AssertionError("workload did not exercise rollback")
                    expected_route = "rtc" if args.rtc else "relay"
                    if (args.auto_profile or args.responsive_profile) and any(p.get("performance_profile") != "balanced" for p in report["peers"]):
                        raise AssertionError("production mobile profile was not applied")
                    if args.responsive_profile and any(p.get("no_rollback") or p.get("repair_enabled") is not True for p in report["peers"]):
                        raise AssertionError("responsive production policy must retain full rollback and reliable repair")
                    if any(peer.get("transport") != expected_route for peer in report["peers"]):
                        raise AssertionError("requested transport was not used")
                    for player, peer in enumerate(report["peers"]):
                        expected_delay = (args.strong_input_delay if
                            args.strong_input_delay is not None and player != args.slow_player
                            else args.input_delay)
                        if peer.get("input_delay") != expected_delay:
                            raise AssertionError("requested input delay was not applied")
                    if any(peer.get("prediction_window") != args.prediction_window for peer in report["peers"]):
                        raise AssertionError("requested prediction window was not applied")
                    if any(peer.get("snapshot_policy") != args.snapshot_policy for peer in report["peers"]):
                        raise AssertionError("requested snapshot policy was not applied")
                    if any(peer.get("snapshot_layout") != args.snapshot_layout for peer in report["peers"]):
                        raise AssertionError("requested snapshot layout was not applied")
                    if any(peer.get("snapshot_copy") != args.snapshot_copy for peer in report["peers"]):
                        raise AssertionError("requested snapshot copy was not applied")
                    if any(peer.get("snapshot_restore") != args.snapshot_restore for peer in report["peers"]):
                        raise AssertionError("requested restore policy was not applied")
                    if any(peer.get("checkpoint_frames") != args.checkpoint_frames for peer in report["peers"]):
                        raise AssertionError("requested checkpoint span was not applied")
                    if any(peer.get("bullet_snapshot") != (args.strong_bullet_snapshot if
                           args.strong_bullet_snapshot is not None and player != args.slow_player
                           else args.bullet_snapshot) for player, peer in enumerate(report["peers"])):
                        raise AssertionError("requested bullet snapshot mode was not applied")
                    if any(peer.get("touch_delta_prediction_frames") != args.touch_delta_prediction_frames for peer in report["peers"]):
                        raise AssertionError("requested touch delta prediction window was not applied")
                    if any(peer.get("dormant_bullet_elision") != (args.dormant_bullet_elision and peer.get("bullet_snapshot") == "live") for peer in report["peers"]):
                        raise AssertionError("requested dormant Bullet capture mode was not applied")
                    if any(abs(float(peer.get("touch_quantization") or 0) - args.touch_quantization) > 1e-6 for peer in report["peers"]):
                        raise AssertionError("requested touch quantization was not applied")
                    if any(bool(peer.get("touch_equivalent_absorb")) != args.touch_equivalent_absorb for peer in report["peers"]):
                        raise AssertionError("requested touch equivalence mode was not applied")
                    if any(bool(peer.get("incremental_reconcile")) != args.incremental_reconcile for peer in report["peers"]):
                        raise AssertionError("requested incremental reconcile mode was not applied")
                    if any(peer.get("cap60") != args.limit_presentation_60 for peer in report["peers"]):
                        raise AssertionError("requested presentation cap was not applied")
                    if args.rtc_input_delay_ms is not None:
                        for peer in report["peers"]:
                            injected = peer.get("input_impairment") or {}
                            if (not injected.get("matched") or not injected.get("sent") or
                                    injected.get("errors") or injected.get("overflow")):
                                raise AssertionError("RTC input impairment was not exercised successfully")
                    for player, peer in enumerate(report["peers"]):
                        expected_no_rollback = args.no_rollback or (
                            (args.slow_no_rollback or args.slow_buffered_policy) and
                            player == args.slow_player)
                        if bool(peer.get("no_rollback")) != expected_no_rollback:
                            raise AssertionError("requested rollback mode was not applied")
                    if any(bool(peer.get("keep_rollback_snapshots")) != args.keep_rollback_snapshots for peer in report["peers"]):
                        raise AssertionError("requested snapshot mode was not applied")
                    if any(peer.get("dense_bullets") != args.dense_bullets for peer in report["peers"]):
                        raise AssertionError("requested dense bullet load was not applied")
                    if any(peer.get("curved_bullets") != args.curved_bullets for peer in report["peers"]):
                        raise AssertionError("requested curved bullet load was not applied")
                    if args.dense_bullets:
                        for peer in report["peers"]:
                            costs = peer.get("costs", {})
                            if (costs.get("denseObservedFrames", 0) < args.frames - 315 or
                                    costs.get("denseMinBullets", 0) < args.dense_bullets * 0.9):
                                raise AssertionError("dense workload did not persist throughout the measurement window")
                            if any(value["counts"][1] < args.dense_bullets * 0.9 for value in peer.get("canonical", {}).values()):
                                raise AssertionError("dense workload absent at authoritative checkpoint")
                    if any(not peer.get("costs", {}).get("captures") for peer in report["peers"]):
                        raise AssertionError("cost instrumentation was not active")
                    report["status"] = "PASS"
                finally:
                    if profile_started:
                        profile = profile_session.send("Profiler.stop")["profile"]
                        args.cpu_profile.parent.mkdir(parents=True, exist_ok=True)
                        args.cpu_profile.write_text(json.dumps(profile), encoding="utf-8")
                    for browser in browsers:
                        browser.close()
        except Exception as error:
            report["status"] = "FAIL"
            report["error"] = str(error)
            report["console_tails"] = [lines[-35:] for lines in messages]
        finally:
            relay.terminate()
            try:
                relay.wait(timeout=5)
            except subprocess.TimeoutExpired:
                relay.kill()
                relay.wait(timeout=5)
            relay_log.seek(0)
            report["relay_log"] = relay_log.read()[-128000 if args.relay_diagnostics else -8000:]
            server.shutdown()
            thread.join(timeout=5)
    text = json.dumps(report, ensure_ascii=False, indent=2)
    print(text, flush=True)
    if args.output:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(text + "\n", encoding="utf-8")
    return 0 if report.get("status") == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
