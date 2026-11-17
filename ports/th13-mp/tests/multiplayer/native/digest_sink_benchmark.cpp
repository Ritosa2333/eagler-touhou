#include "multiplayer/DigestSink.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <vector>
using th13::multiplayer::DigestSink;

// Independent literal bytewise FNV reference, with no zero-run optimization.
struct Reference {
    uint64_t h=0xcbf29ce484222325ull;
    void byte(uint8_t x) { h=(h^x)*0x100000001b3ull; }
    void u32(uint32_t x) { for(unsigned i=0;i<4;++i) byte(uint8_t(x>>(i*8))); }
    void u64(uint64_t x) { u32(uint32_t(x));u32(uint32_t(x>>32)); }
    void tag(const char* s) { byte(uint8_t(0x9e3779b97f4a7c15ull^std::strlen(s))); while(*s)byte(uint8_t(*s++));byte(0); }
};
static uint64_t rng=0xd1b54a32d192ed03ull;
static uint32_t next() { rng^=rng<<13;rng^=rng>>7;rng^=rng<<17;return uint32_t(rng); }
int main() {
    DigestSink sink; Reference ref;
    auto check=[&] { if(sink.value()!=ref.h) { std::printf("MISMATCH %016llx %016llx\n",(unsigned long long)sink.value(),(unsigned long long)ref.h);return false; }return true; };
    const uint32_t bits[]={0,0x80000000u,0x7f800000u,0xff800000u,0x7fc00000u,0x7fa12345u,0xffffffffu,0x01000000u,0x00000001u};
    for(uint32_t x:bits) { float f;std::memcpy(&f,&x,4);sink.f32(f);ref.u32(x);if(!check())return 1; }
    for(unsigned n : {0u,1u,255u,256u,1023u,1024u,1025u,8192u}) {
        for(unsigned i=0;i<n;++i) {sink.u8(0);ref.byte(0);} if(!check())return 1;
        sink.u32(0x01000100);ref.u32(0x01000100);if(!check())return 1;
    }
    for(unsigned i=0;i<1000000;++i) {
        const uint32_t x=i%4?next():0;
        switch(i%7) {
        case 0:sink.u8(uint8_t(x));ref.byte(uint8_t(x));break;
        case 1:{const uint64_t value=(uint64_t(x)<<32)|next();sink.u64(value);ref.u64(value);break;}
        case 2:sink.tag("zero-run");ref.tag("zero-run");break;
        case 3:sink.i32(static_cast<int32_t>(x));ref.u32(x);break;
        case 4:{float f;std::memcpy(&f,&x,4);sink.f32(f);ref.u32(x);break;}
        default:sink.u32(x);ref.u32(x);break;
        }
        if((i%997)==0&&!check())return 1;
    }
    if(!check())return 1;
    auto wrong=ref;wrong.byte(1);if(sink.value()==wrong.h)return 2;
    std::vector<uint32_t> data(1<<18);
    for(auto& x:data) {x=next();if((x&7)<5)x=0;}
    volatile uint64_t result=0;
    std::array<double,7> elapsed{};
    for(double& t:elapsed) {
        auto start=std::chrono::steady_clock::now();
        for(unsigned r=0;r<128;++r) {
            DigestSink d;
            for(uint32_t x:data) {
                float f;std::memcpy(&f,&x,4);
                d.f32(f); d.i32(static_cast<int32_t>(x));
                d.u64((uint64_t(x)<<32)|x);
            }
            result^=d.value();
        }
        t=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();
    }
    std::sort(elapsed.begin(),elapsed.end());
    std::printf("PASS reference million operations; median_ms=%.3f min_ms=%.3f max_ms=%.3f result=%llu\n",elapsed[3],elapsed[0],elapsed[6],(unsigned long long)result);
}
