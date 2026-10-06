#include "../../src/anm_scene.hpp"
#include <sstream>
#include <iomanip>
using namespace th12::anm_logic;
Scene scene;uint16_t seed=0x1234,displaySeed=0xbeef;uint32_t calls=0,displayCalls=0;std::string output;
uint32_t next(uint16_t& value,uint32_t& count){uint16_t a=uint16_t((value^0x9630)-0x6553),b=uint16_t((a<<2)|(a>>14)),c=uint16_t((b^0x9630)-0x6553);value=uint16_t((c<<2)|(c>>14));count+=2;return uint32_t(a)<<16|c;}
extern "C" {
int load(const uint8_t* p,int n){std::string error;return scene.registry.loadBank(0,p,n,error);}
void reset(){scene.reset();seed=0x1234;displaySeed=0xbeef;calls=displayCalls=0;scene.runtime.gameRandom32=[](){return next(seed,calls);};scene.runtime.displayRandom32=[](){return next(displaySeed,displayCalls);};}
int bind(int key,int script){return scene.bind(key,1,0,script)!=nullptr;}
void primary(){scene.tickPrimary();scene.collect();}
void secondary(){scene.tickSecondary();scene.collect();}
void scene_remove(int key){scene.remove(key);scene.collect();}
const char* snapshot(){std::ostringstream out;out<<std::setprecision(9)<<"{\"gameSeedCounter\":["<<seed<<','<<calls<<"],\"primary\":[";
  bool comma=false;auto write=[&](Membership membership){for(const auto* node:scene.chain(membership)){if(comma)out<<',';comma=true;const auto& s=node->vm.state();out<<"{\"script\":"<<s.script<<",\"clock\":"<<s.clock<<",\"float0\":"<<s.floats[0]<<'}';}};
  write(Membership::Primary);out<<"],\"secondary\":[";comma=false;write(Membership::Secondary);out<<"]}";output=out.str();return output.c_str();}
}
