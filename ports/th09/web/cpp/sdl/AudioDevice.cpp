#define MA_NO_DEVICE_IO
#define MA_NO_RESOURCE_MANAGER
#define MA_NO_MP3
#define MA_NO_FLAC
#define MA_NO_ENCODING
#define MA_NO_THREADING
#ifdef _WIN32
// The mixer consumes float samples. Omit stb_vorbis integer helpers and their Windows macro collisions.
#define STB_VORBIS_NO_INTEGER_CONVERSION
#endif
#include "../../../portable/sdl/third_party/stb_vorbis.h"
#define MINIAUDIO_IMPLEMENTATION
#include "../../../portable/sdl/third_party/miniaudio.h"
#include "AudioDevice.hpp"
#include "Assets.hpp"
#include "RuntimePath.hpp"
#include "PerformanceDiagnostics.hpp"
#include "../game/MusicCatalog.hpp"
#include <cmath>
#include <algorithm>
#include <utility>
#include <eagler/audio/WebSfx.hpp>
namespace web_sfx = eagler::audio::web_sfx;
namespace th09::sdl {
namespace {
u32 word(const u8* p){u32 v;std::memcpy(&v,p,4);return v;}
u16 short_word(const u8* p){u16 v;std::memcpy(&v,p,2);return v;}
float volume(i32 db){return std::pow(10.f,float(db)/2000.f);}
class MixerLock {
public:
#ifdef _WIN32
    explicit MixerLock(SDL_AudioStream* stream):stream_(stream&&SDL_LockAudioStream(stream)?stream:nullptr){}
    ~MixerLock(){if(stream_)SDL_UnlockAudioStream(stream_);}
#else
    explicit MixerLock(SDL_AudioStream*){}
#endif
    MixerLock(const MixerLock&)=delete;
    MixerLock& operator=(const MixerLock&)=delete;
private:
#ifdef _WIN32
    SDL_AudioStream* stream_;
#endif
};
#ifdef _WIN32
bool read_packed_music(const u8* format, std::vector<u8>& wave){
    const auto start=word(format+16), length=word(format+28);
    if(start<8||!length||length>128*1024*1024||short_word(format+32)!=1)return false;
    auto* file=SDL_IOFromFile("thbgm.dat","rb");
    if(!file)return false;
    u8 header[8]{};
    const auto file_size=SDL_GetIOSize(file);
    const bool valid=file_size>=0&&u64(start)+length<=u64(file_size)&&SDL_ReadIO(file,header,sizeof(header))==sizeof(header)
        &&std::memcmp(header,"ZWAV",4)==0&&word(header+4)==1&&SDL_SeekIO(file,start,SDL_IO_SEEK_SET)==start;
    if(!valid){SDL_CloseIO(file);return false;}
    wave.resize(44+length);
    auto put=[&](size_t offset,u32 value){std::memcpy(wave.data()+offset,&value,4);};
    std::memcpy(wave.data(),"RIFF",4);put(4,36+length);
    std::memcpy(wave.data()+8,"WAVEfmt ",8);put(16,16);
    std::memcpy(wave.data()+20,format+32,16);
    std::memcpy(wave.data()+36,"data",4);put(40,length);
    const bool read=SDL_ReadIO(file,wave.data()+44,length)==length;
    SDL_CloseIO(file);
    return read;
}
#endif
}
struct AudioDevice::Impl {
    struct Voice {
        ma_decoder decoder{};ma_sound sound{};bool decoded=false,attached=false;
#ifdef _WIN32
        std::vector<u8> music_bytes;
#endif
        void detach(){if(attached){ma_sound_uninit(&sound);attached=false;}}
        ~Voice(){web_sfx::erase(&sound);detach();if(decoded)ma_decoder_uninit(&decoder);}
    };
    static constexpr i32 MixFrames=512,FrameBytes=2*sizeof(float);
    ma_engine engine{};SDL_AudioStream* stream=nullptr;bool engine_ready=false,ready=false,suspended=false,music_paused=false;
#ifndef _WIN32
    bool refill=true;
#endif
    std::array<std::unique_ptr<Voice>,54> sounds;std::array<std::vector<u8>,39> samples;std::unique_ptr<Voice> music;
    std::vector<u8> formats;i32 track=-1,fade=0,fade_total=0;mutable u32 stats[8]{};
    u32 pcmChunks=0,pcmFrames=0,pcmError=0;
#ifdef _WIN32
    bool room_music_audible = false;
    static void SDLCALL fill(void* userdata,SDL_AudioStream*,int additionalBytes,int){
        // SDL holds the stream lock and calls this on the device thread.
        auto& mixer=*static_cast<Impl*>(userdata);
        if(mixer.suspended)return;
        i32 remaining=additionalBytes/FrameBytes+(additionalBytes%FrameBytes!=0);
        while(remaining>0){
            const auto written=mixer.write_pcm(std::min(remaining,MixFrames));
            if(!written)return;
            remaining-=written;
        }
    }
#endif
    ~Impl(){
        if(stream)SDL_DestroyAudioStream(stream);
        music.reset();
        for(auto& sound:sounds)sound.reset();
        if(engine_ready)ma_engine_uninit(&engine);
    }
    i32 write_pcm(i32 requested){
        float pcm[MixFrames*2]{};
        ma_uint64 frames=0;
        ma_result read;
        {TH09_PERF_SCOPE(mix_cost,AudioMix);read=ma_engine_read_pcm_frames(&engine,pcm,requested,&frames);}
        TH09_PERF_COUNT(PcmFramesRead,frames);
        if(read!=MA_SUCCESS){TH09_PERF_COUNT(AudioReadErrors,1);pcmError=1;return 0;}
        if(!frames)return 0;
        for(auto& value:pcm)value=std::clamp(value,-1.f,1.f);
        bool queued;
        {TH09_PERF_SCOPE(put_cost,AudioPut);queued=SDL_PutAudioStreamData(stream,pcm,int(frames*FrameBytes));}
        if(!queued){TH09_PERF_COUNT(AudioPutErrors,1);pcmError=2;return 0;}
        ++pcmChunks;
        pcmFrames+=u32(frames);
        TH09_PERF_COUNT(PcmChunks,1);
        TH09_PERF_COUNT(PcmFramesQueued,frames);
        return i32(frames);
    }
    std::unique_ptr<Voice> detach_music(){
        if(music)music->detach();
        return std::move(music);
    }
    // TH09 never changes voice pitch or uses Doppler. Avoid the dynamic pitch
    // filter at unity rate; miniaudio still enables necessary resampling when
    // a decoder's sample rate differs from the engine's 44100 Hz.
    bool attach(Voice& v){if(ma_sound_init_from_data_source(&engine,&v.decoder,MA_SOUND_FLAG_NO_SPATIALIZATION|MA_SOUND_FLAG_NO_PITCH,nullptr,&v.sound)!=MA_SUCCESS)return false;v.attached=true;return true;}
    Voice* voice(u32 n){return n&&n<=sounds.size()?sounds[n-1].get():nullptr;}
};
AudioDevice::AudioDevice():impl(std::make_unique<Impl>()){}
AudioDevice::~AudioDevice()=default;
bool AudioDevice::initialize(ResourceReader& resources){
    TH09_PERF_SCOPE(initialize_cost,AudioInit);
    if(impl->ready)return true;if(impl->engine_ready)impl=std::make_unique<Impl>();auto& a=*impl;auto engine=ma_engine_config_init();engine.noDevice=MA_TRUE;engine.channels=2;engine.sampleRate=44100;engine.defaultVolumeSmoothTimeInPCMFrames=0;
    if(ma_engine_init(&engine,&a.engine)!=MA_SUCCESS){error="Unable to initialize audio mixer";return false;}a.engine_ready=true;
    SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES,"512");const SDL_AudioSpec spec{SDL_AUDIO_F32,2,44100};
    if(SDL_InitSubSystem(SDL_INIT_AUDIO))a.stream=SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,&spec,nullptr,nullptr);
    if(!a.stream){error=SDL_GetError();return false;}
    for(u32 n=0;n<a.samples.size();++n)if(!resources.read(sound_samples[n],a.samples[n])){error=std::string("Missing sound: ")+sound_samples[n];return false;}
    for(u32 n=0;n<a.sounds.size();++n){auto voice=std::make_unique<Impl::Voice>();const auto& bytes=a.samples[sound_definitions[n].sample];auto config=ma_decoder_config_init(ma_format_f32,2,44100);
        if(ma_decoder_init_memory(bytes.data(),bytes.size(),&config,&voice->decoder)!=MA_SUCCESS){error="Invalid TH09 WAV sample";return false;}voice->decoded=true;TH09_PERF_COUNT(SfxDecoderOpens,1);
        if(!a.attach(*voice)){error="Unable to attach sound voice";return false;}ma_uint64 frames=0,read=0;
        if(ma_decoder_get_length_in_pcm_frames(&voice->decoder,&frames)==MA_SUCCESS&&frames&&frames<=UINT32_MAX/8){
            std::vector<float> pcm(size_t(frames)*2);
            if(ma_decoder_read_pcm_frames(&voice->decoder,pcm.data(),frames,&read)==MA_SUCCESS&&read==frames)
                web_sfx::upload(&voice->sound,pcm.data(),unsigned(frames),2,44100);
            ma_decoder_seek_to_pcm_frame(&voice->decoder,0);
        }
        effects.buffers[n]=n+1;a.sounds[n]=std::move(voice);
    }
    if(!resources.read("thbgm.fmt",a.formats)){error="Missing TH09 music layout";return false;}
#ifdef _WIN32
    if(!SDL_SetAudioStreamGetCallback(a.stream,Impl::fill,&a)){error=SDL_GetError();return false;}
#endif
    a.ready=true;
    SDL_ResumeAudioStreamDevice(a.stream);
    error.clear();
    return true;
}
bool AudioDevice::music(i32 track){
    TH09_PERF_SCOPE(music_cost,MusicSelect);
    auto& a=*impl;
    if(!a.ready)return false;
    if(track<0){
        const MixerLock lock(a.stream);
        if(a.music)ma_sound_stop(&a.music->sound);
        a.track=-1;
        return true;
    }
    // A muted host transfers no OGG at all, so selecting the track must not open
    // the decoder; otherwise the title and every match fail on a missing file.
    if(!music_enabled){
        std::unique_ptr<Impl::Voice> previous;
        {
            const MixerLock lock(a.stream);
            previous=a.detach_music();
            a.track=track;
            a.fade=0;
            a.music_paused=false;
        }
        error.clear();
        return true;
    }
    const char* stem=nullptr;for(const auto& t:music_tracks)if(t.cue==track){stem=t.file;break;}if(!stem){error="Unknown TH09 music cue";return false;}
    const auto filename=std::string(stem)+".wav";const u8* format=nullptr;
    for(u32 n=0;n+52<=a.formats.size();n+=52)if(std::strncmp(reinterpret_cast<const char*>(a.formats.data()+n),filename.c_str(),16)==0){format=a.formats.data()+n;break;}
    if(!format){error="Music layout entry not found";return false;}
    if(a.track==track&&a.music){
        const MixerLock lock(a.stream);
        TH09_PERF_COUNT(MusicSameTrackRestarts,1);
        a.fade=0;
        a.music_paused=false;
        ma_sound_seek_to_pcm_frame(&a.music->sound,0);
        refresh_volume();
        ma_sound_start(&a.music->sound);
        return true;
    }
    auto next=std::make_unique<Impl::Voice>();const auto path=std::string(runtime_path("/music/"))+stem+".ogg";const u32 rate=word(format+36),align=short_word(format+44),channels=short_word(format+34),total=word(format+28),intro=word(format+24);
    if(!align||!rate||!channels||intro>=total){error="Invalid music loop";return false;}
    auto config=ma_decoder_config_init(ma_format_f32,channels,rate);
    auto decoded=MA_ERROR;
#ifdef _WIN32
    const auto loose=std::string("music/")+filename;
    if(read_file(path.c_str(),next->music_bytes))
        decoded=ma_decoder_init_memory(next->music_bytes.data(),next->music_bytes.size(),&config,&next->decoder);
    if(decoded!=MA_SUCCESS&&read_packed_music(format,next->music_bytes))
        decoded=ma_decoder_init_memory(next->music_bytes.data(),next->music_bytes.size(),&config,&next->decoder);
    if(decoded!=MA_SUCCESS&&read_file(loose.c_str(),next->music_bytes))
        decoded=ma_decoder_init_memory(next->music_bytes.data(),next->music_bytes.size(),&config,&next->decoder);
#else
    if(decoded!=MA_SUCCESS)decoded=ma_decoder_init_file(path.c_str(),&config,&next->decoder);
#endif
    if(decoded!=MA_SUCCESS){error="Unable to decode "+path;return false;}next->decoded=true;TH09_PERF_COUNT(MusicDecoderOpens,1);
    ma_uint64 length=0;if(ma_decoder_get_length_in_pcm_frames(&next->decoder,&length)!=MA_SUCCESS||length!=total/align){error="Music PCM frame count differs from original: "+path;return false;}
    ma_data_source_set_loop_point_in_pcm_frames(&next->decoder,intro/align,total/align);
    std::unique_ptr<Impl::Voice> previous;
    {
        const MixerLock lock(a.stream);
        if(!a.attach(*next)){error="Unable to attach music voice";return false;}
        ma_sound_set_looping(&next->sound,MA_TRUE);
        previous=a.detach_music();
        a.music=std::move(next);
        a.track=track;
        a.fade=0;
        a.music_paused=false;
        refresh_volume();
        ma_sound_start(&a.music->sound);
    }
    return true;
}
bool AudioDevice::music_file(const char* name){std::string file=name?name:"";const auto slash=file.find_last_of("/\\");if(slash!=std::string::npos)file.erase(0,slash+1);const auto dot=file.find_last_of('.');if(dot!=std::string::npos)file.erase(dot);for(const auto& track:music_tracks)if(file==track.file)return music(track.cue);error="Unknown music filename: "+file;return false;}
void AudioDevice::fade_music(i32 frames){auto& a=*impl;const MixerLock lock(a.stream);a.fade=a.fade_total=std::max(0,frames);if(!frames&&a.music)ma_sound_stop(&a.music->sound);}
void AudioDevice::pause_music(bool paused){auto& a=*impl;const MixerLock lock(a.stream);if(!a.music||a.music_paused==paused)return;a.music_paused=paused;if(paused)ma_sound_stop(&a.music->sound);else ma_sound_start(&a.music->sound);}
void AudioDevice::refresh_volume(){
    auto& a=*impl;
    const MixerLock lock(a.stream);
    bool audible=music_enabled;
#ifdef _WIN32
    audible=audible&&!a.room_music_audible;
#endif
    if(a.music)ma_sound_set_volume(&a.music->sound,audible?volume(SoundEffects::adjusted_volume(a.fade?a.fade*5000/a.fade_total-5000:0,music_volume,true)):0.f);
}
#ifdef _WIN32
void AudioDevice::set_room_music_audible(bool audible){
    if(impl->room_music_audible==audible)return;
    impl->room_music_audible=audible;
    refresh_volume();
}
#endif
void AudioDevice::update(){TH09_PERF_SCOPE(update_cost,AudioUpdate);auto& a=*impl;const MixerLock lock(a.stream);effects.process();if(a.music&&a.fade&&!a.music_paused){--a.fade;if(!a.fade)ma_sound_stop(&a.music->sound);}refresh_volume();}
void AudioDevice::pump(){
#ifndef _WIN32
    TH09_PERF_SCOPE(pump_cost,AudioPump);
    TH09_PERF_GAUGE(QueueFramesBefore,-1);TH09_PERF_GAUGE(QueueFramesAfterEstimate,-1);TH09_PERF_GAUGE(LastPumpChunks,0);
    auto& a=*impl;if(!a.ready||!a.stream||a.suspended)return;i32 queued=std::max(0,SDL_GetAudioStreamQueued(a.stream))/8;
    TH09_PERF_COUNT(PumpReadyCalls,1);TH09_PERF_COUNT(QueueEmptyCalls,queued==0?1:0);
    TH09_PERF_GAUGE(QueueFramesBefore,queued);TH09_PERF_GAUGE(QueueFramesAfterEstimate,queued);
    // Keep at most 46ms of PCM ahead of a fresh effect at 44100Hz. Previously
    // the 6144-frame queue alone could defer feedback by up to 139ms.
    if(!a.refill&&queued<1536)a.refill=true;if(!a.refill)return;if(queued>=2048){a.refill=false;return;}
    TH09_PERF_COUNT(PumpRefillCalls,1);
    for(i32 n=0;n<4&&queued<2048;++n){
        const auto written=a.write_pcm(std::min(Impl::MixFrames,2048-queued));
        if(!written)return;
        queued+=written;
        TH09_PERF_GAUGE(QueueFramesAfterEstimate,queued);TH09_PERF_GAUGE(LastPumpChunks,n+1);
    }
    if(queued>=2048)a.refill=false;
#endif
}
void AudioDevice::suspend(bool paused){
    auto& a=*impl;
    {
        const MixerLock lock(a.stream);
        a.suspended=paused;
    }
    // SDL takes the device lock before the stream lock in its audio callback.
    web_sfx::suspend(paused);
    if(a.stream){if(paused)SDL_PauseAudioStreamDevice(a.stream);else SDL_ResumeAudioStreamDevice(a.stream);}
}
void AudioDevice::sound_stop(u32 n){const MixerLock lock(impl->stream);if(auto* v=impl->voice(n)){web_sfx::stop(&v->sound);ma_sound_stop(&v->sound);}}
void AudioDevice::sound_position(u32 n,u32 frame){const MixerLock lock(impl->stream);if(auto* v=impl->voice(n)){web_sfx::seek(&v->sound,frame,44100);ma_sound_seek_to_pcm_frame(&v->sound,frame);}}
void AudioDevice::sound_pan(u32 n,i32 value){const MixerLock lock(impl->stream);if(auto* v=impl->voice(n)){const float pan=value>=0?1.f-std::pow(10.f,-float(value)/2000.f):std::pow(10.f,float(value)/2000.f)-1.f;web_sfx::pan(&v->sound,pan);ma_sound_set_pan_mode(&v->sound,ma_pan_mode_balance);ma_sound_set_pan(&v->sound,pan);}}
void AudioDevice::sound_volume(u32 n,i32 value){const MixerLock lock(impl->stream);if(auto* v=impl->voice(n)){web_sfx::volume(&v->sound,volume(value));ma_sound_set_volume(&v->sound,volume(value));}}
void AudioDevice::sound_play(u32 n){const MixerLock lock(impl->stream);if(auto* v=impl->voice(n))if(!web_sfx::play(&v->sound))ma_sound_start(&v->sound);}
const u32* AudioDevice::statistics()const{
    auto& a=*impl;
    const MixerLock lock(a.stream);
    a.stats[0]=a.ready;
    a.stats[1]=a.pcmChunks;
    a.stats[2]=a.pcmFrames;
    a.stats[3]=a.pcmError;
    a.stats[4]=a.stream?std::max(0,SDL_GetAudioStreamQueued(a.stream))/Impl::FrameBytes:0;
    a.stats[5]=u32(a.track);
    a.stats[6]=a.fade;
    a.stats[7]=a.music?u32(std::round(ma_sound_get_volume(&a.music->sound)*1000000.f)):0;
    return a.stats;
}
}
