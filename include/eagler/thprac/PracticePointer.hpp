// Interactive-window ownership is decided on down. Read-only panels defer
// tap/swipe classification; crossing a window never steals a game-owned drag.
#pragma once
#include <array>
#include <cstdint>

namespace eagler::thprac {
class PracticePointer {
    struct Slot { std::int64_t id = 0; bool active = false; };
    std::array<Slot, 32> ui_{};
    struct Pending { std::int64_t id=0; bool active=false; float x=0,y=0; };
    std::array<Pending, 32> pending_{};
public:
    enum class TouchRoute { Game, Ui, Pending, BeginGame, Tap };
    struct TouchDecision { TouchRoute route; float x,y; };
    void reset() { for (auto& slot : ui_) slot.active = false; for(auto& slot:pending_)slot.active=false; }
    // Read-only panels retain taps, but a swipe starting there belongs to the
    // game. Replay its original down before the first move, never jump from
    // the threshold position. Coordinates are logical game pixels.
    TouchDecision route_touch(unsigned type,std::int64_t id,bool hit,bool passive,float x,float y) {
        // A reused finger id starts a fresh gesture even if its previous UP
        // was lost. This applies across interactive and read-only panels.
        if(type==0){
            for(auto& slot:ui_)if(slot.active&&slot.id==id)slot.active=false;
            for(auto& slot:pending_)if(slot.active&&slot.id==id)slot.active=false;
        }
        for(auto& slot:pending_)if(slot.active&&slot.id==id){
            const float dx=x-slot.x,dy=y-slot.y;
            if(dx*dx+dy*dy>64.f){slot.active=false;return {TouchRoute::BeginGame,slot.x,slot.y};}
            if(type==2){slot.active=false;return {TouchRoute::Tap,slot.x,slot.y};}
            return {TouchRoute::Pending,slot.x,slot.y};
        }
        if(type==0&&passive){
            for(auto& slot:pending_)if(!slot.active){slot={id,true,x,y};return {TouchRoute::Pending,x,y};}
            return {TouchRoute::Game,x,y};
        }
        return {route(type,id,hit)?TouchRoute::Ui:TouchRoute::Game,x,y};
    }
    bool route(unsigned type, std::int64_t id, bool hit) {
        if (type == 0) {
            for (auto& slot : ui_) if (slot.active && slot.id == id) slot.active = false;
            for (auto& slot : pending_) if (slot.active && slot.id == id) slot.active = false;
        }
        for (auto& slot : ui_) if (slot.active && slot.id == id) {
            if (type == 2) slot.active = false;
            return true;
        }
        if (type != 0 || !hit) return false;
        for (auto& slot : ui_) if (!slot.active) {
            slot = {id, true}; return true;
        }
        return false;
    }
};
} // namespace eagler::thprac
