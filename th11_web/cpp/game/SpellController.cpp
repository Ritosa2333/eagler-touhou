#include "SpellController.hpp"
#include "EnemyFrame.hpp"
#include "AsciiText.hpp"
#include <algorithm>
#include <cstdio>
#include <cstring>
namespace th11 {
SpellController::SpellController(EnemyFrameWorld& w,GameEconomy& e,SpellEffects& fx,SpellRecords& r,const float* speed)
    :world(w),economy(e),effects(fx),records(r),rate(speed){timer.set(0,rate);}
void SpellController::hide_circle(){world.spell_flags|=0x10;effects.spell_circle_end();}
bool SpellController::begin(i32 id,i32 duration,const char* text,Vec3 boss,bool bomb){
    if(id<0||id>=175||selection<0||selection>=6||!text||std::strlen(text)>=name.size())return false;
    timer.set(0,rate);world.spell_elapsed=0;world.spell_id=id;name.fill(0);std::strcpy(name.data(),text);
    world.spell_flags=(world.spell_flags&~0x18u)|3;
    if(!replay)for(i32 table:{selection,6}){auto& r=records.entries[table][id];r.name=name;if(r.attempts<99999)++r.attempts;}
    world.spell_flags&=~0x20u;if(bomb&&selection!=5)world.spell_flags|=0x20;
    world.spell_flags&=~0x40u;frame_count=1;
    timeout=duration;circle_position=boss;
    world.spell_bonus=signed_bits(u32(wrapping_add(economy.difficulty,stage))*1000000u);
    initial_bonus=std::min(world.spell_bonus,99999999);
    return effects.spell_begin_visuals(id,duration,name.data());
}
bool SpellController::update(float player_y,Vec3 boss,bool bomb){
    if(!(world.spell_flags&1))return true;
    frame_count=wrapping_add(frame_count,1);
    if(timer.current>=60)effects.spell_background(false);
    if(timer.current>=300&&!(world.spell_flags&8)){
        const i32 denominator=wrapping_sub(timeout,300),numerator=wrapping_sub(initial_bonus,initial_bonus/10);
        if(!denominator||(denominator==-1&&numerator==INT32_MIN))return false;
        const i32 value=wrapping_sub(world.spell_bonus,numerator/denominator);world.spell_bonus=wrapping_sub(value,value%10);
    }
    if(!effects.spell_update_visuals())return false;
#ifdef TH_ENABLE_THPRAC
    if(!practice_time_lock)
#endif
    timer.tick();world.spell_elapsed=timer.current;
    if(timer.current>=120){
        if(!(world.spell_flags&4)&&player_y<96){effects.spell_title_interrupt(3);world.spell_flags|=4;}
        else if((world.spell_flags&4)&&player_y>128){effects.spell_title_interrupt(2);world.spell_flags&=~4u;}
    }
    // 4982f8 stores the widened f32 coefficient, not the decimal double .05.
    auto smooth=[](float old,float target){const float delta=float(double(target)-old),step=float(double(delta)*double(.05f));return float(double(old)+step);};
    circle_position={smooth(circle_position.x,boss.x),smooth(circle_position.y,boss.y),smooth(circle_position.z,boss.z)};
    effects.spell_circle_position(circle_position);
    if((world.spell_flags&0x20)&&!bomb)world.spell_flags&=~0x20u;
    return true;
}
bool SpellController::end(){
    if(!(world.spell_flags&1))return true;
    effects.spell_background(true);effects.spell_title_interrupt(1);world.spell_flags&=~0x21u;effects.spell_circle_end();
    if(!(world.spell_flags&2))return effects.spell_result(false,0);
    economy.add_score(world.spell_bonus);
    if(!effects.spell_result(true,world.spell_bonus))return false;
    if(!replay){if(world.spell_id<0||world.spell_id>=175||selection<0||selection>=6)return false;
        for(i32 table:{selection,6}){auto& r=records.entries[table][world.spell_id];if(r.captures<99999)++r.captures;}}
    return effects.spell_sound(45);
}
void SpellController::survival(){world.spell_flags|=8;}
bool SpellController::queue_text(AsciiText& output,u32 title_color)const{
    // 40c4a0: the title ANM supplies alpha; the ASCII atlas supplies digits
    // (and '$', the failed-bonus symbol), on the inner ASCII render pass.
    if(!(world.spell_flags&1))return true;
    if(selection<0||selection>=6||world.spell_id<0||world.spell_id>=175)return false;
    AsciiStyle style;style.font=2;style.pass=1;style.color=0xffffff|(title_color&0xff000000);
    char text[64];
    if(world.spell_flags&2){
        std::snprintf(text,sizeof(text),"%.8d",world.spell_bonus);
        if(!output.add(text,{265,51,0},style))return false;
    }else if(!output.add("$",{279,51,0},style))return false;
    const auto& record=records.entries[selection][world.spell_id];
    std::snprintf(text,sizeof(text),"%.2d/%.2d",record.captures,record.attempts);
    return output.add(text,{360,51,0},style);
}
}
