#include "SpellCard.hpp"
#include "Localization.hpp"
namespace th10 {
static AnmVm* child_script(AnmVm* parent,std::int16_t script){for(auto* node=&parent->child_node;node;node=node->next)if(node->value->script_index==script)return node->value;return nullptr;}
// 0x409280. Keep the title, script creation and saved-record updates in their
// original order. The spell circle has two children that receive its duration.
void SpellCard::start(i32 id,i32 name_id,const char* title,i32 frames,SpellEnvironment& env){
    if(!(elapsed_flags&1)){elapsed.rate=env.rate;elapsed_flags|=1;}elapsed.initialize(-1);number=id;
    std::memcpy(name,title,std::strlen(title)+1);spell_flags=(spell_flags&~0x18u)|3;
    if(*env.replay_mode!=1){auto& selected=env.record(id,false);std::memcpy(selected.name,title,std::strlen(title)+1);if(selected.attempts<99999)++selected.attempts;auto& combined=env.record(id,true);std::memcpy(combined.name,title,std::strlen(title)+1);if(combined.attempts<99999)++combined.attempts;}
    for(u32 i=0;i<8;++i)env.initialize_animation(bonus_digits[i],SpellAnimationFile::Interface,58+i,false);
    for(u32 i=0;i<5;++i)env.initialize_animation(record_digits[i],SpellAnimationFile::Interface,66+i,false);
    title_animations[0]=env.create_animation(SpellAnimationFile::Effects,1);
    title_animations[1]=env.create_animation(SpellAnimationFile::Text,72);
    title_animations[2]=env.create_animation(SpellAnimationFile::Effects,2);
    // Stored records keep the original Japanese name (like th08); only the
    // announcement display point looks the translated name up. The lookup key
    // is the raw ECL id, not the difficulty-adjusted record index, because
    // spells.etl is keyed by the former (thcrap's spell_id).
    env.draw_name(env.registry->find_and_clear(title_animations[1]),Localization::SpellName(u32(name_id),title));env.play_sound(14);
    circle_animation=env.create_animation(SpellAnimationFile::Bullets,417);circle_position=env.boss_position();env.registry->set_position(circle_animation,circle_position,true);
    child_script(env.registry->find_and_clear(circle_animation),415)->integer_variables[2]=frames;
    child_script(env.registry->find_and_clear(circle_animation),416)->integer_variables[2]=frames;duration=frames;
    const u32 value=(static_cast<u32>(env.game->stage)*3+10)*static_cast<u32>(env.game->item_value)*10;std::memcpy(&bonus,&value,4);initial_bonus=bonus;if(initial_bonus>=100000000)initial_bonus=99999999;
    env.create_animation(SpellAnimationFile::Bullets,427);
    i32 first,second,overlay;
    switch(env.game->stage){
    case 1:first=12;second=11;overlay=id<2?15:14;break;
    case 2:first=14;second=15;overlay=17;break;
    case 3:first=18;second=19;overlay=21;break;
    case 4:first=19;second=20;overlay=22;break;
    case 5:first=12;second=13;overlay=15;break;
    case 6:first=33;second=34;overlay=36;break;
    case 7:if(env.game->section<24){first=38;second=-1;overlay=40;}else{first=27;second=28;overlay=30;}break;
    default:return;
    }
    env.initialize_animation(backgrounds[0],SpellAnimationFile::Boss,first,true);
    if(second>=0)env.initialize_animation(backgrounds[1],SpellAnimationFile::Boss,second,true);
    env.create_animation(SpellAnimationFile::Boss,overlay);
}
}
