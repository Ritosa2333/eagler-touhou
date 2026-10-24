#include "../../src/game/Laser.hpp"
#include <iomanip>
#include <sstream>
using namespace th12::laser;
namespace {
Manager manager;
std::string result;
struct EmissionWorld final : World {
  std::vector<th12::eb::Emission> emissions;
  int missing = 0;
  void emitBullet(const th12::eb::Emission& e) override { emissions.push_back(e); }
  void unsupported(uint32_t) override { ++missing; }
} world;
} // namespace
extern "C" {
void emission_begin(int kind, const float* values, const int32_t* words) {
  manager.reset();
  world.emissions.clear();
  world.missing = 0;
  Parameters p;
  p.position = {20, 128, 0};
  p.angle = .7f;
  p.speed = 2;
  p.width = 8;
  p.initialLength = p.maxLength = 96;
  p.type = kind == 2 ? 0 : 17;
  p.color = 4;
  p.delay = 8;
  p.grow = 3;
  p.hold = 30;
  p.shrink = 5;
  p.nodeCount = 8;
  p.flags = kind == 1 ? 2 : 0;
  p.program.records[0] = {values[0], 1, words[0], 3, 0x80000, false};
  p.program.records[1] = {values[1], .2f, words[1], 0, 0x100000, false};
  manager.spawn(Kind(kind), p, world);
}
void emission_tick() {
  manager.tick(world);
}
const char* emission_snapshot() {
  std::ostringstream out;
  out << std::setprecision(9) << "{\"missing\":" << world.missing << ",\"emissions\":[";
  bool comma = false;
  for (const auto& e : world.emissions) {
    if (comma)
      out << ',';
    comma = true;
    out << '[' << e.type << ',' << e.color << ',' << e.x << ',' << e.y << ',' << e.angle << ','
        << e.spacing << ',' << e.speed << ',' << e.slow << ',' << e.count << ',' << e.layers << ','
        << e.mode << ',' << e.program.first << ',' << e.program.flags << ','
        << e.program.transformSound << ']';
  }
  out << "],\"nodes\":[";
  comma = false;
  for (const auto* o : manager.ordered()) {
    if (comma)
      out << ',';
    comma = true;
    out << '[' << o->state.position.x << ',' << o->state.position.y << ',' << int(o->state.phase)
        << ',' << o->state.cursor << ']';
  }
  out << "]}";
  result = out.str();
  return result.c_str();
}
}
