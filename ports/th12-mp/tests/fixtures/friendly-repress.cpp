#include "../../src/game/FriendlyPool.hpp"
#include "../../src/game/GameSession.hpp"
#include "../../src/game/GameState.hpp"
using namespace th12;
namespace {
std::string snapshot;
}
extern "C" {
void friendly_repress_prepare(int loadout, int x, int y, int stockPower) {
  const int initial[]{x, y, stockPower, 2, 2, 0, 1000000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  startGame(loadout / 2, loadout % 2, 1, 4796, initial);
}
const char* friendly_repress_tick(int held) {
  tickPlayer(held, 0);
  std::ostringstream out;
  out << std::setprecision(9) << "{\"playerFixed\":[" << px << ',' << py
      << "],\"shootAge\":" << shotSchedule.frame << ",\"options\":[";
  for (int i = 0; i < playerMotion.count; ++i) {
    if (i)
      out << ',';
    out << '[' << playerMotion.options[i].x << ',' << playerMotion.options[i].y << ']';
  }
  out << "],\"friendly\":[";
  bool comma = false;
  for (int slot = 0; slot < FriendlyCapacity; ++slot) {
    const auto* b = friendlySlots[slot];
    if (!b || !b->active)
      continue;
    if (comma)
      out << ',';
    comma = true;
    out << '[' << slot << ',' << int(b->weapon.phase) << ',' << b->age << ',' << b->x << ',' << b->y
        << ',' << b->type << ',' << b->option << ',' << b->damage << ',' << b->hitWidth << ','
        << b->hitHeight << ']';
  }
  out << "]}";
  snapshot = out.str();
  animationScene.tickPrimary();
  animationScene.collect();
  ++frame;
  return snapshot.c_str();
}
int friendly_repress_size() {
  return int(snapshot.size());
}
}
