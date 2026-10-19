#pragma once
#include "enemy-fixture.hpp"
#include "../../cpp/game/EnemyAnimations.hpp"
namespace th11::test {
struct EnemyAnimationFixture {
    AnmResource resource;AnmManager manager;EnemyAnimations animations{manager};EnemyFixture enemy;
    bool load(const u8* bytes,u32 size){
        if(!resource.open(bytes,size))return false;animations.resources[0]={&resource,0};
        enemy.commands.animations=&animations;enemy.world.shared_random=&manager.script_rng;
        enemy.commands.animation_user=this;enemy.commands.animation_visibility=[](u32 id,bool visible,void* p){static_cast<EnemyAnimationFixture*>(p)->animations.visible(id,visible);};
        enemy.world.animation_user=this;enemy.world.animation_sprite=[](u32 id,void* p){return static_cast<EnemyAnimationFixture*>(p)->animations.sprite(id);};
        return true;
    }
};
}
