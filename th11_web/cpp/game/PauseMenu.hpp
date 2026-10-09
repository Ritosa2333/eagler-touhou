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
    bool practice_save_disabled=false;
    std::vector<i32> sounds;
    PauseAction action=PauseAction::None;
    std::string error;
    bool begin(bool replay=false);
    AnmVm* background_vm()noexcept{return animations.find(background_animation);}
    bool begin_end(bool practice=false,bool completed=false);
#ifdef TH11_MULTIPLAYER
    bool multiplayer_result=false,multiplayer_pause=false;
    bool begin_multiplayer_pause();
    bool update_multiplayer_pause(u32 pressed);
    bool begin_multiplayer_end(bool completed=false);
#endif
    bool update(u32 pressed,u32 repeat);
    void queue_ascii(AsciiText&)const;
private:
    AnmManager& animations;AnmResource& text;AnmResource& front;ScoreFile& scores;
    void transition(i32 next){state=next;timer.set(0,&animations.rate);}
    void family(u32 id,i32 label);
    void choose(i32 script);
    void move(u32 keys,i32 base);
    void escape(u32 pressed);
    void save_menu(u32 pressed,u32 repeat);
    void end_menu(u32 pressed,u32 repeat);
    void show_end_choices(i32 script,i32 next,i32 count);
    void visible(bool value);
    void name_begin();
    void name_move(u32);
    i32 name_confirm();
};
}
