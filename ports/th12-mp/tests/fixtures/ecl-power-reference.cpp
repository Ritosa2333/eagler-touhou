#include "../../src/game/EclInterpreter.hpp"
#include "../../src/game/GameSession.hpp"
#include "../../src/game/GameState.hpp"
#include "../../src/game/ItemPresentation.hpp"
#include <sstream>
#include <iomanip>
using namespace th12;
std::string result;
extern "C" {
void power_reference_begin(int difficulty, int seed, int initialPower, int initialDrop) {
  startGame(2, 1, difficulty, seed);
  power = initialPower; enemies.front()->drop = initialDrop;
  th12::random = {u16(seed), 0};
}
unsigned power_reference_int(int value) {
  power = value; globals[-9930] = {44, true};
  return u32(globalInt(*enemies.front(), -9930));
}
float power_reference_float(int value) {
  power = value; globals[-9930] = {44, true};
  return global(*enemies.front(), -9930, true);
}
void power_reference_tick(int value) {
  power = value;
  tickEnemies();
  enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const auto& e) { return !e->active; }), enemies.end());
  syncItemPresentation();
  tickItems(false);
}
const char* power_reference_snapshot() {
  std::ostringstream out; out << std::setprecision(9);
  out << "{\"power\":" << power << ",\"rng\":[" << th12::random.seed << ',' << th12::random.calls
      << "],\"enemies\":" << enemies.size() << ",\"items\":[";
  bool comma = false;
  for (size_t slot = 0; slot < itemManager.entities.size(); slot++) {
    const auto& e = itemManager.entities[slot]; if (!e.active()) continue;
    if (comma) out << ','; comma = true;
    out << "{\"slot\":" << slot << ",\"state\":" << int(e.state) << ",\"type\":" << e.type
        << ",\"position\":[" << e.position.x << ',' << e.position.y << "],\"velocity\":["
        << e.velocity.x << ',' << e.velocity.y << "],\"age\":" << e.age << '}';
  }
  out << "]}"; result = out.str(); return result.c_str();
}
}
