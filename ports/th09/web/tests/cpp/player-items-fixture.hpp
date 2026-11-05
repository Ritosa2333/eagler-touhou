#pragma once
#include "../../cpp/game/PlayerItems.hpp"
#include <vector>
namespace player_items_test {
using namespace th09;
struct Transfer {float delay=0;TransferParameters parameters;};
struct Fixture:PlayerItemActions {
    Rng random;PlayerItems manager{random,*this};ItemContext context;std::vector<std::array<u32,10>> events;std::vector<Transfer> transfers;
    static u32 bits(float f){u32 n;std::memcpy(&n,&f,4);return n;}
    u32 index(const AnmVm& v){for(u32 i=0;i<4;++i)if(&manager.items[i].animation==&v)return i;return ~0u;}
    void start_animation(AnmVm& v,i32 script)override{events.push_back({0,index(v),u32(script)});std::memset(&v,0,sizeof(v));v.visible=1;v.scriptIndex=i16(script);}
    void draw_animation(AnmVm& v)override{events.push_back({1,index(v)});}
    void charge(float f)override{events.push_back({2,bits(f)});}
    TransferParameters* create_transfer(i32 effect,const Vec3& p,const Vec3& target,float delay)override{events.push_back({3,u32(effect),bits(p.x),bits(p.y),bits(p.z),bits(target.x),bits(target.y),bits(target.z)});transfers.push_back({delay,{}});return &transfers.back().parameters;}
    void combo(const Vec3& p,i32 n,i32 s,i32 character,i32 score)override{events.push_back({4,bits(p.x),bits(p.y),bits(p.z),u32(n),u32(s),u32(character),u32(score)});}
    void play_sound(i32 id,i32 pan)override{events.push_back({5,u32(id),u32(pan)});}
    void clear(){events.clear();transfers.clear();}
};
static_assert(sizeof(PlayerItem)==0x2c4);
}
