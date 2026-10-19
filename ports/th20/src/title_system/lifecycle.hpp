#pragma once
#include "title.hpp"
namespace th20::source::title {
struct LifecycleEnvironment {
    virtual ~LifecycleEnvironment()=default;
    virtual void publish(TitleInf*)=0;
    virtual scheduler::Node* register_callback(TitleInf&,int,bool)=0;
    virtual sprite::AnimationFile* load_file(int,const char*)=0;
    virtual void load_error()=0;
    virtual void remove(scheduler::Node*)=0;
    virtual void unload_file(int)=0;
    virtual void retire(runtime::CallbackOwner*)=0;
    virtual void interrupt(std::uint32_t)=0;
    virtual void release_mesh(sprite::RenderMesh*)=0;
    virtual void rebuild_input()=0;
};
LifecycleEnvironment& lifecycle_environment();
int initialize(TitleInf&,LifecycleEnvironment&); //51f3c0
}
