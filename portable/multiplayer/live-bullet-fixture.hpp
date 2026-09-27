#pragma once
#ifndef TH_MULTIPLAYER_FIXTURES
#error Native Bullet byte oracle must not enter production
#endif
#include "../../th08_web/cpp/platform/BrowserRuntime.hpp"
#include "../../th08_web/cpp/multiplayer/PoolsJournal.hpp"
namespace th08::multiplayer::fixture {
inline const u32* live_bullet_probe(BrowserRuntime& runtime){
    static u32 out[10]{};std::fill(out,out+10,0);out[0]=1;
    auto& game=runtime.app.game;auto& session=runtime.app.session;
    if(!game.ready()||session.netplay.Configured())return out;
    BulletEmission shot;shot.sprite=0;shot.color=2;shot.position={190,100,.1f};
    shot.speed=.1f;shot.ending_speed=.1f;shot.count=1;shot.layers=1;
    const u32 first=u32(game.projectile_pool.next_slot-game.projectile_pool.bullets);
    for(u32 flags:{0u,2u,4u,8u}){shot.flags=flags;game.bullets.emit(shot);}
    if(game.bullets.invalid())return out;
    auto& transforming=game.projectile_pool.bullets[first];
    transforming.flags|=0x4000;transforming.extra_flags=0;transforming.current_extra=0;
    transforming.extras[0]={0,0,1,0,0x4000,0};
    PoolsJournal journal;journal.audit_bullets=true;
    if(!journal.Bind(game.bullets,game.items,game.effect_system,session.random))return out;
    const auto expected=journal.AuditHash();
    for(u32 mode=0;mode<5;++mode){
        out[2]=mode+1;
        if(!journal.BeginFrame(mode))return out;
        switch(mode){
        case 0:game.bullets.clear(1);shot.flags=4;game.bullets.emit(shot);break;
        case 1:if(!game.bullets.update())return out;break;
        case 2:game.globals.scene_actions->clear_projectiles_near({190,100,.1f},1000);break;
        case 3:if(!game.globals.scene_actions->clear_projectiles(1))return out;break;
        case 4:game.bullets.clear(1);break;
        }
        if(game.bullets.invalid()||!journal.EndFrame())return out;
        // AuditHash includes ALL pool bytes; UndoTo additionally memcmps all
        // 1537 Bullet slots against a dense copy made before native writers.
        if(journal.AuditHash()==expected||!journal.UndoTo(mode)||journal.AuditHash()!=expected)return out;
        out[4+mode]=1;
    }
    out[3]=journal.bullet_audit_restores;out[2]=0;out[1]=out[3]==5;return out;
}
}
