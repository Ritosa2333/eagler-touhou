#include "overlay.hpp"
#include "../gameplay/player_state.hpp"
namespace th20::source::overlay {
int selected_stone(const game_session::Player& player,int slot) noexcept {const unsigned offset=slot==1?0x14:slot==2?0x10:slot==3?0x18:0xc;return gameplay::player_state::read<int>(player,offset);}
std::uint8_t inherited_stone(const game_session::Player& player,int slot) noexcept {return player.bytes_2c[slot==1?2:slot==2?1:slot==3?3:0];}
bool main_active(const WeaponStoneInf& owner){return owner.main_weapon&&owner.main_weapon->phase_active();}
bool unfocused_active(const WeaponStoneInf& owner){return owner.unfocused_weapon&&owner.unfocused_weapon->unfocused_shooting();}
bool focused_active(const WeaponStoneInf& owner){return owner.focused_weapon&&owner.focused_weapon->focused_shooting();}
bool passive_active(const WeaponStoneInf& owner){return owner.passive_weapon&&owner.passive_weapon->passive_active();}
}
