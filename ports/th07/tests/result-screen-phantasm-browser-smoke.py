#!/usr/bin/env python3
"""Repeatedly exercise the real TH07 Phantasm ResultScreen in WebAssembly."""

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
AUDIT_HTML = ROOT / "build-web-th07-netplay-replay-audit" / "th07.html"


class QuietHandler(http.server.SimpleHTTPRequestHandler):
    def log_message(self, _format: str, *_args: object) -> None:
        pass


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", type=int, default=8143)
    parser.add_argument("--rounds", type=int, default=5)
    parser.add_argument("--timeout", type=float, default=10.0)
    args = parser.parse_args()

    if not AUDIT_HTML.exists():
        raise RuntimeError("TH07 replay-audit Web build is missing")

    handler = functools.partial(QuietHandler, directory=str(ROOT))
    with socketserver.TCPServer(("127.0.0.1", args.port), handler) as server:
        thread = threading.Thread(target=server.serve_forever, daemon=True)
        thread.start()
        try:
            with sync_playwright() as playwright:
                browser = playwright.chromium.launch(headless=True)
                try:
                    for round_index in range(args.rounds):
                        context = browser.new_context(viewport={"width": 900, "height": 700})
                        page = context.new_page()
                        console: list[str] = []
                        page_errors: list[str] = []
                        page.on("console", lambda message, out=console: out.append(message.text))
                        page.on("pageerror", lambda error, out=page_errors: out.append(str(error)))
                        target = (
                            f"http://127.0.0.1:{args.port}/"
                            "tests/result-screen-phantasm-browser-host.html"
                        )
                        page.goto(target, wait_until="load", timeout=30_000)
                        deadline = time.time() + args.timeout
                        while time.time() < deadline and not any(
                            "Result Stats visual test phantasm=1" in value for value in console
                        ):
                            page.wait_for_timeout(50)
                        hit = any(
                            "Result Stats visual test phantasm=1" in value for value in console
                        )
                        if not hit:
                            runtime_state = page.evaluate(
                                """() => ({
                                  launched: !!globalThis.__th07PhantasmResultLaunched,
                                  failure: globalThis.__th07PhantasmResultFailure || '',
                                  exit: globalThis.__th07PhantasmResultExit || '',
                                  debugHarness: document.getElementById('runtime')?.contentWindow
                                    ?.Module?.eaglerOptions?.debugHarness || null,
                                  status: document.getElementById('status')?.textContent || '',
                                })"""
                            )
                            raise RuntimeError(
                                f"round {round_index + 1}: Phantasm ResultScreen was not dispatched; "
                                f"runtime={runtime_state} tail={console[-30:]}"
                            )
                        # Keep the production DrawStats/OnDraw path alive long enough
                        # for the old difficulty=5 out-of-bounds reads to repeat.
                        page.wait_for_timeout(2500)
                        runtime_alive = page.evaluate(
                            """async () => {
                              const runtime = document.getElementById('runtime')?.contentWindow;
                              if (!runtime?.Module?.canvas?.isConnected) return false;
                              return await new Promise(resolve => runtime.setTimeout(
                                () => resolve(true), 0));
                            }"""
                        )
                        if page.is_closed() or page_errors or not runtime_alive:
                            raise RuntimeError(
                                f"round {round_index + 1}: ResultScreen failed; "
                                f"closed={page.is_closed()} alive={runtime_alive} "
                                f"errors={page_errors} tail={console[-30:]}"
                            )
                        context.close()
                finally:
                    browser.close()
        finally:
            server.shutdown()

    print(f"TH07 Phantasm ResultScreen browser smoke: PASS rounds={args.rounds}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
