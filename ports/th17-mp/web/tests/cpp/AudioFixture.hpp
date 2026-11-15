#include "../../cpp/game/SoundEffects.hpp"
struct SoundEffectsFixture final:SoundOutput {
 std::array<u8,80> available{},playing{};std::vector<i32> events;SoundEffects effects{*this};std::array<i32,80*4> projection{};
 SoundEffectsFixture(){available.fill(1);}
 bool sound_available(u32 id)const override{return available[id]!=0;}
 bool sound_playing(u32 id)override{events.insert(events.end(),{1,i32(id),playing[id]});return playing[id]!=0;}
 void sound_stop(u32 id)override{events.insert(events.end(),{2,i32(id)});playing[id]=0;}
 void sound_position(u32 id,u32 frame)override{events.insert(events.end(),{3,i32(id),i32(frame)});}
 void sound_pan(u32 id,i32 value)override{events.insert(events.end(),{4,i32(id),value});}
 void sound_volume(u32 id,i32 value)override{events.insert(events.end(),{5,i32(id),value});}
 void sound_play(u32 id,u32 flags)override{events.insert(events.end(),{6,i32(id),i32(flags)});playing[id]=1;}
 const i32* fields(){for(u32 id=0;id<80;id++){auto& v=effects.voices[id];projection[id*4]=v.renewal;projection[id*4+1]=v.pan;projection[id*4+2]=v.resume;projection[id*4+3]=i32(v.definition-sound_definitions.data());}return projection.data();}
};
API(sound_effects_create) SoundEffectsFixture* sound_effects_create(){return new SoundEffectsFixture;}
API(sound_effects_delete) void sound_effects_delete(SoundEffectsFixture* p){delete p;}
API(sound_effects_enqueue) bool sound_effects_enqueue(SoundEffectsFixture* p,i32 id,i32 pan){return p->effects.enqueue(id,pan);}
API(sound_effects_positioned) bool sound_effects_positioned(SoundEffectsFixture* p,i32 id,float x){return p->effects.positioned(id,x);}
API(sound_effects_stop) bool sound_effects_stop(SoundEffectsFixture* p,i32 id){p->events.clear();return p->effects.stop(id);}
API(sound_effects_process) void sound_effects_process(SoundEffectsFixture* p){p->events.clear();p->effects.process();}
API(sound_effects_suspend) void sound_effects_suspend(SoundEffectsFixture* p){p->events.clear();p->effects.suspend();}
API(sound_effects_resume) void sound_effects_resume(SoundEffectsFixture* p){p->events.clear();p->effects.resume();}
API(sound_effects_volume) i32 sound_effects_volume(i32 base,i32 master){return SoundEffects::adjusted_volume(base,master);}
API(sound_effects_configure) void sound_effects_configure(SoundEffectsFixture* p,bool enabled,i32 volume){p->effects.enabled=enabled;p->effects.master_volume=volume;}
API(sound_effects_queue) SoundQueue* sound_effects_queue(SoundEffectsFixture* p){return &p->effects.state;}
API(sound_effects_voices) const i32* sound_effects_voices(SoundEffectsFixture* p){return p->fields();}
API(sound_effects_available) u8* sound_effects_available(SoundEffectsFixture* p){return p->available.data();}
API(sound_effects_playing) u8* sound_effects_playing(SoundEffectsFixture* p){return p->playing.data();}
API(sound_effects_event_count) u32 sound_effects_event_count(SoundEffectsFixture* p){return p->events.size();}
API(sound_effects_events) const i32* sound_effects_events(SoundEffectsFixture* p){return p->events.data();}
API(sound_effects_error) const char* sound_effects_error(SoundEffectsFixture* p){return p->effects.error.c_str();}

#include "../../cpp/game/MusicLayout.hpp"
API(music_layout_create) MusicLayout* music_layout_create(){return new MusicLayout;}
API(music_layout_delete) void music_layout_delete(MusicLayout* p){delete p;}
API(music_layout_open) bool music_layout_open(MusicLayout* p,const u8* bytes,u32 size){return p->open(bytes,size);}
API(music_layout_count) u32 music_layout_count(MusicLayout* p){return p->count();}
API(music_layout_index) i32 music_layout_index(MusicLayout* p,const char* value){return p->original_index(value);}
API(music_layout_error) const char* music_layout_error(MusicLayout* p){return p->error.c_str();}
API(music_layout_name) const char* music_layout_name(MusicLayout* p,u32 i){auto* t=p->track(i);return t?t->filename.c_str():"";}
API(music_layout_fields) const u32* music_layout_fields(MusicLayout* p,u32 i){static std::array<u32,10> v;auto* t=p->track(i);if(!t)return nullptr;v={t->offset,t->length,t->loop_start,t->loop_end,t->rate,t->channels,t->alignment,t->bits,u32(t->frames()),u32(t->loop_frame())};return v.data();}
API(music_adjusted_volume) i32 music_adjusted_volume(i32 base,i32 master){return adjusted_music_volume(base,master);}

#include "../../cpp/game/MusicCommands.hpp"
struct MusicCommandsFixture final:MusicCommandOutput {
 MusicLayout layout;MusicCommands commands{layout,*this};bool ready=true,busy=false,release=false,waiting=false,cached=false;i32 fill_result=0,fade=0;std::vector<i32> events;
 bool music_ready()const override{return ready;}bool music_busy()const override{return busy;}bool music_has_intro()const override{return true;}bool release_pending()const override{return release;}bool release_waiting()override{if(!waiting)release=false;return waiting;}
 static i32 hash(const std::string& value){u32 n=2166136261;for(u8 b:value){n^=b;n*=16777619;}return signed_bits(n);}
 void preload_reset()override{events.push_back(9);}bool prepare_music(i32 slot,const std::string& value)override{events.insert(events.end(),{11,slot,hash(value)});return true;}
 bool cached_music(i32 slot)override{events.insert(events.end(),{10,slot});return cached;}
 void stop_music_stream(bool close)override{fade=0;events.insert(events.end(),{1,close?1:0});}
 void rewind_music_stream()override{events.push_back(14);}
 bool load_music(const std::string& file,i32 index)override{events.insert(events.end(),{2,index});return true;}void select_music(i32 index)override{events.insert(events.end(),{3,index});}
 i32 fill_music(bool alternate,bool initial)override{events.insert(events.end(),{4,alternate?1:0,initial?1:0});return fill_result;}
 void start_music_stream()override{events.push_back(5);}void signal_music_release()override{events.push_back(12);}void close_music_stream()override{events.push_back(13);ready=false;}
 void fade_music_stream(i32 frames)override{fade=frames;}void pause_music_stream(bool pause)override{events.insert(events.end(),{7,pause?1:0});}
 void refresh_music_volume()override{events.push_back(6);}void switch_music_wave(i32 index)override{events.insert(events.end(),{8,index});}
};
API(music_commands_create) MusicCommandsFixture* music_commands_create(){return new MusicCommandsFixture;}
API(music_commands_delete) void music_commands_delete(MusicCommandsFixture* p){delete p;}
API(music_commands_layout) bool music_commands_layout(MusicCommandsFixture* p,const u8* bytes,u32 size){return p->layout.open(bytes,size);}
API(music_commands_enqueue) bool music_commands_enqueue(MusicCommandsFixture* p,i32 code,i32 value,const char* text){return p->commands.enqueue(code,value,text);}
API(music_commands_process) bool music_commands_process(MusicCommandsFixture* p){p->events.clear();return p->commands.process();}
API(music_commands_configure) void music_commands_configure(MusicCommandsFixture* p,bool alternate,u32 mode,bool ready,bool busy,bool release,bool waiting,bool cached,i32 fill){p->commands.alternate=alternate;p->commands.music_mode=u8(mode);p->ready=ready;p->busy=busy;p->release=release;p->waiting=waiting;p->cached=cached;p->fill_result=fill;}
API(music_commands_queue) const MusicRequest* music_commands_queue(MusicCommandsFixture* p){return p->commands.requests.data();}
API(music_commands_count) u32 music_commands_count(MusicCommandsFixture* p){return p->events.size();}
API(music_commands_events) const i32* music_commands_events(MusicCommandsFixture* p){return p->events.data();}
API(music_commands_wave) const char* music_commands_wave(MusicCommandsFixture* p){return p->commands.current_wave.c_str();}
API(music_commands_prepared) const char* music_commands_prepared(MusicCommandsFixture* p,u32 slot){return p->commands.prepared[slot].c_str();}
API(music_commands_fade) i32 music_commands_fade(MusicCommandsFixture* p){return p->fade;}
API(music_commands_ready) bool music_commands_ready(MusicCommandsFixture* p){return p->ready;}
API(music_commands_release) bool music_commands_release(MusicCommandsFixture* p){return p->release;}
API(music_commands_error) const char* music_commands_error(MusicCommandsFixture* p){return p->commands.error.c_str();}
