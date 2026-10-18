#pragma once
#include "EclProgram.hpp"
#include "AnmFile.hpp"
namespace th10 {
struct EclResourceEnvironment {
    virtual void* allocate(u32 bytes)=0;
    virtual void release(void* memory)=0;
    virtual u8* read_file(const char* name)=0;
    virtual i32 process_header(EclProgram& program,const u8* data)=0;
};
struct EnemyScriptResourceEnvironment : EclResourceEnvironment {
    AnmFile** enemy_animations;
    virtual AnmFile* load_animation(u32 slot,const char* name)=0;
    virtual void missing_animation()=0;
    i32 process_header(EclProgram& program,const u8* data) override;
};
}
