#include "../../src/anm_logic.hpp"
#include "../../src/bullet_visuals.hpp"
using namespace th12::anm_logic;
Registry registry;Runtime runtime;VM machine;uint16_t gameSeed=0x1234,displaySeed=0xbeef;
uint32_t gameCounter=0,displayCounter=0,unsupported=0,spawnCount=0;
std::vector<std::unique_ptr<VM>> children;
uint32_t rng(uint16_t& seed,uint32_t& counter){
    uint16_t a=uint16_t((seed^0x9630)-0x6553);seed=uint16_t((a<<2)|(a>>14));
    uint16_t b=uint16_t((seed^0x9630)-0x6553);seed=uint16_t((b<<2)|(b>>14));counter+=2;return (uint32_t(a)<<16)|b;
}
extern "C" {
int bullet_sprite(int type,int color,int sprite){int result=INT32_MIN;return th12::bullet_visuals::resolve(type,color,sprite,result)?result:INT32_MIN;}
int load(int bank,const uint8_t* p,int n){std::string error;return registry.loadBank(bank,p,n,error);}
int bind(int bank,int script,int initialDomain){
    machine={};children.clear();gameSeed=0x1234;displaySeed=0xbeef;gameCounter=displayCounter=unsupported=spawnCount=0;
    runtime={};runtime.gameRandom32=[](){return rng(gameSeed,gameCounter);};runtime.displayRandom32=[](){return rng(displaySeed,displayCounter);};
    runtime.unsupported=[](int,int,int,const char*){unsupported++;};
    runtime.spawnChild=[](const ChildRequest& request){
        auto child=std::make_unique<VM>();child->control().engineX=request.x;child->control().engineY=request.y;child->control().engineZ=request.z;
        child->bind(registry,request.bank,request.script,runtime);children.push_back(std::move(child));spawnCount++;
    };
    runtime.cameraPosition={11,22,33};runtime.cameraEyeOffset={44,55,66};
    machine.control().flags2=initialDomain;machine.control().displayDomain=initialDomain;return machine.bind(registry,bank,script,runtime);
}
int tick(){return machine.tick(runtime);}
int rebind(int bank,int script){return machine.rebindWithoutReset(registry,bank,script,runtime);}
void interrupt(int n){machine.interrupt(n);}
float snapshot[47];float* state(){
    auto& s=machine.state();size_t i=0;
    for(auto v:{s.x,s.y,s.z,s.rx,s.ry,s.rotation,s.sx,s.sy,s.u,s.v})snapshot[i++]=v;
    for(int shift:{24,16,8,0})snapshot[i++]=float((s.primary>>shift)&255);
    for(int shift:{24,16,8,0})snapshot[i++]=float((s.secondary>>shift)&255);
    for(auto v:s.integers)snapshot[i++]=float(v);for(auto v:s.floats)snapshot[i++]=v;
    for(auto v:{s.clock,s.ticks,s.sprite,s.layer,s.mode,int(s.visible),int(s.held),int(s.ended),int(s.deleteRequested),int(s.displayDomain),int(gameSeed),int(gameCounter),int(displaySeed),int(displayCounter),int(unsupported),int(spawnCount),int(s.flags),int(s.flags2),int(children.size())})snapshot[i++]=float(v);
    return snapshot;
}
}
