#!/usr/bin/env python3
"""Temporary browser driver for the built-in TH07 demo determinism probe."""

from __future__ import annotations

import argparse
import json
import time

from playwright.sync_api import sync_playwright


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--url",
        default="http://127.0.0.1:8766/th07-eagler/tests/demo-determinism-browser-host.html",
    )
    parser.add_argument("--timeout", type=float, default=300.0)
    parser.add_argument("--max-frames", type=int, default=14000)
    parser.add_argument("--demo", type=int, choices=(0, 1, 2), default=0)
    args = parser.parse_args()

    separator = "&" if "?" in args.url else "?"
    target_url = f"{args.url}{separator}demo={args.demo}"

    with sync_playwright() as playwright:
        browser = playwright.chromium.launch(
            headless=True,
            args=["--disable-frame-rate-limit", "--disable-gpu-vsync"],
        )
        page = browser.new_page(viewport={"width": 800, "height": 700})
        logs: list[str] = []
        errors: list[str] = []
        saw_mismatch = [False]
        saw_complete = [False]

        def on_console(message) -> None:
            text = message.text
            logs.append(text)
            if "demo determinism:" in text:
                print(text, flush=True)
                if "FIRST_MISMATCH" in text:
                    saw_mismatch[0] = True
                if "COMPLETE" in text or "PASS" in text:
                    saw_complete[0] = True

        def on_page_error(error) -> None:
            errors.append(str(error))
            print("PAGEERROR " + str(error), flush=True)

        page.on("console", on_console)
        page.on("pageerror", on_page_error)
        page.goto(target_url, wait_until="domcontentloaded", timeout=30_000)
        print("LOADED " + target_url, flush=True)

        page.wait_for_function(
            "() => globalThis.__demoDeterminismLaunched || globalThis.__demoDeterminismFailure",
            timeout=30_000,
        )
        failure = page.evaluate("() => globalThis.__demoDeterminismFailure || ''")
        if failure:
            raise RuntimeError(failure)

        deadline = time.time() + args.timeout
        next_pulse = 0.0
        runtime_state: dict[str, object] = {}
        highest_frame = -1
        while (
            time.time() < deadline
            and not saw_mismatch[0]
            and not saw_complete[0]
            and not errors
        ):
            now = time.time()
            if now >= next_pulse and not any("armed built-in demo" in line for line in logs):
                page.keyboard.press("z")
                next_pulse = now + 0.5
            runtime_state = page.evaluate(
                """() => {
                  const runtime = document.getElementById('runtime')?.contentWindow;
                  return {
                    frame: runtime?.__eaglerDemoDeterminismFrame ?? -1,
                    comparedFrames: runtime?.__eaglerDemoDeterminismComparedFrames ?? 0,
                    mismatch: runtime?.__eaglerDemoDeterminismFirstMismatch ?? null,
                  };
                }"""
            )
            if runtime_state.get("mismatch"):
                saw_mismatch[0] = True
            current_frame = int(runtime_state.get("frame", -1))
            if current_frame >= 0 and highest_frame > 1000 and current_frame < highest_frame:
                saw_complete[0] = True
            highest_frame = max(highest_frame, current_frame)
            if current_frame >= args.max_frames:
                saw_complete[0] = True
            time.sleep(0.05)

        print(
            "DONE "
            + json.dumps(
                {
                    "mismatch": saw_mismatch[0],
                    "complete": saw_complete[0],
                    "errors": errors,
                    "determinismLines": sum("demo determinism:" in line for line in logs),
                    "runtime": runtime_state,
                },
                ensure_ascii=False,
            ),
            flush=True,
        )
        browser.close()

    return 0 if not errors else 1


if __name__ == "__main__":
    raise SystemExit(main())
