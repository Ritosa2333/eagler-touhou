#include "TitleMainMenu.hpp"
namespace th15 {
bool TitleMainMenu::fail(const std::string& reason){if(error.empty())error=reason;return false;}
bool TitleMainMenu::present(u32& handle){if(animations.registry.find(handle))return true;handle=0;return false;}
bool TitleMainMenu::create(i32 script){state.handles[script]=animations.create(title_bank,script,-1,0);return state.handles[script]||fail(animations.error);}
bool TitleMainMenu::interrupt(i32 script,i32 label,bool immediate){return animations.interrupt(state.handles[script],label,immediate)||fail(animations.error);}
bool TitleMainMenu::child_interrupt(i32 script,i32 label,bool immediate){
    auto* root=animations.registry.find(state.handles[0]);if(!root){state.handles[0]=0;return fail("Title menu root unavailable");}
    auto* child=animations.registry.find_child_script(*root,script,0);if(!child)return fail("Title menu child unavailable");
    child->pending_interrupt=label;return !immediate||animations.tick_instance(*child)>=0||fail(animations.error);
}
bool TitleMainMenu::sound(i32 id){return play_sound?play_sound(id)||fail("Title sound request failed"):fail("Title sound service unavailable");}
void TitleMainMenu::reset_mode(i32 mode)noexcept{if((player.mode_flags&0x30)!=0x20)progress.spell_id=-1;player.mode_flags=(player.mode_flags&~0x30u)|(u32(mode)<<4&0x30);}
bool TitleMainMenu::paint(bool extra_available){
    for(i32 i=0;i<state.menu.cursor;i++)if(!child_interrupt(i+3,30,true)||!child_interrupt(i+12,30,true))return false;
    for(i32 i=state.menu.cursor+1;i<9;i++)if(!child_interrupt(i+3,31,true)||!child_interrupt(i+12,31,true))return false;
    return extra_available||(child_interrupt(4,29,false)&&child_interrupt(13,29,false));
}
bool TitleMainMenu::select_animation(bool extra_available){return interrupt(0,3,true)&&interrupt(0,i32(i16(state.menu.cursor))+7,false)&&paint(extra_available);}
bool TitleMainMenu::update(u32 pressed,u32 repeated,bool extra_available){
    switch(state.substate){
    case 0:
        state.menu.count=9;state.menu.wrapping=true;
        if(!extra_available&&!state.menu.disable(1))return fail(state.menu.error);
        if(player.mode_flags&0x30){state.menu.select(2);reset_mode(0);}
        state.change_substate(1);
        if(!(state.flags&2)){
            if(!present(state.handles[89])&&(!create(89)||!interrupt(89,2,true)))return false;
            if(!present(state.handles[93])&&(!create(93)||!interrupt(93,2,true)))return false;
            if(state.previous_screen!=TitleScreen::Options&&!interrupt(89,2,true))return false;
            state.age.set(120);
        }else{if(!create(89)||!create(93))return false;state.flags&=~2u;}
        [[fallthrough]];
    case 1:
        if(state.age.current==120){
            if(!create(0))return false;
            if(!present(state.portrait)){state.portrait=animations.create(portrait_bank,0,-1,0);if(!state.portrait)return fail(animations.error);}
            if(!paint(extra_available))return false;
        }
        if(state.age.current>130){state.change_substate(2);if(!interrupt(0,3,true)||!interrupt(0,i32(i16(state.menu.cursor))+17,false)||!paint(extra_available))return false;}
        break;
    case 2:
        state.menu.previous=state.menu.cursor;
        if((pressed|repeated)&16)state.menu.move(-1);
        if((pressed|repeated)&32)state.menu.move(1);
        if(!state.menu.error.empty())return fail(state.menu.error);
        if(state.menu.previous!=state.menu.cursor&&(!sound(10)||!select_animation(extra_available)))return false;
        if(pressed&0x102){
            if(state.menu.cursor==8){if(!sound(9))return false;state.change_substate(4);return true;}
            if(!sound(9))return false;state.menu.select(8);if(!select_animation(extra_available))return false;
        }
        if(pressed&0x80001){
            if(!interrupt(0,6,false))return false;
            const i32 selected=state.menu.cursor;
            if(selected>=0&&selected<=8){
                if(!sound(selected==8?9:7))return false;
                if(selected!=6&&selected!=8){if(!interrupt(93,1,false))return false;state.handles[93]=0;if(!animations.interrupt(state.portrait,1,false))return fail(animations.error);}
                state.change_substate(4);
            }
        }
        break;
    case 4:
        if(state.age.current>19){
            switch(state.menu.cursor){
            case 0:reset_mode(0);if(!interrupt(89,3,true))return false;state.change_screen(TitleScreen::Mode);state.menu.push();state.menu.select(settings.preferred_mode);player.mode_flags=(player.mode_flags&~0x300u)|(u32(settings.preferred_mode)<<8&0x300);progress.difficulty=settings.configured_difficulty;break;
            case 1:reset_mode(0);if(!interrupt(89,3,true))return false;state.change_screen(TitleScreen::Difficulty);state.menu.push();settings.saved_difficulty=progress.difficulty;player.mode_flags&=~0x300u;progress.difficulty=4;state.menu.select(0);break;
            case 2:reset_mode(1);if(!interrupt(89,3,true))return false;state.menu.push();state.menu.select(settings.configured_difficulty);progress.difficulty=settings.configured_difficulty;state.change_screen(TitleScreen::Difficulty);player.mode_flags&=~0x300u;break;
            case 3:case 4:case 5:case 7:{if(!interrupt(89,3,true))return false;const auto destination=state.menu.cursor==3?TitleScreen::Replay:state.menu.cursor==4?TitleScreen::PlayerData:state.menu.cursor==5?TitleScreen::MusicRoom:TitleScreen::Manual;state.change_screen(destination);state.menu.push();break;}
            case 6:state.change_screen(TitleScreen::Options);state.menu.push();break;
            case 8:state.change_screen(TitleScreen::Quit);break;
            }
        }
        break;
    }
    return true;
}
}
