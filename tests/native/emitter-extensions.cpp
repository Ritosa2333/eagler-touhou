#include "game/BulletRules.hpp"
#include <bit>
#include <cassert>
#include <iostream>

int main(){
    using namespace th128::bullet;
    int index=0;
    while(std::cin>>index){
        auto emitter=resetEmitter();ExtensionRaw prior;
        prior.parameterR=prior.parameterS=prior.parameterT=prior.parameterU=std::bit_cast<float>(0xa5a5a5a5u);
        prior.parameterA=prior.parameterB=prior.parameterC=prior.parameterD=std::bit_cast<int>(0xa5a5a5a5u);
        prior.type=prior.control=std::bit_cast<int>(0xa5a5a5a5u);prior.callback=0xa5a5a5a5u;
        assert(setExtensionRecord(emitter,index,prior));
        ExtensionRaw update;std::cin>>update.control>>update.type>>update.parameterA>>update.parameterB>>update.parameterC>>update.parameterD;
        for(auto* value:{&update.parameterR,&update.parameterS,&update.parameterT,&update.parameterU}){
            std::uint32_t raw=0;std::cin>>raw;*value=std::bit_cast<float>(raw);
        }
        assert(setExtendedExtension(emitter,index,update));const auto result=extensionRecord(emitter,index);assert(result);
        bool first=true;
        for(const auto value:serializeExtension(*result)){if(!first)std::cout<<' ';first=false;std::cout<<value;}
        std::cout<<'\n';
        const auto before=*result;ExtensionRaw legacy;legacy.parameterR=.125f;legacy.parameterS=-2;
        legacy.type=2;legacy.control=1;legacy.parameterA=60;legacy.parameterB=-1;
        assert(setLegacyExtension(emitter,index,legacy));const auto merged=extensionRecord(emitter,index);assert(merged);
        assert(merged->parameterT==before.parameterT&&merged->parameterU==before.parameterU&&merged->parameterC==before.parameterC
            &&merged->parameterD==before.parameterD&&merged->callback==before.callback);
        assert(!setExtendedExtension(emitter,18,update)&&!extensionRecord(emitter,-1));
    }
    auto emitter=resetEmitter();emitter.type=8;emitter.color=10;emitter.extension0.type=2;emitter.extension0.parameterA=1;
    ExtensionRaw second;second.type=16;second.parameterA=60;second.parameterB=1;second.parameterC=2;second.parameterS=-999;
    assert(setExtendedExtension(emitter,1,second));
    const auto birth=spawnTailPrefix(emitter,{8,2.4f,16,4},{},{},0x13);assert(birth);
    assert(birth->extensionIndex==1&&birth->laterExtensions[0].type==16&&birth->laterExtensions[0].parameterC==2);
    assert(!nextExtensionEmpty(*birth));
}
