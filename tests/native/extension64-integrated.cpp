#include "game/BulletUpdateHost.hpp"
#include <bit>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>

int main(int argc,char** argv){
    assert(argc==3);
    std::string error;
    const auto read=[](const char* path){std::ifstream file(path,std::ios::binary);assert(file);return std::vector<unsigned char>{std::istreambuf_iterator<char>(file),{}};};
    const auto profileBytes=read(argv[1]),animation=read(argv[2]);
    th128::bullet::Profile profile;assert(profile.load(profileBytes.data(),profileBytes.size(),error));
    th128::bullet::BulletVisuals visuals;assert(visuals.load(animation.data(),animation.size(),error));
    int mode=0;float positionX=0,positionY=0,speed=0,angle=0,radius=0;int limit=0,control=0,count=0,soundId=-1;
    float positionZ=0;
    while(std::cin>>mode>>positionX>>positionY>>speed>>angle>>limit>>control>>count>>radius>>soundId>>positionZ){
        using namespace th128::bullet;
        Birth birth;birth.type=8;birth.color=10;birth.angle=angle;birth.speed=speed;
        birth.state.position={positionX,positionY,positionZ};birth.state.velocity={0,0,0};birth.state.flags=0x11;
        birth.state.motionState=1;birth.state.transformSound=soundId;birth.state.activeExtensions=0;
        birth.state.extensionIndex=1;birth.extensionListEmpty=false;
        ExtensionRaw record;record.type=64;record.parameterR=radius;record.parameterA=limit;record.parameterB=control;
        birth.state.laterExtensions[0]=record;
        if(mode==1||mode==4){
            auto initialized=initializeExtension64(record);assert(initialized);
            birth.extension64=*initialized;birth.extension64.count=count;birth.state.activeExtensions=0x40;
            birth.state.extensionIndex=2;birth.extensionListEmpty=true;
        }
        if(mode==2)birth.state.laterExtensions[0].type=65;
        if(mode==3)birth.state.laterExtensions[0].parameterB=-1;
        BulletVisuals::CreateInfo info;info.script=43;info.type=8;info.color=10;info.birthInterrupt=8;
        for(int index=0;index<4;++index)info.spriteRemap[std::size_t(index)]=*profile.remap(10,index);
        auto visual=visuals.create(info,error);assert(visual);visual->primary.control().integers[0]=1;
        std::vector<int> sounds;
        const auto sound=[&](int identifier,std::string&){sounds.push_back(identifier);return true;};
        bool removed=false;
        const bool ok=updateBulletWithKnownMisses(birth,visuals,*visual,{1000,1000,0},0,removed,error,{},{},{},sound);
        if(!ok){std::cout<<"FAIL "<<error<<'\n';continue;}
        const auto bits=[](float value){return std::bit_cast<std::uint32_t>(value);};
        std::cout<<bits(birth.state.position.x)<<' '<<bits(birth.state.position.y)<<' '<<bits(birth.state.position.z)<<' '
            <<bits(birth.state.velocity.x)<<' '<<bits(birth.state.velocity.y)<<' '<<bits(birth.state.velocity.z)<<' '
            <<birth.state.activeExtensions<<' '<<birth.extension64.count<<' '<<sounds.size()<<'\n';
    }
}
