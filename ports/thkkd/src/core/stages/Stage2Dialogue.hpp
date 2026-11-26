// 恢复64727E..647E7C的二面三角色战前/战后脚本，公共绘制和输入沿用原顺序。
#pragma once
#include "core/stages/Stage1Dialogue.hpp"
#include "core/stages/Stage2DialogueConstants.hpp"
namespace kaikeidou {
namespace stage2_dialogue_detail {
using dialogue_detail::ScriptStep;
inline constexpr std::array<ScriptStep,15> reimu_before{{
    {0,1,3,0x6c88e8,0x6c88f8,0},{0xffffffffu,0,0,0,0,1},{1,2,3,0x6c890c,0,0},
    {0xffffffffu,2,2,0x6c8924,0x6c8950,0},{0xffffffffu,1,2,0x6c8964,0,2},
    {0xffffffffu,2,0,0x6c8984,0x6c89ac,0},{0xffffffffu,1,2,0x6c89c4,0x6c89e0,0},
    {0xffffffffu,1,7,0x6c8a04,0x6c8a30,0},{0xffffffffu,2,2,0x6c8a4c,0x6c8a6c,0},
    {0xffffffffu,1,3,0x6c8a88,0x6c8aa0,0},{0xffffffffu,2,0,0x6c8aac,0,0},
    {0xffffffffu,1,6,0x6c8ad4,0x6c8ae8,0},{0xffffffffu,2,2,0x6c8b0c,0,4},
    {0xffffffffu,2,2,0x6c8b2c,0,0},{0xffffffffu,0,0,0,0,8}
}};
inline constexpr std::array<ScriptStep,14> marisa_before{{
    {1,1,0,0x6c8b48,0x6c8b58,0},{0xffffffffu,0,0,0,0,1},{1,2,0,0x6c8b6c,0,2},
    {0xffffffffu,2,2,0x6c8b94,0,0},{0xffffffffu,1,3,0x6c8bb8,0x6c8bd8,0},
    {0xffffffffu,1,0,0x6c8bfc,0x6c8c20,0},{0xffffffffu,2,0,0x6c8c40,0,0},
    {0xffffffffu,2,2,0x6c8c4c,0,0},{0xffffffffu,1,5,0x6c8c64,0x6c8c88,0},
    {0xffffffffu,2,0,0x6c8cb4,0,0},{0xffffffffu,1,2,0x6c8cd4,0,0},
    {0xffffffffu,2,1,0x6c8d00,0,4},{0xffffffffu,2,2,0x6c8d24,0x6c8d3c,0},{0xffffffffu,0,0,0,0,8}
}};
inline constexpr std::array<ScriptStep,13> third_before{{
    {2,1,0,0x6c8d58,0x6c8d68,0},{0xffffffffu,0,0,0,0,1},{1,2,2,0x6c8d8c,0,0},
    {0xffffffffu,1,5,0x6c8da0,0,0},{0xffffffffu,1,5,0x6c8db0,0x6c8dc8,0},
    {0xffffffffu,1,3,0x6c8ddc,0,2},{0xffffffffu,2,1,0x6c8e08,0x6c8e18,0},
    {0xffffffffu,1,7,0x6c8e38,0,0},{0xffffffffu,2,1,0x6c8e58,0,0},
    {0xffffffffu,1,0,0x6c8e64,0,0},{0xffffffffu,2,3,0x6c8e74,0x6c8e9c,4},
    {0xffffffffu,2,2,0x6c8eb4,0x6c8ec8,0},{0xffffffffu,0,0,0,0,8}
}};
inline dialogue_detail::Lines script(DialogueState& state,DialogueContext& context,EnemyPool& enemies,Enemy& prefix,DialogueServices& services) {
    auto& f=state.fields;
    if(f.word(0)!=2&&f.word(0)!=3)return dialogue_detail::stage1_script(state,context,enemies,prefix,services);
    dialogue_detail::Lines text;ScriptStep step;const auto phase=f.word(8);bool selected=false;
    if(f.word(0)==2){
        const auto choose=[&](const auto& rows){if(phase>=1&&phase<=rows.size()){step=rows[phase-1];selected=true;}};
        if(context.character==0)choose(reimu_before);else if(context.character==1)choose(marisa_before);else if(context.character==2)choose(third_before);
    }else if(context.character<3){
        if(phase==1){constexpr std::array<ScriptStep,3> rows{{{0,1,2,0x6c8ee4,0,0},{1,1,2,0x6c8f08,0x6c8f2c,0},{2,1,0,0x6c8f40,0,0}}};step=rows[context.character];selected=true;}
        else if(phase==2){step.action=8;selected=true;}
    }
    if(!selected)return text;
    if(step.action==1){
        if(context.scene==0){
            f.word(0x0c,64);const auto index=services.allocate_boss(1);
            auto& enemy=index==-1?prefix:enemies.enemies.at(static_cast<std::size_t>(index));
            enemy.word(8,36);enemy.quad(0x38,dialogue_detail::constant(0x6d1df0));enemy.quad(0x40,dialogue_detail::constant(0x6d1848));
            enemy.quad(0xa8,dialogue_detail::constant(0x6d1a60));enemy.quad(0xb0,dialogue_detail::constant(0x6d1a90));enemy.word(0x80,0);
        }
        return text;
    }
    if(step.action==8){f.word(0x10,3);return text;}
    if(step.portrait!=0xffffffffu)f.word(step.speaker==1?0x14:0x20,step.portrait);
    f.word(0x10,step.speaker);f.word(step.speaker==1?0x2c:0x38,step.expression);
    text.first=line(step.text1);text.second=line(step.text2);
    if(step.action==2){state.word_a7e7fc=400;state.word_a78e30=0;}
    if(step.action==4&&f.word(4)==0)services.music(4,1);
    return text;
}
}
inline void update_stage2_dialogue(DialogueState& state,DialogueContext& context,EnemyPool& enemies,
                                  Enemy& enemy_prefix,DialogueServices& services,unsigned precision){
    update_stage1_dialogue(state,context,enemies,enemy_prefix,services,precision,stage2_dialogue_detail::script);
}
}
