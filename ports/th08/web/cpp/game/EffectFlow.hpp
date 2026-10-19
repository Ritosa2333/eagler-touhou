#pragma once
#include "EffectSystem.hpp"
#include "BackgroundState.hpp"
namespace th08 {
struct EffectResources {
    virtual ~EffectResources()=default;
    virtual AnmLoaded* load(i32 index,const char* path)=0;
    virtual AnmLoaded* get(i32 index)=0;
    virtual void release(i32 index)=0;
};
struct EffectLoadContext {i32 stage=0,spell=0;bool practice=false,keep_resources=false;};
class EffectFlow {
    EffectSystem& effects;BackgroundState& background;EffectResources& resources;
    Chain* chain=nullptr;ChainElement calculation,drawing;
public:
    EffectLoadContext context;
    EffectFlow(EffectSystem& e,BackgroundState& b,EffectResources& r):effects(e),background(b),resources(r){}
    ~EffectFlow(){detach();}
    bool setup();void release();bool attach(Chain&);void detach();
};
}
