#include "../../src/game/BulletGate.hpp"
th12::eb::BulletGate gate;float rate=1,result[4];bool active=false;
extern "C" {
void reset(int duration,float r){gate.begin(duration);rate=r;active=true;}
const float* tick(){bool completed=false;if(active){completed=gate.tick(rate);if(completed)active=false;}result[0]=gate.remaining;result[1]=gate.value;result[2]=active?1024:0;result[3]=completed;return result;}
}
