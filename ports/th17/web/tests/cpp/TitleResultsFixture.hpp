#include "../../cpp/game/TitleResultsMenu.hpp"
#include "../../cpp/game/TitleReplaySave.hpp"
struct TitleResultsFixture final:TitleResultsServices,TitleReplaySaveServices,HudDrawServices,ReplayCalendarServices {
 TitleMainFixture& t;GameRun run;PlayerLifeSession player;ItemScoreState points;RecordStore records;
 TitleResultsMenu menu;TitleReplaySave saving;std::vector<i32> events;std::vector<HudTextDraw> texts;ReplayCalendar date;std::vector<i64> timestamps;PauseScoreDetails details;
 std::array<std::shared_ptr<Replay>,25> disk{};std::shared_ptr<Replay> saved;std::array<u8,0xa0> live{};std::array<i32,8> fields{};
 explicit TitleResultsFixture(TitleMainFixture& title):t(title),menu(t.state,run,player,points,records,t.animations,*this),saving(t.state,run,records,t.animations,menu,*this){}
 static i32 hash(const std::string& s){u32 h=2166136261;for(u8 b:s)h=(h^b)*16777619;return signed_bits(h);}
 bool sound(i32 id)override{events.insert(events.end(),{4,id});return true;}
 bool music(const std::string& name,i32 track)override{events.insert(events.end(),{1,hash(name),track});return records.unlock_music(track);}
 bool release_live_replay()override{events.push_back(3);return true;}
 bool capture_score_details(PauseScoreDetails& out)override{events.push_back(2);out=details;return true;}
 bool read_slot(i32 i,std::shared_ptr<Replay>& out)override{events.insert(events.end(),{5,i});out=disk[i];return true;}
 bool prepare_live_replay(bool full)override{events.insert(events.end(),{6,full?1:0});return true;}
 bool save_slot(i32 slot,const std::array<char,9>& name)override{events.insert(events.end(),{7,slot,hash(name.data())});disk[slot]=saved;return true;}
 bool hud_text(const HudTextDraw& text)override{texts.push_back(text);return true;}
 bool calendar(i64 value,ReplayCalendar& out)override{timestamps.push_back(value);out=date;return true;}
};
API(title_results_create) TitleResultsFixture* title_results_create(TitleMainFixture* t){return new TitleResultsFixture(*t);}
API(title_results_delete) void title_results_delete(TitleResultsFixture* p){delete p;}
API(title_results_configure) void title_results_configure(TitleResultsFixture* p,i32 c,i32 a,i32 d,i32 score,i32 continues){p->run.character=c;p->run.animal=a;p->run.difficulty=d;p->run.scoring.continues=continues;p->points.score=score;p->run.stage=6;}
API(title_results_import) bool title_results_import(TitleResultsFixture* p,const u8* chars,const u8* settings){return p->records.import_blocks(chars,settings);}
API(title_results_blocks) const u8* title_results_blocks(TitleResultsFixture* p,u32 settings){const auto& file=p->records.export_blocks();return settings?file.settings.data():file.characters[0].data();}
API(title_results_details) PauseScoreDetails* title_results_details(TitleResultsFixture* p){return &p->details;}
API(title_results_date) ReplayCalendar* title_results_date(TitleResultsFixture* p){return &p->date;}
API(title_results_name) char* title_results_name(TitleResultsFixture* p){return p->menu.name.data();}
API(title_results_grid) MenuCursor* title_results_grid(TitleResultsFixture* p){return &p->menu.names;}
API(title_results_fields) const i32* title_results_fields(TitleResultsFixture* p){p->fields={p->menu.names.selected,p->menu.names.reserved,p->menu.names.count,p->menu.names.wrap,p->menu.name_length,i32(p->menu.name_not_required),p->run.stage,p->run.initial_stage};return p->fields.data();}
API(title_results_save_selected) i32 title_results_save_selected(TitleResultsFixture* p){return p->saving.selected;}
API(title_results_update) bool title_results_update(TitleResultsFixture* p,u32 held,u32 repeated,u32 save){p->events.clear();return save?p->saving.update(held,repeated):p->menu.update(held,repeated);}
API(title_results_events) const i32* title_results_events(TitleResultsFixture* p){return p->events.data();}
API(title_results_count) u32 title_results_count(TitleResultsFixture* p){return p->events.size();}
API(title_results_draw) bool title_results_draw(TitleResultsFixture* p,u32 save){p->texts.clear();p->timestamps.clear();return save?p->saving.draw(*p,*p,&p->live):p->menu.draw(*p,*p);}
API(title_results_text_count) u32 title_results_text_count(TitleResultsFixture* p){return p->texts.size();}
API(title_results_text) const char* title_results_text(TitleResultsFixture* p,u32 i){return p->texts[i].text.c_str();}
API(title_results_style) const HudTextStyle* title_results_style(TitleResultsFixture* p,u32 i){return &p->texts[i].style;}
API(title_results_position) const Vec3* title_results_position(TitleResultsFixture* p,u32 i){return &p->texts[i].position;}
API(title_results_error) const char* title_results_error(TitleResultsFixture* p,u32 save){return save?p->saving.error.c_str():p->menu.error.c_str();}
API(title_results_live) u8* title_results_live(TitleResultsFixture* p){return p->live.data();}
API(title_results_file) bool title_results_file(TitleResultsFixture* p,i32 i,const u8* bytes,u32 size){auto file=std::make_shared<Replay>();if(!file->open(bytes,size))return false;if(i<0)p->saved=std::move(file);else if(i<25)p->disk[i]=std::move(file);else return false;return true;}
