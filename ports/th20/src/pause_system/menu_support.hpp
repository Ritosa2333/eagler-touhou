#pragma once
#include "pause.hpp"
#include "../progress_state/profile.hpp"
namespace th20::source::pause {
void save_cursor(menu::Cursor&); //4c60b0
void restore_cursor(menu::Cursor&); //4c6010
void exclude(menu::Cursor&,int); //4e1ab0
int continue_count(game_session::Session&); //4b7fc0
int credits(const game_session::Session&); //4e66d0
const char* name_characters(); //actualCP9325aff7c
void initialize_name(PauseInf&);
void update_ranking(PauseInf&); //4e6250 ->50f170/50f2f0
int insert_high_score(progress::Profile&); //50f170, shared with Title name entry
void finish_choice(PauseInf&,Services&); //4e2260 substate18
namespace unrecovered {
runtime::CallbackOwner* create_help(); //4bfc70 ->4bee70 HelpInf
runtime::CallbackOwner* create_options(const sprite::Vec3&); //4e1080 OptionInf
}
}
