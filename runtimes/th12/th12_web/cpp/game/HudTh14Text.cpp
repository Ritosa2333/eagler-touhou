#include "HudTh14Text.hpp"
#include "Th14LifeFragments.hpp"
#include "HudCounterWordsTh14.hpp"
#include "SpellTiming.hpp"
#include <cstdio>
#include <algorithm>
namespace th11 {
#if PORT_TITLE == 14
namespace {
void grouped(char* out,u32 value){
    if(value<1000)std::snprintf(out,64,"%u",value);
    else if(value<1000000)std::snprintf(out,64,"%u,%03u",value/1000,value%1000);
    else if(value<1000000000)std::snprintf(out,64,"%u,%03u,%03u",value/1000000,value/1000%1000,value%1000);
    else std::snprintf(out,64,"%u,%03u,%03u,%03u",value/1000000000,value/1000000%1000,value/1000%1000,value%1000);
}
void score_text(char* out,u32 value,i32 continues){
    if(value<100)std::snprintf(out,64,"%d",signed_bits(value*10u+u32(continues)));
    else if(value<100000)std::snprintf(out,64,"%u,%02u%d",value/100,value%100,continues);
    else if(value<100000000)std::snprintf(out,64,"%u,%03u,%02u%d",value/100000,value/100%1000,value%100,continues);
    else std::snprintf(out,64,"%u,%03u,%03u,%02u%d",value/100000000,value/100000%1000,value/100%1000,value%100,continues);
}
}
bool th14_hud_awards(AsciiText& output,const Th14HudAwards& data){
    // Retail431140..43144a: grouped result bonus and two spell-time rows.
    AsciiStyle style;style.font=4;style.pass=2;char text[64];
    if(data.result){
        grouped(text,u32(data.bonus));style.color=u32(data.result_alpha)<<24|0xffffff;
        style.horizontal=0;
        if(!output.add(text,{224,200,0},style))return false;
    }
    if(data.time){
        style.horizontal=1;
        const auto best=decode_spell_time(data.encoded);
        for(u32 row=0;row<2;++row){
            style.scale={1,1};style.color=u32(data.time_alpha)<<24|(row?0x808080:0xffffff);
            const i32 sec=row?best.seconds:std::min(data.frames/60,999),hund=row?best.hundredths:signed_bits(u32(data.frames%60)*100u)/60;
            std::snprintf(text,sizeof text,"%3d.",sec);if(!output.add(text,{224,row?224.f:208.f,0},style))return false;
            style.scale={.6f,.6f};std::snprintf(text,sizeof text,"%.2ds",hund);if(!output.add(text,{268,row?230.f:214.f,0},style))return false;
        }
    }return true;
}
bool th14_hud_counters(AsciiText& output,const GameEconomy& e,const HudScore& score,u8 alpha){
    // Retail 43144a..4318b6 and the grouping helpers 40bb60/40bc70.
    // Keep the native alignment until display scaling has rounded the origin.
    // AsciiText then performs the original per-character float subtraction.
    AsciiStyle style;style.font=4;style.color=u32(alpha)<<24|0x808080;char text[64];
    auto put=[&](float x,float y,bool right=false){style.horizontal=right?2:1;return output.add(text,{x,y,0},style);};
    return th14_hud_counter_words([&](HudCounterReadTh14 word)->i32{
        using R=HudCounterReadTh14;
        switch(word){
        case R::HighContinues:return th14_hud_load(score.high_continues);
        case R::High:return score.high_value();
        case R::Continues:return score.continues_value();
        case R::Displayed:return score.displayed_value();
        case R::AwardsWidth:case R::AwardsLife:case R::AwardsRequirement:case R::AwardsBomb:return th14_hud_load(e.th14_life_awards);
        case R::LifeFragments:return th14_hud_load(e.life_fragments);
        case R::BombFragments:return th14_hud_load(e.th14_bomb_fragments);
        case R::PowerInteger:case R::PowerFraction:return th14_hud_load(e.power);
        case R::StepInteger:case R::StepModulo:case R::StepFraction:case R::StepMaximum:return th14_hud_load(e.power_step);
        case R::MaxPower:return th14_hud_load(e.max_power);
        case R::PointValue:return th14_hud_load(e.point_value);
        case R::Graze:return th14_hud_load(e.graze);
        }return 0;
    },[&](HudCounterTextTh14 kind,i32 value,i32 suffix,float x){
        using T=HudCounterTextTh14;
        style.scale={.6f,.6f};style.color=u32(alpha)<<24|0xffffff;
        switch(kind){
        case T::High:case T::Score:
            style.scale={1,1};if(kind==T::High)style.color=u32(alpha)<<24|0x808080;
            score_text(text,u32(value),suffix);return put(x,kind==T::High?44.f:66.f,true);
        case T::Life:case T::Bombs:
            std::snprintf(text,sizeof text,"%3d",value);return put(x,kind==T::Life?122.f:160.f);
        case T::LifeRequirement:case T::BombRequirement:
            std::snprintf(text,sizeof text,"/%d",value);return put(x,kind==T::LifeRequirement?122.f:160.f);
        case T::LifeUnavailable:case T::BombUnavailable:
            std::snprintf(text,sizeof text,"---/---");return put(x,kind==T::LifeUnavailable?122.f:160.f);
        case T::PowerInteger:case T::PowerFraction:case T::PowerMaximum:case T::PowerZeros:
            style.color=u32(alpha)<<24|0xff8030;
            if(kind==T::PowerInteger||kind==T::PowerMaximum)style.scale={1,1};
            if(kind==T::PowerInteger)std::snprintf(text,sizeof text,"%d.",value);
            else if(kind==T::PowerFraction)std::snprintf(text,sizeof text,"%.2d",value);
            else if(kind==T::PowerMaximum)std::snprintf(text,sizeof text,"/%d.",value);
            else std::snprintf(text,sizeof text,"00");
            return put(x,kind==T::PowerInteger||kind==T::PowerMaximum?216.f:223.f);
        case T::Points:case T::Graze:
            style.scale={1,1};style.color=u32(alpha)<<24|(kind==T::Points?0x40c0ff:0x808080);
            grouped(text,u32(value));return put(x,kind==T::Points?238.f:260.f,true);
        }return false;
    });
}
#endif
}
