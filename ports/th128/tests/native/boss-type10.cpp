#include "game/BulletManager.hpp"
#include "game/BulletVisuals.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
int main(int count,char** arguments){
    assert(count==3);const auto read=[](const char* name){std::ifstream file(name,std::ios::binary);assert(file);return std::vector<unsigned char>{std::istreambuf_iterator<char>(file),{}};};
    const auto bytes=read(arguments[1]),animation=read(arguments[2]);std::string error;
    using namespace th128::bullet;Profile profile;
    if(!profile.loadTyped(10,bytes.data(),bytes.size(),error)){std::cerr<<error<<'\n';return 1;}
    assert(profile.script==45&&profile.tail.hitboxSize==2.4f&&profile.tail.auxiliarySize==16&&profile.tail.value==4);
    BulletVisuals visuals;assert(visuals.load(animation.data(),animation.size(),error));
    th128::anm::Registry registry;assert(registry.loadBank(0,animation.data(),animation.size(),error));
    for(int color=0;color<16;++color){
        BulletVisuals::CreateInfo info;info.type=10;info.color=color;info.script=45;info.birthInterrupt=8;
        for(int index=0;index<4;++index)info.spriteRemap[std::size_t(index)]=*profile.remap(color,index);
        auto actual=visuals.create(info,error);if(!actual){std::cerr<<error<<'\n';return 1;}
        th128::anm::Runtime runtime;runtime.remapSprite=[&](int index){return info.spriteRemap.at(std::size_t(index));};th128::anm::VM expected;
        assert(expected.bind(registry,0,45,runtime));expected.interrupt(8);expected.tick(runtime);
        for(int tick=0;tick<100;++tick){const auto* state=visuals.animationState(*actual);const auto& reference=expected.state();
            assert(state&&state->sprite==reference.sprite&&state->ticks==reference.ticks&&state->clock==reference.clock&&state->sx==reference.sx&&state->sy==reference.sy&&state->primary==reference.primary);
            assert(visuals.advance(*actual,1,error));expected.tick(runtime);
        }
        Manager manager;assert(manager.setConstructor([](const Birth& birth,std::string&){return birth.type==10&&birth.script==45;}));
        auto emitter=resetEmitter();emitter.type=10;emitter.color=color;emitter.positionY=100;emitter.extension0.type=2;emitter.extension0.parameterA=1;
        assert(manager.emit(emitter,profile,{0,400,0},0,error)&&manager.activeCount()==1);
    }
    std::cout<<"type10 native profile, all16 colors and100 actual ANM ticks match owning VM\n";
}
