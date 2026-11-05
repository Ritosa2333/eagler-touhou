#include "Callbacks.hpp"
#include <cstdlib>
#include <cstdio>
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
UpdateChainEntry* Callbacks::allocate_entry(){return static_cast<UpdateChainEntry*>(std::calloc(1,sizeof(UpdateChainEntry)));}
void Callbacks::release_entry(UpdateChainEntry* entry){std::free(entry);}
}
