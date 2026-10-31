#include "../../src/game/Laser.hpp"
#include <sstream>
static std::string output;
extern "C" const char* moving_laser_prefix_clear(float offset, float radius) {
  using namespace th12::laser;
  Manager manager;
  World world;
  Parameters p;
  p.position = {94.4f, 144, 0};
  p.angle = 0;
  p.initialLength = p.maxLength = 96;
  p.travelDistance = 1000;
  p.width = 16;
  p.speed = 1;
  p.type = 7;
  p.color = 6;
  const int id = manager.spawn(Kind::Moving, p, world);
  auto* object = manager.find(id);
  object->state.headOffset = offset;
  const int removed = manager.clearCircle(world, p.position, radius, 1, true);
  std::ostringstream text;
  text.precision(9);
  const auto capture = [&] {
    const auto& s = object->state;
    text << "{\"phase\":" << int(s.phase) << ",\"position\":[" << s.position.x << ','
         << s.position.y << ',' << s.position.z << "],\"geometry\":[" << s.angle << ',' << s.length
         << ',' << s.width << ',' << s.speed << ',' << s.headOffset
         << "],\"maxLength\":" << s.parameters.maxLength
         << ",\"retirePending\":" << int(s.retirePending) << '}';
  };
  text << "{\"removed\":" << removed << ",\"afterClear\":";
  capture();
  manager.tick(world);
  text << ",\"afterTick\":";
  capture();
  text << '}';
  output = text.str();
  return output.c_str();
}
