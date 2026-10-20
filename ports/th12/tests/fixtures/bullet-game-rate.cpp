#include "../../src/bullet.hpp"
#include <sstream>
using namespace th12::eb;
struct RateWorld:World{
 float rate=1;
 float gameRate()const override{return rate;}
 void emit(const Emission&)override{}
 void unsupported(uint32_t)override{}
 bool ownsStateAnimation(const State&)const override{return true;}
 int animationInteger(const State&,int,int)override{return 0;}
}world;
State state;std::string output;
extern "C"{
void rate_begin(float rate,int phase){world.rate=rate;state={};initialize(state,1,{},3,0,12,120,.3f,1.8f,world);state.phase=Phase(phase);state.offscreenGrace=1000;}
void rate_record(int index,int kind,int c,int d,float a,float b,int concurrent){set(state.program,index,concurrent,kind,c,d,a,b);}
void rate_dispatch(){start(state,world);}
void rate_tick(){tick(state,world);}
const char* rate_snapshot(){std::ostringstream out;out.precision(9);out<<"{\"position\":["<<state.x<<','<<state.y<<"],\"velocity\":["<<state.vx<<','<<state.vy<<"],\"speed\":"<<state.speed<<",\"angle\":"<<state.angle<<",\"active\":"<<state.active<<",\"cursor\":"<<state.cursor<<",\"phase\":"<<uint32_t(state.phase)<<",\"age\":"<<state.age<<'}';output=out.str();return output.c_str();}
}
