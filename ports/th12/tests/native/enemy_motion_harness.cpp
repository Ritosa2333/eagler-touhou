#include "../../src/enemy_motion.hpp"
using namespace th12::enemy_motion;
static Movement movement;
static float data[30];
extern "C" {
void setup(int relative,float angle,float radius,float delta,float speed,float targetSpeed,float targetRadius,float targetDelta,int duration,int mode){
    movement={};movement.absolute.position={0,128};movement.combined.position={0,128};
    setCircle(movement,relative,angle,speed,radius,delta);
    interpolateCircle(movement,relative,duration,mode,targetSpeed,targetRadius,targetDelta);
}
void tick(){th12::enemy_motion::tick(movement);}
float quantized(float value){return quantize(value);}
float* snapshot(){
    int index=0;
    for(const auto* m:{&movement.absolute,&movement.relative,&movement.combined}){
        for(float v:{m->position.x,m->position.y,m->vector.x,m->vector.y,m->speed,m->angle,m->radius,m->radiusDelta,float(m->flags)})data[index++]=v;
    }
    data[27]=movement.absoluteRadius.duration;data[28]=movement.relativeRadius.duration;return data;
}
}
