#include "../../src/game/GameSession.hpp"
#include "../../src/game/GameState.hpp"
#include "../../src/game/LaserWorld.hpp"
#include <iomanip>
#include <sstream>
using namespace th12;
namespace {
struct FixtureWorld : laser::World {
  std::vector<laser::Vec3> points;
  float gameRate() const override { return laserWorld.gameRate(); }
  laser::Vec3 playerPosition() const override { return laserWorld.playerPosition(); }
  bool bossPosition(laser::Vec3& p) const override { return laserWorld.bossPosition(p); }
  laser::Collision lineCollision(const laser::Vec3& p, float a, float w, float l) override {
    return laserWorld.lineCollision(p, a, w, l);
  }
  void graze(const laser::Vec3& p) override { laserWorld.graze(p); }
  void updateAnimationGeometry(laser::Laser& o, laser::Role r,
                               const laser::AnimationGeometry& g) override {
    laserWorld.updateAnimationGeometry(o, r, g);
  }
  bool tickAnimation(laser::Laser& o, laser::Role r) override {
    return laserWorld.tickAnimation(o, r);
  }
  uint32_t next32() override { return laserWorld.next32(); }
  void spawnCancelEffect(const laser::Vec3& p, int s, float a, float w) override {
    laserWorld.spawnCancelEffect(p, s, a, w);
  }
  // Match the original sampler's passive Item allocation boundary: record
  // requests, while cancellation ANM and the real game RNG remain executable.
  void pointItem(const laser::Vec3& p, int, float, float) override { points.push_back(p); }
} world;
std::string result;
laser::Laser* object() { return laserManager.ordered().front(); }
void vector(std::ostream& out, const laser::Vec3& p) {
  out << '[' << p.x << ',' << p.y << ',' << p.z << ']';
}
} // namespace
extern "C" {
void curve_history_setup(int seed) {
  startGame(0, 0, 1, seed);
  laserManager.tick(laserWorld); // Same one initial Laser20 pass as Native.
  world.points.clear();
}
void curve_history_input(const float* input, int count) {
  auto& s = object()->state;
  s.nodes.clear();
  for (int i = 0; i < count; ++i)
    s.nodes.push_back({{input[i * 5], input[i * 5 + 1], input[i * 5 + 2]}, input[i * 5 + 3],
                       input[i * 5 + 4]});
  s.position = s.nodes.front().position; // Explicit synthetic input XYZ.
}
int curve_history_clear(int rectangle, unsigned convert, int force) {
  return rectangle ? object()->clearRectangle(laserManager, world, {0, 100, 0}, {32, 32, 0},
                                              convert != 0, force != 0)
                   : object()->clearCircle(laserManager, world, {0, 100, 0}, 16, convert,
                                           force != 0);
}
void curve_history_update() { object()->update(laserManager, world); }
void curve_history_primary() { animationScene.tickPrimary(); }
const char* curve_history_snapshot() {
  const auto& s = object()->state;
  std::ostringstream out;out << std::setprecision(9) << "{\"position\":";vector(out,s.position);
  out << ",\"velocity\":";vector(out,s.velocity);
  out << ",\"angleWidthSpeed\":[" << s.angle << ',' << s.length << ',' << s.width << ',' << s.speed
      << "],\"laserPhase\":" << int(s.phase) << ",\"retirePending\":" << int(s.retirePending)
      << ",\"protection\":" << s.protection << ",\"grazeAge\":[" << s.grazeAge.previous << ','
      << s.grazeAge.current << ',' << s.grazeAge.value << "],\"nodes\":[";
  bool comma=false;
  for(const auto& n:s.nodes){if(comma)out<<',';comma=true;
    out << '[' << n.position.x << ',' << n.position.y << ',' << n.position.z << ',' << n.angle
        << ',' << n.speed << ']';}
  out << "],\"gameSeedCounter\":[" << th12::random.seed << ',' << th12::random.calls << "],\"primaryEffects\":[";
  comma=false;
  for(const auto* node:animationScene.chain(anm_logic::Membership::Primary)){
    if(node->owner!=0)continue;
    const auto& a=node->vm.state();if(comma)out<<',';comma=true;
    // Native root factory stores viewport-adjusted engine XYZ. Attached
    // child engine XYZ remains relative; its parent's origin is separate.
    out << "{\"script\":" << a.script << ",\"clock\":" << a.clock << ",\"engine\":["
        << float(double(a.engineX)+(node->parent ? 0 : node->viewportX)) << ','
        << float(double(a.engineY)+(node->parent ? 0 : node->viewportY))
        << ',' << a.engineZ << "],\"rotation\":[" << a.rx << ',' << a.ry << ',' << a.rotation
        << "],\"layer\":" << a.layer << '}';}
  out << "],\"pointRequests\":[";comma=false;
  for(const auto& p:world.points){if(comma)out<<',';comma=true;vector(out,p);}
  out << "]}";result=out.str();return result.c_str();
}
}
