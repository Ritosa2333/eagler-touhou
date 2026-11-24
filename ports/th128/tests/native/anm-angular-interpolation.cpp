#include "animation/AnmLogic.hpp"
#include "../ecl-fixture.hpp"
#include <cassert>
#include <cstdint>
#include <iostream>
using namespace ecl_fixture;

Bytes animation(){
    Bytes bytes(108);put32(bytes,0,7);put16(bytes,6,1);put16(bytes,10,128);put16(bytes,12,64);
    put32(bytes,16,72);bytes[72]='x';put32(bytes,64,0);put32(bytes,68,80);
    put16(bytes,80,111);put16(bytes,82,12);put32(bytes,88,0);
    put16(bytes,92,63);put16(bytes,94,8);put16(bytes,100,65535);return bytes;
}

th128::anm::Script angular(std::uint32_t startBits,int duration,int mode,
                           std::uint32_t targetBits){
    th128::anm::Script script;
    script.commands.push_back({49,0,0,0,{floatBits(0.f),floatBits(0.f),startBits}});
    script.commands.push_back({113,0,0,12,{std::uint32_t(duration),std::uint32_t(mode),targetBits}});
    script.commands.push_back({63,0,0,24,{}});
    script.offsets={{0,0},{12,1},{24,2}};
    return script;
}

int main(){
    char kind=0;
    while(std::cin>>kind){
        th128::anm::Registry registry;std::string error;const auto bytes=animation();
        assert(registry.loadBank(0,bytes.data(),bytes.size(),error));
        th128::anm::VM vm;th128::anm::Runtime runtime;int advances=0;
        if(kind=='A'||kind=='Z'){
            int duration=0,mode=9;std::uint32_t startBits=0,targetBits=0;double delta=1;
            if(kind=='A')std::cin>>duration>>mode>>startBits>>targetBits>>delta>>advances;
            else{duration=0;std::cin>>startBits>>targetBits>>advances;}
            runtime.delta=float(delta);
            registry.bank(0)->scripts[0]=angular(startBits,duration,mode,targetBits);
        }else if(kind=='U'){
            int opcode=0;std::cin>>opcode;
            th128::anm::Script script;
            script.commands.push_back({std::int16_t(opcode),0,0,0,{0}});
            script.offsets={{0,0}};
            registry.bank(0)->scripts[0]=script;
        }else return 2;
        assert(vm.bind(registry,0,0,runtime));
        if(vm.faulted()){
            std::cout<<"fault "<<vm.diagnostics().faultOpcode<<' '
                     <<floatBits(vm.state().rotation)<<'\n';
            continue;
        }
        std::cout<<"ok "<<floatBits(vm.state().rotation);
        for(int tick=0;tick<advances&&!vm.faulted();++tick){
            vm.tick(runtime);
            std::cout<<' '<<floatBits(vm.state().rotation);
        }
        if(vm.faulted())std::cout<<" fault "<<vm.diagnostics().faultOpcode;
        std::cout<<'\n';
    }
}
