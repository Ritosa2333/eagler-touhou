#pragma once
#include "BackgroundScript.hpp"
#include "BackgroundView.hpp"
namespace th08 {
struct BackgroundResources {
    virtual ~BackgroundResources()=default;
    virtual AnmLoaded* load(i32 index,const char* path)=0;
    virtual AnmLoaded* get(i32 index)=0;
    virtual void release(i32 index)=0;
    virtual std::vector<u8> stage(const char* path)=0;
};
struct BackgroundLoadContext {bool keep_resources=false;AnmLoaded* text=nullptr;};
class BackgroundFlow {
public:
    BackgroundFlow(BackgroundScript& script,BackgroundView& view,BackgroundResources& resources):script(script),view(view),resources(resources){}
    ~BackgroundFlow(){detach();}
    BackgroundLoadContext context;
    bool setup();void release();bool attach(Chain& chain,i32 stage);void detach();
private:
    BackgroundScript& script;BackgroundView& view;BackgroundResources& resources;
    Chain* chain=nullptr;ChainElement calculation,high,low;
};
}
