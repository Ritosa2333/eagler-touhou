#include "engine/Random.hpp"
#include <cassert>
#include <cstdio>
#include <cstdint>
#include <cstring>

int main() {
    th128::Random generator;
    assert(generator.next16() == 0xc374);
    assert(generator.seed == 0xc374 && generator.calls == 1);
    generator = th128::Random{};
    assert(generator.next32() == 0x30ddeff1u);
    assert(generator.seed == 49095 && generator.calls == 2);
    generator.seed = 0x1234;
    generator.calls = 0;
    assert(generator.next32() == 0x1eb187a1u);
    assert(generator.seed == 7814 && generator.calls == 2);
    generator.seed = 0xffff;
    generator.calls = 0;
    assert(generator.next16() == 0x11f0);
    generator.seed = 0;
    generator.calls = UINT32_MAX;
    assert(generator.next32() == 0x30ddeff1u);
    assert(generator.calls == 1);
    generator.seed=1;generator.calls=0;
    const float angle=generator.signedAngle();
    std::uint32_t angleBits=0;std::memcpy(&angleBits,&angle,4);
    assert(angleBits==0xbff89771u&&generator.calls==2);
    std::puts("native RNG vectors and unsigned counter wrap verified");
}
