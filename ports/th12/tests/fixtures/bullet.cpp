#include "../../src/bullet.hpp"
#include <vector>
using namespace th12::eb;
struct FixtureWorld:World {std::vector<Emission> emissions;std::vector<uint32_t> missing;void emit(const Emission& e)override{emissions.push_back(e);}void unsupported(uint32_t k)override{missing.push_back(k);}} world;
State bullet;Program program;float initialX=0,initialY=120,initialAngle=0,initialSpeed=3;int initialType=0;
float snapshot[18],emission[13];
extern "C" {
void test_reset(float x,float y,float angle,float speed,int type){world.emissions.clear();world.missing.clear();program={};bullet={};initialX=x;initialY=y;initialAngle=angle;initialSpeed=speed;initialType=type;}
int test_set(int slot,int concurrent,uint32_t kind,int c,int d,float a,float b){return set(program,slot,concurrent,kind,c,d,a,b);}
void test_first(int i){program.first=i;}
void test_initialize(){initialize(bullet,1,program,initialType,0,initialX,initialY,initialAngle,initialSpeed,world);}
void test_tick(){tick(bullet,world);}
void test_cancel(){cancel(bullet);}
const float* test_state(){snapshot[0]=bullet.x;snapshot[1]=bullet.y;snapshot[2]=bullet.vx;snapshot[3]=bullet.vy;snapshot[4]=bullet.speed;snapshot[5]=bullet.angle;snapshot[6]=bullet.cursor;snapshot[7]=int(bullet.phase);snapshot[8]=bullet.age;snapshot[9]=bullet.visualAge;snapshot[10]=bullet.animation;snapshot[11]=bullet.interrupt;snapshot[12]=bullet.spawnDuration;snapshot[13]=bullet.collisionDelay;snapshot[14]=bullet.offscreenGrace;snapshot[15]=world.missing.size();snapshot[16]=world.emissions.size();snapshot[17]=bullet.rotateToMotion;return snapshot;}
uint32_t test_active(){return bullet.active;}
int test_collide(int type,float x,float y,float playerHit){State s{};s.phase=Phase::Alive;s.age=100;s.x=x;s.y=y;appearance(s,type,0);return int(collide(s,0,0,playerHit));}
int test_current_collision(){return int(collide(bullet,bullet.x,bullet.y,2));}
const float* test_emission(){const auto& e=world.emissions.back();emission[0]=e.type;emission[1]=e.color;emission[2]=e.x;emission[3]=e.y;emission[4]=e.angle;emission[5]=e.spacing;emission[6]=e.speed;emission[7]=e.slow;emission[8]=e.count;emission[9]=e.layers;emission[10]=e.mode;emission[11]=e.program.first;emission[12]=e.program.flags;return emission;}
}
