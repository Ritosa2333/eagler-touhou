// 原649E38..64B880的五面脚本；20..23在接触判定后保留原第三行的phase覆盖写。
#pragma once
#include "core/stages/Stage4Dialogue.hpp"
#include "core/stages/Stage5DialogueConstants.hpp"
#include "core/stages/Stage5BossTimelineSecondaryContact.hpp"
namespace kaikeidou { namespace stage5_dialogue_detail {
inline dialogue_detail::Lines script(DialogueState& state,DialogueContext& context,EnemyPool& enemies,Enemy& prefix,
 BossAllocationContext& allocation,ProjectilePools& projectiles,const ObjectRecord<240>& player,
 DialogueServices& services,unsigned precision){
    auto& fields=state.fields;const auto mode=fields.word(0),phase=fields.word(8);
    if(!(mode>=9&&mode<=11)&&!(mode>=20&&mode<=23))
        return stage4_dialogue_detail::script(state,context,enemies,prefix,allocation,services);
    dialogue_detail::Lines text;dialogue_detail::ScriptStep row;bool selected=false;
    const auto choose=[&](const auto& rows){if(phase>=1&&phase<=rows.size()){row=rows[phase-1];selected=true;}};
    if(mode>=20&&mode<=23){
        if(mode==20)choose(interlude20);else if(mode==21)choose(interlude21);else if(mode==22)choose(interlude22);else choose(interlude23);
        if(!selected)return text;
        fields.word(0x10,2);text.first=line(row.text1);text.second=line(row.text2);
        if(stage5_boss_secondary_contact(projectiles,player,precision)!=0){fields.word(0x10,3);fields.word(8,0);}
        if(phase==3)fields.word(8,2);
        return text;
    }
    if(mode==9){if(context.character==0)choose(mid_reimu);else if(context.character==1)choose(mid_marisa);else if(context.character==2)choose(mid_third);}
    else if(mode==10){if(context.character==0)choose(reimu_before);else if(context.character==1)choose(marisa_before);else if(context.character==2)choose(third_before);}
    else{if(context.character==0)choose(reimu_after);else if(context.character==1)choose(marisa_after);else if(context.character==2)choose(third_after);}
    if(!selected)return text;
    if(row.action&8u){fields.word(0x10,3);return text;}
    if(row.portrait!=0xffffffffu)fields.word(row.speaker==1?0x14:0x20,row.portrait);
    fields.word(0x10,row.speaker);fields.word(row.speaker==1?0x2c:0x38,row.expression);
    text.first=line(row.text1);text.second=line(row.text2);
    if((row.action&4u)&&fields.word(4)==0)services.music(10,1);
    if(row.action&2u){state.word_a7e7fc=400;state.word_a78e30=0;}
    return text;
}
}
inline void update_stage5_dialogue(DialogueState& state,DialogueContext& context,EnemyPool& enemies,Enemy& prefix,
 BossAllocationContext& allocation,ProjectilePools& projectiles,const ObjectRecord<240>& player,DialogueServices& services,unsigned precision){
    const auto script=[&](DialogueState& s,DialogueContext& c,EnemyPool& e,Enemy& p,DialogueServices& v){
        return stage5_dialogue_detail::script(s,c,e,p,allocation,projectiles,player,v,precision);
    };
    update_stage1_dialogue(state,context,enemies,prefix,services,precision,script);
}
}
