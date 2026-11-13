#include "ShotFiles.hpp"
#include "PlayerFrame.hpp"
#include "AnmRenderer.hpp"
#include "Replay.hpp"
#include "PowerAddWordsTh14.hpp"
#include "HudScalarTh14.hpp"
#if PORT_TITLE == 16
#include "PlayerTh16Life.hpp"
#endif
#include <algorithm>
#include <cmath>
namespace th11 {
namespace {
PlayerBounds bounds(Vec3 p,Vec3 r,double scale=1){return{{float(double(p.x)-double(r.x)*scale),float(double(p.y)-double(r.y)*scale),float(double(p.z)-double(r.z)*scale)},{float(double(p.x)+double(r.x)*scale),float(double(p.y)+double(r.y)*scale),float(double(p.z)+double(r.z)*scale)}};}
Vec3 screen(Vec3 p){return{float(double(p.x)+224),float(double(p.y)+16),p.z};}
}
#if defined(PORT_TITLE) && PORT_TITLE == 14
bool PlayerTh14Scale::request(float value,const float* rate)noexcept{
    factor=1;recall=0;
    if(value<=1.f){flags&=~0x12u;return false;}
    flags|=0x12;auxiliary=0;interpolation.start[0]=1;interpolation.end[0]=value;
    interpolation.initial_tangent[0]=interpolation.final_tangent[0]=0;
    interpolation.duration=120;interpolation.mode=InterpolationMode::Linear;
    interpolation.timer.set(0,rate);return true;
}
float PlayerTh14Scale::update(i32 frame,const float* rate)noexcept{
    if(!(flags&0x10))return 1;
    auto& v=interpolation;
    if(v.duration){
        auto& t=v.timer;const float speed=t.rate?th14_rate_load(t.rate):1.f;t.previous=t.current;
        if(speed>.99f&&speed<1.01f){t.current=wrapping_add(t.current,1);t.fractional+=1.f;}
        else{t.fractional+=speed;t.current=truncate_int(t.fractional);}
        if(t.current>=v.duration){t.set(v.duration,rate);v.duration=0;factor=v.end[0];}
        else{const float f=t.fractional/float(v.duration);factor=(v.end[0]-v.start[0])*f+v.start[0];}
    }
    return v.duration&&frame%3==0?1.f:factor;
}
bool PlayerFrame::set_th14_scale(float value){
    const bool enabled=th14_scale.request(value,&animations.rate);motion.recall_options(enabled);
    return enabled||rebuild_options();
}
void player_th14_scale_bounds(PlayerFrameState& state,Vec3 p,float factor)noexcept{
    const auto scaled=[&](Vec3 half,bool divide){if(divide){half.x*=.5f;half.y*=.5f;half.z*=.5f;}half.x*=factor;half.y*=factor;half.z*=factor;
        return PlayerBounds{{p.x-half.x,p.y-half.y,p.z-half.z},{p.x+half.x,p.y+half.y,p.z+half.z}};};
    state.hit=scaled(state.hit_half,false);state.pickup=scaled(state.pickup_size,true);state.focused_attract=scaled(state.focused_half,false);state.unfocused_attract=scaled(state.pickup_size,false);
}
void player_th14_blink(AnmVm& body,const PlayerFrameState& state)noexcept{
    // 44e16f chooses the modern ANM color mode, before its VM update.
    body.th13_native_flags&=~0x60000u;
    if(state.invincibility.current>0&&state.state_timer.current!=state.state_timer.previous&&state.state_timer.current%3==0){
        body.th13_native_flags|=0x20000;body.secondary_color=0xff0000ff;
    }
}
bool player_th14_tick_clocks(PlayerFrameState& state,const PlayerFrameInput& input,u32 flags)noexcept{
    // 44e8d5..44ea46 advances all three clocks before deciding to fire.
    state.state_timer.tick();state.transition_timer.tick();state.th14_shooting_age.tick();
    return !input.movement.bomb&&input.movement.enemy_manager&&input.movement.enemies&&
        !input.shooting_blocked&&state.th14_shooting_age.current>=20&&!(flags&0x14);
}
bool player_th14_death_drops(GameEconomy& economy,Vec3 p,PlayerFrameWorld& world){
    // Retail 44dda3..44de94: lose half a power step, then emit seven
    // ordinary small-power items. Each angle operation is an SSE float.
    th14_death_power_words([&](DeathPowerAccessTh14 access){
        return access==DeathPowerAccessTh14::Step?th14_hud_load(economy.power_step):th14_hud_load(economy.power);
    },[&](DeathPowerAccessTh14,i32 value){th14_hud_store(economy.power,value);});
    const float target_y=p.y-224.f,dy=target_y-p.y,dx=0.f-p.x;
    const float angle=dx==0&&dy==0?1.5707963705062866f:float(std::atan2(double(dy),double(dx)));
    for(i32 i=0;i<7;++i){
        const float offset=float(i)*3.1415927410125732f/28.f;
        const float a=(offset+angle)-0.39269909262657166f;
        if(!world.spawn_item(1,p,0xffffff,a,3.f))return false;
    }
    return true;
}
#endif
PlayerFrame::PlayerFrame(ShtResource& s,AnmResource& p,AnmResource& b,AnmManager& a,GameEconomy& e,PlayerFrameWorld& w,u16 pf,u16 bf):motion(p,b,a,*this,pf,bf),shots(s,p,a,w,pf),resource(s),bullet_resource(b),animations(a),economy(e),world(w),bullet_file(bf){state.state_timer.set(0,&a.rate);state.transition_timer.set(0,&a.rate);state.invincibility.set(0,&a.rate);}
bool PlayerFrame::initialize(){
    const i32 character=input.movement.character;
    if(!player_selection_valid(character,input.movement.subtype)){last_error=4;return false;}
    auto& s=motion.state;auto& header=resource.header;
    motion.body.rectangle_columns=motion.body.rectangle_rows=16;
    if(!motion.reset_body())return false;
    s.position.x=0;s.position.y=400;s.x=0;s.y=0xc800;motion.set_speeds(header);
    economy.max_power=header.max_power_level*header.power_step;economy.power_step=header.power_step;
#if PORT_TITLE == 14 || PORT_TITLE == 16
    // Retail44c600 / 441152 overwrites the raw SHT step (40) with100 before deriving
    // the two live economy globals. Group indexing also reads that same step.
    header.power_step=100;economy.power_step=100;economy.max_power=header.max_power_level*100;
#endif
    shots.schedule.timer.set(-1,&animations.rate);
#if PORT_TITLE == 14
    // 44c681 copies these three own tables into the selected SHT header.
    constexpr float hitbox[]={2.f,3.f,3.f},attraction[]={5.f,6.f,5.f};
    header.hitbox=hitbox[character];header.attraction_diameter=60.f;header.attraction_speed=attraction[character];
    const float focus=50.f;
    state.th14_shooting_age.set(0,&animations.rate);
#else
    header.hitbox=character?3.5f:2.f;header.attraction_diameter=character?70.f:60.f;header.attraction_speed=character?7.f:5.f;
    const float focus=character?59.f:50.f;
#endif
    const float half=header.hitbox*.5f,pickup=header.attraction_diameter*.5f;
    state.hit_half={half,half,5};state.pickup_size={pickup,pickup,5};state.focused_half={focus,focus,5};
    state.hit=bounds(s.position,state.hit_half);state.pickup=bounds(s.position,state.pickup_size);
    state.focused_attract=bounds(s.position,state.focused_half);state.unfocused_attract=state.focused_attract;
    state.state_timer.set(0,&animations.rate);state.invincibility.set(kPortTitle==14?0:120,&animations.rate);
    s.option_count=0;s.option_lerp=30;return true;
}
bool PlayerFrame::rebuild_options(){
#if PORT_TITLE == 16
    if(!th16_season_shots||!th16_season_animation){last_error=116;return false;}
    shots.th16_season_resource=th16_season_shots;shots.th16_season_animation=th16_season_animation;shots.th16_season=&economy.th16_season;
    const bool ok=motion.rebuild_options_th16(resource.header,th16_season_shots->header,*th16_season_animation,economy,input.movement.character,economy.th16_subseason);
#else
    const bool ok=motion.rebuild_options(resource.header,economy,i32(shot_file_slot(u32(input.movement.character),u32(input.movement.subtype))));
#endif
    if(!ok)last_error=100+motion.last_error;return ok;
}
bool PlayerFrame::restore_replay_position(const ReplayStageState& entry){
    entry.restore_position(motion.state);for(auto& option:motion.options)option.snap=1;
#if PORT_TITLE == 14
    // 455fc5 restores position and455fda the weapon field;456039 then calls
    // 44f970 unconditionally, preserving the option count. The control callback
    // performs its separate post-ECL/HUD refresh later at4368aa/436a9a.
    return rebuild_options();
#else
    // 436da0 recreates the non-default Marisa/B formation, twice. The second
    // call also refreshes max-power auxiliary effects; do not remove it.
    if(entry.weapon){motion.state.option_count=0;return rebuild_options()&&rebuild_options();}
    return true;
#endif
}
bool PlayerFrame::start_stage(){
    // Reset the stage clocks while preserving replay position and weapon mode.
    state.life_state=1;shots.schedule.timer.set(-1,&animations.rate);
    state.state_timer.set(0,&animations.rate);state.transition_timer.set(0,&animations.rate);
    motion.state.transition_timer=0;
#if PORT_TITLE == 14
    shots.start_stage_th14();
    // 44cc10 also resets +6b4 and all 33 movement-history entries.
    state.th14_shooting_age.set(0,&animations.rate);
    for(auto& p:motion.state.th14_history)p={motion.state.x,motion.state.y};
    th14_scale.flags&=~0xfu;th14_scale.interpolation.duration=0;th14_scale.factor=1;th14_scale.recall=0;
    motion.state.th14_movement_factor=1;
    auto& p=shots.player;p.th14_cached_power=p.th14_attached_count=0;
    p.th14_focused_hits=p.th14_plain_hits=p.th14_hits_184bc=0;
#endif
    motion.state.flags&=~7u;motion.state.warp=motion.state.warp_timer=0;
    motion.clear_focus();
    if(!world.display_lives(economy.lives,kPortTitle==14?economy.life_fragments:i16(economy.life_fragments))){last_error=92;return false;}
    motion.recall_options(false);
    if(!rebuild_options()){if(last_error<100)last_error=93;return false;}
    return true;
}
bool PlayerFrame::draw(AnmRenderer& renderer){
    if(state.life_state==2)return true;
#if PORT_TITLE == 16
    // Retail443730 copies world coordinates and sets origin1 before468490.
    motion.body.position=motion.state.position;
    motion.body.flags2=(motion.body.flags2&~0x80000u)|0x40000;
#else
    if constexpr(kPortTitle==14){
        // Retail 44ec10/44ec70 copies playfield coordinates verbatim and
        // selects origin 1. The ANM renderer applies the (320,16) origin.
        motion.body.position=motion.state.position;
        motion.body.flags2=(motion.body.flags2&~0x40000u)|0x20000u;
    }else motion.body.position=screen(motion.state.position);
#endif
    if(renderer.draw(motion.body)==-2){last_error=5;return false;}
    // No shipped option constructor installs the optional native draw callback.
    if constexpr(kPortTitle!=16)for(const auto& option:motion.options)if(option.reservede0){last_error=6;return false;}
    return true;
}
bool PlayerFrame::cancel_all(bool spawning){return world.cancel_bullets(nullptr,0,spawning)&&world.cancel_lasers(nullptr,0,!spawning,spawning);}
void PlayerFrame::fail_spell(){if(!(spell.flags&1))return;if(spell.elapsed>=60){spell.bonus=0;spell.flags&=~0x22u;}else if(input.special_active
#if PORT_TITLE != 14
    &&input.movement.character*2+input.movement.subtype!=5
#endif
    )spell.flags|=0x20;}
bool PlayerFrame::hit(){
#if PORT_TITLE == 16
    struct Services:Th16PlayerHitServices{
        PlayerFrameWorld& world;PlayerMotion& motion;Services(PlayerFrameWorld& w,PlayerMotion& m):world(w),motion(m){}
        bool sound(i32 id)override{return world.player_sound(id);}
        bool effect(Vec3 p,i32 script)override{return world.player_effect(p,script);}
        bool reset_body()override{return motion.reset_body();}
    } services(world,motion);
    return player_th16_hit(state,spell,motion.state.position,motion.state.flags|(input.hit_sound?0u:8u),input.special_active,&animations.rate,services);
#elif PORT_TITLE == 14
    if(input.hit_sound&&!(th14_scale.flags&8)&&!world.player_sound(2)){last_error=2;return false;}
    if(!world.player_effect(motion.state.position,28)){last_error=1;return false;}
    fail_spell();state.state_timer.set(0,&animations.rate);state.life_state=4;
#else
    state.life_state=4;const Vec3 p=screen(motion.state.position);
    for(i32 i=0;i<33;++i){auto* a=animations.create(bullet_resource,i?77:76,bullet_file,0);if(!a){last_error=1;return false;}a->position=p;}
    state.state_timer.set(0,&animations.rate);if(input.hit_sound&&!world.player_sound(4)){last_error=2;return false;}
#endif
    state.invincibility.set(6,&animations.rate);if(!motion.reset_body())return false;
#if PORT_TITLE != 14 && PORT_TITLE != 16
    fail_spell();
#endif
    return true;
}
bool PlayerFrame::die(){
#if PORT_TITLE == 16
    struct Services:Th16PlayerDeathServices{
        PlayerFrameWorld& world;PlayerMotion& motion;AnmManager& animations;Services(PlayerFrameWorld& w,PlayerMotion& m,AnmManager& a):world(w),motion(m),animations(a){}
        bool effect(Vec3 p,i32)override{return world.th16_player_death_effect(p);}
        bool bombs(i32 count,i32 fragments)override{return world.bombs_changed(count,fragments);}
        bool lives(i32 count,i32 fragments)override{return world.display_lives(count,fragments);}
        bool reset_body()override{return motion.reset_body();}
        bool interrupt(u32 id,i16 label)override{auto* root=animations.find(id);if(root){root->interrupt(label);if(!root->child.previous)for(auto* n=root->child.next;n;n=n->next)n->value->interrupt(label);}return true;}
        bool enemy_death()override{return world.enemy_death();}
    } services(world,motion,animations);
    return player_th16_die(state,spell,economy,motion.options.data(),motion.state.option_count,motion.state.position,input.special_active,&animations.rate,services);
#elif PORT_TITLE == 14
    if(!world.player_effect(motion.state.position,27))return false;
#else
    economy.point_value=std::max(wrapping_add(economy.point_value,-10000000),state.minimum_point_value);economy.communication=0;
#endif
    economy.lives=wrapping_add(economy.lives,-1);
#if PORT_TITLE == 14
    if(economy.lives>=0&&!world.display_lives(economy.lives,economy.life_fragments))return false;
#else
    if(economy.lives>=0&&!world.display_lives(economy.lives,i16(economy.life_fragments)))return false;
#endif
    state.life_state=2;state.state_timer.set(0,&animations.rate);state.invincibility.set(180,&animations.rate);
    if(!motion.reset_body())return false;motion.clear_options();
#if PORT_TITLE == 14
    fail_spell();return world.enemy_death();
#else
    if(!input.replay&&!world.record_death())return false;
    fail_spell();economy.add_rank(-1024);return true;
#endif
}
bool PlayerFrame::start_special(){
    if(!world.start_bomb()){last_error=74;return false;}
    input.special_active=true;
#if PORT_TITLE == 14
    // Retail 412110 consumes Bomb stock, preserving power and fragments.
    economy.th14_bombs=std::clamp(wrapping_sub(economy.th14_bombs,1),0,8);
    if(!world.bombs_changed(economy.th14_bombs,economy.th14_bomb_fragments)){last_error=78;return false;}
#else
    economy.power=wrapping_sub(economy.power,economy.power_step);
    if(!rebuild_options()){last_error=75;return false;}
#endif
    return true;
}
bool PlayerFrame::active_frame(){
#if PORT_TITLE == 14
    const bool available=!input.movement.bomb&&input.special_available&&!input.special_active&&economy.th14_bombs>0;
#else
    const bool available=!input.movement.bomb&&input.special_available&&!input.special_active&&economy.power_step&&economy.power/economy.power_step;
#endif
    const u32 bomb_trigger=kPortTitle==14?input.movement.pressed:input.movement.held;
    if(available&&(bomb_trigger&2)&&!start_special()&&last_error!=74)return false;
#if PORT_TITLE != 14
    if(state.state_timer.current<30&&!cancel_all(true)){last_error=76;return false;}
#endif
#if PORT_TITLE == 14
    motion.state.th14_scale_factor=th14_scale.factor;motion.state.th14_scale_flags=th14_scale.flags;
#endif
    motion.state.transition_timer=state.transition_timer.current;return motion.update(input.movement);
}
bool PlayerFrame::death_frame(){
    economy.communication=0;
    if(state.state_timer.current==3){
#if PORT_TITLE == 16
        if(!player_th16_death_drops(economy,motion.state.position,world))return false;
#elif defined(PORT_TITLE) && PORT_TITLE == 14
        if(!player_th14_death_drops(economy,motion.state.position,world))return false;
#else
        const i32 power=economy.power,step=economy.power_step;economy.power=std::max(wrapping_sub(power,economy.max_power),0);
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
#endif
        if(!rebuild_options())return false;
    }
    if(state.state_timer.current>=30){
        if(economy.lives<0){if(!world.game_over(input.replay))return false;}
        else{state.life_state=0;animations.rate=1;
#if PORT_TITLE == 14
        if(!shots.th14_death_area(motion.state.position))return false;
        economy.th14_bombs=3;
        if(!world.bombs_changed(3,economy.th14_bomb_fragments))return false;
#elif PORT_TITLE == 16
        if(!shots.th16_sources.circle(motion.state.position,32.f,16.f,30,150))return false;
        economy.th16_bombs=3;if(!world.bombs_changed(3,economy.th16_bomb_fragments))return false;
#else
        shots.damage_areas.circle(motion.state.position,32,16,30,150,&animations.rate);
#endif
        state.death_position=motion.state.position;motion.state.position.x=0;motion.state.position.y=480;motion.state.x=0;motion.state.y=0xf000;state.invincibility.set(280,&animations.rate);state.state_timer.set(0,&animations.rate);}
    }return true;
}
void PlayerFrame::update_bounds(){const auto p=motion.state.position;
#if PORT_TITLE == 14
    if(th14_scale.flags&0x10){
        player_th14_scale_bounds(state,p,th14_scale.factor);return;
    }
#endif
    state.hit=bounds(p,state.hit_half);state.pickup=bounds(p,state.pickup_size,.5);state.focused_attract=bounds(p,state.focused_half);state.unfocused_attract=bounds(p,state.pickup_size);}
bool PlayerFrame::update(){
    switch(state.life_state){
    case 0:{
#if PORT_TITLE == 16
        if(!player_th16_respawn_frame(state,motion.state,motion.options.data(),world))return false;
#else
        economy.communication=0;auto& s=motion.state;s.y=wrapping_sub(0xf000,signed_bits(u32(state.state_timer.current)*0x2800)/60);s.position.y=float(double(s.y)/128);for(auto& o:motion.options)o.snap=1;s.warp=s.warp_timer=0;
#if PORT_TITLE == 14
        if(state.state_timer.current<30){
            const float r=(float(state.state_timer.current)*512.f)/30.f+64.f;
            if(!world.cancel_lasers(&state.death_position,r,false,true)||!world.cancel_lasers(&state.death_position,r*.25f,false,false))return false;
        }else if(!world.cancel_bullets(&s.position,640.f,false)||!world.cancel_lasers(nullptr,0,false,true))return false;
#else
        if(state.state_timer.current<30){const float r=float(double(state.state_timer.current)*512/30+64),small=float(double(r)*.25);if(!world.cancel_bullets(&state.death_position,r,true)||!world.cancel_bullets(&state.death_position,small,false)||!world.cancel_lasers(&state.death_position,r,false,true)||!world.cancel_lasers(&state.death_position,small,false,false))return false;}
        else if(!cancel_all(true))return false;
#endif
#endif
        if(state.state_timer.current>=60){state.life_state=1;state.state_timer.set(0,&animations.rate);if(!active_frame())return false;}break;
    }
    case 1:if(!active_frame())return false;break;
    case 2:if(!death_frame())return false;break;
    case 3:
#if PORT_TITLE == 14 || PORT_TITLE == 16
        if(state.state_timer.current==15&&!world.cancel_lasers(nullptr,0,true,false))return false;
#else
        if(state.state_timer.current==15&&!cancel_all(false))return false;
#endif
        break;
    case 4:
        economy.communication=0;
        if(state.state_timer.current>7){
            if(!die())return false;
#if PORT_TITLE != 14 && PORT_TITLE != 16
            if(!world.enemy_death())return false;
#endif
            if(!death_frame())return false;
        }else{
#if PORT_TITLE == 14
            const bool available=!input.movement.bomb&&input.special_available&&!input.special_active&&economy.th14_bombs>0;
#else
            const bool available=input.special_available&&!input.special_active&&economy.power_step&&economy.power/economy.power_step;
#endif
            const u32 bomb_trigger=kPortTitle==14?input.movement.pressed:input.movement.held;
            if(available&&(bomb_trigger&2)){
                if(start_special()){state.state_timer.set(60,&animations.rate);state.life_state=1;}
                else if(last_error!=74)return false;
            }
        }break;
    default:break;
    }
#if PORT_TITLE == 14
    motion.state.th14_movement_factor=1;
    motion.state.th14_bomb_shift={};
    shots.update_th14_areas();
#elif PORT_TITLE == 16
    if(!shots.th16_sources.update(&animations.rate)){last_error=119;return false;}
#else
    shots.damage_areas.update();
#endif
#if PORT_TITLE == 14
    // The branch is selected before decrementing; a remaining value of1 still
    // blinks on this frame after it reaches0 (44e0fa..44e1ac).
    player_th14_blink(motion.body,state);
    if(state.invincibility.current>0)state.invincibility.advance(-1);
#else
    if(state.invincibility.current>0){state.invincibility.advance(-1);if(state.state_timer.current!=state.state_timer.previous&&state.state_timer.current%3==0){motion.body.flags|=0x8000;motion.body.secondary_color=0xff0000ff;}else motion.body.flags&=~0x8000u;}else motion.body.flags&=~0x8000u;
#endif
    if(motion.body.update(animations)<0){last_error=3;return false;}
#if PORT_TITLE == 14
    if(th14_scale.flags&0x10){const float visual=th14_scale.update(state.state_timer.current,&animations.rate);motion.body.scale={visual,visual};motion.body.flags|=8;}
#endif
    update_bounds();
#if PORT_TITLE == 14
    const bool may_fire=player_th14_tick_clocks(state,input,th14_scale.flags);
#else
    state.state_timer.tick();state.transition_timer.tick();
#endif
    motion.state.transition_timer=state.transition_timer.current;
    const bool enabled=!input.movement.bomb&&input.movement.enemy_manager&&input.movement.enemies;
#if PORT_TITLE != 14
    if(enabled&&state.state_timer.current%60==0)economy.add_rank(1);
#endif
    auto& p=shots.player;motion.copy_shot_state(p);p.state=state.life_state;p.power=economy.power;p.power_step=economy.power_step;p.character=input.movement.character;p.subtype=input.movement.subtype;p.held=input.movement.held;p.bomb_active=input.movement.bomb;p.enemies_present=input.movement.enemies;p.special_active=input.special_active;
#if PORT_TITLE == 16
    p.th16_enemy_manager=input.movement.enemy_manager;
    p.th16_dialogue=input.shooting_blocked;p.th16_season_count=motion.th16_season_count;
#endif
#if PORT_TITLE == 14
    p.th14_shooting_blocked=input.shooting_blocked||input.movement.bomb;
    p.th14_dialogue_blocked=input.shooting_blocked;p.th14_enemy_manager=input.movement.enemy_manager;
#endif
#if PORT_TITLE == 14
    if(may_fire){if(!shots.fire())return false;}
#else
    if(enabled&&!input.shooting_blocked){if(!shots.fire())return false;}
#endif
    else{shots.schedule.timer.set(-1,&animations.rate);shots.locked_target=nullptr;shots.target_locked=0;
#if PORT_TITLE == 16
        shots.th16_schedule.regular.set(-1,&animations.rate);
#endif
    }
    return shots.update();
}
PlayerCollision PlayerFrame::collision()const noexcept {PlayerCollision p;p.position={motion.state.position.x,motion.state.position.y};p.minimum={state.hit.minimum.x,state.hit.minimum.y};p.maximum={state.hit.maximum.x,state.hit.maximum.y};p.radius=resource.header.hitbox;p.state=state.life_state;p.invincibility_frames=state.invincibility.current;p.flags=motion.state.flags;p.bomb_active=input.movement.bomb;
#if PORT_TITLE == 14
    if(th14_scale.flags&0x10)p.radius=(th14_scale.factor*3.5999999046325684f)*p.radius;
#endif
    return p;}
void PlayerFrame::copy_item_state(ItemPlayer& p)const noexcept {p.position=motion.state.position;p.state=state.life_state;p.attraction_speed=resource.header.attraction_speed;auto rectangle=[](const PlayerBounds& b){return ItemBounds{b.minimum.x,b.minimum.y,b.maximum.x,b.maximum.y};};p.pickup=rectangle(state.pickup);p.focused_attract=rectangle(state.focused_attract);p.unfocused_attract=rectangle(state.unfocused_attract);p.focused=(input.movement.held&8)!=0;p.communication=economy.communication;p.power=economy.power;p.max_power=economy.max_power;}
}
