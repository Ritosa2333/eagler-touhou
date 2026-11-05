#include "../../src/game/Laser.hpp"
#include <array>
using namespace th12::laser;
namespace {
Manager manager;
struct JumpWorld:World {int missing=0;void unsupported(uint32_t)override{++missing;}} world;
std::array<float,15> output;
}
extern "C" {
void jump_begin(int kind,int mode){
 manager.reset();world.missing=0;
 Parameters p;p.position={0,128,0};p.angle=.2f;p.initialLength=p.maxLength=16;
 p.speed=.5f;p.width=16;p.nodeCount=8;p.delay=8;p.grow=3;p.hold=30;p.shrink=5;
 p.type=kind==2?0:17;p.color=4;p.flags=kind==1?2:0;
 if(mode==0){p.program.records[0]={0,-.0785398185f,20,-999999,8,false};p.program.records[1]={0,.0785398185f,20,-999999,8,false};p.program.records[2]={-999999,-999999,0,-999999,0x400000,false};}
 if(mode==1){p.program.records[0]={.01f,.3f,5,0,4,false};p.program.records[1]={-.01f,.3f,5,0,4,false};p.program.records[2]={0,0,0,0,0x400000,false};}
 if(mode==2){p.program.records[0]={0,0,3,0,0x400000,false};p.program.records[1]={0,0,7,0,0x200,false};p.program.records[3]={0,0,9,0,0x200,false};}
 manager.spawn(Kind(kind),p,world);
}
void jump_tick(){manager.tick(world);}
int jump_missing(){return world.missing;}
const float* jump_state(){auto list=manager.ordered();output.fill(0);output[0]=list.size();if(!list.empty()){const auto&s=list[0]->state;output[1]=s.cursor;output[2]=s.activeExtensions;output[3]=s.protection;output[4]=s.age.current;output[5]=s.position.x;output[6]=s.position.y;output[7]=s.position.z;output[8]=s.velocity.x;output[9]=s.velocity.y;output[10]=s.velocity.z;output[11]=s.angle;output[12]=s.length;output[13]=s.width;output[14]=s.speed;}return output.data();}
}
