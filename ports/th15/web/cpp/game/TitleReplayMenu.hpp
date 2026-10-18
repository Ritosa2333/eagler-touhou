#pragma once
#include "TitleAnimations.hpp"
#include "ReplayCatalog.hpp"
#include "SessionState.hpp"
#include "HudDrawData.hpp"
namespace th15 {
struct ReplayCalendar {i32 year=0,month=1,day=1,hour=0,minute=0;};
struct ReplayCalendarServices {virtual ~ReplayCalendarServices()=default;virtual bool calendar(i64 timestamp,ReplayCalendar&)=0;};
struct ReplayStartRequest {std::string filename;i32 stage=1,character=0,subcharacter=0,difficulty=0,spell=-1;bool spell_practice=false;i32 selection=0;};
struct TitleReplayServices {
 virtual ~TitleReplayServices()=default;
 virtual bool request_catalog()=0;virtual bool release_catalog()=0;virtual bool sound(i32)=0;virtual bool fade_music(float)=0;
 virtual bool begin_transition(u32&)=0;virtual bool transition_size(float width,float height)=0;
 virtual bool start_replay(const ReplayStartRequest&)=0;
};
class TitleReplayMenu {
 TitleState& state;SessionState& progress;PlayerLifeSession& player;AnmManager& animations;ReplayCatalog& catalog;TitleReplayServices& services;i32& saved_selection;TitleAnimations visuals;
 bool check(bool,const char*);bool visual(bool);bool choose_stage();bool launch();
public:
 MenuCursor pages;i32 selected_replay=0,selected_stage=0;u32 transition=0;std::string error;
 TitleReplayMenu(TitleState& s,SessionState& p,PlayerLifeSession& player,AnmManager& a,ReplayCatalog& catalog,TitleReplayServices& host,i32& saved_selection,i32 title=16,i32 ascii=5):state(s),progress(p),player(player),animations(a),catalog(catalog),services(host),saved_selection(saved_selection),visuals(s,a,title,ascii){}
 void catalog_finished()noexcept{state.flags=(state.flags&~4u)|8;}
 bool update(u32 pressed,u32 repeated);
 bool draw(HudDrawServices&,ReplayCalendarServices&);
};
}
