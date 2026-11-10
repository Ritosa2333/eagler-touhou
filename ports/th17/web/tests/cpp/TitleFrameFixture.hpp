#include "../../cpp/game/TitleFrame.hpp"
struct TitleFrameFixture final:TitleFrameServices {
 TitleMainFixture& t;TitleFrame frame;std::vector<i32> events;std::array<std::shared_ptr<Replay>,3> demos;
 bool ending=false,ended=false;i32 destination=-1;
 explicit TitleFrameFixture(TitleMainFixture& p):t(p),frame(p.state,p.selection,p.animations,*this){}
 bool release_transient_animations()override{events.push_back(1);return true;}bool clear_title_overlay()override{events.push_back(2);return true;}
 bool read_demo(i32 i,std::shared_ptr<Replay>& p)override{p=demos[i];events.insert(events.end(),{3,i});return p!=nullptr;}
 bool begin_demo(const DemoRequest& r)override{events.insert(events.end(),{4,r.index,r.stage,r.character,r.animal,r.difficulty});destination=13;return true;}
 bool queue_title_music(i32 c,i32 v,const std::string&)override{events.insert(events.end(),{5,c,v});return true;}
 bool clear_current_wave()override{events.push_back(6);return true;}bool unlock_title_music()override{events.push_back(7);return true;}
 bool selection_return_transition()override{events.push_back(8);return true;}bool title_exit(i32 d)override{destination=d;events.insert(events.end(),{9,d});return true;}
 bool fade_out_title_music()override{events.push_back(10);return true;}
 bool update_title_menu(TitleScreen s,const TitleFrameInput&)override{events.insert(events.end(),{11,i32(s)});return true;}
 bool draw_title_menu(TitleScreen s)override{events.insert(events.end(),{12,i32(s)});return true;}
 bool ending_active()const override{return ending;}bool ending_complete()const override{return ended;}bool retire_ending()override{ending=false;events.push_back(13);return true;}
};
API(title_frame_create) TitleFrameFixture* title_frame_create(TitleMainFixture* t){return new TitleFrameFixture(*t);}
API(title_frame_delete) void title_frame_delete(TitleFrameFixture* p){delete p;}
API(title_frame_demo) bool title_frame_demo(TitleFrameFixture* p,u32 i,const u8* b,u32 n){if(i>=3)return false;auto r=std::make_shared<Replay>();if(!r->open(b,n))return false;p->demos[i]=std::move(r);return true;}
API(title_frame_configure) void title_frame_configure(TitleFrameFixture* p,i32 idle,i32 demo,i32 saved,i32 age,bool alternate,bool ending,bool ended){p->frame.demo_idle=idle;p->frame.demo_index=demo;p->frame.saved_difficulty=saved;p->frame.music_age=age;p->frame.alternate_audio=alternate;p->ending=ending;p->ended=ended;p->destination=-1;}
API(title_frame_update) bool title_frame_update(TitleFrameFixture* p,u32 held,u32 pressed,u32 repeated,u32 flags,float rate){p->events.clear();return p->frame.update({held,pressed,repeated,flags,rate});}
API(title_frame_draw) bool title_frame_draw(TitleFrameFixture* p){p->events.clear();return p->frame.draw();}
API(title_frame_events) const i32* title_frame_events(TitleFrameFixture* p){return p->events.data();}
API(title_frame_event_count) u32 title_frame_event_count(TitleFrameFixture* p){return p->events.size();}
API(title_frame_fields) const i32* title_frame_fields(TitleFrameFixture* p){static std::array<i32,6> v;v={p->frame.demo_idle,p->frame.demo_index,p->frame.saved_difficulty,p->frame.music_age,p->destination,p->frame.draw_enabled?1:0};return v.data();}
API(title_frame_error) const char* title_frame_error(TitleFrameFixture* p){return p->frame.error.c_str();}
