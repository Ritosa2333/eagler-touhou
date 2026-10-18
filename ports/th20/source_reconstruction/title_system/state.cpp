#include "title.hpp"
#include "../gameplay/player_state.hpp"
namespace th20::source::title {
void set_state(TitleInf& o,int value){o.previous_state=o.state;o.state=value;o.phase=0;recovered::timer_set(o.age,0);}
void set_phase(TitleInf& o,int value){o.phase=value;recovered::timer_set(o.age,0);}
void set_mode(game_session::Session& s,int value){if(s.mode!=2)gameplay::player_state::write(s.player_table,0x204,-1);s.mode=value;}
void set_character(game_session::Session& s,int value){s.contexts[0].current_player->fields_00[2]=value;}
}
