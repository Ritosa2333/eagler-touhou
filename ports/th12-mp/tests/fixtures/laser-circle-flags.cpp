#include "../../src/game/Laser.hpp"
#include <sstream>
namespace {
struct TestWorld : th12::laser::World {
  std::vector<th12::laser::Vec3> points;
  void pointItem(const th12::laser::Vec3& p,int,float,float) override { points.push_back(p); }
};
std::string output;
}
extern "C" const char* laser_circle_flags(int kind,float radius,unsigned flags) {
  using namespace th12::laser;
  Manager manager;TestWorld world;Parameters parameters;
  parameters.position={94.4f,144,0};parameters.initialLength=parameters.maxLength=96;
  parameters.travelDistance=1000;parameters.width=16;parameters.delay=8;parameters.grow=8;
  parameters.hold=140;parameters.shrink=10;parameters.nodeCount=8;parameters.flags=kind==1?11:1;
  parameters.type=7;parameters.color=6;parameters.lookupId=1;
  manager.spawn(Kind(kind),parameters,world);
  for(int tick=0;tick<8;tick++)manager.tick(world);
  const int returned=manager.clearCircle(world,{94.4f,144,0},radius,flags,true);
  std::ostringstream text;text.precision(9);text<<"{\"returned\":"<<returned<<",\"points\":[";
  bool comma=false;for(const auto& point:world.points){if(comma)text<<',';comma=true;text<<'['<<point.x<<','<<point.y<<']';}
  text<<"],\"nodes\":[";comma=false;
  for(const auto* node:manager.ordered()){if(comma)text<<',';comma=true;const auto& s=node->state;
    text<<"{\"kind\":"<<int(s.kind)<<",\"phase\":"<<int(s.phase)<<",\"position\":["<<s.position.x<<','<<s.position.y<<','<<s.position.z
        <<"],\"geometry\":["<<s.angle<<','<<s.length<<','<<s.width<<','<<s.speed<<"],\"retirePending\":"<<int(s.retirePending)<<'}';}
  text<<"]}";output=text.str();return output.c_str();
}
