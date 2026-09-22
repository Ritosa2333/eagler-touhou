#pragma once
#include "Enemy.hpp"
#include "GameEconomy.hpp"
#include "AnmRegistry.hpp"
namespace th10 {
struct EnemySceneEnvironment {
    const i32* difficulty;
    GameEconomy* game;
    AnmRegistry* registry;
    u32* spell_flags;
    u32* spell_bonus_animation;
    virtual void screen_effect(i32 first,i32 second,i32 third)=0;
    virtual void start_dialogue(i32 id)=0;
    virtual void cancel_projectiles()=0;
    virtual void clear_enemies()=0;
    // [id] is the difficulty-adjusted record index used for captures/attempts;
    // [name_id] is the raw ECL spell id, which is the key used by spells.etl.
    virtual void start_spell(i32 id,i32 name_id,const char* name,i32 parameter)=0;
    virtual void end_spell()=0;
    virtual void delete_lasers()=0;
};
}
