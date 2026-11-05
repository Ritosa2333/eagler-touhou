#!/usr/bin/env python3
"""Focused contracts for the sbrik AsciiManager HUD feature-rebase class."""

from __future__ import annotations

import pathlib
import re
import subprocess


ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = (ROOT / "src" / "AsciiManager.cpp").read_text(encoding="utf-8")
GUI = (ROOT / "src" / "Gui.cpp").read_text(encoding="utf-8")
BASE = "5b9ebe892914ff5666ef68c0cd02719dde7d4ee9"
FINAL = "022c533"


def require(name: str, condition: bool) -> None:
    if not condition:
        raise AssertionError(name)
    print(f"PASS: {name}")


def git(*args: str) -> str:
    return subprocess.check_output(["git", *args], cwd=ROOT).decode(
        "utf-8", errors="replace").replace("\r\n", "\n")


def function_body(source: str, signature: str) -> str:
    start = source.index(signature)
    brace = source.index("{", start)
    depth = 0
    for index in range(brace, len(source)):
        if source[index] == "{":
            depth += 1
        elif source[index] == "}":
            depth -= 1
            if depth == 0:
                return source[start:index + 1]
    raise AssertionError(signature)


def main() -> None:
    diff = git("diff", "--unified=0", f"{BASE}..{FINAL}", "--",
               "src/th07/AsciiManager.cpp")
    require("frozen AsciiManager.cpp has eleven hunks",
            len(re.findall(r"^@@ ", diff, flags=re.MULTILINE)) == 11)

    update = function_body(SOURCE, "u32 AsciiManager::OnUpdate(AsciiManager")
    require("pause redraw is no longer driven by Ascii update counter",
            "renderSkipFrames" not in update)
    scene = function_body(GUI, "void Gui::DrawGameScene()")
    require("static HUD redraw directly checks both menus",
            "g_GameManager.isInPauseMenu || g_GameManager.isInRetryMenu" in scene)

    draw_menus = function_body(SOURCE, "u32 AsciiManager::OnDrawMenus(AsciiManager")
    require("menu sprites flush before full viewport restore",
            draw_menus.index("g_AnmManager->Flush()") <
            draw_menus.index("g_Supervisor.viewport.x = 0"))
    require("portable full viewport is restored after menus",
            "g_Supervisor.viewport.width = 640" in draw_menus and
            "g_Supervisor.viewport.height = 480" in draw_menus and
            "g_Supervisor.gfxDevice->SetViewport(g_Supervisor.viewport)" in draw_menus)

    pause = function_body(SOURCE, "i32 PauseMenu::OnUpdate()")
    require("opening edge cannot immediately close uninitialized pause menu",
            "this->curState != 0" in pause and "this->curState != 4" in pause)
    retry = function_body(SOURCE, "i32 RetryMenu::OnUpdate()")
    require("retry resets each active guest resource pool",
            "i = 1; i < TH07_MULTI_MAX_PLAYERS" in retry and
            "IsPlayerSlotActive((u8)i)" in retry and
            "ResetMultiplayerPlayerResources((u8)i)" in retry)

    popups = function_body(SOURCE, "void AsciiManager::DrawPopups()")
    require("multiplayer Cherry HUD uses shared Border state",
            "IsSharedBorderActive()" in popups and
            "borderActiveForHud" in popups)
    require("multiplayer Cherry HUD uses shared threshold",
            "GetSharedBorderThreshold()" in popups and
            "borderThresholdForHud" in popups)
    require("ordinary Cherry HUD retains vanilla state and threshold",
            "g_Player.hasBorder != BORDER_NONE" in popups and
            "const i32 borderThresholdForHud = 50000" in popups)

    require("Cherry digit interpolation state follows temporary scale",
            SOURCE.count("this->cherryDigit.prevScale = this->cherryDigit.scale") >= 3)
    require("Cherry digit color interpolation state is published before draw",
            SOURCE.count("this->cherryDigit.prevColor = this->cherryDigit.color") >= 2)
    require("shared Border visual preserves previous color",
            "this->cherryBorderActive.prevColor.color = this->cherryGauge.prevColor.color" in popups)

    # The two upstream text filters only consume native diagnostics. Eagler's
    # browser HUD does not emit those strings, so importing prefix-based text
    # suppression would risk hiding ordinary/localized content.
    require("obsolete P2 native diagnostic producer is absent",
            '"P2 L"' not in SOURCE and '"P2 L"' not in GUI)
    require("obsolete NET native diagnostic producer is absent",
            '"NET "' not in SOURCE and '"NET "' not in GUI)


if __name__ == "__main__":
    main()
