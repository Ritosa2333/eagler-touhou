#pragma once
#ifndef TH_MULTIPLAYER_FIXTURES
#error Native pool ownership fixture must not enter production
#endif
#include "../../th08_web/cpp/multiplayer/PoolsJournal.hpp"
#include "../../th08_web/cpp/game/GameplayScene.hpp"

namespace th08::multiplayer::fixture {
inline const u32* pools_journal_probe(GameplayScene& game,GameplaySession& session){
    static u32 result[10]{};std::fill(result,result+10,0);result[0]=1;
    const auto require=[&](bool condition,u32 step){if(!condition)result[2]=step;return condition;};
    if(!require(game.ready()&&!game.invalid()&&session.player_count>=2,1))return result;
    const Vec3 params{0.2f,24,4};
    auto* initial=game.effect_system.fixed_player(1,11,35,{180,100,0.1f},0xffffffff,&params);
    if(!require(initial&&initial->active&&initial->vertices&&game.effect_system.geometry.prepare(*initial)>0,2))return result;
    const auto* vertices=initial->vertices;
    PoolsJournal journal;
    journal.audit_bullets=true;
    if(!require(journal.Bind(game.bullets,game.items,game.effect_system,session.random),3))return result;
    const u32 before=journal.AuditHash();
    const auto mutate=[&](){
        BulletEmission shot;shot.sprite=0;shot.color=2;shot.position={190,100,0};shot.angle=.25f;
        shot.spread=.13f;shot.speed=1.5f;shot.ending_speed=2;shot.count=3;shot.layers=2;shot.pattern=0;
        game.bullets.emit(shot);
        BulletEmission beam;beam.sprite=0;beam.color=0;beam.position={170,80,0};beam.angle=.3f;
        beam.pattern=1;beam.speed=.5f;beam.laser={0,64,96,12,5,30,8,0,6};
        auto* laser=game.bullets.laser(beam);
        const auto address=reinterpret_cast<std::uintptr_t>(laser);
        const auto start=reinterpret_cast<std::uintptr_t>(game.projectile_pool.lasers);
        if(!laser||address<start||address>=start+sizeof(game.projectile_pool.lasers)||!laser->in_use)return false;
        auto* item=game.items.spawn({180,140,0},1,2);
        if(!item||!item->active||!game.items.spawn_for_player({180,138,0},2,0,1))return false;
        const Vec3 next_params{.5f,30,6};
        auto* reused=game.effect_system.fixed_player(1,11,35,{190,120,.1f},0xffeeeeff,&next_params);
        if(reused!=initial||reused->vertices!=vertices)return false;
        reused->height=8;reused->frequency=3;reused->segments=54;reused->geometry_dirty=1;
        if(game.effect_system.geometry.prepare(*reused)!=110)return false;
        auto* born=game.effect_system.spawn(35,{200,100,0},1,0xffffffff,&params);
        if(!born||!born->active||!born->vertices||game.effect_system.geometry.prepare(*born)<=0)return false;
        return !game.bullets.invalid()&&!game.items.invalid()&&!game.effect_system.invalid&&
               !game.effect_system.geometry.invalid;
    };
    if(!require(journal.BeginFrame(0)&&mutate()&&journal.EndFrame(),4))return result;
    const u32 after=journal.AuditHash();result[3]=u32(journal.BytesForFrame(0));
    if(!require(after!=before&&result[3]<sizeof(BulletManagerState)+sizeof(ItemPoolState)+sizeof(EffectPoolState),5))return result;
    if(!require(journal.UndoTo(0)&&journal.AuditHash()==before&&initial->vertices==vertices,6))return result;
    result[4]=1; // all pool bytes, owner assignments and RNG return exactly
    if(!require(journal.BeginFrame(0)&&mutate()&&journal.EndFrame()&&journal.AuditHash()==after,7))return result;
    result[5]=1; // native allocation / replacement reruns identically
    if(!require(journal.UndoTo(0)&&journal.AuditHash()==before,8))return result;
    result[6]=1; // newly-created effects reuse stable, restored vertex storage
    if(!require(journal.BeginFrame(1),9))return result;
    initial->active=0;
    game.effect_system.update();
    if(!require(!game.effect_system.invalid&&!initial->vertices&&journal.EndFrame(),10))return result;
    if(!require(journal.UndoTo(1)&&journal.AuditHash()==before&&initial->vertices==vertices,11))return result;
    result[7]=1; // native inactive cleanup does not free checkpointed arrays
    if(!require(journal.BeginFrame(2)&&journal.EndFrame(),12))return result;
    game.items.reset();
    if(!require(game.items.invalid(),13))return result;
    if(!require(journal.UndoTo(2)&&!game.items.invalid()&&journal.AuditHash()==before,14))return result;
    result[8]=1; // destructive owner reset is fenced while history is live
    journal.Clear();result[9]=1;result[1]=1;return result;
}
}
