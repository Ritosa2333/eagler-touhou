#pragma once
#include "Types.hpp"
namespace th15 {
enum class FrameAction:i32 {
    Remove=0,Continue=1,Repeat=2,StopSuccess=3,Stop=4,Error=5,
    Restart=6,Cleanup=7,StopAlternate=8
};
enum class FramePass {Update,Draw};
class FrameScheduler;
struct FrameCallback {
    using Function=i32(*)(void*);
    Function run=nullptr,initialize=nullptr,cleanup=nullptr;void* owner=nullptr;
    bool enabled=false;
    FrameCallback()=default;
    FrameCallback(const FrameCallback&)=delete;FrameCallback& operator=(const FrameCallback&)=delete;
private:
    friend class FrameScheduler;
    FrameCallback* previous=nullptr;FrameCallback* next=nullptr;
    FrameScheduler* scheduler=nullptr;i32 priority=0;FramePass pass=FramePass::Update;
};
// Stable, externally owned callbacks avoid allocations during a game frame.
// Registration, removal and return actions preserve the original live traversal.
class FrameScheduler {
    FrameCallback* updates=nullptr;FrameCallback* draws=nullptr;
    FrameCallback* continuation=nullptr;
    i32 execute(FramePass);
public:
    bool closing=false;
    FrameScheduler()=default;
    FrameScheduler(const FrameScheduler&)=delete;FrameScheduler& operator=(const FrameScheduler&)=delete;
    ~FrameScheduler(){clear(FramePass::Update);clear(FramePass::Draw);}
    i32 add(FrameCallback&,FramePass,i32 priority);
    void remove(FrameCallback&)noexcept;void clear(FramePass)noexcept;
    i32 update(){return execute(FramePass::Update);}i32 draw(){return execute(FramePass::Draw);}
    bool contains(const FrameCallback& c)const noexcept{return c.scheduler==this;}
};
}
