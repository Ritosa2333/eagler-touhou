#include "../th11_web/cpp/game/Ending.hpp"
#include <cassert>
#include <cstdio>
using namespace th11;
int main(){
    AnmManager animations;AnmResource text;ScoreFile scores;Ending ending(animations,text,scores);
    std::array<u8,12> bytes{};u16 timestamp=300;std::memcpy(bytes.data(),&timestamp,2);bytes[2]=5;bytes[3]=4;
    i32 wait=600;std::memcpy(bytes.data()+4,&wait,4);timestamp=301;std::memcpy(bytes.data()+8,&timestamp,2);
    auto initialize=[&](u32 flags){ending.active=true;ending.flags=flags;ending.frames=0;ending.seen=3;ending.instruction=bytes.data();ending.end=bytes.data()+bytes.size();ending.time.set(0,&animations.rate);ending.wait.set(0,&animations.rate);ending.elapsed.set(0,&animations.rate);};
    initialize(1);assert(ending.update(0,1));assert(ending.instruction==bytes.data()+8&&ending.wait.current==0);
    assert(ending.update(0,0));assert(!ending.active);
    initialize(1);assert(ending.update(512,0));for(int i=0;i<8&&ending.instruction==bytes.data();++i)assert(ending.update(512,0));assert(ending.instruction==bytes.data()+8);
    initialize(1);ending.time.set(300,&animations.rate);assert(ending.update(0,0));assert(ending.wait.current==599);
    assert(ending.update(0,0x80000));assert(ending.wait.current==0&&ending.instruction==bytes.data()+8);
    initialize(2);assert(ending.tick(512,0));assert(ending.frames==12&&ending.time.current==12);
    initialize(2);assert(ending.update(0,1));assert(ending.instruction==bytes.data()+8);
    wait=-1;std::memcpy(bytes.data()+4,&wait,4);initialize(1);ending.time.set(300,&animations.rate);
    assert(ending.update(0,0));assert(ending.instruction==bytes.data());assert(ending.update(512,0));assert(ending.instruction==bytes.data()+8);
    std::puts("TH11 first-view Ending / timestamp Z / wait Z-Enter / first-view staff Ctrl: PASS");
}
