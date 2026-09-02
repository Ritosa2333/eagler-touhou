#pragma once

#include "Multiplayer.hpp"
#include "inttypes.hpp"

#include <array>

namespace MultiplayerGameplay
{
struct PlayerSlot
{
    bool active = false;
    bool temporarilyAbsent = false;
    bool permanentlyDeparted = false;
    u8 character = 0;
    u8 shot = 0;
};

struct SessionState
{
    u8 playerCount = 1;
    u8 localPlayer = 0;
    bool showStagePlayerNames = false;
    bool showContributionStats = true;
    std::array<PlayerSlot, TH06_MULTI_MAX_PLAYERS> players{};
};

// Gameplay-facing state only.  No transport, packet, RTT, room, or socket
// concepts belong in this API.
void ResetToSinglePlayer(u8 character = 0, u8 shot = 0);
bool Configure(const SessionState &state);
const SessionState &GetState();

bool IsMultiplayer();
u8 GetPlayerCount();
u8 GetLocalPlayerSlot();
bool IsPlayerActive(u8 playerId);
bool IsPlayerTemporarilyAbsent(u8 playerId);
bool IsPlayerPermanentlyDeparted(u8 playerId);
u8 GetPlayerCharacter(u8 playerId);
u8 GetPlayerShot(u8 playerId);
bool ShouldShowStagePlayerNames();
bool ShouldShowContributionStats();
const char *GetPlayerName(u8 playerId);
bool ShouldTintPlayer(u8 playerId);
bool ShouldForceContentUnlocks();
} // namespace MultiplayerGameplay
