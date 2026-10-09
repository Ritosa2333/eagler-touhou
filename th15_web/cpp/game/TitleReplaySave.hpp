#pragma once
#include "TitleNameEntry.hpp"
#include "TitleAnimations.hpp"
#include "SessionState.hpp"
#include "RecordStore.hpp"
#include "ReplayRecording.hpp"
#include "TitleReplayMenu.hpp"
namespace th15 {
struct TitleReplaySaveServices {
 virtual bool replay_save_available()const{return true;}
 virtual ~TitleReplaySaveServices()=default;virtual bool read_slot(i32,std::shared_ptr<Replay>&)=0;
 virtual bool sound(i32)=0;virtual bool prepare_live_replay(bool completed)=0;virtual bool save_slot(i32,const std::array<char,9>&)=0;
 virtual bool release_live_replay()=0;virtual bool music(const std::string&,i32)=0;
};
class TitleReplaySave {
 TitleState& state;SessionState& progress;RecordStore& records;AnmManager& animations;TitleNameEntry& editor;TitleAnimations visuals;TitleReplaySaveServices& host;
 bool check(bool,const char*);bool visual(bool);bool prepare_name();bool append(char);bool caption(i32,const u8*,bool live,ReplayCalendarServices&,HudTextDraw&);
public:
 std::array<std::shared_ptr<Replay>,25> files{};i32 selected=0;std::string error;
 TitleReplaySave(TitleState& s,SessionState& p,RecordStore& r,AnmManager& a,TitleNameEntry& n,TitleReplaySaveServices& h,i32 title=16,i32 ascii=5):state(s),progress(p),records(r),animations(a),editor(n),visuals(s,a,title,ascii),host(h){}
 bool update(u32 pressed,u32 repeated);bool draw(HudDrawServices&,ReplayCalendarServices&,const std::array<u8,0xa4>* live);
};
}
