#include "BulletSystem.hpp"
#include "Presentation.hpp"
#ifdef TH_MULTIPLAYER_FIXTURES
#include <emscripten.h>
#include <array>
#endif
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "../multiplayer/JournalTouch.hpp"
#endif
namespace th08 {
#ifdef TH_MULTIPLAYER_FIXTURES
namespace {std::array<double,8> bullet_profile{};}
const double* fixture_bullet_profile() noexcept {return bullet_profile.data();}
#endif
BulletSystem::BulletSystem(BulletManagerState& s,EclGlobals& g,Rng& rng,PlayerSimulation& p,ItemSystem& i,EffectSystem& e,AnmRenderer& r,BulletSystemAudio& a)
 :state(s),globals(g),player(p),inventory(i),effect_system(e),renderer(r),audio(a),creation(s,rng),updater(s,creation,rng),lasers(s,rng),drawing(s,*this){
    creation.actions=this;updater.actions=this;
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    updater.live_cancel_item=&p.status().cancel_item;
#endif
    lasers.actions=this;globals.bullet_actions=this;globals.laser_actions=this;
}
void BulletSystem::synchronize(){creation.timing=updater.timing=lasers.timing=player.timing;updater.player=lasers.player=globals.player;updater.paused=globals.paused;}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
void BulletSystem::publish_collision(PlayerSimulation& recipient){
    auto& p=recipient.status();globals.game_flags=p.context.game_flags;globals.paused=p.context.pause!=0;
    if(&recipient==&player&&globals.gui)std::memcpy(&globals.gui->flags,&p.context.hud_flags,4);
    failed|=recipient.invalid();
}
#endif
void BulletSystem::publish_collision(){const auto& p=player.status();globals.player_state=p.life.state;globals.player_state_timer=p.life.timer;globals.game_flags=p.context.game_flags;globals.paused=p.context.pause!=0;if(globals.gui)std::memcpy(&globals.gui->flags,&p.context.hud_flags,4);failed|=player.invalid();}
bool BulletSystem::initialize(AnmLoaded& file,Rng& rng){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(creation.rollback_journal&&(creation.rollback_journal->IsFrameOpen()||creation.rollback_journal->FrameCount())){failed=true;return false;}
#endif
    state.animation=&file;ready=state.templates.load(file,rng,player.timing);failed=!ready;if(ready)drawing.snapshot();return ready;
}
void BulletSystem::emit(BulletEmission& parameters){
    if(!ready){failed=true;return;}synchronize();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    creation.emit(parameters,bullet_aim(parameters.position,target(parameters.position)),&player.status().context.replay_flags);
#else
    creation.emit(parameters,bullet_aim(parameters.position,globals.player),&player.status().context.replay_flags);
#endif
    failed|=creation.failure!=BulletCreation::Failure::None;
}
LaserState* BulletSystem::laser(BulletEmission& parameters){if(!ready){failed=true;return nullptr;}synchronize();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    lasers.player=target(parameters.position);
#endif
    auto* result=lasers.create(parameters);failed|=lasers.invalid;return result;}
void BulletSystem::clear(i32 mode){PlayerCollision::BarrierBatch barriers(&player.collision());failed|=!cancel_projectiles(state,mode,player.status().cancel_item,this);}
i32 BulletSystem::collision(i32 kind,BulletState& bullet){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(!roster)return 0;
    u32 order[3];const u32 count=roster->ordered(bullet.position,order);
    i32 grazed=0;
    for(u32 i=0;i<count;++i){
        const u32 seat=order[i];
        if(kind==1&&(bullet.grazed&(1u<<seat)))continue;
        auto& recipient=*roster->seats[seat].player;
        const i32 result=kind==0?recipient.collision().barrier({bullet.position.x,bullet.position.y}):
            kind==1?recipient.collision().graze(bullet.position,bullet.sprites.hitbox):
                    recipient.collision().bullet(bullet.position,bullet.sprites.hitbox);
        if(result){
            if(result==2){
                // The cancellation reward belongs to the barrier that actually
                // hit this bullet. In multiplayer the old live pointer always
                // referenced P1, so P2's death-clear (-1) could incorrectly
                // drop P1's current cancel item (usually type 6).
                bullet.padding_dbf=u8(recipient.status().cancel_item+1);
            }
            publish_collision(recipient);
            if(kind==1&&result==1){bullet.grazed|=u8(1u<<seat);grazed=1;continue;}
            return result;
        }
    }
    return grazed;
#else
    // Barrier tests only modify the cancellation region/item. Republishing a
    // cached player state here erased invincibility granted during boss phases.
    if(kind==0)return player.collision().barrier({bullet.position.x,bullet.position.y});
    if(globals.gui)std::memcpy(&player.status().context.hud_flags,&globals.gui->flags,4);
    auto& collision=player.collision();const i32 result=kind==1?collision.graze(bullet.position,bullet.sprites.hitbox):collision.bullet(bullet.position,bullet.sprites.hitbox);
    publish_collision();return result;
#endif
}
void BulletSystem::collision(const Vec2& center,const Vec2& size,const Vec3& origin,float angle,bool graze){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(!roster)return;u32 order[3];const u32 count=roster->ordered(origin,order);
    for(u32 i=0;i<count;++i){auto& recipient=*roster->seats[order[i]].player;
        recipient.collision().laser(center,size,origin,angle,graze);publish_collision(recipient);
    }
#else
    if(globals.gui)std::memcpy(&player.status().context.hud_flags,&globals.gui->flags,4);player.collision().laser(center,size,origin,angle,graze);publish_collision();
#endif
}
bool BulletSystem::update(){
    if(failed||!ready)return false;
    // Enemy ECL (priority 11) can move/rotate/recolor lasers before this
    // priority-14 job. The scene supplies the pre-ECL endpoint; standalone
    // BulletSystem callers retain their original self-contained sampling.
    if(!presentation_prepared){
#ifdef TH_MULTIPLAYER_FIXTURES
        const double start=emscripten_get_now();
#endif
        drawing.snapshot();
#ifdef TH_MULTIPLAYER_FIXTURES
        bullet_profile[0]+=emscripten_get_now()-start;
#endif
    }
    presentation_prepared=false;
    if(globals.game_flags&1024)return true;
#ifdef TH_MULTIPLAYER_FIXTURES
    double start=emscripten_get_now();
#endif
    if(!inventory.update())return false;
#ifdef TH_MULTIPLAYER_FIXTURES
    bullet_profile[1]+=emscripten_get_now()-start;start=emscripten_get_now();
#endif
    synchronize();
#ifdef TH_MULTIPLAYER_FIXTURES
    bullet_profile[2]+=emscripten_get_now()-start;start=emscripten_get_now();
#endif
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // Player/Bomb, Enemy and Item jobs have finished creating regions. Bullet
    // hit/graze creates effects/items but does not create cancellation regions.
    // Rebuild on every forward/resim tick; scopes end before any later job.
    PlayerCollision::BarrierBatch host_barriers(&player.collision());
    PlayerCollision::BarrierBatch guest1_barriers(roster&&roster->count>1&&roster->seats[1].player?&roster->seats[1].player->collision():nullptr);
    PlayerCollision::BarrierBatch guest2_barriers(roster&&roster->count>2&&roster->seats[2].player?&roster->seats[2].player->collision():nullptr);
#else
    PlayerCollision::BarrierBatch host_barriers(&player.collision());
#endif
    if(!updater.update_bullets())return false;
#ifdef TH_MULTIPLAYER_FIXTURES
    bullet_profile[3]+=emscripten_get_now()-start;start=emscripten_get_now();
#endif
    if(!lasers.update())return false;
#ifdef TH_MULTIPLAYER_FIXTURES
    bullet_profile[4]+=emscripten_get_now()-start;
#endif
    if(state.cancel_frames)state.cancel_frames=wrapping_sub(state.cancel_frames,1);state.timer.tick(player.timing);state.unknown_counter=wrapping_add(state.unknown_counter,1);return !failed;
}
void BulletSystem::snapshot_presentation(){
    if(!ready)return;
#ifdef TH_MULTIPLAYER_FIXTURES
    const double start=emscripten_get_now();
#endif
    drawing.snapshot();presentation_prepared=true;
#ifdef TH_MULTIPLAYER_FIXTURES
    bullet_profile[0]+=emscripten_get_now()-start;
#endif
}
bool BulletSystem::draw(const Vec2& origin){
    if(failed||!ready)return false;const bool failed_before=failed;const Vec2 arcade_before=arcade;arcade=origin;const bool result=drawing.draw(globals.game_flags,origin);
    if(presentation::render_only){failed=failed_before;arcade=arcade_before;return true;}return result&&!failed;
}
}
