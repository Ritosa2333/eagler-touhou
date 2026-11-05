#include "TitleClear.hpp"
#include "TitleReplays.hpp"
#include "TitleScores.hpp"
#include "HighRefresh.hpp"
namespace th10 {
namespace {float displayed_elapsed(const Timer& t){if(!high_refresh::active||t.current<=0||t.current-t.previous>2)return t.fractional;return high_refresh::lerp(float(t.previous),t.fractional);}}
void TitleClear::set_stage(i32 stage){auto& env=environment;env.game->stage=env.game->reserved_040=stage;*env.current_stage=env.stages+stage;}
void TitleClear::initialize_name(){
    auto& t=title;auto& env=environment;t.replay_menu.select(0);t.replay_menu.item_count=std::strlen(env.alphabet);t.replay_menu.wrap=1;
    std::strcpy(t.name,(*env.scores)->settings.last_name);if(std::memcmp(t.name,"        ",9))t.replay_menu.move(-1);
    i32 length=8;while(length>0&&t.name[length-1]==' ')--length;t.name_length=length;
}
void TitleClear::move_keyboard(){
    auto& keyboard=title.replay_menu;auto& env=environment;keyboard.reserved=keyboard.selected;
    if((*env.pressed|*env.repeated)&0x10)keyboard.move(-13);if((*env.pressed|*env.repeated)&0x20)keyboard.move(13);
    if((*env.pressed|*env.repeated)&0x40)keyboard.move(keyboard.selected%13==0?12:-1);
    if((*env.pressed|*env.repeated)&0x80)keyboard.move(keyboard.selected%13==12?-12:1);
    if(keyboard.reserved!=keyboard.selected)env.sound(12);
}
bool TitleClear::enter_character(bool for_replay){
    auto& t=title;auto& env=environment;const i32 length=std::strlen(env.alphabet),selected=t.replay_menu.selected;
    if(selected<length-2){
        const char value=selected==length-3?' ':env.alphabet[selected];
        if(t.name_length<8){t.name[t.name_length++]=value;if(t.name_length>=8)t.replay_menu.select(length-1);}else t.name[t.name_length-1]=value;
    }else if(selected==length-2){if(!t.name_length)return true;t.name[--t.name_length]=' ';}
    else if(selected==length-1){
        if(for_replay){
            env.sound(44);char filename[12];replay_slot_filename(filename,t.menu.selected+1);env.delete_replay(t.previews[t.menu.selected]);env.save_replay(filename,t.name);t.previews[t.menu.selected]=env.preview(filename);
            std::strcpy((*env.scores)->settings.last_name,t.name);t.set_phase(2,env.rate);
        }else{
            const auto& game=*env.game;std::strcpy((*env.scores)->characters[game.character*3+game.shot_type].high_scores[game.difficulty][t.menu.selected].name,t.name);
            std::strcpy((*env.scores)->settings.last_name,t.name);t.set_phase(3,env.rate);
        }
    }
    env.sound(10);return false;
}
// 0x432cb0. Clear-score registration temporarily uses the stage-8 marker,
// then returns to stage zero before the name editor becomes interactive.
i32 TitleClear::update_rank(){
    auto& t=title;auto& env=environment;auto& game=*env.game;
    switch(t.phase){
    case 0:{
        t.menu.item_count=30;env.play_music(true);
        if(!env.registry->find(t.animation_ids[94])){t.create_script(94,env);t.animation_ids[195]=env.create(**env.effects,8);}
        t.create_script(104,env);t.set_phase(1,env.rate);t.create_script(152+game.character,env);t.create_script(154+game.character*3+game.shot_type,env);t.create_script(160+game.difficulty,env);
        set_stage(8);const auto rank=insert_high_score((*env.scores)->characters[game.character*3+game.shot_type],*env.ranking);set_stage(0);
        if(rank<0){t.menu.select(-1);t.no_rank=1;}else{t.reset_timer(env.rate);t.menu.wrap=1;t.menu.select(rank);initialize_name();t.no_rank=0;}
        [[fallthrough]];}
    case 1:if(t.elapsed.current>6)t.set_phase(2,env.rate);break;
    case 2:
        if(!t.no_rank)move_keyboard();
        if(*env.pressed&0x1001){if(t.no_rank){t.set_phase(3,env.rate);env.sound(10);}else if(enter_character(false))return 1;}
        if(*env.pressed&10){if(t.no_rank){t.set_phase(3,env.rate);env.sound(10);}else if(t.name_length){env.sound(11);t.name[--t.name_length]=' ';}}break;
    case 3:
        if(t.elapsed.current>=6){t.dismiss_script(104,env);t.dismiss_script(152+game.character,env);t.dismiss_script(154+game.character*3+game.shot_type,env);t.dismiss_script(160+game.difficulty,env);t.set_screen(16,env.rate);}break;
    }
    return 1;
}
// 0x433570. Saving returns to the slot list. Cancel from an empty name also
// returns there, while cancel from a nonempty name erases one character.
i32 TitleClear::update_save(){
    auto& t=title;auto& env=environment;auto& menu=t.menu;
    switch(t.phase){
    case 0:
        menu.item_count=25;menu.wrap=1;menu.select(0);set_stage(8);
        for(i32 slot=0;slot<25;++slot){char filename[12];replay_slot_filename(filename,slot+1);t.previews[slot]=env.preview(filename);}
        t.create_script(105,env);t.set_phase(1,env.rate);[[fallthrough]];
    case 1:if(t.elapsed.current>6)t.set_phase(2,env.rate);break;
    case 2:
        menu.reserved=menu.selected;if((*env.pressed|*env.repeated)&0x10)menu.move(-1);if((*env.pressed|*env.repeated)&0x20)menu.move(1);if(menu.reserved!=menu.selected)env.sound(12);
        if(*env.pressed&10){t.set_phase(4,env.rate);env.sound(11);}
        else if(*env.pressed&0x1001){
            t.replay_index=menu.selected;t.replay_menu.select(0);t.replay_menu.item_count=std::strlen(env.alphabet);t.replay_menu.wrap=1;
            auto* replay=*env.replay;env.timestamp(replay->info->timestamp);replay->info->last_stage=8;
            std::strcpy(t.name,(*env.scores)->settings.last_name);t.name_length=0;if(std::memcmp(t.name,"        ",9))t.replay_menu.move(-1);
            i32 length=8;while(length>0&&t.name[length-1]==' ')--length;t.name_length=length;env.sound(10);t.set_phase(3,env.rate);
        }break;
    case 3:
        move_keyboard();if((*env.pressed&0x1001)&&enter_character(true))return 1;
        if(*env.pressed&10){if(t.name_length){env.sound(11);t.name[--t.name_length]=' ';}else t.set_phase(2,env.rate);}break;
    case 4:
        if(t.elapsed.current>=6){
            t.dismiss_script(105,env);t.signal_script(90,8,env);t.signal_script(91,8,env);t.dismiss_script(94,env);env.registry->interrupt(t.animation_ids[195],1);t.set_screen(2,env.rate);menu.pop();
            if(*env.replay)env.delete_replay(*env.replay);env.play_music(false);for(i32 slot=0;slot<25;++slot)if(t.previews[slot])env.delete_replay(t.previews[slot]);std::memset(t.previews,0,sizeof(t.previews));
        }break;
    }
    return 1;
}
static void draw_name(const TitleMenu& t,Vec3 position,ResultsDrawEnvironment& env){
    const i32 length=std::strlen(env.alphabet);*env.color=0xffffffff;env.print(position,"%s",{reinterpret_cast<uintptr_t>(t.name)});
    position.x=(Extended::from_int(static_cast<i32>(static_cast<u32>(t.name_length)*9u))+number(position.x)).to_float();if(t.name_length==8)position.x=Scalar::sub(position.x,9.0f);
    *env.color=0xffffff00;env.print(position,"_");*env.color=0xffffffff;position={212,360,0};
    for(i32 i=0;i<length;++i){*env.color=t.replay_menu.selected==i?0xffffff00:0xff808080;const i32 glyph=i<length-3?static_cast<std::int8_t>(env.alphabet[i]):i==length-3?129:i==length-2?127:128;env.print(position,"%c",{static_cast<u32>(glyph)});
        if(i%13==12){position.x=212;position.y=Scalar::add(position.y,16.0f);}else position.x=Scalar::add(position.x,18.0f);}
    *env.color=0xffffffff;
}
// 0x433230 leaves text mode enabled for the following title draw callbacks.
i32 draw_title_clear_rank(const TitleMenu& t,ResultsDrawEnvironment& env){
    if(t.phase!=2)return 1;*env.text_mode=1;const i32 difficulty=env.game->difficulty;Vec3 position{48,160,0};
    for(i32 row=0;row<10;++row){
        const u32 shade=255-row*16;*env.color=t.no_rank?0xff0000ff|(shade<<16)|(shade<<8):t.menu.selected==row?0xffffffff:0xff404040;
        draw_score_entry(env.game->character*3+env.game->shot_type,difficulty,row,position,env);position.y=Scalar::add(position.y,18.0f);
    }
    if(!t.no_rank){position={84,(Extended::from_int(t.menu.selected)*number(18.0f)+number(160.0f)).to_float(),0};draw_name(t,position,env);}return 1;
}
i32 draw_title_clear_save(const TitleMenu& t,ResultsDrawEnvironment& env,const char* const* difficulties){
    if(t.phase==2)return draw_title_replays(t,env,difficulties);
    if(t.phase!=3)return 1;
    Vec3 position{58,240,0};
    if(t.elapsed.current<10)position.y=((Extended::from_int(static_cast<i32>(static_cast<u32>(t.replay_index)*15u+80u))-number(240.0f))*(number(10.0f)-number(displayed_elapsed(t.elapsed)))*number(.1f)+number(240.0f)).to_float();
    ReplayInfo display=*(*env.replay)->info;std::memcpy(display.name,"        ",9);display.last_stage=8;
    draw_replay_description(display,t.replay_index+1,position,env,difficulties);
    if(t.elapsed.current>=10)draw_name(t,{112,240,0},env);
    *env.color=0xffffffff;*env.text_mode=0;return 1;
}
}
