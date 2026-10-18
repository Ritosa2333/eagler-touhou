#include "TitleManualMenu.hpp"
namespace th15 {
bool TitleManualMenu::update(){
 auto check=[&](bool ok){if(!ok&&error.empty())error=visuals.error;return ok;};
 if(state.substate==0){if(!check(visuals.prompt())||!check(visuals.create(107)))return false;state.change_substate(1);manual.offset_x=128;}
 else if(state.substate==1&&manual.finished){if(!check(visuals.retire(203))||!check(visuals.retire(107)))return false;state.change_screen(TitleScreen::Main);}
 return true;
}
}
