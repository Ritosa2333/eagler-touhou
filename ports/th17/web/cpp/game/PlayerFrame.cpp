#include "PlayerFrame.hpp"
namespace th17 {
PlayerFrame::PlayerFrame(PlayerMotion& p,PlayerAnmHost& v,AnmManager& a,PlayerLife& l,DamageSources& d,PlayerShots& s,PlayerShotContext& c,PlayerFrameHost& w,i32 ch,i32 resource):motion(p),visuals(v),animations(a),life(l),damage(d),shots(s),shot_context(c),world(w),character(ch),player_resource(resource),shooting_host(a,resource),shooting(p,shooting_host){
    shooting_host.shoot=[this](i32 frame,i32 continuous){shot_context.shoot_frame=frame;return shots.fire(frame,continuous);};
    shooting_host.audio=[this](i32 id,ShotSoundAction action,float x){return world.shot_audio(id,action,x);};
}
bool PlayerFrame::visual_feedback(float rate){
    auto& vm=visuals.root;
    if(life.invulnerability.current>0){
        life.invulnerability.decrement(&rate);
        if(life.age.current!=life.age.previous&&life.age.current%3==0){vm.visual.secondary_color=0xff0000ff;vm.visual.flags=(vm.visual.flags&~0x40000u)|0x20000;}else vm.visual.flags&=~0x60000u;
        return true;
    }
    vm.visual.flags&=~0x60000u;const bool red=motion.behavior_flags&0x20;
    if(!red&&motion.movement_scale<=1.01f)return true;
    if(life.age.current%8<4){vm.visual.secondary_color=red?0xffff0000:0xffffff00;vm.visual.flags=(vm.visual.flags&~0x40000u)|0x20000;}
    constexpr i32 scripts[3]={17,19,21};if(character<0||character>=3){error="Player feedback character unavailable";return false;}
    const u32 handle=animations.create(player_resource,scripts[character],-1,0,motion.position);auto* effect=animations.registry.find(handle);
    if(!effect||!effect->select_sprite(vm.visual.sprite)){error=animations.error.empty()?"Player feedback sprite unavailable":animations.error;return false;}
    if(red)effect->visual.color=0xffff0000;return true;
}
bool PlayerFrame::update(const PlayerFrameContext& frame,float& rate){
    context=frame;active_rate=&rate;animations.rate=rate;shots.set_rate(rate);motion.input_frame=input_age.current;
    if(!life.advance_state(context.pressed,rate,context.hud_available,*this)){error=life.error.empty()?"Player lifecycle service failed":life.error;return false;}
    if(!world.refresh(context)){error="Player world context unavailable";return false;}animations.rate=rate;shots.set_rate(rate);damage.tick(rate);
    if(!visual_feedback(rate))return false;motion.movement_scale=1;motion.external_velocity=context.background_delta;
    if(animations.tick_instance(visuals.root)<0){error=animations.error;return false;}bounds.update(motion,life.age,visuals.root,rate);life.age.tick(&rate);input_age.tick(&rate);ready_age.tick(&rate);motion.input_frame=input_age.current;
    auto& session=life.session_state();shot_context.power=session.power;shot_context.power_step=session.power_step;shot_context.enemy_scene=context.enemy_present;shot_context.targets=context.targets;shot_context.bomb_active=context.hud_bomb_active;shot_context.background_delta=context.background_delta;shot_context.player_state=life.state;shot_context.roaring_flags=session.roaring_flags;shot_context.roaring_kind=context.roaring_kind;
    if(!context.hud_bomb_active&&context.enemy_present&&context.focus_allowed&&!(context.game_flags&0x4000)&&ready_age.current>19&&!(motion.behavior_flags&20)){
        if(!shooting.update(life.state,context.held&1,rate,character,session.animal,session.roaring_flags,context.roaring_kind)){error=shooting.error.empty()?"Player shooting service failed":shooting.error;return false;}
    }else if(!shooting.blocked()){error="Player blocked-shooting cleanup failed";return false;}
    shot_context.shoot_frame=shooting.shot.current;if(!shots.update()){error=shots.error;return false;}
    if(auto* vm=animations.registry.find(shooting.charge_handle))vm->visual.translation=motion.position;else shooting.charge_handle=0;
    motion.behavior_flags&=~0x40u;
    collision.position={motion.position.x,motion.position.y};collision.hitbox_min={bounds.hit.minimum.x,bounds.hit.minimum.y};collision.hitbox_max={bounds.hit.maximum.x,bounds.hit.maximum.y};collision.state=life.state;collision.invulnerability=life.invulnerability.current;collision.enlarged=motion.behavior_flags&16;collision.size_multiplier=motion.enlargement;collision.bomb_active=context.hud_bomb_active;collision.laser_half_size={bounds.hit_half_size.x,bounds.hit_half_size.y};active_rate=nullptr;return true;
}
}
