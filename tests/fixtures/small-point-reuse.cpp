#include "../../src/game/Items.hpp"
using namespace th12::item_system;
static Manager manager;
static Entity* item;
static FrameView frame;
static float output[9];
extern "C" {
void small_point_setup(int type, float stale) {
  manager.reset();
  const size_t slot = type == 9 ? Manager::ordinaryCapacity + Manager::tokenCapacity : 0;
  manager.entities[slot].attractionSpeed = stale;
  item = manager.spawn(type, {-180, 100}, -pi / 2, .6000000238418579f);
  frame = {};
  frame.player = PlayerView::native(0, {150, 400}, false, 1, PlayerView::PlayerPhase::Initialized);
}
void small_point_tick() {
  manager.tick(frame);
}
const float* small_point_state() {
  output[0] = int(item->state);
  output[1] = item->type;
  output[2] = item->position.x;
  output[3] = item->position.y;
  output[4] = item->velocity.x;
  output[5] = item->velocity.y;
  output[6] = item->age;
  output[7] = item->attractionSpeed;
  output[8] = item->delay;
  return output;
}
}
