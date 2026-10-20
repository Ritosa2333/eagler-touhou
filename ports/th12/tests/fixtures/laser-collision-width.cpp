#include "../../src/game/Laser.hpp"
#include "../../src/game/LaserCollision.hpp"
#include <array>
using namespace th12::laser;
namespace {
Manager manager;
struct CollisionWidthWorld : World {
  PlayerCollision player;
  std::vector<std::array<float, 7>> queries;
  int grazes = 0;
  Vec3 playerPosition() const override { return player.position; }
  Collision lineCollision(const Vec3& origin, float angle, float transverse, float length) override {
    const auto result = collidePlayerLine(origin, angle, transverse, length, player);
    queries.push_back({origin.x, origin.y, origin.z, angle, transverse, length, float(result.code)});
    return result.code;
  }
  void graze(const Vec3&) override { ++grazes; }
} world;
}
extern "C" {
void width_begin(int kind, float x, float y, float angle, float length, float speed, float width,
                 float playerX, float playerY, float playerWidth) {
  manager.reset();
  world.queries.clear();
  world.grazes = 0;
  world.player = {};
  world.player.position = {playerX, playerY, 0};
  world.player.hitWidth = world.player.hitHeight = playerWidth;
  world.player.state = 1;
  world.player.invulnerability = 1;
  Parameters p;
  p.position = {x, y, 0};
  p.angle = angle;
  p.initialLength = p.maxLength = length;
  p.speed = speed;
  p.width = width;
  p.type = kind == 2 ? 0 : 17;
  p.color = 4;
  p.nodeCount = 8;
  p.delay = 8;
  p.grow = 3;
  p.hold = 1000;
  p.shrink = 5;
  p.flags = kind == 1 ? 2 : 0;
  manager.spawn(Kind(kind), p, world);
  auto& state = manager.ordered().front()->state;
  state.phase = Phase::Active;
  state.width = width;
  state.age.reset(18);
}
void width_tick() { manager.tick(world); }
int width_query_count() { return int(world.queries.size()); }
const float* width_query(int index) { return world.queries[size_t(index)].data(); }
int width_grazes() { return world.grazes; }
}
