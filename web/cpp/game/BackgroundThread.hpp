#pragma once
#include "UpdateChain.hpp"
namespace th10 {
struct BackgroundThreadEnvironment {
    virtual u32 begin_thread(CallbackToken callback,void* argument,u32 flags,u32& id)=0;
    virtual u32 wait_thread(u32 handle,u32 milliseconds)=0;
    virtual void close_thread(u32 handle)=0;
    virtual void sleep(u32 milliseconds)=0;
};
struct BackgroundThread {
    u32 original_virtual_table,handle,id,stop_requested,running;
    void* argument;
    CallbackToken callback;
    void stop(BackgroundThreadEnvironment& environment);
    void start(CallbackToken callback,void* argument,bool suspended,BackgroundThreadEnvironment& environment);
};
static_assert(sizeof(BackgroundThread)==0x1c);
}
