#ifdef TH11_MULTIPLAYER
#include "PauseMenu.hpp"
namespace th11 {
bool PauseMenu::begin_multiplayer_pause(){
    if(!begin(false))return false;
    multiplayer_pause=true;transition(32);cursor.count=4;cursor.wrap=1;
    cursor.disabled_count=0;cursor.select(0);
    // Return ends the confirmed session; Restart advances its generation.
    // Neither action may rebuild or retire just the local world.
    return animations.find(menu_animation)!=nullptr;
}
bool PauseMenu::update_multiplayer_pause(u32 pressed,u32 repeat){
    sounds.clear();action=PauseAction::None;scan_requested=save_requested=recording_metadata_requested=false;
    if(state>=4&&state<=10){
        // Use the original confirmation, 25 slots and name-entry owner.
        multiplayer_pause=false;const bool ok=update(pressed,repeat);multiplayer_pause=true;
        if(state==3)state=33;
        return ok;
    }
    if((state==32||state==33)&&(pressed&0x200000)){action=PauseAction::Restart;return true;}
    if(state==33){
        move(pressed|repeat,7);
        if((pressed&0x80001)&&cursor.selected==1){sounds.push_back(10);choose(78);action=PauseAction::Title;return true;}
        if((pressed&0x80001)&&cursor.selected==2){sounds.push_back(10);choose(79);transition(7);timer.tick();elapsed.tick();return true;}
        if((pressed&0x80001)&&cursor.selected==3){sounds.push_back(10);choose(80);action=PauseAction::Restart;return true;}
    }
    if((state==32||state==33)&&((pressed&256)||(state==33&&(pressed&0x80001)))){
        cursor.select(0);
        sounds.push_back(10);family(background_animation,1);family(menu_animation,1);transition(34);
    }
    switch(state){
    case 32:if(timer.current>=10){family(menu_animation,7);transition(33);}break;
    case 33:break;
    case 34:if(timer.current>11){state=0;action=PauseAction::Resume;}break;
    case 0:break;
    default:error="invalid MP pause state";return false;
    }
    timer.tick();elapsed.tick();return true;
}
bool PauseMenu::begin_multiplayer_end(bool cleared){
    multiplayer_result=true;multiplayer_pause=false;replay=false;
    // The original end menu owns the one shared score, name registration,
    // native Replay Save and Return. Only Continue/Retry are unavailable.
    return begin_end(false,cleared);
}
}
#endif
