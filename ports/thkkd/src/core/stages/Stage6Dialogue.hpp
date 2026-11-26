// 原64B880..64CB60六面战前对话；第三行分配Boss并按原顺序写敌14。
#pragma once
#include "core/stages/Stage5Dialogue.hpp"
#include "core/stages/Stage6DialogueConstants.hpp"
namespace kaikeidou { namespace stage6_dialogue_detail {
inline dialogue_detail::Lines script(DialogueState& state,DialogueContext& context,EnemyPool& enemies,Enemy& prefix,
 BossAllocationContext& allocation,ProjectilePools& projectiles,const ObjectRecord<240>& player,DialogueServices& services,unsigned precision){
    auto& fields=state.fields;
    if(fields.word(0)!=12)return stage5_dialogue_detail::script(state,context,enemies,prefix,allocation,projectiles,player,services,precision);
    dialogue_detail::Lines text;dialogue_detail::ScriptStep row;bool selected=false;const auto phase=fields.word(8);
    const auto choose=[&](const auto& rows){if(phase>=1&&phase<=rows.size()){row=rows[phase-1];selected=true;}};
    if(context.character==0)choose(reimu_before);else if(context.character==1)choose(marisa_before);else if(context.character==2)choose(third_before);
    if(!selected)return text;
    if(row.action&1u){
        if(context.scene==0){
            fields.word(0x0c,128);const auto slot=services.allocate_boss(6);
            auto& enemy=slot==-1?prefix:enemies.enemies.at(static_cast<std::size_t>(slot));
            enemy.word(8,14);enemy.quad(0x38,boss_constant(0x6d1a60));enemy.quad(0x40,boss_constant(0x6d1b58));
            enemy.quad(0xa8,boss_constant(0x6d1a60));enemy.quad(0xb0,boss_constant(0x6d1d00));
        }
        return text;
    }
    if(row.action&8u){fields.word(0x10,3);return text;}
    if(row.portrait!=0xffffffffu)fields.word(row.speaker==1?0x14:0x20,row.portrait);
    fields.word(0x10,row.speaker);fields.word(row.speaker==1?0x2c:0x38,row.expression);
    text.first=line(row.text1);text.second=line(row.text2);
    if((row.action&4u)&&fields.word(4)==0)services.music(12,1);
    if(row.action&2u){state.word_a7e7fc=400;state.word_a78e30=0;}
    return text;
}
}
inline void update_stage6_dialogue(DialogueState& state,DialogueContext& context,EnemyPool& enemies,Enemy& prefix,
 BossAllocationContext& allocation,ProjectilePools& projectiles,const ObjectRecord<240>& player,DialogueServices& services,unsigned precision){
    const auto script=[&](DialogueState& s,DialogueContext& c,EnemyPool& e,Enemy& p,DialogueServices& v){return stage6_dialogue_detail::script(s,c,e,p,allocation,projectiles,player,v,precision);};
    update_stage1_dialogue(state,context,enemies,prefix,services,precision,script);
}
}
