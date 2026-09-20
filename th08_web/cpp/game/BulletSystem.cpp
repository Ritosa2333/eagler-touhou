#include "BulletSystem.hpp"
#include "Presentation.hpp"
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
LaserState* BulletSystem::laser(BulletEmission& parameters){if(!ready){failed=true;return nullptr;}synchronize();auto* result=lasers.create(parameters);failed|=lasers.invalid;return result;}
void BulletSystem::clear(i32 mode){failed|=!cancel_projectiles(state,mode,player.status().cancel_item,this);}
i32 BulletSystem::collision(i32 kind,BulletState& bullet){
    // Barrier tests only modify the cancellation region/item. Republishing a
    // cached player state here erased invincibility granted during boss phases.
    if(kind==0)return player.collision().barrier({bullet.position.x,bullet.position.y});
    if(globals.gui)std::memcpy(&player.status().context.hud_flags,&globals.gui->flags,4);
    auto& collision=player.collision();const i32 result=kind==1?collision.graze(bullet.position,bullet.sprites.hitbox):collision.bullet(bullet.position,bullet.sprites.hitbox);
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
    if(!inventory.update())return false;synchronize();if(!updater.update_bullets()||!lasers.update())return false;
    if(state.cancel_frames)state.cancel_frames=wrapping_sub(state.cancel_frames,1);state.timer.tick(player.timing);state.unknown_counter=wrapping_add(state.unknown_counter,1);return !failed;
}
bool BulletSystem::draw(const Vec2& origin){
    if(failed||!ready)return false;const bool failed_before=failed;const Vec2 arcade_before=arcade;arcade=origin;const bool result=drawing.draw(globals.game_flags,origin);
    if(presentation::render_only){failed=failed_before;arcade=arcade_before;return true;}return result&&!failed;
}
}
