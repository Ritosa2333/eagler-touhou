from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def text(path: str) -> str:
    return (ROOT / path).read_text(encoding="utf-8")


ascii_manager = text("src/AsciiManager.cpp")
supervisor = text("src/Supervisor.cpp")
supervisor_h = text("src/Supervisor.hpp")
multiplayer = text("src/Multiplayer.hpp")

# R is owned by the Pause menu only. There must be no global gameplay hotkey
# that jumps directly to restart outside StageMenu::OnUpdateGameMenu().
pause = ascii_manager.split("i32 StageMenu::OnUpdateGameMenu()", 1)[1].split(
    "void StageMenu::OnDrawGameMenu()", 1
)[0]
assert "MultiplayerGameplay::IsMultiplayer()" in pause
assert "WAS_PRESSED(TH_BUTTON_R)" in pause
assert "GAME_MENU_PAUSE_SELECTED_RESTART" in pause
assert "SUPERVISOR_STATE_GAMEMANAGER_RESTART" in pause
assert "!g_GameManager.isInReplay" in pause

# The restart is a fresh attempt, not TH06's ordinary stage-reinit path. This
# mirrors TH07 state 10: Story returns to Stage 1, while Extra/Practice restart
# the current stage, then GameManager is registered while the dedicated restart
# state is still visible so fresh-run resource initialization executes.
assert "SUPERVISOR_STATE_GAMEMANAGER_RESTART" in supervisor_h
restart = supervisor.split("case SUPERVISOR_STATE_GAMEMANAGER_RESTART:", 1)[1].split(
    "case SUPERVISOR_STATE_MAINMENU_REPLAY:", 1
)[0]
assert "GameManager::CutChain();" in restart
assert "!g_GameManager.isInPracticeMode && g_GameManager.difficulty < EXTRA" in restart
assert "g_GameManager.currentStage = 0;" in restart
assert "g_GameManager.currentStage--;" in restart
assert "ReplayManager::SaveReplay(NULL, NULL);" in restart
assert "GameManager::RegisterChain()" in restart
assert "s->curState = SUPERVISOR_STATE_GAMEMANAGER;" in restart

# Old live peers must not mix with the new lifecycle, while ABI5 Replay remains
# readable because Pause-menu Restart is never part of replayed gameplay.
assert "TH06_MULTI_GAMEPLAY_ABI = 8;" in multiplayer

print("TH06 multiplayer Pause R Restart contract: PASS")
