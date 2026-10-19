#include "AttackController.hpp"
#include <algorithm>
namespace th09 {
bool AttackController::begin(i32 variant,i32 level,i32 parameter,const char* text){
    if(variant<0||variant>=9||level<0||level>2||!text||std::strlen(text)>=name.size())return false;
    auto& own=services.players[side];auto& other=services.players[1-side];const i32 display_level=other.levels[level<2?0:1];
    services.reward_time=std::max(wrapping_sub(services.reward_time,500),0);
    const i32 slot=level==2?1:0;
    if(slot){
        if(services.boss(side))return false;
        if(services.boss(1-side)){services.boss(1-side)->behavior_flags|=0x100;other.boss_counters=wrapping_add(other.boss_counters,1);}
        other.boss_count=wrapping_add(other.boss_count,1);
    }
    active_variants[slot]=-1;parameters[slot][0]=parameter;parameters[slot][1]=variant;parameters[slot][2]=level;notices[slot]=1;
    std::memcpy(name.data(),text,std::strlen(text)+1);
    if(!slot)other.spell_count=wrapping_add(other.spell_count,1);
    const i32 script=variant%3;
    // The original passes uninitialized stack coordinates here. Every original
    // attack script sets its position before its first simulation movement.
    services.spawn_attack_enemy(side,script,script==2?1400:30,script==1?10000:1000);
    active_variants[slot]=variant;
    if(other.levels[slot]<16)++other.levels[slot];if(slot&&own.levels[1]<16)++own.levels[1];
    services.play_sound(slot?53:14,0);
    for(u32 i=0;i<5;++i)services.start_animation(animations[i+2],i+30);
    services.set_sprite(animations[5],false,display_level/10+25);services.set_sprite(animations[6],false,display_level%10+25);services.set_sprite(animations[4],false,level+21);
    services.game_flags|=(1u<<u32(side+11));time.reset();
    services.background_transition(side,1,30);services.background_transition(1-side,2,30);
    const i32 portrait=other.portrait_script+(side==1?0:2);services.portrait(1-side,0,portrait);services.portrait(1-side,1,portrait+1);
    return true;
}
void AttackController::notify_pattern(u32 kind){
    if(kind<3){if(notices[0])notices[0]=2;}
    else if(kind==3&&notices[1]){notices[1]=2;services.boss_background(side);}
}
void AttackController::show_name(){
    services.start_animation(animations[0],side+18);services.set_sprite(animations[1],true,side+4);animations[1].scale={1,1};services.draw_text(animations[1],name.data(),0xfffffe,0);
}
void AttackController::update(){
    if(notices[0]>1){if(active_variants[1]<0&&active_variants[0]>=0)show_name();notices[0]=0;}
    if(notices[1]>1){if(active_variants[1]>=0)show_name();notices[1]=0;}
    if((services.game_flags&0x1800)&&time.current>30)services.game_flags&=~(1u<<u32(side+11));
    const auto finished=[&](i32 variant){return variant%3==2?services.boss(side)==nullptr:time.current>192;};
    if(active_variants[0]>=0&&finished(active_variants[0])){if(active_variants[1]<0)animations[0].flags&=~2u;active_variants[0]=-1;}
    if(active_variants[1]>=0&&finished(active_variants[1])){animations[0].flags&=~2u;active_variants[1]=-1;services.play_sound(15,0);services.reset_background(side);}
    if(services.advance_animation(animations[0]))animations[0].flags&=~2u;
    for(u32 i=2;i<animations.size();++i)if(services.advance_animation(animations[i]))animations[i].flags&=~2u;
    time.tick(services.timing);
}
void AttackController::draw(){
    services.begin_draw(side);auto& title=animations[0];auto& text=animations[1];
    if(title.flags&2){title.pos=services.geometry[side].to_screen(title.pos2);services.draw_animation(title);text.pos=title.pos;text.pos.y+=10;text.color1=title.color1;std::memcpy(&text.flags,&title.flags,sizeof(u32));services.draw_animation(text);}
    for(u32 i=2;i<animations.size();++i){auto& a=animations[i];if(a.flags&2){a.pos=services.geometry[side].to_screen(a.pos2);services.draw_animation(a);}}
}
}
