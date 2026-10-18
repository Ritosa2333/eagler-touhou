#pragma once
#include "EnemyScript.hpp"
#include "EnemyAnimations.hpp"
namespace th11 {
struct EnemyFrameWorld {
    bool practice_time_lock=false;
    i32 player_state=1,special_active=0,spell_id=0;
    u32 player_flags=0,spell_flags=0,bomb_animation_flags=0;
    bool special_ending=false,target_locked=false;
    Enemy* target=nullptr;
    i32 countdown_seconds=0,countdown_hundredths=0,spell_elapsed=0,spell_bonus=0,shared_spell_state=0;
    virtual ~EnemyFrameWorld()=default;
#ifdef TH11_MULTIPLAYER
    virtual bool multiplayer_world()const{return false;}
    virtual void multiplayer_enemy_target(Vec3){}
    virtual void multiplayer_shot_target(EnemyState&){}
    virtual i32 multiplayer_boss_damage(EnemyState&,i32 damage){return damage;}
#endif
    virtual bool shot_damage(EnemyState&,i32&){return false;}
    virtual bool player_collision(EnemyState&,float,i32&){return false;}
    virtual bool tick_callback(EnemyState&,i32&){return false;}
    virtual bool damage_callback(EnemyState&,i32&){return false;}
    virtual bool collision_callback(EnemyState&){return false;}
    virtual bool graze(EnemyState&){return false;}
    virtual bool sound(i32,float){return false;}
    virtual bool add_score(i32){return false;}
    virtual bool score_popup(Vec3,i32){return false;}
    virtual bool drop_items(EnemyState&){return false;}
    virtual bool cancel_reward(Vec3){return false;}
    virtual bool cancel_shot(Vec3,float){return false;}
    virtual bool deform(EnemyState&){return false;}
    virtual bool release_deformation(EnemyState&){return false;}
    virtual void clear_shot_targets(Enemy*)=0;
    virtual bool project_spawn(Vec3,Vec3&){return false;}
    virtual i32& shared_spell_counter(){return shared_spell_state;}
    Vec3 camera_position{};
};
// Original 0x411750 ordering: movement, scripts, damage/interrupts, collision,
// animation/target/flash, deformation, timers. -2 is an unavailable dependency.
i32 enemy_frame(EnemyScriptServices&,EnemyFrameWorld&);
i32 enemy_death(EnemyState&,EnemyAnimations&,EnemyFrameWorld&);
}
