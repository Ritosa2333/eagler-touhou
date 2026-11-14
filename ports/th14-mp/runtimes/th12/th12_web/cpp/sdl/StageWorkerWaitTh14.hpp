#pragma once
#include "../game/Types.hpp"
namespace th11::sdl {
// The minimum wait belongs to the dispatched worker, independent of when the
// main event loop gets to deliver its completion callback.
struct StageWorkerWaitTh14 {
    static constexpr u64 minimum_ns=60000000;
    u64 generation=0,started_ns=0;
    bool waiting=false,dispatched=false;
    void begin(bool initial){++generation;waiting=!initial;dispatched=false;}
    bool owns(u64 token)const{return token==generation&&waiting;}
    bool start(u64 token,u64 now){
        if(!owns(token)||dispatched)return false;
        started_ns=now;dispatched=true;return true;
    }
    bool ready(u64 now)const{
        return !waiting||(dispatched&&now>=started_ns&&now-started_ns>=minimum_ns);
    }
    u64 remaining(u64 now)const{
        if(!dispatched||now<started_ns)return minimum_ns;
        const u64 elapsed=now-started_ns;
        return elapsed<minimum_ns?minimum_ns-elapsed:0;
    }
    bool finish(u64 token,u64 now){
        if(!owns(token)||!ready(now))return false;
        waiting=false;dispatched=false;return true;
    }
    void cancel(){++generation;waiting=false;dispatched=false;}
};
}
