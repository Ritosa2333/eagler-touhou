#include "SpellController.hpp"
#include "EnemyFrame.hpp"
#include "AsciiText.hpp"
#include "PortTitle.hpp"
#include "ScoreScalarTh14.hpp"
#include <algorithm>
#include <cstdio>
#include <cstring>
namespace th11 {
void SpellController::invalidate_capture(){world.spell_flags|=8;}
SpellController::SpellController(EnemyFrameWorld& w,GameEconomy& e,SpellEffects& fx,SpellRecords& r,const float* speed)
    :world(w),economy(e),effects(fx),records(r),rate(speed){timer.set(0,rate);}
void SpellController::hide_circle(){world.spell_flags|=0x10;effects.spell_circle_end();}
void SpellController::title_orientation(i32 value){
    if constexpr(kPortTitle==14||kPortTitle==16)world.spell_flags=(world.spell_flags&~0x100u)|((u32(value)&1)<<8);
}
bool SpellController::begin(i32 id,i32 duration,const char* text,Vec3 boss,bool bomb){
    if(id<0||id>=(kPortTitle==16?119:175)||selection<0||selection>=(kPortTitle==16?4:6)||!text||std::strlen(text)>=name.size())return false;
    timer.set(0,rate);world.spell_elapsed=0;world.spell_id=id;name.fill(0);std::strcpy(name.data(),text);
#if PORT_TITLE == 14 || PORT_TITLE == 16
    world.spell_flags=(world.spell_flags&~0x98u)|3;
    if(!replay)for(i32 table:{selection,kPortTitle==16?4:6}){auto& r=records.entries[table][id];r.name=name;
        auto& attempts=(run_flags&0x30)==0x20?r.practice_attempts:r.attempts;
        if(attempts<99999)++attempts;}
    if(!effects.spell_timer_interrupt(2))return false;
    world.spell_flags&=~0x20u;if constexpr(kPortTitle==14){if(bomb)world.spell_flags|=0x20;}
#else
    world.spell_flags=(world.spell_flags&~0x18u)|3;
    if(!replay)for(i32 table:{selection,6}){auto& r=records.entries[table][id];r.name=name;if(r.attempts<99999)++r.attempts;}
    world.spell_flags&=~0x20u;if(bomb&&selection!=5)world.spell_flags|=0x20;
#endif
    world.spell_flags&=~0x40u;frame_count=1;
    timeout=duration;circle_position=boss;
#if PORT_TITLE == 16
    // Complete417f00 uses a difficulty table, multiplied by stage.
    if(economy.difficulty<0||economy.difficulty>4)return false;
    const i32 bonuses[5]={500000,1000000,1500000,2000000,1000000};
    world.spell_bonus=signed_bits(u32(stage)*u32(bonuses[economy.difficulty]));
    initial_bonus=std::min(world.spell_bonus,999999999);
#elif PORT_TITLE == 14
    world.spell_bonus=signed_bits(u32(stage)*3000000u+u32(economy.difficulty)*2000000u);
    initial_bonus=std::min(world.spell_bonus,999999999);
#else
    world.spell_bonus=signed_bits(u32(wrapping_add(economy.difficulty,stage))*1000000u);
    initial_bonus=std::min(world.spell_bonus,99999999);
#endif
    return effects.spell_begin_visuals(id,duration,name.data());
}
bool SpellController::update(float player_y,Vec3 boss,bool bomb){
    if(!(world.spell_flags&1))return true;
    frame_count=wrapping_add(frame_count,1);
    if(timer.current>=60&&(kPortTitle!=16||!(world.spell_flags&0x200)))effects.spell_background(false);
    if(timer.current>=300&&!(world.spell_flags&8)){
        const i32 denominator=wrapping_sub(timeout,300),numerator=wrapping_sub(initial_bonus,initial_bonus/((kPortTitle==14||kPortTitle==16)?3:10));
        if(!denominator||(denominator==-1&&numerator==INT32_MIN))return false;
        const i32 value=wrapping_sub(world.spell_bonus,numerator/denominator);world.spell_bonus=wrapping_sub(value,value%10);
    }
    if constexpr(kPortTitle!=14&&kPortTitle!=16){if(!effects.spell_update_visuals())return false;}
    timer.tick();world.spell_elapsed=timer.current;
    if(timer.current>=120){
        const bool inverted=(kPortTitle==14||kPortTitle==16)&&(world.spell_flags&0x100);
        const bool hide=inverted?player_y>352:player_y<96,show=inverted?player_y<320:player_y>128;
        if(!(world.spell_flags&4)&&hide){effects.spell_title_interrupt(3);world.spell_flags|=4;}
        else if((world.spell_flags&4)&&show){effects.spell_title_interrupt(2);world.spell_flags&=~4u;}
    }
    // 4982f8 stores the widened f32 coefficient, not the decimal double .05.
#if PORT_TITLE == 16
    auto smooth=[](float old,float target){return float(old+float(float(target-old)*.05f));};
#else
    auto smooth=[](float old,float target){const float delta=float(double(target)-old),step=float(double(delta)*double(.05f));return float(double(old)+step);};
#endif
    circle_position={smooth(circle_position.x,boss.x),smooth(circle_position.y,boss.y),smooth(circle_position.z,boss.z)};
    effects.spell_circle_position(circle_position);
    if((world.spell_flags&0x20)&&!bomb)world.spell_flags&=~0x20u;
    return true;
}
bool SpellController::end(){
    if(!(world.spell_flags&1))return true;
    effects.spell_background(true);effects.spell_title_interrupt(1);
#if PORT_TITLE == 14 || PORT_TITLE == 16
    world.spell_flags&=~1u;effects.spell_background_end();world.spell_flags&=~0x20u;
    if(!effects.spell_timer_interrupt(3))return false;
    effects.spell_circle_end();
    if(!(world.spell_flags&2)){
        if(!effects.spell_result(false,0))return false;
        return !(world.spell_flags&0x80)||effects.spell_sound(69);
    }
#if PORT_TITLE == 16
    effects.spell_award_score(world.spell_bonus,economy);
#else
    th14_spell_score_words([&](){return th14_hud_load(economy.score_units);},[&](){return world.spell_bonus;},
        [&](i32 value){th14_hud_store(economy.score_units,value);});
#endif
#else
    world.spell_flags&=~0x21u;effects.spell_circle_end();
    if(!(world.spell_flags&2))return effects.spell_result(false,0);
    economy.add_score(world.spell_bonus);
#endif
    if(!effects.spell_result(true,world.spell_bonus))return false;
    if(!replay){if(world.spell_id<0||world.spell_id>=(kPortTitle==16?119:175)||selection<0||selection>=(kPortTitle==16?4:6))return false;
        for(i32 table:{selection,kPortTitle==16?4:6}){auto& r=records.entries[table][world.spell_id];
#if PORT_TITLE == 14 || PORT_TITLE == 16
            auto& captures=(run_flags&0x30)==0x20?r.practice_captures:r.captures;
#else
            auto& captures=r.captures;
#endif
            if(captures<99999)++captures;}}
    if(!effects.spell_sound((kPortTitle==14||kPortTitle==16)?46:45))return false;
    return (kPortTitle!=14&&kPortTitle!=16)||!(world.spell_flags&0x80)||effects.spell_sound(69);
}
void SpellController::survival(){world.spell_flags|=8;}
bool SpellController::queue_text(AsciiText& output,u32 title_color)const{
    // 40c4a0: the title ANM supplies alpha; the ASCII atlas supplies digits
    // (and '$', the failed-bonus symbol), on the inner ASCII render pass.
    if(!(world.spell_flags&1))return true;
    if(selection<0||selection>=(kPortTitle==16?4:6)||world.spell_id<0||world.spell_id>=(kPortTitle==16?119:175))return false;
    AsciiStyle style;style.font=2;style.pass=kPortTitle==14?2:1;style.color=0xffffff|(title_color&0xff000000);
    char text[64];
    if(world.spell_flags&2){
        std::snprintf(text,sizeof(text),(kPortTitle==14||kPortTitle==16)?"%8d":"%.8d",world.spell_bonus);
        if(!output.add(text,{(kPortTitle==14||kPortTitle==16)?266.f:265.f,(kPortTitle==14||kPortTitle==16)?35.f:51.f,0},style))return false;
    }else if(!output.add("$",{kPortTitle==16?282.f:kPortTitle==14?282.f:279.f,(kPortTitle==14||kPortTitle==16)?35.f:51.f,0},style))return false;
    const auto& record=records.entries[selection][world.spell_id];
#if PORT_TITLE == 14 || PORT_TITLE == 16
    const i32 captures=(run_flags&0x30)==0x20?record.practice_captures:record.captures;
    const i32 attempts=(run_flags&0x30)==0x20?record.practice_attempts:record.attempts;
    if(captures>=100)std::snprintf(text,sizeof(text),"MASTER");
    else if(attempts>=100)std::snprintf(text,sizeof(text),"%.2d/99+",captures);
    else std::snprintf(text,sizeof(text),"%.2d/%.2d",captures,attempts);
    return output.add(text,{360,35,0},style);
#else
    std::snprintf(text,sizeof(text),"%.2d/%.2d",record.captures,record.attempts);
    return output.add(text,{360,51,0},style);
#endif
}
}
