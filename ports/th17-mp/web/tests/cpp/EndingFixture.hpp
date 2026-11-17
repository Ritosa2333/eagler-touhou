#include "../../cpp/game/EndingScript.hpp"
struct EndingScriptFixture final:EndingServices {
 MessageResource program;EndingScript ending{*this};std::vector<i32> events;struct Text{u32 handle;std::string bytes;u32 color;};std::vector<Text> texts;u32 next=100;
 static i32 hash(const std::string& name){u32 h=2166136261;for(u8 ch:name){h^=ch;h*=16777619;}return signed_bits(h);}
 bool create_text(i32 script,u32& h)override{h=next++;events.insert(events.end(),{1,script,signed_bits(h)});return true;}
 bool text(u32 h,const std::string& value,u32 color)override{texts.push_back({h,value,color});events.insert(events.end(),{2,signed_bits(h),signed_bits(color),hash(value)});return true;}
 bool interrupt(u32 h,i32 label)override{events.insert(events.end(),{3,signed_bits(h),label});return true;}
 bool retire(u32& h)override{events.insert(events.end(),{4,signed_bits(h)});h=0;return true;}
 bool load_animation(i32 bank,const std::string& name)override{events.insert(events.end(),{5,bank,hash(name)});return true;}
 bool create_animation(i32 bank,i32 script,u32& h)override{h=next++;events.insert(events.end(),{6,bank,script,signed_bits(h)});return true;}
 bool loading_overlay()override{events.push_back(7);return true;}bool end_loading_overlay()override{events.push_back(8);return true;}
 bool read_staff(const std::string& name,MessageResource& out)override{events.insert(events.end(),{9,hash(name)});return out.open(staff_bytes.data(),staff_bytes.size(),true);}
 bool prepare_music(const std::string& name)override{events.insert(events.end(),{10,hash(name)});return true;}bool start_music(i32 track)override{events.insert(events.end(),{11,track});return true;}
 bool fade_music(i32 seconds)override{events.insert(events.end(),{12,seconds});return true;}bool sound(i32 id)override{events.insert(events.end(),{13,id});return true;}
 bool shake(i32 kind,i32 amount)override{events.insert(events.end(),{14,kind,amount});return true;}bool reset_caption_state()override{events.push_back(15);return true;}
 std::vector<u8> staff_bytes;std::array<i32,29> projected{};
 const i32* fields(){auto& s=ending.state;projected[0]=signed_bits(s.instruction_offset);projected[1]=signed_bits(s.flags);projected[2]=s.line;projected[3]=signed_bits(s.color);for(u32 i=0;i<5;i++)projected[4+i]=signed_bits(s.text_handles[i]);for(u32 i=0;i<4;i++)projected[9+i]=s.banks[i];for(u32 i=0;i<16;i++)projected[13+i]=signed_bits(s.sprites[i]);return projected.data();}
};
API(ending_script_create) EndingScriptFixture* ending_script_create(){return new EndingScriptFixture;}
API(ending_script_delete) void ending_script_delete(EndingScriptFixture* p){delete p;}
API(ending_script_initialize) bool ending_script_initialize(EndingScriptFixture* p,const u8* data,u32 size){p->events.clear();p->texts.clear();if(!p->program.open(data,size,true)||p->program.scripts.empty())return false;return p->ending.initialize(p->program.scripts[0]);}
API(ending_script_step) bool ending_script_step(EndingScriptFixture* p,u32 held,u32 pressed,u32 held_frames,float rate){p->events.clear();p->texts.clear();return p->ending.step({held,pressed,held_frames,rate});}
API(ending_script_configure) void ending_script_configure(EndingScriptFixture* p,i32 difficulty,i32 continues,u32 mode,u32 first){p->ending.run={difficulty,continues,first,mode!=0};}
API(ending_script_staff) void ending_script_staff(EndingScriptFixture* p,const u8* data,u32 size){p->staff_bytes.assign(data,data+size);}
API(ending_script_ready) bool ending_script_ready(EndingScriptFixture* p,i32 bank){p->events.clear();p->texts.clear();return p->ending.animation_ready(bank);}
API(ending_script_events) const i32* ending_script_events(EndingScriptFixture* p){return p->events.data();}
API(ending_script_event_count) u32 ending_script_event_count(EndingScriptFixture* p){return p->events.size();}
API(ending_script_fields) const i32* ending_script_fields(EndingScriptFixture* p){return p->fields();}
API(ending_script_timer) const Timer* ending_script_timer(EndingScriptFixture* p,u32 kind){return kind==0?&p->ending.state.age:kind==1?&p->ending.state.clock:&p->ending.state.wait;}
API(ending_script_finished) bool ending_script_finished(EndingScriptFixture* p){return p->ending.finished;}
API(ending_script_error) const char* ending_script_error(EndingScriptFixture* p){return p->ending.error.empty()?p->program.error.c_str():p->ending.error.c_str();}
API(ending_script_text_count) u32 ending_script_text_count(EndingScriptFixture* p){return p->texts.size();}
API(ending_script_text_bytes) const char* ending_script_text_bytes(EndingScriptFixture* p,u32 index){return p->texts[index].bytes.c_str();}

#include "../../cpp/game/EndingFrame.hpp"
struct EndingFrameFixture final:EndingDestination {
 EndingScriptFixture& owner;FrameScheduler scheduler;EndingFrame frame;i32 destination=-1;
 explicit EndingFrameFixture(EndingScriptFixture& value):owner(value),frame(value.ending,*this,scheduler){}
 bool ending_destination(i32 value)override{destination=value;return true;}
};
API(ending_frame_create) EndingFrameFixture* ending_frame_create(EndingScriptFixture* p){return new EndingFrameFixture(*p);}
API(ending_frame_delete) void ending_frame_delete(EndingFrameFixture* p){delete p;}
API(ending_frame_update) i32 ending_frame_update(EndingFrameFixture* p,u32 held,u32 pressed,u32 held_frames,float rate,u32 system){p->destination=-1;p->owner.events.clear();p->owner.texts.clear();p->frame.controls({held,pressed,held_frames,rate},system);return p->frame.update();}
API(ending_frame_fields) i32 ending_frame_fields(EndingFrameFixture* p,u32 kind){return kind?p->destination:p->frame.frames;}
API(ending_frame_error) const char* ending_frame_error(EndingFrameFixture* p){return p->frame.error.c_str();}

