from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def text(path: str) -> str:
    return (ROOT / path).read_text(encoding="utf-8")


result = text("src/ResultScreen.cpp")
driver = text("src/netplay/Th06LanStageProbe.cpp")

# Multiplayer results may still offer/save Replay, but must never merge the
# room economy into ordinary single-player score.dat progression.
assert "bool ShouldSkipPersistentResultWrite()" in result
assert "return MultiplayerGameplay::IsMultiplayer();" in result
write_score = result.split("void ResultScreen::WriteScore(ResultScreen *resultScreen)", 1)[1]
write_score = write_score.split("u32 ResultScreen::GetHighScore", 1)[0]
assert "if (ShouldSkipPersistentResultWrite())\n        return;" in write_score
assert "ReplayManager::SaveReplay(replayPath, this->replayName);" in result

# The in-memory per-stage best score must also stay single-player-only; merely
# skipping the final file write is not enough if later UI code can persist it.
assert "if (!ShouldSkipPersistentResultWrite() &&\n        resultScreen->resultScreenState == RESULT_SCREEN_STATE_EXIT" in result

# The shared gameplay ABI owns the confirmed gameplay lifecycle, then deliberately retires
# before the vanilla local Ending/Result chain. Result persistence remains
# isolated without rewriting the user's ordinary CLRD history.
assert "constexpr std::uint32_t GAMEPLAY_ABI = TH06_MULTI_GAMEPLAY_ABI;" in driver
assert "TH06_MULTI_GAMEPLAY_ABI = 8;" in text("src/Multiplayer.hpp")
owns = driver.split("bool SessionStillOwnsStageState()", 1)[1].split(
    "bool SharedUiNeedsConfirmedInputs()", 1
)[0]
assert "SUPERVISOR_STATE_GAMEMANAGER" in owns
assert "SUPERVISOR_STATE_ENDING" not in owns
run_calc = driver.split("int RunCalcChain()", 1)[1]
assert "g_Initialized && !SessionStillOwnsStageState()" in run_calc
assert "RetireGameplaySession();" in run_calc
assert "void NormalizeMultiplayerEndingSkipHistory()" in driver
normalize = driver.split("void NormalizeMultiplayerEndingSkipHistory()", 1)[1].split(
    "bool RemoteInputsTimedOut()", 1
)[0]
assert "difficultyCleared" not in normalize
assert "NormalizeMultiplayerEndingSkipHistory();" in driver

# Shared non-gameplay UI (including Ending) is never predicted.
assert "SharedUiNeedsConfirmedInputs()" in driver
assert "ConfirmedThroughAllRemotes() < g_SimFrame" in driver

print("TH06 multiplayer Result/Ending contract: PASS")
