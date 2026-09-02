#include "GameManager.hpp"
#include "Supervisor.hpp"

#include <cassert>
#include <cstdio>

GameManager g_GameManager;
Supervisor g_Supervisor;
GameManager::GameManager() = default;
i32 GetActivePlayerCount() { return 1; }

int main()
{
    g_GameManager.livesRemaining = 2;
    g_GameManager.bombsRemaining = 3;
    g_GameManager.currentPower = 64;
    ResetMultiplayerPlayerResources();

    assert(GetPlayerLives(0) == 2 && GetPlayerLives(1) == 2 && GetPlayerLives(2) == 2);
    assert(GetPlayerBombs(0) == 3 && GetPlayerBombs(1) == 3 && GetPlayerBombs(2) == 3);
    assert(GetPlayerPower(0) == 64 && GetPlayerPower(1) == 64 && GetPlayerPower(2) == 64);

    SetPlayerLives(1, 1);
    SetPlayerBombs(2, 0);
    SetPlayerPower(1, 96);
    assert(g_GameManager.livesRemaining == 2);
    assert(g_GameManager.bombsRemaining == 3);
    assert(g_GameManager.currentPower == 64);
    assert(GetPlayerLives(1) == 1);
    assert(GetPlayerBombs(2) == 0);
    assert(GetPlayerPower(1) == 96);

    SetPlayerLives(0, 4);
    SetPlayerBombs(0, 5);
    SetPlayerPower(0, 128);
    assert(g_GameManager.livesRemaining == 4);
    assert(g_GameManager.bombsRemaining == 5);
    assert(g_GameManager.currentPower == 128);

    ResetPlayerContributionStats();
    AddPlayerEnemiesDefeated(0, 3);
    AddPlayerEnemiesDefeated(2, 7);
    AddPlayerDamageDealt(0, 1200);
    AddPlayerDamageDealt(1, 900);
    assert(GetPlayerEnemiesDefeated(0) == 3);
    assert(GetPlayerEnemiesDefeated(1) == 0);
    assert(GetPlayerEnemiesDefeated(2) == 7);
    assert(GetPlayerDamageDealt(0) == 1200);
    assert(GetPlayerDamageDealt(1) == 900);
    assert(GetPlayerDamageDealt(2) == 0);

    AddPlayerEnemiesDefeated(0, 0xffffffffu);
    AddPlayerDamageDealt(1, 0xffffffffu);
    assert(GetPlayerEnemiesDefeated(0) == 0xffffffffu);
    assert(GetPlayerDamageDealt(1) == 0xffffffffu);
    AddPlayerEnemiesDefeated(TH06_MULTI_MAX_PLAYERS, 1);
    AddPlayerDamageDealt(TH06_MULTI_MAX_PLAYERS, 1);
    assert(GetPlayerEnemiesDefeated(TH06_MULTI_MAX_PLAYERS) == 0);
    assert(GetPlayerDamageDealt(TH06_MULTI_MAX_PLAYERS) == 0);

    ResetPlayerContributionStats();
    for (u8 playerId = 0; playerId < TH06_MULTI_MAX_PLAYERS; ++playerId)
    {
        assert(GetPlayerEnemiesDefeated(playerId) == 0);
        assert(GetPlayerDamageDealt(playerId) == 0);
    }

    std::puts("TH06 multiplayer resources: PASS");
}
