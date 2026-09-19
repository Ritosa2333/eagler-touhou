// TH08 in-game HUD and stage-completion presentation.
#pragma once
#include "Dialogue.hpp"
#include "AsciiManager.hpp"
#include <memory>
namespace th08 {
struct GuiContext {
    i32 difficulty=0;Vec3 player;
    i32 stage_frames=1,human_frames=0,youkai_frames=0;
    bool practice_replay=false,time_stopped=false,paused=false,retry=false;
    bool spell_active=false,boss_exists=false;u16 input=0;
    i32 familiar_count=0,familiar_multiplier=0;
    u32 graphics_options=0;
};
class GuiController {
public:
    GuiController(GuiState& gui,GuiImplState& display,DialogueContext& scene,GuiContext& context,GameGlobals& globals,GameValues& values,GameConfiguration& config,AnmExecutor& executor,AsciiManager& ascii,AnmRenderer& renderer,DialogueActions& actions)
      :gui(gui),display(display),scene(scene),context(context),globals(globals),values(values),config(config),executor(executor),ascii(ascii),renderer(renderer),actions(actions){}
    void update_stage();
    void draw_clear();
    void draw_popups();
    void draw_hud();
    void draw_stage();
    void show_bonus(i32 value);
    void show_popup(i32 value,i32 type);
    void show_spell_bonus(i32 value);
    bool finished()const{return display.loading_portrait.activeSpriteIndex>=0&&display.loading_portrait.stopped;}
    bool clock(i32 action);
    bool capture();
    void reset_clear();
    // Section warps skip the opening stage title; the tied clock intro must
    // never execute then (upstream th08_disable_title). Set by GuiFlow::setup.
    bool clock_intro_enabled=true;
private:
    friend class GuiFlow;
    GuiState& gui;GuiImplState& display;DialogueContext& scene;GuiContext& context;
    GameGlobals& globals;GameValues& values;GameConfiguration& config;AnmExecutor& executor;AsciiManager& ascii;AnmRenderer& renderer;DialogueActions& actions;
    struct PresentationState {
        std::unique_ptr<GuiImplState> display;
        GuiFormattedText bonus{},popup{},spell_bonus{};
        float boss_life=0;u32 boss_opacity=0;bool boss_present=false;u8 boss_life_state=0;bool valid=false;
    } presentation_state;
    void snapshot_presentation();
    AnmVm presentation_vm(const AnmVm&)const;
    void draw_presented_no_rotation(AnmVm&);
    void draw_presented_2d(AnmVm&);
    void draw_presented_world(AnmVm&);
    Vec3 presentation_text_position(const GuiFormattedText&,const GuiFormattedText&)const;
    bool start(AnmVm& vm,AnmLoaded* file,i32 script,bool reset_position=false);
    bool software()const{return context.graphics_options&257;}
};
}
