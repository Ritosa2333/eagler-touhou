#include "animation/AnmLogic.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <iterator>
int main(int count,char** arguments){
    assert(count==2);std::ifstream input(arguments[1],std::ios::binary);
    const std::vector<std::uint8_t> bytes{std::istreambuf_iterator<char>(input),{}};
    int opcode=0;std::uint32_t value=0;
    while(std::cin>>opcode>>value){
        th128::anm::Registry registry;th128::anm::Bank bank;th128::anm::Script script;
        script.commands.push_back({std::int16_t(opcode),0,0,0,{value}});
        script.commands.push_back({63,1,0,12,{}});script.offsets={{0,0},{12,1}};
        std::string error;assert(registry.loadBank(6,bytes.data(),bytes.size(),error));
        registry.bank(6)->scripts[0]=std::move(script);
        th128::anm::VM vm;th128::anm::Runtime runtime;
        assert(vm.bind(registry,6,0,runtime));assert(!vm.faulted());
        const auto& state=vm.state();std::cout<<state.flags<<' '<<state.anchorX<<' '<<state.anchorY<<' '<<state.mode<<' '<<state.blend<<'\n';
    }
}
