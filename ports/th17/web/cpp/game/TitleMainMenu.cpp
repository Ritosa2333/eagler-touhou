#include "TitleMainMenu.hpp"
namespace th17 {
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
bool TitleMainMenu::paint(bool extra_available){
    for(i32 i=0;i<state.menu.selected;i++)if(!child_interrupt(i+3,30,true)||!child_interrupt(i+13,30,true))return false;
    for(i32 i=state.menu.selected+1;i<10;i++)if(!child_interrupt(i+3,31,true)||!child_interrupt(i+13,31,true))return false;
    return extra_available||(child_interrupt(4,29,false)&&child_interrupt(14,29,false));
}
bool TitleMainMenu::select_animation(bool extra_available){return interrupt(0,3,true)&&interrupt(0,i32(i16(state.menu.selected))+7,false)&&paint(extra_available);}
bool TitleMainMenu::update(u32 pressed,u32 repeated,bool extra_available){
    switch(state.substate){
    case 0:
        state.menu.count=10;state.menu.wrap=1;
        if(!extra_available&&!state.menu.disable(1))return fail("Title menu disabled-item list exhausted");
        if(selection.mode_flags&0x30){state.menu.select((selection.mode_flags>>4&3)==2?3:2);selection.mode(0);}
        state.change_substate(1);
        if(!(state.flags&2)){
            if(!present(state.handles[86])&&(!create(86)||!interrupt(86,2,true)))return false;
            if(!present(state.handles[90])&&(!create(90)||!interrupt(90,2,true)))return false;
            if(state.previous_screen!=TitleScreen::Options&&!interrupt(86,2,true))return false;
            state.age.set(120);
        }else{if(!create(86)||!create(90))return false;state.flags&=~2u;}
        [[fallthrough]];
    case 1:
        if(state.age.current==120){
            if(!create(0))return false;
            if(!present(state.portrait)){state.portrait=animations.create(portrait_bank,0,-1,0);if(!state.portrait)return fail(animations.error);}
            if(!paint(extra_available))return false;
        }
        if(state.age.current>130){state.change_substate(2);if(!interrupt(0,3,true)||!interrupt(0,i32(i16(state.menu.selected))+17,false)||!paint(extra_available))return false;}
        break;
    case 2:
        state.menu.reserved=state.menu.selected;
        if((pressed|repeated)&16)state.menu.step(-1);
        if((pressed|repeated)&32)state.menu.step(1);
        if(state.menu.reserved!=state.menu.selected&&(!sound(10)||!select_animation(extra_available)))return false;
        if(pressed&0x102){
            if(state.menu.selected==9){if(!sound(9))return false;state.change_substate(4);return true;}
            if(!sound(9))return false;state.menu.select(9);if(!select_animation(extra_available))return false;
        }
        if(pressed&0x80001){
            if(!interrupt(0,6,false))return false;
            const i32 selected=state.menu.selected;
            if(selected>=0&&selected<=9){
                if(!sound(selected==9?9:7))return false;
                if(selected!=7&&selected!=9){if(!interrupt(90,1,false))return false;state.handles[90]=0;if(!animations.interrupt(state.portrait,1,false))return fail(animations.error);}
                state.change_substate(4);
            }
        }
        break;
    case 4:
        if(state.age.current>19){
            switch(state.menu.selected){
            case 0:case 1:case 2:{const i32 selected=state.menu.selected;selection.mode(selected==2?1:0);if(!interrupt(86,3,true))return false;state.change_screen(TitleScreen::Difficulty);state.menu.push();selection.difficulty=selected==1?4:settings.configured_difficulty;state.menu.select(selected==1?0:settings.configured_difficulty);break;}
            case 3:selection.mode(2);if(!interrupt(86,3,true))return false;state.change_screen(TitleScreen::SpellPractice);state.menu.push();state.spell_character.count=9;state.spell_character.wrap=1;state.spell_character.select(settings.last_spell_character);state.menu.select(0);break;
            case 4:case 5:case 6:case 8:{if(!interrupt(86,3,true))return false;const auto destination=state.menu.selected==4?TitleScreen::Replay:state.menu.selected==5?TitleScreen::PlayerData:state.menu.selected==6?TitleScreen::MusicRoom:TitleScreen::Manual;state.change_screen(destination);state.menu.push();if(destination==TitleScreen::PlayerData)state.menu.select(0);break;}
            case 7:state.change_screen(TitleScreen::Options);state.menu.push();break;
            case 9:state.change_screen(TitleScreen::Quit);break;
            }
        }
        break;
    }
    return true;
}
}
