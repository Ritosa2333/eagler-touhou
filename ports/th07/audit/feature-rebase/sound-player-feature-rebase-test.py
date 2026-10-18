#!/usr/bin/env python3
"""Focused contracts for the sbrik SoundPlayer feature-rebase class."""

from __future__ import annotations

import pathlib
import re
import subprocess


ROOT = pathlib.Path(__file__).resolve().parents[2]
SOUND = (ROOT / "src" / "SoundPlayer.cpp").read_text(encoding="utf-8")
WINDOW = (ROOT / "src" / "GameWindow.cpp").read_text(encoding="utf-8")
STAGE = (ROOT / "src" / "netplay" / "Th07LanStageProbe.cpp").read_text(encoding="utf-8")
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


def main() -> None:
    diff = git("diff", "--unified=0", f"{BASE}..{FINAL}", "--", "src/th07/SoundPlayer.cpp")
    require("frozen SoundPlayer.cpp has two hunks",
            len(re.findall(r"^@@ ", diff, flags=re.MULTILINE)) == 2)
    require("native Netplay audio shell is excluded",
            '#include "Netplay.hpp"' not in SOUND and "Netplay::IsBgmEnabled" not in SOUND)
    require("portable audio keeps local music preference",
            "g_Supervisor.cfg.musicMode == MUSIC_OFF" in SOUND)
    require("portable StartBGM retains browser audio path",
            "ZunResult SoundPlayer::StartBGM" in SOUND and
            "FileSystem::GetBasePath(path)" in SOUND)
    require("SFX suppression is limited to rollback side effects",
            "if (Netplay::SideEffects::IsSpeculative())\n        return;" in SOUND)
    require("production simulation marks only resimulation passes",
            "SideEffects::SetSpeculative(resimulation);" in STAGE)
    require("side-effect suppression is cleared after each simulation pass",
            "SideEffects::SetSpeculative(false);" in STAGE)
    require("audio queues advance only after a logical tick",
            "if (lastSimulationTickAdvanced)\n            g_SoundPlayer.ProcessQueues();" in WINDOW)
    require("Web audio transition guard remains paired",
            "SetWebAudioBgmTransition(true)" in SOUND and
            "SetWebAudioBgmTransition(false)" in SOUND)


if __name__ == "__main__":
    main()
