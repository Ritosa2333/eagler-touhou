#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Authoritative input samples are multiplayer-only
#endif
#include <eagler/netplay/NetplayProtocol.hpp>
#include <cmath>
#include <cstdint>

namespace th08::multiplayer {
// A fully initialized movement payload inside native Player state. Transport
// flags are explicit; physical device state never enters the rollback owner.
struct AnalogInput {
    float x=0,y=0;
    std::uint8_t mode=0,unlimited=0,touchUsed=0,touchBomb=0;
};
static_assert(sizeof(AnalogInput)==12);
inline bool ValidInputSample(const Netplay::FrameInput& in) noexcept {
    if(!Netplay::IsValidFrameInput(in)||(in.buttons&0x8000u)||
       (in.touchBomb&&!(in.buttons&2)))return false;
    switch(in.analogMode){
    case Netplay::AnalogMode::None:return in.x==0&&in.y==0&&!in.unlimited;
    case Netplay::AnalogMode::Joystick:return std::abs(in.x)<=1&&std::abs(in.y)<=1&&!in.unlimited;
    case Netplay::AnalogMode::DirectTouch:return std::abs(in.x)<=8192&&std::abs(in.y)<=8192;
    default:return false;
    }
}
inline AnalogInput MovementSample(const Netplay::FrameInput& in) noexcept {
    return {in.x,in.y,std::uint8_t(in.analogMode),std::uint8_t(in.unlimited),std::uint8_t(in.touchUsed),std::uint8_t(in.touchBomb)};
}
}
