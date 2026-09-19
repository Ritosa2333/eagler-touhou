#include "GameManager.hpp"

#include <string.h>

#include "GameErrorContext.hpp"
#include "Gui.hpp"
#include "Player.hpp"
#include "SoundPlayer.hpp"

// P1 remains in TH07's original integrity-checked globals. P2/P3 are kept in
// sidecar pools so multiplayer can have independent resources without
// changing the original save/replay structures.
MultiplayerPlayerResources
    g_MultiplayerPlayerResources[TH07_MULTI_MAX_GUESTS] = {
        {0, 0, 0}, {0, 0, 0}};
MultiplayerContributionStats
    g_MultiplayerContributionStats[TH07_MULTI_MAX_PLAYERS] = {
        {0, 0}, {0, 0}, {0, 0}};

static MultiplayerPlayerResources *GetSidecarResources(u8 playerId)
{
    if (playerId == 0 || playerId >= TH07_MULTI_MAX_PLAYERS)
    {
        return NULL;
    }
    return &g_MultiplayerPlayerResources[playerId - 1];
}

void GameManager::AddCherryPlusForPlayer(i32 amount, u8 playerId)
{
    (void)playerId;
    i32 oldCherry = g_GameManager.cherry;
    i32 borderThreshold = GetSharedBorderThreshold();

    g_GameManager.cherry += amount;
    if (g_GameManager.cherry > g_GameManager.cherryMax)
    {
        g_GameManager.cherry = g_GameManager.cherryMax;
    }
    // Cherry-plus is a single shared Shinra border gauge. The player id is
    // retained in this helper's API because item rewards are player-targeted,
    // but it must never create a second P2 gauge.
    if (0 < amount && !IsSharedBorderActive())
    {
        g_GameManager.cherryPlus += amount;
        if (g_GameManager.cherryPlus >=
            g_GameManager.globals->cherryStart + borderThreshold)
        {
            g_GameManager.cherryPlus =
                g_GameManager.globals->cherryStart + borderThreshold;
            ActivateSharedBorder();
        }
    }
    if (g_GameManager.cherry >= g_GameManager.cherryMax &&
        oldCherry != g_GameManager.cherry)
    {
        g_Gui.ShowStatusPopup(
            g_GameManager.cherry - g_GameManager.globals->cherryStart, 3);
    }
}

i32 GetSharedBorderThreshold()
{
    // Two-player mode retains TH07's 50,000 gauge. The third player raises
    // collection throughput by roughly 50%, so 3P uses 75,000. A contracted
    // one-player multiplayer run returns to the original 50,000 threshold.
    return GetActivePlayerCount() >= 3 ? 75000 : 50000;
}

f32 GetMultiplayerBossDamageMultiplier()
{
    i32 activeCount = GetActivePlayerCount();
    if (activeCount >= 3)
    {
        return 2.0f / 3.0f;
    }
    if (activeCount == 2)
    {
        return 0.75f;
    }
    return 1.0f;
}

f32 GetMultiplayerBombDamageMultiplier()
{
    // Three simultaneous bomb invulnerability windows are much stronger than
    // the two-player case.  Scale only bomb hitbox damage; normal shots keep
    // their full value and the existing boss multiplier remains separate.
    return GetActivePlayerCount() >= 3 ? 2.0f / 3.0f : 1.0f;
}

i32 GetMultiplayerRankPenalty(i32 amount)
{
    // Rank is one shared value, but deaths and bombs are per player, so three
    // players feed it three times as many penalties as the difficulty curve
    // was tuned for and the patterns thin out. Dividing each penalty by the
    // player count restores the single-player rate of decay: the same amount
    // of rank is lost per unit of play, no matter how many ships are losing
    // it. What goes up - the timed IncreaseSubrank - is already shared, so it
    // is left alone.
    i32 activeCount = GetActivePlayerCount();
    if (activeCount <= 1)
    {
        return amount;
    }
    return amount / activeCount;
}

void ApplyActivePlayerCountParameters(i32 previousCount, i32 newCount)
{
    i32 cherryRange;
    i32 newThreshold;

    if (!g_GameManager.globals || previousCount == newCount)
    {
        return;
    }
    cherryRange = g_GameManager.cherryMax -
        g_GameManager.globals->cherryStart;
    if (cherryRange < 0)
    {
        cherryRange = 0;
    }
    if (previousCount < 3 && newCount >= 3)
    {
        cherryRange = cherryRange * 3 / 2;
    }
    else if (previousCount >= 3 && newCount < 3)
    {
        cherryRange = cherryRange * 2 / 3;
    }
    g_GameManager.cherryMax =
        g_GameManager.globals->cherryStart + cherryRange;
    if (g_GameManager.cherry > g_GameManager.cherryMax)
    {
        g_GameManager.cherry = g_GameManager.cherryMax;
    }

    newThreshold = newCount >= 3 ? 75000 : 50000;
    if (newCount < previousCount &&
        g_GameManager.cherryPlus >=
            g_GameManager.globals->cherryStart + newThreshold)
    {
        // A disconnect must not trigger a border merely because the threshold
        // became lower at the contraction frame.
        g_GameManager.cherryPlus =
            g_GameManager.globals->cherryStart + newThreshold - 1;
    }
}

i32 GetPlayerLives(u8 playerId)
{
    MultiplayerPlayerResources *resources = GetSidecarResources(playerId);
    return resources ? resources->livesRemaining
                     : (i32)g_GameManager.globals->livesRemaining;
}

i32 GetPlayerBombs(u8 playerId)
{
    MultiplayerPlayerResources *resources = GetSidecarResources(playerId);
    return resources ? resources->bombsRemaining
                     : (i32)g_GameManager.globals->bombsRemaining;
}

i32 GetPlayerPower(u8 playerId)
{
    MultiplayerPlayerResources *resources = GetSidecarResources(playerId);
    return resources ? resources->currentPower
                     : (i32)g_GameManager.globals->currentPower;
}

i32 GetPlayerCherryPlus(u8 playerId)
{
    (void)playerId;
    return g_GameManager.cherryPlus;
}

u32 GetPlayerEnemiesDefeated(u8 playerId)
{
    if (playerId >= TH07_MULTI_MAX_PLAYERS)
    {
        return 0;
    }
    return g_MultiplayerContributionStats[playerId].enemiesDefeated;
}

u32 GetPlayerDamageDealt(u8 playerId)
{
    if (playerId >= TH07_MULTI_MAX_PLAYERS)
    {
        return 0;
    }
    return g_MultiplayerContributionStats[playerId].damageDealt;
}

void AddPlayerEnemiesDefeated(u8 playerId, u32 amount)
{
    MultiplayerContributionStats *stats;
    if (playerId >= TH07_MULTI_MAX_PLAYERS)
    {
        return;
    }
    stats = &g_MultiplayerContributionStats[playerId];
    if (0xffffffffu - stats->enemiesDefeated < amount)
    {
        stats->enemiesDefeated = 0xffffffffu;
    }
    else
    {
        stats->enemiesDefeated += amount;
    }
}

void AddPlayerDamageDealt(u8 playerId, u32 amount)
{
    MultiplayerContributionStats *stats;
    if (playerId >= TH07_MULTI_MAX_PLAYERS)
    {
        return;
    }
    stats = &g_MultiplayerContributionStats[playerId];
    if (0xffffffffu - stats->damageDealt < amount)
    {
        stats->damageDealt = 0xffffffffu;
    }
    else
    {
        stats->damageDealt += amount;
    }
}

void ResetPlayerContributionStats()
{
    memset(g_MultiplayerContributionStats, 0,
           sizeof(g_MultiplayerContributionStats));
}

void SetPlayerLives(u8 playerId, i32 amount)
{
    MultiplayerPlayerResources *resources = GetSidecarResources(playerId);
    if (!resources)
    {
        g_GameManager.SetLivesRemaining(amount);
    }
    else
    {
        resources->livesRemaining = amount;
    }
}

void SetPlayerBombs(u8 playerId, i32 amount)
{
    MultiplayerPlayerResources *resources = GetSidecarResources(playerId);
    if (!resources)
    {
        g_GameManager.SetBombsRemainingAndComputeCsum(amount);
    }
    else
    {
        resources->bombsRemaining = amount;
    }
}

void SetPlayerPower(u8 playerId, i32 amount)
{
    MultiplayerPlayerResources *resources = GetSidecarResources(playerId);
    if (!resources)
    {
        g_GameManager.SetCurrentPower(amount);
        g_GameManager.RegenerateGameIntegrityCsum();
    }
    else
    {
        resources->currentPower = amount;
    }
}

void SetPlayerCherryPlus(u8 playerId, i32 amount)
{
    (void)playerId;
    g_GameManager.cherryPlus = amount;
}

void AddPlayerLives(u8 playerId, i32 amount)
{
    MultiplayerPlayerResources *resources = GetSidecarResources(playerId);
    if (!resources)
    {
        g_GameManager.AddLivesRemaining(amount);
    }
    else
    {
        resources->livesRemaining += amount;
    }
}

void AddPlayerBombs(u8 playerId, i32 amount)
{
    MultiplayerPlayerResources *resources = GetSidecarResources(playerId);
    if (!resources)
    {
        g_GameManager.AddBombsRemaining(amount);
    }
    else
    {
        resources->bombsRemaining += amount;
    }
}

void AddPlayerPower(u8 playerId, i32 amount)
{
    MultiplayerPlayerResources *resources = GetSidecarResources(playerId);
    if (!resources)
    {
        g_GameManager.AddCurrentPower(amount);
    }
    else
    {
        resources->currentPower += amount;
    }
}

void AddPlayerCherryPlus(u8 playerId, i32 amount)
{
    (void)playerId;
    g_GameManager.cherryPlus += amount;
}

void ResetMultiplayerPlayerResources(u8 playerId)
{
    MultiplayerPlayerResources *resources = GetSidecarResources(playerId);
    Player *player;
    if (!resources)
    {
        return;
    }
    player = &g_Players[playerId];
    resources->livesRemaining = g_GameManager.defaultCfg
        ? g_GameManager.defaultCfg->lifeCount : 0;
    resources->bombsRemaining = player->shooterData
        ? (i32)player->shooterData->initialBombs : 0;
    resources->currentPower = 0;
}

void ResetPlayer2Resources()
{
    ResetMultiplayerPlayerResources(1);
}

// Grants one life, or a bomb once lives are capped, without the sound and the
// subrank increase. Those belong to the award as a whole: an award that covers
// several players must not raise subrank several times, because subrank drives
// rank and that would quietly make multiplayer runs harder.
static bool GrantPlayerExtend(u8 playerId)
{
    MultiplayerPlayerResources *resources = GetSidecarResources(playerId);
    if (!resources)
    {
        // P1 stays in the integrity-checked globals, so it has to go through
        // the accessors that maintain the checksum.
        if ((i32)g_GameManager.globals->livesRemaining < 8)
        {
            g_GameManager.AddLivesRemaining(1);
            g_Gui.lifeDisplayUpdateFrames = 2;
            return true;
        }
        if ((i32)g_GameManager.globals->bombsRemaining < 8)
        {
            g_GameManager.AddBombsRemaining(1);
            g_Gui.bombDisplayUpdateFrames = 2;
            return true;
        }
        return false;
    }
    if (resources->livesRemaining < 8)
    {
        resources->livesRemaining++;
        g_Gui.lifeDisplayUpdateFrames = 2;
        return true;
    }
    if (resources->bombsRemaining < 8)
    {
        resources->bombsRemaining++;
        g_Gui.bombDisplayUpdateFrames = 2;
        return true;
    }
    return false;
}

void ExtendPlayerFromItem(u8 playerId)
{
    if (GrantPlayerExtend(playerId))
    {
        g_SoundPlayer.PlaySoundByIdx(SOUND_EXTEND, 0);
        g_GameManager.IncreaseSubrank(200);
    }
}

// Point-item extends are earned from a counter that every player feeds:
// ItemManager increments pointItemsCollectedForExtend whoever picked the item
// up. The reward used to land on P1 unconditionally, because the threshold
// path called GameManager::ExtendFromPoints, which only knows about P1's
// globals. Every active player receives the extend instead.
void ExtendAllPlayersFromPoints()
{
    i32 playerId;
    bool awarded = false;

    for (playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; playerId++)
    {
        if (!IsPlayerSlotActive((u8)playerId))
        {
            continue;
        }
        if (GrantPlayerExtend((u8)playerId))
        {
            awarded = true;
        }
    }
    if (awarded)
    {
        g_SoundPlayer.PlaySoundByIdx(SOUND_EXTEND, 0);
        g_GameManager.IncreaseSubrank(200);
    }
    // Reported because nothing else makes this observable. The reward used to
    // land on P1 whoever collected the points, and no test or log would have
    // shown it.
    g_GameErrorContext.Log(
        "info : point extend awarded P1 %d P2 %d P3 %d\r\n",
        GetPlayerLives(0), GetPlayerLives(1), GetPlayerLives(2));
}
