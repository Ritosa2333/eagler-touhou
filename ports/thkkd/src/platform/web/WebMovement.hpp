#pragma once
#include "core/frontend/ReplayGameplayInput.hpp"
namespace kaikeidou {
bool apply_web_movement(std::uint64_t& x,std::uint64_t& y,std::uint64_t old_x,std::uint64_t old_y,
    std::uint64_t speed,std::uint32_t state,std::uint32_t frame,std::uint32_t cursor,bool replay);
template<class Memory>ReplayGameplayInputOutcome web_update_replay_gameplay_movement(Memory& memory,PlayerShotFireInput& physical,bool replay_variant,unsigned precision){
    const auto old_x=memory.quad(0xa8cdf8),old_y=memory.quad(0xa8ce00);
    auto outcome=update_replay_gameplay_movement(memory,physical,replay_variant,precision);
    auto x=memory.quad(0xa8cdf8),y=memory.quad(0xa8ce00);
    const bool blocked=outcome.dead_return;
    const auto slow=outcome.input.slow&&!(outcome.input.slow>>31);
    if(apply_web_movement(x,y,old_x,old_y,memory.quad(slow?0xa8ce28:0xa8ce18),blocked?1:0,
        memory.word(0xb3e2e8),memory.word(0xb43110),memory.word(0xb4310c)!=0)){
        memory.quad(0xa8cdf8,x);memory.quad(0xa8ce00,y);memory.reconcile();
    }
    return outcome;
}
}
