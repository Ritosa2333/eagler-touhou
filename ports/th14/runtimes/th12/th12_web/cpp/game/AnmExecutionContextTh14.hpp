#pragma once
#include "PortTitle.hpp"
#if PORT_TITLE == 14
namespace th11 {
// One actual loading callback owns its execution context until return/join.
// Retail435ac8 executes FNINIT on that thread, not on the D3D/Main thread.
// Keep Main's addressable texture precision flag and ShotManager stack pin.
class AnmExecutionContextTh14 {
    const void* owner;
    bool high;
    AnmExecutionContextTh14* previous;
#if defined(TH14_CPP_STAGE_WORKER) && defined(_REENTRANT)
    static inline thread_local AnmExecutionContextTh14* active=nullptr;
#else
    static inline AnmExecutionContextTh14* active=nullptr;
#endif
public:
    explicit AnmExecutionContextTh14(const void* environment,bool fninit_high=true)noexcept:
        owner(environment),high(fninit_high),previous(active){active=this;}
    ~AnmExecutionContextTh14(){active=previous;}
    AnmExecutionContextTh14(const AnmExecutionContextTh14&)=delete;
    AnmExecutionContextTh14& operator=(const AnmExecutionContextTh14&)=delete;
    static bool& precision(const void* environment,bool& main)noexcept{
        for(auto* scope=active;scope;scope=scope->previous)
            if(scope->owner==environment)return scope->high;
        return main;
    }
    static bool owns(const void* environment)noexcept{
        for(auto* scope=active;scope;scope=scope->previous)
            if(scope->owner==environment)return true;
        return false;
    }
};
}
#endif
