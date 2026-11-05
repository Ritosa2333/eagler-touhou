#pragma once
#include "../game_session/session.hpp"
#include "../progress_state/manager.hpp"
#include <functional>
namespace th20::source::gameplay {
void accumulate_playtime(game_session::Session&,progress::SaveManager&,int replay_mode,int scene,const std::function<double()>& clock); //4bce10
void accumulate_playtime();
}
