#include "BulletSystem.hpp"
#include "Presentation.hpp"
#include "ValidationScene.hpp"
namespace th08 {
BulletSystem::BulletSystem(BulletManagerState& s,EclGlobals& g,Rng& rng,PlayerSimulation& p,ItemSystem& i,EffectSystem& e,AnmRenderer& r,BulletSystemAudio& a)
 :state(s),globals(g),player(p),inventory(i),effect_system(e),renderer(r),audio(a),creation(s,rng),updater(s,creation,rng),lasers(s,rng),drawing(s,*this){
    creation.actions=this;updater.actions=this;updater.live_cancel_item=&p.status().cancel_item;lasers.actions=this;globals.bullet_actions=this;globals.laser_actions=this;
}
void BulletSystem::synchronize(){creation.timing=updater.timing=lasers.timing=player.timing;updater.player=lasers.player=globals.player;updater.paused=globals.paused;}
void BulletSystem::publish_collision(){const auto& p=player.status();globals.player_state=p.life.state;globals.player_state_timer=p.life.timer;globals.game_flags=p.context.game_flags;globals.paused=p.context.pause!=0;if(globals.gui)std::memcpy(&globals.gui->flags,&p.context.hud_flags,4);failed|=player.invalid();}
bool BulletSystem::initialize(AnmLoaded& file,Rng& rng){state.animation=&file;ready=state.templates.load(file,rng,player.timing);failed=!ready;if(ready)drawing.snapshot();return ready;}
void BulletSystem::emit(BulletEmission& parameters){
    if(!ready){failed=true;return;}synchronize();creation.emit(parameters,bullet_aim(parameters.position,globals.player),&player.status().context.replay_flags);failed|=creation.failure!=BulletCreation::Failure::None;
}
bool BulletSystem::validation_deathbomb_eligible(bool scene_enabled,bool write_lock_valid,bool quiet_scene,bool custom_practice)const noexcept{
    const auto& p=player.status();const auto& type=state.templates.types[0];
    bool no_lasers=true;for(const auto& laser:state.lasers)if(laser.in_use){no_lasers=false;break;}
    const bool normal_game=validation_scene_request_eligible(true,true,globals.game_flags,custom_practice)&&
        !globals.paused&&!p.context.pause&&!p.context.game_over;
    const bool quiet=quiet_scene&&state.live_bullet_count()==0&&no_lasers;
    const bool template_ready=type.animation[0].loadedSprite!=nullptr;
    return validation_deathbomb_stimulus_eligible(scene_enabled,write_lock_valid,ready&&!failed,
        normal_game,quiet,template_ready,p.life.state,deathbomb_stimulus.request_sequence!=0);
}
bool BulletSystem::request_validation_deathbomb_hit(bool scene_enabled,bool write_lock_valid,bool quiet_scene,bool custom_practice){
    if(!validation_deathbomb_eligible(scene_enabled,write_lock_valid,quiet_scene,custom_practice)){
        deathbomb_stimulus.refuse();return false;
    }
    const auto& position=player.status().motion.movement.position;
    const auto spec=validation_deathbomb_emission_spec(position.x,position.y,position.z);
    BulletEmission emission{};emission.sprite=i16(spec.sprite);emission.color=i16(spec.color);
    emission.position={spec.x,spec.y,spec.z};emission.angle=spec.angle;emission.spread=spec.spread;
    emission.speed=spec.speed;emission.ending_speed=spec.ending_speed;emission.count=i16(spec.count);
    emission.layers=i16(spec.layers);emission.pattern=i16(spec.pattern);emission.sound=-1;
    emission.transform_sound=-1;emission.extra_index=0;
    BulletState* const candidate=state.next_slot;
    emit(emission);
    if(!candidate||!candidate->state||candidate->sprites.animation[0].loadedSprite!=state.templates.types[0].animation[0].loadedSprite){
        deathbomb_stimulus.refuse();return false;
    }
    if(!deathbomb_stimulus.accept())return false;
    deathbomb_projectile=candidate;
    return true;
}
LaserState* BulletSystem::laser(BulletEmission& parameters){if(!ready){failed=true;return nullptr;}synchronize();auto* result=lasers.create(parameters);failed|=lasers.invalid;return result;}
void BulletSystem::clear(i32 mode){PlayerCollision::BarrierBatch barriers(&player.collision());failed|=!cancel_projectiles(state,mode,player.status().cancel_item,this);}
i32 BulletSystem::collision(i32 kind,BulletState& bullet){
    // Barrier tests only modify the cancellation region/item. Republishing a
    // cached player state here erased invincibility granted during boss phases.
    if(kind==0)return player.collision().barrier({bullet.position.x,bullet.position.y});
    if(globals.gui)std::memcpy(&player.status().context.hud_flags,&globals.gui->flags,4);
    const bool tracked=kind==2&&deathbomb_projectile==&bullet&&deathbomb_stimulus.projectile_status==DeathbombStimulusReceiptV1::Emitted;
    const i32 before=tracked?player.status().life.state:0;
    auto& collision=player.collision();const i32 result=kind==1?collision.graze(bullet.position,bullet.sprites.hitbox):collision.bullet(bullet.position,bullet.sprites.hitbox);
    if(tracked){const auto& state=player.status();const auto& input=player.consumed_input();
        if(input.valid&&result==1)validation_hit={input.sequence,u32(before),u32(state.life.state),u32(result),validation_hit[4]+1};
        deathbomb_stimulus.observe_collision(before,state.life.state,state.life.predead_count,result,input.sequence,input.valid);}
    publish_collision();return result;
}
void BulletSystem::collision(const Vec2& center,const Vec2& size,const Vec3& origin,float angle,bool graze){if(globals.gui)std::memcpy(&player.status().context.hud_flags,&globals.gui->flags,4);player.collision().laser(center,size,origin,angle,graze);publish_collision();}
bool BulletSystem::update(){
    if(failed||!ready)return false;
    // Enemy ECL (priority 11) can move/rotate/recolor lasers before this
    // priority-14 job. The scene supplies the pre-ECL endpoint; standalone
    // BulletSystem callers retain their original self-contained sampling.
    if(!presentation_prepared)drawing.snapshot();presentation_prepared=false;
    if(globals.game_flags&1024)return true;
    if(!inventory.update())return false;synchronize();
    PlayerCollision::BarrierBatch barriers(&player.collision());
    if(!updater.update_bullets()||!lasers.update())return false;
    if(deathbomb_projectile&&!deathbomb_projectile->state){deathbomb_stimulus.retire_without_hit();deathbomb_projectile=nullptr;}
    if(state.cancel_frames)state.cancel_frames=wrapping_sub(state.cancel_frames,1);state.timer.tick(player.timing);state.unknown_counter=wrapping_add(state.unknown_counter,1);return !failed;
}
bool BulletSystem::draw(const Vec2& origin){
    if(failed||!ready)return false;const bool failed_before=failed;const Vec2 arcade_before=arcade;arcade=origin;const bool result=drawing.draw(globals.game_flags,origin);
    if(presentation::render_only){failed=failed_before;arcade=arcade_before;return true;}return result&&!failed;
}
}
