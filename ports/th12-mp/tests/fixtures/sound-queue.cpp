#include "../../src/game/SoundQueue.hpp"
#include <sstream>
#include <string>
std::string output;
extern "C" {
void reset(){th12::sound_system::queue.reset();}
void request(int id,float x,int kind){if(kind==2)th12::sound_system::queue.stop(id);else th12::sound_system::queue.play(id,x,kind==1);}
const char* snapshot(){std::ostringstream out;out<<'[';bool comma=false;for(const auto& slot:th12::sound_system::queue.slots()){if(slot.id<0)break;if(comma)out<<',';comma=true;out<<"{\"id\":"<<slot.id<<",\"count\":"<<slot.count<<",\"pans\":[";for(int i=0;i<slot.count;i++){if(i)out<<',';out<<slot.pans[i];}out<<"]}";}out<<']';output=out.str();return output.c_str();}
int events(){return th12::sound_system::capture();}
const void* events_ptr(){return th12::sound_system::drawState.data();}
int events_stride(){return sizeof(th12::sound_system::Event);}
}
