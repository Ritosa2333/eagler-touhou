#pragma once
#include "../../cpp/game/Player.hpp"
#include <vector>
namespace player_frame_test {
using namespace th09;
struct Fixture:PlayerActions {
    EclWorldState world;Player player{world,*this};PlayerFrameContext context;DamageRules damage;AnmLoadedSprite sprite{};
    std::vector<std::array<u32,8>> events;std::array<TransferParameters,4096> transfers{};u32 transfer_count=0;bool boss=true;u32 frame_number=0;
    static u32 bits(float f){u32 n;std::memcpy(&n,&f,4);return n;}
    i32 index(const AnmVm& vm){if(&vm==&player.body)return 1000;for(u32 i=0;i<128;++i)if(&vm==&player.shots.shots[i].animation)return i32(i);for(u32 i=0;i<4;++i)if(&vm==&player.items.items[i].animation)return i32(i+2000);return -1;}
    void start_animation(AnmVm& vm,i32 script)override{events.push_back({0,u32(index(vm)),u32(script)});std::memset(&vm,0,sizeof(vm));vm.scriptIndex=i16(script);vm.visible=1;vm.type=1;vm.color1.d3dColor=-1;vm.loadedSprite=&sprite;}
    bool advance_animation(AnmVm& vm)override{const i32 i=index(vm);events.push_back({1,u32(i)});return i<128&&(frame_number+u32(i))%107==106;}
    void draw_animation(AnmVm& vm,bool fading)override{events.push_back({2,u32(index(vm)),u32(fading)});}
    void play_positioned_sound(i32 id,float x)override{events.push_back({3,u32(id),bits(x)});}
    void play_sound(i32 id,i32 pan)override{events.push_back({4,u32(id),u32(pan)});}
    void effect(i32 id,const Vec3& p,i32 slot)override{events.push_back({5,u32(id),bits(p.x),bits(p.y),bits(p.z),u32(slot),0xffffffff});}
    bool effect_active(i32)override{return false;}
    Vec3 effect_position(i32)override{return {};}
    bool rewards_blocked()const override{return damage.blocked;}
    bool opponent_has_boss()const override{return !boss;}
    i32 opposing_spirits()const override{return 0;}
    void attack(i32 type,i32 level)override{events.push_back({6,u32(type),u32(level)});}
    void add_score(i32 score)override{events.push_back({7,u32(score)});}
    void score_popup(const Vec3& p,i32 score,u32 color)override{events.push_back({8,bits(p.x),bits(p.y),bits(p.z),u32(score),color});}
    void character_meter(ComboState&,const Vec3&)override{events.push_back({9});}
    TransferParameters* create_transfer(i32 type,const Vec3& p,const Vec3& dest)override{return delayed_transfer(type,p,dest,0);}
    TransferParameters* delayed_transfer(i32 type,const Vec3& p,const Vec3& dest,float delay)override{events.push_back({10,u32(type),bits(p.x),bits(p.y),bits(p.z),bits(dest.x),bits(dest.y),bits(delay)});return transfer_count<transfers.size()?&transfers[transfer_count++]:nullptr;}
    void opponent_survival_display(i32 frames)override{events.push_back({11,u32(frames)});}
    void opponent_survival_expired()override{events.push_back({12});}
    void begin_charge()override{events.push_back({13});}
    void end_charge()override{events.push_back({14});}
    bool opponent_boss_available()override{events.push_back({15});return boss;}
    void charged_attack(i32 level,const std::string&)override{events.push_back({6,u32(level),u32(level)});}
    void protection_effect(i32 id,const Vec3& p)override{events.push_back({16,u32(id),bits(p.x),bits(p.y),bits(p.z)});}
    void opponent_wins(i32 side)override{events.push_back({17,u32(side)});}
    void slotted_effect(i32 id,const Vec3& p,i32 slot,u32 color)override{events.push_back({5,u32(id),bits(p.x),bits(p.y),bits(p.z),u32(slot),color});}
    void critical_health()override{events.push_back({18});}
    void damage_flash(i32,i32,u32)override{}
    void begin_focus(const Vec3& p,u32 side,u32 character)override{static constexpr i32 types[]={10,16,17,18,26,24,27,25,28,29,30,31,32,33,36,37};slotted_effect(7,p,side,0xffffffff);slotted_effect(types[character],p,side+2,0xffffffff);}
    void end_focus()override{events.push_back({19});}
    void shield_position(const Vec3&)override{}
    void remove_shield()override{}
    void charge_level(i32 level)override{events.push_back({20,u32(level)});}
    void body_interrupt(AnmVm& vm,i32 label)override{start_animation(vm,label);}
    void item_animation(AnmVm& vm,i32 script)override{start_animation(vm,script);}
    void draw_body(AnmVm& vm)override{draw_animation(vm,false);}
    void draw_item(AnmVm& vm)override{draw_animation(vm,false);}
    void critical_opponent_time(i32 frames)override{events.push_back({21,u32(frames)});}
};
}
