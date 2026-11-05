#pragma once
#include "BackgroundThread.hpp"
#include "CommonResources.hpp"
namespace th10 {
struct StartupEnvironment;
struct StartupScreen {
    u32 flags,state;
    UpdateChainEntry *update_entry,*draw_entry;
    BackgroundThread loader;
    u32 reserved_02c;
    AnmVm animation;
    u32 opening_animation;
    AnmFile* opening_file;
    i32 opening_ready,resources_ready,elapsed;
    void initialize(StartupScreen** current) noexcept;
    i32 start(StartupEnvironment& environment);
    void shutdown(StartupEnvironment& environment);
    static StartupScreen* create(StartupEnvironment& environment);
    i32 load(StartupEnvironment& environment);
    i32 update(StartupEnvironment& environment);
    i32 draw(StartupEnvironment& environment);
    static i32 load_stage_assets(StartupEnvironment& environment);
    static i32 release_stage_assets(StartupEnvironment& environment);
};
static_assert(offsetof(StartupScreen,animation)==0x30&&offsetof(StartupScreen,opening_file)==0x3e0);
static_assert(offsetof(StartupScreen,elapsed)==0x3ec&&sizeof(StartupScreen)==0x3f0);
enum class StartupError { CommonResources,MusicFormat,FrontAnimations,BulletAnimations };
struct StartupEnvironment : BackgroundThreadEnvironment {
    StartupScreen** current;
    CommonResources** common;
    UpdateChain** chain;
    UpdateChainEnvironment* callbacks;
    CallbackToken update_callback,draw_callback,loader_callback,thread_vtable;
    AnmFile **slots,**loading_animations;
    u32* engine_flags;
    const u32* display_flags;
    i32* pending_screen;
    u8** music_format;
    char* music_filename;
    const char *opening_name,*text_name,*format_name,*music_name,*front_name,*bullet_name;
    virtual StartupScreen* allocate()=0;
    virtual void delete_object(void* object)=0;
    virtual void free_bytes(void* bytes)=0;
    virtual AnmFile* load_animations(i32 slot,const char* filename)=0;
    virtual void release_animations(AnmFile& file)=0;
    virtual bool create_common()=0;
    virtual void delete_common(CommonResources& resources)=0;
    virtual void create_scores()=0;
    virtual void save_scores()=0;
    virtual void delete_scores()=0;
    virtual u8* read_file(const char* filename)=0;
    virtual bool file_exists(const char* filename)=0;
    virtual void report(StartupError error)=0;
    virtual void initialize_audio()=0;
    virtual void load_music(const char* filename)=0;
    virtual u32 create_opening_animation(AnmFile& file)=0;
    virtual u32 create_loading_animation(AnmFile& file)=0;
    void release_slot(i32 slot);
};
}
