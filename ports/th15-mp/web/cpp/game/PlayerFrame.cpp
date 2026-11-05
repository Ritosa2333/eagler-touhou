#include "PlayerFrame.hpp"
namespace th15 {
PlayerFrame::PlayerFrame(PlayerMotion& p,PlayerAnmHost& v,AnmManager& a,PlayerLife& l,DamageSources& d,PlayerShots& s,PlayerShotContext& c,PlayerFrameHost& w):motion(p),visuals(v),animations(a),life(l),damage(d),shots(s),shot_context(c),world(w){shooting.fire=[this](i32 frame,i32 continuous){shot_context.shoot_frame=frame;return shots.fire(frame,continuous);};}
bool PlayerFrame::update(const PlayerFrameContext& frame,float& rate){
    // Purple 454a70 samples on the player update, never on presentation redraw.
    if(life.practice&&life.practice->enabled&&life.practice->record_keys)life.practice->record_keys(frame.held);
    motion.flip_vertical_step=life.practice&&life.practice->enabled&&life.practice->flip_screen_y;
    context=frame;active_rate=&rate;animations.rate=rate;shots.set_rate(rate);motion.input_frame=input_age.current;
    if(!life.advance_state(context.pressed,rate,context.hud_available,*this)){error=life.error.empty()?"Player lifecycle service failed":life.error;return false;}
    if(!world.refresh(context)){error="Player world context unavailable";return false;}animations.rate=rate;shots.set_rate(rate);motion.movement_scale=1;motion.external_velocity=context.background_delta;damage.tick(rate);
    auto& vm=visuals.root;bool flash=false;if(life.invulnerability.current>0){life.invulnerability.decrement(&rate);flash=life.age.current!=life.age.previous&&life.age.current%3==0;}
    if(flash){vm.visual.secondary_color=0xff0000ff;vm.visual.flags=(vm.visual.flags&~0x40000u)|0x20000;}else vm.visual.flags&=~0x60000u;
    if(animations.tick_instance(vm)<0){error=animations.error;return false;}bounds.update(motion,life.age,vm,rate);life.age.tick(&rate);input_age.tick(&rate);ready_age.tick(&rate);motion.input_frame=input_age.current;
    auto& session=life.session_state();shot_context.power=session.power;shot_context.power_step=session.power_step;shot_context.laser_power[0]=motion.option_count;shot_context.enemy_scene=context.enemy_present;shot_context.enemies=context.enemies;shot_context.bomb_active=context.hud_bomb_active;shot_context.background_delta=context.background_delta;
    if(!context.hud_bomb_active&&context.enemy_present&&context.focus_allowed&&!(context.game_flags&0x4000)&&ready_age.current>19&&!(motion.behavior_flags&20)){if(!shooting.update(life.state,context.held&1,rate)){error=shooting.error;return false;}}
    else{shooting.shot.set(-1);shooting.continuous.set(-1);shooting.auxiliary=0;shooting.auxiliary_active=false;if(!world.stop_sound(30)||!world.stop_sound(55)){error="Player shot audio stop failed";return false;}}
    shot_context.shoot_frame=shooting.shot.current;if(!shots.update()){error=shots.error;return false;}
    collision.position={motion.position.x,motion.position.y};collision.hitbox_min={bounds.hit.minimum.x,bounds.hit.minimum.y};collision.hitbox_max={bounds.hit.maximum.x,bounds.hit.maximum.y};collision.state=life.state;collision.invulnerability=life.invulnerability.current;collision.enlarged=motion.behavior_flags&16;collision.size_multiplier=motion.enlargement;collision.bomb_active=context.hud_bomb_active;collision.laser_half_size={bounds.hit_half_size.x,bounds.hit_half_size.y};active_rate=nullptr;return true;
}
}
