#!/usr/bin/env python3
"""Focused disposition checks for the sbrik GameErrorContext diff."""

from __future__ import annotations

import pathlib
import re
import subprocess


ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = (ROOT / "src" / "GameErrorContext.cpp").read_text(encoding="utf-8")
HEADER = (ROOT / "src" / "GameErrorContext.hpp").read_text(encoding="utf-8")
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
    diff = git("diff", "--unified=0", f"{BASE}..{FINAL}", "--",
               "src/th07/GameErrorContext.cpp")
    require("frozen GameErrorContext.cpp has three hunks",
            len(re.findall(r"^@@ ", diff, flags=re.MULTILINE)) == 3)
    require("hash-aware log recycling is excluded",
            "hash trace" not in SOURCE and "state verify compared" not in SOURCE)
    require("RNG automatic-repair diagnostics are excluded",
            "RNG resync" not in SOURCE and "RNG mismatch" not in SOURCE)
    require("native rollback log policy is excluded",
            "IsDiscardableNetplayLogLine" not in SOURCE and
            "MakePriorityLogRoom" not in SOURCE)
    require("portable formatter is bounded",
            SOURCE.count("vsnprintf(") == 2 and "vsprintf(" not in SOURCE)
    require("localized diagnostic format is preserved",
            SOURCE.count("Localization::LogString(fmt)") == 2)
    require("fatal diagnostics still request the error dialog",
            "this->m_ShowMessageBox = true;" in SOURCE)
    require("existing fixed log buffer contract is unchanged",
            "char m_Buffer[8192];" in HEADER and "m_Buffer + 0x1fff" in SOURCE)


if __name__ == "__main__":
    main()
