#include "SpellSystem.hpp"
#include "EclNative.hpp"
#include "AsciiManager.hpp"
#include "GuiState.hpp"
#include "Localization.hpp"
namespace th08 {
namespace {
void interpolate(AnmVm& vm,u32 kind,i32 duration,u8 mode){vm.interpCurrentTimers[kind].set(0);vm.interpEndTimers[kind].set(duration);vm.interpModes[kind]=mode;}
void decode(char* to,const u8* from,u32 size,u8 key){for(u32 i=0;i<size;i++)to[i]=char(from[i]^key);}
void redraw(EclGlobals& g){if(g.gui)g.gui->flags.lives=g.gui->flags.bombs=3;}
}
SpellSystem::SpellSystem(EclGlobals& g,GameGlobals& n,GameValues& v,HighScore& h,SpellRecord* r,EffectSystem& e,BackgroundState& b,AnmExecutor& a,SpellPresentation& p,PlayerBombState& pb,PracticeState& practice,SpellSystemActions& sa)
    :globals(g),numbers(n),values(v),high_score(h),records(r),effects(e),background(b),anm(a),presentation(p),bomb(pb),practice(practice),actions(sa){globals.spell_actions=this;}
bool SpellSystem::reward(bool point_value){
    auto& s=globals;s.spell_reward_effect=nullptr;actions.spell_bonus(signed_bits(s.spell_pending_bonus));values.add_score(signed_bits(s.spell_pending_bonus));s.spell_flags&=~256u;
    if(s.spell_time_items>0){values.add_time_orbs(s.spell_time_items);if(point_value)numbers.point_value=wrapping_add(numbers.point_value,signed_bits(u32(s.spell_time_items)*10u));s.spell_time_items=0;}return true;
}
bool SpellSystem::begin(EclVm& enemy,const EclInstruction& ins){
    if(ins.size<0xf4)return false;const auto* p=reinterpret_cast<const u8*>(&ins);i16 portrait;u16 number;u32 bonus;std::memcpy(&portrait,p+12,2);std::memcpy(&number,p+14,2);std::memcpy(&bonus,p+16,4);
    return begin(enemy,number,portrait,bonus,p+0x14,p+0x44,p+0x74,p+0xb4);
}
bool SpellSystem::begin(EclVm& enemy,u32 number,i32 portrait,u32 bonus,const u8* name,const u8* owner,const u8* comment1,const u8* comment2){
    if(number>=spell_count||globals.shot<0||globals.shot>=12||(!records&&!(globals.game_flags&8))||enemy.timeout/60==0)return false;
    auto& s=globals;s.spell_flags=(s.spell_flags|5u)&~(512u|16u|8u|64u|128u|1024u);
    s.spell_number=number;s.spell_enemy=&enemy;s.spell_enemy_index=u32(enemy.pool_index);s.spell_bonus=bonus;s.spell_capture_bonus=(enemy.flags&0x8000000)?99999990:signed_bits(bonus);
    s.spell_bonus_decay=(bonus-bonus/7)/u32(enemy.timeout/60);s.spell_remaining.set(enemy.timeout);s.spell_initial.set(enemy.timeout);decode(s.spell_name,name,48,0xaa);
    if(!std::memchr(s.spell_name,0,48))return false;
    presentation.context.game_flags=s.game_flags;presentation.context.current_spell=s.current_spell;
    // Storage keeps the decoded Japanese name (CATK records, replays); only the announcement display point looks up spells.etl.
    if(!presentation.enemy(portrait,Localization::SpellName(u32(number),s.spell_name),0)||!actions.clear_projectiles(1))return false;
    background.spell_state=1;background.spell_frames=0;
    if(background.spell_vm_count<0||background.spell_vm_count>32)return false;
    for(i32 i=0;i<background.spell_vm_count;i++){
        auto* file=effects.state.stage_animation;const i32 script=wrapping_add(background.dialogue_state,i);
        if(!file||!file->scripts||u32(script)>=file->scriptCount)return false;auto& vm=background.spell_vms[i];vm.anmFile=file;vm.scriptIndex=i16(script);anm.start(*file,vm,file->scripts[script]);
    }
    background.callback=nullptr;enemy.rank_speed_low=-.5f;enemy.rank_speed_high=.5f;enemy.rank_count_low=enemy.rank_count_high=enemy.rank_layers_low=enemy.rank_layers_high=0;s.spell_panel_color=0x80808080;
    if((s.spell_flags&256)&&!reward())return false;s.spell_flags&=~2048u;
    auto* effect=effects.fixed((s.game_flags&0x180)?52:39,enemy.position,1,0xffffffff);s.spell_effect=effect;if(effects.invalid||!effect)return false;
    interpolate(*effect,AnmInterp_Pos,100,6);effect->posInitial.x=8;effect->posFinal.x=256;effect->posInitial.y=64;effect->posFinal.y=0;effect->pos.y=64;
    effect->position=enemy.position;effect->segments=64;effect->angle=0;effect->radius=256;effect->width=15;effect->frequency=6;
    s.spell_flags=(s.spell_flags&~32u)|((s.game_flags>>7&1)<<5);redraw(s);s.spell_flags&=~64u;
    if(!(s.game_flags&8)){
        auto& record=records[number];std::memcpy(record.name,s.spell_name,std::strlen(s.spell_name)+1);
        char decoded[48];decode(decoded,owner,48,0xbb);const auto* end=static_cast<const char*>(std::memchr(decoded,0,48));if(!end)return false;std::memcpy(record.owner,decoded,end-decoded+1);
        const bool practice=s.game_flags&0x4000;if(practice){std::memcpy(s.spell_comment1,comment1,64);std::memcpy(s.spell_comment2,comment2,64);}
        encounter_spell(record,u32(s.shot),practice,u8(s.difficulty));
    }
    return !anm.invalid;
}
void SpellSystem::add_bonus(i32 amount){
    auto& s=globals;if(s.spell_flags&2048)return;s.spell_bonus+=u32(amount);
    if(s.spell_bonus>=u32(s.spell_capture_bonus))s.spell_bonus=u32(s.spell_capture_bonus);else s.spell_bonus_decay+=u32(amount/120);
}
bool SpellSystem::end(){
    auto& s=globals;if(s.ascii)s.ascii->blindness_color=0;
    if(s.spell_flags&1){
        bool captured=false;s.spell_flags&=~1u;presentation.end_enemy();
        if(!(s.spell_flags&8)){
            i32 score=0;if(!actions.cancel_projectiles(8000,true,score)||!actions.cancel_enemies(8000,score))return false;
            if(score){values.add_score(score);actions.bonus(score);}
            if(s.spell_flags&4){
                if(!s.spell_enemy||s.spell_number>=spell_count||s.shot<0||s.shot>=12||(!records&&!(s.game_flags&8)))return false;
                s.spell_pending_bonus=s.spell_bonus;
                if(s.spell_enemy->flags&0x8000000)s.spell_time_items=700;
                else {const i32 threshold=wrapping_sub(s.spell_initial.current,s.spell_initial.current/7),remaining=s.spell_remaining.current;
                    if(remaining>=threshold)s.spell_time_items=1000;else if(remaining<180)s.spell_time_items=100;else{const i32 divisor=wrapping_sub(threshold,180);if(!divisor)return false;s.spell_time_items=wrapping_add(signed_bits(u32(wrapping_sub(remaining,180))*900u)/divisor,100);}
                }
                s.spell_flags|=512;
                if(!(s.game_flags&8)){
                    auto& record=records[s.spell_number];const bool practice=s.game_flags&0x4000;
                    if(practice){decode(record.comment1,reinterpret_cast<const u8*>(s.spell_comment1),64,0xdd);decode(record.comment2,reinterpret_cast<const u8*>(s.spell_comment2),64,0xee);}
                    capture_spell(record,u32(s.shot),practice,u8(s.difficulty),s.spell_bonus);++high_score.spell_counters[s.spell_number];
                }
                numbers.captured_spells=wrapping_add(numbers.captured_spells,1);
                static constexpr u16 last_spells[]{10,11,12,29,30,31,51,52,53,74,75,76,97,98,99,116,117,118,143,144,145,146,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,204};
                if(std::binary_search(std::begin(last_spells),std::end(last_spells),u16(s.spell_number)))++practice.tracker_last_spell_captures;
                captured=true;values.update_integrity();
            }
        }
        if(auto* effect=s.spell_effect){
            if(captured){
                s.spell_flags|=256;if(s.game_flags&0x180)s.game_flags=(s.game_flags&~0x180u)|0x100;
                interpolate(*effect,AnmInterp_Pos,30,6);effect->posInitial.x=effect->radius;effect->posFinal.x=256;effect->posInitial.y=effect->height;effect->posFinal.y=0;effect->pos.x=effect->radius;effect->pos.y=effect->height;
                interpolate(*effect,AnmInterp_RGB1,60,3);effect->color1Initial=effect->color1;effect->color1Final.d3dColor=0x20d080a0;effect->frequency=6;effect->age.set(0);
                s.spell_reward_effect=effect;s.spell_effect=nullptr;actions.sound(35,0);
            }else{if(s.game_flags&0x180)s.game_flags&=~0x180u;effect->active=0;s.spell_effect=nullptr;actions.popup(0,(s.spell_flags&32)?6:5);}
        }
        bomb.cooldown=16;if(s.player_state==0){s.player_state_timer.set(70);s.player_state=3;}redraw(s);actions.sound(15,0);background.spell_state=0;
    }
    if(s.spell_enemy)s.spell_enemy->flags&=~0x8000000u;s.spell_enemy=nullptr;s.spell_flags&=~2048u;return true;
}
}
