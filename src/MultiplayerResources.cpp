#include "GameManager.hpp"

#include "Player.hpp"
#include "Supervisor.hpp"

#include <cstring>

// Keep P1 in the original TH06 GameManager fields so ordinary save/replay
// layout remains untouched. Only guest stable slots need sidecar resources.
MultiplayerPlayerResources
    g_MultiplayerPlayerResources[TH06_MULTI_MAX_GUESTS] = {
        {0, 0, 0}, {0, 0, 0}};
MultiplayerContributionStats
    g_MultiplayerContributionStats[TH06_MULTI_MAX_PLAYERS] = {
        {0, 0}, {0, 0}, {0, 0}};

namespace
{
MultiplayerPlayerResources *GetSidecarResources(u8 playerId)
{
    if (playerId == 0 || playerId >= TH06_MULTI_MAX_PLAYERS)
        return nullptr;
    return &g_MultiplayerPlayerResources[playerId - 1];
}
} // namespace

i32 GetPlayerLives(u8 playerId)
{
    MultiplayerPlayerResources *resources = GetSidecarResources(playerId);
    return resources ? resources->livesRemaining : g_GameManager.livesRemaining;
}

i32 GetPlayerBombs(u8 playerId)
{
    MultiplayerPlayerResources *resources = GetSidecarResources(playerId);
    return resources ? resources->bombsRemaining : g_GameManager.bombsRemaining;
}

i32 GetPlayerPower(u8 playerId)
{
    MultiplayerPlayerResources *resources = GetSidecarResources(playerId);
    return resources ? resources->currentPower : g_GameManager.currentPower;
}

u32 GetPlayerEnemiesDefeated(u8 playerId)
{
    if (playerId >= TH06_MULTI_MAX_PLAYERS)
        return 0;
    return g_MultiplayerContributionStats[playerId].enemiesDefeated;
}

u32 GetPlayerDamageDealt(u8 playerId)
{
    if (playerId >= TH06_MULTI_MAX_PLAYERS)
        return 0;
    return g_MultiplayerContributionStats[playerId].damageDealt;
}

void SetPlayerLives(u8 playerId, i32 amount)
{
    MultiplayerPlayerResources *resources = GetSidecarResources(playerId);
    if (resources)
        resources->livesRemaining = amount;
    else
        g_GameManager.livesRemaining = static_cast<i8>(amount);
}

void SetPlayerBombs(u8 playerId, i32 amount)
{
    MultiplayerPlayerResources *resources = GetSidecarResources(playerId);
    if (resources)
        resources->bombsRemaining = amount;
    else
        g_GameManager.bombsRemaining = static_cast<i8>(amount);
}

void SetPlayerPower(u8 playerId, i32 amount)
{
    MultiplayerPlayerResources *resources = GetSidecarResources(playerId);
    if (resources)
        resources->currentPower = amount;
    else
        g_GameManager.currentPower = static_cast<u16>(amount);
}

void AddPlayerLives(u8 playerId, i32 amount)
{
    SetPlayerLives(playerId, GetPlayerLives(playerId) + amount);
}

void AddPlayerPower(u8 playerId, i32 amount)
{
    SetPlayerPower(playerId, GetPlayerPower(playerId) + amount);
}

void AddPlayerEnemiesDefeated(u8 playerId, u32 amount)
{
    if (playerId >= TH06_MULTI_MAX_PLAYERS)
        return;
    MultiplayerContributionStats &stats = g_MultiplayerContributionStats[playerId];
    if (0xffffffffu - stats.enemiesDefeated < amount)
        stats.enemiesDefeated = 0xffffffffu;
    else
        stats.enemiesDefeated += amount;
}

void AddPlayerDamageDealt(u8 playerId, u32 amount)
{
    if (playerId >= TH06_MULTI_MAX_PLAYERS)
        return;
    MultiplayerContributionStats &stats = g_MultiplayerContributionStats[playerId];
    if (0xffffffffu - stats.damageDealt < amount)
        stats.damageDealt = 0xffffffffu;
    else
        stats.damageDealt += amount;
}

void ResetPlayerContributionStats()
{
    std::memset(g_MultiplayerContributionStats, 0,
                sizeof(g_MultiplayerContributionStats));
}

f32 GetMultiplayerBossDamageMultiplier()
{
    const i32 activeCount = GetActivePlayerCount();
    if (activeCount >= 3)
        return 2.0f / 3.0f;
    if (activeCount == 2)
        return 0.75f;
    return 1.0f;
}

i32 GetMultiplayerRankPenalty(i32 amount)
{
    const i32 activeCount = GetActivePlayerCount();
    return activeCount > 1 ? amount / activeCount : amount;
}

void ResetMultiplayerPlayerResources()
{
    // At a fresh gameplay registration every guest receives the same vanilla
    // starting resources as P1. Stage/practice setup may already have changed
    // P1 power, so copy the authoritative current P1 values rather than
    // reconstructing mode-specific rules here.
    for (MultiplayerPlayerResources &resources : g_MultiplayerPlayerResources)
    {
        resources.livesRemaining = g_GameManager.livesRemaining;
        resources.bombsRemaining = g_GameManager.bombsRemaining;
        resources.currentPower = g_GameManager.currentPower;
    }
}
