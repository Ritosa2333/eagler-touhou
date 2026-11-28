#include "game/BulletUpdateHost.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
std::vector<unsigned char> read(const char* name){std::ifstream input(name,std::ios::binary);assert(input);return {std::istreambuf_iterator<char>(input),{}};}
int main(int count,char** arguments){
    assert(count==3);using namespace th128::bullet;std::string error;const auto profileBytes=read(arguments[1]),animation=read(arguments[2]);
    Profile profile;assert(profile.loadTyped(4,profileBytes.data(),profileBytes.size(),error));assert(profile.script==39&&profile.tail.value==3);
    BulletVisuals visuals;assert(visuals.load(animation.data(),animation.size(),error));std::map<unsigned,BulletVisuals::Visual> animations;
    Manager manager;assert(manager.setConstructor([&](const Birth& birth,std::string& reason){
        BulletVisuals::CreateInfo info;info.type=birth.type;info.color=birth.color;info.script=birth.script;info.birthInterrupt=birth.state.animationInterrupt;
        for(int index=0;index<4;++index)info.spriteRemap[std::size_t(index)]=*profile.remap(birth.color,index);
        const auto visual=visuals.create(info,reason);if(!visual)return false;animations.emplace(birth.id,*visual);return true;
    }));
    assert(manager.setUpdater([&](Birth& birth,bool& removed,std::string& reason){return updateBulletWithKnownMisses(birth,visuals,animations.at(birth.id),{180,400,0},2,removed,reason);}));
    auto emitter=resetEmitter();emitter.type=4;emitter.color=6;emitter.positionX=10;emitter.positionY=144;emitter.aimMode=3;emitter.angle=.5f;emitter.speed=1.5f;
    assert(setSpawnRadius(emitter,24));emitter.extension0.type=2;emitter.extension0.control=1;emitter.extension0.parameterA=1;
    emitter.laterExtensions[0].type=1;emitter.laterExtensions[0].control=1;
    assert(manager.emit(emitter,profile,{180,400,0},0,error));auto born=manager.snapshots().front();assert(born.type==4&&born.script==39&&born.state.profileValue==3);
    const auto geometry=*emissionPrefix(emitter,0,0,0);const auto origin=*spawnPositionPrefix(emitter,geometry);
    assert(born.state.position.x==float(double(origin.x)-float(double(geometry.velocityX)*4.)));
    bool accelerated=false,settled=false;
    for(int frame=0;frame<60;++frame){
        if(!manager.tick(error)){std::cerr<<error<<'\n';return 1;}const auto live=manager.snapshots();assert(live.size()==1);
        if(live[0].state.activeExtensions&1u){accelerated=true;assert(live[0].state.motionState==1);}
        if(accelerated&&!(live[0].state.activeExtensions&1u)){settled=true;assert(live[0].state.extensionIndex==2);}
    }
    assert(accelerated&&settled);std::cout<<"actual type4 pool, native radius24, ANM39 and ETEX1 complete burst/settle passed\n";
}
