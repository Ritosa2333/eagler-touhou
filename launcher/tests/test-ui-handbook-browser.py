"""Check production room specimens and populated directory fields without Native or Relay."""
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
import json
import re
from pathlib import Path
import subprocess
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
READ_DIRECTORY = """() => {
  const visible = node => {
    if (!node || !node.getClientRects().length) return false;
    for (let at=node; at; at=at.parentElement) {
      const style=getComputedStyle(at);
      if (at.hidden || style.display==='none' || style.visibility==='hidden' || +style.opacity===0) return false;
    }
    return true;
  };
  return [...document.querySelectorAll('.lobby-room-row')].map(row => {
    const box = node => {const r=node.getBoundingClientRect();return {x:r.x,y:r.y,width:r.width,height:r.height,right:r.right,bottom:r.bottom};};
    const read = selector => [...row.querySelectorAll(selector)].map(node => ({text:node.textContent.trim(), visible:visible(node)}));
    const names = [...row.querySelectorAll('.lobby-seat-name')].map(node => {
      const range=document.createRange(); range.selectNodeContents(node);
      const box=node.getBoundingClientRect();
      return {text:node.textContent, visible:visible(node), scrollable:getComputedStyle(node).overflowX==='auto' && node.tabIndex===0,
        clipped:[...range.getClientRects()].some(rect => rect.left<box.left-1 || rect.right>box.right+1 || rect.bottom>box.bottom+1)};
    });
    const button=row.querySelector('.lobby-join').getBoundingClientRect();
    return {key:row.dataset.key, names, controls:read('.lobby-seat-control'), watchers:read('.lobby-room-watchers'),
      count:row.querySelector('.lobby-party').getAttribute('aria-label'), state:read('.lobby-room-state'), rules:read('.lobby-room-rules'),
      elapsed:read('.room-elapsed'), score:read('.book-room-score'), overflow:row.scrollWidth>row.clientWidth+1,
      heading:row.querySelector('.lobby-work-name').textContent,
      cover:box(row.querySelector('.lobby-cover')), coverNumbers:row.querySelectorAll('.lobby-cover-number').length,
      difficulty:row.querySelector('.lobby-difficulty')?.parentElement.tagName,
      code:box(row.querySelector('.lobby-room-code')), rulesBox:box(row.querySelector('.lobby-room-rules')),
      divider:row.querySelector('.lobby-room-fact-divider')?.textContent,
      duel:row.querySelector('[data-duel="true"]') && {
        players:[...row.querySelectorAll('.lobby-public-player')].map(node => ({box:box(node),avatar:box(node.querySelector('.lobby-seat')),name:box(node.querySelector('.lobby-seat-name'))})),
        score:box(row.querySelector('.book-room-score')),
      },
      button:{width:button.width,height:button.height}};
  });
}"""


def check_directory(rows, rooms):
    assert len(rows) == len(rooms), rows
    for room in rooms:
        row = next(row for row in rows if row["key"] == room["product"] + "-" + room["code"])
        names = [seat["name"] if seat else "等待玩家" for seat in room["seats"]]
        assert [name["text"] for name in row["names"]] == names, row
        assert all(name["visible"] and (not name["clipped"] or name["scrollable"]) for name in row["names"]), row
        assert all(field["visible"] for key in ["controls", "watchers", "state"] for field in row[key]), row
        assert row["rules"][0]["visible"] == bool(room["disableCheatMovement"] or room["challengeMode"]), row
        assert len(row["controls"]) == sum(bool(seat) for seat in room["seats"]), row
        assert row["watchers"][0]["text"] == f'{room["spectators"]} 人旁观', row
        assert row["count"] == f'{room["players"]}/{room["capacity"]} 人', row
        assert " " not in row["heading"] and row["heading"].startswith("東方"), row
        assert abs(row["cover"]["width"] - row["cover"]["height"]) < 1 and row["coverNumbers"] == 0, row
        if row.get("difficulty"):
            assert row["difficulty"] == "H2", row
        if room["disableCheatMovement"] or room["challengeMode"]:
            assert row["rulesBox"]["x"] >= row["code"]["right"] or row["rulesBox"]["y"] >= row["code"]["bottom"] - 1, row
        assert bool(row["elapsed"]) == (room["phase"] == "playing"), row
        assert all(field["visible"] for field in row["elapsed"] + row["score"]), row
        if room["product"] == "th09mp":
            assert row["score"][0]["text"].endswith("3 : 5"), row
            assert re.search(r"(?:^|\D)0?8(?:[:：]00|\s*分)", row["elapsed"][0]["text"]), row
            left, right = row["duel"]["players"]
            score = row["duel"]["score"]
            assert abs(left["box"]["width"] - right["box"]["width"]) < 1, row
            assert left["name"]["right"] <= left["avatar"]["x"] < score["x"] < right["avatar"]["x"] < right["name"]["x"], row
        if row["elapsed"]:
            assert row["divider"] == "|", row
        assert not row["overflow"], row
        assert row["button"]["width"] >= 44 and row["button"]["height"] >= 43.9, row


def check_populated_directory(page, base, cases):
    fixture = json.loads(subprocess.check_output([
        "node", "--input-type=module", "-e",
        "import {createLobbyDirectoryFixture} from './tests/fixtures/lobby-directory.mjs'; console.log(JSON.stringify(createLobbyDirectoryFixture()));",
    ], cwd=ROOT, text=True, encoding="utf-8"))
    page.route("**/site-manifest.json*", lambda route: route.fulfill(json=fixture["manifest"]))

    def connect(socket):
        # The current directory protocol needs an initial snapshot before the
        # client requests a filtered refresh. Keep that handshake local to the
        # fixture so this gate never depends on a public lobby or relay.
        socket.send(json.dumps({
            "type": "directory", "version": 1, "product": "",
            "rooms": fixture["rooms"], "total": fixture["total"],
        }))

        def receive(message):
            request = json.loads(message)
            if request.get("type") == "refresh":
                product = request.get("product", "")
                rooms = [room for room in fixture["rooms"] if not product or room["product"] == product]
                socket.send(json.dumps({"type": "directory", "version": 1, "product": product, "rooms": rooms, "total": fixture["total"]}))
        socket.on_message(receive)

    page.route_web_socket("wss://directory.visual.invalid/netplay*", connect)
    session = page.context.new_cdp_session(page)
    for width, height in [(1280, 800), (390, 844), (320, 800), (844, 390)]:
        session.send('Emulation.setTouchEmulationEnabled', {'enabled': width <= 844, 'maxTouchPoints': 5})
        page.set_viewport_size({"width": width, "height": height})
        for theme in ["dark", "light"]:
            page.goto(base + "/lobby.html")
            page.locator(".lobby-room-row").first.wait_for()
            if page.locator("html").get_attribute("data-book-theme") != theme:
                page.locator(".book-theme-toggle").click()
            assert page.locator('.book-room-view-toggle').count() == 0
            assert page.locator('#listHead').count() == 0
            assert page.locator("#connectionNote").count() == 0
            rows = page.evaluate(READ_DIRECTORY)
            check_directory(rows, fixture["rooms"])
            assert_theme_surface(page, theme, '.lobby-list-surface')
            assert page.evaluate("document.documentElement.scrollWidth <= innerWidth"), (width, height, theme)
            header = page.evaluate("""() => {
                const menu=document.getElementById('mastheadMenuToggle').getBoundingClientRect();
                const bar=document.querySelector('.book-masthead').getBoundingClientRect();
                return {gap:bar.right-menu.right,width:menu.width,height:menu.height};
            }""")
            assert 0 <= header['gap'] <= 40, (width, height, header)
            if width <= 844:
                assert header['width'] >= 43.9 and header['height'] >= 43.9, header
            layout = page.evaluate("""() => {
                const box=node=>{const r=node.getBoundingClientRect();return {top:r.top,bottom:r.bottom,height:r.height,left:r.left,right:r.right};};
                const library=document.querySelector('.lobby-library');
                return {library:{width:library.clientWidth,scroll:library.scrollWidth},
                  business:[...document.querySelectorAll('#createButton,#watchReplayButton,#codeButton')].map(box),
                  favorites:[...document.querySelectorAll('.book-lobby-desktop [data-lobby-product]')].map(row=>({row:box(row),button:box(row.querySelector('.book-lobby-favorite'))})),
                  countInside:!!document.querySelector('.lobby-list-surface #roomCount'),
                  count:box(document.querySelector('#roomCount')),panel:box(document.querySelector('.lobby-list-surface')),
                  refreshBorder:getComputedStyle(document.querySelector('#refreshRooms')).borderTopStyle};
            }""")
            assert layout['library']['scroll'] <= layout['library']['width'] + 1, layout
            assert layout['countInside'], layout
            assert abs(layout['count']['bottom'] - layout['panel']['bottom']) <= 2, layout
            assert layout['refreshBorder'] == 'solid', layout
            if width <= 844:
                assert max(box['top'] for box in layout['business']) - min(box['top'] for box in layout['business']) < 1, layout
                assert max(box['height'] for box in layout['business']) - min(box['height'] for box in layout['business']) < 1, layout
            else:
                assert all(item['button']['right'] <= item['row']['right'] + 1 for item in layout['favorites']), layout
            cases.append({"viewport": [width, height], "theme": theme, "rows": rows})
            page.screenshot(path=str(OUTPUT / f"directory-{width}x{height}-{theme}.png"), full_page=True)
    page.set_viewport_size({"width": 390, "height": 844})
    page.goto(base + '/lobby.html')
    th06_filter = page.locator('.book-lobby-mobile [data-lobby-toggle="th06mp"]')
    th06_filter.click()
    page.wait_for_function("document.querySelector('.book-lobby-mobile [data-lobby-toggle=\"th06mp\"]')?.getAttribute('aria-checked') === 'true'")
    th09_filter = page.locator('.book-lobby-mobile [data-lobby-toggle="th09mp"]')
    th09_filter.click()
    page.wait_for_function("document.querySelector('.book-lobby-mobile [data-lobby-toggle=\"th09mp\"]')?.getAttribute('aria-checked') === 'true'")
    page.wait_for_function("document.querySelectorAll('.lobby-room-row').length===2")
    page.locator('.book-lobby-picker-trigger').click()
    popup = page.get_by_role('menu', name='联机作品')
    row_heights = popup.locator('.book-lobby-menu-row').evaluate_all('(nodes)=>nodes.map(node=>node.getBoundingClientRect().height)')
    assert max(row_heights) - min(row_heights) < 1, row_heights
    popup.get_by_role('menuitemcheckbox', name='筛选 東方風神録', exact=True).click()
    assert popup.is_visible(), 'Multiple selection must not close the work list'
    page.wait_for_function("document.querySelectorAll('.lobby-room-row').length===3")
    assert page.locator('.book-lobby-mobile [aria-checked="true"]').count() == 3
    assert page.locator('.book-lobby-desktop [role="checkbox"][aria-checked="true"]').count() == 3
    anchor, panel = page.locator('.book-lobby-picker-trigger').bounding_box(), popup.bounding_box()
    assert panel['y'] >= anchor['y'] + anchor['height'] - 1
    assert panel['x'] >= 0 and panel['x'] + panel['width'] <= 390
    popup.press('Escape')
    page.locator('.book-lobby-mobile .book-lobby-all').click()
    page.wait_for_function("document.querySelectorAll('.lobby-room-row').length===5")
    page.locator('.book-lobby-mobile [data-lobby-icon="th07mp"]').click()
    page.locator('#emptyState').wait_for()
    empty = page.evaluate("""() => {
        const rect=node=>{const r=node.getBoundingClientRect();return {top:r.top,bottom:r.bottom};};
        return {listHidden:document.getElementById('roomList').hidden,
          panel:rect(document.querySelector('.lobby-list-surface')),empty:rect(document.getElementById('emptyState')),
          countHidden:document.getElementById('roomCount').hidden,heading:!!document.querySelector('.book-lobby-filter-heading')};
    }""")
    assert empty['listHidden'] and empty['countHidden'] and not empty['heading'], empty
    assert abs(empty['empty']['top'] - empty['panel']['top']) <= 2, empty
    assert abs(empty['empty']['bottom'] - empty['panel']['bottom']) <= 2, empty
    page.locator('#emptyAction').click()
    page.wait_for_function("document.querySelectorAll('.lobby-room-row').length===5")
    session.send('Emulation.setTouchEmulationEnabled', {'enabled': False})
    session.detach()

    # Reuse the real image error handler. CSS must expose its fallback after failure.
    cover = page.locator('.lobby-room-row[data-key="th09mp-4109"] .lobby-cover img')
    source = cover.get_attribute("src")
    page.route("**/" + source.rsplit("/", 1)[-1] + "*", lambda route: route.abort())
    cover.evaluate("img => {const url = new URL(img.src); url.searchParams.set('fixture-error', String(Date.now())); img.src = url.href;}")
    page.locator('.lobby-room-row[data-key="th09mp-4109"] .lobby-cover img[hidden]').wait_for(state="attached")
    fallback = page.locator('.lobby-room-row[data-key="th09mp-4109"] .lobby-cover-number')
    assert fallback.is_visible() and fallback.inner_text() == "09"


class Handler(SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=str(SITE), **kwargs)

    def log_message(self, *_args):
        pass

    def translate_path(self, path):
        target = Path(super().translate_path(path))
        if target.suffix == '.html' and target.is_dir():
            return str(target / 'index.html')
        return str(target)


def assert_theme_surface(page, theme, selector):
    colors = page.locator(selector).evaluate("""node => {
        const css=getComputedStyle(node), root=document.documentElement;
        return {theme:root.dataset.bookTheme, actual:css.backgroundColor, expected:css.getPropertyValue('--kit-card').trim(),
          lock:!!document.querySelector('meta[name="darkreader-lock"]')};
    }""")
    value = colors['expected'].lstrip('#')
    if len(value) == 3:
        value = ''.join(char * 2 for char in value)
    expected = 'rgb(' + ', '.join(str(int(value[index:index+2], 16)) for index in [0, 2, 4]) + ')'
    assert colors['theme'] == theme and colors['lock'] and colors['actual'] == expected, colors


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


def check_page_anatomy(page, base):
    page.goto(base + '/ui-review.html#modules')
    page.locator('.catalog-page-inventory > details').first.wait_for()
    pages = page.locator('.catalog-page-inventory > details').evaluate_all('(nodes) => nodes.map(node => node.id)')
    assert len(pages) == 15, pages
    for page_id in pages:
        page.goto(base + '/ui-review.html#' + page_id)
        opened = page.locator('#' + page_id + '[open]')
        opened.locator('.catalog-page-region').first.wait_for()
        targets = opened.locator('.catalog-part-states a').evaluate_all('(nodes) => nodes.map(node => new URL(node.href).hash.slice(1))')
        assert targets, page_id
        assert page.evaluate('(ids) => ids.every(id => document.getElementById(id))', targets), page_id
        target = targets[-1]
        opened.locator(f'a[href$="#{target}"]').click()
        page.wait_for_function('(id) => {const rect=document.getElementById(id)?.getBoundingClientRect();return rect && rect.top<innerHeight && rect.bottom>=0;}', arg=target)
        assert page.url.endswith('#' + target), (page_id, page.url)
        page.go_back()
        page.wait_for_function('(id) => {const node=document.getElementById(id);return location.hash==="#"+id && node?.open;}', arg=page_id)
        page.go_forward()
        page.wait_for_function('(id) => {const rect=document.getElementById(id)?.getBoundingClientRect();return location.hash==="#"+id && rect && rect.top<innerHeight && rect.bottom>=0;}', arg=target)
        ids = page.locator('[id]').evaluate_all('(nodes) => nodes.map(node => node.id)')
        assert len(ids) == len(set(ids)), (page_id, 'duplicate IDs')
        assert page.locator('iframe').count() == 0


def main():
    if not any(path.is_file() for path in [SITE / "ui-review.html", SITE / "ui-review.html/index.html"]):
        raise RuntimeError("Build the frontend before running handbook checks")
    OUTPUT.mkdir(parents=True, exist_ok=True)
    server = ThreadingHTTPServer(("127.0.0.1", 0), Handler)
    worker = Thread(target=server.serve_forever, daemon=True)
    worker.start()
    base = f"http://127.0.0.1:{server.server_port}"
    cases, directory_cases, errors, sockets = [], [], [], []
    try:
        with sync_playwright() as runtime:
            browser = runtime.chromium.launch()
            context = browser.new_context(service_workers="block")
            page = context.new_page()
            page.on("pageerror", lambda error: errors.append(str(error)))
            page.on("websocket", lambda connection: sockets.append(connection.url))
            try:
                for width, height in [(1280, 800), (390, 844), (320, 800), (844, 390)]:
                    page.set_viewport_size({"width": width, "height": height})
                    for theme in ["dark", "light"]:
                        page.goto(base + "/ui-review.html#page-room")
                        page.locator("header.ui-review-header button:not([disabled])").wait_for()
                        if page.locator("html").get_attribute("data-book-theme") != theme:
                            page.locator("header.ui-review-header").get_by_role("button", name="深色外观").click()
                        page.locator('#page-room[open] #mpSeatStage').wait_for()
                        for count in [2, 3]:
                            page.get_by_role('radiogroup', name='房间示例人数').get_by_role('radio', name=f'{count}P', exact=True).click()
                            baseline = None
                            for state in ["加载", "失败", "重连", "未准备", "已准备", "空席"]:
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
                check_page_anatomy(page, base)
                check_populated_directory(page, base, directory_cases)
                assert not errors, errors
                assert not page.locator("#player").evaluate("node => node.classList.contains('open')")
                assert page.locator("iframe[src]:not([src='about:blank'])").count() == 0
            except Exception:
                page.screenshot(path=str(OUTPUT / "failure.png"))
                raise
            finally:
                browser.close()
        print(f"PASS | {len(cases)} handbook roster states | {len(directory_cases)} populated directory views")
    finally:
        (OUTPUT / "result.json").write_text(json.dumps({"cases": cases, "directoryCases": directory_cases, "errors": errors, "websockets": sockets}, ensure_ascii=False, indent=2), encoding="utf-8")
        server.shutdown()
        server.server_close()
        worker.join()


if __name__ == "__main__":
    main()
