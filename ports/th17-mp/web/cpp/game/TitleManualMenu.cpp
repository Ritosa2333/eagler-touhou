#include "TitleManualMenu.hpp"
namespace th17 {
bool TitleManualMenu::update(){
 auto check=[&](bool ok){if(!ok&&error.empty())error=visuals.error;return ok;};
 if(state.substate==0){if(!check(visuals.prompt())||!check(visuals.create(103)))return false;state.change_substate(1);manual.offset_x=128;}
 else if(state.substate==1&&manual.finished){if(!check(visuals.retire(TitleAnimations::prompt_slot))||!check(visuals.retire(103)))return false;state.change_screen(TitleScreen::Main);}
 return true;
}
}
