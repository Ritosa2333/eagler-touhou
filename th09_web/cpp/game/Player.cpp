#include "Player.hpp"
#include "HazardTrace.hpp"
#include "Localization.hpp"
#include <algorithm>
namespace th09 {
bool Player::initialize(const u8* data,u32 size,u32 side,u32 character,i32 controller){
    if(side>1||character>15||!resource.load(data,size))return false;
    // thcrap th09/spells.js indexes each attack by the original .sht slot:
    // character * 10 + level. Slots whose Japanese name repeats an earlier
    // level (most characters repeat level 0 at level 1) have no entry of
    // their own, so resolve those against the character's base spell instead
    // of emitting the untranslated CP932 name, which the pack font cannot
    // render. This mirrors TH10 SpellStart resolving the raw spell id.
    const auto originals=resource.spell_names;
    for(u32 level=0;level<resource.spell_names.size();++level){
        const char* translated=Localization::SpellName(character*10+level,nullptr);
        if(translated==nullptr&&level>0&&originals[level]==originals[0])
            translated=Localization::SpellName(character*10,nullptr);
        resource.spell_names[level]=translated!=nullptr?translated:originals[level];
    }
    motion.player=side;motion.character=character;life.input_controller=controller;shots.side=i32(side);
    motion.hit_extent={resource.hit_size*.5f,resource.hit_size*.5f,5};motion.graze_extent={resource.graze_size*.5f,resource.graze_size*.5f,5};motion.item_extent={resource.item_size*.5f,resource.item_size*.5f,5};
    control.available=100;initialized=true;return true;
}
void Player::reset_round(i32 health,i32 difficulty,i32 round,float height){
    actions.body_interrupt(body,0);motion.position={-160,height-64,.49f};shots.areas.reset();
    motion.direction=0;combo_state.pending=0;control.player_state=5;control.protection.reset(120);
    for(auto& shot:shots.shots)shot.state=0;motion.base_scale=motion.effect_scale={1,1};motion.history.fill(motion.position);
    control.flags=(control.flags&~1u)|2;control.cooldown.reset(6);cpu.charge_goal=std::min(float(world.random.range(400))+100.f,400.f);
    flush_combo();control.shock.reset();control.charge=0;motion.health=health;
    if(difficulty==4){motion.health=1;control.available=400;}
    if(difficulty>=0&&difficulty<=4)attack_levels[0]=attack_levels[1]=std::min(round,2)+(difficulty<2?1:difficulty==2?4:difficulty==3?8:12);
    if(motion.health==1)control.flags|=8;control.normal_fire.reset(-1);motion.angle=-1.5707963705062866f;motion.focus_effects=false;cpu.reset_damage();std::memset(items.items.data(),0,sizeof(items.items));
    motion.flags=control.flags;
}
void Player::charge(float n){ChargeGauge gauge{control.available};const auto level=gauge.add(n,motion.character);control.available=gauge.value;if(level>=0)actions.charge_level(level);}
void Player::synchronize_shots(){shots.player_position=motion.position;shots.player_scale=motion.base_scale;shots.side=i32(motion.player);}
void Player::update(const PlayerFrameContext& context,DamageRules& damage){
    frame=context;if(frame.game_flags&0x1800)return;const bool frozen=(frame.field_flags&1)!=0;
    if(frozen&&motion.character!=2)return;
    if(!frozen){ItemContext item;item.state=control.player_state;item.side=i32(motion.player);item.rank=world.rank;item.difficulty=world.difficulty;item.pickup_size=resource.item_size;item.player=motion.position;item.pickup_bounds=motion.item_bounds;item.geometry[0]=frame.geometry[0];item.geometry[1]=frame.geometry[1];items.update(item);}
    if(control.player_state==5)return;
    if(control.player_state==4)life.recover(frame.limits);else if(control.player_state==1)life.enter();
    if(life.shield_active&&control.player_state==3)actions.shield_position(motion.position);life.update_timers(frame.timing);
    if(life.input_controller>0){
        auto c=frame.computer;c.player.state=control.player_state;c.player.invulnerability=control.shock;c.player.position=motion.position;c.player.half_extent=motion.hit_extent;c.player.hit_radius=resource.hit_size;c.player.hit_bounds=motion.hit_bounds;
        c.side=i32(motion.player);
        c.health=motion.health;c.base_scale=motion.base_scale;c.effect_scale=motion.effect_scale;c.speeds=resource.movement;c.limits=frame.limits;c.field_flags=frame.field_flags;c.scene_locked=frame.scene_state!=0;c.dialogue=frame.dialogue;c.extra_mode=frame.extra_mode;
        c.item_target={items.attraction.x,items.attraction.y,0};c.attack_areas=shots.areas.count;
        CpuPlayer(cpu,world.random,hazards,actions).update(input,c,control,frame.timing);motion.hit_bounds=c.player.hit_bounds;if(!c.scene_locked&&!c.dialogue)damage.extra_damage=c.extra_damage;
    }
    shots.areas.update();ShotControl shooting(control,shots.areas,*this);
    if(!frame.dialogue)shooting.bomb(input,frame.scene_state!=0,resource,motion.position);
    if(control.player_state!=2&&control.player_state!=4&&control.player_state!=1){motion.flags=control.flags;motion.update(input.held,frame.automatic_focus,input.fire_frames,resource.movement,frame.limits,frame.timing.rate,*this);control.flags=motion.flags;}
    actions.advance_animation(body);synchronize_shots();shots.update(frame.timing,frame.field_flags);motion.base_scale=shots.player_scale;
    if(!frame.dialogue){
        combo_state.pending=wrapping_add(combo_state.pending,1);
        if(control.flags&8){actions.play_sound(48,motion.player?500:-500);actions.critical_health();control.flags&=~8u;if(frame.extra_mode&&motion.player==0)actions.critical_opponent_time(CpuPlayer::survival_seconds(frame.opponent_level)*60-frame.opponent_survival_time);}
        if(!frame.scene_state)shooting.update(input,frame.automatic_focus,i32(motion.player),resource,motion.position,frame.timing);
    }
    if(!frozen&&!frame.scene_state)life.collide(hazards,resource.hit_size,damage,resource);
    shots.target=secondary_target={-999,-999,0};target_priority=0;motion.effect_scale={1,1};motion.flags=control.flags;
    if(!frozen)combo_system().update_timers(combo_state);hazard_trace.push(i32(motion.player),hazards);hazards.clear();
}
void Player::draw(bool fading){
    const auto& geometry=frame.geometry[motion.player];if(fading){shots.draw(geometry,true);return;}
    if(control.player_state!=5){shots.draw(geometry,false);if(motion.health==0&&(frame.scene_state>1||frame.ending_frames>30))return;
        if(!frame.hide_players){body.pos=geometry.to_screen(motion.position);body.pos.z=.1f;actions.draw_body(body);}}
    items.draw(geometry);
}
}
