#!/usr/bin/env python3
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
COMMON = ROOT / "third_party" / "eagler-common"
DRIVER = (ROOT / "src/netplay/Th07LanStageProbe.cpp").read_text(encoding="utf-8")
SHELL = (ROOT / "resources/shell.html").read_text(encoding="utf-8")
PEER = (COMMON / "src/netplay/BrowserPeerTransport.cpp").read_text(encoding="utf-8")

assert (COMMON / "include/eagler/netplay/InputRepairBudget.hpp").is_file()
assert (COMMON / "testkit/rtc-input-impairment.cjs").is_file()
assert not (ROOT / "src/netplay/InputRepairBudget.hpp").exists()
assert not (ROOT / "tests/rtc-input-impairment.cjs").exists()
assert '#include <eagler/netplay/InputRepairBudget.hpp>' in DRIVER
assert "std::array<InputRepairBudget, MAX_PLAYERS> g_InputRepairBudgets" in DRIVER
fast_send = DRIVER.index("!TransportSendTo(peer, wire.data(), wire.size())")
budget = DRIVER.index("g_InputRepairBudgets[peer].ShouldRepair(", fast_send)
repair = DRIVER.index("g_BrowserPeerTransport.SendRepairTo(peer, wire.data(), wire.size())", budget)
assert fast_send < budget < repair
assert "packet.firstInputFrame, packet.inputCount != 0, SDL_GetTicks()" in DRIVER
assert "inputRepairSent" in PEER
assert '"netplayReliableInputRepair"' in SHELL
assert "options.netplayReliableInputRepair ??" in SHELL
assert 'options.netplayMode === "lan" && !options.netplaySpectator && !options.replayViewer' in SHELL
print("TH07 reliable input repair wiring: PASS")
