#!/usr/bin/env python3
"""Real TH07 single-player Replay menu/playback gate."""

from __future__ import annotations

import argparse
import functools
import http.server
import socket
import socketserver
import threading
import time
from pathlib import Path

from playwright.sync_api import sync_playwright


ROOT = Path(__file__).resolve().parents[1]


class FixtureHandler(http.server.SimpleHTTPRequestHandler):
    fixture: Path

    def do_GET(self) -> None:
        if self.path.split("?", 1)[0] == "/__ordinary_replay_fixture.rpy":
            data = self.fixture.read_bytes()
            self.send_response(200)
            self.send_header("Content-Type", "application/octet-stream")
            self.send_header("Content-Length", str(len(data)))
            self.end_headers()
            self.wfile.write(data)
            return
        super().do_GET()

    def log_message(self, _format: str, *_args: object) -> None:
        pass


def free_port() -> int:
    with socket.socket() as sock:
        sock.bind(("127.0.0.1", 0))
        return int(sock.getsockname()[1])


def pulse_select(page) -> None:
    page.evaluate("""() => {
      const runtime = document.getElementById('runtime')?.contentWindow;
      const init = { key: 'z', code: 'KeyZ', bubbles: true, cancelable: true };
      runtime?.dispatchEvent(new KeyboardEvent('keydown', init));
      runtime?.setTimeout(() => runtime.dispatchEvent(new KeyboardEvent('keyup', init)), 80);
    }""")


def snapshot(page) -> dict[str, object]:
    return page.evaluate("""() => {
      const runtime = document.getElementById('runtime')?.contentWindow;
      return {
        launched: !!globalThis.__th07OrdinaryReplayLaunched,
        failure: String(globalThis.__th07OrdinaryReplayFailure || ''),
        observed: !!runtime?.__eaglerOrdinaryReplayPlaybackObserved,
        frame: Number(runtime?.__eaglerOrdinaryReplayPlaybackFrame ?? -1),
        input: Number(runtime?.__eaglerOrdinaryReplayPlaybackInput ?? -1),
        stage: Number(runtime?.__eaglerOrdinaryReplayPlaybackStage ?? -1),
        files: globalThis.__th07OrdinaryReplayFiles || [],
        filesFound: Number(runtime?.__eaglerOrdinaryReplayFilesFound ?? -1),
      };
    }""")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--fixture", type=Path, required=True)
    parser.add_argument("--timeout", type=float, default=90.0)
    parser.add_argument("--screenshot", type=Path)
    args = parser.parse_args()
    data = args.fixture.read_bytes()
    if data[:4] != b"T7RP":
        raise RuntimeError(f"fixture has invalid magic: {data[:4]!r}")

    FixtureHandler.fixture = args.fixture.resolve()
    handler = functools.partial(FixtureHandler, directory=str(ROOT))
    port = free_port()
    with socketserver.TCPServer(("127.0.0.1", port), handler) as server:
        thread = threading.Thread(target=server.serve_forever, daemon=True)
        thread.start()
        try:
            with sync_playwright() as playwright:
                browser = playwright.chromium.launch(headless=True)
                page = browser.new_page(viewport={"width": 900, "height": 700})
                errors: list[str] = []
                page.on("pageerror", lambda error: errors.append(str(error)))
                page.goto(
                    f"http://127.0.0.1:{port}/tests/ordinary-replay-browser-host.html",
                    wait_until="domcontentloaded", timeout=30_000)
                deadline = time.time() + min(args.timeout, 165.0)
                next_pulse = 0.0
                state = snapshot(page)
                while time.time() < deadline:
                    state = snapshot(page)
                    if errors or state["failure"]:
                        break
                    if state["observed"] and state["frame"] >= 300:
                        break
                    if state["launched"] and time.time() >= next_pulse:
                        pulse_select(page)
                        next_pulse = time.time() + 1.0
                    time.sleep(0.1)
                if args.screenshot:
                    page.screenshot(path=str(args.screenshot), full_page=True)
                browser.close()
        finally:
            server.shutdown()

    if errors or state["failure"] or not state["observed"] or state["frame"] < 300:
        raise RuntimeError(f"TH07 ordinary Replay incomplete: state={state} errors={errors}")
    print(
        "TH07 ordinary Replay browser smoke: PASS "
        f"frame={state['frame']} stage={state['stage']} input={state['input']}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
