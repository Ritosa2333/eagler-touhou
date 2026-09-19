from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
player = (ROOT / "src" / "Player.cpp").read_text(encoding="utf-8")
game = (ROOT / "src" / "GameManager.cpp").read_text(encoding="utf-8")
driver = (ROOT / "src" / "netplay" / "Th07LanStageProbe.cpp").read_text(encoding="utf-8")
replay = (ROOT / "src" / "ReplayManager.cpp").read_text(encoding="utf-8")

assert "ApplyActivePlayerCountParameters(2, 3)" not in player, \
    "3P CherryMax scaling must not run from stage-local Player callbacks"
assert game.count("ApplyActivePlayerCountParameters(2, 3)") == 1, \
    "fresh-run 3P CherryMax scaling must occur exactly once"
call = game.index("ApplyActivePlayerCountParameters(2, 3)")
init = game.index("arg->cherryMax = arg->globals->cherryStart + 200000;")
assert call > init, "3P scale must be applied after initial CherryMax setup"
window = game[max(0, call - 500):call]
assert "!g_GameManager.replay" in window, "Replay playback must restore recorded Cherry state instead of rescaling"
assert "MultiplayerGameplay::GetPlayerCount() >= 3" in window, "3P scale must be gated by player count"
assert "constexpr std::uint32_t GAMEPLAY_ABI = TH07_MULTI_GAMEPLAY_ABI;" in driver, "CherryMax gameplay change must bump live ABI"
assert "config.gameplayAbi = TH07_MULTI_GAMEPLAY_ABI;" in replay, "new multiplayer Replays must record ABI 3"
print("TH07 multiplayer CherryMax contract: PASS fresh-run-only=1 abi=4")
