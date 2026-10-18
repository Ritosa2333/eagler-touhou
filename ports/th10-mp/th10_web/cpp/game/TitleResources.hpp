#pragma once
#include "TitleMenu.hpp"
#include "StartupScreen.hpp"
namespace th10 {
struct TitleResourceEnvironment : BackgroundThreadEnvironment {
    TitleMenu** current;
    StartupScreen** startup;
    const u32* engine_flags;
    i32 *pending_screen,*menu_state;
    AnmFile** slots;
    AnmRegistry* registry;
    UpdateChain** chain;
    UpdateChainEnvironment* callbacks;
    CallbackToken update_callback,draw_callback,loader_callback,title_vtable,thread_vtable;
    virtual TitleMenu* allocate()=0;
    virtual void delete_object(void* object)=0;
    virtual AnmFile* load_animations(i32 slot,const char* filename)=0;
    virtual void release_animations(AnmFile& file)=0;
    virtual void delete_replay(Replay* replay)=0;
    virtual void free_file(void* file)=0;
    virtual void report_error()=0;
    // Native retains its authored intro. Browser adapters may finish as soon
    // as startup has actually presented and the title resources are ready.
    virtual bool startup_presentation_complete(const StartupScreen& screen)const{return screen.elapsed>=300;}
};
struct TitleResources {
    TitleMenu& title;
    TitleResourceEnvironment& environment;
    i32 start();
    void shutdown();
    static TitleMenu* create(TitleResourceEnvironment& environment);
    static i32 load(TitleResourceEnvironment& environment);
};
struct TitleLoadingTask {
    bool started=false,waiting_for_startup=false,done=false;
    bool advance(TitleResourceEnvironment& environment);
};
}
