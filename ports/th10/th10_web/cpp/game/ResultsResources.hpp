#pragma once
#include "Results.hpp"
namespace th10 {
struct ResultsResourceEnvironment {
    Results** current;
    UpdateChain** chain;
    UpdateChainEnvironment* callbacks;
    CallbackToken update_callback,draw_callback;
    const float* rate;
    AnmRegistry* registry;
    virtual Results* allocate()=0;
    virtual void delete_object(Results* result)=0;
    virtual void delete_replay(Replay* replay)=0;
};
struct ResultsResources {
    Results& results;
    ResultsResourceEnvironment& environment;
    i32 start();
    void shutdown();
    static Results* create(ResultsResourceEnvironment& environment);
};
}
