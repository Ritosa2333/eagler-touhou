#pragma once
#include "EnemyManager.hpp"
#include "EclResources.hpp"
namespace th10 {
struct EnemySystemsEnvironment {
    UpdateChain* chain;
    UpdateChainEnvironment* callbacks;
    EnemyManagerEnvironment* enemies;
    EnemyScriptResourceEnvironment* resources;
    EnemyManager** active_enemies;
    AnmFile* bullet_animation_file;
    AnmFile** animation_slots;
    const u32* game_flags;
    const float* rate;
    void* program_type_table;
    void* generic_program_type_table;
    CallbackToken update_callback,draw_callback;
    virtual void* allocate(u32 bytes)=0;
    virtual void release(void* memory)=0;
    virtual void discard_file_animations(AnmFile* file)=0;
    virtual void unload_animation(AnmFile& file)=0;
};
}
