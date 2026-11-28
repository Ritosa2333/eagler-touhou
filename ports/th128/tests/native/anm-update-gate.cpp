#include "animation/AnmLogic.hpp"
#include "../ecl-fixture.hpp"
#include <cassert>
#include <iostream>
using namespace ecl_fixture;

Bytes animation(std::int32_t value){
    Bytes bytes(108);put32(bytes,0,7);put16(bytes,6,1);put16(bytes,10,128);put16(bytes,12,64);
    put32(bytes,16,72);bytes[72]='x';put32(bytes,64,0);put32(bytes,68,80);
    put16(bytes,80,111);put16(bytes,82,12);put32(bytes,88,std::uint32_t(value));
    put16(bytes,92,63);put16(bytes,94,8);put16(bytes,100,65535);return bytes;
}

int main(){
    th128::anm::Registry registry;std::string error;const auto initial=animation(0);
    assert(registry.loadBank(0,initial.data(),initial.size(),error));
    char command=0;
    while(std::cin>>command){
        th128::anm::VM vm;th128::anm::Runtime runtime;
        runtime.updateControl=th128::anm::UpdateControl{false,0};
        assert(vm.bind(registry,0,0,runtime));
        if(command=='H'){
            std::uint32_t previous=0;std::int32_t value=0;std::cin>>previous>>value;
            const auto bytes=animation(value);assert(registry.loadBank(1,bytes.data(),bytes.size(),error));
            vm.control().flags2=previous;assert(vm.rebindWithoutReset(registry,1,0,runtime));assert(!vm.faulted());
            std::cout<<vm.state().flags2<<'\n';
        }else if(command=='G'){
            std::uint32_t flags=0,controlFlags=0;int irq=0,present=0;std::cin>>flags>>irq>>present>>controlFlags;
            vm.control().flags2=flags;runtime.updateControl=th128::anm::UpdateControl{present!=0,controlFlags};
            if(irq)vm.interrupt(irq);
            const auto before=vm.state();const auto diagnostic=vm.diagnostics();vm.tick(runtime);
            assert(!vm.faulted());const bool paused=before.ticks==vm.state().ticks;
            if(paused){assert(vm.state().clock==before.clock&&vm.state().held==before.held);
                assert(vm.diagnostics().pc==diagnostic.pc&&vm.diagnostics().pendingInterrupt==diagnostic.pendingInterrupt);}
            else assert(vm.state().ticks==before.ticks+1);
            std::cout<<paused<<'\n';
        }else return 2;
    }
    th128::anm::VM missing;th128::anm::Runtime unknown;
    assert(missing.bind(registry,0,0,unknown));missing.control().flags2=0x40;
    const auto before=missing.state();const auto diagnostic=missing.diagnostics();missing.tick(unknown);
    assert(missing.faulted()&&missing.diagnostics().faultOpcode==111);
    assert(missing.state().ticks==before.ticks&&missing.state().clock==before.clock&&missing.diagnostics().pc==diagnostic.pc);
    const auto retained=missing.diagnostics().reason;missing.tick(unknown);assert(missing.diagnostics().reason==retained);
}
