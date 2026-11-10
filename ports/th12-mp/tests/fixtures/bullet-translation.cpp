#include "../../src/bullet.hpp"
#include <sstream>
using namespace th12::eb;
struct TranslationWorld : World {
 void emit(const Emission&) override {}
 void unsupported(uint32_t) override {}
} world;
State state;std::string output;
extern "C" {
void translation_begin(float angle,float speed,int duration,int polar,float turn){
 state={};Program program;set(program,0,false,0x8000000,duration,0,angle,speed);
 if(polar)set(program,1,true,8,60000,0,0,turn);
 initialize(state,1,program,3,0,12,120,.3f,1.8f,world);
}
void translation_tick(){tick(state,world);}
int translation_callback(float rate,int elapsed){state.active=0x800000a;state.translation.timer=elapsed;return tickTranslation(state,rate);}
const char* translation_snapshot(){
 auto& m=state.translation;std::ostringstream out;out.precision(9);
 out<<"{\"position\":["<<state.x<<','<<state.y<<"],\"velocity\":["<<state.vx<<','<<state.vy
 <<"],\"speed\":"<<state.speed<<",\"angle\":"<<state.angle<<",\"active\":"<<state.active
 <<",\"cursor\":"<<state.cursor<<",\"translation\":["<<m.timer<<','<<m.b<<','<<m.a<<','<<m.vx<<','<<m.vy<<",0,"<<m.duration<<"]}";
 output=out.str();return output.c_str();
}
}
