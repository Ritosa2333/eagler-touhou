#!/usr/bin/env python3
"""Focused contracts for the sbrik GameWindow feature-rebase class."""

from __future__ import annotations

import pathlib
import re
import subprocess


ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = (ROOT / "src" / "GameWindow.cpp").read_text(encoding="utf-8")
HEADER = (ROOT / "src" / "GameWindow.hpp").read_text(encoding="utf-8")
INPUT_PROBE = (ROOT / "src" / "netplay" / "Th07LanStageProbe.cpp").read_text(encoding="utf-8")
BASE = "5b9ebe892914ff5666ef68c0cd02719dde7d4ee9"
FINAL = "022c533"


def require(name: str, condition: bool) -> None:
    if not condition:
        raise AssertionError(name)
    print(f"PASS: {name}")


def git(*args: str) -> str:
    return subprocess.check_output(
        ["git", *args], cwd=ROOT, text=True, encoding="utf-8", errors="replace"
    ).replace("\r\n", "\n")


def hunk_count(path: str) -> int:
    diff = git("diff", "--unified=0", f"{BASE}..{FINAL}", "--", path)
    return len(re.findall(r"^@@ ", diff, flags=re.MULTILINE))


def main() -> None:
    require("frozen GameWindow.cpp has ten hunks",
            hunk_count("src/th07/GameWindow.cpp") == 10)
    require("frozen GameWindow.hpp has two hunks",
            hunk_count("src/th07/GameWindow.hpp") == 2)
    require("native Netplay display shell is excluded",
            '#include "Netplay.hpp"' not in SOURCE and
            "Netplay::ForceFullscreen" not in SOURCE and
            "Netplay::ForceWindowed" not in SOURCE)
    require("native peer-selected window dimensions are excluded",
            "Netplay::GetWindowClientWidth" not in SOURCE and
            "Netplay::GetWindowClientHeight" not in SOURCE)
    require("portable SDL window host is retained",
            "SDL_Window *window" in HEADER and "SDL_CreateWindow" in SOURCE)
    require("native original-render wrapper is excluded",
            "OriginalRender" not in HEADER and "TH07_COMPILE_ORIGINAL_RENDER" not in HEADER)

    require("simulation keeps a fixed sixty-hertz base with bounded netplay pacing",
            "const f64 baseTargetDt = 1.0 / 60.0" in SOURCE and
            "baseTargetDt * Netplay::Th07LanStageProbe::SimulationIntervalScale()" in SOURCE)
    require("wall clock spikes are bounded", "if (elapsed > 0.1)" in SOURCE and "elapsed = 0.1" in SOURCE)
    require("netplay catchup is bounded to six ticks",
            "constexpr i32 maxNetplayCatchupTicks = 6" in SOURCE and
            "catchupTicks < maxNetplayCatchupTicks" in SOURCE)
    require("stalled tick preserves bounded backlog",
            "if (!lastSimulationTickAdvanced)" in SOURCE and
            "targetDt * (f64)maxNetplayCatchupTicks" in SOURCE)
    require("advanced tick alone commits audio queues",
            "if (lastSimulationTickAdvanced)\n            g_SoundPlayer.ProcessQueues();" in SOURCE)
    require("render interpolation is accumulator-derived and clamped",
            "g_RenderAlpha = std::clamp" in SOURCE and
            "this->accumulator / targetDt" in SOURCE)
    require("sixty-hertz presentation draws current state",
            "if (limitPresentationTo60)" in SOURCE and "g_RenderAlpha = 1.0f" in SOURCE)
    require("pause and retry draw current state",
            "g_GameManager.isInPauseMenu || g_GameManager.isInRetryMenu" in SOURCE)
    require("draw-only frames suppress ANM simulation advance",
            "g_SuppressAnmAdvance = !updated" in SOURCE and
            "g_SuppressAnmAdvance = false" in SOURCE)
    require("camera shake uses the same render alpha",
            "prevShakeOffset.Lerp(g_AnmManager->shakeOffset, g_RenderAlpha)" in SOURCE)
    require("Web audio pump remains presentation-cadenced", "g_SoundPlayer.PumpWebAudio()" in SOURCE)

    require("physical touch and controller are sampled once per scheduled frame",
            "Input::BeginCapture();" in INPUT_PROBE and
            "(void)Controller::GetInput();" in INPUT_PROBE and
            "Input::SetReplayOverride(CombinedButtons(decision));" in INPUT_PROBE)


if __name__ == "__main__":
    main()
