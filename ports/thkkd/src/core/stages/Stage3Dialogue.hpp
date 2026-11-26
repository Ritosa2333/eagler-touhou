// 原647F0B..648991的三面战前/战后脚本；公共段与模式4沿用已验收实现。
#pragma once
#include "core/stages/Stage3MidbossDialogue.hpp"
#include "core/stages/Stage3DialogueConstants.hpp"
namespace kaikeidou { namespace stage3_dialogue_detail {
inline dialogue_detail::Lines script(DialogueState& state,DialogueContext& context,EnemyPool& enemies,Enemy& prefix,DialogueServices& services){
    auto& fields=state.fields;
    if(fields.word(0)!=5&&fields.word(0)!=6)return stage3_midboss_dialogue_detail::script(state,context,enemies,prefix,services);
    dialogue_detail::Lines text;dialogue_detail::ScriptStep row;bool selected=false;
    const auto phase=fields.word(8);
    const auto choose=[&](const auto& rows){if(phase>=1&&phase<=rows.size()){row=rows[phase-1];selected=true;}};
    if(fields.word(0)==5){
        if(context.character==0)choose(reimu_before);else if(context.character==1)choose(marisa_before);else if(context.character==2)choose(third_before);
    }else{
        if(context.character==0)choose(reimu_after);else if(context.character==1)choose(marisa_after);else if(context.character==2)choose(third_after);
    }
    if(!selected)return text;
    if(row.action&8u){fields.word(0x10,3);return text;}
    if(row.portrait!=0xffffffffu)fields.word(row.speaker==1?0x14:0x20,row.portrait);
    fields.word(0x10,row.speaker);fields.word(row.speaker==1?0x2c:0x38,row.expression);
    text.first=line(row.text1);text.second=line(row.text2);
    if((row.action&4u)&&fields.word(4)==0)services.music(6,1);
    if(row.action&2u){state.word_a7e7fc=400;state.word_a78e30=0;}
    return text;
}
}
inline void update_stage3_dialogue(DialogueState& state,DialogueContext& context,EnemyPool& enemies,Enemy& prefix,DialogueServices& services,unsigned precision){
    update_stage1_dialogue(state,context,enemies,prefix,services,precision,stage3_dialogue_detail::script);
}
}
