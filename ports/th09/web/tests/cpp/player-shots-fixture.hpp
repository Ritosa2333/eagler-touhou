#pragma once
#include "../../cpp/game/PlayerShots.hpp"
#include <vector>
namespace player_shots_test {
using namespace th09;
struct Fixture:PlayerShotActions {
    ShotResource resource;PlayerShots player{*this};AnmLoadedSprite sprite{};PlayfieldGeometry geometry;FrameTiming timing;
    Vec3 effect_pos{19.5f,280.25f,.125f};bool effect_enabled=false;i32 frame=0;
    std::vector<std::array<u32,8>> events;std::vector<AnmVm> drawn;
    Fixture(){sprite.widthPx=24;sprite.heightPx=32;}
    static u32 bits(float f){u32 n;std::memcpy(&n,&f,4);return n;}
    i32 index(const AnmVm& v){for(u32 i=0;i<128;++i)if(&player.shots[i].animation==&v)return i32(i);return -1;}
    void start_animation(AnmVm& v,i32 script)override{events.push_back({0,u32(index(v)),u32(script)});std::memset(&v,0,sizeof(v));v.scriptIndex=i16(script);v.visible=1;v.type=1;v.color1.d3dColor=-1;v.loadedSprite=&sprite;}
    bool advance_animation(AnmVm& v)override{const i32 i=index(v);events.push_back({1,u32(i)});return (frame+i)%113==112;}
    void draw_animation(AnmVm& v,bool fading)override{events.push_back({2,u32(index(v)),u32(fading)});drawn.push_back(v);}
    void play_positioned_sound(i32 id,float x)override{events.push_back({3,u32(id),bits(x)});}
    void play_sound(i32 id,i32 pan)override{events.push_back({4,u32(id),u32(pan)});}
    void effect(i32 id,const Vec3& pos,i32 slot)override{events.push_back({5,u32(id),bits(pos.x),bits(pos.y),bits(pos.z),u32(slot)});}
    bool effect_active(i32 slot)override{events.push_back({6,u32(slot)});return effect_enabled;}
    Vec3 effect_position(i32 slot)override{events.push_back({6,u32(slot)});return effect_pos;}
    void clear_events(){events.clear();drawn.clear();}
};
static_assert(offsetof(PlayerShot,position)==0x2a4&&offsetof(PlayerShot,time)==0x454&&offsetof(PlayerShot,hits)==0x470);
}
