#include "../../src/game/Laser.hpp"
#include <sstream>
namespace {std::string output;struct TestWorld:th12::laser::World{};}
extern "C" const char* timed_fragment_viewport(int axis,float origin){
 using namespace th12::laser;Manager manager;TestWorld world;Parameters p;
 p.position=axis?Vec3{0,origin,0}:Vec3{origin,128,0};
 p.angle=axis?1.5707963705062866f:0;p.initialLength=p.maxLength=128;p.width=16;
 p.delay=8;p.grow=16;p.hold=60;p.shrink=15;p.type=7;p.color=2;p.flags=2;p.lookupId=1;
 manager.spawn(Kind::Timed,p,world);
 const Vec3 center=axis?Vec3{0,p.position.y+32,0}:Vec3{p.position.x+32,128,0};
 const int removed=manager.clearCircle(world,center,20,0,true);
 std::ostringstream out;out.precision(9);out<<"{\"returned\":"<<removed<<",\"nodes\":[";bool comma=false;
 for(const auto* l:manager.ordered()){if(comma)out<<',';comma=true;const auto& s=l->state;
  out<<"{\"kind\":"<<int(s.kind)<<",\"position\":["<<s.position.x<<','<<s.position.y<<','<<s.position.z<<"],\"geometry\":["<<s.angle<<','<<s.length<<','<<s.width<<','<<s.speed<<"]}";
 }out<<"]}";output=out.str();return output.c_str();
}
