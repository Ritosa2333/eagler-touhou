#pragma once
#include "GameState.hpp"
#include "Common.hpp"
#include "Fonts.hpp"
#include "Audio.hpp"
#include "Scores.hpp"
#include "ReplayFiles.hpp"
#include "ScreenEffects.hpp"
#include "../game/AudioGame.hpp"
#include "../game/TitleResources.hpp"
#include "../game/TitleLoop.hpp"
#include "../game/TitleKeys.hpp"
#include "../game/TitleOptions.hpp"
#include "../game/TitleScores.hpp"
#include "../game/TitleReplays.hpp"
#include "../game/TitleClear.hpp"
#include "../game/TitleDraw.hpp"
#include "../game/MusicRoom.hpp"
namespace th10::browser {
struct Title;
template<class Base> struct MenuMain : Base {
    Title& owner;
    explicit MenuMain(Title&);
    u32 create(AnmFile&,i32) override;
    void sound(i32) override;
    void interrupt_immediately(u32,i32) override;
};
template<class Base> struct MenuSelection : MenuMain<Base> {
    explicit MenuSelection(Title&);
    void show_loading(float,float) override;
    void hide_screen() override;
    void fade_music(float) override;
    bool read_keyboard() override;
};
struct MenuKeys final : MenuMain<TitleKeysEnvironment> {
    explicit MenuKeys(Title&);
    const u8* buttons() override;
    void bind_digit(AnmVm&,i32) override;
};
struct MenuOptions final : MenuMain<TitleOptionsEnvironment> {
    explicit MenuOptions(Title&);
    void apply_music_volume() override;
    void bind_digit(AnmVm&,i32) override;
};
struct MenuScores final : MenuMain<TitleScoreEnvironment> {
    explicit MenuScores(Title&);
    bool read_keyboard() override;
    void text(AnmVm*,u32,const char*,const u32*,u32) override;
};
struct MenuReplays final : MenuSelection<TitleReplayEnvironment> {
    u32 search_index=0;bool searching=false;char pattern[64]{};
    explicit MenuReplays(Title&);
    Replay* preview(const char*) override;
    void delete_replay(Replay*) override;
    void make_directory(const char*) override {}
    void change_directory(const char*) override {} // Preview and search paths are rooted explicitly.
    u32 find_first(const char*,ReplaySearchEntry&) override;
    bool find_next(u32,ReplaySearchEntry&) override;
    void find_close(u32) override;
#ifdef TH_ENABLE_THPRAC
    void reset_practice() override;
    void check_practice(const char*) override;
    void activate_practice() override;
#endif
};
struct MenuMusic final : MusicRoomEnvironment {
    Title& owner;
    explicit MenuMusic(Title&);
    u32 create(AnmFile&,i32) override;
    char* read_file(i32&) override;
    void free_file(char*) override;
    void text(AnmVm&,u32,const char*) override;
    void numbered_text(AnmVm&,u32,i32,const char*) override;
    void locked_text(AnmVm&,u32,i32) override;
    void sound(i32) override;
    void music_command(i32) override;
    void load_music(const char*) override;
    void play_music() override;
};
struct MenuDraw final : ResultsDrawEnvironment {
    Title& owner;
    explicit MenuDraw(Title&);
    ReplayDate local_date(i32) override;
    void text(const Vec3&,const char*,const u32*,u32) override;
};
struct MenuClear final : MenuMain<TitleClearEnvironment> {
    explicit MenuClear(Title&);
    void play_music(bool) override;
    void timestamp(i32&) override;
    Replay* preview(const char*) override;
    void delete_replay(Replay*) override;
    void save_replay(const char*,const char*) override;
};
struct MenuLoop final : TitleLoopEnvironment {
    Title& owner;
    explicit MenuLoop(Title&);
    u32 create(AnmFile&,i32) override;
    Replay* load_replay(const char*) override;
    void delete_replay(Replay*) override;
    void play_title_music() override;
    void stop_music() override;
    void update_menu(TitleMenu&,i32) override;
    void draw_menu(TitleMenu&,i32) override;
};
struct MenuResources final : TitleResourceEnvironment {
    Title& owner;
    explicit MenuResources(Title&);
    TitleMenu* allocate() override;
    void delete_object(void*) override;
    AnmFile* load_animations(i32,const char*) override;
    void release_animations(AnmFile&) override;
    void delete_replay(Replay*) override;
    void free_file(void*) override;
    void report_error() override;
    u32 begin_thread(CallbackToken,void*,u32,u32&) override;
    u32 wait_thread(u32,u32) override;
    void close_thread(u32) override;
    void sleep(u32) override;
};
struct Title final : CallbackReceiver {
    GameState& state;AnimationEngine& engine;Common& common;Fonts& fonts;
    Input& input;Audio& audio;Scores& scores;ScreenEffects& effects;
    const MenuData& data;ReplayCalendar& calendar;
    TitleMenu* value=nullptr;StartupScreen* startup=nullptr;
    UpdateChain* update_chain;TitleLoadingTask loading_task;bool loading=false;i32 error=0;
    struct Preview {ReplayDocument document;Preview* next;Preview(FileSystem& files,u32 flags,Preview* next):document(files,flags),next(next){}};
    Preview* previews=nullptr;
    ResultsEnvironment* results=nullptr;
    MenuMain<TitleMainEnvironment> main;MenuSelection<TitleSelectionEnvironment> selection;
    MenuKeys keys;MenuOptions options;MenuScores score;MenuReplays replays;
    MenuMusic music;MenuDraw draw;MenuClear clear;MenuLoop loop;MenuResources resources;ReplayWriter writer;
    Title(GameState&,AnimationEngine&,Common&,Fonts&,Input&,Audio&,Scores&,ScreenEffects&);
    ~Title();
    bool initialize();void advance_loading();
    void bind_callbacks(Callbacks&) override;
#ifndef TH_NATIVE_PLATFORM
    bool invoke(CallbackToken,void*,i32&) override;
#endif
    u32 create(AnmFile&,i32);
    void sound(i32);
    void text(AnmVm&,u32,const char*,const u32*,u32,TextAlignment);
    Replay* preview(const char*);
    void delete_replay(Replay*);
    AudioGame music_control();
    template<class Environment> void bind_animation(Environment& env){env.registry=&engine.manager.registry;env.rate=&engine.speed;env.tangent=&engine.tangent;}
};
}
