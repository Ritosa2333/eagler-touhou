#include "game/BulletUpdateHost.hpp"
#include <bit>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
int main(int argc,char** argv){assert(argc==3);std::string error;
 const auto read=[](const char* path){std::ifstream file(path,std::ios::binary);assert(file);return std::vector<unsigned char>{std::istreambuf_iterator<char>(file),{}};};
 const auto profileBytes=read(argv[1]),animation=read(argv[2]);th128::bullet::Profile profile;assert(profile.load(profileBytes.data(),profileBytes.size(),error));th128::bullet::BulletVisuals visuals;assert(visuals.load(animation.data(),animation.size(),error));
 int active=0,remaining=0,duration=0;while(std::cin>>active>>remaining>>duration){using namespace th128::bullet;Birth birth;birth.type=8;birth.color=10;birth.angle=.5f;birth.speed=1;birth.state.position={10,100,.1f};birth.state.velocity={.5f,1.5f,0};birth.state.flags=0x11;birth.state.motionState=1;birth.state.transformSound=-1;
  birth.state.extensionIndex=active?2:1;birth.state.activeExtensions=active?0x80000000u:0;birth.delayRemaining=remaining;
  ExtensionRaw delay;delay.type=std::bit_cast<int>(0x80000000u);delay.parameterA=duration;birth.state.laterExtensions[0]=delay;
  ExtensionRaw acceleration;acceleration.type=4;acceleration.parameterA=30;acceleration.parameterB=-999999;acceleration.parameterR=.0166666675f;acceleration.parameterS=-999;birth.state.laterExtensions[1]=acceleration;birth.extensionListEmpty=nextExtensionEmpty(birth.state);
  BulletVisuals::CreateInfo info;info.type=8;info.color=10;info.script=43;info.birthInterrupt=8;for(int index=0;index<4;++index)info.spriteRemap[std::size_t(index)]=*profile.remap(10,index);auto visual=visuals.create(info,error);assert(visual);visual->primary.control().integers[0]=1;bool removed=false;
  if(!updateBulletWithKnownMisses(birth,visuals,*visual,{0,400,0},2,removed,error)||removed){std::cerr<<error<<'\n';return 1;}
  for(const float value:{birth.state.position.x,birth.state.position.y,birth.state.position.z,birth.state.velocity.x,birth.state.velocity.y,birth.state.velocity.z,birth.speed,birth.angle})std::cout<<std::bit_cast<std::uint32_t>(value)<<' ';
  std::cout<<birth.state.extensionIndex<<' '<<birth.state.activeExtensions<<' '<<birth.delayRemaining<<' '<<birth.extension4.time<<'\n';
 }
}
