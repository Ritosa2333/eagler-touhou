#!/usr/bin/env python3
"""TH07 rollback -> save -> menu -> multiplayer Replay playback gate."""

from __future__ import annotations

import functools
import argparse
import re
import urllib.parse
import http.server
import os
import socket
import socketserver
import subprocess
import sys
import threading
import time
from pathlib import Path

from integration_support import require_host_relay
from playwright.sync_api import sync_playwright


ROOT = Path(__file__).resolve().parents[1]
HOST_ROOT, RELAY_SCRIPT = require_host_relay()


class QuietHandler(http.server.SimpleHTTPRequestHandler):
    def log_message(self, _format: str, *_args: object) -> None:
        pass


def free_port() -> int:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.bind(("127.0.0.1", 0))
        return int(sock.getsockname()[1])


def pulse_runtime_select(page) -> None:
    page.evaluate(
        """() => {
          const runtime = document.getElementById('runtime')?.contentWindow;
          runtime?.dispatchEvent(new KeyboardEvent('keydown', {
            key: 'z', code: 'KeyZ', bubbles: true, cancelable: true,
          }));
          runtime?.setTimeout(() => runtime.dispatchEvent(new KeyboardEvent('keyup', {
            key: 'z', code: 'KeyZ', bubbles: true, cancelable: true,
          })), 50);
        }"""
    )


def snapshot(page) -> dict[str, object]:
    return page.evaluate(
        """() => {
          const runtime = document.getElementById('runtime')?.contentWindow;
          return {
            launched: !!globalThis.__th07ReplaySmokeLaunched,
            hostFailure: String(globalThis.__th07ReplaySmokeFailure || ''),
            failed: !!runtime?.__eaglerNetplayFailed,
            error: String(runtime?.__eaglerNetplayError || ''),
            frame: Number(runtime?.__eaglerNetplayLanFrame || 0),
            rollback: Number(runtime?.__eaglerNetplayLanRollback || 0),
            replaySaved: !!runtime?.__eaglerNetplayReplaySaved,
            replaySavedPath: String(runtime?.__eaglerNetplayReplaySavedPath || ''),
            replaySavedStoragePath: String(runtime?.__eaglerNetplayReplaySavedStoragePath || ''),
            playback: !!runtime?.__eaglerNetplayReplayPlaybackObserved,
            playbackFrame: Number(runtime?.__eaglerNetplayReplayPlaybackFrame ?? -1),
            playbackPlayers: Number(runtime?.__eaglerNetplayReplayPlaybackPlayerCount || 0),
            independent: !!runtime?.__eaglerNetplayReplayPlaybackIndependentInputs,
            playbackStage: Number(runtime?.__eaglerNetplayReplayPlaybackStage || 0),
            expectedFrames: Number(runtime?.__eaglerNetplayReplayExpectedFrames || 0),
            comparedFrames: Number(runtime?.__eaglerNetplayReplayComparedFrames || 0),
            inputMismatch: !!runtime?.__eaglerNetplayReplayInputMismatch,
            inputCoverage: Number(runtime?.__eaglerNetplayReplayInputCoverage || 0),
            snapshotBackend: String(runtime?.__eaglerNetplayBulletSnapshot || ''),
            configuredDelay: Number(runtime?.Module?.eaglerOptions?.netplayInputDelayFrames),
          };
        }"""
    )


def main() -> int:
    parser = argparse.ArgumentParser(description="Verify MP Replay storage and exact input playback")
    parser.add_argument("players", type=int, nargs="?", default=2, choices=(2,3))
    parser.add_argument("--browser-channel", default=None)
    parser.add_argument("--build", default="build-web-th07-netplay-replay-audit")
    parser.add_argument("--input-delay", type=int, choices=range(13))
    parser.add_argument("--bullet-snapshot", choices=("journal","live"))
    args = parser.parse_args()
    if not re.fullmatch(r"[A-Za-z0-9_.-]+", args.build):
        parser.error("invalid build directory")
    player_count = args.players
    test_frames = 600 if player_count == 2 else 180
    playback_target = 300 if player_count == 2 else 60
    http_port = free_port()
    relay_port = free_port()
    room = f"th07-replay-{int(time.time() * 1000)}"
    env = os.environ.copy()
    env.update({
        "TH07_RELAY_HOST": "127.0.0.1",
        "TH07_RELAY_PORT": str(relay_port),
        "TH07_RELAY_DELAY_MS": "55",
        "TH07_RELAY_JITTER_MS": "10",
        "TH07_STUN_URLS": "",
    })
    handler = functools.partial(QuietHandler, directory=str(ROOT))
    with http.server.ThreadingHTTPServer(("127.0.0.1", http_port), handler) as server:
        thread = threading.Thread(target=server.serve_forever, daemon=True)
        thread.start()
        relay = subprocess.Popen(
            ["node", str(RELAY_SCRIPT)], cwd=HOST_ROOT, env=env,
            stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True,
        )
        failures = [""] * player_count
        messages: list[list[str]] = [[] for _ in range(player_count)]
        try:
            assert relay.stdout is not None
            relay_line = relay.stdout.readline().strip()
            if " relay listening " not in relay_line.lower():
                raise RuntimeError(f"relay failed to start: {relay_line}")
            with sync_playwright() as playwright:
                browsers = []
                pages = []
                try:
                    for player in range(player_count):
                        browser = playwright.chromium.launch(
                            headless=True,
                            channel=args.browser_channel,
                            args=[
                                "--disable-background-timer-throttling",
                                "--disable-backgrounding-occluded-windows",
                                "--disable-renderer-backgrounding",
                            ],
                        )
                        browsers.append(browser)
                        context = browser.new_context(viewport={"width": 960, "height": 720})
                        context.add_init_script("delete globalThis.RTCPeerConnection")
                        page = context.new_page()
                        page.on("pageerror", lambda error, i=player: failures.__setitem__(i, str(error)))
                        page.on("console", lambda message, i=player: messages[i].append(message.text))
                        relay_url = (
                            f"ws://127.0.0.1:{relay_port}/?room={room}&player={player}"
                        )
                        page.goto(
                            f"http://127.0.0.1:{http_port}/tests/"
                            f"netplay-replay-browser-host.html?player={player}&players={player_count}"
                            f"&frames={test_frames}&relay={relay_url}&" + urllib.parse.urlencode({
                                "build": args.build,
                                **({"inputDelay":args.input_delay} if args.input_delay is not None else {}),
                                **({"bulletSnapshot":args.bullet_snapshot} if args.bullet_snapshot else {}),
                            }),
                            wait_until="load", timeout=30_000,
                        )
                        pages.append(page)

                    deadline = time.time() + 120
                    last_pulse = [0.0] * player_count
                    states = [snapshot(page) for page in pages]
                    while time.time() < deadline:
                        states = [snapshot(page) for page in pages]
                        if any(failures) or any(state["hostFailure"] or state["failed"] for state in states):
                            break
                        now = time.time()
                        for index, (page, state) in enumerate(zip(pages, states)):
                            if state["replaySaved"] and not state["playback"] and now - last_pulse[index] >= 1:
                                pulse_runtime_select(page)
                                last_pulse[index] = now
                        if all(
                            state["playback"] and state["playbackFrame"] >= playback_target and
                            state["comparedFrames"] >= playback_target
                            for state in states
                        ):
                            break
                        time.sleep(0.1)
                finally:
                    for browser in browsers:
                        browser.close()
        finally:
            relay.terminate()
            try:
                relay.wait(timeout=5)
            except subprocess.TimeoutExpired:
                relay.kill()
            server.shutdown()

    if any(failures):
        raise RuntimeError(f"page errors: {failures}; tails={[values[-30:] for values in messages]}")
    for state in states:
        if args.input_delay is not None and state["configuredDelay"] != args.input_delay:
            raise AssertionError(f"Replay test did not use requested input delay: {state}")
        if args.bullet_snapshot and state["snapshotBackend"] != args.bullet_snapshot:
            raise AssertionError(f"Replay test did not use requested snapshot backend: {state}")
    if not all(
        state["launched"] and not state["hostFailure"] and not state["failed"] and
        state["rollback"] > 0 and state["replaySaved"] and
        str(state["replaySavedPath"]).endswith(".rpyx") and
        str(state["replaySavedStoragePath"]).startswith("/savesth07-multiplayer/replay/") and
        state["playback"] and state["playbackFrame"] >= playback_target and
        state["playbackPlayers"] == player_count and state["independent"] and
        state["playbackStage"] == 1 and state["expectedFrames"] == test_frames and
        state["inputCoverage"] == 31 and
        state["comparedFrames"] >= playback_target and not state["inputMismatch"]
        for state in states
    ):
        raise RuntimeError(
            f"TH07 Replay lifecycle incomplete: states={states}; "
            f"tails={[values[-30:] for values in messages]}"
        )
    print(
        "TH07 rollback Replay save/playback: PASS "
        f"players={player_count} "
        f"rollback={'/'.join(str(state['rollback']) for state in states)} "
        f"frame={'/'.join(str(state['playbackFrame']) for state in states)} "
        f"exact={'/'.join(str(state['comparedFrames']) for state in states)}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
