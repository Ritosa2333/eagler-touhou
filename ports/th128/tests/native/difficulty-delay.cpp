#include "ecl/EclVm.hpp"
#include "../ecl-fixture.hpp"
#include <cassert>
int main(){
    using namespace ecl_fixture;using namespace th128::ecl;
    const auto bytes=program({{"main",join({instruction(448,{5,5,1,1}),instruction(600,{}),instruction(10,{})})}});
    Program program;std::string error;assert(program.load(bytes.data(),bytes.size(),error));Vm vm(program);unsigned calls=0;
    assert(vm.registerHostOpcode(448,[](HostCall& call){int selected=0;return call.readInteger(1,selected)&&call.subtractTime(selected);},error));
    assert(vm.registerHostOpcode(600,[&](HostCall& call){++calls;return call.time()==0;},error));assert(vm.start("main"));
    assert(vm.runTick().status==VmStatus::Running);assert(vm.threads()[0].time==-4&&calls==0);
    for(int frame=0;frame<4;++frame){assert(vm.runTick().status==VmStatus::Running);assert(calls==0);}
    assert(vm.runTick().status==VmStatus::Finished&&calls==1);
}
