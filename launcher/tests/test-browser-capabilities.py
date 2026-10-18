"""L4 browser capability checks against current Launcher and Runtime builds.

Preconditions: current source served at --url with hosted DATA available.
Mutations: isolated browser contexts; navigator.getGamepads is unavailable.
Invariant: optional browser APIs do not prevent Runtime first-frame.
Proves: named browser/game launch lifecycle. Does NOT prove device input or gameplay.
"""

import argparse
import json
import subprocess
import time
import traceback
from pathlib import Path

from playwright.sync_api import sync_playwright

ROOT = Path(__file__).resolve().parents[1]


def products():
    result = subprocess.run(
        ["node", "--input-type=module", "-e",
         "import {PRODUCT_GAMES} from './product-catalog.mjs'; console.log(JSON.stringify(PRODUCT_GAMES))"],
        cwd=ROOT, check=True, capture_output=True, text=True, encoding="utf-8",
    )
    return json.loads(result.stdout)


def run_case(page, url, game):
    events = []
    page.expose_function("__recordCapabilityEvent", lambda event: events.append(event))
    page.add_init_script("""(() => {
      Object.defineProperty(navigator, 'getGamepads', { configurable: true, value: undefined });
      addEventListener('message', event => {
        const frame = document.getElementById('gameFrame');
        const message = event.data || {};
        if (event.source === frame?.contentWindow && event.origin === location.origin &&
            message.protocol === 'eagler-touhou/1' && message.event) {
          window.__recordCapabilityEvent({game: message.game, event: message.event, error: message.error}).catch(() => {});
        }
      });
    })()""")
    page.goto(url, wait_until="load", timeout=30000)
    page.wait_for_function("window.__eaglerBoot?.done === true", timeout=30000)
    page.evaluate("""game => {
      localStorage.setItem('eagler-touhou-first-use-notice-seen-v1', '1');
      document.querySelector('#firstUseNoticeDialog')?.close();
      localStorage.setItem(`eagler-touhou-game-options-v1-${game}`, JSON.stringify({music:'none',musicPreferenceExplicit:true,options:{}}));
      document.querySelector(`[data-game='${game}']:not(.game-multiplayer)`).click();
      const music = document.getElementById('musicSelect');
      music.value = 'none'; music.dispatchEvent(new Event('change', {bubbles:true}));
      document.getElementById('launch').click();
    }""", game)
    deadline = time.monotonic() + 180
    while time.monotonic() < deadline:
        if page.locator("#decisionDialog").evaluate("element => element.open"):
            page.locator("#decisionConfirm").click()
        matching = [event for event in events if event.get("game") == game]
        failure = next((event for event in matching if event.get("event") in ("error", "fatal")), None)
        if failure:
            raise AssertionError(failure)
        if any(event.get("event") == "first-frame" for event in matching):
            assert page.evaluate("typeof navigator.getGamepads === 'undefined'")
            return matching
        page.wait_for_timeout(100)
    raise TimeoutError(events)


def main():
    catalog = products()
    default_games = [game for game, product in catalog.items() if product["dataProvider"] == "emscripten-preload"]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--url", default="http://127.0.0.1:8130/")
    parser.add_argument("--games", nargs="+", choices=catalog, default=default_games)
    parser.add_argument("--browsers", nargs="+", choices=["chromium", "webkit"], default=["chromium", "webkit"])
    parser.add_argument("--output", type=Path, default=ROOT / "artifacts" / "validation" / "browser-capabilities" / time.strftime("%Y%m%d-%H%M%S"))
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=False)
    records = []
    with sync_playwright() as playwright:
        for engine in args.browsers:
            with getattr(playwright, engine).launch(headless=True) as browser:
                for game in args.games:
                    record = {"browser": engine, "browserVersion": browser.version, "game": game,
                              "capability": "gamepad-absent", "level": "L4"}
                    with browser.new_context(locale="zh-CN") as context:
                        page = context.new_page()
                        try:
                            record["events"] = run_case(page, args.url, game)
                            record["status"] = "PASS"
                        except Exception:
                            record.update(status="FAIL", error=traceback.format_exc())
                            page.screenshot(path=str(args.output / f"{engine}-{game}.png"))
                    records.append(record)
                    (args.output / "report.json").write_text(json.dumps(records, ensure_ascii=False, indent=2), encoding="utf-8")
                    print(f"{engine}/{game}/gamepad-absent: {record['status']}", flush=True)
    raise SystemExit(1 if any(record["status"] != "PASS" for record in records) else 0)


if __name__ == "__main__":
    main()
