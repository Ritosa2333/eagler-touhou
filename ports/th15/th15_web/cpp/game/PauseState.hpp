#pragma once
#include "MenuCursor.hpp"
#include "Timer.hpp"
namespace th15 {
enum class PauseScreen:i32 {Inactive=0,Pause=1,GameOver=2,Results=3};
struct PauseState {
 Timer age,selection_age;MenuCursor menu,names;
 PauseScreen screen=PauseScreen::Inactive,previous=PauseScreen::Inactive;
 i32 phase=0,result_mode=0;u32 flags=0;
 u32 menu_animation=0,snapshot_animation=0;i32 front_bank=-1;
 float saved_rate=1;i32 saved_frame_skip=0;
 std::string saved_music;double saved_music_position=0;
 std::array<char,9> name{};i32 name_length=0;bool name_not_required=false,retry_controls=false;
 PauseState(){age.set(0);selection_age.set(0);menu.wrapping=names.wrapping=false;}
 void select(PauseScreen next)noexcept{previous=screen;screen=next;phase=0;menu.disabled_count=0;age.set(0);selection_age.set(0);}
 void select_phase(i32 next)noexcept{phase=next;age.set(0);}
};
}
