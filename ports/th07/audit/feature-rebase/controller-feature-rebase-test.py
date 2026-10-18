#!/usr/bin/env python3
"""Focused contracts for the sbrik Controller feature-rebase class."""

from __future__ import annotations

import pathlib
import re
import subprocess


ROOT = pathlib.Path(__file__).resolve().parents[2]
COMMON = ROOT / "third_party" / "eagler-common"
SOURCE = (ROOT / "src" / "Controller.cpp").read_text(encoding="utf-8")
HEADER = (ROOT / "src" / "Controller.hpp").read_text(encoding="utf-8")
NET_INPUT = (COMMON / "src" / "netplay" / "NetplayInput.cpp").read_text(encoding="utf-8")
INPUT_CONFIG = (ROOT / "src" / "netplay" / "NetplayInputConfig.hpp").read_text(encoding="utf-8")
BASE = "5b9ebe892914ff5666ef68c0cd02719dde7d4ee9"
FINAL = "022c533"


def require(name: str, condition: bool) -> None:
    if not condition:
        raise AssertionError(name)
    print(f"PASS: {name}")


def git(*args: str) -> str:
    return subprocess.check_output(
        ["git", *args], cwd=ROOT, text=True, encoding="utf-8"
    ).replace("\r\n", "\n")


def hunk_count(path: str) -> int:
    diff = git("diff", "--unified=0", f"{BASE}..{FINAL}", "--", path)
    return len(re.findall(r"^@@ ", diff, flags=re.MULTILINE))


def main() -> None:
    require("frozen Controller.cpp has two hunks", hunk_count("src/th07/Controller.cpp") == 2)
    require("frozen Controller.hpp has three hunks", hunk_count("src/th07/Controller.hpp") == 3)

    require("current and previous input are selected by stable player slot",
            "g_CurFrameGameInputs[(player)->initParam]" in HEADER and
            "g_LastFrameGameInputs[(player)->initParam]" in HEADER)
    require("edge detection compares the same player lane",
            HEADER.count("g_CurFrameGameInputs[(player)->initParam]") >= 3 and
            "g_LastFrameGameInputs[(player)->initParam]" in HEADER)
    require("ordinary gameplay keeps vanilla input macros",
            "#define IS_PRESSED_PLAYER(player, key) IS_PRESSED_GAME(key)" in HEADER and
            "#define WAS_PRESSED_PLAYER(player, key) WAS_PRESSED_GAME(key)" in HEADER)

    require("Windows local co-op sampler is excluded",
            "GetInput2" not in SOURCE + HEADER and "GetAsyncKeyState" not in SOURCE)
    require("native controller diagnostic hunk is excluded",
            "controller detected name" not in SOURCE and "GameErrorContext" not in SOURCE)

    replay_guard = SOURCE.index("if (Netplay::Input::ReplayOverrideActive())")
    keyboard_sample = SOURCE.index("SDL_GetKeyboardState")
    touch_sample = SOURCE.index("buttons |= Touch::GetButtonBits();")
    require("rollback replay avoids physical input resampling", replay_guard < keyboard_sample < touch_sample)
    require("Eagler keyboard and gamepad sources remain composed",
            "EaglerOptions::BrowserKeyboardBits()" in SOURCE and
            "EaglerOptions::BrowserGamepadDirectionBits()" in SOURCE)
    require("touch joystick keeps configured deadzone",
            "EaglerOptions::TouchJoystickX()" in SOURCE and
            "g_Supervisor.cfg.padAxisX" in SOURCE and
            "g_Supervisor.cfg.padAxisY" in SOURCE)
    require("touch buttons join the sampled logical word before netplay",
            touch_sample < SOURCE.index("return Netplay::Input::ResolveLocal(buttons)", touch_sample))

    require("synchronized lanes carry full touch payload",
            "g_PlayerInputOverrides[i] = inputs[i]" in NET_INPUT and
            "g_PlayerButtonOverrides[i] = inputs[i].buttons" in NET_INPUT)
    require("logical lanes commit before gameplay simulation",
            "InputConfig::CommitGameInputs(g_PlayerButtonOverrides)" in NET_INPUT and
            "g_LastFrameGameInputs[player] = g_CurFrameGameInputs[player]" in INPUT_CONFIG and
            "g_CurFrameGameInputs[player] = buttons[player]" in INPUT_CONFIG)
    require("raw touch is never sampled from the transport layer",
            "Touch::" not in NET_INPUT and "EaglerOptions::" not in NET_INPUT)


if __name__ == "__main__":
    main()
