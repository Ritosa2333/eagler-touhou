#include "PlayerLife.hpp"
#include <algorithm>
#include <cmath>
namespace th09 {
void PlayerLife::damage(DamageRules& rules,const ShotResource& resource){
    if(rules.blocked)return;actions.play_positioned_sound(4,motion.position.x);const i32 amount=input_controller?wrapping_add(rules.damage,rules.extra_damage):rules.damage;
    if(motion.health<2){
        if(motion.character==8&&control.available>=200){ShotControl bomb(control,areas,actions);bomb.automatic_defence(resource,motion.position);return;}
        motion.health=0;actions.opponent_wins(1-i32(motion.player));actions.slotted_effect(19,motion.position,4,0xff4040ff);
    }else{
        const i32 remaining=wrapping_sub(motion.health,amount);
        if(remaining<2){actions.play_sound(48,motion.player?500:-500);actions.critical_health();motion.health=1;}else motion.health=remaining;
    }
    control.player_state=4;control.protection.reset();motion.angle=float(random.signed_unit())*3.1415927410125732f;
    motion.step={float(std::cos(double(motion.angle))),float(std::sin(double(motion.angle)))};
    actions.damage_flash(i32(motion.player),60,0x80ff0000);
    if(motion.focus_effects){actions.end_focus();motion.focus_effects=false;}
    rules.rank_charge=std::max(wrapping_sub(rules.rank_charge,1500),0);if(rules.damage>=5)rules.damage=wrapping_sub(rules.damage,2);else if(rules.damage>=3)rules.damage=wrapping_sub(rules.damage,1);
    actions.flush_combo();actions.reset_ai();
}
bool PlayerLife::collide(PlayerHazards& hazards,float radius,DamageRules& rules,const ShotResource& resource){
    if(control.player_state!=0||control.shock.current>0)return false;
    HazardPlayer p;p.state=control.player_state;p.invulnerability=control.shock;p.position=motion.position;p.half_extent=motion.hit_extent;p.hit_radius=radius;
    auto hit=hazards.first_hit(p);motion.hit_bounds=p.hit_bounds;if(!hit)return false;if(hit->bullet)hit->bullet->state=5;damage(rules,resource);return true;
}
void PlayerLife::recover(const PlayfieldLimits& limits){
    if(control.protection.value()>=60){actions.charge(motion.health==1?400.f:130.f-float(motion.health)*10.f);control.player_state=3;control.protection.reset(60);
        areas.circle(motion.position,16,20,0,8,0,AttackAreaKind::cancel);areas.circle(motion.position,16,20,10,8,0,AttackAreaKind::special_damage);return;
    }
    const float time=60.f-control.protection.value();motion.position.x=(time*motion.step.x)*.05f+motion.position.x;motion.position.y=(time*motion.step.y)*.05f+motion.position.y;motion.position.z=(time*knockback_z)*.05f+motion.position.z;
    motion.position.x=std::clamp(motion.position.x,limits.origin.x,limits.extent.x+limits.origin.x);motion.position.y=std::clamp(motion.position.y,limits.origin.y,limits.extent.y+limits.origin.y);
    body.color1.d3dColor=i32((u32(control.protection.current*255/10)<<24)|0xffffff);
}
void PlayerLife::enter(){
    display_frames=60;const float remaining=1.f-control.protection.value()*.03333333507180214f;body.scale.y=remaining+remaining+1.f;body.scale.x=1.f-remaining;body.blendMode=1;
    motion.base_scale={1,1};body.color1.d3dColor=i32((u32(control.protection.current*255/30)<<24)|0xffffff);hidden=0;
    motion.position.x=remaining*(motion.player?176.f:-176.f);motion.position.y=remaining*64.f+384.f;
    if(control.protection.current>29){control.player_state=3;body.scale={1,1};body.color1.d3dColor=-1;body.blendMode=0;motion.position.x=0;motion.position.y=384;}
}
void PlayerLife::update_timers(const FrameTiming& timing){
    if(display_frames)display_frames=wrapping_sub(display_frames,1);
    if(control.player_state==3){
        if(shield_active)shield_position=motion.position;control.protection.decrement(1,timing);
        if(control.protection.current<1){if(shield_active){actions.remove_shield();shield_active=false;}control.player_state=0;control.protection.reset();body.color1.d3dColor=-1;}
        else body.color1.d3dColor=control.protection.current%8>1?-1:i32(0xfff02020);
    }else control.protection.tick(timing);
    if(control.shock.current>0){body.color1.d3dColor=control.protection.current%4<2?i32(0xfff02020):-1;control.shock.decrement(1,timing);if(control.shock.current<=0){control.shock.reset();body.color1.d3dColor=-1;}}
}
}
