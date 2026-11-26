// 原647E7C..647F0B的三面中Boss对话，公共绘制与输入沿用既有实现。
#pragma once
#include "core/stages/Stage2Dialogue.hpp"
namespace kaikeidou { namespace stage3_midboss_dialogue_detail {
inline constexpr std::array<unsigned char,4> text0{{0xe0,0xc5,0xa1,0xad}};
inline constexpr std::array<unsigned char,12> text1{{0xba,0xa3,0xc9,0xcf,0xb5,0xc4,0xd4,0xe5,0xd0,0xbc,0xa3,0xbf}};
inline dialogue_detail::Lines script(DialogueState& state,DialogueContext& context,EnemyPool& enemies,Enemy& prefix,DialogueServices& services){
    auto& f=state.fields;
    if(f.word(0)!=4)return stage2_dialogue_detail::script(state,context,enemies,prefix,services);
    dialogue_detail::Lines text;
    switch(f.word(8)){
    case 1:f.word(0x20,2);f.word(0x10,2);f.word(0x38,0);text.first={reinterpret_cast<const char*>(text0.data()),text0.size()};break;
    case 2:f.word(0x10,2);f.word(0x38,0);text.first={reinterpret_cast<const char*>(text1.data()),text1.size()};break;
    case 3:f.word(0x10,3);break;
    default:break;
    }return text;
}
}
inline void update_stage3_midboss_dialogue(DialogueState& state,DialogueContext& context,EnemyPool& enemies,Enemy& prefix,DialogueServices& services,unsigned precision){
    update_stage1_dialogue(state,context,enemies,prefix,services,precision,stage3_midboss_dialogue_detail::script);
}
}
