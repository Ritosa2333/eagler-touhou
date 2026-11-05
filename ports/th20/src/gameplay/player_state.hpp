#pragma once
#include "../game_session/session.hpp"
#include <array>
#include <cstring>
namespace th20::source::gameplay::player_state {
using game_session::Player;
using game_session::PlayerTable;
// Offset access is confined to trivially copyable reconstructed storage. memcpy
// preserves representation without introducing misaligned or aliasing reads.
template<class T,class Object> T read(const Object& object,std::size_t offset) noexcept {
    T value;std::memcpy(&value,reinterpret_cast<const std::uint8_t*>(&object)+offset,sizeof(T));return value;
}
template<class T,class Object> void write(Object& object,std::size_t offset,T value) noexcept {
    std::memcpy(reinterpret_cast<std::uint8_t*>(&object)+offset,&value,sizeof(T));
}
struct Field {
    std::uint32_t address;
    std::uint16_t offset;
    std::uint8_t width;
    bool clamped;
    std::int32_t lower,upper;
};
// Each entry describes one original setter, not an inferred gameplay rule.
extern const std::array<Field,45> player_setters;
void set(Player&,const Field&,std::int32_t) noexcept;
void set(Player&,std::uint32_t original_address,std::int32_t);
class BombObserver {
public:
    virtual ~BombObserver()=default;
    virtual void update(std::int32_t bombs,std::int32_t fragments,std::int32_t maximum)=0; //4b8650
};
void set_bombs(Player&,std::int32_t,BombObserver*); //4e15e0, observer=null means actual nullable5c06a4
void reset_player(Player&,std::int32_t,BombObserver*); //4bbde0
void reset_players(PlayerTable&,BombObserver*); //4bbd80
void set_table_counter(PlayerTable&,std::size_t offset,std::int32_t,std::int32_t maximum) noexcept;
void set_meter(PlayerTable&,float) noexcept; //4bdfd0
std::int32_t stage(PlayerTable&) noexcept; //474d80, mutating clamp+1f4
std::int32_t previous_stage(PlayerTable&) noexcept; //4bd5c0, mutating clamp+1f8
std::int32_t spell(PlayerTable&) noexcept; //499480, mutating clamp+204
std::int32_t starting_power(Player&) noexcept; //4b81d0, mutating clamp+38
inline std::int32_t difficulty(const PlayerTable& table) noexcept {return static_cast<std::int32_t>(table.field_1e0);} //4640c0
inline std::uint64_t score(const Player& player) noexcept {return read<std::uint64_t>(player,0);} //44c090
}
