#pragma once
#include "TitleMenus.hpp"
#include "AsciiManager.hpp"
#include "GameValues.hpp"
namespace th08 {
struct TitleFlowState {
    i32 screen_effect_counter=0,pause_state=0;
    float frame_rate=1;
    i32 transition_counters[5]{};
    i32 background_wait=0;
    bool background_running=false,close_requested=false,capture_pending=false;
};
struct TitleFlowActions {
    virtual ~TitleFlowActions()=default;
    virtual std::vector<u8> read_score()=0;
    virtual AnmLoaded* preload_animation(i32 index,const char* path)=0;
    virtual bool preload_background(const char* path)=0;
    virtual void loading(bool capture)=0;
    virtual void start_effect()=0;
    virtual void fade_in()=0;
    virtual void fade_loading()=0;
    virtual void begin_loading()=0;
};
class TitleFlow {
public:
    TitleFlow(TitleMenus& menus,AsciiManager& ascii,const AsciiContext& ascii_context,TitleFlowActions& actions)
        :menus(menus),ascii(ascii),ascii_context(ascii_context),actions(actions){}
    TitleFlowState state;
    GameConfiguration game_config;GameGlobals game_values;
    // A caller presents each information image using TitleInformation and
    // acknowledges it before loading the next scene resource.
    const char* begin();
    const char* complete_information();
    bool setup();
private:
    TitleMenus& menus;AsciiManager& ascii;const AsciiContext& ascii_context;TitleFlowActions& actions;
    const char* information[3]{};i32 information_count=0,information_index=0;
    void finish_begin();
};
}
