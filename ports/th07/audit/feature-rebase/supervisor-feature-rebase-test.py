#!/usr/bin/env python3
"""Focused contracts for the sbrik Supervisor feature-rebase class."""
from __future__ import annotations
import pathlib, re, subprocess

ROOT = pathlib.Path(__file__).resolve().parents[2]
COMMON = ROOT / "third_party" / "eagler-common"
SOURCE = (ROOT / "src" / "Supervisor.cpp").read_text(encoding="utf-8")
HEADER = (ROOT / "src" / "Supervisor.hpp").read_text(encoding="utf-8")
INPUT = (COMMON / "src" / "netplay" / "NetplayInput.cpp").read_text(encoding="utf-8")
INPUT_CONFIG = (ROOT / "src" / "netplay" / "NetplayInputConfig.hpp").read_text(encoding="utf-8")
ROLLBACK = (ROOT / "src" / "netplay" / "Th07RollbackState.cpp").read_text(encoding="utf-8")
GAME = (ROOT / "src" / "GameManager.cpp").read_text(encoding="utf-8")
MAIN = (ROOT / "src" / "main.cpp").read_text(encoding="utf-8")
BASE = "5b9ebe892914ff5666ef68c0cd02719dde7d4ee9"; FINAL = "022c533"

def require(name, ok):
    if not ok: raise AssertionError(name)
    print(f"PASS: {name}")
def git(*args):
    return subprocess.check_output(["git", *args], cwd=ROOT).decode("utf-8", errors="replace").replace("\r\n", "\n")
def body(source, signature):
    start=source.index(signature); brace=source.index("{",start); depth=0
    for i in range(brace,len(source)):
        if source[i]=='{': depth+=1
        elif source[i]=='}':
            depth-=1
            if depth==0:return source[start:i+1]
    raise AssertionError(signature)

def main():
    cpp=git("diff","--unified=0",f"{BASE}..{FINAL}","--","src/th07/Supervisor.cpp")
    hpp=git("diff","--unified=0",f"{BASE}..{FINAL}","--","src/th07/Supervisor.hpp")
    require("frozen Supervisor.cpp has forty-eight hunks",len(re.findall(r"^@@ ",cpp,re.M))==48)
    require("frozen Supervisor.hpp has two hunks",len(re.findall(r"^@@ ",hpp,re.M))==2)
    require("native Netplay shell is excluded",
            '#include "Netplay.hpp"' not in SOURCE and
            not any(token in SOURCE for token in ("WinSock", "SOCKET", "CreateThread", "LowLatency", "sockaddr", "sendto(")) and
            "Netplay::SideEffects::IsSpeculative()" in SOURCE)
    require("multiplayer header preserves stable P1/P2/P3 lane storage", "g_CurFrameGameInputs[TH07_MULTI_MAX_PLAYERS]" in HEADER and "g_LastFrameGameInputs[TH07_MULTI_MAX_PLAYERS]" in HEADER)
    update=body(SOURCE,"u32 Supervisor::OnUpdate")
    require("Supervisor samples physical input only for the local P1 lane", "Controller::GetInput()" in update and "g_CurFrameRawInput" in update)
    require("synchronized remote lanes are committed before gameplay",
            "SetPlayerInputOverrides" in INPUT and
            "InputConfig::CommitGameInputs(g_PlayerButtonOverrides)" in INPUT and
            "g_LastFrameGameInputs[player] = g_CurFrameGameInputs[player]" in INPUT_CONFIG and
            "g_CurFrameGameInputs[player] = buttons[player]" in INPUT_CONFIG)
    require("rollback captures raw/game lane state", "TouchObject(&g_CurFrameRawInput)" in ROLLBACK and "TouchObject(&g_CurFrameGameInputs)" in ROLLBACK and "TouchObject(&g_LastFrameGameInputs)" in ROLLBACK)
    require("multiplayer startup normalizes life and slow-mode semantics", "arg->defaultCfg->lifeCount = 2" in GAME and "arg->defaultCfg->slowMode = 0" in GAME)
    seed_index = MAIN.index("g_Rng.seed = (u16)requestedSeed")
    configure_index = MAIN.index("MultiplayerGameplay::Configure(gameplaySession)")
    stage_index = MAIN.index("g_GameManager.currentStage = requestedDifficulty", configure_index)
    require("Eagler session seed is installed before harness gameplay dispatch",
            seed_index < configure_index < stage_index and
            "requestedDifficulty < DIFF_EXTRA" in MAIN[stage_index:stage_index + 180])
    require("portable audio/Web lifecycle remains in Supervisor", "IsWebOggMode" in SOURCE and "PlayLoadedAudio" in SOURCE and "StopAudio" in SOURCE)
    require("Supervisor gameplay update does not sample raw touch directly", "Touch::" not in update and "GetPlayerPresentationOffset" not in update)
    require("native diagnostics/layout changes are not introduced", "GetAsyncKeyState" not in cpp and "WinSock" not in cpp and "HashState" not in cpp)

if __name__ == "__main__": main()
