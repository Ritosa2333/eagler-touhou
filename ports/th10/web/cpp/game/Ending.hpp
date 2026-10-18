#pragma once
#include "Dialogue.hpp"
#include "BackgroundThread.hpp"
#include "ScoreData.hpp"
namespace th10 {
struct EndingEnvironment;
struct EndingScript {
    u32 reserved_000;
    Timer elapsed;u32 elapsed_flags;
    Timer script_time;u32 script_time_flags;
    Timer wait;u32 wait_flags;
    u32 lines[5];
    MessageInstruction* instruction;
    u32 reserved_058[6];
    const char* pending_animation_name;
    u32 flags; i32 next_line;u32 color;
    AnmFile* files[4];
    u32 animations[16];
    BackgroundThread loader;
    void initialize(MessageInstruction* script,EndingEnvironment& environment);
    void release(EndingEnvironment& environment);
    i32 update(EndingEnvironment& environment);
    i32 tick(EndingEnvironment& environment);
    i32 load_animations(EndingEnvironment& environment);
};
static_assert(sizeof(EndingScript)==0xec&&offsetof(EndingScript,instruction)==0x54&&offsetof(EndingScript,loader)==0xd0);
struct Ending {
    u32 flags,manager_state;
    UpdateChainEntry *update_entry,*draw_entry;
    u32 reserved_010;
    u8* file;
    EndingScript* script;
    i32 ending;
    u32 newly_unlocked;
    i32 frames;
    void initialize(Ending** current) noexcept;
    i32 start(EndingEnvironment& environment);
    void shutdown(EndingEnvironment& environment);
    u8* load_file(const char* name,EndingEnvironment& environment);
    i32 update(EndingEnvironment& environment);
    static Ending* create(EndingEnvironment& environment);
};
static_assert(sizeof(Ending)==0x28);
struct EndingEnvironment : BackgroundThreadEnvironment {
    Ending** current;
    GameEconomy* game;
    ScoreData** scores;
    UpdateChain** chain;
    UpdateChainEnvironment* callbacks;
    CallbackToken update_callback,draw_callback,loader_callback;
    u32 thread_vtable;
    AnmRegistry* registry;
    AnmFile** text_animations;
    AnmFile** animation_slots;
    const char* const* ending_files;
    char *filename,*decoded_text;
    u32* loading_animation;
    const u32 *engine_flags,*held;
    const u16* pressed;
    i32 *pending_screen,*menu_state;
    float* rate;
    virtual void* allocate(u32 bytes)=0;
    virtual void delete_object(void* object)=0;
    virtual void free_bytes(void* bytes)=0;
    virtual u8* read_file(const char* name)=0;
    virtual void report_error()=0;
    virtual void show_loading()=0;
    virtual u32 create_animation(AnmFile& file,i32 script)=0;
    virtual void draw_text(AnmVm* vm,u32 color,const char* text)=0;
    virtual void sound(i32 sound)=0;
    virtual void fade(i32 kind,i32 frames)=0;
    virtual void load_music(const char* name)=0;
    virtual void play_music(i32 track)=0;
    virtual void fade_music(float seconds)=0;
    virtual AnmFile* load_animations(i32 slot,const char* name)=0;
    virtual void unload_animations(i32 slot)=0;
    virtual void release_animations(AnmFile& file)=0;
    virtual void release_capture()=0;
};
}
