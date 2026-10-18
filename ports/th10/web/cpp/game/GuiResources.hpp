#pragma once
#include "Gui.hpp"
#include "GameProgression.hpp"
namespace th10 {
struct PracticeState;
struct GuiResourceEnvironment {
#ifdef TH_ENABLE_THPRAC
    // Live practice run, used by th10_logo's stage-title suppression. Set by the
    // HUD owner (browser::Hud) from the session's practice state.
    const PracticeState* practice=nullptr;
#endif
    GameEconomy* game;
    Gui** current;
    const StageConfiguration** stage;
    u8** cached_message;
    float* rate;
    char* filename;
    const i32* current_screen;
    const u32* display_difficulty;
    const i32* controller_stage;
    AnmFile** effects;
    AnmFile** slots;
    AnmRegistry* registry;
    UpdateChain** chain;
    UpdateChainEnvironment* callbacks;
    CallbackToken update_callback,draw_callback;
    virtual Gui* allocate()=0;
    virtual AnmFile* load_animations(i32 slot,const char* name)=0;
    virtual void release_animations(AnmFile& file)=0;
    virtual void delete_object(void* object)=0;
    virtual void free_bytes(void* bytes)=0;
    virtual u8* read_file(const char* name)=0;
    virtual void report_error()=0;
    virtual u32 create_animation(AnmFile& file,i32 script)=0;
    virtual void initialize_animation(AnmFile& file,AnmVm& vm,i32 script)=0;
    virtual void bind_sprite(AnmFile& file,AnmVm& vm,i32 sprite)=0;
};
struct GuiResources {
    Gui& gui;
    GuiResourceEnvironment& environment;
    i32 load_stage();
    i32 start();
    void activate();
    void unload_stage();
    void discard_stage();
    void shutdown();
    static Gui* create(GuiResourceEnvironment& environment);
    static i32 preload(GuiResourceEnvironment& environment);
    static i32 release_front(GuiResourceEnvironment& environment);
};
}
