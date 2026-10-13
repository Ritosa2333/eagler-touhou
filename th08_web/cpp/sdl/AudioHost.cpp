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
#include "../platform/PlatformDevices.hpp"
#include <SDL3/SDL.h>
#include <emscripten.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
EM_JS(void, th08_midi_event, (int op,const unsigned char* data,unsigned length,unsigned time), {
    if(Module['runtimeMidi'])Module['runtimeMidi'](op,data?HEAPU8.slice(data,data+length):null,time);
});
namespace th08 {
namespace {
struct Buffer {
    WaveFormat format{};std::vector<u8> pcm;std::vector<u8> full_pcm;ma_audio_buffer data{};ma_decoder decoder{};ma_sound sound{};
    bool data_ready=false,decoder_ready=false,sound_ready=false;bool loop=false;bool music=false,pending=false,requested=false;u32 cursor=0,intro_frames=0,total_frames=0;std::string name;i32 volume=0,pan=0;
    void clear_source(){if(sound_ready){ma_sound_uninit(&sound);sound_ready=false;}if(data_ready){ma_audio_buffer_uninit(&data);data_ready=false;}if(decoder_ready){ma_decoder_uninit(&decoder);decoder_ready=false;}full_pcm.clear();}
    ~Buffer(){clear_source();}
};
struct Audio {
    ma_engine engine{};SDL_AudioStream* stream=nullptr;bool ready=false,paused=false,music=true,ogg_full=false,refill=true;u32 error=0,pumps=0,mixed=0,min_queue=~0u,resource_changes=0;float rms=0;
    float music_speed=1.f;std::map<u32,std::unique_ptr<Buffer>> buffers;
    bool initialize(){if(ready)return true;auto config=ma_engine_config_init();config.noDevice=MA_TRUE;config.channels=2;config.sampleRate=44100;config.defaultVolumeSmoothTimeInPCMFrames=0;
        if(ma_engine_init(&config,&engine)!=MA_SUCCESS){error=1;return false;}
        SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES,"2048");SDL_AudioSpec spec{SDL_AUDIO_F32,2,44100};
        if(SDL_InitSubSystem(SDL_INIT_AUDIO))stream=SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,&spec,nullptr,nullptr);
        if(!stream){ma_engine_uninit(&engine);error=2;return false;}ready=true;SDL_ResumeAudioStreamDevice(stream);return true;}
    bool attach(Buffer& b,ma_data_source* source){if(ma_sound_init_from_data_source(&engine,source,MA_SOUND_FLAG_NO_SPATIALIZATION,nullptr,&b.sound)!=MA_SUCCESS)return false;b.sound_ready=true;return true;}
    void controls(Buffer& b){if(b.music)ma_sound_set_pitch(&b.sound,music_speed);ma_sound_set_volume(&b.sound,std::pow(10.f,float(b.volume)/2000.f));ma_sound_set_pan_mode(&b.sound,ma_pan_mode_balance);
        ma_sound_set_pan(&b.sound,b.pan>=0?1.f-std::pow(10.f,-float(b.pan)/2000.f):std::pow(10.f,float(b.pan)/2000.f)-1.f);}
    void pump(){if(!ready||paused)return;auto queued=std::max(0,SDL_GetAudioStreamQueued(stream))/8;min_queue=std::min(min_queue,u32(queued));
        if(!refill&&queued<4096)refill=true;if(!refill)return;if(queued>=6144){refill=false;return;}
        for(int block=0;block<6&&queued<6144;++block){
        float pcm[2048]{};ma_uint64 frames=0;if(ma_engine_read_pcm_frames(&engine,pcm,1024,&frames)!=MA_SUCCESS){error=3;return;}
        double energy=0;for(auto& v:pcm){v=std::clamp(v,-1.f,1.f);energy+=double(v)*v;}rms=std::sqrt(energy/2048);
        if(!SDL_PutAudioStreamData(stream,pcm,sizeof(pcm))){error=4;return;}++pumps;mixed+=1024;queued+=1024;
        }if(queued>=6144)refill=false;
    }
}audio;
template<class T>const T& value(u32 p){return *reinterpret_cast<const T*>(p);}
}
struct SDLSound final:SoundDevice {
 i32 create_pcm(u32 handle,const WaveFormat& format,const u8* data,u32 size)override{if(!audio.initialize())return -1;
if(!PcmWave::valid(format)||size%format.align)return -1;
        auto buf=std::make_unique<Buffer>();buf->format=format;buf->pcm.assign(data,data+size);
        auto config=ma_audio_buffer_config_init(format.bits==8?ma_format_u8:ma_format_s16,format.channels,size/format.align,buf->pcm.data(),nullptr);config.sampleRate=format.rate;
        if(ma_audio_buffer_init(&config,&buf->data)!=MA_SUCCESS)return -1;buf->data_ready=true;if(!audio.attach(*buf,&buf->data))return -1;audio.buffers[handle]=std::move(buf);return 0;
 }
 bool attach_ogg(Buffer& b){
        const auto filename="/bgm-ogg/"+b.name.substr(0,b.name.rfind('.'))+".ogg";
        auto config=ma_decoder_config_init(ma_format_s16,b.format.channels,b.format.rate);
        if(ma_decoder_init_file(filename.c_str(),&config,&b.decoder)!=MA_SUCCESS)return false;b.decoder_ready=true;
        ma_uint64 length=0;if(ma_decoder_get_length_in_pcm_frames(&b.decoder,&length)!=MA_SUCCESS||length!=b.total_frames)return false;
        if(audio.ogg_full){
            const size_t frame_bytes=size_t(b.format.channels)*sizeof(ma_int16);
            b.full_pcm.resize(size_t(length)*frame_bytes);ma_uint64 decoded=0;
            while(decoded<length){ma_uint64 actual=0;const auto result=ma_decoder_read_pcm_frames(&b.decoder,b.full_pcm.data()+size_t(decoded)*frame_bytes,std::min<ma_uint64>(4096,length-decoded),&actual);if((result!=MA_SUCCESS&&result!=MA_AT_END)||!actual){b.full_pcm.clear();return false;}decoded+=actual;}
            ma_decoder_uninit(&b.decoder);b.decoder_ready=false;
            auto pcm_config=ma_audio_buffer_config_init(ma_format_s16,b.format.channels,length,b.full_pcm.data(),nullptr);pcm_config.sampleRate=b.format.rate;
            if(ma_audio_buffer_init(&pcm_config,&b.data)!=MA_SUCCESS)return false;b.data_ready=true;
            if(ma_data_source_set_loop_point_in_pcm_frames(&b.data,b.intro_frames,length)!=MA_SUCCESS)return false;
            if(!audio.attach(b,&b.data))return false;
        }else{
            if(ma_data_source_set_loop_point_in_pcm_frames(&b.decoder,b.intro_frames,length)!=MA_SUCCESS)return false;
            if(!audio.attach(b,&b.decoder))return false;
        }
        audio.controls(b);b.pending=false;return true;
 }
 i32 music_format(u32 handle,const BgmFormat& f,bool loop)override{
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
        ++audio.resource_changes;if(!audio.music)return;
        for(auto& entry:audio.buffers){auto& b=*entry.second;if(!b.music||!b.pending)continue;const auto was_requested=b.requested;if(attach_ogg(b)){ma_sound_set_looping(&b.sound,b.loop);if(b.cursor)ma_sound_seek_to_pcm_frame(&b.sound,b.cursor/b.format.align);if(was_requested)ma_sound_start(&b.sound);}else b.clear_source();}
 }
 Buffer* get(u32 id){auto it=audio.buffers.find(id);return it==audio.buffers.end()?nullptr:it->second.get();}
 i32 stop(u32 id)override{if(!audio.initialize())return -1;if(auto* b=get(id)){b->requested=false;if(b->sound_ready)ma_sound_stop(&b->sound);}return 0;}
 i32 position(u32 id,u32 cursor)override{if(!audio.initialize())return -1;if(auto* b=get(id)){b->cursor=cursor;if(!b->sound_ready)return 0;return ma_sound_seek_to_pcm_frame(&b->sound,cursor/b->format.align)==MA_SUCCESS?0:-1;}return 0;}
 i32 pan(u32 id,i32 value)override{if(!audio.initialize())return -1;if(auto* b=get(id)){b->pan=value;if(b->sound_ready)audio.controls(*b);}return 0;}
 i32 volume(u32 id,i32 value)override{if(!audio.initialize())return -1;if(auto* b=get(id)){b->volume=value;if(b->sound_ready)audio.controls(*b);}return 0;}
 i32 play(u32 id,u32,u32 flags)override{if(!audio.initialize())return -1;if(auto* b=get(id)){if(id==1000&&!audio.music)return 0;if(b->pending){b->requested=true;return 0;}if(id!=1000){b->loop=flags&1;ma_sound_set_looping(&b->sound,b->loop);}return ma_sound_start(&b->sound)==MA_SUCCESS?0:-1;}return 0;}
 void release(u32 id)override{if(audio.initialize())audio.buffers.erase(id);}
 void midi_open()override{th08_midi_event(60,nullptr,0,0);}
 void midi_close(u32 time)override{th08_midi_event(61,nullptr,0,time);}
 void midi_message(const u8* data,u32 size,u32 time)override{th08_midi_event(62,data,size,time);}
};
SoundDevice& sound_device(){static SDLSound sound;return sound;}
void sdl_audio_pump(){audio.pump();}
void sdl_audio_music_speed(float ratio){audio.music_speed=std::isfinite(ratio)?std::clamp(ratio,1.f/60.f,100.f):1.f;for(auto& [id,buffer]:audio.buffers)if(buffer->music&&buffer->sound_ready)ma_sound_set_pitch(&buffer->sound,audio.music_speed);}
void sdl_audio_pause(bool pause){if(audio.paused==pause)return;audio.paused=pause;if(audio.stream){if(pause)SDL_PauseAudioStreamDevice(audio.stream);else SDL_ResumeAudioStreamDevice(audio.stream);}}
void sdl_audio_shutdown(){audio.buffers.clear();if(audio.stream)SDL_DestroyAudioStream(audio.stream);audio.stream=nullptr;if(audio.ready)ma_engine_uninit(&audio.engine);audio.ready=false;audio.music_speed=1.f;}
extern "C" {
__attribute__((export_name("sdl_music_enabled"))) void sdl_music_enabled(u32 on){audio.music=on!=0;if(!audio.music)if(auto* b=audio.buffers.count(1000)?audio.buffers[1000].get():nullptr){b->requested=false;if(b->sound_ready)ma_sound_stop(&b->sound);}}
__attribute__((export_name("sdl_ogg_decode_mode"))) void sdl_ogg_decode_mode(u32 full){audio.ogg_full=full!=0;}
__attribute__((export_name("sdl_music_resource_changed"))) void sdl_music_resource_changed(){static_cast<SDLSound&>(sound_device()).resource_changed();}
__attribute__((export_name("sdl_music_stats"))) const u32* sdl_music_stats(){static u32 out[6]{};out[0]=audio.music;out[1]=audio.ogg_full;out[2]=audio.buffers.count(1000)?audio.buffers[1000]->sound_ready:0;out[3]=audio.buffers.count(1000)?audio.buffers[1000]->pending:0;out[4]=audio.buffers.size();out[5]=audio.resource_changes;return out;}
__attribute__((export_name("sdl_audio_stats"))) const u32* sdl_audio_stats(){static u32 out[12];out[0]=audio.ready;out[1]=audio.pumps;out[2]=audio.mixed;out[3]=audio.buffers.size();out[4]=audio.error;out[5]=audio.stream?std::max(0,SDL_GetAudioStreamQueued(audio.stream))/8:0;out[7]=audio.min_queue;std::memcpy(out+10,&audio.rms,4);return out;}
}
}
#endif
