#include "Results.hpp"
namespace th10 {
static void replay_name(char (&name)[12],i32 slot){std::memcpy(name,"th10_00.rpy",11);name[11]=0;name[5]='0'+slot/10;name[6]='0'+slot%10;}
void Results::move_keyboard(ResultsEnvironment& env){
    keyboard.reserved=keyboard.selected;
    if(env.repeat(0x10))keyboard.move(-13);if(env.repeat(0x20))keyboard.move(13);
    if(env.repeat(0x40))keyboard.move(keyboard.selected%13==0?12:-1);
    if(env.repeat(0x80))keyboard.move(keyboard.selected%13==12?-12:1);
    if(keyboard.reserved!=keyboard.selected)env.sound(12);
}
// The last three cells are space, backspace and accept. A full name overwrites
// its final character, while reaching eight characters selects accept.
bool Results::enter_character(ResultsEnvironment& env,bool for_replay){
    const i32 length=std::strlen(env.alphabet),selected=keyboard.selected;
    if(selected<length-2){
        const char value=selected==length-3?' ':env.alphabet[selected];
        if(name_length<8){name[name_length++]=value;if(name_length>=8)keyboard.select(length-1);}else name[name_length-1]=value;
    }else if(selected==length-2){
        if(!name_length)return true;name[--name_length]=' ';if(for_replay){env.sound(11);return true;}
    }else if(selected==length-1){
        if(for_replay){
            env.sound(44);char filename[12];replay_name(filename,menu.selected+1);
            env.delete_replay(previews[menu.selected]);env.save_replay(filename,name);previews[menu.selected]=env.preview(filename);state=10;reset_timer(env.rate);
            std::memcpy((*env.scores)->settings.last_name,name,std::strlen(name)+1);return true;
        }
        auto& game=*env.game;std::memcpy((*env.scores)->characters[game.character*3+game.shot_type].high_scores[game.difficulty][menu.selected].name,name,std::strlen(name)+1);
        std::memcpy((*env.scores)->settings.last_name,name,std::strlen(name)+1);env.visible(menu_animation,true);state=8;menu.item_count=3;menu.wrap=1;menu.select(0);env.interrupt(menu_animation,3);env.interrupt(menu_animation,static_cast<u16>(menu.selected+7));
    }
    env.sound(10);return false;
}
// 0x4236f0. This contains the complete post-game menu state machine, including
// ranking entry, replay slot previews and both name-entry confirmation paths.
void Results::update_results(ResultsEnvironment& env){
    switch(state){
    case 6:
        if(elapsed.current>=10&&(*env.pressed&0x1001)){env.sound(10);state=7;env.interrupt(menu_animation,2);reset_timer(env.rate);}return;
    case 7:
        if(elapsed.current<10)return;finalize_score_display(**env.gui,*env.game);
        if(!(env.game->flags&0x10)){register_score(env);reset_timer(env.rate);if(!no_rank){state=12;env.visible(menu_animation,false);return;}}
        else{auto& game=*env.game;auto* location=(*env.scores)->characters[game.character*3+game.shot_type].statistics+0x14+(game.stage+game.difficulty*6)*8;i32 high;std::memcpy(&high,location,4);if(high<game.score)std::memcpy(location,&game.score,4);}
        state=8;menu.item_count=3;menu.wrap=1;menu.select(2);env.interrupt(menu_animation,3);env.interrupt(menu_animation,static_cast<u16>(menu.selected+7));return;
    case 8:
        menu.reserved=menu.selected;if(env.repeat(0x10))menu.move(-1);if(env.repeat(0x20))menu.move(1);
        if(menu.reserved!=menu.selected){env.interrupt(menu_animation,static_cast<u16>(menu.selected+7));env.sound(12);}
        if(*env.pressed&0x1001){
            env.interrupt(env.child(menu_animation,menu.selected+124),6);env.sound(10);state=13;reset_timer(env.rate);finalize_score_display(**env.gui,*env.game);
            if(menu.selected==1){state=10;reset_timer(env.rate);env.visible(menu_animation,false);menu.push();menu.item_count=25;menu.wrap=1;menu.select(0);for(i32 i=0;i<25;++i){char filename[12];replay_name(filename,i+1);previews[i]=env.preview(filename);}}
            else if(menu.selected==2){state=13;reset_timer(env.rate);env.sound(10);}
        }
        if(*env.pressed&10){env.sound(11);if(menu.selected!=2){menu.select(2);env.interrupt(menu_animation,static_cast<u16>(menu.selected+7));}}return;
    case 10:
        if(elapsed.current<10)return;menu.reserved=menu.selected;if(env.repeat(0x10))menu.move(-1);if(env.repeat(0x20))menu.move(1);if(menu.reserved!=menu.selected)env.sound(12);
        if(*env.pressed&0x1001){state=11;reset_timer(env.rate);initialize_name(env,true);env.sound(10);return;}
        if(*env.pressed&10){state=8;reset_timer(env.rate);env.visible(menu_animation,true);menu.pop();menu.item_count=3;menu.wrap=1;env.interrupt(menu_animation,static_cast<u16>(menu.selected+7));for(auto*& preview:previews){if(preview)env.delete_replay(preview);preview=nullptr;}state=8;reset_timer(env.rate);env.sound(11);}return;
    case 11:
        if(elapsed.current<10)return;move_keyboard(env);
        if((*env.pressed&0x1001)&&enter_character(env,true))return;
        if(*env.pressed&10){env.sound(11);if(name_length){name[--name_length]=' ';return;}state=10;reset_timer(env.rate);}return;
    case 12:
        if(elapsed.current<10)return;if(!no_rank)move_keyboard(env);
        if(*env.pressed&0x1001){if(!no_rank){if(enter_character(env,false))return;}else{env.visible(menu_animation,true);state=8;menu.item_count=3;menu.wrap=1;menu.select(0);env.interrupt(menu_animation,3);env.interrupt(menu_animation,static_cast<u16>(menu.selected+7));}}
        if(*env.pressed&10){if(menu.selected<0){state=8;menu.item_count=3;menu.select(0);reset_timer(env.rate);env.sound(10);return;}if(name_length){env.sound(11);name[--name_length]=' ';}}return;
    case 13:
        if(elapsed.current>=12){dismiss(env);state=21;if(menu.selected==0)*env.pending_screen=cleared?10:13;else if(menu.selected==1||menu.selected==2)env.select_screen(4);}return;
    default:return;
    }
}
}
