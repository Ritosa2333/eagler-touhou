#pragma once
#include "player-frame-fixture.hpp"
#include "../../cpp/game/BombController.hpp"
namespace th11::test {
struct BombFixture:BombWorld {
    PlayerFrameFixture frame;AnmResource text;BombController controller;std::vector<u32> events;i32 count=0;u32 background_color=0;
    BombFixture(ShtResource& s,AnmResource& p,AnmResource& b,AnmManager& a):frame(s,p,b,a),controller(a,p,frame.player,*this,0,&text){}
    static u32 bits(float f){return PlayerFrameFixture::bits(f);}
    bool bomb_sound(i32 id,float x,bool positional)override{events.insert(events.end(),{1,u32(id),bits(x),u32(positional)});return true;}
    bool bomb_stop_sound(i32 id)override{events.insert(events.end(),{2,u32(id)});return true;}
    bool bomb_cancel(Vec3 p,float r,u32 flags,bool convert)override{events.insert(events.end(),{3,bits(p.x),bits(p.y),bits(p.z),bits(r),flags,u32(convert)});return true;}
    i32& bomb_count()override{return count;}
    bool bomb_background_color(u32 color)override{background_color=color;return true;}
    bool bomb_refund_power()override{events.insert(events.end(),{4,10,5});return true;}
    bool bomb_cancel_beam(Vec3 p,bool reward)override{events.insert(events.end(),{6,bits(p.x),bits(p.y),bits(p.z),u32(reward)});return true;}
};
}
