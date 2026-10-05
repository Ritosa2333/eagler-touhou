#include "PlayerSimulation.hpp"
#include "Presentation.hpp"
#include "PresentationAudit.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "../multiplayer/PlayerPresentation.hpp"
#endif
#include <algorithm>
#include <cmath>
namespace th08 {
namespace {
Vec3 presentation_lerp(const Vec3& previous,const Vec3& current){
    return {presentation::lerp_world(previous.x,current.x),presentation::lerp_world(previous.y,current.y),presentation::lerp_world(previous.z,current.z)};
}
bool presentation_near(const Vec3& previous,const Vec3& current){const float dx=current.x-previous.x,dy=current.y-previous.y;return dx*dx+dy*dy<4096.0f;}
}
PlayerSimulation::PlayerSimulation(PlayerSimulationState& s,ShotResource (&r)[2],GameGlobals& v,GameGauge& g,GaugeThresholds& t,GameRank& rank,Rng& random,PlayerSimulationServices a)
    :state(s),resources(r),gauge(g),thresholds(t),services(a),values(v),rank(rank),life(s.life,s.context,s.motion.movement,s.motion.animation,a.life),shots(s.shots,random),patterns(s.bomb_objects,s.bomb,s.life,s.context,s.motion.movement,s.bomb_input,s.shots.regions,random,a.patterns),collisions(s.motion.movement,s.life,s.context,s.shots.regions,s.cancel_item,*this){
    shots.actions=&services.shots;patterns.frame.options=s.motion.options;patterns.frame.main_animation=&s.motion.animation;
}
bool PlayerSimulation::initialize(const PlayerSetupContext& context){
    failed=!initialize_player(state.motion,state.life,state.bomb,state.shots,thresholds,resources[0].settings(),context,services.setup);
    initialized=!failed;if(initialized){state.context.character=state.input.character=context.character;state.context.extent=context.extent;synchronize_shots();presentation_previous_position=state.motion.movement.position;presentation_previous_scale=state.motion.animation.scale;presentation_previous_color=state.motion.animation.color1;presentation_previous_script=state.motion.animation.scriptIndex;presentation_previous_sprite=state.motion.animation.activeSpriteIndex;presentation_previous_life_state=state.life.state;for(u32 i=0;i<4;++i){presentation_previous_options[i]=state.motion.options[i].position;presentation_previous_option_state[i]=state.motion.options[i].state;}presentation_valid=true;}return initialized;
}
void PlayerSimulation::synchronize_shots(){
    auto& s=state.shots;s.position=state.motion.movement.position;for(u32 i=0;i<4;++i)s.options[i]=state.motion.options[i].position;
    s.homing_target=state.frame.homing_target;s.target=state.frame.shot_target;s.enemy_target=state.enemy_origin;s.enemy_available=state.input.enemy_present;s.orbit_angle=state.motion.options[2].shot_angle;
    s.bomb=state.bomb.active;s.game_flags=state.context.game_flags;s.focused=state.motion.form.focused;s.youkai_bonus=gauge.youkai_bonus();s.character=state.context.character;
    s.player_state=state.life.state;s.gui_blocked=state.input.gui_blocked;s.option_active=state.motion.options[0].state!=0;s.collision_timer=state.life.timer;s.time_spell=state.context.time_spell;s.human_bonus=gauge.human_bonus();shots.timing=timing;
}
bool PlayerSimulation::update(){
    if(!initialized)return false;failed=false;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(state.context.pause||state.input.gui_blocked||(state.context.game_flags&516)!=4||
       (state.life.state!=0&&state.life.state!=3))state.touch_remainder={};
#endif
    if(presentation_marker.capture()){
        presentation_previous_animation.capture(state.motion.animation);
        for(u32 i=0;i<4;++i)presentation_previous_option_animation[i].capture(state.motion.options[i].animation);
        presentation_previous_position=state.motion.movement.position;presentation_previous_scale=state.motion.animation.scale;presentation_previous_color=state.motion.animation.color1;presentation_previous_script=state.motion.animation.scriptIndex;presentation_previous_sprite=state.motion.animation.activeSpriteIndex;presentation_previous_life_state=state.life.state;
        for(u32 i=0;i<4;++i){presentation_previous_options[i]=state.motion.options[i].position;presentation_previous_option_state[i]=state.motion.options[i].state;}
        presentation_valid=true;
    }
    state.context.focused=state.motion.form.focused;state.context.gauge=gauge.value();state.bomb_input.buttons=state.input.buttons;state.bomb_input.gui_blocked=state.input.gui_blocked;state.bomb_input.tampered=state.input.tampered;
    synchronize_shots();update_player_frame(state.frame,state.motion,state.life,state.shots.regions,gauge,state.context.pause!=0,*this);synchronize_shots();return !failed;
}
void PlayerSimulation::update_bomb(){failed|=!update_player_bomb(state.bomb,state.bomb_input,state.life,state.context,state.motion.movement,state.motion.animation,resources[0].settings(),timing,*this);}
void PlayerSimulation::update(PlayerBombKind kind){patterns.frame.timing=timing;patterns.frame.homing_target=state.frame.homing_target;patterns.frame.shooting_timer=state.shots.shooting_timer;failed|=!patterns.update(kind);}
bool PlayerSimulation::resolve_death(){return life.resolve_death(resources[0].settings());}
void PlayerSimulation::respawn(){life.respawn(resources[0].settings());
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    state.touch_remainder={};
#endif
}
void PlayerSimulation::update_invincibility(){life.update_invincibility(timing);}
void PlayerSimulation::update_motion(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    const auto previous=state.motion.movement.position;
#endif
    state.input.bomb=state.bomb.active;state.input.bomb_type=state.bomb.type;state.input.character=state.context.character;
    update_player_motion(state.motion,state.input,state.shots.shooting_timer,gauge,resources[0].settings(),resources[1].settings(),timing,services.motion);state.context.focused=state.motion.form.focused;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    const auto moved_x=Scalar::sub(state.motion.movement.position.x,previous.x);
    const auto moved_y=Scalar::sub(state.motion.movement.position.y,previous.y);
    if(state.touch_remainder.active){
        state.touch_remainder.x=Scalar::sub(state.touch_remainder.x,moved_x);
        state.touch_remainder.y=Scalar::sub(state.touch_remainder.y,moved_y);
        if(std::abs(state.touch_remainder.x)<.0001f)state.touch_remainder.x=0;
        if(std::abs(state.touch_remainder.y)<.0001f)state.touch_remainder.y=0;
    }
    if(state.analog.unlimited&&(state.context.game_flags&516)==4&&!state.input.gui_blocked&&
       (moved_x!=0||moved_y!=0))state.unlimited_movement_used=1;
#endif
    if(!state.input.enemy_present)state.frame.target_reference=nullptr;
}
void PlayerSimulation::step_animation(AnmVm& vm){services.shots.step_animation(vm);}
void PlayerSimulation::update_shots(){synchronize_shots();failed|=!shots.update();}
void PlayerSimulation::update_shooting(){
    const ShotFiringInputs input{state.stage_play_frames,state.bomb.active,state.bomb.type,state.input.buttons,state.context.character,state.input.gui_blocked,state.life.state,{}};
    th08::update_shooting(state.shots.shooting_timer,input,timing,*this);
}
void PlayerSimulation::fire(i32 frame){
    synchronize_shots();const auto& resource=resources[state.motion.form.focused!=0];const i32 index=resource.select(state.context.power,state.context.character,state.bomb.active,state.bomb.type,state.bomb.timer.current>=60);
    const auto* stream=index<0?nullptr:resource.stream(index);if(!stream){failed=true;return;}shots.emit(*stream,frame);failed|=shots.failure!=PlayerShots::Failure::None;
}
void PlayerSimulation::die(){state.context.focused=state.motion.form.focused;state.context.gauge=gauge.value();life.die();gauge.set(state.context.gauge);synchronize_shots();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    state.touch_remainder={};
#endif
}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
void PlayerSimulation::enter_spirit(){
    state.touch_remainder={};
    state.life.state=4;state.context.game_over=0;state.bomb.active=0;
    state.life.team_bomb_protection.set(0);
    if(state.motion.gauge.effect){state.motion.gauge.effect->active=0;state.motion.gauge.effect=nullptr;}
    if(state.motion.form.focus_effect){static_cast<EffectState*>(state.motion.form.focus_effect)->active=0;state.motion.form.focus_effect=nullptr;}
    state.shots.shooting_timer.set(-1);
    for(auto& shot:state.shots.shots){if(shot.state||shot.update!=ShotUpdate::None||shot.draw!=ShotDraw::None||shot.hit!=ShotHit::None)services.shots.before_shot_write(shot);shot.state=0;shot.update=ShotUpdate::None;shot.draw=ShotDraw::None;shot.hit=ShotHit::None;}
    for(auto& laser:state.shots.lasers)laser.shot=nullptr;
    for(auto& region:state.shots.regions.damaging)region.reset();
    for(auto& region:state.shots.regions.cancelling)region.reset();
    presentation_valid=false;
}
bool PlayerSimulation::update_spirit(i8& drift_x,i8& drift_y){
    if(!initialized)return false;
    if(state.context.pause)return !failed;
    if(presentation_marker.capture()){
        presentation_previous_position=state.motion.movement.position;
        presentation_previous_animation.capture(state.motion.animation);
        presentation_valid=true;
    }
    auto& p=state.motion.movement.position;
    p.x=Scalar::add(p.x,drift_x>0?.2f:-.2f);
    p.y=Scalar::add(p.y,drift_y>0?.2f:-.2f);
    if(p.x<8.f){p.x=8.f;drift_x=1;}else if(p.x>368.f){p.x=368.f;drift_x=-1;}
    if(p.y<316.f){p.y=316.f;drift_y=1;}else if(p.y>416.f){p.y=416.f;drift_y=-1;}
    state.motion.movement.delta={};
    step_animation(state.motion.animation);
    return !failed;
}
void PlayerSimulation::revive_spirit(){
    state.life.state=3;state.life.timer.set(240);state.life.clear_frames=0;
    state.life.predead_count=profile(false).deathbomb_limit;
    state.context.game_over=0;state.motion.form.focused=2;
    state.motion.animation.scale={1,1};state.motion.animation.blendMode=0;
    state.motion.animation.color1.d3dColor=0xffffffff;
    state.motion.movement.delta={};presentation_valid=false;
}
void PlayerSimulation::place_multiplayer_spawn(u32 seat,u32 count){
    if(count<2||count>3||seat>=count)return;
    auto& movement=state.motion.movement;
    movement.position.x=192.f+32.f*float(i32(2*seat)-i32(count-1))*.5f;
    movement.position.y=384.f;
    for(auto& point:movement.history)point=movement.position;
    state.shots.position=movement.position;
    presentation_previous_position=movement.position;presentation_valid=false;
}
#endif
void PlayerSimulation::graze(const Vec3& position,bool laser){
    PlayerGrazeContext context{state.motion.movement.position,state.bomb.active,state.context.hud_flags,state.context.replay_flags,state.context.character,state.motion.form.youkai,state.context.time_spell,u8(services.world.boss_present()),{}};
    graze_player(context,values,gauge,rank,position,laser,*this);state.context.hud_flags=context.hud_flags;state.context.replay_flags=context.replay_flags;state.context.gauge=gauge.value();
}
i32 PlayerSimulation::damage(const Vec3& position,const Vec3& size,i32& time_items,i32* bomb_hit){synchronize_shots();const i32 result=shots.damage(position,size,time_items,bomb_hit);failed|=shots.failure!=PlayerShots::Failure::None;return result;}
Vec3 PlayerSimulation::presentation_position()const{
    const auto& p=state.motion.movement.position;
    if(presentation::render_only&&presentation::active&&presentation_valid&&
       presentation_previous_life_state==state.life.state&&presentation_near(presentation_previous_position,p))
        return presentation_lerp(presentation_previous_position,p);
    return p;
}
bool PlayerSimulation::draw(const Vec2& offset,bool impacts,u8 proximity_alpha){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(state.life.state==4){
        if(!impacts){
            AnmVm vm=state.motion.animation;
            Vec3 p=state.motion.movement.position;
            if(presentation::active&&presentation_valid&&presentation_near(presentation_previous_position,p))p=presentation_lerp(presentation_previous_position,p);
            vm.pos={Scalar::add(offset.x,p.x),Scalar::add(offset.y,p.y),.1f};
            vm.color1.a=multiplayer::clamp_player_alpha(u8(std::min<u32>(vm.color1.a,80)),proximity_alpha);
            vm.color2.a=multiplayer::clamp_player_alpha(vm.color2.a,proximity_alpha);
            services.motion.draw_player(vm);
        }
        return !failed;
    }
#endif
    TH08_AUDIT_SCOPE(Player,&state.motion,state.motion.animation.currentTimeInScript.current,impacts?1:0);
    const bool failed_before=failed;const auto shot_failure_before=shots.failure;
    if(!presentation::render_only)synchronize_shots();shots.draw(impacts,offset,proximity_alpha);if(!presentation::render_only)failed|=shots.failure!=PlayerShots::Failure::None;
    if(!impacts){
        if(state.bomb.active){
            const Vec3 saved=state.motion.movement.position;
            const bool smooth=presentation::active&&presentation_valid&&presentation_previous_life_state==state.life.state&&presentation_near(presentation_previous_position,state.motion.movement.position);
            if(smooth)state.motion.movement.position=presentation_lerp(presentation_previous_position,state.motion.movement.position);
            const bool bomb_ok=patterns.draw(player_bomb_kind(state.context.character,state.bomb.type),offset);if(!presentation::render_only)failed|=!bomb_ok;
            if(smooth)state.motion.movement.position=saved;
        }
        if(presentation::render_only){
            auto draw=state.motion;
            if(presentation::active&&presentation_valid){
                presentation_previous_animation.apply(state.motion.animation,draw.animation,presentation::world_alpha);
                for(u32 i=0;i<4;++i)if(presentation_previous_option_state[i]==state.motion.options[i].state)
                    presentation_previous_option_animation[i].apply(state.motion.options[i].animation,draw.options[i].animation,presentation::world_alpha);
                draw.movement.position=presentation_position();
                for(u32 i=0;i<4;++i)if(presentation_previous_option_state[i]==state.motion.options[i].state&&presentation_near(presentation_previous_options[i],state.motion.options[i].position))draw.options[i].position=presentation_lerp(presentation_previous_options[i],state.motion.options[i].position);
                // TH08's death/respawn owner directly animates the player's
                // scale and alpha rather than using a generic ANM interpolator.
                // Smooth only those two continuous lifecycle states; state 3's
                // deliberate red/white invincibility blink stays discrete.
                auto& animation=draw.animation;const auto& current=state.motion.animation;
                if((state.life.state==1||state.life.state==2)&&presentation_previous_life_state==state.life.state&&presentation_previous_script==current.scriptIndex&&presentation_previous_sprite==current.activeSpriteIndex){
                    if(presentation_previous_scale.x*current.scale.x>=0&&presentation_previous_scale.y*current.scale.y>=0)animation.scale={presentation::lerp_world(presentation_previous_scale.x,current.scale.x),presentation::lerp_world(presentation_previous_scale.y,current.scale.y)};
                    animation.color1.a=u8(std::clamp(presentation::lerp_world(float(presentation_previous_color.a),float(current.color1.a)),0.0f,255.0f));
                }
            }
            draw_player_motion(draw,offset,state.context.game_over,services.motion,proximity_alpha);
        }else draw_player_motion(state.motion,offset,state.context.game_over,services.motion,proximity_alpha);
    }
    if(presentation::render_only){failed=failed_before;shots.failure=shot_failure_before;}
    return !failed;
}
}
