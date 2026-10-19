#include "menu_support.hpp"
#include "../progress_state/records.hpp"
#include <algorithm>
namespace th20::source::pause {namespace pr=progress;
int continue_count(game_session::Session& session){auto& value=session.player_table.continue_count;value=std::clamp(value,0,9);return value;}
int credits(const game_session::Session& session){return pr::read<int>(&session,0x78);}
const char* name_characters(){return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*   ";}
}
