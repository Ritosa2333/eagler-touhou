#pragma once
#include "AnmFile.hpp"
#include "UpdateChain.hpp"
namespace th10 {
struct CommonResourceEnvironment;
struct AsciiRenderEnvironment {
    virtual void select_camera(bool world)=0;
    virtual void draw_character(AnmVm& animation,bool pixel_aligned)=0;
};
struct AsciiText {
    char text[64];
    Vec3 position;
    u32 color;
    Vec2 scale;
    u32 reserved_058;
    i32 camera,font,shadow;
};
static_assert(sizeof(AsciiText)==0x68);
struct CommonResources {
    CallbackToken virtual_table;
    u32 flags,state;
    UpdateChainEntry *update_entry,*draw_entry;
    AnmVm characters,small_characters;
    AsciiText text[256],early_text[64];
    i32 text_count,early_text_count;
    u32 color;
    Vec2 scale;
    i32 camera,reserved_8984,shadow,character_width,frames;
    AnmFile *effects,*capture,*text_animations;
    u32 introduction_animation,loading_animation;
    UpdateChainEntry* early_draw_entry;
    void initialize(CommonResources** current) noexcept;
    i32 start(CommonResourceEnvironment& environment);
    void shutdown(CommonResourceEnvironment& environment);
    static CommonResources* create(CommonResourceEnvironment& environment);
    i32 update() noexcept;
    void enable() noexcept;
    void queue(const char* value,const Vec3& position,bool early) noexcept;
    void mark_small() noexcept;
    i32 draw(bool early,AsciiRenderEnvironment& environment);
};
static_assert(offsetof(CommonResources,characters)==0x14);
static_assert(offsetof(CommonResources,text)==0x76c);
static_assert(offsetof(CommonResources,early_text)==0x6f6c);
static_assert(offsetof(CommonResources,text_count)==0x896c);
static_assert(offsetof(CommonResources,effects)==0x8994);
static_assert(sizeof(CommonResources)==0x89ac);
struct CommonResourceEnvironment {
    CommonResources** current;
    UpdateChain** chain;
    UpdateChainEnvironment* callbacks;
    CallbackToken virtual_table,update_callback,draw_callback,early_draw_callback;
    AnmFile** slots;
    const char *effects_name,*text_name,*capture_name;
    virtual CommonResources* allocate()=0;
    virtual void delete_object(void* object)=0;
    virtual void free_geometry(void* geometry)=0;
    virtual AnmFile* load_animations(i32 slot,const char* name)=0;
    virtual void release_animations(AnmFile& file)=0;
    virtual void bind_sprite(AnmFile& file,AnmVm& animation,i32 sprite)=0;
    virtual void report_error()=0;
};
}
