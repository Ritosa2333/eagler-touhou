#include "player_state.hpp"
#include <algorithm>
#include <stdexcept>
#include <xmmintrin.h>
namespace th20::source::gameplay::player_state {
const std::array<Field,45> player_setters={{
    {0x4be340,0xec,4,false,0,0},{0x4be0a0,0x30,4,true,0,400},
    {0x4be140,0x34,4,true,400,400},{0x4be0f0,0x38,4,true,100,400},
    {0x4bd9e0,0xd4,4,true,2,10},{0x4bda80,0xd0,4,true,0,10},
    {0x4bdf30,0xc0,4,true,0,10},{0x4bdbf0,0xc4,4,true,0,100},
    {0x4bdc60,0xe4,4,true,0,99999999},{0x4be940,0x4c,4,true,0,500},
    {0x4bea00,0x50,4,true,100,500},{0x4be9b0,0x54,4,true,1,100},
    {0x4bdd10,0x9c,4,true,0,100},{0x4bec10,0xa0,4,true,0,100},
    {0x4be990,0xa4,1,false,0,0},{0x4be190,0xa5,1,false,0,0},
    {0x4beaf0,0xa8,4,true,0,100},{0x4bdc40,0xb0,1,false,0,0},
    {0x4bea50,0x44,4,true,0,1000000},{0x4bdf80,0x3c,4,true,0,1000000},
    {0x4be050,0x40,4,true,10000,1000000},{0x4beaa0,0x48,4,true,5000,10000},
    {0x4be440,0xac,4,true,0,100},{0x4be620,0x5c,4,true,0,10000},
    {0x4be760,0x64,4,true,0,1000},{0x4be670,0x68,4,true,0,1000},
    {0x4be7b0,0x6c,4,true,0,1000},{0x4be6c0,0x70,4,true,0,1000},
    {0x4be710,0x60,4,true,0,5000},{0x4be8a0,0x74,4,true,0,4},
    {0x4be800,0x78,4,true,0,4},{0x4be8f0,0x7c,4,true,0,4},
    {0x4be850,0x80,4,true,0,4},{0x4be530,0x84,4,true,0,999999},
    {0x4be490,0x88,4,true,0,999999},{0x4be5d0,0x8c,4,true,0,999999},
    {0x4be4e0,0x90,4,true,0,999999},{0x4be580,0x94,4,true,0,999999},
    {0x4bdee0,0xbc,4,true,0,7},{0x4bda30,0xd8,4,true,0,7},
    {0x4bde90,0xb8,4,true,-1,7},
    {0x41de70,0x1c,4,false,0,0},{0x41df50,0x24,4,false,0,0},
    {0x41df30,0x20,4,false,0,0},{0x412d10,0x28,4,false,0,0}
}};
void set(Player& player,const Field& field,std::int32_t value) noexcept {
    if(field.clamped) value=std::clamp(value,field.lower,field.upper);
    if(field.width==1) write(player,field.offset,static_cast<std::uint8_t>(value));
    else write(player,field.offset,value);
}
void set(Player& player,std::uint32_t address,std::int32_t value) {
    for(const auto& field:player_setters) if(field.address==address) {set(player,field,value);return;}
    throw std::logic_error("Unknown recovered player setter address");
}
static std::int32_t clamp_field(Player& player,std::size_t offset,std::int32_t lower,std::int32_t upper) noexcept {
    const auto value=std::clamp(read<std::int32_t>(player,offset),lower,upper);write(player,offset,value);return value;
}
void set_bombs(Player& player,std::int32_t value,BombObserver* observer) {
    write(player,0xcc,std::max(value,0));
    auto maximum=clamp_field(player,0xd8,0,7); //4b7e20 mutates maximum even without observer
    if(maximum<read<std::int32_t>(player,0xcc)) {
        maximum=clamp_field(player,0xd8,0,7);write(player,0xcc,maximum);
    }
    if(observer) {
        maximum=clamp_field(player,0xd8,0,7);
        const auto fragments=clamp_field(player,0xd0,0,10);
        const auto bombs=clamp_field(player,0xcc,0,10);
        observer->update(bombs,fragments,maximum);
    }
}
void reset_player(Player& player,std::int32_t index,BombObserver* observer) {
    set(player,0x4be340,index);set(player,0x4be0a0,0);set(player,0x4be140,400);
    set(player,0x4be0f0,100);set(player,0x4bd9e0,2);
    set_bombs(player,clamp_field(player,0xd4,2,10),observer);
    // Original order matters: HUD observer sees the previous fragments before
    // 4bda80 clears them. Untouched score, loadout, lives and padding survive.
    constexpr std::int32_t values[]={0,0,0,0,0,100,1,0,0,0,0,0,0,0,0,10000,5000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    static_assert(std::size(values)==33);
    for(std::size_t i=0;i<std::size(values);++i) set(player,player_setters[i+5],values[i]);
}
void reset_players(PlayerTable& table,BombObserver* observer) {
    write(table,0x210,std::uint32_t{0});write(table,0x208,std::uint32_t{0});
    for(int index=0;index<2;++index) reset_player(table.players[index],index,observer);
}
void set_table_counter(PlayerTable& table,std::size_t offset,std::int32_t value,std::int32_t maximum) noexcept {
    write(table,offset,std::clamp(value,0,maximum));
}
void set_meter(PlayerTable& table,float value) noexcept {
    const auto scaled=_mm_mul_ss(_mm_set_ss(value),_mm_set_ss(100.0f));
    auto result=_mm_cvtt_ss2si(scaled); // preserves x86 INT_MIN result for NaN/overflow
    const auto upper=read<std::int32_t>(table,0x21c),lower=read<std::int32_t>(table,0x218);
    if(result<upper) {if(result<lower)result=lower;} else result=upper;
    write(table,0x214,result);
}
std::int32_t stage(PlayerTable& table) noexcept {
    const auto result=std::clamp(read<std::int32_t>(table,0x1f4),0,999);write(table,0x1f4,result);return result;
}
std::int32_t spell(PlayerTable& table) noexcept {
    const auto result=std::clamp(read<std::int32_t>(table,0x204),-1,9999);write(table,0x204,result);return result;
}
std::int32_t previous_stage(PlayerTable& table) noexcept {
    const auto result=std::clamp(read<std::int32_t>(table,0x1f8),0,999);write(table,0x1f8,result);return result;
}
std::int32_t starting_power(Player& player) noexcept {return clamp_field(player,0x38,100,400);}
}
