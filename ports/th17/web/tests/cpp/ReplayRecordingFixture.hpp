#include "../../cpp/game/ReplayRecording.hpp"
struct ReplayRecordingFixture {ReplayRecording recording;ReplayExportDetails details;};
API(recording_create) ReplayRecordingFixture* recording_create(){return new ReplayRecordingFixture;}
API(recording_delete) void recording_delete(ReplayRecordingFixture* p){delete p;}
API(recording_metadata) u8* recording_metadata(ReplayRecordingFixture* p){return p->recording.description().data();}
API(recording_details) ReplayExportDetails* recording_details(ReplayRecordingFixture* p){return &p->details;}
API(recording_begin) i32 recording_begin(ReplayRecordingFixture* p,const u8* h,u32 n){return p->recording.begin(h,n);}
API(recording_activate) void recording_activate(ReplayRecordingFixture* p){p->recording.activate();}
API(recording_web_rules) void recording_web_rules(ReplayRecordingFixture* p){p->recording.use_current_web_rules();}
API(recording_tick) i32 recording_tick(ReplayRecordingFixture* p,u32 held,u32 pressed,u32 released,float fps,u32 paused){return p->recording.tick({u16(held),u16(pressed),u16(released)},fps,paused);}
API(recording_finish) void recording_finish(ReplayRecordingFixture* p,i64 timestamp,i32 stage,u32 clear){p->recording.finish(timestamp,stage,clear);}
API(recording_write) i32 recording_write(ReplayRecordingFixture* p,const char* name,u32 terminate){return p->recording.write(name,p->details,terminate);}
API(recording_data) const u8* recording_data(ReplayRecordingFixture* p){return p->recording.output().data();}
API(recording_size) u32 recording_size(ReplayRecordingFixture* p){return p->recording.output().size();}
API(recording_clock) i32 recording_clock(ReplayRecordingFixture* p){return p->recording.frame_clock();}
API(recording_error) const char* recording_error(ReplayRecordingFixture* p){return p->recording.error().c_str();}
API(recording_touch_tick) i32 recording_touch_tick(ReplayRecordingFixture* p,u32 held,u32 pressed,u32 released,float fps,u32 paused,i32 mode,float x,float y){return p->recording.tick({u16(held),u16(pressed),u16(released)},fps,paused,{mode,x,y});}
API(recording_uses_touch) u32 recording_uses_touch(ReplayRecordingFixture* p){return p->recording.uses_touch();}
