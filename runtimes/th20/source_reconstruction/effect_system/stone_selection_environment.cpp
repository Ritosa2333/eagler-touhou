#include "stone_selection.hpp"
#include "../gameplay/gameplay.hpp"
#include "../overlay_system/overlay.hpp"
#include "../stone_menu/stone.hpp"
#include "../progress_state/manager.hpp"
namespace th20::source::effects::stone_selection_environment {
bool updates_enabled(){auto* c=gameplay::controller;return !c||(c->update_node&&(c->update_node->flags&2));}
bool button(unsigned index){auto* c=overlay::controller(0);switch(index){
case 0:return overlay::main_active(*c);
case 1:return overlay::unfocused_active(*c);
case 2:return overlay::focused_active(*c);
default:return overlay::passive_active(*c);}}
int stone(unsigned index){return overlay::selected_stone(*game_session::context(0).current_player,index);}
bool alternate(unsigned index){return overlay::inherited_stone(*game_session::context(0).current_player,index)!=0;}
int selected_profile(unsigned index){return progress::manager->selected_profile(index,recovered::signed_bits(game_session::context(0).current_player->fields_00[2]));}
sprite::AnimationFile& file(){return *stone_menu::controller->file;}
}
