#include "ecl/EclVm.hpp"
#include "../ecl-fixture.hpp"
#include <iostream>
using namespace ecl_fixture;
int main(){try{int opcode=0;std::uint32_t first=0,second=0;while(std::cin>>opcode>>first>>second){
    const auto command=opcode==81?instruction(81,{floatBits(0),floatBits(4),first,second},0,3):opcode==82?instruction(82,{floatBits(0)},0,1):instruction(86,{floatBits(0),first,second},0,1);
    const auto bytes=program({{"main",join({instruction(40,{16}),instruction(44,{first}),instruction(45,{floatBits(0)},0,1),command,instruction(0,{},1000)})}});th128::ecl::Program source;std::string error;require(source.load(bytes.data(),bytes.size(),error),error);
    th128::ecl::Vm vm(source);require(vm.start("main"),"start");require(vm.runTick().status==th128::ecl::VmStatus::Running,vm.error().message);const auto thread=vm.threads()[0];
    std::cout<<thread.locals[0];if(opcode==81)std::cout<<' '<<thread.locals[1];std::cout<<'\n';
}}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
