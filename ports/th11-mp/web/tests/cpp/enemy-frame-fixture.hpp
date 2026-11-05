#pragma once
#include "enemy-animation-fixture.hpp"
#include "../../cpp/game/EnemyFrame.hpp"
namespace th11::test {
struct EnemyFrameFixture:EnemyAnimationFixture {
    struct World:EnemyFrameWorld {
        struct Event{i32 kind,value;float x,y;};
        std::vector<Event> events;
        std::vector<Vec3> cancel_rewards;
        i32 damage=0,collision=0,tick_result=0;
        bool shot_damage(EnemyState& e,i32& out)override{out=damage;events.push_back({0,damage,e.current.position.x,e.current.position.y});return true;}
        bool player_collision(EnemyState& e,float radius,i32& out)override{out=collision;events.push_back({1,collision,radius,0});return true;}
        bool sound(i32 id,float x)override{events.push_back({2,id,x,0});return true;}
        bool add_score(i32 value)override{events.push_back({3,value,0,0});return true;}
        bool score_popup(Vec3 p,i32 value)override{events.push_back({9,value,p.x,p.y});return true;}
        bool drop_items(EnemyState& e)override{events.push_back({4,e.drops.primary,0,0});for(auto& n:e.drops.counts)n=0;return true;}
        bool cancel_reward(Vec3 p)override{cancel_rewards.push_back(p);return true;}
        bool graze(EnemyState&)override{events.push_back({5,0,0,0});return true;}
        bool tick_callback(EnemyState&,i32& out)override{out=tick_result;events.push_back({6,out,0,0});return true;}
        bool damage_callback(EnemyState&,i32& out)override{out=damage;events.push_back({7,out,0,0});return true;}
        bool collision_callback(EnemyState&)override{events.push_back({8,0,0,0});return true;}
        void clear_shot_targets(Enemy*)override{}
    } frame_world;
    void setting(u32 key,i32 value){auto& w=frame_world;switch(key){
        case 0:w.damage=value;break;case 1:w.collision=value;break;case 2:w.player_state=value;break;
        case 3:w.special_active=value;break;case 4:w.spell_id=value;break;case 5:w.player_flags=u32(value);break;
        case 6:w.spell_flags=u32(value);break;case 7:w.special_ending=value;break;case 8:w.tick_result=value;break;
        case 9:w.spell_elapsed=value;break;case 10:w.bomb_animation_flags=u32(value);break;
    }}
};
}
