#include "game/PlayerFireHost.hpp"
#include <cassert>
#include <algorithm>
#include <iostream>
#include <vector>
int main(){
    using namespace th128::player;
    fire::Host host;std::string error;int power=10000;std::vector<int> emitted,sounds;unsigned activations=0;
    auto emit=[&](int count,std::string&){emitted.push_back(count);return true;};
    auto sound=[&](int operation,int identifier,std::string&){sounds.push_back(operation*1000+identifier);return true;};
    auto freeze=[&](int saved,std::string&){assert(saved>=3000);++activations;return true;};
    assert(host.configure(-1,{-1,0,0,0x4b6f58,1},error));
    assert(host.tick(1,{1,1,0},0,0,power,emit,sound,freeze,error));assert(emitted.front()==0&&host.state().held.current==1);
    assert(host.tick(1,{0,0,1},0,0,power,emit,sound,freeze,error));assert(host.state().held.current==0&&activations==0);
    for(int tick=0;tick<20;++tick)assert(host.tick(1,{},0,0,power,emit,sound,freeze,error));
    emitted.clear();sounds.clear();
    for(int tick=0;tick<45;++tick)assert(host.tick(1,{1,std::uint16_t(tick==0),0},0,0,power,emit,sound,freeze,error));
    assert(host.state().held.current==45);assert(host.state().savedPower==10000);
    assert(std::count(sounds.begin(),sounds.end(),31)==1&&std::count(sounds.begin(),sounds.end(),70)==1);
    assert(emitted.size()==15&&emitted.front()==0&&emitted.back()==14);
    assert(host.tick(1,{0,0,1},0,0,power,emit,sound,freeze,error));assert(activations==1&&power==0&&host.state().held.current==-40);
    for(int tick=0;tick<40;++tick)assert(host.tick(1,{},0,0,power,emit,sound,freeze,error));
    assert(power==0&&host.state().held.current==0);
    assert(host.tick(1,{},0,0,power,emit,sound,freeze,error));assert(power==5);
    fire::Host dialogue;assert(dialogue.configure(6,{44,45,45,0x4b6f58,1},error));sounds.clear();
    assert(dialogue.suspend(sound,error));assert(dialogue.state().shot==-1&&dialogue.state().held.current==0&&dialogue.state().held.previous==-1);
    assert((sounds==std::vector<int>{1031,1070}));assert(activations==1);
    fire::Host failure;assert(failure.configure(-1,{-1,0,0,0x4b6f58,1},error));
    const auto rejected=[](int,std::string& text){text="required emit rejected";return false;};
    assert(!failure.tick(1,{1,1,0},0,0,power,rejected,sound,freeze,error));
    const auto retained=power;assert(!failure.tick(1,{},0,0,power,emit,sound,freeze,error));assert(power==retained);
    std::cout<<"real Z tap/hold/release shot timer, sounds, mandatory freeze and recovery passed\n";
}
