#pragma once
#include "PlayerRules.hpp"
#include <algorithm>
#include <cmath>

namespace th128::player {
struct TouchMotion {
    unsigned mode=0;
    std::int32_t x=0,y=0;
    bool operator==(const TouchMotion&) const=default;
};
inline constexpr unsigned SweptUnlimitedTouch = 4;
inline bool unlimitedTouch(unsigned mode) noexcept {return mode==2||mode==SweptUnlimitedTouch;}
inline bool validTouch(TouchMotion command) noexcept {
    if(command.mode==0)return command.x==0&&command.y==0;
    if(command.mode==1||unlimitedTouch(command.mode))return command.x>=MinimumHorizontalRaw&&command.x<=MaximumHorizontalRaw
        &&command.y>=MinimumVerticalRaw&&command.y<=MaximumVerticalRaw;
    return command.mode==3&&command.x>=-32767&&command.x<=32767&&command.y>=-32767&&command.y<=32767;
}
inline bool usedUnlimitedTouchMovement(RawPosition before,const Movement& movement,TouchMotion command,Input input) noexcept {
    return unlimitedTouch(command.mode)&&!(input.holding&(UpKey|DownKey|LeftKey|RightKey))
        &&(movement.position.horizontal!=before.horizontal||movement.position.vertical!=before.vertical);
}
inline Movement moveTouch(Movement base,RawPosition position,TouchMotion command) noexcept {
    if(!command.mode||!validTouch(command))return base;
    double dx=command.mode==3?command.x:command.x-position.horizontal;
    double dy=command.mode==3?command.y:command.y-position.vertical;
    const double speed=base.focused?FocusedCardinalRaw:NormalCardinalRaw;
    if(command.mode==3){const auto scale=speed/std::max(32767.,std::hypot(dx,dy));dx*=scale;dy*=scale;}
    else if(command.mode==1){const auto length=std::hypot(dx,dy);if(length>speed){dx*=speed/length;dy*=speed/length;}}
    const RawDelta delta{std::int32_t(dx),std::int32_t(dy)};
    const auto moved=advancePosition(position,delta);
    std::uint16_t keys=0;
    if(delta.horizontal<0)keys|=LeftKey;else if(delta.horizontal>0)keys|=RightKey;
    if(delta.vertical<0)keys|=UpKey;else if(delta.vertical>0)keys|=DownKey;
    return {direction({keys,0,0}),base.focused,delta,moved};
}
}
