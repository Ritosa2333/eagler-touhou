"""TH20 mobile touch gate: free-direction movement and menu gestures.

Runs the Launcher in a touch-emulated mobile context, enables the Launcher's own
touch option and drives the Runtime through the Launcher's direct-touch surface
with real touch events. It proves:

  1. a drag at a non-axis angle moves the player along that angle, in the
     direction of the finger (the recovered eight-way input would snap it to
     45-degree steps, which is what "stiff" touch movement looked like);
  2. the pause menu reports the shared controller's menu context, so a tap
     confirms/resumes and a two-finger tap cancels/leaves it;
  3. keyboard movement keeps working next to touch.

Options are owned by the Launcher: the gate turns touch on through the Launcher
UI instead of re-configuring the Runtime, because the Launcher (re)applies its
own touch options after launch.

Usage: python tests/test-th20-touch-browser.py
"""
from __future__ import annotations

import math
import os
import re
import socket
import subprocess
import time
from pathlib import Path

from playwright.sync_api import sync_playwright, TimeoutError as PlaywrightTimeoutError

PROJECT = Path(__file__).resolve().parents[1]
WORKSPACE = PROJECT.parent
VIEWPORT = {"width": 412, "height": 915}


def free_port() -> int:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.bind(("127.0.0.1", 0))
        return int(sock.getsockname()[1])


def wait_http(url: str, timeout: float = 120.0) -> None:
    import urllib.request
    deadline = time.time() + timeout
    while time.time() < deadline:
        try:
            with urllib.request.urlopen(url, timeout=1) as response:
                if response.status < 400:
                    return
        except Exception:
            time.sleep(.25)
    raise RuntimeError("Launcher HTTP server did not start")


def runtime_eval(page, expression: str):
    """Evaluate in the Runtime frame; other frames answer null and are skipped."""
    for frame in page.frames:
        try:
            value = frame.evaluate(expression)
        except Exception:
            continue
        if value is not None:
            return value
    return None


PROBE = """() => {
  const core = globalThis.__th20Runtime?.core;
  if (!core) return null;
  const probe = new Float32Array(core.memory.buffer, core.sdl_touch_probe(), 17);
  const player = new Float32Array(core.memory.buffer, core.sdl_player_state(), 4);
  return {probe: Array.from(probe), player: Array.from(player)};
}"""

# [context, dragging, analog_active, analog_x, analog_y, confirm_pulse, escape_pulse, pause_state,
#  enabled, ready, motion, buttons_current, buttons_pressed, window_active,
#  confirm_pulses, escape_pulses, sample_ticks]
CONTEXT, DRAGGING, ANALOG, ANALOG_X, ANALOG_Y = 0, 1, 2, 3, 4
PAUSE_STATE, ENABLED, READY, BUTTONS, WINDOW_ACTIVE = 7, 8, 9, 11, 13
CONFIRM_PULSES, ESCAPE_PULSES, SAMPLE_TICKS = 14, 15, 16


def state(page) -> dict | None:
    return runtime_eval(page, PROBE)


def key(page, code: str, down: bool) -> None:
    """Send a hosted key the way the Runtime shell does (UTF-8 pointer)."""
    runtime_eval(page, f"""() => {{
      const core = globalThis.__th20Runtime?.core;
      if (!core) return null;
      const bytes = new TextEncoder().encode({code!r} + '\\0');
      const pointer = core.graphics_allocate(bytes.length);
      try {{
        new Uint8Array(core.memory.buffer, pointer, bytes.length).set(bytes);
        core.sdl_key(pointer, {1 if down else 0});
      }} finally {{ core.graphics_free(pointer); }}
      return true;
    }}""")


class MobileTouch:
    """The Launcher's direct-touch surface, driven with trusted touch events."""

    def __init__(self, page, context, rect):
        self.page = page
        self.rect = rect
        self.client = context.new_cdp_session(page)
        self.next_id = 1000

    def point(self, x: float, y: float) -> tuple[float, float]:
        return self.rect["x"] + x, self.rect["y"] + y

    def dispatch(self, kind: str, points) -> None:
        self.client.send("Input.dispatchTouchEvent", {
            "type": kind,
            "touchPoints": [{"x": x, "y": y, "id": identifier} for identifier, x, y in points],
        })

    def tap(self, x: float, y: float, hold_ms: int = 60) -> None:
        self.next_id += 1
        point = self.point(x, y)
        self.dispatch("touchStart", [(self.next_id, *point)])
        self.page.wait_for_timeout(hold_ms)
        self.dispatch("touchEnd", [])


def ticking(page, timeout: float = 15.0) -> bool:
    """Wait until the Runtime's frame loop is advancing its input samples."""
    first = state(page)
    if not first:
        return False
    deadline = time.time() + timeout
    while time.time() < deadline:
        page.wait_for_timeout(80)
        snapshot = state(page)
        if snapshot and snapshot["probe"][SAMPLE_TICKS] != first["probe"][SAMPLE_TICKS]:
            return True
    return False


def wait_enabled(page, timeout: float = 30.0) -> bool:
    """Wait for the Launcher's touch option to reach the Runtime."""
    deadline = time.time() + timeout
    while time.time() < deadline:
        snapshot = state(page)
        if snapshot and snapshot["probe"][ENABLED] == 1:
            return True
        page.wait_for_timeout(200)
    return False


def wait_paused(page, paused: bool, timeout: float = 20.0) -> dict | None:
    deadline = time.time() + timeout
    snapshot = None
    while time.time() < deadline:
        snapshot = state(page)
        if snapshot and (snapshot["probe"][PAUSE_STATE] != 0) == paused:
            return snapshot
        page.wait_for_timeout(150)
    return None


def observed(page, index: int, timeout: float = 2.0, interval_ms: int = 20) -> bool:
    """Report whether a drained one-shot pulse counter became nonzero."""
    deadline = time.time() + timeout
    while time.time() < deadline:
        snapshot = state(page)
        if snapshot and snapshot["probe"][index] != 0:
            return True
        page.wait_for_timeout(interval_ms)
    return False


def canvas_size(page) -> tuple[float, float]:
    size = runtime_eval(page, """() => {
      if (!globalThis.__th20Runtime) return null;
      const element = document.querySelector('canvas');
      if (!element) return null;
      const rect = element.getBoundingClientRect();
      return rect.width > 0 && rect.height > 0 ? [rect.width, rect.height] : null;
    }""")
    assert size, "the Runtime canvas was never measurable"
    return float(size[0]), float(size[1])


def start_game(touch: MobileTouch, page, timeout: float = 25.0) -> None:
    """Start a run by tapping, the way a phone player does.

    Taps are the mobile path under test: the shared controller turns a tap in a
    menu/dialogue context into a confirm pulse, which the port maps onto the
    configured confirm binding. The title starts its attract loop about 30
    seconds in, so passing this bound means tapping advanced the menus rather
    than the demo reaching gameplay on its own.
    """
    deadline = time.time() + timeout
    while time.time() < deadline:
        snapshot = state(page)
        if snapshot and snapshot["probe"][CONTEXT] == 1 and snapshot["player"][0] == 1 and snapshot["player"][1] == 1:
            return
        touch.tap(VIEWPORT["width"] / 2, VIEWPORT["height"] / 2)
        page.wait_for_timeout(700)
    raise RuntimeError(f"TH20 never reached gameplay by tapping: {state(page)}")


def drag_movement(touch: MobileTouch, page, angle: float = -30.0, length_px: float = 60.0,
                  samples: int = 45) -> tuple[float, float, float, list[tuple]]:
    """Drag towards `angle` (up-right for -30) and report the movement angles.

    Returns (player path angle, analog vector angle, travelled distance, trail).
    The expected field-space direction is derived from the Runtime canvas rect,
    because the Launcher hands over fractions of its canvas and the shared
    controller maps those to the 640x480 playfield. The drag is sampled while the
    finger is still down: the player reaches the target within a few frames and
    then reports no vector at all.
    """
    assert ticking(page), "the Runtime never advanced its input samples"
    width, height = canvas_size(page)
    # field_x = (px / canvas_width) * 640 and field_y = (py / canvas_height) * 480,
    # so this screen delta drags exactly along `angle` in field units and stays
    # well inside the playfield instead of pinning the player to a border.
    ratio = (height / 480.0) / (width / 640.0)
    dx = length_px
    dy = length_px * math.tan(math.radians(angle)) * ratio
    start_x, start_y = width / 2, height / 2
    assert start_x + dx < width and start_y + dy > 0, "the drag would leave the canvas"
    touch.next_id += 1
    identifier = touch.next_id
    touch.dispatch("touchStart", [(identifier, *touch.point(start_x, start_y))])
    page.wait_for_timeout(80)
    touch.dispatch("touchMove", [(identifier, *touch.point(start_x + dx, start_y + dy))])
    trail = []
    for _ in range(samples):
        page.wait_for_timeout(16)
        snapshot = state(page)
        if snapshot:
            probe, player = snapshot["probe"], snapshot["player"]
            trail.append((probe[ANALOG], player[2], player[3], probe[ANALOG_X], probe[ANALOG_Y],
                          probe[CONTEXT], probe[DRAGGING], probe[ENABLED]))
    touch.dispatch("touchEnd", [])
    active = [sample for sample in trail if sample[0] == 1]
    assert active, f"the drag never activated the direct-touch path: {trail[:3]}"
    moving = [sample for sample in active if math.hypot(sample[3], sample[4]) > 20]
    assert moving, f"the drag produced no movement vector: {active[:3]}"
    first, last = active[0], moving[-1]
    path_dx, path_dy = last[1] - first[1], last[2] - first[2]
    vector_x = sum(sample[3] for sample in moving)
    vector_y = sum(sample[4] for sample in moving)
    summary = {"canvas": (width, height), "active": len(active), "moving": len(moving),
               "first": trail[0], "last": trail[-1], "moving_head": moving[:2]}
    assert len(active) >= 5, f"the direct-touch path was active for too few frames: {summary}"
    assert math.hypot(path_dx, path_dy) > 10, f"the drag moved the player too little: {summary}"
    return (math.degrees(math.atan2(path_dy, path_dx)), math.degrees(math.atan2(vector_y, vector_x)),
            math.hypot(path_dx, path_dy), trail)


def main() -> None:
    port = free_port()
    url = f"http://127.0.0.1:{port}/?game=th20"
    content = WORKSPACE / "games" / "th20-content"
    if not (content / "th20.data").is_file():
        raise SystemExit(f"prepared TH20 content not found: {content}")
    env = os.environ.copy()
    env.update({
        "EAGLER_DEVELOPMENT_GAMES": "th20",
        "EAGLER_TH20_CONTENT_DIR": str(content),
        "EAGLER_DEVELOPMENT_VANILLA_FONT": str(WORKSPACE / "prepared" / "eagler-touhou-hosted-five-games-20260924" / "shared" / "msgothic.ttc"),
        "EAGLER_DEVELOPMENT_UNICODE_FONT": str(WORKSPACE / "prepared" / "eagler-touhou-hosted-five-games-20260924" / "shared" / "unifont.otf"),
    })
    server = subprocess.Popen(["node", "scripts/serve.mjs", str(port)], cwd=PROJECT, env=env,
                              stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    try:
        wait_http(f"http://127.0.0.1:{port}/")
        with sync_playwright() as playwright:
            browser = playwright.chromium.launch(headless=True)
            context = browser.new_context(viewport=VIEWPORT, has_touch=True, is_mobile=True,
                                          device_scale_factor=2)
            page = context.new_page()
            errors: list[str] = []
            benign = re.compile(r"^(?:Failed to load resource|th20 wasm build|sdl_game_open|tick \d+ scene \d+|worker\[)")
            missing_card_art = re.compile(r"/assets/th\d+-card\.webp$")
            page.on("pageerror", lambda error: errors.append(f"pageerror:{error}"))
            page.on("console", lambda message: errors.append(f"{message.type}:{message.text}")
                    if message.type == "error" and not benign.search(message.text) else None)
            page.on("response", lambda response: errors.append(f"http{response.status}:{response.url}")
                    if response.status >= 400 and not missing_card_art.search(response.url) else None)
            try:
                page.goto(url, wait_until="load", timeout=60_000)
                page.wait_for_function("window.__eaglerBoot?.done === true", timeout=90_000)
                page.wait_for_timeout(800)
                page.evaluate("document.querySelector('#firstUseNoticeDialog')?.close()")

                # The Launcher owns the touch option; enabling it here is the
                # real mobile path and avoids re-configuring a running Runtime.
                assert page.locator("#touchToggle").is_visible(), "the touch option is not offered"
                page.locator("#touchToggle").click(force=True)
                page.wait_for_timeout(200)
                # The movement select lives in the collapsed settings panel; a
                # fresh profile already defaults to "touch", but pin it anyway.
                page.evaluate("""() => {
                  const select = document.querySelector('#touchMovementMode');
                  select.value = 'touch';
                  select.dispatchEvent(new Event('change', {bubbles: true}));
                }""")
                page.wait_for_timeout(200)
                assert page.locator("#touchMovementMode").input_value() == "touch"
                page.locator("#musicSelect").select_option("none", force=True)

                deadline = time.time() + 90
                while not page.locator("#player").evaluate("el => el.classList.contains('open')"):
                    if page.locator("#decisionDialog[open]").count():
                        try:
                            page.locator("#decisionConfirm").click(timeout=1200)
                        except PlaywrightTimeoutError:
                            pass
                    else:
                        try:
                            page.locator("#launch").click(timeout=1200)
                        except PlaywrightTimeoutError:
                            pass
                    page.wait_for_timeout(250)
                    if time.time() > deadline:
                        raise RuntimeError("the TH20 Runtime never opened")
                page.wait_for_selector("#player.open", timeout=60_000)
                page.wait_for_timeout(1000)
                if page.locator("#touchHelp").is_visible():
                    page.locator("#touchHelpClose").click()

                deadline = time.time() + 180
                while time.time() < deadline and not state(page):
                    page.wait_for_timeout(500)
                assert state(page), "TH20 Runtime never booted"
                assert wait_enabled(page), f"the Launcher's touch option never reached the Runtime: {state(page)}"
                touch = MobileTouch(page, context, page.locator("#gameFrame").bounding_box())

                start_game(touch, page)
                live = state(page)
                assert live["probe"][READY] == 1, f"gameplay never reported a live player: {live}"
                assert live["probe"][WINDOW_ACTIVE] == 1, f"the input window is not active: {live}"

                # Free-direction movement: -30 degrees is not an octant of the
                # recovered eight-way input.
                path_angle, vector_angle, distance, trail = drag_movement(touch, page)
                assert abs(vector_angle - (-30)) < 15, (
                    f"the ported movement vector is not the finger direction: {vector_angle:.1f} "
                    f"(trail={trail[:3]})")
                assert abs(path_angle - (-30)) < 20, (
                    f"the player did not travel along the drag: {path_angle:.1f} (trail={trail[:3]})")

                # Keyboard keeps the recovered eight-way path.
                before = state(page)["player"]
                key(page, "ArrowRight", True)
                page.wait_for_timeout(500)
                held = state(page)["probe"][BUTTONS]
                key(page, "ArrowRight", False)
                page.wait_for_timeout(120)
                after = state(page)["player"]
                assert held != 0, "the hosted keyboard key never reached the game input owner"
                assert after[2] - before[2] > 4, f"keyboard movement regressed: {before} -> {after}"

                # Pause menu: the shared controller must own the menu context so a
                # tap confirms and a two-finger tap cancels.
                assert page.locator("#touchEscape").is_visible(), "the touch pause button is missing"
                page.locator("#touchEscape").click()
                paused = wait_paused(page, True)
                assert paused, f"the Launcher's pause button never opened the menu: {state(page)}"
                assert paused["probe"][CONTEXT] == 0, f"the pause menu must report the menu context: {paused}"
                assert paused["probe"][ANALOG] == 0, "a paused game must not run the movement path"

                touch.tap(VIEWPORT["width"] / 2, VIEWPORT["height"] / 2)
                assert observed(page, CONFIRM_PULSES), "a tap in the pause menu produced no confirm pulse"
                assert wait_paused(page, False), f"a tap did not confirm/resume: {state(page)}"

                page.locator("#touchEscape").click()
                assert wait_paused(page, True), f"the pause menu did not reopen: {state(page)}"
                touch.next_id += 1
                first_id, second_id = touch.next_id, touch.next_id + 1
                touch.dispatch("touchStart", [(first_id, *touch.point(VIEWPORT["width"] / 2 - 30, VIEWPORT["height"] / 2)),
                                              (second_id, *touch.point(VIEWPORT["width"] / 2 + 30, VIEWPORT["height"] / 2))])
                page.wait_for_timeout(70)
                touch.dispatch("touchEnd", [])
                assert observed(page, ESCAPE_PULSES), "a two-finger tap in the pause menu produced no cancel pulse"
                assert wait_paused(page, False), f"a two-finger tap did not cancel: {state(page)}"

                assert not errors, errors
                print(f"TH20 touch: PASS path_angle={path_angle:.1f}deg vector_angle={vector_angle:.1f}deg "
                      f"distance={distance:.1f} tap_confirm=ok two_finger_cancel=ok pause_button=ok keyboard=ok")
            finally:
                browser.close()
    finally:
        server.terminate()
        try:
            server.wait(timeout=10)
        except subprocess.TimeoutExpired:
            server.kill()
            server.wait(timeout=10)


if __name__ == "__main__":
    main()
