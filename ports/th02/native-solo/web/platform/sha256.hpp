#pragma once
#include <array>
#include <bit>
#include <cstdint>
#include <span>

namespace soew::web {
// FIPS 180-4, sections 4.2.2 and 6.2. The Win32-compatible digest call is synchronous.
inline std::array<std::uint8_t,32> sha256(std::span<const std::uint8_t> input){
    constexpr std::array<std::uint32_t,64> k={
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2,
    };
    std::array<std::uint32_t,8> hash={0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19};
    const auto block=[&](std::span<const std::uint8_t,64> data){
        std::array<std::uint32_t,64> w;
        for(unsigned i=0;i<16;++i)
            w[i]=(std::uint32_t(data[4*i])<<24)|(std::uint32_t(data[4*i+1])<<16)|(std::uint32_t(data[4*i+2])<<8)|data[4*i+3];
        for(unsigned i=16;i<64;++i){
            const auto x=w[i-15],y=w[i-2];
            w[i]=w[i-16]+(std::rotr(x,7)^std::rotr(x,18)^(x>>3))+w[i-7]+(std::rotr(y,17)^std::rotr(y,19)^(y>>10));
        }
        auto [a,b,c,d,e,f,g,h]=hash;
        for(unsigned i=0;i<64;++i){
            const auto t=h+(std::rotr(e,6)^std::rotr(e,11)^std::rotr(e,25))+((e&f)^(~e&g))+k[i]+w[i];
            const auto u=(std::rotr(a,2)^std::rotr(a,13)^std::rotr(a,22))+((a&b)^(a&c)^(b&c));
            h=g;g=f;f=e;e=d+t;d=c;c=b;b=a;a=t+u;
        }
        const std::array result={a,b,c,d,e,f,g,h};
        for(unsigned i=0;i<8;++i)hash[i]+=result[i];
    };
    const auto bits=std::uint64_t(input.size())*8;
    while(input.size()>=64){block(input.first<64>());input=input.subspan(64);}
    std::array<std::uint8_t,64> tail{};
    for(unsigned i=0;i<input.size();++i)tail[i]=input[i];
    tail[input.size()]=0x80;
    if(input.size()>=56){block(tail);tail.fill(0);}
    for(unsigned i=0;i<8;++i)tail[63-i]=std::uint8_t(bits>>(8*i));
    block(tail);
    std::array<std::uint8_t,32> out;
    for(unsigned i=0;i<32;++i)out[i]=std::uint8_t(hash[i/4]>>(24-8*(i%4)));
    return out;
}
}
