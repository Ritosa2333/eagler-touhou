#include "TitleAnimalMenu.hpp"
#include "GameProgression.hpp"
#include <cstdio>
namespace th17 {
bool TitleAnimalMenu::check(bool ok,const char* message){if(!ok&&error.empty())error=message;return ok;}
bool TitleAnimalMenu::visual(bool ok){return check(ok,visuals.error.c_str());}
bool TitleAnimalMenu::choose_stage(i32 root){
    settings.preferred_animal=selection.animal;selection.animal=state.menu.selected;state.menu.push();
    if(!visual(visuals.retire(root))||!visual(visuals.retire(95)))return false;
    state.change_screen(TitleScreen::Stage);return true;
}
bool TitleAnimalMenu::update(u32 pressed,u32 repeated,const TitleRecords& records){
    const i32 root=131+(selection.difficulty==4?2:0);
    switch(state.substate){
    case 0:
        state.menu.count=3;
        if(selection.difficulty==4){
            if(!records.extra(selection.character,state.menu.selected))for(i32 a=0;a<3;a++)if(records.extra(selection.character,a)){state.menu.select(a);break;}
            for(i32 a=0;a<3;a++)if(!records.extra(selection.character,a)&&!state.menu.disable(a))return check(false,"No animal spirit available for Extra");
        }
        if(!visual(visuals.retire(root)))return false;
        if(state.return_reason==4){state.menu.select(selection.animal);return choose_stage(root);}
        state.substate=1;[[fallthrough]];
    case 1:
        if(state.age.current>30){
            if(!check(host.sound(72),"Animal selection sound unavailable")||!visual(visuals.create(root))||!visual(visuals.interrupt(root,3,true))||!visual(visuals.interrupt(root,i32(i16(state.menu.selected))+31,true)))return false;
            state.change_substate(1);if(!visual(visuals.create(95)))return false;
            for(i32 a=0;a<3;a++)if(!records.cleared[selection.character*3+a][selection.difficulty]&&!visual(visuals.hide_child(root,142+a)))return false;
            state.change_substate(2);
        }break;
    case 2:
        state.menu.reserved=state.menu.selected;
        if((pressed|repeated)&16){if(!check(host.sound(10),"Animal selection sound unavailable")||!visual(visuals.interrupt(root,i32(i16(state.menu.selected))+25,true)))return false;state.menu.step(-1);if(!visual(visuals.interrupt(root,i32(i16(state.menu.selected))+13)))return false;}
        if((pressed|repeated)&32){if(!check(host.sound(10),"Animal selection sound unavailable")||!visual(visuals.interrupt(root,i32(i16(state.menu.selected))+19,true)))return false;state.menu.step(1);if(!visual(visuals.interrupt(root,i32(i16(state.menu.selected))+7)))return false;}
        if(pressed&0x102){state.change_substate(4);return check(host.sound(9),"Animal cancellation sound unavailable");}
        if(pressed&0x80001){
            if(!check(host.sound(7),"Animal confirmation sound unavailable"))return false;state.change_substate(3);
            if(!check(host.sound(50),"Game start sound unavailable"))return false;
            if(!(selection.mode_flags&0x30)&&!check(host.fade_menu_music(),"Game start music service unavailable"))return false;
        }break;
    case 3:
        if(state.age.current==10){
            if(selection.mode_flags&0x30)return choose_stage(root);
            if(!check(host.show_loading_indicator(),"Game loading indicator unavailable")||!check(host.begin_transition(transition),"Game transition service unavailable")||!visual(animations.interrupt(transition,7)))return false;
        }
        if(state.age.current>39){
            selection.animal=state.menu.selected;settings.preferred_animal=selection.animal;state.menu.push();selection.spell_id=-1;state.change_screen(TitleScreen::Quit);
            selection.stage=selection.difficulty<4?1:7;return check(host.start_game(selection.stage),"Game start service unavailable");
        }break;
    case 4:
        if(state.age.current>5){selection.animal=state.menu.selected;settings.preferred_animal=selection.animal;if(!visual(visuals.retire(root))||!visual(visuals.retire(95)))return false;state.change_screen(TitleScreen::Character);state.menu.pop();}break;
    }
    return true;
}
bool TitleStageMenu::check(bool ok,const char* message){if(!ok&&error.empty())error=message;return ok;}
bool TitleStageMenu::visual(bool ok){return check(ok,visuals.error.c_str());}
bool TitleStageMenu::update(u32 pressed,u32 repeated,const TitleRecords& records){
    switch(state.substate){
    case 0:
        state.menu.count=6;state.menu.select(settings.practice_stage);if(!visual(visuals.create(102)))return false;state.change_substate(1);if(state.return_reason==4)state.return_reason=1;[[fallthrough]];
    case 1:if(state.age.current>10)state.change_substate(2);break;
    case 2:
        state.menu.reserved=state.menu.selected;if((pressed|repeated)&16)state.menu.step(-1);if((pressed|repeated)&32)state.menu.step(1);
        if(state.menu.reserved!=state.menu.selected&&!check(host.sound(10),"Practice selection sound unavailable"))return false;
        if(pressed&0x102){state.change_substate(4);settings.practice_stage=state.menu.selected;return check(host.sound(9),"Practice cancellation sound unavailable");}
        if(pressed&0x80001){
            const i32 c=selection.character,a=selection.animal,d=selection.difficulty,s=state.menu.selected;
            if(c<0||c>=3||a<0||a>=3||d<0||d>=5||s<0||s>=8)return check(false,"Practice selection outside records");
            if(!records.stage_available[c*3+a][d][s])return check(host.sound(16),"Practice unavailable sound service missing");
            state.change_substate(3);if(!check(host.sound(7),"Practice confirmation sound unavailable")||!check(host.sound(50),"Game start sound unavailable"))return false;
            settings.practice_stage=state.menu.selected;selection.practice_checkpoint=host.checkpoint_key();return check(host.fade_menu_music(),"Practice music service unavailable");
        }break;
    case 3:
        if(state.age.current==10&&(!check(host.show_loading_indicator(),"Practice loading indicator unavailable")||!check(host.begin_transition(transition),"Practice transition service unavailable")||!visual(animations.interrupt(transition,7))))return false;
        if(state.age.current>39){state.menu.push();state.change_screen(TitleScreen::Quit);selection.stage=state.menu.selected+1;state.return_reason=4;settings.practice_stage=state.menu.selected;return check(host.start_game(selection.stage),"Practice game start service unavailable");}break;
    case 4:
        if(state.age.current>5){if(!visual(visuals.retire(102)))return false;state.change_screen(TitleScreen::Animal);state.menu.pop();}break;
    }
    return true;
}
bool TitleStageMenu::draw(const GameRecords& records,HudDrawServices& sink){
    if(!error.empty())return false;
    if(state.substate<2||state.substate>3||(state.age.current<10&&state.substate!=3))return true;
    const i32 character=selection.character,animal=selection.animal,difficulty=selection.difficulty;
    if(u32(character)>=3||u32(animal)>=3||u32(difficulty)>=5)return check(false,"Practice drawing selection outside records");
    const i32 loadout=character*3+animal;HudTextDraw text;text.style.shadow=true;char line[64];
    for(i32 stage=0;stage<6;stage++){
        const bool available=records.title.stage_available[loadout][difficulty][stage]!=0;
        text.position={240,float(192+stage*18),0};text.style.color=0xff808080;
        if(stage==state.menu.selected)text.style.color=!available?0xffdfdfdf:state.substate==3&&state.age.current%4>=2?0xff000000:0xffffff00;
        if(available)std::snprintf(line,sizeof line,"Stage %d  %.8d0",stage+1,records.practice_scores[loadout][difficulty][stage+1]);
        else std::snprintf(line,sizeof line,"Stage %d  ---------",stage+1);
        text.text=line;if(!check(sink.hud_text(text),"Practice stage text rendering failed"))return false;
    }
    return true;
}
}
