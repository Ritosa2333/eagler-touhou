"""Real-runtime TH09MP launch gate.

The mixed-entry test stubs `th09.html` so it can prove the Launcher room
plumbing cheaply and deterministically. This test instead boots the real TH09
Runtime twice from the Launcher's TH09MP card and requires the actual WASM
bridge, `shared-netplay.mjs` handshake, shared relay and TH09 versus match to
run: both peers must report an active LAN session with advancing frames.

Preconditions: the TH09 DATA/layout content directory (or
`EAGLER_TH09_CONTENT_DIR`) plus the shared vanilla/Unicode fonts, or an
`eagler-touhou/package/1` offline ZIP passed as `--package-zip=PATH`. Fonts are
otherwise located from `EAGLER_DEVELOPMENT_VANILLA_FONT`/
`EAGLER_DEVELOPMENT_UNICODE_FONT` or an assembled `prepared/*/shared/` site.
`--music=none` runs the muted path (no OGG transferred) instead of OGG,
`--host-entry=title` hosts from TH09's own game-title versus entry instead of
the Launcher card, and `--touch-check` drags the Runtime's own touch surface
inside the live match and requires the player to settle on the finger.
`--spectator-check` admits a third browser to the spectator rail before start
and requires it to replay the same confirmed TH09 input frames.
"""
from __future__ import annotations

import os
import shutil
import socket
import subprocess
import sys
import time
import uuid
import zipfile
from pathlib import Path

from playwright.sync_api import sync_playwright, TimeoutError as PlaywrightTimeoutError


PROJECT = Path(__file__).resolve().parents[1]
WORKSPACE = PROJECT.parent
FRAME_TARGET = 120


def free_port() -> int:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.bind(("127.0.0.1", 0))
        return int(sock.getsockname()[1])


def wait_http(url: str) -> None:
    import urllib.request
    deadline = time.time() + 30
    while time.time() < deadline:
        try:
            with urllib.request.urlopen(url, timeout=.5) as response:
                if response.status < 400:
                    return
        except Exception:
            time.sleep(.1)
    raise RuntimeError("Launcher HTTP server did not start")


def extract_package(path: Path, scratch: Path) -> tuple[Path, Path | None, Path | None]:
    """Extract TH09 DATA + shared fonts from an offline package ZIP.

    The offline packages carry `games/th09/th09.data`, `games/th09/music/ogg/*`
    and `shared/{msgothic.ttc,unifont.otf}`, so an extracted package is a
    complete content root for this gate on a machine without the loose
    workspace content directory.
    """
    if not path.is_file():
        raise SystemExit(f"package zip not found: {path}")
    with zipfile.ZipFile(path) as archive:
        names = set(archive.namelist())
        if "games/th09/th09.data" not in names:
            raise SystemExit(f"{path} is not a TH09 offline package (no games/th09/th09.data)")
        members = [name for name in names
                   if name.startswith("games/th09/") or name.startswith("shared/")]
        archive.extractall(scratch, members=members)
    content = scratch / "games" / "th09"
    # Packages store tracks as games/th09/music/ogg/*; the development content
    # declaration mounts them flat from <content>/music.
    nested = content / "music" / "ogg"
    if nested.is_dir():
        for track in nested.glob("*.ogg"):
            track.replace(content / "music" / track.name)
        nested.rmdir()
    vanilla = scratch / "shared" / "msgothic.ttc"
    unicode_font = scratch / "shared" / "unifont.otf"
    return content, (vanilla if vanilla.is_file() else None), (unicode_font if unicode_font.is_file() else None)


def shared_font(name: str, variable: str) -> Path | None:
    override = os.environ.get(variable)
    if override:
        path = Path(override)
        return path if path.is_file() else None
    prepared = WORKSPACE / "prepared"
    if prepared.is_dir():
        for candidate in sorted(prepared.glob(f"*/shared/{name}")):
            if candidate.is_file():
                return candidate
    return None


def confirm_decisions(page, timeout: float = 30.0) -> int:
    """Answer the music/touch confirmations a real Runtime launch raises."""
    selector = "#decisionDialog[open]:not(.closing)"
    accepted = 0
    deadline = time.time() + timeout
    while time.time() < deadline:
        dialog = page.locator(selector)
        if not dialog.count():
            if accepted:
                return accepted
            page.wait_for_timeout(150)
            continue
        try:
            dialog.locator("#decisionConfirm").click(timeout=1500)
        except PlaywrightTimeoutError:
            if not page.locator(selector).count():
                accepted += 1
                continue
            raise
        accepted += 1
    return accepted


def netplay_state(page) -> dict | None:
    for frame in page.frames:
        try:
            state = frame.evaluate("""() => {
              const core = globalThis.__th09Runtime?.core;
              const options = core?.eaglerOptions || globalThis.Module?.eaglerOptions || null;
              return {
                active: globalThis.__eaglerNetplayLanActive === true,
                frame: Number(globalThis.__eaglerNetplayLanFrame ?? -1),
                error: String(document.querySelector('#error')?.textContent || ''),
                mode: options?.netplayMode ?? null,
                player: Number(options?.netplayPlayer ?? -1),
                url: String(options?.netplayUrl || ''),
              };
            }""")
        except Exception:
            continue
        if isinstance(state, dict) and (state.get("mode") == "lan" or state.get("active")):
            return state
    return None


def runtime_hash_capture(page, start: bool) -> dict | bool:
    for frame in page.frames:
        try:
            value = frame.evaluate("""start => {
              const runtime = globalThis.__th09Runtime;
              if (!runtime?.core?._th09_network_hash) return null;
              if (start) {
                const hashes = globalThis.__th09SpectatorTestHashes = {};
                setInterval(() => {
                  const frame = Number(globalThis.__eaglerNetplayLanFrame ?? -1);
                  if (frame > 0 && hashes[frame] === undefined)
                    hashes[frame] = runtime.core._th09_network_hash() >>> 0;
                }, 4);
                return true;
              }
              return globalThis.__th09SpectatorTestHashes || {};
            }""", start)
            if value is not None:
                return value
        except Exception:
            continue
    raise RuntimeError("TH09 Runtime frame not found for state hash capture")


def title_status(page) -> list | None:
    """Read TH09's title state: [frames, in_title, screen, state, selection, ...]."""
    for frame in page.frames:
        try:
            values = frame.evaluate("""() => {
              const core = globalThis.__th09Runtime?.core;
              if (!core) return null;
              const at = core._th09_title_status() / 4;
              return Array.from(core.HEAP32.subarray(at, at + 8));
            }""")
        except Exception:
            continue
        if isinstance(values, list) and len(values) == 8:
            return values
    return None


def wait_title(page, predicate, timeout: float, what: str) -> list:
    deadline = time.time() + timeout
    last = None
    while time.time() < deadline:
        last = title_status(page)
        if last and predicate(last):
            return last
        page.wait_for_timeout(200)
    raise RuntimeError(f"TH09 title never reached {what}: last={last}")


def tap_key(page, key: str, hold_ms: int = 50) -> None:
    """Press a game key long enough for the Runtime's frame-sampled input.

    The Launcher forwards keydown/keyup to the Runtime; TH09 samples held keys
    once per frame, so an instantaneous synthetic press would be missed. The
    Runtime document owns focus while a game runs, so the press is handled there.
    """
    page.keyboard.down(key)
    page.wait_for_timeout(hold_ms)
    page.keyboard.up(key)
    page.wait_for_timeout(140)


def move_selection_to(page, target: int, timeout: float = 20.0) -> list:
    """Walk a title menu to `target`, correcting for held-key auto-repeat."""
    deadline = time.time() + timeout
    last = None
    while time.time() < deadline:
        last = title_status(page)
        if last[4] == target:
            return last
        tap_key(page, "ArrowUp" if last[4] > target else "ArrowDown")
    raise RuntimeError(f"TH09 title menu never reached selection {target}: {last}")


def title_network_entry(page, music_modes: list) -> str:
    """Boot the real normal Runtime and use its own versus entry.

    TH09's main menu item "Match Start" opens the versus-type screen; its fifth
    option is the network match, which calls `title_network()` ->
    `Module.onNetworkRequest` -> the Launcher room dialog. This proves the
    in-game entry against the real Runtime instead of a stub event.
    """
    page.locator('.game[data-game="th09"]:not([data-product])').click()
    music_modes.append(select_room_music(page, None, "#musicSelect"))
    page.locator("#launch").click()
    deadline = time.time() + 60
    while time.time() < deadline and not page.locator("#player").evaluate(
            "element => element.classList.contains('open')"):
        confirm_decisions(page, timeout=.5)
    page.wait_for_selector("#player.open", timeout=30_000)

    # The attract loop starts itself after ~1500 idle frames; any key returns to
    # the title, so never assume the first key press reaches the menu.
    deadline = time.time() + 120
    while time.time() < deadline:
        state = title_status(page)
        if state and state[1] == 1 and state[2] == 1 and state[3] == 1:
            break
        tap_key(page, "KeyZ")
    else:
        raise RuntimeError(f"TH09 title never reached the main menu: {title_status(page)}")

    move_selection_to(page, 2)  # "Match Start"
    tap_key(page, "KeyZ")
    wait_title(page, lambda state: state[2] == 6, 30, "the versus-type screen")
    move_selection_to(page, 4)  # network match
    tap_key(page, "KeyZ")

    page.wait_for_selector("#th09NetworkDialog:not([hidden])", timeout=30_000)
    page.locator("#th09NetworkCreate").click()
    page.wait_for_selector("#th09NetworkRoom:not([hidden]) #mpRoomView:not([hidden])", timeout=10_000)
    return page.locator("#mpRoomCode").inner_text().strip()


def select_room_music(page, requested: str | None = None, selector: str = "#mpMusicSelect") -> str:
    """Select the room's music mode; default prefers real OGG playback.

    TH09 mounts music only in OGG modes, so the muted "none" mode must also
    start a match: the Runtime may not fail merely because the host transferred
    no track. `--music=none` exercises that path deliberately.
    """
    select = page.locator(selector)
    values = select.evaluate("element => [...element.options].map(option => option.value)")
    if requested:
        assert requested in values, f"music mode {requested} is unavailable: {values}"
        mode = requested
    else:
        mode = "ogg-stream" if "ogg-stream" in values else "none" if "none" in values else values[0]
    select.select_option(mode, force=True)
    return mode


def touch_probe(page) -> list | None:
    """Local side touch diagnostics: position, velocity and applied motion sample."""
    for frame in page.frames:
        try:
            values = frame.evaluate("""() => {
              const core = globalThis.__th09Runtime?.core;
              if (!core || typeof core._th09_touch_probe !== 'function') return null;
              const at = core._th09_touch_probe() / 4;
              const words = core.HEAPU32.subarray(at, at + 8);
              const buffer = new ArrayBuffer(4), view = new DataView(buffer);
              return Array.from(words, word => { view.setUint32(0, word, true); return view.getFloat32(0, true); });
            }""")
        except Exception:
            continue
        if isinstance(values, list) and len(values) == 8:
            return values
    return None


def point_touch(page, touch_type: int, x: float, y: float) -> None:
    """Deliver one pointer sample to the Runtime's own touch surface."""
    for frame in page.frames:
        try:
            handled = frame.evaluate("""(args) => {
              const core = globalThis.__th09Runtime?.core;
              if (!core) return false;
              core._th09_touch(args.type, 1, args.x, args.y);
              return true;
            }""", {"type": touch_type, "x": x, "y": y})
        except Exception:
            continue
        if handled:
            return
    raise RuntimeError(f"TH09 Runtime is not running; launcher state: {launch_state(page)}")


def assert_touch_converges(page, frames: int = 120) -> dict:
    """Drag the Runtime's own touch surface and require it to reach the finger.

    A networked gesture travels with the lockstep input delay. When it is shipped
    as a velocity sampled from the sender's position, the player aims from a
    position `lead` frames in the future and orbits the finger forever; the
    shipped absolute target must settle exactly on it instead.
    """
    for frame in page.frames:
        try:
            frame.evaluate("() => globalThis.__th09Runtime?.core?._th09_touch_options(1, 0, 1, 0, 0)")
        except Exception:
            continue
    start = touch_probe(page)
    assert start, f"touch probe never reported a position; launcher state: {launch_state(page)}"
    start_x, start_y = start[0], start[1]
    # Touch coordinates are normalised over the Runtime canvas; the shared touch
    # controller maps a normalised delta to 640x480 field units.
    drag_x, drag_y = 0.2, 0.1
    point_touch(page, 0, 0.5, 0.5)
    page.wait_for_timeout(120)
    point_touch(page, 1, 0.5 + drag_x, 0.5 + drag_y)
    trail = []
    for _ in range(frames):
        page.wait_for_timeout(16)
        state = touch_probe(page)
        if state:
            trail.append((state[0], state[1]))
    settled = touch_probe(page)
    point_touch(page, 2, 0.5 + drag_x, 0.5 + drag_y)
    assert trail, "touch probe never reported a position while dragging"
    moved = ((trail[-1][0] - start_x) ** 2 + (trail[-1][1] - start_y) ** 2) ** 0.5
    # A converged gesture stops moving; an orbiting one keeps drawing a circle.
    tail = trail[len(trail) * 2 // 3:]
    path = sum(((tail[i + 1][0] - tail[i][0]) ** 2 + (tail[i + 1][1] - tail[i][1]) ** 2) ** 0.5
               for i in range(len(tail) - 1))
    assert moved > 60, (f"touch drag barely moved the player: moved={moved:.1f} "
                        f"start=({start_x:.1f},{start_y:.1f}) first={trail[:3]}")
    assert settled and settled[5] == 1, f"networked touch must ship an absolute target, got {settled}"
    distance = ((trail[-1][0] - settled[6]) ** 2 + (trail[-1][1] - settled[7]) ** 2) ** 0.5
    assert distance < 10, (
        f"touch drag did not reach the finger: start=({start_x:.1f},{start_y:.1f}) "
        f"target=({settled[6]:.1f},{settled[7]:.1f}) end=({trail[-1][0]:.1f},{trail[-1][1]:.1f}) "
        f"distance={distance:.1f} tail_path={path:.1f} samples={len(trail)}")
    assert path < 12, f"touch drag kept circling after reaching the finger: tail_path={path:.1f} tail={tail}"
    return {"start": (round(start_x, 1), round(start_y, 1)),
            "target": (round(settled[6], 1), round(settled[7], 1)),
            "end": (round(trail[-1][0], 1), round(trail[-1][1], 1)),
            "distance": round(distance, 2), "tail_path": round(path, 2)}


def launch_state(page) -> dict:
    return page.evaluate("""() => ({
      src: document.querySelector('#gameFrame')?.getAttribute('src') || '',
      playerOpen: document.querySelector('#player')?.classList.contains('open') === true,
      status: document.querySelector('#playerStatus')?.textContent || '',
      toast: document.querySelector('#toastText')?.textContent || '',
      pending: document.querySelector('#decisionDialog[open]')?.innerText || '',
      body: document.body.innerText.slice(-800),
    })""")


def wait_netplay(pages, target: int = FRAME_TARGET, timeout: float = 180.0) -> list[dict]:
    deadline = time.time() + timeout
    last: list[dict | None] = [None, None]
    while time.time() < deadline:
        for page in pages:
            confirm_decisions(page, timeout=.2)
        last = [netplay_state(page) for page in pages]
        if all(state and state["active"] and state["frame"] >= target for state in last):
            return [state for state in last if state]
        time.sleep(.25)
    diagnostics = [launch_state(page) for page in pages]
    raise RuntimeError(f"TH09 netplay did not reach frame {target}: last={last} diagnostics={diagnostics}")


def main() -> None:
    requested_music = next((arg.split("=", 1)[1] for arg in sys.argv[1:] if arg.startswith("--music=")), None)
    package_arg = next((arg.split("=", 1)[1] for arg in sys.argv[1:] if arg.startswith("--package-zip=")), None)
    host_entry = next((arg.split("=", 1)[1] for arg in sys.argv[1:] if arg.startswith("--host-entry=")), "card")
    touch_check = "--touch-check" in sys.argv[1:]
    spectator_check = "--spectator-check" in sys.argv[1:]
    unknown = [arg for arg in sys.argv[1:]
               if not arg.startswith(("--music=", "--package-zip=", "--host-entry="))
               and arg not in ("--touch-check", "--spectator-check")]
    if unknown or host_entry not in ("card", "title"):
        raise SystemExit("usage: test-th09mp-launch.py [--music=ogg-stream|none] [--package-zip=PATH] "
                         f"[--host-entry=card|title] [--touch-check] [--spectator-check] (unexpected: {unknown})")

    scratch = None
    if package_arg:
        # The development server only serves paths under the workspace root, and
        # the Host Manifest carries a workspace-relative DATA source, so an
        # extracted package must live inside the workspace (cleaned up below).
        scratch = WORKSPACE / "prepared" / f"th09mp-package-{uuid.uuid4().hex[:8]}"
        scratch.mkdir(parents=True)
        content, vanilla, unicode_font = extract_package(Path(package_arg), scratch)
    else:
        content = Path(os.environ.get("EAGLER_TH09_CONTENT_DIR") or WORKSPACE / "games" / "th09")
        vanilla = shared_font("msgothic.ttc", "EAGLER_DEVELOPMENT_VANILLA_FONT")
        unicode_font = shared_font("unifont.otf", "EAGLER_DEVELOPMENT_UNICODE_FONT")
    if not (content / "th09.data").is_file():
        raise SystemExit(f"TH09 DATA not found under {content}; set EAGLER_TH09_CONTENT_DIR or --package-zip")
    missing = [name for name, path in (("msgothic.ttc", vanilla), ("unifont.otf", unicode_font)) if path is None]
    if missing:
        raise SystemExit(
            "TH09MP launch needs the shared Runtime fonts "
            f"({', '.join(missing)}); set EAGLER_DEVELOPMENT_VANILLA_FONT / "
            "EAGLER_DEVELOPMENT_UNICODE_FONT, pass --package-zip, or assemble a site under prepared/*/shared"
        )

    http_port, relay_port = free_port(), free_port()
    while relay_port == http_port:
        relay_port = free_port()
    url = f"http://127.0.0.1:{http_port}/"
    relay_url = f"ws://127.0.0.1:{relay_port}/"
    http_env = os.environ.copy()
    http_env.update({
        "EAGLER_DEVELOPMENT_GAMES": "th09",
        "EAGLER_TH09_CONTENT_DIR": str(content),
        "EAGLER_TOUHOU_NETPLAY_RELAY": relay_url,
        "EAGLER_DEVELOPMENT_VANILLA_FONT": str(vanilla),
        "EAGLER_DEVELOPMENT_UNICODE_FONT": str(unicode_font),
    })
    http = subprocess.Popen(["node", "scripts/serve.mjs", str(http_port)], cwd=PROJECT,
                            env=http_env, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    env = os.environ.copy()
    env.update({"TH07_RELAY_HOST": "127.0.0.1", "TH07_RELAY_PORT": str(relay_port),
                "TH07_STUN_URLS": "", "TH07_RTC_TIMEOUT_MS": "1000"})
    relay = subprocess.Popen(["node", "server/netplay-relay.mjs"], cwd=PROJECT, env=env,
                             stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    try:
        wait_http(url)
        with sync_playwright() as playwright:
            browser = playwright.chromium.launch(headless=True)
            contexts = [browser.new_context(viewport={"width": 1280, "height": 900}, service_workers="block")
                        for _ in range(3 if spectator_check else 2)]
            try:
                errors: list[str] = []
                pages = []
                for index, context in enumerate(contexts):
                    page = context.new_page()
                    page.on("pageerror", lambda error, index=index: errors.append(f"P{index}: {error}"))
                    page.goto(url, wait_until="load", timeout=30_000)
                    page.wait_for_function("window.__eaglerBoot?.done === true", timeout=30_000)
                    notice = page.locator("#firstUseNoticeDialog")
                    if notice.count() and notice.evaluate("dialog => dialog.open"):
                        page.locator("#firstUseNoticeClose").click()
                    pages.append(page)

                host, guest = pages[:2]
                viewer = pages[2] if spectator_check else None
                music_modes = []
                if host_entry == "title":
                    # Host through TH09's own "妖怪対妖怪" entry, guest through the card.
                    code = title_network_entry(host, music_modes)
                    rail = host.locator("#mpSpectatorRail")
                    assert rail.evaluate("element => element.parentElement?.id") == "th09NetworkDialog"
                    before = rail.bounding_box()
                    head = rail.locator(".mp-spectator-rail-head").bounding_box()
                    assert before and head
                    host.mouse.move(head["x"] + head["width"] / 2, head["y"] + head["height"] / 2)
                    host.mouse.down()
                    host.mouse.move(head["x"] + head["width"] / 2 - 80,
                                    head["y"] + head["height"] / 2 + 35, steps=6)
                    host.mouse.up()
                    after = rail.bounding_box()
                    assert after and abs(after["x"] - before["x"]) > 40, (before, after)
                    # The in-game room is a viewport portal on phones too. A
                    # real touch must move its visible rail and its hit target
                    # together, including after touchEnd releases capture.
                    host.evaluate("async () => { if (document.fullscreenElement) await document.exitFullscreen(); }")
                    host.set_viewport_size({"width": 430, "height": 900})
                    mobile_before = rail.bounding_box()
                    head = rail.locator(".mp-spectator-rail-head").bounding_box()
                    assert mobile_before and head
                    start_x = head["x"] + head["width"] / 2
                    start_y = head["y"] + head["height"] / 2
                    cdp = host.context.new_cdp_session(host)
                    cdp.send("Input.dispatchTouchEvent", {"type": "touchStart", "touchPoints": [{"x": start_x, "y": start_y}]})
                    cdp.send("Input.dispatchTouchEvent", {"type": "touchMove", "touchPoints": [{"x": start_x - 75, "y": start_y - 95}]})
                    cdp.send("Input.dispatchTouchEvent", {"type": "touchEnd", "touchPoints": []})
                    mobile_after = rail.bounding_box()
                    assert mobile_after and abs(mobile_after["x"] - mobile_before["x"]) > 35, (mobile_before, mobile_after)
                    hit = host.evaluate("""() => {
                      const rail = document.querySelector('#mpSpectatorRail');
                      const rect = rail.getBoundingClientRect();
                      return rail.contains(document.elementFromPoint(rect.left + rect.width / 2, rect.top + 10));
                    }""")
                    assert hit, (mobile_before, mobile_after)
                    host.set_viewport_size({"width": 1280, "height": 900})
                    guest.locator('[data-product="th09mp"]').click()
                    music_modes.append(select_room_music(guest, requested_music))
                else:
                    for page in pages:
                        page.locator('[data-product="th09mp"]').click()
                        music_modes.append(select_room_music(page, requested_music))
                    host.locator("#mpCreateRoom").click()
                    host.wait_for_selector("#mpRoomView:not([hidden])", timeout=10_000)
                    code = host.locator("#mpRoomCode").inner_text().strip()
                assert len(code) == 4 and code.isdigit(), code

                guest.locator("#mpJoinCode").fill(code)
                guest.locator("#mpJoinRoom").click()
                guest.wait_for_selector('[data-mp-seat-drop="1"] button:not([disabled])', timeout=10_000)
                guest.locator('[data-mp-seat-drop="1"] button').click()
                if viewer:
                    viewer.locator('[data-product="th09mp"]').click()
                    viewer.locator("#mpJoinCode").fill(code)
                    viewer.locator("#mpJoinRoom").click()
                    viewer.wait_for_selector("#mpSpectatorJoin:not([hidden])", timeout=10_000)
                    viewer.locator("#mpSpectatorJoin").click()
                    viewer.wait_for_function("() => Number(document.querySelector('#mpSpectatorCount')?.textContent) >= 1", timeout=10_000)
                for page in pages[:2]:
                    page.wait_for_function(
                        "() => [...document.querySelectorAll('[data-mp-seat]')].slice(0, 2)"
                        ".every(seat => seat.classList.contains('occupied'))", timeout=10_000)
                for page in pages[:2]:
                    page.locator("#mpReady").click()
                host.wait_for_function("!document.querySelector('#mpStartGame').disabled", timeout=10_000)
                host.locator("#mpStartGame").click()

                for page in pages:
                    deadline = time.time() + 60
                    while time.time() < deadline and not page.locator("#player").evaluate(
                            "element => element.classList.contains('open')"):
                        confirm_decisions(page, timeout=.5)
                    page.wait_for_selector("#player.open", timeout=30_000)

                states = wait_netplay(pages)
                assert sorted(state["player"] for state in states[:2]) == [0, 1], states
                if viewer:
                    assert states[2]["frame"] >= FRAME_TARGET, states
                    assert "spectator=" in states[2]["url"], states
                    for page in pages:
                        runtime_hash_capture(page, True)
                    time.sleep(3)
                    hashes = [runtime_hash_capture(page, False) for page in pages]
                    common = set(hashes[0]).intersection(hashes[1], hashes[2])
                    assert len(common) >= 15, [len(item) for item in hashes]
                    mismatched = [frame for frame in common if len({item[frame] for item in hashes}) != 1]
                    assert not mismatched, {frame: [item[frame] for item in hashes] for frame in mismatched[:5]}
                assert all(f"room=th09mp-{code}" in state["url"] for state in states), states
                touch = assert_touch_converges(host) if touch_check else None
                assert not errors, errors
                print(f"TH09MP real Runtime launch: PASS room={code} host-entry={host_entry} "
                      f"music={sorted(set(music_modes))} "
                      f"frames={[state['frame'] for state in states]} players={[state['player'] for state in states]}"
                      f" spectator={bool(viewer)}"
                      f"{f' touch={touch}' if touch else ''}")
            finally:
                for context in contexts:
                    context.close()
                browser.close()
    finally:
        relay.terminate()
        http.terminate()
        for process in (relay, http):
            try:
                process.wait(timeout=5)
            except subprocess.TimeoutExpired:
                process.kill()
                process.wait(timeout=5)
        if scratch is not None:
            shutil.rmtree(scratch, ignore_errors=True)


if __name__ == "__main__":
    main()
