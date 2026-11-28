#include "game/BulletManager.hpp"
#include "game/BulletRules.hpp"
#include "game/BulletVisuals.hpp"
#include "game/IceContact.hpp"
#include "engine/Random.hpp"
#include "animation/AnmLogic.hpp"
#include <bit>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

using namespace th128::bullet;
static std::vector<unsigned char> read(const char* path){
    std::ifstream input(path,std::ios::binary);assert(input);
    return {std::istreambuf_iterator<char>(input),{}};
}
static void require(bool ok,const std::string& message){if(!ok){std::cerr<<message<<'\n';std::exit(1);}}
int main(int argc,char** argv){
    require(argc==3,"usage: type6-profile <profile.bin> <bullet.anm>");
    const auto profileBytes=read(argv[1]);const auto animation=read(argv[2]);
    require(profileBytes.size()==288,"type6 profile size");
    std::string error;Profile profile;
    require(profile.loadTyped(6,profileBytes.data(),profileBytes.size(),error),error);
    require(profile.type==6&&profile.script==41&&profile.secondaryScript==0,"type6 script/secondary");
    require(profile.tail.hitboxSize==4.f&&profile.tail.auxiliarySize==16.f,"type6 hitbox/aux");
    require(profile.tail.value==3&&profile.tail.freezePolicy==0,"type6 appearance/freeze");
    require(!profile.identitySpriteRemap(),"type6 must use color remap, not identity callback");
    require(profile.remap(0,0)==96&&profile.remap(0,1)==323&&profile.remap(0,2)==336&&profile.remap(0,3)==337,
            "type6 color0 remap");
    require(profile.remap(1,0)==97&&profile.remap(1,3)==339,"type6 color1 remap");
    require(profile.cancellationScript(0)==337&&profile.cancellationScript(1)==339
        &&profile.cancellationScript(2)==341&&profile.cancellationScript(3)==343,"type6 cancellation scripts");
    require(profile.cancellationScript(15)==367,"type6 last color cancellation");
    {
        const auto copy=profile;auto corrupt=profileBytes;
        corrupt[0]=42;require(!profile.loadTyped(6,corrupt.data(),corrupt.size(),error)
            &&profile.script==copy.script,"type6 wrong script retained old profile");
        corrupt=profileBytes;corrupt[0x104]=corrupt[0x105]=corrupt[0x106]=corrupt[0x107]=0;require(!profile.loadTyped(6,corrupt.data(),corrupt.size(),error),
            "type6 wrong hitbox");
        corrupt=profileBytes;corrupt[0x108]=corrupt[0x109]=corrupt[0x10a]=corrupt[0x10b]=0;require(!profile.loadTyped(6,corrupt.data(),corrupt.size(),error),
            "type6 wrong auxiliary");
        corrupt=profileBytes;corrupt[0x10c]=4;require(!profile.loadTyped(6,corrupt.data(),corrupt.size(),error),
            "type6 wrong appearance");
        corrupt=profileBytes;corrupt[0x110]=8;require(!profile.loadTyped(6,corrupt.data(),corrupt.size(),error),
            "type6 wrong cancellation mode");
        corrupt=profileBytes;corrupt[0x118]=1;require(!profile.loadTyped(6,corrupt.data(),corrupt.size(),error),
            "type6 wrong freeze policy");
        require(!profile.loadTyped(6,profileBytes.data(),profileBytes.size()-1,error),"type6 short profile");
        require(!profile.loadTyped(5,profileBytes.data(),profileBytes.size(),error),"type6 type mismatch");
    }
    std::vector<Birth> births;Manager manager;
    require(manager.setConstructor([&](const Birth& birth,std::string&){births.push_back(birth);return true;}),"constructor");
    auto emitter=resetEmitter();emitter.type=6;emitter.color=0;emitter.count=1;emitter.aimMode=2;
    emitter.speed=emitter.endSpeed=1.5f;
    require(manager.emit(emitter,profile,{0,400,0},0,error),error);
    require(births.size()==1&&manager.activeCount()==1,"type6 birth count");
    const auto& birth=births.front();
    require(birth.type==6&&birth.script==41&&birth.cancellationScript==337,"type6 birth fields");
    require(birth.state.hitboxSize==4.f&&birth.state.auxiliarySize==16.f
        &&birth.state.profileValue==3&&birth.state.freezePolicy==0,"type6 birth profile/state");
    require(th128::ice::canFreezeBullet(1,profile.tail.freezePolicy,0)
        &&th128::ice::canFreezeBullet(1,profile.tail.freezePolicy,1000),"type6 policy0 ordinary freeze");
    emitter.type=8;require(!manager.emit(emitter,profile,{0,400,0},0,error),"type6 profile mismatch rejected");
    BulletVisuals visuals;require(visuals.load(animation.data(),animation.size(),error),error);
    th128::Random gameRandom,displayRandom;gameRandom.seed=displayRandom.seed=0x1234;
    visuals.setRandomSources([&]{return gameRandom.next32();},[&]{return displayRandom.next32();});
    BulletVisuals::CreateInfo info;info.type=6;info.color=0;info.script=41;
    for(std::size_t index=0;index<info.spriteRemap.size();++index){
        const auto mapped=profile.remap(0,std::int32_t(index));require(mapped.has_value(),"type6 remap");
        info.spriteRemap[index]=*mapped;
    }
    auto visual=visuals.create(info,error);
    require(visual.has_value(),error);
    require(visual->primary.state().script==41&&!visual->hasSecondary,"type6 primary script/no secondary");
    require(visual->primary.state().sprite>=0&&std::size_t(visual->primary.state().sprite)<588,
            "type6 real sprite");
    for(int tick=0;tick<90;++tick)require(visuals.advance(*visual,1,error),error);
    require(!visual->primary.faulted(),"type6 visual fault");
    BulletVisuals::CreateInfo wrong=info;wrong.script=42;
    require(!visuals.create(wrong,error),"type6 wrong visual script rejected");
    std::cout<<"type6 profile exact bytes, script41, color remap, cancellations, freeze0, birth and 90-tick visual passed\n";
}
