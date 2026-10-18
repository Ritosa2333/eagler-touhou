#include "stage_clear.hpp"
#include "../gameplay/player_state.hpp"
#include <algorithm>
#include <stdexcept>
namespace th20::source::stage_clear {
namespace ps=gameplay::player_state;
namespace {
int clamp(game_session::Player& player,unsigned offset,int max){const auto value=std::clamp(ps::read<int>(player,offset),0,max);ps::write(player,offset,value);return value;}
}
int clamped_level(game_session::Player& player,unsigned slot){if(slot>=4)throw std::out_of_range("StageClear level slot");return clamp(player,0x84+4*slot,999999);}
int clamped_phase(game_session::Player& player,unsigned slot){if(slot>=4)throw std::out_of_range("StageClear phase slot");return clamp(player,0x74+4*slot,4);}
std::uint32_t calculate_bonus(game_session::Player& player,int stage){
    std::uint32_t result=static_cast<unsigned>(stage)*100000u;
    const auto power=static_cast<unsigned>(clamp(player,0x44,1000000));
    // All operations use x86 low-word arithmetic, including overflow before idiv.
    unsigned levels=static_cast<unsigned>(clamped_level(player,0));levels+=clamped_level(player,1);levels+=clamped_level(player,3);levels+=clamped_level(player,2);result+=power*levels;
    unsigned phases=static_cast<unsigned>(clamped_phase(player,0));phases+=clamped_phase(player,1);phases+=clamped_phase(player,3);phases+=clamped_phase(player,2);result+=phases*100000u;
    result-=static_cast<unsigned>(recovered::signed_bits(result)%10);return result;
}
}
