#!/usr/bin/env python3
"""Run the TH07 TH_DEV_TOOLS ReplayExtension round-trip inside the Web runtime."""

from __future__ import annotations

import argparse
import functools
import http.server
import socketserver
import threading
import time
from pathlib import Path

from playwright.sync_api import sync_playwright


ROOT = Path(__file__).resolve().parents[1]


class QuietHandler(http.server.SimpleHTTPRequestHandler):
    def log_message(self, _format: str, *_args: object) -> None:
        pass


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", type=int, default=8141)
    parser.add_argument("--timeout", type=float, default=45.0)
    args = parser.parse_args()

    handler = functools.partial(QuietHandler, directory=str(ROOT))
    with socketserver.TCPServer(("127.0.0.1", args.port), handler) as server:
        thread = threading.Thread(target=server.serve_forever, daemon=True)
        thread.start()
        try:
            with sync_playwright() as playwright:
                browser = playwright.chromium.launch(
                    headless=True,
                    args=["--autoplay-policy=no-user-gesture-required"],
                )
                page = browser.new_page(viewport={"width": 900, "height": 700})
                console_messages: list[str] = []
                page.on("console", lambda message: console_messages.append(message.text))
                page.on("pageerror", lambda error: console_messages.append(f"pageerror: {error}"))
                page.goto(
                    f"http://127.0.0.1:{args.port}/tests/replay-extension-browser-host.html",
                    wait_until="domcontentloaded",
                )
                deadline = time.time() + args.timeout
                state: dict[str, str] = {}
                while time.time() < deadline:
                    state = page.evaluate(
                        """() => ({
                          exit: globalThis.__th07ReplayExtensionSelfTestExit || '',
                          failure: globalThis.__th07ReplayExtensionSelfTestFailure || '',
                        })"""
                    )
                    if state["failure"] or state["exit"]:
                        break
                    time.sleep(0.05)
                browser.close()
        finally:
            server.shutdown()

    passed_log = any(
        "ReplayExtension round-trip self-test: PASS" in message
        for message in console_messages
    )
    if state.get("failure"):
        raise RuntimeError(state["failure"])
    if state.get("exit") != "success" or not passed_log:
        raise RuntimeError(
            "ReplayExtension browser self-test failed or timed out; "
            f"state={state}; console={console_messages[-30:]}"
        )
    print("TH07 ReplayExtension browser round-trip: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
