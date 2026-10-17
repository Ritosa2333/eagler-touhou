#pragma once
#include "GameActors.hpp"
#include "GameState.hpp"
#include "Common.hpp"
#include "Fonts.hpp"
#include "Audio.hpp"
#include "Scores.hpp"
#include "ScreenEffects.hpp"
#include "../game/GuiResources.hpp"
#include "../game/GuiFrame.hpp"
#include "../game/Dialogue.hpp"
#include "../game/AudioGame.hpp"
namespace th10::browser {
struct HudActions {
    virtual void clear_for_dialogue()=0;
    virtual void show_clear_results()=0;
};
struct Hud;
struct HudMessages final:DialogueEnvironment {
    Hud& owner;char text[2048]{};
    explicit HudMessages(Hud&);
    AnmFile& file(DialogueAnimationFile);
    Dialogue* allocate() override;
    u32 create_animation(DialogueAnimationFile,i32) override;
    void bind_sprite(AnmVm&,DialogueAnimationFile,i32,bool) override;
    void draw_text(AnmVm*,u32,const char*) override;
    void clear_projectiles_and_enemies() override;
    void play_sound(i32) override;
    void start_music() override;void fade_music(float) override;void complete_stage() override;
};
struct HudFrame final:GuiFrameEnvironment,GuiDrawEnvironment {
    Hud& owner;explicit HudFrame(Hud&);
    void update_animation(AnmVm&) override;
    void bind_digit(AnmFile&,AnmVm&,i32) override;
    u32 create_animation(AnmFile&,i32) override;
    i32 update_dialogue(Dialogue&) override;void release_dialogue(Dialogue*) override;
    void play_sound(i32) override;void draw_animation(AnmVm&) override;
    void rectangle(const ScreenRect&,u32) override;
    float presentation_boss_health(float current) override;
};
struct HudScore final:GuiScoreEnvironment {
    Hud& owner;explicit HudScore(Hud&);
    void bind_digit(AnmFile&,AnmVm&,i32) override;void update_animation(AnmVm&) override;void add_life() override;
};
struct HudNotification final:GuiAnimationEnvironment {
    Hud& owner;explicit HudNotification(Hud&);
    u32 create(AnmFile&,i32) override;void bind_sprite(AnmVm&,i32) override;
};
struct HudEconomy final:EconomyEnvironment {
    Hud& owner;explicit HudEconomy(Hud& o):owner(o){}
    void show_notification(i32) override;void play_global_sound(i32) override;void update_lives(i32) override;
};
struct HudProgress final:GameProgressionEnvironment {
    Hud& owner;explicit HudProgress(Hud&);
    void stage_clear_notification() override;void select_screen(i32) override;
    void show_results() override;void fade_ending() override;
};
struct Hud final:GuiResourceEnvironment,CallbackReceiver {
    GameState& state;GameActors& actors;AnimationEngine& engine;Common& common;Fonts& fonts;
    Input& input;Audio& audio;Scores& records;ScreenEffects& screen_effects;HudActions& actions;
    UpdateChain* update_chain;u8* message_cache=nullptr;char resource_name[260]{};u32 difficulty_visible=0;i32 error=0;
    float previous_boss_health=0;Enemy* previous_boss=nullptr;bool boss_presentation_valid=false;
    Hud(GameState&,GameActors&,AnimationEngine&,Common&,Fonts&,Input&,Audio&,Scores&,ScreenEffects&,HudActions&);
    ~Hud();
    bool initialize();void activate();void shutdown();void bind_callbacks(Callbacks&) override;
#ifndef TH_NATIVE_PLATFORM
    bool invoke(CallbackToken,void*,i32&) override;
#endif
    void start_dialogue(i32);void notify(i32,i32);void update_score();void update_power(i32,i32);
    void sound(i32);u32 animation(AnmFile&,i32);AudioGame music();
    Gui* allocate() override;
    AnmFile* load_animations(i32,const char*) override;void release_animations(AnmFile&) override;
    void delete_object(void*) override;void free_bytes(void*) override;
    u8* read_file(const char*) override;void report_error() override;
    u32 create_animation(AnmFile&,i32) override;
    void initialize_animation(AnmFile&,AnmVm&,i32) override;void bind_sprite(AnmFile&,AnmVm&,i32) override;
};
}
