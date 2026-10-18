#include "manager.hpp"
#include "../game_session/session.hpp"
namespace th20::source::progress {
Profile* current_profile(SaveManager& owner) noexcept {const auto& selected=*game_session::context(0).current_player;return find_profile(owner.current,read<int>(&selected,8),read<int>(&selected,12));}
Profile& fallback_profile(SaveManager& owner) noexcept {return owner.current.profiles[18];}
}
