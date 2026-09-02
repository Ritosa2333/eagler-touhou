#include "multiplayer/GameplaySession.hpp"

#include <cassert>
#include <iostream>

using namespace MultiplayerGameplay;

int main()
{
    ResetToSinglePlayer(0, 1);
    assert(!IsMultiplayer());
    assert(GetPlayerCount() == 1);
    assert(GetLocalPlayerSlot() == 0);
    assert(GetPlayerCharacter(0) == 0);
    assert(GetPlayerShot(0) == 1);

    SessionState state;
    state.playerCount = 3;
    state.localPlayer = 2;
    state.showStagePlayerNames = true;
    state.players[0] = PlayerSlot{true, false, false, 0, 0};
    state.players[1] = PlayerSlot{true, false, false, 1, 1};
    state.players[2] = PlayerSlot{true, false, false, 0, 1};
    assert(Configure(state));
    assert(IsMultiplayer());
    assert(GetPlayerCount() == 3);
    assert(GetLocalPlayerSlot() == 2);
    assert(IsPlayerActive(0) && IsPlayerActive(1) && IsPlayerActive(2));
    assert(ShouldShowStagePlayerNames());
    assert(ShouldTintPlayer(2));

    SessionState invalid = state;
    invalid.players[1].character = 2; // TH07-only third-character value.
    assert(!Configure(invalid));
    invalid = state;
    invalid.players[1].shot = 2;
    assert(!Configure(invalid));

    SessionState departed = state;
    departed.players[1].permanentlyDeparted = true;
    departed.players[1].temporarilyAbsent = true;
    assert(Configure(departed));
    assert(!IsPlayerActive(1));
    assert(!IsPlayerTemporarilyAbsent(1));
    assert(IsPlayerPermanentlyDeparted(1));

    std::cout << "TH06 multiplayer gameplay session: PASS\n";
    return 0;
}
