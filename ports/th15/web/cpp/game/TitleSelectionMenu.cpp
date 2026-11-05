#include "TitleSelectionMenu.hpp"
namespace th15 {
bool TitleModeMenu::check(bool ok){if(!ok&&error.empty())error=visuals.error.empty()?state.menu.error:visuals.error;return ok;}
bool TitleModeMenu::sound(i32 id){if(play_sound&&play_sound(id))return true;error="Title mode sound service unavailable";return false;}
bool TitleModeMenu::update(u32 pressed,u32 repeated){
    switch(state.substate){
    case 0:
        if(!check(visuals.prompt()))return false;state.menu.count=2;state.menu.wrapping=false;
        if(!check(visuals.retire(111))||!check(visuals.create(111))||!check(visuals.interrupt(111,3,true))||!check(visuals.interrupt(111,state.menu.cursor+7))||!check(visuals.create(108)))return false;
        state.change_substate(1);[[fallthrough]];
    case 1:if(state.age.current>6)state.change_substate(2);break;
    case 2:
        if(progress.difficulty<4){
            state.menu.previous=state.menu.cursor;
            if((pressed|repeated)&0x50)state.menu.move(-1);
            if((pressed|repeated)&0xa0)state.menu.move(1);
            if(!state.menu.error.empty())return check(false);
            if(state.menu.previous!=state.menu.cursor&&(!sound(10)||!check(visuals.interrupt(111,3,true))||!check(visuals.interrupt(111,i32(i16(state.menu.cursor))+7))))return false;
        }
        if(pressed&0x102){state.change_substate(4);return sound(9)&&check(visuals.retire(111));}
        if(pressed&0x80001){
            const i32 index=state.menu.cursor;
            if(!check(visuals.child_interrupt(111,index+109,6))||!check(visuals.child_interrupt(111,index+112,6))||!check(visuals.child_interrupt(111,110-index,5))||!check(visuals.child_interrupt(111,113-index,5)))return false;
            state.change_substate(3);return sound(7);
        }
        break;
    case 3:
        if(state.age.current>13){
            if(!check(visuals.retire(108)))return false;state.change_screen(TitleScreen::Difficulty);
            player.mode_flags=(player.mode_flags&~0x300u)|(u32(state.menu.cursor==0)<<8);settings.preferred_mode=state.menu.cursor;
            state.menu.push();state.menu.wrapping=true;state.menu.count=4;state.menu.select(settings.configured_difficulty);progress.difficulty=settings.configured_difficulty;
        }break;
    case 4:
        if(state.age.current>5){
            if(!check(visuals.retire(108))||!check(visuals.retire(111))||!check(visuals.retire(203)))return false;
            state.change_screen(TitleScreen::Main);player.mode_flags=(player.mode_flags&~0x300u)|(u32(state.menu.cursor==0)<<8);settings.preferred_mode=state.menu.cursor;state.menu.pop();
        }break;
    }
    return true;
}
bool TitleDifficultyMenu::check(bool ok){if(!ok&&error.empty())error=visuals.error.empty()?state.menu.error:visuals.error;return ok;}
bool TitleDifficultyMenu::sound(i32 id){if(play_sound&&play_sound(id))return true;error="Title difficulty sound service unavailable";return false;}
void TitleDifficultyMenu::choose_character()noexcept{
    state.change_screen(TitleScreen::Character);
    if(progress.difficulty<4){settings.configured_difficulty=state.menu.cursor;progress.difficulty=state.menu.cursor;}
    state.menu.push();state.menu.wrapping=true;state.menu.count=4;state.menu.select(settings.preferred_character);progress.character=settings.preferred_character;
}
void TitleDifficultyMenu::return_main(){
    state.change_screen(TitleScreen::Main);visuals.retire(203);state.change_screen(TitleScreen::Main);
    player.mode_flags=(player.mode_flags&~0x300u)|(u32(settings.preferred_mode)<<8&0x300);state.menu.pop();
}
bool TitleDifficultyMenu::update(u32 pressed,u32 repeated,const std::array<bool,5>& cleared){
    const i32 root=124+(progress.difficulty>3);
    switch(state.substate){
    case 0:
        if(!check(visuals.prompt()))return false;state.menu.wrapping=false;state.menu.count=progress.difficulty<4?4:1;
        if(!check(visuals.retire(root))||!check(visuals.create(root))||!check(visuals.interrupt(root,3,true))||!check(visuals.interrupt(root,i32(i16(state.menu.cursor))+13)))return false;
        if(state.return_reason==4){
            state.menu.select(progress.difficulty);
            if(!check(visuals.interrupt(root,3,true))||!check(visuals.interrupt(root,i32(i16(state.menu.cursor))+7,true))||!check(visuals.interrupt(root,6,true))||!check(visuals.child_interrupt(root,state.menu.cursor+114,2)))return false;
            choose_character();return true;
        }
        if(!check(visuals.create(97)))return false;state.change_substate(1);
        if(progress.difficulty<4){for(i32 difficulty=0;difficulty<4;difficulty++)if(!cleared[difficulty]&&!check(visuals.hide_child(root,140+difficulty)))return false;}
        else if(!cleared[4]&&!check(visuals.hide_child(root,144)))return false;
        [[fallthrough]];
    case 1:if(state.age.current>6)state.change_substate(2);break;
    case 2:
        if(progress.difficulty<4){
            state.menu.previous=state.menu.cursor;
            if((pressed|repeated)&0x50)state.menu.move(-1);
            if((pressed|repeated)&0xa0)state.menu.move(1);
            if(!state.menu.error.empty())return check(false);
            if(state.menu.previous!=state.menu.cursor&&(!sound(10)||!check(visuals.interrupt(root,3,true))||!check(visuals.interrupt(root,i32(i16(state.menu.cursor))+7))))return false;
        }
        if(pressed&0x102){state.change_substate(4);return sound(9)&&check(visuals.retire(root));}
        if(pressed&0x80001){
            if(!check(visuals.interrupt(root,6))||!check(visuals.child_interrupt(root,progress.difficulty<4?state.menu.cursor+114:118,2)))return false;
            state.change_substate(3);return sound(7);
        }
        break;
    case 3:if(state.age.current>13){if(!check(visuals.retire(97)))return false;choose_character();}break;
    case 4:
        if(state.age.current>5){
            if(!check(visuals.retire(97)))return false;
            if(player.mode_flags&0x30){settings.configured_difficulty=state.menu.cursor;progress.difficulty=state.menu.cursor;return_main();}
            else if(progress.difficulty<4){state.change_screen(TitleScreen::Mode);progress.difficulty=settings.configured_difficulty=state.menu.cursor;state.menu.pop();}
            else{progress.difficulty=settings.configured_difficulty;return_main();}
            if(!visuals.error.empty())return check(false);
        }break;
    }
    return true;
}
}
