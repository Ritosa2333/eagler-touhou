#pragma once
#include "AnmLayout.hpp"
#include <cstddef>
namespace th08 {
struct AnmLoaded {
    i32 anmIdx=0;void* rawData=nullptr;i32 totalEntries=0;
    AnmLoadedSprite* sprites=nullptr;AnmRawInstr** scripts=nullptr;void* textures=nullptr;
    i32 numberEntriesToBeLoaded=0;
    u32 spriteCount=0,scriptCount=0;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // Runtime ownership follows the shared file, including independent ANM
    // executors. These trailing hooks are never journaled or hashed.
    void* script_write_context=nullptr;
    bool (*script_before_write)(void*,const AnmLoaded&,void*,std::size_t)=nullptr;
    bool BeforeScriptWrite(void* address,std::size_t bytes)const {
        return !script_before_write||script_before_write(script_write_context,*this,address,bytes);
    }
    void ClearScriptWriteHook(){script_before_write=nullptr;script_write_context=nullptr;}
#endif
    i32 SetSprite(AnmVm* vm,i32 index) noexcept;
};
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
inline constexpr std::size_t anm_loaded_state_bytes=offsetof(AnmLoaded,script_write_context);
#endif
class AnmExecutor {
public:
    explicit AnmExecutor(Rng& random):rng(random){}
    FrameTiming timing;
    u32 executed=0;
    bool invalid=false;
    bool execute(AnmVm& vm);
    void start(AnmLoaded& file,AnmVm& vm,AnmRawInstr* script);
private:
    Rng& rng;
    void advance(AnmVm& vm);
};
}
