#include "../../src/game/Laser.hpp"
#include <cmath>
using namespace th12::laser;
struct Probe : Laser {
  void initialize(World&) override {}
  bool update(Manager& manager,World& world) override {return extensions(manager,world);}
} probe;
struct ProbeWorld : World {float rate=1;float gameRate()const override{return rate;}Vec3 playerPosition()const override{return {22,200,0};}} world;
Manager manager;float result[8];
extern "C" {
void begin(int kind,int extension,int duration,float a,float b,float rate){probe.state={};auto& s=probe.state;s.kind=Kind(kind);s.phase=Phase::Active;s.position={12,100,0};s.angle=.33f;s.speed=2.5f;s.velocity={float(std::cos(double(s.angle))*s.speed),float(std::sin(double(s.angle))*s.speed),0};s.parameters.program.records[0]={a,b,duration,0,uint32_t(extension),0};world.rate=rate;}
void tick(){probe.update(manager,world);}
const float* snapshot(int extension){auto& s=probe.state;auto& m=extension==4?s.scalarAcceleration:s.polarAcceleration;result[0]=s.speed;result[1]=s.angle;result[2]=s.velocity.x;result[3]=s.velocity.y;result[4]=s.velocity.z;result[5]=s.activeExtensions;result[6]=m.timer.current;result[7]=m.timer.value;return result;}
}
