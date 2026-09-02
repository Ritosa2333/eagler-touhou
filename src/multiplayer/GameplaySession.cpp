#include "GameplaySession.hpp"

namespace MultiplayerGameplay
{
namespace
{
SessionState MakeDefaultState(u8 character, u8 shot)
{
    SessionState state;
    state.playerCount = 1;
    state.localPlayer = 0;
    state.players[0].active = true;
    state.players[0].character = character;
    state.players[0].shot = shot;
    return state;
}

SessionState g_State = MakeDefaultState(0, 0);
}

void ResetToSinglePlayer(u8 character, u8 shot)
{
    g_State = MakeDefaultState(character, shot);
}

bool Configure(const SessionState &state)
{
    if (state.playerCount < 1 || state.playerCount > TH06_MULTI_MAX_PLAYERS ||
        state.localPlayer >= state.playerCount)
    {
        return false;
    }

    SessionState normalized = state;
    for (u8 playerId = 0; playerId < TH06_MULTI_MAX_PLAYERS; ++playerId)
    {
        PlayerSlot &slot = normalized.players[playerId];
        if (playerId >= normalized.playerCount)
        {
            slot = {};
            continue;
        }
        // TH06 has exactly Reimu/Marisa and shot A/B.  Do not import TH07's
        // third character or any Cherry/Border/Spirit gameplay semantics.
        if (slot.character > 1 || slot.shot > 1)
        {
            return false;
        }
        if (slot.permanentlyDeparted)
        {
            slot.active = false;
            slot.temporarilyAbsent = false;
        }
    }
    // P1 is the original TH06 slot and must exist for a valid gameplay
    // session.  Guest lifecycle changes may disable only guest slots.
    normalized.players[0].active = true;
    normalized.players[0].permanentlyDeparted = false;
    g_State = normalized;
    return true;
}

const SessionState &GetState()
{
    return g_State;
}

bool IsMultiplayer()
{
    return g_State.playerCount > 1;
}

u8 GetPlayerCount()
{
    return g_State.playerCount;
}

u8 GetLocalPlayerSlot()
{
    return g_State.localPlayer;
}

bool IsPlayerActive(u8 playerId)
{
    return playerId < g_State.playerCount &&
           g_State.players[playerId].active &&
           !g_State.players[playerId].permanentlyDeparted;
}

bool IsPlayerTemporarilyAbsent(u8 playerId)
{
    return playerId < g_State.playerCount &&
           g_State.players[playerId].temporarilyAbsent;
}

bool IsPlayerPermanentlyDeparted(u8 playerId)
{
    return playerId >= g_State.playerCount ||
           g_State.players[playerId].permanentlyDeparted;
}

u8 GetPlayerCharacter(u8 playerId)
{
    return playerId < g_State.playerCount ? g_State.players[playerId].character : 0;
}

u8 GetPlayerShot(u8 playerId)
{
    return playerId < g_State.playerCount ? g_State.players[playerId].shot : 0;
}

bool ShouldShowStagePlayerNames()
{
    return IsMultiplayer() && g_State.showStagePlayerNames;
}

bool ShouldShowContributionStats()
{
    return IsMultiplayer() && g_State.showContributionStats;
}

const char *GetPlayerName(u8 playerId)
{
    static const char *const defaultNames[TH06_MULTI_MAX_PLAYERS] = {
        "Player", "Player2", "Player3"};
    return playerId < TH06_MULTI_MAX_PLAYERS ? defaultNames[playerId]
                                             : "Player";
}

bool ShouldTintPlayer(u8 playerId)
{
    if (!IsMultiplayer() || playerId == 0 || playerId >= g_State.playerCount)
    {
        return false;
    }
    return GetPlayerCharacter(playerId) == GetPlayerCharacter(0);
}

bool ShouldForceContentUnlocks()
{
    // Multiplayer rooms must not depend on one peer's local score.dat unlock
    // state. This is runtime-only and never mutates the original unlock data.
    return IsMultiplayer();
}
} // namespace MultiplayerGameplay
