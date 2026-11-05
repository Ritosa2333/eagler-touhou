#pragma once
#include "ecl-fixture.hpp"
#include "../../cpp/game/EnemyManager.hpp"
#include "../../cpp/game/EclGameOperations.hpp"
namespace enemy_manager_test {
using namespace th09;
struct Fixture:ecl_test::VmFixture::Emissions,EclSceneActions {
    EclWorldState world;EclPlayfieldState field,opponent;EnemyManager manager{world,field,opponent};
    EclGameOperations operations{manager,*this};std::vector<std::array<u32,12>> scene_events;i32 scene_value=0,rank_time=0;
    Fixture(){manager.bindings={nullptr,&operations,this,this,this};}
    static u32 bits(float f){u32 b;std::memcpy(&b,&f,4);return b;}
    void play_positioned_sound(i32 id,float x)override{scene_events.push_back({0,u32(id),bits(x)});}
    void effect(const EclEffectRequest& r)override{scene_events.push_back({1,u32(r.type),u32(r.count),r.color,u32(r.explicit_velocity),bits(r.position.x),bits(r.position.y),bits(r.position.z),bits(r.velocity.x),bits(r.velocity.y),bits(r.velocity.z)});}
    void boss_indicator(i32 index,i16 state)override{scene_events.push_back({2,u32(index),u32(i32(state))});}
    void boss_indicator_position(i32 index,const Vec3& p)override{scene_events.push_back({3,u32(index),bits(p.x),bits(p.y),bits(p.z)});}
    void release_attached_effects(EclVm& vm)override{scene_events.push_back({4,u32(vm.values.boss_id),u32(vm.status.attached_effect_count)});}
    void score_popup(const Vec3& p,i32 amount,u32 color)override{scene_events.push_back({5,u32(amount),color,bits(p.x),bits(p.y),bits(p.z)});}
    void scene_setting(i32 v)override{scene_value=v;}
    void add_script_extra_time(i32 frames)override{rank_time=wrapping_add(rank_time,frames);}
    void end_attack(u32 kind)override{scene_events.push_back({8,kind});}
    EclVm& actor(u32 index){return index==999?manager.prototype:manager.enemies[index%(EnemyManager::capacity+1)];}
    void* value(u32 actor_index,u32 field_index){
        const auto& f=ecl_test::fields[field_index];auto& e=actor(actor_index);
        if(f.base==0)return reinterpret_cast<u8*>(&e.values)+f.offset;
        if(f.base==1)return &e.primary.locals;
        if(f.base==5)return reinterpret_cast<u8*>(&world)+f.offset -offsetof(ecl_test::Fixture,world);
        if(f.base==4)return reinterpret_cast<u8*>(&opponent)+f.offset -offsetof(ecl_test::Fixture,opponent);
        return reinterpret_cast<u8*>(&field)+f.offset -offsetof(ecl_test::Fixture,field);
    }
};
}
