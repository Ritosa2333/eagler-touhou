#ifdef TH11_MULTIPLAYER
#include "PauseMenu.hpp"
namespace th11 {
bool PauseMenu::begin_multiplayer_pause(){
    if(!begin(false))return false;
    multiplayer_pause=true;transition(32);cursor.count=1;cursor.select(0);
    // Keep the native pause headline, ornament and Resume. Runtime/confirmed
    // generation owners retain room exit and restart; ordinary Replay Save
    // and single-player Return/Retry must not acquire an MP action here.
    auto* menu=animations.find(menu_animation);if(!menu)return false;
    for(auto* node=menu->child.next;node;){auto* next=node->next;auto& vm=*node->value;
        if(vm.script_index==78||vm.script_index==79||vm.script_index==80)animations.destroy(vm);
        node=next;
    }
    return true;
}
bool PauseMenu::update_multiplayer_pause(u32 pressed){
    sounds.clear();action=PauseAction::None;scan_requested=save_requested=recording_metadata_requested=false;
    if((state==32||state==33)&&((pressed&256)||(state==33&&(pressed&0x80001)))){
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
