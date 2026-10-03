// Native sound ownership, following the TH07 web runtime's miniaudio -> SDL3
// stream split. Original PCM, DirectSound gain units and notification offsets
// remain part of TH10's recovered audio logic; no PCM crosses a JS message port.
#define MA_NO_DEVICE_IO
#define MA_NO_RESOURCE_MANAGER
#define MA_NO_WAV
#define MA_NO_MP3
#define MA_NO_FLAC
#define MA_NO_ENCODING
#define MA_NO_THREADING
// File-backed Vorbis is the only canonical BGM source. Keeping stb_vorbis in
// this translation unit gives miniaudio both seekable streaming and decode-
// to-memory support without a JS-side audio path.
#include "../../../portable/sdl/third_party/stb_vorbis.h"
#define MINIAUDIO_IMPLEMENTATION
#include "../../../portable/sdl/third_party/miniaudio.h"
#include <SDL3/SDL.h>
#include "../platform/Audio.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "MusicLayout.hpp"

namespace {
using th10::u32;using th10::i32;using th10::u8;
template<class T=u32>T* ptr(u32 p){return reinterpret_cast<T*>(uintptr_t(p));}
u32 address(const void* p){return reinterpret_cast<uintptr_t>(p);}
u32 read32(const u8* p){u32 v;std::memcpy(&v,p,4);return v;}
unsigned read16(const u8* p){return unsigned(p[0])|(unsigned(p[1])<<8);}
struct Buffer {
    u32 refs=1,flags=0,length=0,cursor=0,last_cursor=0,frequency=44100;
    i32 volume=0,pan=0;bool device=false,playing=false,loop=false,sound_ready=false,data_ready=false;
    uint64_t started=0;std::array<u8,18> format{{1,0,2,0,68,172,0,0,16,177,2,0,4,0,16,0,0,0}};
    std::shared_ptr<std::vector<u8>> pcm;
    std::vector<std::pair<u32,u32>> notifications;
    ma_audio_buffer data{};ma_sound sound{};
    ~Buffer(){if(sound_ready)ma_sound_uninit(&sound);if(data_ready)ma_audio_buffer_uninit(&data);}
};
struct Host {
    std::map<u32,std::unique_ptr<Buffer>> objects;std::map<u32,bool> events;
    std::vector<th10::browser::Audio*> owners;
    u32 next=1,next_event=1;uint64_t millis=1000;ma_engine engine{};
    SDL_AudioStream* stream=nullptr;bool ready=false,paused=false,refilling=true;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // Browser suspension and Replay output suppression are independent
    // owners. Releasing a seek must never release a visibility/manual pause.
    bool caller_paused=false,replay_seeking=false;
#endif
    // TH07's web queue policy: bounded 1024-frame work slices, hysteresis.
    static constexpr int chunk=1024,low=4096,high=6144;
    u32 pumps=0,empty_checks=0,mixed_frames=0,min_queued=~0u,error=0;
    float rms=0;bool primed=false;
    bool initialize(){
        if(ready)return true;
        auto cfg=ma_engine_config_init();cfg.noDevice=MA_TRUE;cfg.sampleRate=44100;cfg.channels=2;
        // DirectSound changes are immediate; do not add miniaudio's default
        // short gain ramp to the game's own fades.
        cfg.defaultVolumeSmoothTimeInPCMFrames=0;
        if(ma_engine_init(&cfg,&engine)!=MA_SUCCESS){error=1;return false;}
        SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES,"2048");
        SDL_AudioSpec spec{SDL_AUDIO_F32,2,44100};
        if(SDL_InitSubSystem(SDL_INIT_AUDIO))stream=SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,&spec,nullptr,nullptr);
        if(!stream){ma_engine_uninit(&engine);error=2;return false;}
        ready=true;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        paused=caller_paused||replay_seeking;
        if(paused)SDL_PauseAudioStreamDevice(stream);else SDL_ResumeAudioStreamDevice(stream);
#else
        paused=false;SDL_ResumeAudioStreamDevice(stream);
#endif
        return true;
    }
    Buffer* get(u32 id){auto it=objects.find(id);return it==objects.end()?nullptr:it->second.get();}
    void destroy(){objects.clear();events.clear();if(stream)SDL_DestroyAudioStream(stream);stream=nullptr;if(ready)ma_engine_uninit(&engine);ready=false;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        caller_paused=replay_seeking=paused=false;
#endif
    }
    u32 position(Buffer& b){
        if(!b.playing)return b.cursor;
        const auto align=std::max(1u,read16(b.format.data()+12));
        // Stream storage can be rewritten only after the mixer has consumed
        // it. Wall time and browser playback can diverge during a stall or
        // suspension; using that clock here overwrites unread music samples.
        if(b.sound_ready&&!b.notifications.empty()){
            ma_uint64 frame=0;ma_sound_get_cursor_in_pcm_frames(&b.sound,&frame);
            return u32(frame*align%b.length);
        }
        const uint64_t pos=b.cursor+((millis-b.started)*b.frequency/1000)*align;
        if(b.loop)return u32(pos%b.length);
        if(pos>=b.length){b.playing=false;return b.cursor=0;}return u32(pos);
    }
    void controls(Buffer& b){if(!b.sound_ready)return;
        ma_sound_set_volume(&b.sound,std::pow(10.f,float(b.volume)/2000.f));
        ma_sound_set_pan_mode(&b.sound,ma_pan_mode_balance);
        ma_sound_set_pan(&b.sound,b.pan>=0?1.f-std::pow(10.f,-float(b.pan)/2000.f):std::pow(10.f,float(b.pan)/2000.f)-1.f);
        ma_sound_set_pitch(&b.sound,float(b.frequency)/std::max(1u,read32(b.format.data()+4)));
    }
    bool sound(Buffer& b){
        if(b.sound_ready||b.flags&1)return true;
        const auto channels=read16(b.format.data()+2),align=read16(b.format.data()+12),bits=read16(b.format.data()+14);
        if(read16(b.format.data())!=1||!align||!channels||(bits!=8&&bits!=16))return false;
        auto cfg=ma_audio_buffer_config_init(bits==8?ma_format_u8:ma_format_s16,channels,b.length/align,b.pcm->data(),nullptr);
        cfg.sampleRate=read32(b.format.data()+4);
        if(ma_audio_buffer_init(&cfg,&b.data)!=MA_SUCCESS)return false;b.data_ready=true;
        if(ma_sound_init_from_data_source(&engine,&b.data,MA_SOUND_FLAG_NO_SPATIALIZATION,nullptr,&b.sound)!=MA_SUCCESS)return false;
        b.sound_ready=true;controls(b);return true;
    }
    u32 buffer(u32 flags,u32 length,const u8* format,std::shared_ptr<std::vector<u8>> shared={}){
        auto b=std::make_unique<Buffer>();b->flags=flags;b->length=length?length:4096;
        if(format)std::memcpy(b->format.data(),format,18);b->frequency=read32(b->format.data()+4);
        b->pcm=shared?shared:std::make_shared<std::vector<u8>>(b->length,read16(b->format.data()+14)==8?128:0);
        const auto id=next++;objects[id]=std::move(b);return id;
    }
    bool notify(Buffer& b){
        const auto pos=position(b),last=b.last_cursor;bool signaled=false;
        for(const auto& note:b.notifications)if((pos>=last&&note.first>=last&&note.first<pos)||(pos<last&&(note.first>=last||note.first<pos))){
            auto it=events.find(note.second);if(it!=events.end()){it->second=true;signaled=true;}
        }
        b.last_cursor=pos;return signaled;
    }
    ma_uint64 render(float* out,ma_uint64 count){
        ma_uint64 done=0;
        while(done<count){
            ma_uint64 frames=0;const auto wanted=std::min<ma_uint64>(chunk,count-done);
            if(ma_engine_read_pcm_frames(&engine,out+done*2,wanted,&frames)!=MA_SUCCESS){error=3;break;}
            done+=frames;bool signaled=false;
            for(auto& entry:objects){auto& b=*entry.second;if(b.playing&&!b.notifications.empty())signaled=notify(b)||signaled;}
            // Service the original refill worker between mixer slices. A
            // single delayed display callback must not collapse several
            // quarter-second ring notifications into one auto-reset event.
            if(signaled)for(auto* owner:owners)owner->pump();
            if(frames<wanted)break;
        }
        return done;
    }
    void pump(){
        if(!ready||paused)return;
        int queued=std::max(0,SDL_GetAudioStreamQueued(stream))/8;
        min_queued=std::min(min_queued,u32(queued));
        // An empty application queue is not proof of an audible underrun:
        // SDL/browser may already be playing the last transferred block.
        if(primed&&queued==0)++empty_checks;
        if(!refilling&&queued<low)refilling=true;if(!refilling)return;
        if(queued>=high){refilling=false;primed=true;return;}
        for(int block=0;block<(high+chunk-1)/chunk&&queued<high;++block){
        float pcm[chunk*2]{};
        if(render(pcm,chunk)!=chunk){error=3;return;}
        double energy=0;for(auto& sample:pcm){sample=std::clamp(sample,-1.f,1.f);energy+=double(sample)*sample;}
        rms=std::sqrt(energy/(chunk*2));
        if(!SDL_PutAudioStreamData(stream,pcm,sizeof(pcm))){error=4;return;}
        ++pumps;mixed_frames+=chunk;queued+=chunk;
        }
        if(queued>=high){refilling=false;primed=true;}
    }
}host;
}
extern "C" {
u32 audio_host_create(){if(!host.initialize())return 0;auto b=std::make_unique<Buffer>();b->device=true;const auto id=host.next++;host.objects[id]=std::move(b);return id;}
i32 audio_host_call(u32 id,u32 op,const u32* a){
    auto* p=host.get(id);if(!p)return i32(0x80070057);auto& b=*p;
    using O=th10::browser::AudioOperation;
    switch(static_cast<O>(op)){
    case O::AddRef:return ++b.refs;
    case O::Release:if(--b.refs)return b.refs;host.objects.erase(id);return 0;
    case O::CreateBuffer:{auto* desc=ptr(a[0]);*ptr(a[1])=host.buffer(desc[1],desc[2],ptr<u8>(desc[4]));break;}
    case O::Duplicate:{auto* src=host.get(a[0]);if(!src)return -1;const auto copy=host.buffer(src->flags,src->length,src->format.data(),src->pcm);auto& dest=*host.get(copy);dest.frequency=src->frequency;dest.volume=src->volume;dest.pan=src->pan;*ptr(a[1])=copy;break;}
    case O::Cooperative:case O::Restore:case O::Unlock:break;
    case O::Format:if(b.sound_ready){ma_sound_uninit(&b.sound);b.sound_ready=false;}if(b.data_ready){ma_audio_buffer_uninit(&b.data);b.data_ready=false;}std::memcpy(b.format.data(),ptr(a[0]),18);b.frequency=read32(b.format.data()+4);break;
    case O::Status:host.position(b);*ptr(a[0])=b.playing?(b.loop?5:1):0;break;
    case O::Volume:b.volume=i32(a[0]);host.controls(b);break;
    case O::Pan:b.pan=i32(a[0]);host.controls(b);break;
    case O::Frequency:b.cursor=host.position(b);b.started=host.millis;b.frequency=a[0]?a[0]:read32(b.format.data()+4);host.controls(b);break;
    case O::GetVolume:*ptr(a[0])=u32(b.volume);break;
    case O::GetPan:*ptr(a[0])=u32(b.pan);break;
    case O::GetFrequency:*ptr(a[0])=b.frequency;break;
    case O::Play:
        if(!host.sound(b)){host.error=5;return -1;}
        if(b.playing)b.cursor=host.position(b);b.playing=true;b.loop=!!(a[2]&1);b.started=host.millis;b.last_cursor=b.cursor;
        if(b.sound_ready){ma_sound_set_looping(&b.sound,b.loop);ma_sound_seek_to_pcm_frame(&b.sound,b.cursor/std::max(1u,read16(b.format.data()+12)));ma_sound_start(&b.sound);}break;
    case O::Stop:b.cursor=host.position(b);b.playing=false;if(b.sound_ready)ma_sound_stop(&b.sound);break;
    case O::Position:b.cursor=a[0]%b.length;b.started=host.millis;if(b.sound_ready)ma_sound_seek_to_pcm_frame(&b.sound,b.cursor/std::max(1u,read16(b.format.data()+12)));break;
    case O::CurrentPosition:{const auto pos=host.position(b);if(a[0])*ptr(a[0])=pos;if(a[1])*ptr(a[1])=(pos+2048)%b.length;break;}
    case O::Lock:{const auto offset=a[0]%b.length,len=a[6]&2?b.length:a[1],first=std::min(len,b.length-offset);if(len>b.length)return -1;
        *ptr(a[2])=address(b.pcm->data()+offset);*ptr(a[3])=first;if(a[4])*ptr(a[4])=len>first?address(b.pcm->data()):0;if(a[5])*ptr(a[5])=len-first;break;}
    // One native buffer owns both interfaces. This preserves COM reference
    // ownership without building a second browser-side object graph.
    case O::Query:++b.refs;*ptr(a[1])=id;break;
    case O::Notifications:{auto* notes=ptr(a[1]);b.notifications.clear();for(u32 i=0;i<a[0];i++)b.notifications.emplace_back(notes[i*2],notes[i*2+1]);break;}
    case O::GetFormat:if(a[2])*ptr(a[2])=18;if(a[0])std::memcpy(ptr(a[0]),b.format.data(),std::min(a[1],18u));break;
    case O::Caps:{u32 caps[]{20,b.flags,b.length,0,0};std::memcpy(ptr(a[0]),caps,sizeof(caps));break;}
    default:return -1;
    }return 0;
}
u32 audio_host_event(u32 op,u32 id){
    if(op==0){const auto next=host.next_event++;host.events[next]=false;return next;}
    if(op==2){host.events.erase(id);return 0;}auto it=host.events.find(id);if(it==host.events.end())return ~0u;
    const bool signaled=it->second;it->second=false;return signaled?0:0x102;
}
void audio_host_advance(u32 ms){host.millis+=ms;}
void audio_host_attach(th10::browser::Audio* owner,bool attach){
    if(attach)host.owners.push_back(owner);
    else host.owners.erase(std::remove(host.owners.begin(),host.owners.end(),owner),host.owners.end());
}
#define EXPORT(name) __attribute__((export_name(name)))
EXPORT("sdl_audio_pump") void sdl_audio_pump(){host.pump();}
EXPORT("sdl_audio_pause") void sdl_audio_pause(u32 paused){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    host.caller_paused=paused!=0;paused=host.caller_paused||host.replay_seeking;
#endif
    if(host.paused==(paused!=0))return;host.paused=paused!=0;if(!host.stream)return;if(paused)SDL_PauseAudioStreamDevice(host.stream);else SDL_ResumeAudioStreamDevice(host.stream);
}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
// Replay seeking advances the existing mixer/refill owners without sending
// the skipped interval to the device. This is not a new BGM player or a live
// frame-pacing policy. Ordinary builds do not contain these helpers.
void sdl_audio_replay_seek_output(u32 seeking){
    if(host.replay_seeking==(seeking!=0))return;
    host.replay_seeking=seeking!=0;
    sdl_audio_pause(host.caller_paused);
    if(host.stream)SDL_ClearAudioStream(host.stream);
    host.primed=false;host.refilling=true;
}
u32 sdl_audio_replay_seek_tick(){
    if(!host.ready)return 1;
    // 44100 Hz / 60 native ticks. render() services the original stream
    // notifications and maintains the selected track, loop and position.
    float discarded[735*2]{};
    return host.render(discarded,735)==735?1:0;
}
#endif
EXPORT("sdl_audio_shutdown") void sdl_audio_shutdown(){host.destroy();}
EXPORT("sdl_audio_stats") u32 sdl_audio_stats(){static u32 values[12];values[0]=host.ready;values[1]=host.objects.size();values[2]=host.events.size();values[3]=host.pumps;values[4]=host.mixed_frames;values[5]=host.stream?std::max(0,SDL_GetAudioStreamQueued(host.stream))/8:0;values[6]=host.empty_checks;values[7]=host.min_queued;values[8]=host.error;values[9]=host.paused;std::memcpy(values+10,&host.rms,4);values[11]=u32(host.millis);return address(values);}
// Deterministic PCM/ownership probes exercise the same mixer used by SDL.
EXPORT("sdl_audio_render") u32 sdl_audio_render(float* out,u32 frames){return host.ready?u32(host.render(out,frames)):0;}
EXPORT("sdl_audio_event") u32 sdl_audio_event(u32 op,u32 id){return audio_host_event(op,id);}
EXPORT("sdl_audio_device") u32 sdl_audio_device(){return audio_host_create();}
}

// SDL file data source for the game's original PCM archive address space.
// Each retail archive region is mapped to one canonical OGG. The game still
// owns archive offsets, loop/fade state and the refill worker.
namespace {
bool music_enabled=true,ogg_full=false;
constexpr const char* music_names[] = {
    "02","00","01","03","04","05","06","07","08","09",
    "10","11","12","15","16","13","14","17"
};
struct MusicFile;
std::vector<MusicFile*> music_streams;
struct MusicFile {
    uint64_t cursor=0,decoder_frame=0;int track=-1;SDL_IOStream* source=nullptr;ma_decoder decoder{};std::vector<u8> full_pcm;bool valid=false,full_ready=false,waiting=false;
    ~MusicFile(){music_streams.erase(std::remove(music_streams.begin(),music_streams.end(),this),music_streams.end());reset();}
    void reset(){if(valid)ma_decoder_uninit(&decoder);if(source)SDL_CloseIO(source);source=nullptr;valid=false;full_ready=false;waiting=false;track=-1;full_pcm.clear();}
    static ma_result read(ma_decoder* d,void* out,size_t size,size_t* read){*read=SDL_ReadIO(static_cast<MusicFile*>(d->pUserData)->source,out,size);return *read?MA_SUCCESS:MA_AT_END;}
    static ma_result seek(ma_decoder* d,ma_int64 offset,ma_seek_origin origin){return SDL_SeekIO(static_cast<MusicFile*>(d->pUserData)->source,offset,origin==ma_seek_origin_start?SDL_IO_SEEK_SET:origin==ma_seek_origin_current?SDL_IO_SEEK_CUR:SDL_IO_SEEK_END)>=0?MA_SUCCESS:MA_BAD_SEEK;}
    bool select(int n,const MusicEntry& entry){
        if(n<0||n>=int(sizeof(music_names)/sizeof(music_names[0])))return false;
        if(track==n&&(valid||full_ready))return true;
        reset();track=n;char name[64];std::snprintf(name,sizeof(name),"/bgm-ogg/th10_%s.ogg",music_names[n]);source=SDL_IOFromFile(name,"rb");if(!source){waiting=true;return false;}
        auto cfg=ma_decoder_config_init(ma_format_s16,2,44100);if(ma_decoder_init(read,seek,this,&cfg,&decoder)!=MA_SUCCESS){reset();return false;}valid=true;decoder_frame=0;
        if(!ogg_full)return true;
        const auto frames=entry.length/4;full_pcm.resize(size_t(entry.length));uint64_t decoded=0;
        while(decoded<frames){ma_uint64 actual=0;const auto result=ma_decoder_read_pcm_frames(&decoder,full_pcm.data()+size_t(decoded)*4,std::min<uint64_t>(4096,frames-decoded),&actual);if((result!=MA_SUCCESS&&result!=MA_AT_END)||!actual){reset();return false;}decoded+=actual;}
        ma_decoder_uninit(&decoder);valid=false;SDL_CloseIO(source);source=nullptr;full_ready=true;waiting=false;return true;
    }
    void resource_changed(){if(waiting){const auto n=track;reset();track=n;}}
};
constexpr uint64_t music_size=uint64_t(music_entries[17].offset)+music_entries[17].length;
Sint64 musicSize(void*){return music_size;}
Sint64 musicSeek(void* user,Sint64 off,SDL_IOWhence origin){auto& f=*static_cast<MusicFile*>(user);const auto pos=(origin==SDL_IO_SEEK_SET?0:origin==SDL_IO_SEEK_CUR?Sint64(f.cursor):Sint64(music_size))+off;if(pos<0)return -1;f.cursor=pos;return pos;}
size_t musicRead(void* user,void* out,size_t size,SDL_IOStatus* status){auto& f=*static_cast<MusicFile*>(user);auto* dest=static_cast<u8*>(out);size_t done=0;*status=SDL_IO_STATUS_READY;
    if(!music_enabled){const auto count=std::min<uint64_t>(size,f.cursor<music_size?music_size-f.cursor:0);std::memset(out,0,count);f.cursor+=count;if(f.cursor>=music_size)*status=SDL_IO_STATUS_EOF;return count;}
    while(done<size&&f.cursor<music_size){
        if(f.cursor<16){const auto n=std::min<uint64_t>(size-done,16-f.cursor);std::memset(dest+done,0,n);done+=n;f.cursor+=n;continue;}
        int n=17;while(n>0&&f.cursor<music_entries[n].offset)--n;const auto& t=music_entries[n];
        if(!f.select(n,t)){
            // A managed OGG may arrive after the Runtime starts. Do not turn
            // a missing optional component into an IO error; the resource
            // change export invalidates this pending selection for retry.
            const auto count=std::min<uint64_t>(size-done,t.length-(f.cursor-t.offset));std::memset(dest+done,0,count);done+=count;f.cursor+=count;continue;
        }
        const auto local=f.cursor-t.offset,frame=local/4;const size_t count=std::min<uint64_t>(size-done,t.length-local),leading=local%4;
        if(f.full_ready){std::memcpy(dest+done,f.full_pcm.data()+local,count);f.cursor+=count;done+=count;continue;}
        if(frame!=f.decoder_frame&&ma_decoder_seek_to_pcm_frame(&f.decoder,frame)!=MA_SUCCESS){*status=SDL_IO_STATUS_ERROR;break;}
        ma_uint64 actual=0;const auto frames=(count+leading+3)/4;
        std::vector<u8> scratch;void* target=dest+done;if(leading||count%4){scratch.resize(frames*4);target=scratch.data();}
        const auto result=ma_decoder_read_pcm_frames(&f.decoder,target,frames,&actual);f.decoder_frame=frame+actual;
        const auto bytes=std::min<uint64_t>(count,actual*4>leading?actual*4-leading:0);if((result!=MA_SUCCESS&&result!=MA_AT_END)||!bytes){*status=SDL_IO_STATUS_ERROR;break;}
        if(!scratch.empty())std::memcpy(dest+done,scratch.data()+leading,bytes);f.cursor+=bytes;done+=bytes;
    }
    if(f.cursor>=music_size)*status=SDL_IO_STATUS_EOF;return done;
}
bool musicClose(void* user){delete static_cast<MusicFile*>(user);return true;}
}
extern "C" SDL_IOStream* th10_music_stream(){SDL_IOStreamInterface iface{};SDL_INIT_INTERFACE(&iface);iface.size=musicSize;iface.seek=musicSeek;iface.read=musicRead;iface.close=musicClose;auto* f=new MusicFile();music_streams.push_back(f);auto* io=SDL_OpenIO(&iface,f);if(!io){music_streams.pop_back();delete f;}return io;}
extern "C" __attribute__((export_name("sdl_music_enabled"))) void sdl_music_enabled(u32 enabled){music_enabled=enabled!=0;}
extern "C" __attribute__((export_name("sdl_ogg_decode_mode"))) void sdl_ogg_decode_mode(u32 full){ogg_full=full!=0;}
extern "C" __attribute__((export_name("sdl_music_resource_changed"))) void sdl_music_resource_changed(){for(auto* stream:music_streams)stream->resource_changed();}
extern "C" __attribute__((export_name("sdl_music_stats"))) const u32* sdl_music_stats(){static u32 out[6]{};out[0]=music_enabled;out[1]=ogg_full;out[2]=music_streams.size();out[3]=out[4]=out[5]=0;for(auto* stream:music_streams){out[3]+=stream->waiting;out[4]+=stream->valid;out[5]+=stream->full_ready;}return out;}
