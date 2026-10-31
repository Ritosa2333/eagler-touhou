#include "player-death-resources.cpp"
#include "../../src/game/EclInterpreter.hpp"
extern "C" {
void phase_counter_begin(int loadout, int stock) {
  death_resource_begin(loadout, stock, 2);
  th12::Enemy enemy;
  th12::Context context;
  th12::storeInt(context, enemy, -9949, 0);
  th12::storeInt(context, enemy, -9948, 0);
  th12::storeInt(context, enemy, -9947, 1);
}
const unsigned* phase_counter_snapshot() {
  static unsigned row[3];
  th12::Enemy enemy;
  for (int i = 0; i < 3; ++i)
    row[i] = th12::globalInt(enemy, -9949 + i);
  return row;
}
}
