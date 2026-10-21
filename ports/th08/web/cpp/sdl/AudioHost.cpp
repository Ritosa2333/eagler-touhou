#ifdef TH_NATIVE_PLATFORM
// The same miniaudio -> SDL stream boundary used by the TH07/TH10 hosts.
// MusicControl, MIDI sequencing, fades and SoundEffects remain TH08 owners.
#define MA_NO_DEVICE_IO
#define MA_NO_RESOURCE_MANAGER
#define MA_NO_WAV
#define MA_NO_MP3
#define MA_NO_FLAC
#define MA_NO_ENCODING
#define MA_NO_THREADING
// File-backed Vorbis supports exact PCM seeks and the original loop boundaries.
#include "../../../portable/sdl/third_party/stb_vorbis.h"
#define MINIAUDIO_IMPLEMENTATION
#include "../../../portable/sdl/third_party/miniaudio.h"
#include "PlatformHost.hpp"
#include "AudioAudit.hpp"
#include "../platform/PlatformDevices.hpp"
#include <SDL3/SDL.h>
#include <emscripten.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include <eagler/audio/WebSfx.hpp>
namespace web_sfx = eagler::audio::web_sfx;
EM_JS(void, th08_midi_event, (int op,const unsigned char* data,unsigned length,unsigned time), {
    if(Module['runtimeMidi'])Module['runtimeMidi'](op,data?HEAPU8.slice(data,data+length):null,time);
});
EM_JS(int, th08_ogg_file_exists, (const char* path), {
    try{return Module['FS']&&Module['FS'].analyzePath(UTF8ToString(path)).exists?1:0;}catch(error){return 0;}
});
EM_JS(void, th08_music_resource_notice, (const char* message), {
    if(Module['runtimeNotice'])Module['runtimeNotice'](UTF8ToString(message));
});
EM_JS(int, th08_sfx_latency_probe_is_enabled, (), {
    return globalThis['__th08SfxLatencyProbeEnabled']===true?1:0;
});
EM_JS(double, th08_sfx_latency_context_time, (), {
    if(globalThis['__th08SfxLatencyProbeEnabled']!==true)return NaN;
    const context=Module['SDL3']&&Module['SDL3'].audioContext;
    const now=context?context.currentTime:null;
    return Number.isFinite(now)?now:NaN;
});
// Each opt-in stream is bounded to 512 rows; overflow is counted and invalidates evidence.
EM_JS(void, th08_sfx_latency_upload, (unsigned handle,unsigned voice_key,unsigned source_rate,unsigned source_frames,unsigned channels,int uploaded), {
    if(globalThis['__th08SfxLatencyProbeEnabled']!==true)return;
    const context=Module['SDL3']&&Module['SDL3'].audioContext;
    const now=context?context.currentTime:null,sampleRate=context?context.sampleRate:null;
    const contextTime=Number.isFinite(now)?now:null;
    const contextFrame=contextTime!==null&&Number.isFinite(sampleRate)?contextTime*sampleRate:null;
    const trace=globalThis['__th08SfxLatencyTrace']||(globalThis['__th08SfxLatencyTrace']={sequence:0,recordLimit:512,uploads:[],events:[],routes:[],errors:[],dropped:{uploads:0,events:0,routes:0,errors:0}});
    const append=(kind,row)=>{if(trace[kind].length<trace.recordLimit)trace[kind].push(row);else ++trace.dropped[kind];};
    const notify=(name,row)=>{try{globalThis['__th08SfxLatencyProbe']?.[name]?.(row);}catch(error){append('errors',{callback:name,message:String(error&&error.message||error)});}};
    const row={serial:++trace.sequence,handle,voiceKey:String(voice_key),sourceRate:source_rate,sourceFrames:source_frames,
        channels,uploadSuccess:!!uploaded,contextSampleRate:sampleRate,
        contextTime,contextFrame};
    append('uploads',row);notify('onUpload',row);
});
EM_JS(void, th08_sfx_latency_event_trace, (unsigned index,unsigned handle,unsigned voice_key,unsigned queue_frames,unsigned queue_rate,unsigned source_rate,double clock_before,double clock_after), {
    if(globalThis['__th08SfxLatencyProbeEnabled']!==true)return;
    const context=Module['SDL3']&&Module['SDL3'].audioContext;
    const sampleRate=context?context.sampleRate:null;
    const queueClockBeforeTime=Number.isFinite(clock_before)?clock_before:null;
    const queueClockAfterTime=Number.isFinite(clock_after)?clock_after:null;
    const contextTime=queueClockBeforeTime;
    const contextFrame=contextTime!==null&&Number.isFinite(sampleRate)?contextTime*sampleRate:null;
    const queueClockBeforeFrame=queueClockBeforeTime!==null&&Number.isFinite(sampleRate)?queueClockBeforeTime*sampleRate:null;
    const queueClockAfterFrame=queueClockAfterTime!==null&&Number.isFinite(sampleRate)?queueClockAfterTime*sampleRate:null;
    const trace=globalThis['__th08SfxLatencyTrace']||(globalThis['__th08SfxLatencyTrace']={sequence:0,recordLimit:512,uploads:[],events:[],routes:[],errors:[],dropped:{uploads:0,events:0,routes:0,errors:0}});
    const append=(kind,row)=>{if(trace[kind].length<trace.recordLimit)trace[kind].push(row);else ++trace.dropped[kind];};
    const notify=(name,row)=>{try{globalThis['__th08SfxLatencyProbe']?.[name]?.(row);}catch(error){append('errors',{callback:name,message:String(error&&error.message||error)});}};
    const row={serial:++trace.sequence,logicalIndex:index,handle,voiceKey:String(voice_key),queueFrames:queue_frames,
        queueRate:queue_rate,sourceRate:source_rate,contextSampleRate:sampleRate,
        contextTime,contextFrame,queueClockBeforeTime,queueClockAfterTime,queueClockBeforeFrame,queueClockAfterFrame,
        queueSampleOrder:'cpp-sdl-queue-clock-bracket'};
    append('events',row);notify('onNativeEvent',row);
});
EM_JS(void, th08_sfx_latency_route_trace, (unsigned handle,unsigned voice_key,unsigned queue_frames,unsigned queue_rate,int direct,int fallback,int fallback_started,int pending,double clock_before,double clock_after), {
    if(globalThis['__th08SfxLatencyProbeEnabled']!==true)return;
    const context=Module['SDL3']&&Module['SDL3'].audioContext;
    const sampleRate=context?context.sampleRate:null;
    const queueClockBeforeTime=Number.isFinite(clock_before)?clock_before:null;
    const queueClockAfterTime=Number.isFinite(clock_after)?clock_after:null;
    const contextTime=queueClockBeforeTime;
    const contextFrame=contextTime!==null&&Number.isFinite(sampleRate)?contextTime*sampleRate:null;
    const queueClockBeforeFrame=queueClockBeforeTime!==null&&Number.isFinite(sampleRate)?queueClockBeforeTime*sampleRate:null;
    const queueClockAfterFrame=queueClockAfterTime!==null&&Number.isFinite(sampleRate)?queueClockAfterTime*sampleRate:null;
    const trace=globalThis['__th08SfxLatencyTrace']||(globalThis['__th08SfxLatencyTrace']={sequence:0,recordLimit:512,uploads:[],events:[],routes:[],errors:[],dropped:{uploads:0,events:0,routes:0,errors:0}});
    const append=(kind,row)=>{if(trace[kind].length<trace.recordLimit)trace[kind].push(row);else ++trace.dropped[kind];};
    const notify=(name,row)=>{try{globalThis['__th08SfxLatencyProbe']?.[name]?.(row);}catch(error){append('errors',{callback:name,message:String(error&&error.message||error)});}};
    let event=null;
    for(const candidate of trace.events){if(candidate.handle===handle&&candidate.voiceKey===String(voice_key)&&!candidate.routeSerial){event=candidate;break;}}
    const serial=++trace.sequence;if(event)event.routeSerial=serial;
    const row={serial,eventSerial:event?.serial??null,logicalIndex:event?.logicalIndex??null,handle,
        voiceKey:String(voice_key),direct:!!direct,fallback:!!fallback,fallbackStarted:!!fallback_started,pending:!!pending,
        queueFrames:queue_frames,queueRate:queue_rate,contextSampleRate:sampleRate,
        contextTime,contextFrame,queueClockBeforeTime,queueClockAfterTime,queueClockBeforeFrame,queueClockAfterFrame,
        queueSampleOrder:'cpp-sdl-queue-clock-bracket'};
    append('routes',row);notify('onRoute',row);
});
EM_JS(void, th08_sfx_latency_reset_trace, (int shutdown), {
    if(globalThis['__th08SfxLatencyProbeEnabled']!==true){delete globalThis['__th08SfxLatencyTrace'];return;}
    globalThis['__th08SfxLatencyTrace']={sequence:0,recordLimit:512,uploads:[],events:[],routes:[],errors:[],dropped:{uploads:0,events:0,routes:0,errors:0}};
    if(shutdown)try{globalThis['__th08SfxLatencyProbe']?.onAudioShutdown?.();}catch(error){globalThis['__th08SfxLatencyTrace'].errors.push({callback:'onAudioShutdown',message:String(error&&error.message||error)});}
});
namespace th08 {
namespace {
struct Buffer {
    WaveFormat format{};std::vector<u8> pcm;std::vector<u8> full_pcm;ma_audio_buffer data{};ma_decoder decoder{};ma_sound sound{};
    bool data_ready=false,decoder_ready=false,sound_ready=false;bool loop=false;bool music=false,pending=false,requested=false,diagnostic_sent=false;
    bool web_sfx_uploaded=false;u32 web_sfx_voice_key=0;u32 cursor=0,intro_frames=0,total_frames=0;std::string name;i32 volume=0,pan=0;
    void clear_source(){audit::audio_operation(audit::AudioOperation::ClearSource);web_sfx::erase(&sound);if(sound_ready){ma_sound_uninit(&sound);sound_ready=false;}if(data_ready){ma_audio_buffer_uninit(&data);data_ready=false;}if(decoder_ready){ma_decoder_uninit(&decoder);decoder_ready=false;}full_pcm.clear();}
    ~Buffer(){clear_source();}
};
struct Audio {
    ma_engine engine{};SDL_AudioStream* stream=nullptr;bool ready=false,paused=false,music=true,ogg_full=false,ogg_resources_complete=false,refill=true;u32 error=0,pumps=0,mixed=0,min_queue=~0u,resource_changes=0;float rms=0;
    float music_speed=1.f;u32 sample_rate=44100;bool sfx_probe=false;std::map<u32,std::unique_ptr<Buffer>> buffers;
    bool initialize(){if(ready)return true;audit::audio_operation(audit::AudioOperation::Initialize);sfx_probe=th08_sfx_latency_probe_is_enabled()!=0;if(sfx_probe)th08_sfx_latency_reset_trace(0);auto config=ma_engine_config_init();config.noDevice=MA_TRUE;config.channels=2;config.sampleRate=sample_rate;config.defaultVolumeSmoothTimeInPCMFrames=0;
        if(ma_engine_init(&config,&engine)!=MA_SUCCESS){error=1;return false;}
        // SDL Emscripten doubles this request to a 2048-frame output block.
        SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES,"1024");SDL_AudioSpec spec{SDL_AUDIO_F32,2,static_cast<int>(sample_rate)};
        if(SDL_InitSubSystem(SDL_INIT_AUDIO))stream=SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,&spec,nullptr,nullptr);
        if(!stream){ma_engine_uninit(&engine);error=2;return false;}ready=true;web_sfx::suspend(paused);SDL_ResumeAudioStreamDevice(stream);return true;}
    bool attach(Buffer& b,ma_data_source* source){if(ma_sound_init_from_data_source(&engine,source,MA_SOUND_FLAG_NO_SPATIALIZATION,nullptr,&b.sound)!=MA_SUCCESS)return false;b.sound_ready=true;return true;}
    void controls(Buffer& b){if(b.music)ma_sound_set_pitch(&b.sound,music_speed);const float gain=std::pow(10.f,float(b.volume)/2000.f),pan=b.pan>=0?1.f-std::pow(10.f,-float(b.pan)/2000.f):std::pow(10.f,float(b.pan)/2000.f)-1.f;
        web_sfx::volume(&b.sound,gain);web_sfx::pan(&b.sound,pan);ma_sound_set_volume(&b.sound,gain);ma_sound_set_pan_mode(&b.sound,ma_pan_mode_balance);ma_sound_set_pan(&b.sound,pan);}
    void pump(){audit::audio_operation(audit::AudioOperation::Pump);if(!ready||paused)return;auto queued=std::max(0,SDL_GetAudioStreamQueued(stream))/8;min_queue=std::min(min_queue,u32(queued));
        constexpr int low=2048,high=3072,chunk=1024;
        if(!refill&&queued<low)refill=true;if(!refill)return;if(queued>=high){refill=false;return;}
        for(int block=0;block<3&&queued<high;++block){
        float pcm[chunk*2]{};const int wanted=std::min(chunk,high-queued);ma_uint64 frames=0;if(ma_engine_read_pcm_frames(&engine,pcm,wanted,&frames)!=MA_SUCCESS||frames!=wanted){error=3;return;}
        double energy=0;for(int i=0;i<wanted*2;++i){auto& v=pcm[i];v=std::clamp(v,-1.f,1.f);energy+=double(v)*v;}rms=std::sqrt(energy/(wanted*2));
        audit::audio_operation(audit::AudioOperation::StreamWrite);if(!SDL_PutAudioStreamData(stream,pcm,wanted*8)){error=4;return;}++pumps;mixed+=wanted;queued+=wanted;
        }if(queued>=high)refill=false;
    }
}audio;
template<class T>const T& value(u32 p){return *reinterpret_cast<const T*>(p);}
void report_ogg_resource_error(Buffer& buffer,const std::string& filename,const char* reason){
    if(!audio.ogg_resources_complete||buffer.diagnostic_sent)return;
    const std::string message=std::string("音乐资源错误：")+filename+"（"+reason+"）";
    th08_music_resource_notice(message.c_str());buffer.diagnostic_sent=true;
}
}
extern "C" void th08_sfx_latency_event(u32 index,u32 handle){
    if(!audio.sfx_probe)return;
    const auto found=audio.buffers.find(handle);if(found==audio.buffers.end())return;const auto& buffer=*found->second;
    const double clock_before=th08_sfx_latency_context_time();
    const u32 queued_frames=audio.stream?static_cast<u32>(std::max(0,SDL_GetAudioStreamQueued(audio.stream))/8):0;
    const double clock_after=th08_sfx_latency_context_time();
    th08_sfx_latency_event_trace(index,handle,buffer.web_sfx_voice_key,queued_frames,audio.sample_rate,buffer.format.rate,clock_before,clock_after);
}
namespace {
void observe_sfx_route(u32 id,const Buffer& buffer,int direct,int fallback,int fallback_started,int pending){
    if(!audio.sfx_probe||id>46)return;
    const double clock_before=th08_sfx_latency_context_time();
    const u32 queued_frames=audio.stream?static_cast<u32>(std::max(0,SDL_GetAudioStreamQueued(audio.stream))/8):0;
    const double clock_after=th08_sfx_latency_context_time();
    th08_sfx_latency_route_trace(id,buffer.web_sfx_voice_key,queued_frames,audio.sample_rate,direct,fallback,fallback_started,pending,
        clock_before,clock_after);
}
}
struct SDLSound final:SoundDevice {
 i32 create_pcm(u32 handle,const WaveFormat& format,const u8* data,u32 size)override{audit::audio_operation(audit::AudioOperation::CreatePcm);if(!audio.initialize())return -1;
if(!PcmWave::valid(format)||size%format.align)return -1;
        auto buf=std::make_unique<Buffer>();buf->format=format;buf->pcm.assign(data,data+size);
        auto config=ma_audio_buffer_config_init(format.bits==8?ma_format_u8:ma_format_s16,format.channels,size/format.align,buf->pcm.data(),nullptr);config.sampleRate=format.rate;
        if(ma_audio_buffer_init(&config,&buf->data)!=MA_SUCCESS)return -1;buf->data_ready=true;if(!audio.attach(*buf,&buf->data))return -1;
        buf->web_sfx_voice_key=web_sfx::key(&buf->sound);
        buf->web_sfx_uploaded=web_sfx::upload(&buf->sound,buf->pcm.data(),size/format.align,format.channels,format.rate,format.bits==8?2:1);
        if(audio.sfx_probe)th08_sfx_latency_upload(handle,buf->web_sfx_voice_key,format.rate,size/format.align,format.channels,buf->web_sfx_uploaded);
        audio.buffers[handle]=std::move(buf);return 0;
 }
 bool attach_ogg(Buffer& b){
        audit::audio_operation(audit::AudioOperation::AttachOgg);
        const auto filename="/bgm-ogg/"+b.name.substr(0,b.name.rfind('.'))+".ogg";
        if(!th08_ogg_file_exists(filename.c_str())){report_ogg_resource_error(b,filename,"文件不存在");return false;}
        auto config=ma_decoder_config_init(ma_format_s16,b.format.channels,b.format.rate);
        if(ma_decoder_init_file(filename.c_str(),&config,&b.decoder)!=MA_SUCCESS){report_ogg_resource_error(b,filename,"OGG 解码失败");return false;}b.decoder_ready=true;
        ma_uint64 length=0;if(ma_decoder_get_length_in_pcm_frames(&b.decoder,&length)!=MA_SUCCESS||length!=b.total_frames){report_ogg_resource_error(b,filename,"曲目长度校验失败");return false;}
        if(audio.ogg_full){
            const size_t frame_bytes=size_t(b.format.channels)*sizeof(ma_int16);
            b.full_pcm.resize(size_t(length)*frame_bytes);ma_uint64 decoded=0;
            while(decoded<length){ma_uint64 actual=0;const auto result=ma_decoder_read_pcm_frames(&b.decoder,b.full_pcm.data()+size_t(decoded)*frame_bytes,std::min<ma_uint64>(4096,length-decoded),&actual);if((result!=MA_SUCCESS&&result!=MA_AT_END)||!actual){b.full_pcm.clear();report_ogg_resource_error(b,filename,"PCM 读取失败");return false;}decoded+=actual;}
            ma_decoder_uninit(&b.decoder);b.decoder_ready=false;
            auto pcm_config=ma_audio_buffer_config_init(ma_format_s16,b.format.channels,length,b.full_pcm.data(),nullptr);pcm_config.sampleRate=b.format.rate;
            if(ma_audio_buffer_init(&pcm_config,&b.data)!=MA_SUCCESS){report_ogg_resource_error(b,filename,"PCM 创建失败");return false;}b.data_ready=true;
            if(ma_data_source_set_loop_point_in_pcm_frames(&b.data,b.intro_frames,length)!=MA_SUCCESS){report_ogg_resource_error(b,filename,"循环区间无效");return false;}
            if(!audio.attach(b,&b.data)){report_ogg_resource_error(b,filename,"音频源创建失败");return false;}
        }else{
            if(ma_data_source_set_loop_point_in_pcm_frames(&b.decoder,b.intro_frames,length)!=MA_SUCCESS){report_ogg_resource_error(b,filename,"循环区间无效");return false;}
            if(!audio.attach(b,&b.decoder)){report_ogg_resource_error(b,filename,"音频源创建失败");return false;}
        }
        audio.controls(b);b.pending=false;b.diagnostic_sent=false;return true;
 }
 i32 music_format(u32 handle,const BgmFormat& f,bool loop)override{
        audit::audio_operation(audit::AudioOperation::MusicFormat);
        if(!audio.music)return 0;if(!audio.initialize())return -1;
        if(!PcmWave::valid(f.format)||f.intro<0||f.total<=f.intro||u32(f.total)%f.format.align)return -1;
        const std::string name(f.name,strnlen(f.name,16));auto found=audio.buffers.find(handle);
        if(found!=audio.buffers.end()&&found->second->name==name){auto& b=*found->second;b.loop=loop;if(b.sound_ready)ma_sound_set_looping(&b.sound,loop);return 0;}
        if(name.find('/')!=std::string::npos||name.find('\\')!=std::string::npos||name.find("..")!=std::string::npos)return -1;
        auto buf=std::make_unique<Buffer>();buf->format=f.format;buf->name=name;buf->loop=loop;buf->music=true;buf->intro_frames=u32(f.intro)/f.format.align;buf->total_frames=u32(f.total)/f.format.align;
        if(!attach_ogg(*buf)){buf->clear_source();buf->pending=true;}else ma_sound_set_looping(&buf->sound,loop);
        audio.buffers[handle]=std::move(buf);return 0;
 }
    void resource_changed(){
        audit::audio_operation(audit::AudioOperation::ResourceChange);
        ++audio.resource_changes;if(!audio.music)return;
        for(auto& entry:audio.buffers){auto& b=*entry.second;if(!b.music||!b.pending)continue;const auto was_requested=b.requested;if(attach_ogg(b)){ma_sound_set_looping(&b.sound,b.loop);if(b.cursor)ma_sound_seek_to_pcm_frame(&b.sound,b.cursor/b.format.align);if(was_requested)ma_sound_start(&b.sound);}else b.clear_source();}
 }
 Buffer* get(u32 id){auto it=audio.buffers.find(id);return it==audio.buffers.end()?nullptr:it->second.get();}
 i32 stop(u32 id)override{audit::audio_operation(audit::AudioOperation::Stop);if(!audio.initialize())return -1;if(auto* b=get(id)){b->requested=false;web_sfx::stop(&b->sound);if(b->sound_ready)ma_sound_stop(&b->sound);}return 0;}
 i32 position(u32 id,u32 cursor)override{audit::audio_operation(audit::AudioOperation::Seek);if(!audio.initialize())return -1;if(auto* b=get(id)){b->cursor=cursor;web_sfx::seek(&b->sound,cursor/b->format.align,b->format.rate);if(!b->sound_ready)return 0;return ma_sound_seek_to_pcm_frame(&b->sound,cursor/b->format.align)==MA_SUCCESS?0:-1;}return 0;}
 i32 pan(u32 id,i32 value)override{audit::audio_operation(audit::AudioOperation::Pan);if(!audio.initialize())return -1;if(auto* b=get(id)){b->pan=value;if(b->sound_ready)audio.controls(*b);}return 0;}
 i32 volume(u32 id,i32 value)override{audit::audio_operation(audit::AudioOperation::Volume);if(!audio.initialize())return -1;if(auto* b=get(id)){b->volume=value;if(b->sound_ready)audio.controls(*b);}return 0;}
 i32 play(u32 id,u32,u32 flags)override{audit::audio_operation(audit::AudioOperation::Play);if(!audio.initialize())return -1;if(auto* b=get(id)){if(id==1000&&!audio.music)return 0;
     if(b->pending){b->requested=true;if(audio.sfx_probe&&id<=46)observe_sfx_route(id,*b,0,0,0,1);return 0;}
     if(id!=1000){b->loop=flags&1;ma_sound_set_looping(&b->sound,b->loop);const bool direct=web_sfx::play(&b->sound,b->loop);if(direct){if(audio.sfx_probe&&id<=46)observe_sfx_route(id,*b,1,0,0,0);return 0;}}
     const bool fallback_started=ma_sound_start(&b->sound)==MA_SUCCESS;if(audio.sfx_probe&&id<=46)observe_sfx_route(id,*b,0,id!=1000,fallback_started&&id!=1000,0);return fallback_started?0:-1;}return 0;}
 void release(u32 id)override{audit::audio_operation(audit::AudioOperation::Release);if(audio.initialize())audio.buffers.erase(id);}
 void midi_open()override{audit::audio_operation(audit::AudioOperation::MidiOpen);th08_midi_event(60,nullptr,0,0);}
 void midi_close(u32 time)override{audit::audio_operation(audit::AudioOperation::MidiClose);th08_midi_event(61,nullptr,0,time);}
 void midi_message(const u8* data,u32 size,u32 time)override{audit::audio_operation(audit::AudioOperation::MidiMessage);th08_midi_event(62,data,size,time);}
};
SoundDevice& sound_device(){static SDLSound sound;return sound;}
void sdl_audio_pump(){audio.pump();}
void sdl_audio_music_speed(float ratio){audit::audio_operation(audit::AudioOperation::MusicSpeed);audio.music_speed=std::isfinite(ratio)?std::clamp(ratio,1.f/60.f,100.f):1.f;for(auto& [id,buffer]:audio.buffers)if(buffer->music&&buffer->sound_ready)ma_sound_set_pitch(&buffer->sound,audio.music_speed);}
void sdl_audio_pause(bool pause){audit::audio_operation(audit::AudioOperation::Pause);if(audio.paused==pause)return;audio.paused=pause;web_sfx::suspend(pause);if(audio.stream){if(pause)SDL_PauseAudioStreamDevice(audio.stream);else SDL_ResumeAudioStreamDevice(audio.stream);}}
void sdl_audio_shutdown(){audit::audio_operation(audit::AudioOperation::Shutdown);if(audio.sfx_probe)th08_sfx_latency_reset_trace(1);audio.sfx_probe=false;audio.buffers.clear();if(audio.stream)SDL_DestroyAudioStream(audio.stream);audio.stream=nullptr;if(audio.ready)ma_engine_uninit(&audio.engine);audio.ready=false;audio.music_speed=1.f;}
extern "C" {
__attribute__((export_name("sdl_music_enabled"))) void sdl_music_enabled(u32 on){audit::audio_operation(audit::AudioOperation::MusicEnabled);audio.music=on!=0;if(!audio.music)if(auto* b=audio.buffers.count(1000)?audio.buffers[1000].get():nullptr){b->requested=false;if(b->sound_ready)ma_sound_stop(&b->sound);}}
__attribute__((export_name("sdl_ogg_decode_mode"))) void sdl_ogg_decode_mode(u32 full){audit::audio_operation(audit::AudioOperation::DecodeMode);audio.ogg_full=full!=0;}
#if defined(TH_PRESENTATION_AUDIT)
__attribute__((export_name("sdl_audit_audio"))) const u32* sdl_audit_audio(){return audit::audio_snapshot();}
__attribute__((export_name("sdl_audit_audio_words"))) u32 sdl_audit_audio_words(){return audit::audio_operation_count+2;}
#endif
__attribute__((export_name("sdl_music_resource_changed"))) void sdl_music_resource_changed(){static_cast<SDLSound&>(sound_device()).resource_changed();}
__attribute__((export_name("sdl_music_resources_complete"))) void sdl_music_resources_complete(u32 complete){audio.ogg_resources_complete=complete!=0;}
__attribute__((export_name("sdl_music_stats"))) const u32* sdl_music_stats(){static u32 out[6]{};out[0]=audio.music;out[1]=audio.ogg_full;out[2]=audio.buffers.count(1000)?audio.buffers[1000]->sound_ready:0;out[3]=audio.buffers.count(1000)?audio.buffers[1000]->pending:0;out[4]=audio.buffers.size();out[5]=audio.resource_changes;return out;}
// Read-only music state for acceptance probes. The original six-word decoder
// ABI remains stable; this separate record exposes the active source cursor
// without changing playback, scheduling or pause ownership.
__attribute__((export_name("sdl_music_observation"))) const u32* sdl_music_observation(){
    static u32 out[16]{};std::fill(out,out+16,0);out[0]=1;const auto found=audio.buffers.find(1000);if(found==audio.buffers.end())return out;
    const auto& b=*found->second;out[1]=1;out[2]=audio.paused;out[3]=b.sound_ready;out[4]=b.pending;out[5]=b.requested;out[6]=b.loop;
    ma_uint64 cursor=0;if(b.sound_ready){out[7]=ma_sound_is_playing(&b.sound);ma_sound_get_cursor_in_pcm_frames(&b.sound,&cursor);}
    out[8]=u32(cursor);out[9]=u32(cursor>>32);out[10]=b.total_frames;out[11]=b.intro_frames;out[12]=b.format.rate;
    out[13]=audio.stream?u32(std::max(0,SDL_GetAudioStreamQueued(audio.stream))/8):0;out[14]=audio.mixed;out[15]=audio.error;return out;
}
__attribute__((export_name("sdl_music_name"))) const char* sdl_music_name(){
    static char name[17]{};name[0]=0;const auto found=audio.buffers.find(1000);if(found!=audio.buffers.end())std::strncpy(name,found->second->name.c_str(),sizeof(name)-1);return name;
}
__attribute__((export_name("sdl_audio_stats"))) const u32* sdl_audio_stats(){static u32 out[12];out[0]=audio.ready;out[1]=audio.pumps;out[2]=audio.mixed;out[3]=audio.buffers.size();out[4]=audio.error;out[5]=audio.stream?std::max(0,SDL_GetAudioStreamQueued(audio.stream))/8:0;out[7]=audio.min_queue;std::memcpy(out+10,&audio.rms,4);return out;}
}
}
#endif
