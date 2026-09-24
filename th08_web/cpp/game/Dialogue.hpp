// TH08 MSG execution and dialogue presentation. All control flow is C++;
// resource scripts contain only the original game's message instructions.
#pragma once
#include "GuiState.hpp"
#include "AnmExecutor.hpp"
#include "AnmText.hpp"
#include "AnmRenderer.hpp"
#include "GameValues.hpp"
namespace th08 {
struct DialogueContext {
    u32 flags=0;i32 stage=0,character=0,player_state=0,background_state=0,hud_redraw=0;
    u16 input=0,previous_input=0;i8 replay_clear=0;u8 padding[3]{};
    float arcade_x=32;
    AnmLoaded *faces[4]{},*text=nullptr,*ascii=nullptr,*capture=nullptr;
    ClearRecord clears[13];
    char song_paths[3][128]{};
};
struct DialogueActions {
    virtual ~DialogueActions()=default;
    virtual void copy_enemy_name(i32 index)=0;
    virtual void clear_bullets()=0;
    virtual void despawn_enemies()=0;
    virtual void collect_items()=0;
    virtual void sound(i32 index)=0;
    virtual bool play_music(i32 index,i32 song)=0;
    virtual void play_audio(const char* path,i32 song)=0;
    virtual void stop_audio()=0;
    virtual void fade_music(float seconds)=0;
    virtual void fade_screen(i32 frames,u32 color,i32 priority)=0;
    virtual void capture_arcade(const AnmLoadedSprite& sprite)=0;
};
class Dialogue {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    friend class multiplayer::WorldJournal;
#endif
public:
    Dialogue(GuiState& gui,GuiImplState& display,DialogueContext& context,GameGlobals& globals,GameValues& values,AnmExecutor& executor,TextWriter& text,AnmRenderer& renderer,DialogueActions& actions)
        :gui(gui),display(display),state(display.dialogue),context(context),globals(globals),values(values),executor(executor),text_writer(text),renderer(renderer),actions(actions){}
    MessageProgram program;
    bool load(const u8* bytes,u32 size);
    void release();
    bool read(i32 index);
    i32 update();
    i32 draw();
    bool waiting()const{return state.message>=0&&!state.ignore_wait;}
    bool present()const{return state.message>=0||state.message==-2;}
private:
    GuiState& gui;GuiImplState& display;DialogueState& state;DialogueContext& context;
    GameGlobals& globals;GameValues& values;AnmExecutor& executor;TextWriter& text_writer;AnmRenderer& renderer;DialogueActions& actions;
    struct PresentationVm {Vec3 pos{},pos2{};i16 script=-1;bool visible=false;};
    PresentationVm presentation_portraits[4]{},presentation_lines[2]{},presentation_intro[2]{};
    float presentation_timer=0; i32 presentation_message=-1; bool presentation_valid=false;
    void snapshot_presentation();
    AnmVm presentation_vm(const AnmVm&,const PresentationVm&)const;
    bool pressed(u16 mask)const{return (context.input&mask)&&(context.input&mask)!=(context.previous_input&mask);}
    bool start(AnmVm& vm,AnmLoaded* file,i32 script);
    bool sprite(AnmVm& vm,AnmLoaded* file,i32 index);
    bool configure(i32 portrait);
    bool line(i32 index,i32 color,const u8* encoded,u32 size);
    void erase_second(i32 color);
    bool stage_results();
    i32 invalid(){state.message=-1;state.instruction=nullptr;return -1;}
};
}
