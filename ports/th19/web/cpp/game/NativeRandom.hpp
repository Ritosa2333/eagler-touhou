#pragma once
#include <cstdint>

namespace th19 {
// v1.10c 0x402540, 0x402630, 0x404bd0, 0x404c90.
// Default range of the RNG object used by ANM (0x5c6638).
// Stream ownership/call ordering is separate from this arithmetic primitive.
struct NativeRandom {
    uint32_t state=1;
    static constexpr uint32_t range=0xffff00u;
    uint32_t next(){
        state=uint32_t(uint64_t(state)*48271u%2147483647u);
        return state%range;
    }
    uint32_t bounded(uint32_t bound){return bound?next()%bound:0;}
    float unit(float scale=1.f){return (float(next())/(float(range)-1.f))*scale;}
    float signedUnit(float scale=1.f){return (float(next())/(float(range)/2.f-1.f)-1.f)*scale;}
};
}
