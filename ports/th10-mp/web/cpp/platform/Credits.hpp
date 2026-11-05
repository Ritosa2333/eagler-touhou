#pragma once
#include "World.hpp"
#include "Captures.hpp"
#include "../game/Ending.hpp"
namespace th10::browser {
struct Credits final:EndingEnvironment,CallbackReceiver {
    World& owner;Captures& captures;Ending* value=nullptr;EndingScript* pending_loader=nullptr;
    char filename_storage[260]{},decoded_storage[2048]{};i32 error=0;
    Credits(World&,Captures&);~Credits();bool initialize();void advance_loading();
    void bind_callbacks(Callbacks&) override;
#ifndef TH_NATIVE_PLATFORM
    bool invoke(CallbackToken,void*,i32&) override;
#endif
    void* allocate(u32) override;void delete_object(void*) override;void free_bytes(void*) override;
    u8* read_file(const char*) override;void report_error() override;void show_loading() override;
    u32 create_animation(AnmFile&,i32) override;void draw_text(AnmVm*,u32,const char*) override;
    void sound(i32) override;void fade(i32,i32) override;void load_music(const char*) override;
    void play_music(i32) override;void fade_music(float) override;
    AnmFile* load_animations(i32,const char*) override;void unload_animations(i32) override;
    void release_animations(AnmFile&) override;void release_capture() override;
    u32 begin_thread(CallbackToken,void*,u32,u32&) override;u32 wait_thread(u32,u32) override;
    void close_thread(u32) override;void sleep(u32) override;
};
}
