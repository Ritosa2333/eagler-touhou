from __future__ import annotations

import json
import os
import socket
import subprocess
import time
import urllib.request
from pathlib import Path

from playwright.sync_api import sync_playwright


PROJECT = Path(__file__).resolve().parents[2]


def free_port() -> int:
    with socket.socket() as sock:
        sock.bind(("127.0.0.1", 0))
        return int(sock.getsockname()[1])


def swipe(page, start: tuple[int, int], end: tuple[int, int]) -> None:
    page.mouse.move(*start)
    page.mouse.down()
    page.mouse.move(*end, steps=8)
    page.mouse.up()


def main() -> int:
    port = free_port()
    server = subprocess.Popen(
        ["node", "scripts/serve.mjs", str(port)],
        cwd=PROJECT,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    try:
        deadline = time.time() + 10
        while True:
            try:
                urllib.request.urlopen(f"http://127.0.0.1:{port}/", timeout=1).close()
                break
            except Exception:
                if time.time() >= deadline:
                    raise
                time.sleep(0.1)

        with sync_playwright() as playwright:
            browser = playwright.chromium.launch(headless=True)
            page = browser.new_page(viewport={"width": 430, "height": 820})
            page.goto(f"http://127.0.0.1:{port}/", wait_until="load")
            page.wait_for_function("window.__eaglerBoot?.done === true")
            page.wait_for_timeout(800)
            page.evaluate("""() => {
              document.getElementById('changelogDialog')?.close();
              document.getElementById('siteNotice').hidden = true;
            }""")
            page.wait_for_timeout(240)
            assert page.locator("#changelogEdgeCue").count() == 0
            assert page.locator("#mpSettingsRoomDrawerToggle").is_hidden()
            artifact_dir = os.environ.get("EAGLER_EDGE_DRAWER_ARTIFACT_DIR")
            if artifact_dir:
                Path(artifact_dir).mkdir(parents=True, exist_ok=True)
                page.screenshot(path=str(Path(artifact_dir) / "changelog-cue-closed.png"), full_page=True)

            swipe(page, (428, 320), (348, 321))
            page.wait_for_function("document.getElementById('changelogDialog')?.open === true")
            page.locator("#changelogCloseHint").click()
            page.wait_for_function("document.getElementById('changelogDialog')?.open === false")

            swipe(page, (428, 320), (348, 321))
            page.wait_for_function("document.getElementById('changelogDialog')?.open === true")
            page.wait_for_timeout(320)
            changelog_box = page.locator("#changelogDialog").bounding_box()
            assert changelog_box and abs(changelog_box["x"] + changelog_box["width"] - 430) < 1.5, changelog_box
            backdrop = page.locator("#changelogDialog").evaluate(
                "element => getComputedStyle(element, '::backdrop').backgroundColor"
            )
            assert backdrop in ("rgba(0, 0, 0, 0)", "transparent"), backdrop
            heading_style = page.locator("#changelogText .changelog-item h2").first.evaluate(
                "element => ({ size: parseFloat(getComputedStyle(element).fontSize), weight: parseInt(getComputedStyle(element).fontWeight, 10) })"
            )
            assert heading_style["size"] >= 21 and heading_style["weight"] >= 700, heading_style
            if artifact_dir:
                Path(artifact_dir).mkdir(parents=True, exist_ok=True)
                page.screenshot(path=str(Path(artifact_dir) / "changelog-right.png"), full_page=True)

            swipe(page, (90, 320), (170, 321))
            page.wait_for_function("document.getElementById('changelogDialog')?.open === false")

            swipe(page, (2, 600), (82, 601))
            page.wait_for_function("document.getElementById('siteNotice')?.hidden === false")
            page.wait_for_timeout(300)
            notice_box = page.locator("#siteNotice").bounding_box()
            assert notice_box and abs(notice_box["x"]) < 1.5, notice_box
            if artifact_dir:
                page.screenshot(path=str(Path(artifact_dir) / "notice-left.png"), full_page=True)

            swipe(page, (300, 600), (220, 601))
            page.wait_for_function("document.getElementById('siteNotice')?.hidden === true")

            result = {
                "pass": True,
                "viewport": [430, 820],
                "changelog": {"side": "right", "box": changelog_box, "backdrop": backdrop, "heading": heading_style},
                "cue": "removed",
                "notice": {"side": "left", "box": notice_box},
                "gestures": ["right-edge-reveal", "bottom-hint-close", "right-retract", "left-edge-reveal", "left-retract"],
            }
            print(json.dumps(result, ensure_ascii=False))
            browser.close()
        return 0
    finally:
        server.terminate()
        server.wait(timeout=5)


if __name__ == "__main__":
    raise SystemExit(main())
