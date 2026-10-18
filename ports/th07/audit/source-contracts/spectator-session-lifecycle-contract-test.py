#!/usr/bin/env python3
"""Protect the one-relay-run lifetime of TH06/TH07 spectator admissions."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[3]


def require(label: str, condition: bool) -> None:
    if not condition:
        raise AssertionError(label)


def check(game: str, source_name: str) -> None:
    source = (ROOT / f"{game}-eagler" / "src" / "netplay" / source_name).read_text(
        encoding="utf-8"
    )
    require(f"{game}: spectator retirement latch exists", "g_SpectatorRunRetired" in source)
    retire = source.split("void RetireGameplaySession()", 1)[1].split("bool TransportConnect", 1)[0]
    require(
        f"{game}: retired spectator closes its old read-only transport",
        "g_SpectatorRunRetired = true;" in retire
        and "g_BrowserPeerTransport.Close();" in retire
        and "g_ProductionTransportStarted = false;" in retire,
    )
    run_calc = source.split("if (!RequestedInternal())", 1)[1]
    latch = run_calc.find("if (!g_Initialized && g_SpectatorRunRetired)")
    preconnect = run_calc.find("StartProductionTransportEarly()")
    require(
        f"{game}: spectator latch blocks reconnect/reinitialize before preconnect",
        latch >= 0 and preconnect >= 0 and latch < preconnect,
    )
    require(
        f"{game}: players still retain generation-based restart support",
        "++g_SessionGeneration;" in retire and "CurrentSessionId()" in source,
    )


check("th06", "Th06LanStageProbe.cpp")
check("th07", "Th07LanStageProbe.cpp")
print("Spectator session lifecycle contract: PASS")
