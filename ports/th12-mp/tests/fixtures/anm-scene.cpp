#include "../../src/anm_scene.hpp"
#include <sstream>
#include <iomanip>
using namespace th12::anm_logic;
Scene scene;uint16_t seed=0x1234,displaySeed=0xbeef;uint32_t calls=0,displayCalls=0,unsupported=0;std::string output;
uint32_t next(uint16_t& value,uint32_t& count){uint16_t a=uint16_t((value^0x9630)-0x6553),b=uint16_t((a<<2)|(a>>14)),c=uint16_t((b^0x9630)-0x6553);value=uint16_t((c<<2)|(c>>14));count+=2;return uint32_t(a)<<16|c;}
extern "C" {
int load(const uint8_t* p,int n){std::string error;return scene.registry.loadBank(0,p,n,error);}
int load_bank(int bank,const uint8_t* p,int n){std::string error;return scene.registry.loadBank(bank,p,n,error);}
void reset(){scene.reset();seed=0x1234;displaySeed=0xbeef;calls=displayCalls=unsupported=0;scene.runtime.gameRandom32=[](){return next(seed,calls);};scene.runtime.displayRandom32=[](){return next(displaySeed,displayCalls);};scene.runtime.unsupported=[](int,int,int,const char*){unsupported++;};}
int bind(int key,int script){return scene.bind(key,1,0,script)!=nullptr;}
int bind_at(int key,int script,float x,float y){return scene.bind(key,1,0,script,Membership::Primary,x,y)!=nullptr;}
int bind_bank_xyz(int key,int bank,int script,float x,float y,float z,int layer){return scene.bind(key,1,bank,script,Membership::Primary,x,y,{},false,nullptr,0,0,layer,false,z)!=nullptr;}
void scene_interrupt(int script,int code,int immediate){for(const auto& [id,node]:scene.nodes())if(node->alive&&node->vm.state().script==script){scene.interruptNode(id,code,immediate);return;}}
void scene_rotate(int script,float x,float y,float z){for(const auto& [id,node]:scene.nodes())if(node->alive&&node->vm.state().script==script){auto& state=node->vm.control();state.rx=x;state.ry=y;state.rotation=z;return;}}
void primary(){scene.tickPrimary();scene.collect();}
void secondary(){scene.tickSecondary();scene.collect();}
void scene_remove(int key){scene.remove(key);scene.collect();}
const char* snapshot(){std::ostringstream out;out<<std::setprecision(9)<<"{\"gameSeedCounter\":["<<seed<<','<<calls<<"],\"displaySeedCounter\":["<<displaySeed<<','<<displayCalls<<"],\"unsupported\":"<<unsupported<<",\"primary\":[";
  bool comma=false;auto write=[&](Membership membership){for(const auto* node:scene.chain(membership)){if(comma)out<<',';comma=true;const auto& s=node->vm.state();out<<"{\"script\":"<<s.script<<",\"clock\":"<<s.clock<<",\"float0\":"<<s.floats[0]
    <<",\"pendingInterrupt\":"<<node->vm.diagnostics().pendingInterrupt<<",\"scale\":["<<s.sx<<','<<s.sy<<"],\"position\":["<<s.x<<','<<s.y<<','<<s.z<<"],\"engine\":["<<s.engineX<<','<<s.engineY<<','<<s.engineZ<<"],\"offset\":["<<s.offsetX<<','<<s.offsetY<<','<<s.offsetZ<<"],\"attached\":"<<(node->childPrevious?"true":"false")<<",\"children\":[";
    bool inner=false;for(const auto* child:scene.children(node->id)){if(inner)out<<',';inner=true;out<<child->vm.state().script;}
    out<<"],\"rotation\":["<<s.rx<<','<<s.ry<<','<<s.rotation<<"],\"sprite\":"<<s.sprite<<",\"spriteBank\":"<<s.spriteBank<<",\"scriptBank\":"<<s.bank<<'}';}};
  write(Membership::Primary);out<<"],\"secondary\":[";comma=false;write(Membership::Secondary);out<<"]}";output=out.str();return output.c_str();}
}
