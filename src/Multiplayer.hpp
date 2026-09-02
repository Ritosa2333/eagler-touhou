#pragma once

// Gameplay-only multiplayer constants shared by the TH06 multiplayer build.
//
// Keep this header deliberately free of transport/session/rollback types.  The
// original game owners may depend on a fixed player-slot count, but networking
// remains isolated under src/netplay/ so the gameplay feature can be exercised
// locally without pulling a socket implementation into Player/GameManager.
constexpr int TH06_MULTI_MAX_PLAYERS = 3;
constexpr int TH06_MULTI_MAX_GUESTS = TH06_MULTI_MAX_PLAYERS - 1;
// Bump only when multiplayer gameplay semantics/state ownership changes in a
// way that can affect deterministic netplay or Replay evolution. Netplay and
// EAGX Replay must share this owner so their compatibility labels cannot drift.
constexpr unsigned int TH06_MULTI_GAMEPLAY_ABI = 8;
