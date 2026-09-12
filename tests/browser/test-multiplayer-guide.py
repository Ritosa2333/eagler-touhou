from __future__ import annotations

import json
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
                urllib.request.urlopen(f"http://127.0.0.1:{port}/MULTIPLAYER.md", timeout=1).close()
                break
            except Exception:
                if time.time() >= deadline:
                    raise
                time.sleep(0.1)

        with sync_playwright() as playwright:
            browser = playwright.chromium.launch(headless=True)
            page = browser.new_page(viewport={"width": 430, "height": 820})
            errors: list[str] = []
            page.on("pageerror", lambda error: errors.append(str(error)))
            page.goto(f"http://127.0.0.1:{port}/?game=th07mp", wait_until="load")
            page.wait_for_function("window.__eaglerBoot?.done === true")
            page.wait_for_timeout(800)
            page.evaluate("""() => {
              const changelog = document.getElementById('changelogDialog');
              if (changelog?.open) changelog.close();
            }""")
            assert page.locator("#mpGuideOpen").inner_text() == "联机玩法介绍"
            assert page.locator("#mpGuideOpen").locator("svg").count() == 1
            assert page.locator("#mpGuideOpen").evaluate(
                "button => Boolean(button.compareDocumentPosition(document.getElementById('mpNetworkCheck')) & Node.DOCUMENT_POSITION_FOLLOWING)"
            )
            page.locator("#mpGuideOpen").click()
            page.wait_for_function("document.querySelectorAll('#mpGuideContent h2').length === 9")
            assert page.locator("#mpGuideContent h2").first.inner_text() == "1. Boss 生命值缩放"
            assert "20 点火力" in page.locator("#mpGuideContent").inner_text()
            backdrop = page.locator("#mpGuideDialog").evaluate(
                "element => ({ background: getComputedStyle(element, '::backdrop').backgroundColor, filter: getComputedStyle(element, '::backdrop').backdropFilter })"
            )
            assert backdrop["background"] in ("rgba(0, 0, 0, 0)", "transparent"), backdrop
            assert backdrop["filter"] in ("none", ""), backdrop
            assert not errors, errors
            print(json.dumps({"multiplayerGuide": "PASS", "headings": 9, "viewport": [430, 820], "backdrop": backdrop}, ensure_ascii=False))
            browser.close()
        return 0
    finally:
        server.terminate()
        server.wait(timeout=5)


if __name__ == "__main__":
    raise SystemExit(main())
