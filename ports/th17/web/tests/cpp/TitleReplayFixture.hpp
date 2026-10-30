#include "../../cpp/game/TitleReplayMenu.hpp"
struct TitleReplayFixture final:TitleReplayServices,HudDrawServices,ReplayCalendarServices {
 TitleMainFixture& t;GameRun run;PlayerLifeSession player;ReplayCatalog catalog;TitleReplayMenu menu;i32 saved=0;bool immediate=false;std::vector<i32> events;std::string filename;
 struct Pending {i32 slot;std::string name;std::shared_ptr<Replay> replay;};std::vector<Pending> pending;ReplayCalendar date{2026,10,23,0,0};i64 timestamp=0;std::vector<HudTextDraw> text;
 explicit TitleReplayFixture(TitleMainFixture& p):t(p),menu(p.state,run,player,p.animations,catalog,*this,saved){}
 bool request_catalog()override{events.push_back(1);for(auto& e:pending)if(!catalog.set(e.slot,e.name,e.replay))return false;if(immediate)menu.catalog_finished();return true;}
 bool release_catalog()override{events.push_back(2);return true;}bool sound(i32 id)override{t.sounds.push_back(id);return true;}
 bool fade_music(float value)override{events.insert(events.end(),{3,i32(float_to_bits(value))});return true;}
 bool begin_transition(u32& handle)override{events.push_back(4);handle=0;return true;}
 bool transition_size(float w,float h)override{events.insert(events.end(),{5,i32(float_to_bits(w)),i32(float_to_bits(h))});return true;}
 bool start_replay(const ReplayStartRequest& r)override{events.push_back(6);filename=r.filename;return true;}
 bool hud_text(const HudTextDraw& draw)override{text.push_back(draw);return true;}
 bool calendar(i64 time,ReplayCalendar& value)override{timestamp=time;value=date;return true;}
};
API(title_replay_create) TitleReplayFixture* title_replay_create(TitleMainFixture* t){return new TitleReplayFixture(*t);}
API(title_replay_delete) void title_replay_delete(TitleReplayFixture* p){delete p;}
API(title_replay_configure) void title_replay_configure(TitleReplayFixture* p,i32 saved,bool ready){p->saved=saved;p->immediate=ready;p->t.state.menu.count=10;p->t.state.menu.select(5);p->t.state.menu.push();p->t.state.change_screen(TitleScreen::Replay);p->run.scoring.mode_flags=0x110;}
API(title_replay_file) bool title_replay_file(TitleReplayFixture* p,i32 slot,const char* name,const u8* bytes,u32 size){auto replay=std::make_shared<Replay>();if(!replay->open(bytes,size))return false;p->pending.push_back({slot,name,std::move(replay)});return true;}
API(title_replay_update) bool title_replay_update(TitleReplayFixture* p,u32 pressed,u32 repeated){p->events.clear();p->t.sounds.clear();return p->menu.update(pressed,repeated);}
API(title_replay_draw) bool title_replay_draw(TitleReplayFixture* p){p->text.clear();return p->menu.draw(*p,*p);}
API(title_replay_ready) void title_replay_ready(TitleReplayFixture* p){p->menu.catalog_finished();}
API(title_replay_pages) MenuCursor* title_replay_pages(TitleReplayFixture* p){return &p->menu.pages;}
API(title_replay_run) GameRun* title_replay_run(TitleReplayFixture* p){return &p->run;}
API(title_replay_value) i32 title_replay_value(TitleReplayFixture* p,u32 i){switch(i){case 0:return p->menu.pages.selected;case 1:return p->menu.selected_replay;case 2:return p->menu.selected_stage;case 3:return p->saved;case 4:{i32 count=0;for(i32 j=0;j<100;j++)count+=p->catalog.entry(j)!=nullptr;return count;}default:return -1;}}
API(title_replay_events) const i32* title_replay_events(TitleReplayFixture* p){return p->events.data();}
API(title_replay_event_count) u32 title_replay_event_count(TitleReplayFixture* p){return p->events.size();}
API(title_replay_error) const char* title_replay_error(TitleReplayFixture* p){return p->menu.error.c_str();}
API(title_replay_filename) const char* title_replay_filename(TitleReplayFixture* p){return p->filename.c_str();}
API(title_replay_text_count) u32 title_replay_text_count(TitleReplayFixture* p){return p->text.size();}
API(title_replay_text) const char* title_replay_text(TitleReplayFixture* p,u32 i){return p->text[i].text.c_str();}
API(title_replay_text_style) const HudTextStyle* title_replay_text_style(TitleReplayFixture* p,u32 i){return &p->text[i].style;}
API(title_replay_text_position) const Vec3* title_replay_text_position(TitleReplayFixture* p,u32 i){return &p->text[i].position;}
API(title_replay_date) ReplayCalendar* title_replay_date(TitleReplayFixture* p){return &p->date;}
API(title_replay_timestamp) i64 title_replay_timestamp(TitleReplayFixture* p){return p->timestamp;}
