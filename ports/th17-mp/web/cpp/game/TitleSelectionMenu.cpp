#include "TitleSelectionMenu.hpp"
namespace th17 {
bool TitleDifficultyMenu::visual(bool ok){if(!ok&&error.empty())error=visuals.error;return ok;}
bool TitleDifficultyMenu::sound(i32 id){if(play_sound&&play_sound(id))return true;error="Title difficulty sound service unavailable";return false;}
void TitleDifficultyMenu::choose_character(){
    state.change_screen(TitleScreen::Character);
    if(selection.difficulty<4)selection.difficulty=settings.configured_difficulty=state.menu.selected;
    state.menu.push();state.menu.wrap=1;state.menu.count=3;state.menu.select(settings.last_spell_character);selection.character=settings.last_spell_character;
}
bool TitleDifficultyMenu::update(u32 pressed,u32 repeated,const TitleRecords& records){
    const i32 root=116+(selection.difficulty>3);
    switch(state.substate){
    case 0:
        if(!visual(visuals.prompt()))return false;state.menu.wrap=0;state.menu.count=selection.difficulty<4?4:1;
        if(!visual(visuals.retire(root))||!visual(visuals.create(root))||!visual(visuals.interrupt(root,3,true))||!visual(visuals.interrupt(root,i32(i16(state.menu.selected))+13)))return false;
        if(state.return_reason==4){
            state.menu.select(selection.difficulty);
            if(!visual(visuals.interrupt(root,3,true))||!visual(visuals.interrupt(root,i32(i16(state.menu.selected))+7,true))||!visual(visuals.interrupt(root,6,true))||!visual(visuals.child_interrupt(root,state.menu.selected+106,2)))return false;
            choose_character();return true;
        }
        if(!visual(visuals.create(93)))return false;state.change_substate(1);
        if(selection.difficulty<4){for(i32 d=0;d<4;d++)if(!records.all_cleared(d)&&!visual(visuals.hide_child(root,137+d)))return false;}
        else if(!records.all_cleared(4)&&!visual(visuals.hide_child(root,141)))return false;
        [[fallthrough]];
    case 1:if(state.age.current>6)state.change_substate(2);break;
    case 2:
        if(selection.difficulty<4){
            state.menu.reserved=state.menu.selected;
            if((pressed|repeated)&0x50)state.menu.step(-1);
            if((pressed|repeated)&0xa0)state.menu.step(1);
            if(state.menu.reserved!=state.menu.selected&&(!sound(10)||!visual(visuals.interrupt(root,3,true))||!visual(visuals.interrupt(root,i32(i16(state.menu.selected))+7))))return false;
        }
        if(pressed&0x102){state.change_substate(4);return sound(9)&&visual(visuals.retire(root));}
        if(pressed&0x80001){if(!visual(visuals.interrupt(root,6))||!visual(visuals.child_interrupt(root,selection.difficulty<4?state.menu.selected+106:110,2)))return false;state.change_substate(3);return sound(7);}break;
    case 3:if(state.age.current>13){if(!visual(visuals.retire(93)))return false;choose_character();}break;
    case 4:
        if(state.age.current>5){
            if(!visual(visuals.retire(93)))return false;
            if(!(selection.mode_flags&0x30)&&selection.difficulty<4){state.change_screen(TitleScreen::Main);selection.difficulty=settings.configured_difficulty=state.menu.selected;state.menu.pop();return true;}
            if(selection.mode_flags&0x30)settings.configured_difficulty=state.menu.selected;
            selection.difficulty=settings.configured_difficulty;state.change_screen(TitleScreen::Main);
            if(!visual(visuals.retire(TitleAnimations::prompt_slot)))return false;
            state.change_screen(TitleScreen::Main);state.menu.pop();
        }break;
    }
    return true;
}
bool TitleCharacterMenu::visual(bool ok){if(!ok&&error.empty())error=visuals.error;return ok;}
bool TitleCharacterMenu::sound(i32 id){if(play_sound&&play_sound(id))return true;error="Title character sound service unavailable";return false;}
bool TitleCharacterMenu::selected_children(i32 root,bool recovering){
    if(!visual(visuals.child_interrupt(root,state.menu.selected+121,6))||!visual(visuals.child_interrupt(root,state.menu.selected+124,6))||!visual(visuals.child_interrupt(root,82,6))||!visual(visuals.child_interrupt(root,83,6)))return false;
    return recovering||visual(visuals.child_interrupt(root,147,6));
}
bool TitleCharacterMenu::choose_animal(i32 root){
    state.change_screen(TitleScreen::Animal);selection.character=state.menu.selected;settings.last_spell_character=selection.character;
    state.menu.push();state.menu.wrap=1;state.menu.count=3;state.menu.select(settings.preferred_animal);
    return visual(visuals.interrupt(root,6))&&visual(visuals.retire(94));
}
bool TitleCharacterMenu::update(u32 pressed,u32 repeated,const TitleRecords& records){
    const i32 root=130+(selection.difficulty==4?2:0);
    switch(state.substate){
    case 0:
        state.menu.count=3;
        if(selection.difficulty==4){
            if(!records.extra(state.menu.selected))for(i32 c=0;c<3;c++)if(records.extra(c)){state.menu.select(c);break;}
            for(i32 c=0;c<3;c++)if(!records.extra(c)&&!state.menu.disable(c)){error="No character available for Extra";return false;}
        }
        if(!visuals.present(94)&&!visual(visuals.create(94)))return false;
        if(!visuals.present(root)&&(!visual(visuals.retire(root))||!visual(visuals.create(root))))return false;
        if(!visual(visuals.interrupt(root,3,true))||!visual(visuals.interrupt(root,i32(i16(state.menu.selected))+7)))return false;
        state.change_substate(1);
        for(i32 c=0;c<3;c++)if(!records.all_cleared(selection.difficulty,c)&&!visual(visuals.hide_child(root,134+c)))return false;
        if(state.return_reason==4){state.menu.select(selection.character);if(!visual(visuals.interrupt(root,i32(i16(state.menu.selected))+7,true))||!selected_children(root,true))return false;return choose_animal(root);}
        [[fallthrough]];
    case 1:if(state.age.current>6)state.change_substate(2);break;
    case 2:
        state.menu.reserved=state.menu.selected;
        if((pressed|repeated)&0x50){if(!sound(10)||!visual(visuals.interrupt(root,i32(i16(state.menu.selected))+25,true)))return false;state.menu.step(-1);if(!visual(visuals.interrupt(root,i32(i16(state.menu.selected))+13)))return false;}
        if((pressed|repeated)&0xa0){if(!sound(10)||!visual(visuals.interrupt(root,i32(i16(state.menu.selected))+19,true)))return false;state.menu.step(1);if(!visual(visuals.interrupt(root,i32(i16(state.menu.selected))+7)))return false;}
        if(pressed&0x102){state.change_substate(4);return sound(9);}
        if(pressed&0x80001){if(!selected_children(root)||!sound(7))return false;state.change_substate(3);}break;
    case 3:if(state.age.current>=14)return choose_animal(root);break;
    case 4:
        if(state.age.current>5){if(!visual(visuals.retire(root))||!visual(visuals.retire(94)))return false;state.change_screen(TitleScreen::Difficulty);selection.character=state.menu.selected;state.menu.pop();settings.last_spell_character=selection.character;}break;
    }
    return true;
}
}
