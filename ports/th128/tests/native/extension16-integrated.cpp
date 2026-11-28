#include "game/BulletUpdateHost.hpp"
#include <bit>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>

int main(int argc,char** argv){
    assert(argc==3||argc==4);std::string error;const bool type4=argc==4;
    const auto read=[](const char* path){std::ifstream file(path,std::ios::binary);assert(file);return std::vector<unsigned char>{std::istreambuf_iterator<char>(file),{}};};
    const auto profileBytes=read(argv[1]),animation=read(argv[2]);th128::bullet::Profile profile;assert(profile.load(profileBytes.data(),profileBytes.size(),error));
    th128::bullet::BulletVisuals visuals;assert(visuals.load(animation.data(),animation.size(),error));
    int half=0,active=0,time=0,cooldown=0;
    while(std::cin>>half>>active>>time>>cooldown){
        using namespace th128::bullet;Birth birth;birth.type=8;birth.color=10;birth.angle=.5f;birth.speed=3;
        birth.state.position={10,100,.1f};birth.state.velocity={.5f,1.5f,0};birth.state.flags=0x11;birth.state.motionState=half?2:1;
        birth.state.transformSound=-1;birth.state.extensionIndex=1;birth.extensionListEmpty=false;birth.cooldown=cooldown;
        ExtensionRaw record;record.type=type4?4:16;record.parameterR=type4?.0166666675f:0;record.parameterA=60;record.parameterB=1;record.parameterC=2;record.parameterS=-999;
        if(type4)birth.speed=1;
        birth.state.laterExtensions[0]=record;
        if(active&&!type4){birth.extension16=*initializeExtension16(record,3,.5f);birth.extension16.time=time;birth.extension16.previous=time-1;
            birth.extension16.fraction=float(time);birth.state.activeExtensions=16;birth.state.extensionIndex=2;birth.extensionListEmpty=true;}
        if(active&&type4){birth.extension4=*initializeExtension4(record,.5f,birth.state.position,{0,400,0});birth.extension4.time=time;
            birth.extension4.previous=time-1;birth.extension4.fraction=float(time);birth.state.activeExtensions=4;birth.state.extensionIndex=2;birth.extensionListEmpty=true;}
        BulletVisuals::CreateInfo info;info.script=43;info.type=8;info.color=10;info.birthInterrupt=8;
        for(int index=0;index<4;++index)info.spriteRemap[std::size_t(index)]=*profile.remap(10,index);
        auto visual=visuals.create(info,error);assert(visual);visual->primary.control().integers[0]=1;
        bool removed=false;assert(updateBulletWithKnownMisses(birth,visuals,*visual,{0,400,0},2,removed,error)&&!removed);
        for(const float value:{birth.state.position.x,birth.state.position.y,birth.state.position.z,birth.state.velocity.x,birth.state.velocity.y,birth.state.velocity.z})
            std::cout<<std::bit_cast<std::uint32_t>(value)<<' ';
        std::cout<<birth.lifeClock<<' '<<birth.cooldown<<' '<<birth.grace<<' '<<(type4?birth.extension4.time:birth.extension16.time)<<' '<<birth.state.activeExtensions<<'\n';
    }
}
