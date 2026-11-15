#pragma once
#include "Types.hpp"
#include <array>
#include <vector>
namespace th19 {
// Native evidence: 0x40f130 (clear), 0x537aa0 (weighted append),
// 0x40f110 (table lookup), 0x5378c0 / 0x518b7c (256 random bytes).
// Reproducing a native run requires its RNG state at table generation time.
class WaveSchedule {
public:
    void seed(u32 state){
        state%=2147483647u;if(!state)state=1;
        for(auto& draw:draws_){
            state=u32(u64(state)*48271u%2147483647u);
            draw=u8((state%0xffff00u)&255u);
        }
        ready_=true;cursor_=0;last_state_=state;
    }
    void set_draws(const std::array<u8,256>& draws){draws_=draws;ready_=true;cursor_=0;}
    void clear_pool(){pool_.clear();} // opcode 1010 does NOT reset the cursor
    void append(i32 wave,i32 weight){
        while(weight-->0&&pool_.size()<256)pool_.push_back(wave);
    }
    bool next(i32& wave){
        if(!ready_||pool_.empty())return false;
        wave=pool_[draws_[cursor_]%pool_.size()];cursor_=(cursor_+1)&255;return true;
    }
    const std::vector<i32>& pool()const{return pool_;}
    u32 cursor()const{return cursor_;}
    u32 rng_state()const{return last_state_;}
private:
    std::array<u8,256> draws_{};
    std::vector<i32> pool_;
    u32 cursor_=0,last_state_=0;
    bool ready_=false;
};
}
