#pragma once
#include "../game/UpdateChain.hpp"
#include <map>
#include <array>
#include "../game/CallbackNames.hpp"
namespace th10::browser {
struct Callbacks;
struct CallbackReceiver {
    void* callback_context=this;
#ifndef TH_NATIVE_PLATFORM
    virtual bool invoke(CallbackToken token,void* owner,i32& result)=0;
#endif
    virtual void bind_callbacks(Callbacks&){}
};
struct Callbacks final:UpdateChainEnvironment {
    CallbackReceiver& receiver;u8 depth=0;u32 calls=0;
    explicit Callbacks(CallbackReceiver& receiver):receiver(receiver){lock_depth=&depth;}
    using NativeFunction=NativeCallback::Function;
    using Binding=NativeCallback;
 #ifdef TH_NATIVE_PLATFORM
    std::array<Binding,callback_id::Count> native{};
    void bind(CallbackToken token,void* context,NativeFunction function,i32 parameter=0){if(token==0||token>=native.size())__builtin_trap();native[token]={context,function,parameter};}
    void unbind(void* context){for(auto& binding:native)if(binding.context==context)binding={};}
 #else
    std::map<CallbackToken,Binding> native;
    void bind(CallbackToken token,void* context,NativeFunction function,i32 parameter=0){native[token]={context,function,parameter};}
    void unbind(void* context){for(auto it=native.begin();it!=native.end();)if(it->second.context==context)it=native.erase(it);else ++it;}
 #endif
    void enter_lock() override {}
    void leave_lock() override {}
    i32 invoke(CallbackToken,void*) override;
    NativeCallback resolve(CallbackToken) override;
    UpdateChainEntry* allocate_entry() override;
    void release_entry(UpdateChainEntry*) override;
};
}
