#include "PlayerFrame.hpp"
#include "AnmRenderer.hpp"
#include "Replay.hpp"
#include "PracticeConfig.hpp"
#include <algorithm>
#include <cmath>
namespace th11 {
namespace {
PlayerBounds bounds(Vec3 p,Vec3 r,double scale=1){return{{float(double(p.x)-double(r.x)*scale),float(double(p.y)-double(r.y)*scale),float(double(p.z)-double(r.z)*scale)},{float(double(p.x)+double(r.x)*scale),float(double(p.y)+double(r.y)*scale),float(double(p.z)+double(r.z)*scale)}};}
Vec3 screen(Vec3 p){return{float(double(p.x)+224),float(double(p.y)+16),p.z};}
}
PlayerFrame::PlayerFrame(ShtResource& s,AnmResource& p,AnmResource& b,AnmManager& a,GameEconomy& e,PlayerFrameWorld& w,u16 pf,u16 bf):motion(p,b,a,*this,pf,bf),shots(s,p,a,w,pf),resource(s),bullet_resource(b),animations(a),economy(e),world(w),bullet_file(bf){state.state_timer.set(0,&a.rate);state.transition_timer.set(0,&a.rate);state.invincibility.set(0,&a.rate);}
bool PlayerFrame::initialize(){
    const i32 character=input.movement.character;
    if(character<0||character>1||input.movement.subtype<0||input.movement.subtype>2){last_error=4;return false;}
    auto& s=motion.state;auto& header=resource.header;
    motion.body.rectangle_columns=motion.body.rectangle_rows=16;
    if(!motion.reset_body())return false;
    s.position.x=0;s.position.y=400;s.x=0;s.y=0xc800;motion.set_speeds(header);
    economy.max_power=header.max_power_level*header.power_step;economy.power_step=header.power_step;
    shots.schedule.timer.set(-1,&animations.rate);
    header.hitbox=character?3.5f:2.f;header.attraction_diameter=character?70.f:60.f;header.attraction_speed=character?7.f:5.f;
    const float half=header.hitbox*.5f,pickup=header.attraction_diameter*.5f,focus=character?59.f:50.f;
    state.hit_half={half,half,5};state.pickup_size={pickup,pickup,5};state.focused_half={focus,focus,5};
    state.hit=bounds(s.position,state.hit_half);state.pickup=bounds(s.position,state.pickup_size);
    state.focused_attract=bounds(s.position,state.focused_half);state.unfocused_attract=state.focused_attract;
    state.state_timer.set(0,&animations.rate);state.invincibility.set(120,&animations.rate);
    s.option_count=0;s.option_lerp=30;return true;
}
bool PlayerFrame::rebuild_options(){return motion.rebuild_options(resource.header,economy,input.movement.character*3+input.movement.subtype);}
bool PlayerFrame::practice_power_locked()const{
#ifdef TH_ENABLE_THPRAC
    return practice&&practice->enabled&&!input.replay&&(practice->cheats&4);
#else
    return false;
#endif
}
bool PlayerFrame::restore_replay_position(const ReplayStageState& entry){
    entry.restore_position(motion.state);for(auto& option:motion.options)option.snap=1;
    // 436da0 recreates the non-default Marisa/B formation, twice. The second
    // call also refreshes max-power auxiliary effects; do not remove it.
    if(entry.weapon){motion.state.option_count=0;return rebuild_options()&&rebuild_options();}
    return true;
}
bool PlayerFrame::start_stage(){
    // 42fec0 resets lifecycle clocks but preserves position, weapon mode and
    // existing shots. Reinitializing the player here breaks replay entry state.
    state.life_state=1;shots.schedule.timer.set(-1,&animations.rate);
    state.state_timer.set(0,&animations.rate);state.transition_timer.set(0,&animations.rate);
    motion.state.transition_timer=0;
    motion.state.flags&=~7u;motion.state.warp=motion.state.warp_timer=0;
    motion.clear_focus();
    if(!world.display_lives(economy.lives,i16(economy.life_fragments)))return false;
    motion.recall_options(false);return rebuild_options();
}
bool PlayerFrame::draw(AnmRenderer& renderer){
    if(state.life_state==2)return true;
    motion.body.position=screen(motion.state.position);
    if(renderer.draw(motion.body)==-2){last_error=5;return false;}
    // No shipped option constructor installs the optional native draw callback.
    for(const auto& option:motion.options)if(option.reservede0){last_error=6;return false;}
    return true;
}
bool PlayerFrame::cancel_all(bool spawning){return world.cancel_bullets(nullptr,0,spawning)&&world.cancel_lasers(nullptr,0,!spawning,spawning);}
void PlayerFrame::fail_spell(){if(!(spell.flags&1))return;if(spell.elapsed>=60){spell.bonus=0;spell.flags&=~0x22u;}else if(input.special_active&&input.movement.character*3+input.movement.subtype!=5)spell.flags|=0x20;}
bool PlayerFrame::hit(){
#ifdef TH_ENABLE_THPRAC
    if(practice&&practice->enabled&&!input.replay&&(practice->cheats&1))return true;
#endif
    state.life_state=4;const Vec3 p=screen(motion.state.position);
    for(i32 i=0;i<33;++i){auto* a=animations.create(bullet_resource,i?77:76,bullet_file,0);if(!a){last_error=1;return false;}a->position=p;}
    state.state_timer.set(0,&animations.rate);if(input.hit_sound&&!world.player_sound(4)){last_error=2;return false;}
    state.invincibility.set(6,&animations.rate);if(!motion.reset_body())return false;fail_spell();return true;
}
bool PlayerFrame::die(){
    economy.point_value=std::max(wrapping_add(economy.point_value,-10000000),state.minimum_point_value);
#ifdef TH_ENABLE_THPRAC
    if(!(practice&&practice->enabled&&!input.replay&&(practice->cheats&2)))
#endif
    economy.lives=wrapping_add(economy.lives,-1);economy.communication=0;
    if(economy.lives>=0&&!world.display_lives(economy.lives,i16(economy.life_fragments)))return false;
    state.life_state=2;state.state_timer.set(0,&animations.rate);state.invincibility.set(180,&animations.rate);
    if(!motion.reset_body())return false;motion.clear_options();
    if(!input.replay&&!world.record_death())return false;
    fail_spell();economy.add_rank(-1024);return true;
}
bool PlayerFrame::active_frame(){
    if(!input.movement.bomb&&input.special_available&&!input.special_active&&economy.power_step&&economy.power/economy.power_step&&(input.movement.held&2)){
        if(!world.start_bomb())return false;input.special_active=true;if(!practice_power_locked())economy.power=wrapping_sub(economy.power,economy.power_step);if(!rebuild_options())return false;
    }
    if(state.state_timer.current<30&&!cancel_all(true))return false;
    motion.state.transition_timer=state.transition_timer.current;return motion.update(input.movement);
}
bool PlayerFrame::death_frame(){
    economy.communication=0;
    if(state.state_timer.current==3){
        const i32 power=economy.power,step=economy.power_step;if(!practice_power_locked())economy.power=std::max(wrapping_sub(power,economy.max_power),0);
        const Vec3 p=motion.state.position;const float y=float(double(p.y)-224),dy=float(double(y)-p.y),dx=-p.x;
        const float angle=dx==0&&dy==0?1.5707963705062866f:float(std::atan2(double(dy),double(dx)));
        i32 types[7]={1,1,1,1,1,1,1};
        if(economy.lives<=0)types[3]=6;
        else if(power>=step*7)for(auto& t:types)t=4;
        else if(power>=step*6){for(i32 i:{0,1,2,4,5,6})types[i]=4;}
        else if(power>=step*5){for(i32 i:{1,2,3,4,5})types[i]=4;}
        else if(power>=step*4){for(i32 i:{1,2,4,5})types[i]=4;}
        else if(power>=step*3||economy.lives<3){for(i32 i:{1,3,5})types[i]=4;}
        else types[2]=types[4]=4;
        for(i32 i=0;i<7;++i){const float a=float(double(i)*3.1415927410125732/28+angle-0.39269909262657166);if(!world.spawn_item(types[i],p,0xffffff,a,3))return false;}
        if(!rebuild_options())return false;
    }
    if(state.state_timer.current>=30){
        if(economy.lives<0){if(!world.game_over(input.replay))return false;}
        else{state.life_state=0;animations.rate=1;shots.damage_areas.circle(motion.state.position,32,16,30,150,&animations.rate);state.death_position=motion.state.position;motion.state.position.x=0;motion.state.position.y=480;motion.state.x=0;motion.state.y=0xf000;state.invincibility.set(280,&animations.rate);state.state_timer.set(0,&animations.rate);}
    }return true;
}
void PlayerFrame::update_bounds(){const auto p=motion.state.position;state.hit=bounds(p,state.hit_half);state.pickup=bounds(p,state.pickup_size,.5);state.focused_attract=bounds(p,state.focused_half);state.unfocused_attract=bounds(p,state.pickup_size);}
bool PlayerFrame::update(){
    switch(state.life_state){
    case 0:{
        economy.communication=0;auto& s=motion.state;s.y=wrapping_sub(0xf000,signed_bits(u32(state.state_timer.current)*0x2800)/60);s.position.y=float(double(s.y)/128);for(auto& o:motion.options)o.snap=1;s.warp=s.warp_timer=0;
        if(state.state_timer.current<30){const float r=float(double(state.state_timer.current)*512/30+64),small=float(double(r)*.25);if(!world.cancel_bullets(&state.death_position,r,true)||!world.cancel_bullets(&state.death_position,small,false)||!world.cancel_lasers(&state.death_position,r,false,true)||!world.cancel_lasers(&state.death_position,small,false,false))return false;}
        else if(!cancel_all(true))return false;
        if(state.state_timer.current>=60){state.life_state=1;state.state_timer.set(0,&animations.rate);if(!active_frame())return false;}break;
    }
    case 1:if(!active_frame())return false;break;
    case 2:if(!death_frame())return false;break;
    case 3:if(state.state_timer.current==15&&!cancel_all(false))return false;break;
    case 4:
        economy.communication=0;
#ifdef TH_ENABLE_THPRAC
        if(practice&&practice->enabled&&!input.replay&&(practice->cheats&16))input.movement.held|=2;
#endif
        if(state.state_timer.current>7){if(!die()||!world.enemy_death()||!death_frame())return false;}
        else if(input.special_available&&!input.special_active&&economy.power_step&&economy.power/economy.power_step&&(input.movement.held&2)){
            state.state_timer.set(60,&animations.rate);if(!world.start_bomb())return false;input.special_active=true;if(!practice_power_locked())economy.power=wrapping_sub(economy.power,economy.power_step);if(!rebuild_options())return false;state.life_state=1;
        }break;
    default:break;
    }
    shots.damage_areas.update();
    if(state.invincibility.current>0){state.invincibility.advance(-1);if(state.state_timer.current!=state.state_timer.previous&&state.state_timer.current%3==0){motion.body.flags|=0x8000;motion.body.secondary_color=0xff0000ff;}else motion.body.flags&=~0x8000u;}else motion.body.flags&=~0x8000u;
    if(motion.body.update(animations)<0){last_error=3;return false;}update_bounds();state.state_timer.tick();state.transition_timer.tick();motion.state.transition_timer=state.transition_timer.current;
    const bool enabled=!input.movement.bomb&&input.movement.enemy_manager&&input.movement.enemies;
    if(enabled&&state.state_timer.current%60==0)economy.add_rank(1);
    auto& p=shots.player;motion.copy_shot_state(p);p.state=state.life_state;p.power=economy.power;p.power_step=economy.power_step;p.character=input.movement.character;p.subtype=input.movement.subtype;p.held=input.movement.held;p.bomb_active=input.movement.bomb;p.enemies_present=input.movement.enemies;p.special_active=input.special_active;
    if(enabled&&!input.shooting_blocked){if(!shots.fire())return false;}
    else{shots.schedule.timer.set(-1,&animations.rate);shots.locked_target=nullptr;shots.target_locked=0;}
    return shots.update();
}
PlayerCollision PlayerFrame::collision()const noexcept {PlayerCollision p;p.position={motion.state.position.x,motion.state.position.y};p.minimum={state.hit.minimum.x,state.hit.minimum.y};p.maximum={state.hit.maximum.x,state.hit.maximum.y};p.radius=resource.header.hitbox;p.state=state.life_state;p.invincibility_frames=state.invincibility.current;p.flags=motion.state.flags;p.bomb_active=input.movement.bomb;return p;}
void PlayerFrame::copy_item_state(ItemPlayer& p)const noexcept {p.position=motion.state.position;p.state=state.life_state;p.attraction_speed=resource.header.attraction_speed;auto rectangle=[](const PlayerBounds& b){return ItemBounds{b.minimum.x,b.minimum.y,b.maximum.x,b.maximum.y};};p.pickup=rectangle(state.pickup);p.focused_attract=rectangle(state.focused_attract);p.unfocused_attract=rectangle(state.unfocused_attract);p.focused=(input.movement.held&8)!=0;p.communication=economy.communication;p.power=economy.power;p.max_power=economy.max_power;}
}
