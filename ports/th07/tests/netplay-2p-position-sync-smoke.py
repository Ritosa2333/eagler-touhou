from __future__ import annotations

import re
import sys
import time
import urllib.parse

from playwright.sync_api import sync_playwright


def main() -> int:
    if len(sys.argv) != 3:
        raise SystemExit("usage: netplay-2p-position-sync-smoke.py HARNESS_URL RELAY_URL")
    harness, relay_base = sys.argv[1:]
    room = f"position-{int(time.time() * 1000)}"
    messages: list[list[str]] = [[], []]
    with sync_playwright() as playwright:
        browser = playwright.chromium.launch(headless=True)
        pages = []
        for player in range(2):
            context = browser.new_context()
            context.add_init_script("delete globalThis.RTCPeerConnection")
            relay = f"{relay_base}?room={room}&run=1"
            query = urllib.parse.urlencode({
                "debugHarness": "netplay-lan-stage1",
                "netplayMode": "lan",
                "netplayUrl": relay,
                "netplayPlayer": player,
                "netplayPlayerCount": 2,
                "netplaySeed": 19005,
                "netplayDifficulty": 1,
                "netplayPhysicalInput": 1,
                "netplayTestFrames": 360,
            })
            page = context.new_page()
            page.on("console", lambda message, slot=player: messages[slot].append(message.text))
            page.on("pageerror", lambda error, slot=player: messages[slot].append(f"pageerror: {error}"))
            page.goto(f"{harness}?{query}", wait_until="load", timeout=30000)
            pages.append(page)

        try:
            pages[0].wait_for_function(
                "document.getElementById('runtime')?.contentWindow?.__eaglerNetplayLanFrame >= 30",
                timeout=20000,
            )
        except Exception:
            print(f"startup diagnostics={messages}")
            raise
        pages[0].keyboard.down("ArrowLeft")
        pages[1].keyboard.down("ArrowRight")
        for page in pages:
            page.wait_for_function(
                "document.getElementById('runtime')?.contentWindow?.__eaglerNetplayLanStageDone === true",
                timeout=30000,
            )
        browser.close()

    pattern = re.compile(
        r"PLAYERS p1=\((-?[0-9.]+),(-?[0-9.]+)\) "
        r"p2=\((-?[0-9.]+),(-?[0-9.]+)\)"
    )
    positions = []
    for slot in range(2):
        line = next((value for value in messages[slot] if "netplay lan stage: PLAYERS" in value), None)
        if not line:
            raise AssertionError(f"P{slot + 1} missing final positions: {messages[slot][-30:]}")
        match = pattern.search(line)
        if not match:
            raise AssertionError(line)
        positions.append(tuple(float(value) for value in match.groups()))
    if positions[0] != positions[1]:
        raise AssertionError(f"2P position divergence: {positions}")
    passes = [sum("netplay lan stage: PASS" in line for line in endpoint) for endpoint in messages]
    if passes != [1, 1]:
        raise AssertionError(f"missing PASS: {passes}; tails={[endpoint[-20:] for endpoint in messages]}")
    print(f"TH07 2P position sync: PASS positions={positions[0]}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
