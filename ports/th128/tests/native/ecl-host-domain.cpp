#include "ecl/EclVm.hpp"
#include "game/StageWorld.hpp"
#include "../ecl-fixture.hpp"
#include <cstdio>
#include <string>
#include <vector>

using namespace ecl_fixture;
using namespace th128::ecl;

static Program load(const std::vector<std::pair<std::string,Bytes>>& subroutines){
    const auto bytes=program(subroutines);Program result;std::string error;
    require(result.load(bytes.data(),bytes.size(),error),error);return result;
}
static std::vector<std::uint32_t> spawnWords(const std::string& subroutine,float x,float y,int life){
    auto words=stringWords(subroutine);
    words.insert(words.end(),{floatBits(x),floatBits(y),std::uint32_t(life),0,0});return words;
}
static th128::StageWorld* makeWorld(th128::StageWorld& world,const Bytes& data,std::string& reason){
    require(world.load("domain.ecl",data.data(),data.size(),reason)&&world.start("domain.ecl",1,0),reason);
    return &world;
}

int main(){
    try{
    {
        Vm vm(load({{"main",join({instruction(700,{500}),instruction(1)})}}));
        std::string error;require(vm.start("main"),error);
        require(vm.runTick().status==VmStatus::Faulted,"bare VM keeps the unknown 700 fault");
        require(vm.error().code==ErrorCode::UnsupportedOpcode&&vm.error().opcode==700
            &&vm.error().message.find("unregistered host opcode 700")!=std::string::npos,vm.error().message);
    }
    {
        Vm vm(load({{"main",join({instruction(999,{1}),instruction(1)})}}));
        std::string error;require(vm.start("main"),error);
        require(vm.runTick().status==VmStatus::Faulted,"bare VM keeps the unknown 999 fault");
        require(vm.error().opcode==999,"unknown 999 keeps its real opcode in the fault");
    }
    {
        Vm vm(load({{"main",join({instruction(700,{500}),instruction(1)})}}));
        std::string error;require(vm.enableOriginalHostMissDomain(error),error);
        require(vm.start("main"),error);
        const auto result=vm.runTick();
        require(result.status==VmStatus::Finished&&result.instructions==2,"explicit domain advances the out-of-table miss");
        const auto threads=vm.threads();
        require(threads.size()==1&&!threads[0].active,"the miss advances into the real terminal command");
    }
    {
        Vm vm(load({{"main",join({instruction(700,{},0,0,255,0),instruction(1)})}}));
        std::string error;require(vm.enableOriginalHostMissDomain(error),error);
        require(vm.start("main")&&vm.runTick().status==VmStatus::Finished,"zero-parameter 700 is ignored whole");
    }
    {
        Vm vm(load({{"main",join({instruction(700,{floatBits(2.5f)},0,1,255,1),instruction(1)})}}));
        std::string error;require(vm.enableOriginalHostMissDomain(error),error);
        require(vm.start("main")&&vm.runTick().status==VmStatus::Finished,
            "masked 700 operands are never read or resolved");
    }
    {
        Vm vm(load({{"main",join({instruction(700,{},0,0,255,0),instruction(1)})}}));
        std::string error;require(vm.start("main"),error);
        require(!vm.enableOriginalHostMissDomain(error)&&!error.empty(),"domain configuration is fresh-only");
        require(vm.status()==VmStatus::Running,"a rejected domain configuration does not poison the VM");
        Vm faulted(load({{"main",join({instruction(700,{1}),instruction(1)})}}));
        require(faulted.start("main")&&faulted.runTick().status==VmStatus::Faulted,"fault setup");
        require(!faulted.enableOriginalHostMissDomain(error),"a faulted VM cannot enable the domain");
    }
    {
        Vm vm(load({{"main",join({instruction(600,{1}),instruction(1)})}}));
        std::string error;require(vm.enableOriginalHostMissDomain(error),error);
        require(vm.start("main"),error);
        require(vm.runTick().status==VmStatus::Faulted&&vm.error().opcode==600,"in-table unregistered 600 still faults");
        require(vm.error().message.find("unregistered host opcode 600")!=std::string::npos,vm.error().message);
    }
    {
        Vm vm(load({{"main",join({instruction(700,{1}),instruction(1)})}}));
        std::string error;require(vm.enableOriginalHostMissDomain(error),error);
        require(vm.registerHostOpcode(700,[](HostCall&){return false;},error),error);
        require(vm.start("main"),error);
        require(vm.runTick().status==VmStatus::Faulted,"a registered callback error is never skipped by the domain");
        require(vm.error().message.find("host declined opcode 700")!=std::string::npos,vm.error().message);
    }
    {
        Vm vm(load({{"main",join({instruction(700,{1}),instruction(1)})}}));
        std::string error;require(vm.enableOriginalHostMissDomain(error),error);
        unsigned calls=0;require(vm.registerHostOpcode(700,[&](HostCall&){++calls;return true;},error),error);
        require(vm.start("main")&&vm.runTick().status==VmStatus::Finished&&calls==1,
            "a registered callback executes instead of the miss policy");
    }
    {
        Vm vm(load({{"main",join({instruction(700),instruction(1)})}}));
        std::string error;bool hold=true;unsigned calls=0;
        require(vm.registerHostOpcode(700,[&](HostCall& call){++calls;return hold?call.holdCurrentCommand():true;},error),error);
        require(vm.start("main"),error);
        require(vm.runTick().status==VmStatus::Running&&calls==1,"held host remains a wait");
        require(vm.threads()[0].offset==0&&vm.threads()[0].time==0,"hold retains the real cursor and clock");
        hold=false;
        require(vm.runTick().status==VmStatus::Finished&&calls==2,"released hold resumes the authored next command");
    }
    {
        const auto data=program({{"main",join({instruction(700,{500}),instruction(257,spawnWords("child",0,100,20),0,0,255,6),
            instruction(0,{},100)})},{"child",join({instruction(0,{},100)})}});
        th128::StageWorld world;std::string reason;
        makeWorld(world,data,reason);
        require(world.enableLocalEnemyPasses(reason),reason);
        require(world.spawnCount()==1&&world.entities().size()==2,
            "world 700 advances into the real spawn instead of a fault or wait");
        require(world.entities()[0].phaseTimer.current==1,"world root clock advances on the miss tick");
        require(world.tick()&&world.frame()==1,"world frame advances after the miss");
    }
    {
        const auto data=program({{"main",join({instruction(700,{},0,0,255,0),instruction(257,spawnWords("child",0,100,20),0,0,255,6),
            instruction(0,{},100)})},{"child",join({instruction(0,{},100)})}});
        th128::StageWorld world;std::string reason;
        makeWorld(world,data,reason);
        require(world.enableLocalEnemyPasses(reason),reason);
        require(world.spawnCount()==1,"zero-parameter world 700 still advances");
    }
    {
        const auto data=program({{"main",join({instruction(700,{floatBits(2.5f)},0,1,255,1),
            instruction(257,spawnWords("child",0,100,20),0,0,255,6),instruction(0,{},100)})},
            {"child",join({instruction(0,{},100)})}});
        th128::StageWorld world;std::string reason;
        makeWorld(world,data,reason);
        require(world.enableLocalEnemyPasses(reason),reason);
        require(world.spawnCount()==1,"world 700 masked operand is ignored without reading it");
    }
    {
        const auto data=program({{"main",join({instruction(600,{1}),instruction(257,spawnWords("child",0,100,20),0,0,255,6),
            instruction(0,{},100)})},{"child",join({instruction(0,{},100)})}});
        th128::StageWorld world;std::string reason;
        makeWorld(world,data,reason);
        require(!world.enableLocalEnemyPasses(reason),"world keeps in-table unregistered 600 as an explicit fault");
        require(world.faulted()&&world.error().find("unregistered host opcode 600")!=std::string::npos,world.error());
        require(world.spawnCount()==0,"a rejected in-table host opcode never executes the next spawn");
    }
    std::puts("PASS original host-miss domain is explicit, range-bounded and does not replace registered callbacks");
    return 0;
    }catch(const std::exception& error){std::fprintf(stderr,"FAIL ecl host domain: %s\n",error.what());return 1;}
}
