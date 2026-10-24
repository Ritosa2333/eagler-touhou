#include "../../src/bullet.hpp"
#include <vector>
using namespace th12::eb;
struct CancelWorld : World {
  std::vector<int> events;
  void emit(const Emission&) override {}
  void unsupported(uint32_t) override {}
  bool ownsStateAnimation(const State&) const override { return true; }
  bool stateAnimation(State&, AnimationEvent event, int argument = 0) override {
    if (event == AnimationEvent::Interrupt)
      events.push_back(10 + argument);
    if (event == AnimationEvent::BulletTick)
      events.push_back(1);
    if (event == AnimationEvent::Retire)
      events.push_back(2);
    if (event == AnimationEvent::CancelEffect)
      events.push_back(100 + argument);
    return false;
  }
} world;
State state;
float output[8];
extern "C" {
void cancel_begin(int phase, float x, float y, float vx, float vy, int age) {
  world = {};
  state = {};
  state.phase = Phase(phase);
  state.x = x;
  state.y = y;
  state.vx = vx;
  state.vy = vy;
  state.age = state.collisionAge = age;
  state.type = 7;
  state.cancelEffect = -1;
  state.collisionDelay = 9;
  state.offscreenGrace = 10;
  state.interrupt = 0;
}
void cancel_scripted() { cancel(state, world); }
void cancel_tick() { world.events.clear(); tick(state, world); }
const float* cancel_state() {
  output[0] = int(state.phase);
  output[1] = state.x;
  output[2] = state.y;
  output[3] = state.age;
  output[4] = state.collisionDelay;
  output[5] = state.interrupt;
  output[6] = state.pendingRetire;
  output[7] = state.collisionAge;
  return output;
}
int cancel_events_count() { return int(world.events.size()); }
const int* cancel_events() { return world.events.data(); }
}
