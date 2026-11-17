#include "../../cpp/game/AnmInterpolators.hpp"
#include "../../cpp/multiplayer/SnapshotCompare.hpp"
#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace {
constexpr auto bytes=sizeof(th17::AnmInterpolators);
using Clock=std::chrono::steady_clock;
struct Pair {std::array<unsigned char,bytes> first,second;};
std::array<Pair,1024> pairs;
__attribute__((noinline)) bool baseline(const void* a,const void* b){return std::memcmp(a,b,bytes)==0;}
__attribute__((noinline)) bool grouped(const void* a,const void* b){return th17::multiplayer::snapshot_equal<bytes>(a,b);}
}
int main(){
    unsigned seed=1234;
    for(size_t i=0;i<pairs.size();++i){
        auto& p=pairs[i];
        for(auto& byte:p.first){seed=seed*1664525u+1013904223u;byte=seed>>24;}
        p.second=p.first;
        if(i%4==0)p.second[(seed>>8)%bytes]^=1;
    }
    constexpr unsigned iterations=2400000;
    for(unsigned trial=0;trial<7;++trial)for(unsigned order=0;order<2;++order){
        const bool optimized=((trial+order)%2)!=0;
        const auto compare=optimized?grouped:baseline;
        unsigned matches=0;
        const auto begin=Clock::now();
        for(unsigned i=0;i<iterations;++i){const auto& p=pairs[i%pairs.size()];matches+=compare(p.first.data(),p.second.data());}
        const double milliseconds=std::chrono::duration<double,std::milli>(Clock::now()-begin).count();
        if(matches!=iterations/4*3)std::abort();
        std::printf("{\"trial\":%u,\"method\":\"%s\",\"bytes\":%zu,\"iterations\":%u,\"matches\":%u,\"milliseconds\":%.3f}\n",
                    trial,optimized?"grouped":"memcmp",bytes,iterations,matches,milliseconds);
    }
}
