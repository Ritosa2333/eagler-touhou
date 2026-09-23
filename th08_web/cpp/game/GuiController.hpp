// TH08 in-game HUD and stage-completion presentation.
#pragma once
#include "Dialogue.hpp"
#include "AsciiManager.hpp"
#include <memory>
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "../multiplayer/PlayerResources.hpp"
#include "../multiplayer/PlayerRoster.hpp"
#endif
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
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    void bind_multiplayer_resources(const PilotResources* resources,u32 count,u32 local,const PlayerRoster& player_roster){
        pilot_resources=resources;pilot_count=count;local_player=local;roster=&player_roster;
    }
#endif
private:
    friend class GuiFlow;
    GuiState& gui;GuiImplState& display;DialogueContext& scene;GuiContext& context;
    GameGlobals& globals;GameValues& values;GameConfiguration& config;AnmExecutor& executor;AsciiManager& ascii;AnmRenderer& renderer;DialogueActions& actions;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    const PilotResources* pilot_resources=nullptr;const PlayerRoster* roster=nullptr;u32 pilot_count=0,local_player=0;
    void draw_multiplayer_hud();
    float display_lives()const{return pilot_resources?pilot_resources[local_player].lives:0;}
    float display_bombs()const{return pilot_resources?pilot_resources[local_player].bombs:0;}
    float display_power()const{return pilot_resources?pilot_resources[local_player].power:0;}
    i32 team_life_bonus()const{
        i32 total=0;for(u32 seat=0;seat<pilot_count;++seat)if(pilot_resources[seat].lives>0)
            total=wrapping_add(total,signed_bits(u32(Scalar::truncate(pilot_resources[seat].lives))*2500000u));
        return total;
    }
    i32 team_bomb_bonus()const{
        i32 total=0;for(u32 seat=0;seat<pilot_count;++seat)if(pilot_resources[seat].bombs>0)
            total=wrapping_add(total,signed_bits(u32(Scalar::truncate(pilot_resources[seat].bombs))*500000u));
        return total;
    }
#else
    float display_lives()const{return globals.lives;}
    float display_bombs()const{return globals.bombs;}
    float display_power()const{return globals.power;}
    i32 team_life_bonus()const{return signed_bits(u32(Scalar::truncate(globals.lives))*2500000u);}
    i32 team_bomb_bonus()const{return signed_bits(u32(Scalar::truncate(globals.bombs))*500000u);}
#endif
    struct PresentationState {
        std::unique_ptr<GuiImplState> display;
        GuiFormattedText bonus{},popup{},spell_bonus{};
        float boss_life=0;u32 boss_opacity=0;bool boss_present=false;u8 boss_life_state=0;bool valid=false;
    } presentation_state;
    presentation::SnapshotMarker presentation_marker;
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
