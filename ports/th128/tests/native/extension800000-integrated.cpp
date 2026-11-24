#include "game/BulletUpdateHost.hpp"
#include <bit>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>

namespace {
std::uint32_t bits(float value){return std::bit_cast<std::uint32_t>(value);}
}

int main(int argc,char** argv){
    assert(argc==3);
    std::string error;
    const auto read=[](const char* path){std::ifstream file(path,std::ios::binary);assert(file);return std::vector<unsigned char>{std::istreambuf_iterator<char>(file),{}};};
    const auto profileBytes=read(argv[1]),animation=read(argv[2]);
    th128::bullet::Profile profile;assert(profile.load(profileBytes.data(),profileBytes.size(),error));
    th128::bullet::BulletVisuals visuals;assert(visuals.load(animation.data(),animation.size(),error));
    int mode=0,control=0,active=0,index=0;float speed=0,angle=0,px=0,py=0,pz=0;
    while(std::cin>>mode>>control>>active>>index>>speed>>angle>>px>>py>>pz){
        using namespace th128::bullet;
        Birth birth;birth.type=8;birth.color=10;birth.angle=angle;birth.speed=speed;
        birth.state.position={px,py,pz};birth.state.velocity={0,0,0};birth.state.flags=0x11;
        birth.state.motionState=1;birth.state.transformSound=-1;birth.state.activeExtensions=std::uint32_t(active);
        birth.state.extensionIndex=index;birth.extensionListEmpty=false;
        ExtensionRaw record;record.type=0x800000;record.control=control;
        record.parameterA=-999999;record.parameterB=-999999;record.parameterR=-999999;record.parameterS=-999999;
        record.parameterC=0;record.parameterD=0;record.callback=77;
        birth.state.laterExtensions[0]=record;
        if(mode==2)birth.state.laterExtensions[0].type=0x123;
        if(mode==3)birth.state.extensionIndex=18;
        BulletVisuals::CreateInfo info;info.script=43;info.type=8;info.color=10;info.birthInterrupt=8;
        for(int i=0;i<4;++i)info.spriteRemap[std::size_t(i)]=*profile.remap(10,i);
        auto visual=visuals.create(info,error);assert(visual);visual->primary.control().integers[0]=1;
        std::vector<int> sounds;
        const auto sound=[&](int identifier,std::string&){sounds.push_back(identifier);return true;};
        bool removed=false;
        const bool first=updateBulletWithKnownMisses(birth,visuals,*visual,{1000,1000,0},0,removed,error,{},{},{},sound);
        if(!first){std::cout<<"FAIL "<<error<<' '<<birth.state.activeExtensions<<' '<<birth.extension800000.angle<<'\n';continue;}
        if(mode==4){
            birth.extensionListEmpty=true;birth.state.extensionIndex=2;
            const bool second=updateBulletWithKnownMisses(birth,visuals,*visual,{1000,1000,0},0,removed,error,{},{},{},sound);
            if(!second){std::cout<<"FAIL persistence "<<error<<'\n';continue;}
        }
        std::cout<<bits(birth.extension800000.speed)<<' '<<bits(birth.extension800000.angle)<<' '
            <<bits(birth.extension800000.position.x)<<' '<<bits(birth.extension800000.position.y)<<' '
            <<bits(birth.extension800000.position.z)<<' '<<birth.state.extensionIndex<<' '
            <<birth.state.activeExtensions<<' '<<birth.state.motionState<<' '
            <<birth.state.laterExtensions[0].callback<<' '<<sounds.size()<<'\n';
    }
}
