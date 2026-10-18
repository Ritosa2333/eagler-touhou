#pragma once
#include "TitleAnimations.hpp"
#include "SessionState.hpp"
#include "PlayerLife.hpp"
#include "RecordStore.hpp"
#include "PauseScoreRegistration.hpp"
#include "TitleReplayMenu.hpp"
#include "TitleNameEntry.hpp"
namespace th15 {
struct TitleResultsServices:PauseScoreServices {
 virtual bool sound(i32)=0;virtual bool music(const std::string&,i32 track)=0;virtual bool release_live_replay()=0;
};
class TitleResultsMenu:public TitleNameEntry {
 TitleState& state;SessionState& progress;PlayerLifeSession& player;ItemScoreState& score;RecordStore& records;AnmManager& animations;TitleAnimations visuals;TitleResultsServices& host;
 bool check(bool,const char*);bool visual(bool);bool prepare_name();bool append(char);bool finish_name();
public:
 std::string error;
 TitleResultsMenu(TitleState& s,SessionState& p,PlayerLifeSession& v,ItemScoreState& points,RecordStore& r,AnmManager& a,TitleResultsServices& h,i32 title=16,i32 ascii=5):state(s),progress(p),player(v),score(points),records(r),animations(a),visuals(s,a,title,ascii),host(h){names.wrapping=false;}
 bool update(u32 pressed,u32 repeated);bool draw(HudDrawServices&,ReplayCalendarServices&);
};
}
