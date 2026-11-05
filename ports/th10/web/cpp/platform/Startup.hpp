#pragma once
#include "GameState.hpp"
#include "Common.hpp"
#include "Scores.hpp"
#include "Audio.hpp"
#include "../game/StartupScreen.hpp"
namespace th10::browser {
struct Startup final : StartupEnvironment,CallbackReceiver {
    GameState& state;AnimationEngine& engine;FileSystem& files;Audio& audio;
    StartupScreen* value=nullptr;Common* shared=nullptr;Scores* scores=nullptr;
    CommonResources* common_value=nullptr;AnmFile* loading_file=nullptr;
    UpdateChain* update_chain;bool loading=false;i32 error=0;
    Startup(GameState&,AnimationEngine&,FileSystem&,Audio&);
    ~Startup();
    bool initialize();void advance_loading();
    void bind_callbacks(Callbacks&) override;
#ifndef TH_NATIVE_PLATFORM
    bool invoke(CallbackToken,void*,i32&) override;
#endif
    StartupScreen* allocate() override;
    void delete_object(void*) override;
    void free_bytes(void*) override;
    AnmFile* load_animations(i32,const char*) override;
    void release_animations(AnmFile&) override;
    bool create_common() override;
    void delete_common(CommonResources&) override;
    void create_scores() override;
    void save_scores() override;
    void delete_scores() override;
    u8* read_file(const char*) override;
    bool file_exists(const char*) override;
    void report(StartupError) override;
    void initialize_audio() override;
    void load_music(const char*) override;
    u32 create_opening_animation(AnmFile&) override;
    u32 create_loading_animation(AnmFile&) override;
    u32 begin_thread(CallbackToken,void*,u32,u32&) override;
    u32 wait_thread(u32,u32) override;
    void close_thread(u32) override;
    void sleep(u32) override;
};
}
