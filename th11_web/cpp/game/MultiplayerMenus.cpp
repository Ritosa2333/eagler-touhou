#ifdef TH11_MULTIPLAYER
#include "PauseMenu.hpp"
namespace th11 {
bool PauseMenu::begin_multiplayer_pause(){
    if(!begin(false))return false;
    multiplayer_pause=true;transition(32);cursor.count=4;cursor.wrap=1;
    cursor.disabled_count=2;cursor.disabled[0]=1;cursor.disabled[1]=2;cursor.select(0);
    // Resume and Restart retain their native scripts. Restart requests the
    // same confirmed generation fence as R; it never resets the local world.
    auto* menu=animations.find(menu_animation);if(!menu)return false;
    for(auto* node=menu->child.next;node;){auto* next=node->next;auto& vm=*node->value;
        if(vm.script_index==78||vm.script_index==79)animations.destroy(vm);
        node=next;
    }
    return true;
}
bool PauseMenu::update_multiplayer_pause(u32 pressed,u32 repeat){
    sounds.clear();action=PauseAction::None;scan_requested=save_requested=recording_metadata_requested=false;
    if((state==32||state==33)&&(pressed&0x200000)){action=PauseAction::Restart;return true;}
    if(state==33){
        move(pressed|repeat,7);
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
