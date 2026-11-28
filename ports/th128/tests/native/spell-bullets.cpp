#include "game/BulletManager.hpp"
#include "game/BulletVisuals.hpp"
#include "game/BulletTickRules.hpp"
#include <cassert>
#include <bit>
#include <fstream>
#include <iostream>
#include <iterator>
int main(int argc,char** argv){assert(argc==4);using namespace th128::bullet;const int type=std::stoi(argv[1]);const auto read=[](const char* path){std::ifstream file(path,std::ios::binary);assert(file);return std::vector<unsigned char>{std::istreambuf_iterator<char>(file),{}};};
 const auto profileBytes=read(argv[2]),animation=read(argv[3]);Profile profile;std::string error;assert(profile.loadTyped(type,profileBytes.data(),profileBytes.size(),error));assert(profile.tail.freezePolicy==0);
 auto emitter=resetEmitter();emitter.type=type;emitter.extension0.type=2;emitter.extension0.parameterA=1;
 const auto birth=spawnTailPrefix(emitter,profile.tail,{-128,100,.1f},{.5,1.5,0},0x13);assert(birth);std::cout<<birth->flags<<' '<<birth->motionState<<' '<<birth->animationInterrupt<<' '<<birth->extensionIndex<<' '<<birth->profileValue;
 for(const float value:{birth->hitboxSize,birth->auxiliarySize,birth->position.x,birth->position.y,birth->position.z,birth->velocity.x,birth->velocity.y,birth->velocity.z})std::cout<<' '<<std::bit_cast<std::uint32_t>(value);std::cout<<'\n';
 BulletVisuals visuals;assert(visuals.load(animation.data(),animation.size(),error));BulletVisuals::CreateInfo info;info.type=type;info.color=type==23?6:13;info.script=profile.script;info.birthInterrupt=2;for(int index=0;index<4;++index)info.spriteRemap[std::size_t(index)]=*profile.remap(info.color,index);auto visual=visuals.create(info,error);assert(visual);for(int tick=0;tick<40;++tick)assert(visuals.advance(*visual,1,error));
 unsigned mode=0,flags=0;int age=0;while(std::cin>>mode>>age>>flags){TickInput input;input.type=type;input.motionState=std::uint16_t(mode);input.age=age;input.flags=flags;input.position={-128,100,.1f};input.velocity={.5,1.5,0};input.spriteWidth=input.spriteHeight=16;input.grace=5;input.hasSprite=true;input.extensionListEmpty=true;
  const auto step=preAnimationTick(input);assert(step);std::cout<<int(step->continuation)<<' '<<step->next.motionState<<' '<<step->next.flags<<'\n';}
}
