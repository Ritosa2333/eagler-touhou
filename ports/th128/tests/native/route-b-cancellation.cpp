#include "game/CancellationEffects.hpp"
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
using Bytes=std::vector<std::uint8_t>;
static Bytes read(const std::string& path){std::ifstream file(path,std::ios::binary);assert(file);return {std::istreambuf_iterator<char>(file),{}};}
static bool require(bool ok,const std::string& message){if(!ok){std::cerr<<message<<'\n';return false;}return true;}
int main(int argc,char** argv){
    if(argc!=3)return 1;
    const auto animation=read(argv[1]),profile=read(argv[2]);
    if(!require(profile.size()==288,"type20 profile size"))return 1;
    int32_t words[72]{};
    for(std::size_t index=0;index<72;++index)
        words[index]=int32_t(std::uint32_t(profile[index*4])|std::uint32_t(profile[index*4+1])<<8|
            std::uint32_t(profile[index*4+2])<<16|std::uint32_t(profile[index*4+3])<<24);
    const int script=words[4+4*4];
    if(!require(script==377,"type20 color4 cancellation script must be 377"))return 1;
    th128::effects::CancellationEffects effects;std::string error;
    if(!require(effects.load(animation.data(),animation.size(),error),error))return 1;
    if(!require(effects.setGameRandom([](){return 0x12345678u;}),"rng"))return 1;
    const auto handle=effects.spawn(script,320.f,16.f,0.f,23,error);
    if(!require(handle.has_value(),error))return 1;
    if(!require(effects.rootCount()==1&&effects.nodeCount()==4,"377/376 family must own root and three children"))return 1;
    bool root=false,child=false;
    for(const auto& snapshot:effects.snapshots()){
        if(snapshot.state.script==377)root=true;
        if(snapshot.state.script==376)child=true;
    }
    if(!require(root&&child,"377/376 snapshots"))return 1;
    const auto unknown=effects.spawn(379,320.f,16.f,0.f,23,error);
    if(!require(!unknown.has_value()&&error.find("script=379")!=std::string::npos,error))return 1;
    std::cout<<"PASS type20 color4 cancellation 377/376\n";
}
