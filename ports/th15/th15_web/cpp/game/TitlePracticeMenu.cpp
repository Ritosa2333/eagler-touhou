#include "TitlePracticeMenu.hpp"
namespace th15 {
bool TitlePracticeMenu::check(bool ok,const char* reason){if(!ok&&error.empty())error=reason;return ok;}
bool TitlePracticeMenu::visual(bool ok){if(!ok&&error.empty())error=visuals.error;return ok;}
bool TitlePracticeMenu::update(u32 pressed,u32 repeated,i32 numbered_chapter,const TitleRecords& records){
    switch(state.substate){
    case 0:state.menu.count=6;state.menu.select(settings.preferred_stage);if(!visual(visuals.create(106)))return false;state.change_substate(1);if(state.return_reason==4)state.return_reason=1;[[fallthrough]];
    case 1:if(state.age.current==1)host.practice_overlay_state(1);if(state.age.current>10)state.change_substate(2);break;
    case 2:
        if(host.practice_overlay_enabled()){
            i32 stage=state.menu.cursor;const i32 action=host.practice_overlay_action(stage);
            if(action<0){state.change_substate(4);host.practice_overlay_state(4);return check(host.sound(9),"Practice cancel sound failed");}
            if(!action)return true;
            state.menu.cursor=stage;state.change_substate(3);host.practice_overlay_state(3);settings.preferred_stage=stage;state.practice_chapter=0;
            return check(host.sound(7),"Practice confirm sound failed")&&check(host.sound(50),"Practice start sound failed")&&check(host.prepare_game_music(),"Practice music preparation failed");
        }
        state.menu.previous=state.menu.cursor;
        if((pressed|repeated)&16)state.menu.move(-1);
        if((pressed|repeated)&32)state.menu.move(1);
        if(!state.menu.error.empty())return check(false,state.menu.error.c_str());
        if(state.menu.previous!=state.menu.cursor&&!check(host.sound(10),"Practice cursor sound failed"))return false;
        if(pressed&0x102){state.change_substate(4);if(!check(host.sound(9),"Practice cancel sound failed"))return false;settings.preferred_stage=state.menu.cursor;return true;}
        if(pressed&0x80001){
            const i32 character=progress.character+progress.subcharacter;
            if(character<0||character>=4||progress.difficulty<0||progress.difficulty>=5)return check(false,"Invalid practice record selection");
            if(!records.practice_stages[character][progress.difficulty][state.menu.cursor])return check(host.sound(16),"Practice locked-stage sound failed");
            state.change_substate(3);if(!check(host.sound(7),"Practice confirm sound failed")||!check(host.sound(50),"Practice start sound failed"))return false;
            settings.preferred_stage=state.menu.cursor;state.practice_chapter=numbered_chapter>=1&&numbered_chapter<=9?numbered_chapter:0;
            return check(host.prepare_game_music(),"Practice start music preparation failed");
        }break;
    case 3:
        if(state.age.current==10&&(!check(host.begin_transition(state.start_effect),"Practice transition unavailable")||!check(animations.interrupt(state.start_effect,7),"Practice transition interrupt failed")))return false;
        if(state.age.current>39){state.menu.push();state.change_screen(TitleScreen::StartGame);progress.stage=state.menu.cursor+1;state.return_reason=4;settings.preferred_stage=state.menu.cursor;return check(host.start_game(progress.stage),"Practice scene destination failed");}break;
    case 4:if(state.age.current>5){if(!visual(visuals.retire(106)))return false;state.change_screen(TitleScreen::Character);state.menu.pop();}break;
    }
    return true;
}
bool TitleResumeMenu::check(bool ok,const char* reason){if(!ok&&error.empty())error=reason;return ok;}
bool TitleResumeMenu::visual(bool ok){if(!ok&&error.empty())error=visuals.error;return ok;}
bool TitleResumeMenu::update(u32 pressed,u32 repeated){
    const i32 root=134+(progress.difficulty==4);
    switch(state.substate){
    case 0:
        state.menu.count=2;state.menu.select(0);
        if(!visual(visuals.child_interrupt(root,progress.character+147,3))||!visual(visuals.create(85))||!visual(visuals.interrupt(85,state.menu.cursor+7)))return false;
        state.change_substate(1);[[fallthrough]];
    case 1:if(state.age.current>6)state.change_substate(2);break;
    case 2:
        state.menu.previous=state.menu.cursor;
        if((pressed|repeated)&0x40){if(!check(host.sound(10),"Resume cursor sound failed"))return false;state.menu.move(-1);if(!visual(visuals.interrupt(85,state.menu.cursor+7)))return false;}
        if((pressed|repeated)&0x80){if(!check(host.sound(10),"Resume cursor sound failed"))return false;state.menu.move(1);if(!visual(visuals.interrupt(85,state.menu.cursor+7)))return false;}
        if(!state.menu.error.empty())return check(false,state.menu.error.c_str());
        if(pressed&0x102){state.change_substate(4);return check(host.sound(9),"Resume cancel sound failed");}
        if(pressed&0x80001){
            if(!check(host.sound(7),"Resume confirm sound failed"))return false;state.change_substate(3);
            if(!visual(visuals.child_interrupt(85,state.menu.cursor+87,6))||!check(host.sound(50),"Resume start sound failed"))return false;
            if(!(player.mode_flags&0x30))return check(host.prepare_game_music(),"Resume start music preparation failed");
        }break;
    case 3:
        if(state.age.current==10&&(!check(host.begin_transition(state.start_effect),"Resume transition unavailable")||!check(animations.interrupt(state.start_effect,7),"Resume transition interrupt failed")))return false;
        if(state.age.current>39){
            state.menu.push();progress.spell_id=-1;state.change_screen(TitleScreen::StartGame);progress.stage=progress.difficulty<4?1:7;
            if(state.menu.cursor==0){player.mode_flags=(player.mode_flags&~0x100u)|0x200u;if(!check(host.checkpoint_stage(progress.character,progress.difficulty,progress.stage),"Resume checkpoint stage unavailable"))return false;if(progress.stage<1||progress.stage>7)return check(false,"Resume checkpoint stage outside range");}
            return check(host.start_game(progress.stage),"Resume scene destination failed");
        }break;
    case 4:
        if(state.age.current>5){if(!visual(visuals.retire(85))||!visual(visuals.child_interrupt(root,progress.character+147,2)))return false;state.change_screen(TitleScreen::Character);state.menu.pop();}break;
    }
    return true;
}
}
