#pragma once
#include "Replay.hpp"
namespace th10 {
struct ReplayResourceEnvironment {
    ReplayEnvironment* gameplay;
    Replay** current;
    u8* configuration;
    UpdateChain** chain;
    UpdateChainEnvironment* callbacks;
    CallbackToken input_callback,end_frame_callback,draw_callback;
    virtual i32 load(Replay& replay,const char* name)=0;
};
struct ReplayResources {
    Replay& replay;
    ReplayResourceEnvironment& environment;
    i32 start(i32 mode,const char* name);
    void shutdown();
    static Replay* create(i32 mode,const char* name,ReplayResourceEnvironment& environment);
};
}
