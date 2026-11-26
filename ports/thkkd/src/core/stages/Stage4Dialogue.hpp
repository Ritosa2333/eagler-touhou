// 原648991..649E38的四面战前战后脚本，原共享字段在服务调用前直接写回。
#pragma once
#include "core/stages/Stage3Dialogue.hpp"
#include "core/stages/Stage4DialogueConstants.hpp"
#include "core/entities/BossAllocation.hpp"
namespace kaikeidou { namespace stage4_dialogue_detail {
inline dialogue_detail::Lines script(DialogueState& state,DialogueContext& context,EnemyPool& enemies,
 Enemy& prefix,BossAllocationContext& allocation,DialogueServices& services){
    auto& fields=state.fields;
    if(fields.word(0)!=7&&fields.word(0)!=8)
        return stage3_dialogue_detail::script(state,context,enemies,prefix,services);
    dialogue_detail::Lines text;dialogue_detail::ScriptStep row;bool selected=false;
    const auto phase=fields.word(8);
    const auto choose=[&](const auto& rows){if(phase>=1&&phase<=rows.size()){row=rows[phase-1];selected=true;}};
    if(fields.word(0)==7){
        if(context.character==0)choose(reimu_before);else if(context.character==1)choose(marisa_before);else if(context.character==2)choose(third_before);
    }else{
        if(context.character==0)choose(reimu_after);else if(context.character==1)choose(marisa_after);else if(context.character==2)choose(third_after);
    }
    if(!selected)return text;
    if(row.action&1u){
        if(context.scene==0){
            fields.word(0x0c,128);const auto slot=services.allocate_boss(4);
            auto& enemy=slot==-1?prefix:enemies.enemies.at(static_cast<std::size_t>(slot));
            enemy.word(8,73);enemy.quad(0x38,boss_constant(0x6d1a60));enemy.quad(0x40,boss_constant(0x6d1c00));
            enemy.quad(0xa8,0);enemy.quad(0xb0,boss_constant(0x6b25b8));
            enemy.quad(0xb8,boss_constant(0x6d18c0));enemy.quad(0xc0,boss_constant(0x6d1ae8));
            allocation.field_a7ea5c=0;
        }
        return text;
    }
    if(row.action&8u){fields.word(0x10,3);return text;}
    if(row.portrait!=0xffffffffu)fields.word(row.speaker==1?0x14:0x20,row.portrait);
    fields.word(0x10,row.speaker);fields.word(row.speaker==1?0x2c:0x38,row.expression);
    text.first=line(row.text1);text.second=line(row.text2);
    if((row.action&4u)&&fields.word(4)==0)services.music(8,1);
    if(row.action&2u){state.word_a7e7fc=400;state.word_a78e30=0;}
    return text;
}
}
inline void update_stage4_dialogue(DialogueState& state,DialogueContext& context,EnemyPool& enemies,
 Enemy& prefix,BossAllocationContext& allocation,DialogueServices& services,unsigned precision){
    const auto script=[&](DialogueState& s,DialogueContext& c,EnemyPool& e,Enemy& p,DialogueServices& v){
        return stage4_dialogue_detail::script(s,c,e,p,allocation,v);
    };
    update_stage1_dialogue(state,context,enemies,prefix,services,precision,script);
}
}
