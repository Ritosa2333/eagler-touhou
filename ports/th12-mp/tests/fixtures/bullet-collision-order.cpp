#include "../../src/bullet.hpp"
#include <vector>
using namespace th12::eb;
struct CollisionWorld : World {
  int ready = 0, misses = 0, grazes = 0, tails = 0;
  std::vector<float> queries;
  std::vector<int> events;
  void emit(const Emission&) override {}
  void unsupported(uint32_t) override {}
  int animationInteger(const State&, int, int) override { return ready; }
  bool ownsStateAnimation(const State&) const override { return true; }
  bool stateAnimation(State&, AnimationEvent event, int argument = 0) override {
    if (event == AnimationEvent::Interrupt) events.push_back(10 + argument);
    if (event == AnimationEvent::BulletTick) { ++tails; events.push_back(4); }
    return false;
  }
  Collision playerCollision(State& state) override {
    queries.push_back(state.x); queries.push_back(state.y); events.push_back(1);
    const auto result = collide(state, 0, 120, 2);
    if (result == Collision::Hit) { ++misses; events.push_back(2); }
    if (result == Collision::Graze && !state.grazed) { ++grazes; state.grazed = true; events.push_back(3); }
    return result;
  }
} world;
State state;
float output[8];
extern "C" {
void collision_run(int phase, float x, float vx, int age, int ready, int word4, int extension200) {
  world = {}; world.ready = ready; state = {};
  state.phase = Phase(phase); state.x = x; state.y = 120; state.vx = vx; state.age = age;
  state.type = 0; state.cancelEffect = -1; state.collisionDelay = word4; state.offscreenGrace = 10;
  if (extension200) set(state.program, 0, false, 0x200, 5, -999999, -999999, -999999);
  tick(state, world);
  output[0] = int(state.phase); output[1] = state.x; output[2] = state.y;
  output[3] = state.collisionDelay; output[4] = state.interrupt;
  output[5] = world.misses; output[6] = world.grazes; output[7] = world.tails;
}
const float* collision_result() { return output; }
const float* collision_queries() { return world.queries.data(); }
int collision_query_count() { return int(world.queries.size() / 2); }
const int* collision_events() { return world.events.data(); }
int collision_event_count() { return int(world.events.size()); }
}
