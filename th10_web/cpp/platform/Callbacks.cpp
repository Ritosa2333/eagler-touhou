#include "Callbacks.hpp"
#include <cstdlib>
#include <cstdio>
#include <cstring>
namespace th10::browser {
i32 Callbacks::invoke(CallbackToken token,void* owner){++calls;
#ifdef TH_NATIVE_PLATFORM
    if(token<native.size()&&native[token].function)return native[token](owner);
    std::fprintf(stderr,"Unbound named game callback %u\n",token);std::abort();
#else
    const auto found=native.find(token);if(found!=native.end())return found->second(owner);
    i32 result=0;if(!receiver.invoke(token,owner,result))__builtin_trap();return result;
#endif
}
NativeCallback Callbacks::resolve(CallbackToken token){
#ifdef TH_NATIVE_PLATFORM
    if(token<native.size()&&native[token].function)return native[token];
    std::fprintf(stderr,"Unbound named update job %u\n",token);std::abort();
#else
    const auto found=native.find(token);if(found!=native.end())return found->second;
    return UpdateChainEnvironment::resolve(token);
#endif
}
UpdateChainEntry* Callbacks::allocate_entry(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    auto* entry=static_cast<UpdateChainEntry*>(rollback_entries.allocate(sizeof(UpdateChainEntry)));
    if(!entry)return nullptr; // Preserve the existing exhausted-pool result.
    // Metadata was captured at BeginFrame. An initially dormant slot still
    // needs its complete preimage before calloc semantics clear the payload.
    // A journal failure is sticky and rejects EndFrame; as with ANM first
    // writes, allocation itself remains non-null and may finish initialization.
    receiver.preserve_callback_entry(entry);
    std::memset(entry,0,sizeof(UpdateChainEntry));
    return entry;
#else
    return static_cast<UpdateChainEntry*>(std::calloc(1,sizeof(UpdateChainEntry)));
#endif
}
void Callbacks::release_entry(UpdateChainEntry* entry){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(entry&&!rollback_entries.release(entry))__builtin_trap();
#else
    std::free(entry);
#endif
}
}
