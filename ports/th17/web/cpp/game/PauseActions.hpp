#pragma once
#include "PauseRestoration.hpp"
#include "AnmManager.hpp"
#include "GameProgression.hpp"
namespace th17 {
enum class PauseDestination:i32 {TitleSelection=2,Title=4,RestartRun=10,RestartReplay=11,ExtraGameOver=14};
struct PauseActionServices {
 virtual ~PauseActionServices()=default;
 virtual bool resume_looping_sounds()=0;virtual bool resume_music()=0;
 virtual bool resume_saved_music(const std::string&,double position)=0;
 virtual bool reset_audio_slots()=0;virtual bool destination(PauseDestination)=0;
};
struct PauseGameplayActions {
 virtual ~PauseGameplayActions()=default;
virtual bool options_changed()=0;
 virtual bool life_hud(i32,i32)=0;virtual bool bomb_hud(i32,i32)=0;
 virtual bool power_notice()=0;virtual bool resume_dialogue()=0;virtual bool resume_chapter_result()=0;
};
// Closing animation precedes the original continue, replay retry or scene exit.
// Continues retain the world while restoring the exact stock and power rules.
class PauseActions {
 PauseState& state;GameRun& progress;PlayerLifeSession& player;ItemScoreState& score;AnmManager& animations;PauseRestoration& restoration;PauseGameplayActions& gameplay;PauseActionServices& host;i32& frame_skip;
 bool check(bool,const char*);bool continue_run();
public:
 std::string error;
 PauseActions(PauseState& s,GameRun& p,PlayerLifeSession& v,ItemScoreState& points,AnmManager& a,PauseRestoration& r,PauseGameplayActions& g,PauseActionServices& h,i32& skip):state(s),progress(p),player(v),score(points),animations(a),restoration(r),gameplay(g),host(h),frame_skip(skip){}
 bool execute(bool return_to_selection);
};
}
