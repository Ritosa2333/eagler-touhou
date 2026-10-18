#pragma once
#include "gameplay.hpp"
namespace th20::source::player_entity {class Player;void reset_for_stage(Player&); /*4fb450*/}
namespace th20::source::gameplay {
int start_game_frame(GameController&); //4ba940
void complete_background_transition(GameController&); //4bbf90
void request_scene(int); //4a0fb0, event flag9 forces2
namespace unrecovered {void finish_replay_004e5f30();}
}
