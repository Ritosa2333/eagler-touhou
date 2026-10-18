#pragma once
#include "Stage.hpp"
namespace th10 {
enum class StageResourceError {Animation,StageStart};
struct StageResourceEnvironment {
    Stage** background;
    Stage** overlay;
    const i32* stage_number;
    const u8* game_flags;
    const Camera* world;
    float* rate;
    char* filename;
    AnmFile** animation_slots;
    UpdateChain* chain;
    UpdateChainEnvironment* callbacks;
    CallbackToken update_callback,background_callback,foreground_callback;
    virtual Stage* allocate_stage()=0;
    virtual void* allocate_bytes(u32 bytes)=0;
    virtual void release_memory(void* memory)=0;
    virtual u8* read_file(const char* name,u32* size)=0;
    virtual AnmFile* load_animations(i32 slot,const char* name)=0;
    virtual void release_animations(AnmFile& file)=0;
    virtual void report(StageResourceError error)=0;
};
struct StageResources {
    Stage& stage;StageResourceEnvironment& environment;
    i32 load(const char* name);
    i32 start(const char* name,i32 priority_offset);
    void release();
    static Stage* create(const char* name,i32 priority_offset,StageResourceEnvironment& environment);
};
}
