from __future__ import annotations

import json
import socket
import subprocess
import threading
from functools import partial
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path

from playwright.sync_api import sync_playwright


PROJECT = Path(__file__).resolve().parents[2]


class QuietHandler(SimpleHTTPRequestHandler):
    def log_message(self, *_args) -> None:
        pass


def free_port() -> int:
    with socket.socket() as sock:
        sock.bind(("127.0.0.1", 0))
        return int(sock.getsockname()[1])


def main() -> int:
    # Keep this browser test focused on the guide. The full development server
    # needs private Runtime DATA inputs which are unrelated to this surface.
    subprocess.run(
        ["node", "scripts/build-launcher.mjs", "--force"],
        cwd=PROJECT,
        check=True,
        stdout=subprocess.DEVNULL,
    )

    port = free_port()
    handler = partial(QuietHandler, directory=str(PROJECT))
    server = ThreadingHTTPServer(("127.0.0.1", port), handler)
    thread = threading.Thread(target=server.serve_forever, daemon=True)
    thread.start()
    try:
        with sync_playwright() as playwright:
            browser = playwright.chromium.launch(headless=True)
            page = browser.new_page(viewport={"width": 430, "height": 820})
            page.goto(f"http://127.0.0.1:{port}/public/", wait_until="domcontentloaded")
            page.set_content(
                """
<!doctype html>
<html lang="zh-CN">
<head>
  <meta charset="utf-8">
  <link rel="stylesheet" href="/public/styles.css">
</head>
<body>
  <dialog class="multiplayer-guide-dialog" id="mpGuideDialog" aria-labelledby="mpGuideTitle">
    <article class="multiplayer-guide-window">
      <header><h1 id="mpGuideTitle">联机玩法介绍</h1><button id="mpGuideClose">×</button></header>
      <div class="multiplayer-guide-content" id="mpGuideContent"></div>
    </article>
  </dialog>
</body>
</html>
""",
                wait_until="load",
            )
            errors: list[str] = []
            page.on("pageerror", lambda error: errors.append(str(error)))
            page.evaluate(
                """async () => {
                  const module = await import('/.cache/build/browser/assets/launcher/multiplayer-guide.mjs');
                  window.__mpGuide = module.createMultiplayerGuideController({ getGameId: () => 'th07' });
                  await window.__mpGuide.show();
                }"""
            )
            page.wait_for_selector("#mpGuideContent [data-mp-rule-guide]")

            game_tabs = page.locator("#mpGuideContent .multiplayer-rule-game-tab")
            assert game_tabs.count() == 4
            assert [game_tabs.nth(i).get_attribute("data-game") for i in range(4)] == ["th06", "th07", "th08", "th10"]
            assert game_tabs.all_inner_texts() == ["红魔乡", "妖妖梦", "永夜抄", "风神录"]
            for i in range(4):
                box = game_tabs.nth(i).bounding_box()
                assert box and box["x"] >= 0 and box["x"] + box["width"] <= 430
            assert page.locator('#mpGuideContent .multiplayer-rule-game-tab[data-game="th07"]').get_attribute("aria-selected") == "true"

            th07_panel = page.locator('#mpGuideContent .multiplayer-rule-panel[data-game="th07"]')
            common = th07_panel.locator('details[data-scope="common"]')
            specific = th07_panel.locator('details[data-scope="specific"]')
            assert common.get_attribute("open") is None
            assert specific.get_attribute("open") is None
            assert common.locator("summary").inner_text().startswith("通用规则")
            assert specific.locator("summary").inner_text().startswith("本作特有规则")

            page.locator('button[data-game="th06"]').click()
            th06_specific = page.locator('.multiplayer-rule-panel[data-game="th06"] details[data-scope="specific"]')
            th06_specific.locator("summary").click()
            assert th06_specific.locator(".multiplayer-rule-disclosure-body").inner_text().strip() == "无"

            common.locator("summary").click()
            common_text = common.inner_text()
            assert "Boss 生命值倍率" in common_text
            assert "敌人掉落物" in common_text
            assert "Power道具机制如下" not in common_text
            assert "所有玩家都会同时获得奖励残机" in common_text
            assert common.locator("blockquote.markdown-blockquote").count() == 1
            assert "赠送Power者快速点按射击键 8 次" in common_text
            assert "剧情、路线与共享关卡内容" in common_text
            assert "统一跟随房主 / P1 的选择" in common_text
            assert "符卡失败与收取属于同一个共享符卡状态" in common_text
            assert "单个玩家死亡：" in common_text
            assert "全部玩家死亡：" in common_text
            assert "团队团灭后保留 180 个正常游戏逻辑帧" in common_text
            assert "0 ~ 5 Power，且 Power == Bomb（TH10、TH11）" in common_text
            assert "所有作品的多人模式均不进入 Continue 流程" in common_text
            common.locator("summary").click()
            assert common.get_attribute("open") is None

            specific.locator("summary").click()
            assert th07_panel.is_visible()
            assert "樱点+（Cherry+）调整" in th07_panel.inner_text()
            assert "在妖妖梦中，Power机制经过微调" in th07_panel.inner_text()
            specific.locator("summary").click()
            assert specific.get_attribute("open") is None

            page.locator('#mpGuideContent .multiplayer-rule-game-tab[data-game="th10"]').click()
            th10_panel = page.locator('#mpGuideContent .multiplayer-rule-panel[data-game="th10"]')
            th10_specific = th10_panel.locator('details[data-scope="specific"]')
            th10_specific.locator("summary").click()
            th10_text = th10_panel.inner_text()
            assert th10_panel.is_visible()
            assert "风神录没有独立的 Bomb 库存或 Bomb 道具" not in th10_text
            assert "Power 低于 1.00 时视为“无 Bomb”" not in th10_text
            assert page.locator("#mpGuideContent").evaluate("element => element.scrollTop") == 0
            page.locator('#mpGuideContent .multiplayer-rule-game-tab[data-game="th08"]').click()
            th08_panel = page.locator('#mpGuideContent .multiplayer-rule-panel[data-game="th08"]')
            th08_panel.locator('details[data-scope="specific"] summary').click()
            assert "每位玩家拥有独立的人妖率" in th08_panel.inner_text()
            assert "刻符池、夜晚时间与关卡推进为全队共享" in th08_panel.inner_text()

            assert page.locator("#mpGuideContent [onerror]").count() == 0
            assert page.locator("#mpGuideContent [style]").count() == 0
            assert page.locator("#mpGuideContent script").count() == 0
            backdrop = page.locator("#mpGuideDialog").evaluate(
                "element => ({ background: getComputedStyle(element, '::backdrop').backgroundColor, filter: getComputedStyle(element, '::backdrop').backdropFilter })"
            )
            assert backdrop["background"] in ("rgba(0, 0, 0, 0)", "transparent"), backdrop
            assert backdrop["filter"] in ("none", ""), backdrop
            assert not errors, errors
            print(json.dumps({"multiplayerGuide": "PASS", "gameTabs": 4, "viewport": [430, 820], "backdrop": backdrop}, ensure_ascii=False))
            browser.close()
        return 0
    finally:
        server.shutdown()
        server.server_close()
        thread.join(timeout=2)


if __name__ == "__main__":
    raise SystemExit(main())
