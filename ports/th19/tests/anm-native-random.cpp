#include "AnmVm.hpp"
#include "NativeRandom.hpp"
#include <iostream>
#include <cstring>
#include <cassert>
using namespace th19;
int main(){
    uint32_t state,expectedState,expectedBits;int signedValue;float scale;
    size_t count=0;
    while(std::cin>>state>>scale>>signedValue>>expectedState>>expectedBits){
        NativeRandom rng{state};
        const float result=signedValue?rng.signedUnit(scale):rng.unit(scale);
        uint32_t bits;std::memcpy(&bits,&result,4);
        assert(bits==expectedBits&&rng.state==expectedState);
        AnmVm vm;vm.random_scale=scale;AnmVm::restore_random(state);
        const float fromVm=vm.read_var_f(signedValue?10012:10011);
        std::memcpy(&bits,&fromVm,4);
        assert(bits==expectedBits&&AnmVm::random_checkpoint()==expectedState);
        // Restoring the rollback state reproduces the next sample exactly.
        AnmVm::restore_random(state);
        assert(fromVm==vm.read_var_f(signedValue?10012:10011));
        AnmVm::restore_random(state,true);
        AnmVm::restore_random(17);
        const float primary=vm.read_var_f(signedValue?10032:10031);
        std::memcpy(&bits,&primary,4);
        assert(bits==expectedBits&&AnmVm::random_checkpoint(true)==expectedState);
        assert(AnmVm::random_checkpoint()==17);
        ++count;
    }
    assert(count==234);
    AnmVm vm;vm.extra_ivar[2]=0;AnmVm::restore_random(1);
    assert(vm.read_var_i(10022)==0&&AnmVm::random_checkpoint()==1);
    for(uint32_t bound:{1u,7u,65535u,0xffffffffu}){
        NativeRandom expected{0x87654321};AnmVm::restore_random(expected.state);
        vm.extra_ivar[2]=int32_t(bound);
        assert(uint32_t(vm.read_var_i(10022))==expected.bounded(bound));
        assert(AnmVm::random_checkpoint()==expected.state);
    }
    NativeRandom raw{1};AnmVm::restore_random(1);
    assert(vm.read_var_f(10022)==float(raw.next()));
    AnmVm::restore_random(123);AnmVm::restore_random(456,true);
    for(int id:{10010,10011,10012,10030,10031,10032,10038})assert(vm.read_var_i(id)==id);
    assert(vm.read_var_f(10038)==10038.f);
    assert(AnmVm::random_checkpoint()==123&&AnmVm::random_checkpoint(true)==456);
    std::cout<<"PASS: 234 native float bit patterns through RNG and ANM; rollback and bounded draws\n";
}
