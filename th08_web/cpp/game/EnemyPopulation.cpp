#include "EnemyPopulation.hpp"
#include "EnemyRetirement.hpp"
#include "EclSpawn.hpp"
namespace th08 {
void EnemyPopulation::reset(i32 time_items)noexcept{for(auto& enemy:enemies)enemy.reset();spawn_failed=false;replay_flags=0;initial_time_items=time_items;practice_familiar=0;}
EclVm* EnemyPopulation::spawn(const TimelineSpawn& request){return spawn_impl(request,nullptr);}
EclVm* EnemyPopulation::spawn_impl(const TimelineSpawn& request,const EclContext::Locals* inherited){
    replay_flags|=0x1000;u32 index=0;
    while(index<480&&enemies[index]&&(enemies[index]->flags&1))++index;
    if(!enemies[index])enemies[index]=std::make_unique<EclVm>();
    auto& enemy=*enemies[index];bool failed=index==480;
    if(!failed){
        // Defaults of fields already recovered from Initialize (0x429e00).
        if(!executor.start(enemy,program,i16(request.subroutine))){spawn_failed=true;enemy.flags=0;return &enemy;}
        for(auto& point:enemy.trail.points)point.position.x=-999;
        enemy.pool_index=i32(index);enemy.flags=0x4d|(inherited?0:((request.mirror&1)<<18));
        enemy.position=request.position;enemy.life=request.life<0?1:request.life;
        enemy.hitbox={24,24,24};enemy.score_reward=100;enemy.player_protect_squared=1024;enemy.damage_protection={0,0,0};enemy.time_items=initial_time_items;
        for(auto& threshold:enemy.life_thresholds)threshold=-1;
        enemy.timeout=-1;enemy.death_subroutine=-1;
        enemy.animation[1].scriptIndex=enemy.animation[2].scriptIndex=-1;
        enemy.poses[1]=enemy.poses[2]=enemy.poses[5]=0;
        enemy.emitter.sound=7;enemy.emitter.transform_sound=25;enemy.rank_speed_low=-.15f;enemy.rank_speed_high=.15f;enemy.emission_time.set(0);
        // Original Spawn (0x42a4e0) runs the subroutine before assigning
        // requested drops, score and the two life snapshots.
        if(inherited)enemy.main_context.locals=*inherited;
        if(!executor.step(enemy)){enemy.flags&=~1u;failed=true;}
        else{enemy.animation_color=enemy.animation[0].color1.d3dColor;enemy.item_reward=i8(request.item);if(inherited&&request.life>=0)enemy.life=request.life;if(request.score>=0)enemy.score_reward=request.score;enemy.initial_life=enemy.remaining_life=enemy.life;}
    }
    if(request.multiple_items){enemy.power_items=request.power_items;enemy.point_items=request.point_items;}
    if(!failed&&(enemy.flags&2)&&practice_familiar){enemy.practice_familiar=practice_familiar;practice_familiar=0;}
    spawn_failed=failed;return &enemy;
}
u32 EnemyPopulation::active_count()const noexcept{u32 count=0;for(u32 i=0;i<480;++i)if(enemies[i]&&(enemies[i]->flags&1))++count;return count;}
void EnemyPopulation::update_scripts(const FrameTiming& timing){
    for(u32 i=0;i<480;++i)if(auto* enemy=enemies[i].get();enemy&&(enemy->flags&1)){
        if(enemy->flags&0x100){auto& globals=executor.game_state();if(!globals.enemy_actions){enemy->invalid=true;enemy->failure=EclVm::Failure::MissingEnemyActions;continue;}update_enemy_form(*enemy,globals.youkai,*globals.enemy_actions);}
        u32 transitions=0;
        for(;;){
            if(!executor.step(*enemy)){enemy->flags&=~1u;retire_enemy(*enemy,executor.game_state(),replay_flags);break;}
            enemy->integrate_position(timing);
            const bool changed=check_life(*enemy)||(enemy->timeout>=0&&check_timeout(*enemy));
            if(enemy->invalid){enemy->flags&=~1u;break;}
            if(!changed)break;
            if(++transitions>=10000){enemy->invalid=true;enemy->flags&=~1u;break;}
        }
        if((enemy->flags&1)&&!enemy->invalid){
            if(!executor.update_animations(*enemy)){enemy->flags&=~1u;continue;}
            enemy->update_attached_effects();if(!executor.game_state().paused)enemy->lifetime.tick(timing);
            if(enemy->damage_protection.current>0)enemy->damage_protection.decrement(1,timing);
        }
    }
}
}
