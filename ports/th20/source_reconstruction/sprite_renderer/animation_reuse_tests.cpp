#include "animation.hpp"
#include <cassert>
#include <cstring>
#include <cstdio>
#include <initializer_list>
using namespace th20::source::sprite;
int main(){
    // Alternate poisoned lifetimes: reset must be independent of old state,
    // including padding, timer flags, callback addresses and mesh pointers.
    for(unsigned id:{0u,1u,0xfffeu,0xffffu}){
        Animation expected{};expected.index=id;reset_animation_state(expected);
        for(unsigned pattern:{0x55u,0xaau,0xfeu}){
            struct {unsigned before;Animation vm;unsigned after;} storage;
            storage.before=0x12345678;storage.after=0x87654321;
            std::memset(&storage.vm,pattern,sizeof(storage.vm));storage.vm.index=id;
            reset_animation_state(storage.vm);
            assert(std::memcmp(&storage.vm,&expected,sizeof(expected))==0);
            assert(storage.before==0x12345678&&storage.after==0x87654321);
            assert(storage.vm.index==id&&storage.vm.base.vector_50.x==1.f);
            assert(storage.vm.fields_550[5]==0&&storage.vm.fields_550[6]==0);
            assert(storage.vm.timer_4c8.flags==0&&storage.vm.base.timer.flags==0);
        }
    }
    std::puts("PASS: 12 poisoned ANM lifetimes; pool IDs and adjacent storage preserved");
}
