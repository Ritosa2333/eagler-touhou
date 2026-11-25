#pragma once
#include <array>
#include <cstdint>
#include <optional>
#include <stdexcept>
namespace soew {
// ReC98 libs/master.lib/random.asm:47-65. Explicit modulo-2^32 update.
struct IRand {
    std::uint32_t state;
    explicit IRand(std::uint32_t seed):state(seed){}
    std::uint16_t next() {
        state=static_cast<std::uint32_t>(std::uint64_t(state)*0x015a4e35u+1u);
        return static_cast<std::uint16_t>((state>>16)&0x7fffu);
    }
};
// Both TH02 compiled instances share one byte cursor and one buffer.
struct RandomRing {
    std::array<std::uint8_t,256> bytes{};
    std::uint8_t cursor=0;
    // randring[bss].asm places cursor immediately after the 256-byte buffer.
    // next16 increments cursor BEFORE loading the word, so at index255 its
    // high byte reads the now-wrapped cursor (0), not bytes[0].
    void fill(IRand &rng) {for(auto &b:bytes)b=static_cast<std::uint8_t>(rng.next());}
    std::uint8_t next8() {const auto i=cursor;cursor=static_cast<std::uint8_t>(cursor+1);return bytes[i];}
    std::uint8_t masked(std::uint8_t mask) {return next8()&mask;}
    std::uint16_t next16() {
        const auto i=cursor;
        cursor=static_cast<std::uint8_t>(cursor+1);
        const auto high=i==255?cursor:bytes[static_cast<unsigned>(i)+1];
        return static_cast<std::uint16_t>(bytes[i]|(std::uint16_t(high)<<8));
    }
};
// Source input masks: th02/hardware/input.hpp. These are NOT frontend Menu bits.
enum Input : std::uint16_t {Up=1,Down=2,Left=4,Right=8,Shot=16,Bomb=32,Cancel=64,OK=128,Q=256,
                            Focus=0x0400,UpLeft=0x1000,UpRight=0x2000,DownLeft=0x4000,DownRight=0x8000};
// Focus is a native extension in a previously unused replay input bit.
inline constexpr std::uint16_t native_input_mask=Up|Down|Left|Right|Shot|Bomb|Cancel|OK|Q|Focus|UpLeft|UpRight|DownLeft|DownRight;
struct Keys {
    bool up=false,down=false,left=false,right=false;
    bool n8=false,n2=false,n4=false,n6=false,n7=false,n9=false,n1=false,n3=false;
    bool z=false,x=false,space=false,enter=false,escape=false,q=false;
    bool focus=false;
};
inline std::uint16_t sense(const Keys &k) {
    std::uint16_t mask=0;
    if(k.up||k.n8)mask|=Up;
    if(k.down||k.n2)mask|=Down;
    if(k.left||k.n4)mask|=Left;
    if(k.right||k.n6)mask|=Right;
    if(k.n7)mask|=UpLeft;
    if(k.n9)mask|=UpRight;
    if(k.n1)mask|=DownLeft;
    if(k.n3)mask|=DownRight;
    if(k.z||k.space)mask|=Shot;
    if(k.x)mask|=Bomb;
    if(k.escape)mask|=Cancel;
    if(k.enter)mask|=OK;
    if(k.q)mask|=Q;
    if(k.focus)mask|=Focus;
    return mask;
}
}
