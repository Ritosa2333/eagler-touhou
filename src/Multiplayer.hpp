#pragma once

// Gameplay-only multiplayer constants imported from sbrik1111/th07_multi_player.
//
// Keep this header deliberately free of transport/session/rollback types.  The
// original game owners may depend on a fixed player-slot count, but networking
// remains isolated under src/netplay/ so the gameplay feature can be exercised
// locally without pulling a socket implementation into Player/GameManager.
constexpr int TH07_MULTI_MAX_PLAYERS = 3;
constexpr int TH07_MULTI_MAX_GUESTS = TH07_MULTI_MAX_PLAYERS - 1;
// Gameplay ABI records challenge deaths and shares authored Bomb invulnerability.
// Old multiplayer Replays must be rejected rather than replayed with new rules.
constexpr unsigned int TH07_MULTI_GAMEPLAY_ABI = 11;
// Netplay ABI binds the current gameplay and challenge-mode contract.
// Ordinary single-player builds and Replay formats are unchanged.
constexpr unsigned int TH07_MULTI_NETPLAY_ABI = 12;
