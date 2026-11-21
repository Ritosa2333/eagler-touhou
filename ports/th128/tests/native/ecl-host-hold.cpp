#include "ecl/EclVm.hpp"
#include "../ecl-fixture.hpp"
#include <iostream>

using namespace ecl_fixture;
using namespace th128::ecl;
Program load(const std::vector<std::pair<std::string,Bytes>>& subroutines){
    const auto bytes=program(subroutines);Program result;std::string error;
    require(result.load(bytes.data(),bytes.size(),error),error);return result;
}
int main(){
    {
        Vm vm(load({{"main",join({instruction(420),instruction(900),instruction(0,{},10)})}}));
        bool busy=true;unsigned checks=0,after=0;std::string error;
        require(vm.registerHostOpcode(420,[&](HostCall& call){++checks;return busy?call.holdCurrentCommand():true;},error),error);
        require(vm.registerHostOpcode(900,[&](HostCall&){++after;return true;},error),error);
        require(vm.start("main"),"hold start");
        for(int tick=0;tick<3;++tick){
            const auto result=vm.runTick();require(result.status==VmStatus::Running&&result.instructions==1,"busy host executes once per tick");
            require(vm.threads()[0].offset==0&&vm.threads()[0].time==0&&after==0,"busy host retains originalPC and clock");
        }
        busy=false;require(vm.runTick().status==VmStatus::Running&&checks==4&&after==1,"clear host resumes next authored command in same pass");
        require(vm.threads()[0].offset==32&&vm.threads()[0].time==1,"normal exit advances command and clock");
        require(vm.runTick().status==VmStatus::Running&&checks==4&&after==1&&vm.threads()[0].time==2,"no duplicated continuation after release");
    }
    {
        auto spawn=stringWords("worker");spawn.push_back(7);
        Vm vm(load({{"main",join({instruction(16,spawn,0,0,255,2),instruction(420),instruction(0,{},100)})},
            {"worker",join({instruction(901),instruction(0,{},100)})}}));
        std::string error;unsigned workers=0;
        require(vm.registerHostOpcode(420,[](HostCall& call){return call.holdCurrentCommand();},error),error);
        require(vm.registerHostOpcode(901,[&](HostCall&){++workers;return true;},error),error);
        require(vm.start("main")&&vm.runTick().status==VmStatus::Running,"spawn before hold");
        require(vm.runTick().status==VmStatus::Running&&workers==1,"held main does not suppress other scheduled contexts");
        const auto threads=vm.threads();require(threads[0].time==0&&threads[1].time==1,"only held context clock is frozen");
    }
    {
        Vm vm(load({{"main",join({instruction(420,{},0,0,2),instruction(10)})}}));std::string error;unsigned calls=0;
        require(vm.registerHostOpcode(420,[&](HostCall& call){++calls;return call.holdCurrentCommand();},error),error);
        require(vm.start("main",1)&&vm.runTick().status==VmStatus::Finished&&calls==0,"rank-masked host is skipped without blocking");
    }
    for(const int kind:{0,1,2}){
        Vm vm(load({{"main",join({instruction(420,kind==0?std::vector<std::uint32_t>{1}:std::vector<std::uint32_t>{}),instruction(10)})}}));
        std::string error;
        require(vm.registerHostOpcode(420,[&](HostCall& call){
            if(kind==2){vm.runTick();return call.holdCurrentCommand();}
            const bool held=call.holdCurrentCommand();return kind==1?false:held;
        },error),error);
        require(vm.start("main")&&vm.runTick().status==VmStatus::Faulted,"invalid/rejected/reentrant host cannot turn into a successful wait");
        const auto failure=vm.error().code;require(vm.runTick().status==VmStatus::Faulted&&vm.error().code==failure,"held-host faults remain sticky");
    }
    std::cout<<"held host retains nativePC/time, resumes once and leaves other contexts running\n";
}
