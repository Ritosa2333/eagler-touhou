#include "TitleCharacterMenu.hpp"
namespace th15 {
bool TitleCharacterMenu::check(bool ok,const char* reason){if(!ok&&error.empty())error=reason;return ok;}
bool TitleCharacterMenu::visual(bool ok){if(!ok&&error.empty())error=visuals.error;return ok;}
bool TitleCharacterMenu::available(bool& result){result=false;return !(player.mode_flags&0x300)||check(host.checkpoint_available(state.menu.cursor,progress.difficulty,result),"Chapter resume availability failed");}
bool TitleCharacterMenu::selected_children(i32 root){return visual(visuals.child_interrupt(root,state.menu.cursor+126,6))&&visual(visuals.child_interrupt(root,state.menu.cursor+147,6))&&visual(visuals.child_interrupt(root,80,6))&&visual(visuals.child_interrupt(root,81,6));}
bool TitleCharacterMenu::choose_stage(){progress.character=settings.preferred_character=state.menu.cursor;state.menu.push();if(!visual(visuals.retire(98)))return false;state.change_screen(TitleScreen::Stage);return true;}
bool TitleCharacterMenu::choose_resume(){if(!visual(visuals.retire(84)))return false;progress.character=settings.preferred_character=state.menu.cursor;state.menu.push();state.change_screen(TitleScreen::ContinuePrompt);return true;}
bool TitleCharacterMenu::update(u32 pressed,u32 repeated,const TitleRecords& records){
    const i32 root=134+(progress.difficulty==4);
    switch(state.substate){
    case 0:{
        state.menu.count=4;
        if(progress.difficulty==4){
            if(!records.extra_character(state.menu.cursor))for(i32 character=0;character<4;character++)if(records.extra_character(character)){state.menu.select(character);break;}
            for(i32 character=0;character<4;character++)if(!records.extra_character(character)&&!state.menu.disable(character))return check(false,state.menu.error.c_str());
        }
        if(!animations.registry.find(state.handles[98])&&!visual(visuals.create(98)))return false;
        if(!animations.registry.find(state.handles[root])&&(!visual(visuals.retire(root))||!visual(visuals.create(root))))return false;
        if(!visual(visuals.interrupt(root,3,true))||!visual(visuals.interrupt(root,i32(i16(state.menu.cursor))+7)))return false;
        state.change_substate(1);
        for(i32 character=0;character<4;character++)if(!records.cleared(character,progress.difficulty,player.mode_flags)&&!visual(visuals.hide_child(root,136+character)))return false;
        bool resume=false;if(!available(resume))return false;if(resume&&!visual(visuals.create(84)))return false;
        if(state.return_reason!=4){if(!check(host.reset_resume_selection(),"Resume menu selection reset failed"))return false;}
        else{state.menu.select(progress.character);if(!visual(visuals.interrupt(root,i32(i16(state.menu.cursor))+7,true))||!selected_children(root))return false;return choose_stage();}
        [[fallthrough]];
    }
    case 1:if(state.age.current>6)state.change_substate(2);break;
    case 2:
        state.menu.previous=state.menu.cursor;
        if((pressed|repeated)&0x40){
            if(!visual(visuals.retire(84))||!check(host.sound(10),"Character cursor sound failed")||!visual(visuals.interrupt(root,i32(i16(state.menu.cursor))+25,true)))return false;
            state.menu.move(-1);if(!state.menu.error.empty())return check(false,state.menu.error.c_str());
            if(!visual(visuals.interrupt(root,i32(i16(state.menu.cursor))+13)))return false;
            bool resume=false;if(!available(resume))return false;if(resume&&!visual(visuals.create(84)))return false;
        }
        if((pressed|repeated)&0x80){
            if(!visual(visuals.retire(84))||!check(host.sound(10),"Character cursor sound failed")||!visual(visuals.interrupt(root,i32(i16(state.menu.cursor))+19,true)))return false;
            state.menu.move(1);if(!state.menu.error.empty())return check(false,state.menu.error.c_str());
            if(!visual(visuals.interrupt(root,i32(i16(state.menu.cursor))+7)))return false;
            bool resume=false;if(!available(resume))return false;if(resume&&!visual(visuals.create(84)))return false;
        }
        if(pressed&0x102){state.change_substate(4);return check(host.sound(9),"Character cancel sound failed");}
        if(pressed&0x80001){
            if(!selected_children(root)||!check(host.sound(7),"Character confirm sound failed"))return false;state.change_substate(3);
            bool resume=false;if(!available(resume))return false;
            if(!resume){if(!check(host.sound(50),"Game start sound failed"))return false;if(!(player.mode_flags&0x30)&&!check(host.prepare_game_music(),"Game start music preparation failed"))return false;}
        }
        break;
    case 3:
        if(state.age.current==10){
            if(player.mode_flags&0x30){if(!choose_stage())return false;}
            else{bool resume=false;if(!available(resume))return false;if(resume){if(!choose_resume())return false;}
                else{if(!check(host.begin_transition(state.start_effect),"Game start transition unavailable")||!check(animations.interrupt(state.start_effect,7),"Game start transition interrupt failed"))return false;}}
        }
        if(state.age.current>39){
            progress.character=settings.preferred_character=state.menu.cursor;state.menu.push();progress.spell_id=-1;state.change_screen(TitleScreen::StartGame);progress.stage=progress.difficulty<4?1:7;
            if(!check(host.start_game(progress.stage),"Game scene destination failed"))return false;
        }
        break;
    case 4:
        if(state.age.current>5){
            if(!visual(visuals.retire(84))||!visual(visuals.retire(root))||!visual(visuals.retire(98)))return false;
            state.change_screen(TitleScreen::Difficulty);progress.character=state.menu.cursor;state.menu.pop();settings.preferred_character=progress.character;
        }break;
    }
    return true;
}
}
