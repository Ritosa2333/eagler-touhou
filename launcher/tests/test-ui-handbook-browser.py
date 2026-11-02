"""Check production room specimens from a built handbook without Native or Relay."""
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
import json
from pathlib import Path
from threading import Thread

from playwright.sync_api import sync_playwright

ROOT = Path(__file__).resolve().parents[1]
SITE = ROOT / ".cache/build/ui-rewrite/client"
OUTPUT = ROOT.parent / "artifacts/validation/ui-handbook"
READ_ROSTER = """() => [...document.querySelectorAll('#mpSeatStage > .mp-seat')].map(card => {
  const style=getComputedStyle(card), box=card.getBoundingClientRect();
  const name=card.querySelector('.mp-seat-name'), menu=card.querySelector('.mp-seat-moderation:not([hidden]) button');
  const menuBox=menu?.getBoundingClientRect(), nameStyle=name&&getComputedStyle(name);
  return {hidden:card.hidden, display:style.display, tracks:style.gridTemplateRows.trim().split(/\\s+/).length,
    nameEnd:name&&name.getBoundingClientRect().right-parseFloat(nameStyle.paddingRight),
    menu:menuBox&&{start:menuBox.left, width:menuBox.width, height:menuBox.height},
    x:box.x+scrollX, y:box.y+scrollY, width:box.width, height:box.height};
})"""
SETTLE = """async () => {
  const finite=document.getElementById('mpSeatStage').getAnimations({subtree:true})
    .filter(animation => animation.effect.getComputedTiming().iterations!==Infinity);
  await Promise.allSettled(finite.map(animation => animation.finished));
  await new Promise(requestAnimationFrame);
}"""


class Handler(SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=str(SITE), **kwargs)

    def log_message(self, *_args):
        pass


def check_roster(rows, count):
    visible = [row for row in rows if not row["hidden"]]
    assert len(visible) == count, rows
    assert all(row["display"] == "none" and row["height"] == 0 for row in rows if row["hidden"]), rows
    assert all(row["tracks"] == 1 for row in visible), rows
    for row in visible:
        if row.get("menu"):
            assert row["nameEnd"] <= row["menu"]["start"] - 4, row
            assert row["menu"]["width"] >= 43.9 and row["menu"]["height"] >= 43.9, row
    return visible


def main():
    if not (SITE / "ui-review.html").is_file():
        raise RuntimeError("Build the frontend before running handbook checks")
    OUTPUT.mkdir(parents=True, exist_ok=True)
    server = ThreadingHTTPServer(("127.0.0.1", 0), Handler)
    worker = Thread(target=server.serve_forever, daemon=True)
    worker.start()
    base = f"http://127.0.0.1:{server.server_port}"
    cases, errors, sockets = [], [], []
    try:
        with sync_playwright() as runtime:
            browser = runtime.chromium.launch()
            page = browser.new_page()
            page.on("pageerror", lambda error: errors.append(str(error)))
            page.on("websocket", lambda connection: sockets.append(connection.url))
            try:
                for width, height in [(1280, 800), (390, 844), (320, 800), (844, 390)]:
                    page.set_viewport_size({"width": width, "height": height})
                    for theme in ["dark", "light"]:
                        page.goto(base + "/ui-review.html#room-components")
                        page.locator("header.ui-review-header button:not([disabled])").wait_for()
                        if page.locator("html").get_attribute("data-book-theme") != theme:
                            page.locator("header.ui-review-header").get_by_role("button", name="深色外观").click()
                        page.get_by_role("link", name="联机房间部件", exact=True).click()
                        for count in [2, 3]:
                            page.get_by_role("tab", name="房间设置", exact=True).click()
                            page.locator("#mpRoomPlayerCount-trigger").click()
                            page.get_by_role("option", name=f"{count}P", exact=True).click()
                            page.get_by_role("tab", name="快捷发言", exact=True).click()
                            baseline = None
                            for state in ["加载", "重连", "未准备", "已准备", "空席"]:
                                page.get_by_role("radiogroup", name="P2 示例状态").get_by_role("radio", name=state, exact=True).click()
                                page.evaluate(SETTLE)
                                rows = check_roster(page.evaluate(READ_ROSTER), count)
                                if baseline:
                                    for before, after in zip(baseline, rows):
                                        for axis in ["x", "y", "width", "height"]:
                                            assert abs(before[axis] - after[axis]) < 1, (state, axis, before, after)
                                else:
                                    baseline = rows
                                cases.append({"viewport": [width, height], "theme": theme, "players": count, "state": state, "rows": rows})
                assert not errors, errors
                assert not sockets, sockets
                assert page.locator("iframe").count() == 0
            except Exception:
                page.screenshot(path=str(OUTPUT / "failure.png"))
                raise
            finally:
                browser.close()
        print(f"PASS | {len(cases)} handbook roster states")
    finally:
        (OUTPUT / "result.json").write_text(json.dumps({"cases": cases, "errors": errors, "websockets": sockets}, ensure_ascii=False, indent=2), encoding="utf-8")
        server.shutdown()
        server.server_close()
        worker.join()


if __name__ == "__main__":
    main()
