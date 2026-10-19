#pragma once
#include "../../cpp/game/PlayerFrame.hpp"
#include <cstring>
namespace th11::test {
struct PlayerFrameFixture:PlayerFrameWorld {
    GameEconomy economy;PlayerFrame player;std::vector<u32> events;i32 deaths=0;
    PlayerFrameFixture(ShtResource& s,AnmResource& p,AnmResource& b,AnmManager& a):player(s,p,b,a,economy,*this,0,0){}
    static u32 bits(float f){u32 b;std::memcpy(&b,&f,4);return b;}
    bool event(std::initializer_list<u32> v){events.insert(events.end(),v);return true;}
    bool sound(i32 id,float x)override{return event({1,u32(id),bits(x)});}
    bool player_sound(i32 id)override{return event({2,u32(id)});}
    bool attract_items()override{return event({3});}
    bool cancel_bullets(const Vec3* p,float r,bool mode)override{return p?event({4,bits(p->x),bits(p->y),bits(p->z),bits(r),u32(mode)}):event({5,u32(mode)});}
    bool cancel_lasers(const Vec3* p,float r,bool a,bool b)override{return p?event({6,bits(p->x),bits(p->y),bits(p->z),bits(r),u32(a),u32(b)}):event({7,u32(a),u32(b)});}
    bool start_bomb()override{return event({8});}
    bool spawn_item(i32 type,Vec3 p,u32 color,float a,float speed)override{return event({9,u32(type),bits(p.x),bits(p.y),bits(p.z),color,bits(a),bits(speed)});}
    bool display_lives(i32 lives,i32 fragments)override{return event({10,u32(lives),u32(fragments)});}
    bool record_death()override{return event({11});}
    bool enemy_death()override{++deaths;return true;}
    bool game_over(bool replay)override{return event({12,u32(replay)});}
};
}
