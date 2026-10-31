#include "../../src/game/GameSession.hpp"
#include "../../src/game/GameState.hpp"
using namespace th12;
extern "C" {
void focus_prepare(int age, int restored, int count) {
  int initial[17]{};
  initial[1] = 51200;
  initial[2] = 100;
  initial[3] = initial[4] = 2;
  initial[15] = restored;
  startGame(0, 0, 1, 0x1234, initial);
  // Native43c6ea runs after initial placement. The harness prepares this
  // field explicitly before executing the actual production Player callback.
  playerFocused = restored != 0;
  if (!count)
    enemies.clear();
  frame = age;
  loadouts[0].speed = 4;
  loadouts[0].focus = 2;
  loadouts[0].diag = 362.f / 128;
  loadouts[0].focusDiag = 181.f / 128;
}
void focus_update(int held) { tickPlayer(held, 0); }
void focus_player_phase(int state, int ticks) { playerState = state; playerStateTicks = ticks; }
void focus_dialogue(int active) { dialogue = active; }
}
