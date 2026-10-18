#pragma once
#include "Types.hpp"
namespace th10 {
// Callback tokens are resolved by the platform. The development adapter maps
// the executable's callback addresses; a standalone host can use a native table.
using CallbackToken=u32;
struct NativeCallback {
    using Function=i32(*)(void*,void*,i32);
    void* context=nullptr;Function function=nullptr;i32 parameter=0;
    i32 operator()(void* owner)const{return function(context,owner,parameter);}
};
struct UpdateChainEntry {
    i32 priority;
    u32 flags;
    CallbackToken callback,initialize_callback,notify_callback;
    ListNode<UpdateChainEntry> node;
    void* owner;
#ifdef TH_NATIVE_PLATFORM
    NativeCallback function,notification;
#endif
    void initialize() noexcept;
};
#ifndef TH_NATIVE_PLATFORM
static_assert(sizeof(UpdateChainEntry)==0x24);
#endif
struct UpdateChainEnvironment {
    u8* lock_depth;
    virtual void enter_lock()=0;
    virtual void leave_lock()=0;
    virtual i32 invoke(CallbackToken callback,void* owner)=0;
    virtual NativeCallback resolve(CallbackToken callback){return {this,[](void* p,void* owner,i32 token){return static_cast<UpdateChainEnvironment*>(p)->invoke(u32(token),owner);},i32(callback)};}
    virtual UpdateChainEntry* allocate_entry()=0;
    virtual void release_entry(UpdateChainEntry* entry)=0;
    void lock(){enter_lock();++*lock_depth;}
    void unlock(){leave_lock();--*lock_depth;}
};
struct UpdateChain {
    UpdateChainEntry update,draw;
    void initialize() noexcept;
    i32 insert(UpdateChainEntry& entry,i32 priority,bool drawing,UpdateChainEnvironment& environment);
    i32 run(bool drawing,UpdateChainEnvironment& environment);
    void remove(UpdateChainEntry* entry,UpdateChainEnvironment& environment);
    void remove_locked(UpdateChainEntry* entry,UpdateChainEnvironment& environment);
    UpdateChainEntry* add(CallbackToken callback,void* owner,i32 priority,bool drawing,bool unlocked,UpdateChainEnvironment& environment);
    void clear_list(UpdateChainEntry& sentinel,UpdateChainEnvironment& environment);
    static UpdateChainEntry* allocate(CallbackToken callback,UpdateChainEnvironment& environment);
};
#ifndef TH_NATIVE_PLATFORM
static_assert(sizeof(UpdateChain)==0x48);
#endif
}
