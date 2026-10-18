#include "GameState.hpp"
namespace th10::browser {
GameState::GameState(Input& input,bool language):current_stage(menu_data(language).stages),chinese(language){
    game.faith_timer.previous=-1;game.difficulty=1;
    background_color=0xff000000;
    configuration.initialize(reinterpret_cast<const u16*>(input.player_profiles[0].bindings));
}
}
