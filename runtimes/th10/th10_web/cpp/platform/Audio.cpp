#include "Audio.hpp"
#include "AudioData.hpp"
#include <cstdlib>
#ifdef TH_SDL3
extern "C" void audio_host_attach(th10::browser::Audio*,bool);
#endif
namespace th10::browser {
namespace {
u32 pointer(const void* value){return static_cast<u32>(reinterpret_cast<uintptr_t>(value));}
constexpr u32 buffer_interface[4]={0x279afa86,0x11ce4981,0x200021a5,0x60e50baf};
constexpr u32 notification_interface[4]={0xb0210783,0x11d089cd,0xa00008af,0x16cd25c9};
}
AudioWaves::AudioWaves(Audio& a):owner(a){archive_offset=&a.manager.archive_offset;}
u32 AudioWaves::open_file(const char* name){return owner.files.host.open(name,false);}
void AudioWaves::seek_file(u32 h,u32 p){owner.files.host.seek(h,static_cast<i32>(p),0);}
void AudioWaves::read_file(u32 h,u8* b,u32 n,u32* actual){*actual=owner.files.host.read(h,b,n);}
void AudioWaves::close_file(u32 h){owner.files.host.close(h);}
AudioBuffers::AudioBuffers(Audio& a):owner(a){music_volume=&a.manager.music_volume;waves=&a.waves;}
i32 AudioBuffers::status(void* b,u32* flags){return owner.call(b,AudioOperation::Status,{pointer(flags)});}
i32 AudioBuffers::restore(void* b){return owner.call(b,AudioOperation::Restore);}
void AudioBuffers::sleep(u32 ms){owner.advance(ms);}
u32 AudioBuffers::random_buffer(){owner.random_state=owner.random_state*214013u+2531011u;return (owner.random_state>>16)&32767;}
i32 AudioBuffers::volume(void* b,i32 gain){return owner.call(b,AudioOperation::Volume,{static_cast<u32>(gain)});}
i32 AudioBuffers::play(void* b,u32 priority,u32 flags){return owner.call(b,AudioOperation::Play,{0,priority,flags});}
i32 AudioBuffers::stop(void* b){return owner.call(b,AudioOperation::Stop);}
i32 AudioBuffers::position(void* b,u32 offset){return owner.call(b,AudioOperation::Position,{offset});}
i32 AudioBuffers::refill(SoundBuffer& s,void* b,bool repeat){return s.fill(b,repeat,*this);}
i32 AudioBuffers::query(void* b,void** output){return owner.call(b,AudioOperation::Query,{pointer(buffer_interface),pointer(output)});}
i32 AudioBuffers::lock(void* b,u32 offset,u32 bytes,SoundLock& lock,bool split){return owner.call(b,AudioOperation::Lock,{offset,bytes,pointer(&lock.first),pointer(&lock.first_bytes),split?pointer(&lock.second):0,split?pointer(&lock.second_bytes):0,0});}
i32 AudioBuffers::unlock(void* b,const SoundLock& lock){return owner.call(b,AudioOperation::Unlock,{pointer(lock.first),lock.first_bytes,0,0});}
i32 AudioBuffers::current_position(void* b,u32* play,u32* write){return owner.call(b,AudioOperation::CurrentPosition,{pointer(play),pointer(write)});}
AudioSounds::AudioSounds(Audio& a):owner(a){audio=&a.buffers;buffer_vtable=1;stream_vtable=2;}
void* AudioSounds::allocate(u32 n){return std::malloc(n);}
void AudioSounds::free(void* p){std::free(p);}
void AudioSounds::release(void* p){if(p)owner.call(p,AudioOperation::Release);}
i32 AudioSounds::create_buffer(void* d,const SoundBufferDescription& desc,void** output){return owner.call(d,AudioOperation::CreateBuffer,{pointer(&desc),pointer(output),0});}
i32 AudioSounds::query_notifications(void* b,void** output){return owner.call(b,AudioOperation::Query,{pointer(notification_interface),pointer(output)});}
i32 AudioSounds::set_notifications(void* b,u32 count,const SoundNotification* positions){return owner.call(b,AudioOperation::Notifications,{count,pointer(positions)});}
AudioControls::AudioControls(Audio& a):owner(a){display_flags=&a.display_flags;music_enabled=&a.music_enabled;effects_enabled=&a.effects_enabled;effects_volume=&a.manager.effects_volume;definitions=sound_definitions;global_music=&a.manager.music;}
void AudioControls::prepare_track(AudioManager& m,i32 i,const char* n){AudioResources{m,owner.resources}.prepare_track(i,n);}
i32 AudioControls::start_track(AudioManager& m,i32 i){return AudioResources{m,owner.resources}.start_track(i);}
void AudioControls::stop_music(NotifiedSoundStream& m){m.stop_all(owner.buffers);}
i32 AudioControls::reset_music(NotifiedSoundStream& m){return m.reset(owner.buffers);}
i32 AudioControls::fill_music(NotifiedSoundStream& m,void* b,bool repeat){return m.fill(b,repeat,owner.buffers);}
void AudioControls::recreate_music(NotifiedSoundStream& m){SoundResources{owner.sounds}.recreate_notifications(m);}
void AudioControls::select_music(WaveReader& w,const MusicFormat* f){w.select(f,owner.waves);}
void AudioControls::play_music(NotifiedSoundStream& m){m.play(0,1,owner.buffers);}
void AudioControls::pause_music(NotifiedSoundStream& m){m.stop_first(owner.buffers);}
void AudioControls::resume_music(NotifiedSoundStream& m){m.resume(owner.buffers);}
void AudioControls::volume_music(NotifiedSoundStream& m,i32 gain){m.set_volume(gain,owner.buffers);}
void AudioControls::delete_music(NotifiedSoundStream& m){SoundResources{owner.sounds}.release(m);std::free(&m);}
void AudioControls::quit_thread(u32 id){if(auto* task=owner.task(id))task->quit=true;}
u32 AudioControls::wait_thread(u32 id,u32){return owner.wait_task(id);}
void AudioControls::close_handle(u32 id){
    if(owner.task(id))owner.close_task(id);else owner.host.event(2,id);
    // Command 4 has already zeroed the joined thread's public handle. Native
    // task storage can retire that completed worker without a leaked OS handle.
    if(!id)for(auto& task:owner.tasks)if(task.id&&task.kind==AudioTaskKind::Music&&task.done)task={};
}
void AudioControls::effect_stop(void* b){owner.buffers.stop(b);}
void AudioControls::effect_position(void* b,u32 p){owner.buffers.position(b,p);}
void AudioControls::effect_pan(void* b,i32 p){owner.call(b,AudioOperation::Pan,{static_cast<u32>(p)});}
void AudioControls::effect_volume(void* b,i32 v){owner.buffers.volume(b,v);}
void AudioControls::effect_play(void* b){owner.buffers.play(b,0,0);}
AudioResourcesHost::AudioResourcesHost(Audio& a):owner(a){display_flags=&a.display_flags;worker_argument=&a.window;music_enabled=&a.music_enabled;global_music_names=a.manager.music_names;files=&a.waves;}
void AudioResourcesHost::unload_music(AudioManager& m){AudioControl{m,owner.controls}.unload_music();}
i32 AudioResourcesHost::select_music(AudioManager& m,const char* n){return AudioControl{m,owner.controls}.select_music(n);}
i32 AudioResourcesHost::track_index(AudioManager& m,const char* n){return AudioControl{m,owner.controls}.track_index(n);}
u8* AudioResourcesHost::allocate_cache(u32 n){return static_cast<u8*>(std::malloc(n));}
void AudioResourcesHost::free_cache(u8* p){std::free(p);}
u32 AudioResourcesHost::create_event(){return owner.host.event(0,0);}
u32 AudioResourcesHost::create_worker(u32,u32* id){return *id=owner.create_task(AudioTaskKind::Music);}
i32 AudioResourcesHost::create_file_stream(void* d,NotifiedSoundStream** output,const char* name,const MusicFormat* f,u32 chunk,u32 event){return SoundResources{owner.sounds}.create_file_stream(output,static_cast<void**>(d),0x10100,name,sound_algorithm,16,chunk,event,f);}
i32 AudioResourcesHost::create_memory_stream(void* d,NotifiedSoundStream** output,const u8* data,u32 bytes,const MusicFormat* f,u32 chunk,u32 event){return SoundResources{owner.sounds}.create_memory_stream(output,static_cast<void**>(d),0x10100,data,bytes,f,sound_algorithm,16,chunk,event);}
AudioDevices::AudioDevices(Audio& a):owner(a){resources=&a.sounds;load_stop=&a.manager.load_stop;global_manager=&a.manager;definitions=sound_definitions;configured_music=&a.configured_music;configured_effects=&a.configured_effects;source_names=sound_names;device_worker_callback=static_cast<u32>(AudioTaskKind::Device);source_worker_callback=static_cast<u32>(AudioTaskKind::Sources);}
void AudioDevices::sleep(u32 ms){owner.advance(ms);}
void AudioDevices::free_source(void* p){std::free(p);}
void AudioDevices::unlock_source(void* b,const SoundLock& l){owner.call(b,AudioOperation::Unlock,{pointer(l.first),l.first_bytes,pointer(l.second),l.second_bytes});}
void AudioDevices::invalid_source(bool,const char*){owner.error=-1;}
u32 AudioDevices::begin_thread(CallbackToken type,void*,u32,u32& id){return id=owner.create_task(static_cast<AudioTaskKind>(type));}
u32 AudioDevices::wait_thread(u32 id,u32){return owner.wait_task(id);}
void AudioDevices::close_thread(u32 id){owner.close_task(id);}
void AudioDevices::join_loaders(AudioManager& m){AudioDevice{m,*this}.join_loading();}
void AudioDevices::unload_music(AudioManager& m){AudioControl{m,owner.controls}.unload_music();}
void AudioDevices::delete_music(NotifiedSoundStream& m){owner.controls.delete_music(m);}
i32 AudioDevices::create_device(void** output){*output=reinterpret_cast<void*>(static_cast<uintptr_t>(owner.host.create()));return *output?0:-1;}
i32 AudioDevices::cooperative_level(void* d,u32 window){return owner.call(d,AudioOperation::Cooperative,{window,2});}
i32 AudioDevices::set_format(void* b,const u8* f){return owner.call(b,AudioOperation::Format,{pointer(f)});}
void AudioDevices::duplicate_buffer(void* d,void* source,void** output){owner.call(d,AudioOperation::Duplicate,{pointer(source),pointer(output)});}
void AudioDevices::begin_timer(u32){owner.timer_active=true;}
void AudioDevices::end_timer(u32){owner.timer_active=false;}
i32 AudioDevices::load_source(AudioManager& m,i32 i,const char* n){return SoundSources{m,*this}.load(i,n);}
void AudioDevices::log_device(u32 message,const char*){if(message<2)owner.error=-1;}
AudioWorkerEnvironment::AudioWorkerEnvironment(Audio& a):owner(a){source_names=sound_names;current=&a.manager.music;}
u8* AudioWorkerEnvironment::read_wave(const char* name){return ResourceFiles{owner.files}.load(name,nullptr,false);}
void AudioWorkerEnvironment::missing_wave(const char*){owner.error=-1;owner.manager.load_stop=2;}
bool AudioWorkerEnvironment::next_message(u32& message){if(owner.active_task&&owner.active_task->quit){owner.active_task->quit=false;message=0x12;return true;}return false;}
void AudioWorkerEnvironment::fill_stream(NotifiedSoundStream& m){m.update(true,owner.buffers);}
Audio::Audio(AudioHost& h,FileSystem& f):host(h),files(f),waves(*this),buffers(*this),sounds(*this),controls(*this),resources(*this),devices(*this),workers(*this){
#ifdef TH_SDL3
    audio_host_attach(this,true);
#endif
}
Audio::~Audio(){release();
#ifdef TH_SDL3
    audio_host_attach(this,false);
#endif
}
i32 Audio::call(void* h,AudioOperation operation,std::initializer_list<u32> args){return host.call(pointer(h),operation,args.begin());}
AudioTask* Audio::task(u32 id){if(!id)return nullptr;for(auto& task:tasks)if(task.id==id)return &task;return nullptr;}
u32 Audio::create_task(AudioTaskKind kind){for(auto& task:tasks)if(!task.id){task={};task.id=next_task++;task.kind=kind;return task.id;}error=-1;return 0;}
void Audio::pump(){
    if(pumping)return;pumping=true;
    // Producers run before consumers. Each task retains its own native C++
    // progress between calls, including the original loader completion flag.
    for(const auto kind:{AudioTaskKind::Sources,AudioTaskKind::Device,AudioTaskKind::Music})for(auto& t:tasks){
        if(!t.id||t.done||t.kind!=kind)continue;active_task=&t;
        if(kind==AudioTaskKind::Sources)t.done=t.sources.advance(manager,workers);
        else if(kind==AudioTaskKind::Device)t.done=t.device.advance(devices);
        else if(t.quit)t.done=t.music.complete_wait(1,workers);
        else if(manager.music&&host.event(1,manager.notification_event)==0)t.done=t.music.complete_wait(0,workers);
    }
    active_task=nullptr;pumping=false;
}
u32 Audio::wait_task(u32 id){pump();auto* t=task(id);return !t?0xffffffffu:t->done?0:0x102;}
void Audio::close_task(u32 id){if(auto* t=task(id))*t={};}
void Audio::advance(u32 ms){host.advance(ms);pump();}
i32 Audio::load_formats(const char* name){
    if(manager.music)return -1;u32 size=0;auto* bytes=ResourceFiles{files}.load(name,&size,false);
    if(!bytes||size<sizeof(MusicFormat)){std::free(bytes);return -1;}
    // The resource ends in a sentinel used by track_index. Reject malformed
    // metadata instead of permitting an unbounded native pointer traversal.
    u32 end=0;while(end<size&&bytes[end]){if(size-end<sizeof(MusicFormat)||!std::memchr(bytes+end,0,16)){std::free(bytes);return -1;}end+=sizeof(MusicFormat);}
    if(end==size){std::free(bytes);return -1;}
    std::free(manager.formats);manager.formats=reinterpret_cast<MusicFormat*>(bytes);return 0;
}
i32 Audio::begin_loading(u32 w){
    release();error=0;window=w;AudioDevice{manager,devices}.begin_loading(w);AudioDevice{manager,devices}.begin_source_loading();
    pump();if(error){release();return -1;}return 0;
}
i32 Audio::finish_loading(){const i32 result=AudioDevice{manager,devices}.initialize_effects();return error?error:result;}
i32 Audio::initialize(u32 w){const i32 result=begin_loading(w);return result?result:finish_loading();}
void Audio::release(){
    manager.load_stop=2;AudioDevice{manager,devices}.join_loading();AudioDevice{manager,devices}.release();
    // Original command 4 clears music_thread before CloseHandle. Its worker is
    // already joined; retire all remaining finished task records on shutdown.
    for(auto& task:tasks)task={};if(manager.notification_event)host.event(2,manager.notification_event);manager.notification_event=0;
}
i32 Audio::update(){return AudioControl{manager,controls}.update();}
void Audio::advance_fades(){auto* current=static_cast<SoundBuffer*>(manager.music);SoundBuffer::advance_fades(&current,buffers);}
}
