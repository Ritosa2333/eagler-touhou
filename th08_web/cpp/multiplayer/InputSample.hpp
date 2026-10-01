#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Authoritative input samples are multiplayer-only
#endif
#include <eagler/netplay/NetplayProtocol.hpp>
#include <cmath>
#include <cstdint>

namespace th08::multiplayer {
inline constexpr float route_bootstrap_x=8188.0f;
inline constexpr float route_bootstrap_y=-8192.0f;

// A fully initialized movement payload inside native Player state. Transport
// flags are explicit; physical device state never enters the rollback owner.
struct AnalogInput {
    float x=0,y=0;
    std::uint8_t mode=0,unlimited=0,touchUsed=0,touchBomb=0;
};
static_assert(sizeof(AnalogInput)==12);
// Unapplied physical drag belongs to the rewindable player, not the SDL
// producer. Keep this representation padding-free for canonical hashing.
struct TouchRemainder {float x=0,y=0;std::uint32_t active=0;};
static_assert(sizeof(TouchRemainder)==12);
inline bool ValidInputSample(const Netplay::FrameInput& in) noexcept {
    if(!Netplay::IsValidFrameInput(in)||(in.buttons&0x8000u)||
       (in.touchBomb&&!(in.buttons&2)))return false;
    switch(in.analogMode){
    case Netplay::AnalogMode::None:return in.x==0&&in.y==0&&!in.unlimited;
    case Netplay::AnalogMode::Joystick:return std::abs(in.x)<=1&&std::abs(in.y)<=1&&!in.unlimited;
    case Netplay::AnalogMode::DirectTouch:
    case Netplay::AnalogMode::DirectTouchDelta:
    case Netplay::AnalogMode::DirectTouchBegin:return std::abs(in.x)<=8192&&std::abs(in.y)<=8192;
    default:return false;
    }
}
inline AnalogInput MovementSample(const Netplay::FrameInput& in) noexcept {
    return {in.x,in.y,std::uint8_t(in.analogMode),std::uint8_t(in.unlimited),std::uint8_t(in.touchUsed),std::uint8_t(in.touchBomb)};
}
inline Netplay::FrameInput RouteBootstrap(Netplay::FrameInput input,std::uint8_t route) noexcept {
    input.analogMode=Netplay::AnalogMode::DirectTouch;
    input.x=route_bootstrap_x+float(route);input.y=route_bootstrap_y;
    input.unlimited=false;input.touchUsed=false;input.touchBomb=false;return input;
}
inline bool DecodeRouteBootstrap(const Netplay::FrameInput& input,std::uint8_t& route) noexcept {
    if(input.analogMode!=Netplay::AnalogMode::DirectTouch||input.unlimited||input.touchUsed||input.touchBomb||
       input.y!=route_bootstrap_y||input.x<route_bootstrap_x||input.x>route_bootstrap_x+2.0f)return false;
    const float value=input.x-route_bootstrap_x;const auto candidate=std::uint8_t(value);
    if(value!=float(candidate)||candidate>2)return false;route=candidate;return true;
}
inline Netplay::FrameInput StripRouteBootstrap(const Netplay::FrameInput& input) noexcept {
    return Netplay::FrameInput(input.buttons);
}
}
