#include "game/TransientEffects.hpp"
#include <cassert>
#include <cmath>
#include <fstream>
#include <iterator>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "engine/Random.hpp"

int main(int argc,char** argv){
    assert(argc==2);
    std::ifstream stream(argv[1],std::ios::binary);
    const std::vector<unsigned char> bytes((std::istreambuf_iterator<char>(stream)),{});
    th128::TransientEffects effects;std::string error;
    assert(effects.load(bytes.data(),bytes.size(),0x1234,error));
    assert(effects.setDisplaySeed(0x1234));
    th128::Random gameRandom;gameRandom.seed=0x1234;
    assert(effects.setGameRandom([&gameRandom]{return gameRandom.next32();}));
    if(!effects.spawn(175,{-100,300,.1f},23,error)){std::cerr<<error<<'\n';return 2;}
    assert(effects.snapshots().size()==1&&effects.displayRandomCalls()==0&&gameRandom.calls==2);
    assert(!effects.setDisplaySeed(0));
    const auto first=effects.snapshots().at(0);
    assert(first.script==175&&first.sprite==428&&first.ticks==1);
    assert(std::isfinite(first.x)&&std::isfinite(first.y)&&std::abs(first.x-220)<64&&std::abs(first.y-316)<64);
    for(unsigned tick=0;tick<14;++tick)assert(effects.tick(error));
    assert(effects.snapshots().empty());
    assert(effects.setGameRandom([]()->std::uint32_t{throw std::runtime_error("effect RNG fixture");}));
    bool escaped=false;
    try{assert(!effects.spawn(175,{0,0,0},23,error));}catch(...){escaped=true;}
    assert(!escaped&&error.find("callback threw")!=std::string::npos);
    assert(effects.setGameRandom([&gameRandom]{return gameRandom.next32();}));
    assert(!effects.spawn(9999,{0,0,0},23,error));
    assert(effects.snapshots().empty());
    assert(effects.spawn(83,{0,400,0},0,error));
    assert(effects.snapshots().at(0).sprite==446);
    for(unsigned index=0;index<32;++index)assert(effects.spawn(84,{0,400,0},0,error));
    assert(effects.snapshots().size()==33);
    for(unsigned tick=0;tick<45;++tick)assert(effects.tick(error));
    assert(effects.snapshots().empty());
    return 0;
}
