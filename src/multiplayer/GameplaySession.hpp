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
    bool challengeMode = false;
    // Host-authored deterministic gameplay option. The browser room
    // descriptor must provide the same value to every peer before frame zero.
    // It is not a compatibility/hash field and defaults to upstream's OFF.
    bool stage4BossChain = false;
    // Presentation-only local preference. It is never negotiated or used as
    // a room/session compatibility field.
    bool showContributionStats = true;
    std::array<PlayerSlot, TH07_MULTI_MAX_PLAYERS> players{};
};

// Gameplay-facing state only.  No transport, packet, RTT, room, or socket
// concepts belong in this API.
void ResetToSinglePlayer(u8 character = 0, u8 shot = 0);
bool Configure(const SessionState &state);
const SessionState &GetState();

bool IsMultiplayer();
bool IsChallengeMode();
u8 GetPlayerCount();
u8 GetLocalPlayerSlot();
bool IsPlayerActive(u8 playerId);
bool IsPlayerTemporarilyAbsent(u8 playerId);
bool IsPlayerPermanentlyDeparted(u8 playerId);
u8 GetPlayerCharacter(u8 playerId);
u8 GetPlayerShot(u8 playerId);
bool ShouldShowStagePlayerNames();
bool IsStage4BossChainEnabled();
bool ShouldShowContributionStats();
const char *GetPlayerName(u8 playerId);
bool ShouldTintPlayer(u8 playerId);
bool ShouldForceContentUnlocks();
} // namespace MultiplayerGameplay
