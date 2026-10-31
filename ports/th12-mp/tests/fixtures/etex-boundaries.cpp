#include "../../src/bullet.hpp"
#include "../../src/game/AnmTypes.hpp"
using namespace th12::eb;
float output[3];
struct FixtureWorld:World {
  std::array<float,2> dimensions{16,16};
  th12::anm_logic::State animation;
  int unsupportedCount=0,blendCount=0,lastBlend=0;
  void emit(const Emission&)override{}
  void unsupported(uint32_t)override{++unsupportedCount;}
  std::array<float,2> spriteDimensions(const State&)const override{return dimensions;}
  void blendControl(State&,int value)override{
    ++blendCount;lastBlend=value;animation.blend=value;
    animation.flags=(animation.flags&~0xe0u)|(uint32_t(value)<<5);
  }
} world;
Program program;
State bullet;
float initialX=0,initialY=120,initialAngle=0,initialSpeed=0,stateOutput[13];
uint32_t blendOutput[5];
extern "C" {
const float* test_gate(int duration,float rate,float x,float y,float angle,float width,float height,int useGeometry){
  BulletGate gate;gate.begin(duration);
  output[0]=gate.tick({x,y,angle,width,height},useGeometry!=0,rate);
  output[1]=gate.remaining;output[2]=gate.value;return output;
}
void test_reset(float x,float y,float angle,float speed,float width,float height){
  program={};bullet={};world={};world.dimensions={width,height};
  initialX=x;initialY=y;initialAngle=angle;initialSpeed=speed;
}
void test_set(int slot,int concurrent,uint32_t kind,int c,int d,float a,float b){
  set(program,slot,concurrent!=0,kind,c,d,a,b);
}
void test_initialize(){initialize(bullet,1,program,3,0,initialX,initialY,initialAngle,initialSpeed,world);}
void test_tick(){tick(bullet,world);}
const float* test_state(){
  stateOutput[0]=bullet.x;stateOutput[1]=bullet.y;
  stateOutput[2]=bullet.vx;stateOutput[3]=bullet.vy;
  stateOutput[4]=bullet.speed;stateOutput[5]=bullet.angle;
  stateOutput[6]=bullet.active;stateOutput[7]=bullet.cursor;
  stateOutput[8]=bullet.gate.remaining;stateOutput[9]=bullet.gate.value;
  stateOutput[10]=bullet.wait.timer;stateOutput[11]=bullet.acceleration.timer;
  stateOutput[12]=world.unsupportedCount;return stateOutput;
}
const uint32_t* test_blend(uint32_t beforeFlags,int operandC,float rotation){
  program={};bullet={};world={};world.animation.flags=beforeFlags;world.animation.rotation=rotation;
  set(program,0,false,0x10000000,operandC,0,0,0);bullet.program=program;
  start(bullet,world);
  blendOutput[0]=world.animation.flags;blendOutput[1]=bullet.cursor;blendOutput[2]=bullet.active;
  blendOutput[3]=bullet.rotateToMotion;blendOutput[4]=th12::anm_logic::asWord(world.animation.rotation);
  return blendOutput;
}
}
