#include "TitleSelection.hpp"
namespace th10 {
void TitleSelection::move(u32 previous,u32 next,i32 script){
    auto& m=title.menu;auto& env=environment;m.reserved=m.selected;
    if((*env.pressed|*env.repeated)&previous)m.move(-1);
    if((*env.pressed|*env.repeated)&next)m.move(1);
    if(m.reserved!=m.selected){env.sound(12);if(script>=0){env.interrupt_immediately(title.animation_ids[script],3);title.signal_script(script,static_cast<u16>(m.selected+7),env);}}
}
void TitleSelection::select_child(i32 parent,i32 child,i32 label){auto& env=environment;env.registry->interrupt(env.registry->find_child(title.animation_ids[parent],child),static_cast<std::int16_t>(label));}
void TitleSelection::stage_configuration(i32 stage){auto& env=environment;env.game->stage=stage;env.game->reserved_040=stage;*env.current_stage=env.stage_table+stage;}
// 0x430320. Extra has a single difficulty entry and restores the previous
// difficulty when the player returns to the main menu.
i32 TitleSelection::difficulty(){
    auto& t=title;auto& m=t.menu;auto& env=environment;auto& game=*env.game;const i32 script=game.difficulty>=4?120:119;
    switch(t.phase){
    case 0:
        if(!env.registry->find(t.animation_ids[94])){t.create_script(94,env);t.animation_ids[195]=env.create(**env.effects,8);}
        m.item_count=game.difficulty>=4?1:4;t.dismiss_script(script,env);t.create_script(script,env);env.interrupt_immediately(t.animation_ids[script],3);t.signal_script(script,static_cast<u16>(m.selected+17),env);t.create_script(98,env);t.set_phase(1,env.rate);[[fallthrough]];
    case 1:if(t.elapsed.current>6)t.set_phase(2,env.rate);break;
    case 2:
        if(game.difficulty<4)move(0x10,0x20,script);
        if(*env.pressed&10){t.set_phase(4,env.rate);env.sound(11);t.dismiss_script(script,env);return 1;}
        if(*env.pressed&0x1001){t.signal_script(script,6,env);select_child(script,game.difficulty<4?m.selected+109:113,2);t.set_phase(3,env.rate);env.sound(10);}break;
    case 3:
        if(t.elapsed.current>=14){t.dismiss_script(98,env);t.set_screen(7,env.rate);if(game.difficulty<4)game.difficulty=m.selected;m.push();m.item_count=2;m.select(game.character);}break;
    case 4:
        if(t.elapsed.current>=6){t.dismiss_script(98,env);t.signal_script(90,8,env);t.signal_script(91,8,env);t.dismiss_script(94,env);env.registry->interrupt(t.animation_ids[195],1);t.set_screen(2,env.rate);game.difficulty=game.difficulty<4?m.selected:t.saved_difficulty;m.pop();}break;
    }return 1;
}
// 0x4306a0. The two characters retain independent Extra unlocks.
i32 TitleSelection::character(){
    auto& t=title;auto& m=t.menu;auto& env=environment;auto& game=*env.game;
    switch(t.phase){
    case 0:
        m.item_count=2;
        if(game.difficulty==4)for(i32 character=0;character<2;++character){const auto* unlocked=env.extra_unlocked+character*3;if(!unlocked[0]&&!unlocked[1]&&!unlocked[2]){if(m.selected==character)m.select(1-character);m.disabled_items[m.disabled_count++]=character;}}
        t.create_script(99,env);t.dismiss_script(125,env);t.create_script(125,env);env.interrupt_immediately(t.animation_ids[125],3);t.signal_script(125,static_cast<u16>(m.selected+17),env);t.set_phase(1,env.rate);[[fallthrough]];
    case 1:if(t.elapsed.current>6)t.set_phase(2,env.rate);break;
    case 2:
        move(0x40,0x80,125);
        if(*env.pressed&10){t.set_phase(4,env.rate);env.sound(11);return 1;}
        if(*env.pressed&0x1001){select_child(125,m.selected+121,6);select_child(125,m.selected+123,6);select_child(125,122-m.selected,1);select_child(125,124-m.selected,1);env.sound(10);t.set_phase(3,env.rate);}break;
    case 3:
        if(t.elapsed.current>=14){t.dismiss_script(99,env);t.set_screen(8,env.rate);const auto old=game.character,selected=m.selected;game.character=selected;m.push();m.item_count=3;m.select(old==selected?game.shot_type:0);}break;
    case 4:
        if(t.elapsed.current>=6){t.dismiss_script(125,env);t.dismiss_script(99,env);t.set_screen(6,env.rate);game.character=m.selected;m.pop();}break;
    }return 1;
}
// 0x430a60. Shot confirmation either opens practice selection at frame 10 or
// starts the original 32-frame fade and requests gameplay at frame 40.
i32 TitleSelection::shot(){
    auto& t=title;auto& m=t.menu;auto& env=environment;auto& game=*env.game;
    switch(t.phase){
    case 0:
        m.item_count=3;
        if(game.difficulty==4){const auto* unlocked=env.extra_unlocked+game.character*3;for(i32 shot=0;shot<3;++shot)if(!unlocked[shot]){if(m.selected==shot)m.select(shot==0?(unlocked[1]?1:2):shot==1?(unlocked[0]?0:2):(unlocked[0]?0:1));m.disabled_items[m.disabled_count++]=shot;}}
        t.create_script(100,env);t.dismiss_script(150+game.character,env);t.create_script(150+game.character,env);env.interrupt_immediately(t.animation_ids[150+game.character],3);t.signal_script(150+game.character,static_cast<u16>(m.selected+17),env);t.set_phase(1,env.rate);
        for(i32 shot=0;shot<3;++shot){i32 cleared;__builtin_memcpy(&cleared,(*env.scores)->characters[game.character*3+shot].statistics+8+game.difficulty*4,4);if(!cleared)env.registry->set_visibility(env.registry->find_child(t.animation_ids[150+game.character],138+game.character*3+shot),false);}
        [[fallthrough]];
    case 1:if(t.elapsed.current>6)t.set_phase(2,env.rate);break;
    case 2:
        move(0x10,0x20,150+game.character);
        if(*env.pressed&10){t.set_phase(4,env.rate);env.sound(11);return 1;}
        if(*env.pressed&0x1001){t.set_phase(3,env.rate);env.sound(10);select_child(150+game.character,126+game.character*6+m.selected,6);}break;
    case 3:
        if(t.elapsed.current==10){if(!(game.flags&0x10)){env.show_loading(480,392);env.hide_screen();}else{game.shot_type=m.selected;m.push();t.dismiss_script(100,env);t.set_screen(9,env.rate);}}
        if(t.elapsed.current>=40){game.shot_type=m.selected;m.push();if(game.flags&0x10){t.dismiss_script(100,env);t.set_screen(9,env.rate);}else{t.set_screen(3,env.rate);stage_configuration(game.difficulty<4?1:7);*env.pending_screen=7;env.fade_music(6);}}break;
    case 4:
        if(t.elapsed.current>=6){t.dismiss_script(150+game.character,env);t.dismiss_script(100,env);t.set_screen(7,env.rate);m.pop();}break;
    }return 1;
}
// 0x430ff0. A locked stage still samples the number keys, as in the original.
// DirectInput scancodes and virtual-key codes use different offsets.
i32 TitleSelection::stage(){
    auto& t=title;auto& m=t.menu;auto& env=environment;auto& game=*env.game;
    switch(t.phase){
    case 0:m.item_count=6;m.select(*env.remembered_stage);t.create_script(106,env);t.create_script(107+game.character,env);t.set_phase(1,env.rate);[[fallthrough]];
    case 1:if(t.elapsed.current>10)t.set_phase(2,env.rate);break;
    case 2:
        move(0x10,0x20,-1);
        if(*env.pressed&10){t.set_phase(4,env.rate);env.sound(11);*env.remembered_stage=m.selected;return 1;}
        if(*env.pressed&0x1001){const auto* stats=(*env.scores)->characters[game.character*3+game.shot_type].statistics;if(!stats[0x21+(game.difficulty*6+m.selected)*8])env.sound(37);else{t.set_phase(3,env.rate);env.sound(10);}*env.remembered_stage=m.selected;*env.practice_shortcut=0;const i32 base=env.read_keyboard()?2:0x31;for(i32 key=0;key<9;++key)if(env.keyboard[base+key]&0x80){*env.practice_shortcut=key+1;break;}}break;
    case 3:
        if(t.elapsed.current==10){env.show_loading(480,392);env.hide_screen();}
        if(t.elapsed.current>=40){m.push();t.set_screen(3,env.rate);stage_configuration(m.selected+1);env.fade_music(6);*env.pending_screen=7;}break;
    case 4:if(t.elapsed.current>=6){t.dismiss_script(106,env);t.dismiss_script(107+game.character,env);t.set_screen(8,env.rate);m.pop();}break;
    }return 1;
}
}
