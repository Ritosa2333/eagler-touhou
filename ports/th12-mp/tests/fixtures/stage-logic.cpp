#include "../../src/stage_logic.hpp"
using namespace th12::stage_logic;
Program program;State stage;World world;float values[27];int integers[8];
extern "C" {
int stage_load(const uint8_t* bytes,int size){std::string error;return program.load(bytes,size,error);}
void stage_reset(int flags,int age,int zeroPosition){Camera initial;if(zeroPosition){initial.position={};initial.direction={0,0,1};}stage.reset(program,initial);stage.flags=flags;stage.transitionAge=age;}
int stage_tick(){return stage.tick(world);}
const float* stage_camera(){int at=0;for(auto v:{stage.camera.position,stage.camera.direction,stage.camera.up,stage.camera.eyeOffset,stage.camera.offset,stage.camera.velocity})for(float n:v)values[at++]=n;values[at++]=stage.camera.fov;values[at++]=stage.camera.fogStart;values[at++]=stage.camera.fogEnd;for(float n:stage.camera.fogBytes)values[at++]=n;return values;}
const int* stage_state(){integers[0]=stage.clock;integers[1]=stage.callbackCount;integers[2]=stage.flags;integers[3]=stage.camera.fogArgb;integers[4]=stage.pc;integers[5]=program.primitives.size();integers[6]=stage.shakeMode;integers[7]=stage.shakeAge;return integers;}
int stage_label(int label){return stage.seekLabel(label);}
}
