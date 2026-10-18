#pragma once
#include "AnmLayout.hpp"
namespace th09 {
struct AnmLoaded {
    i32 anmIdx=0;void* rawData=nullptr;i32 totalEntries=0;
    AnmLoadedSprite* sprites=nullptr;AnmRawInstr** scripts=nullptr;void* textures=nullptr;
    i32 numberEntriesToBeLoaded=0;
    u32 spriteCount=0,scriptCount=0;
    i32 SetSprite(AnmVm* vm,i32 index) noexcept;
};
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
