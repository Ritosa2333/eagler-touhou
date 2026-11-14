#pragma once
#include "AnmManager.hpp"
#include "MenuCursor.hpp"
#include "ScoreFile.hpp"
#include "ReplayMenu.hpp"
#include "AsciiText.hpp"
#include <string>
#include <vector>
namespace th11 {
enum class PauseAction {None,Resume,Title,Restart,Continue};
// Original in-game overlay. Overlay ANMs advance while the combat registries
// remain frozen; the screenshot request is consumed by the graphics backend.
class PauseMenu {
public:
    PauseMenu(AnmManager& a,AnmResource& text,AnmResource& front,ScoreFile& scores)
        :animations(a),text(text),front(front),scores(scores){timer.set(0,&a.rate);elapsed.set(0,&a.rate);}
    i32 state=0,forced_exit=0;Timer timer,elapsed;
    MenuCursor cursor,name_cursor;
    u32 menu_animation=0,background_animation=0;
    bool replay=false,capture_requested=false;
    std::array<std::shared_ptr<ReplayMenuEntry>,25> replay_files{};
    std::array<char,9> entered_name{};
    i32 name_length=0,completed=0,stage=1;
    i32 unranked=0,selection=0,difficulty=0,result_score=0,continues=0;
    float slowdown=0;
    u64 timestamp=0;
    bool practice=false,scan_requested=false,save_requested=false,recording_metadata_requested=false;
    bool manual_requested=false;
#if PORT_TITLE == 16
    i32 th16_substate=0;
    i32 th16_continues_remaining=5;
    u32 th16_subseason=0;
    AnmVm* th16_overlay(AnmResource&,i32 script,u16 file_id);
    bool update_th16(u32 pressed,u32 repeat);
    bool th16_score_begin();
    bool th16_end_choices();
#endif
#if PORT_TITLE == 14
    i32 spell_practice_id=-1;
#if defined(TH14_CPP_STAGE_DECODE)
    // Borrowed from the live Battle, which outlives this menu's updates.
    const LoadingWordTh14<u32>* th14_live_run_flags=nullptr;
#endif
    std::unique_ptr<AnmResource> manual_resource;
    Timer manual_timer;
    MenuCursor manual_cursor;
    i32 manual_phase=0,manual_substate=0,manual_image_requested=-1;
    bool manual_finished=false;
    float manual_origin_x=0;
    std::array<u32,10> manual_animations{};
    void begin_manual();
    void manual_update(u32,u32);
#endif
    std::vector<i32> sounds;
    PauseAction action=PauseAction::None;
    std::string error;
    bool begin(bool replay=false);
#if PORT_TITLE == 14
    bool begin_replay_end();
#endif
    bool begin_end(bool practice=false,bool completed=false);
    bool update(u32 pressed,u32 repeat);
    void queue_ascii(AsciiText&)const;
private:
    AnmManager& animations;AnmResource& text;AnmResource& front;ScoreFile& scores;
    void transition(i32 next){state=next;timer.set(0,&animations.rate);}
    void family(u32 id,i32 label);
    #if PORT_TITLE == 14
    bool create_backdrop(bool immediate_capture=true);
    #else
    bool create_backdrop();
    #endif
    void choose(i32 script);
    void move(u32 keys,i32 base);
    void escape(u32 pressed);
    void save_menu(u32 pressed,u32 repeat);
    void end_menu(u32 pressed,u32 repeat);
    void show_end_choices(i32 script,i32 next,i32 count);
    void visible(bool value);
#if PORT_TITLE == 14
    i32 remaining_credits_th14()const;
    bool create_deferred_backdrop_th14();
    void manual_choices();
    bool manual_create(i32);
#endif
    void name_begin();
    void name_move(u32);
    i32 name_confirm();
};
}
