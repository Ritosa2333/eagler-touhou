#include "damage.hpp"
#include <cstring>
namespace th20::source::damage {
void add_score(game_session::Player& player,std::uint64_t amount) noexcept {std::uint64_t value;std::memcpy(&value,&player,8);value+=amount/10;if(value>999999999)value=999999999;std::memcpy(&player,&value,8);}
}
