#define MA_NO_DEVICE_IO
#define MA_NO_RESOURCE_MANAGER
#define MA_NO_MP3
#define MA_NO_FLAC
#define MA_NO_ENCODING
#define MA_NO_THREADING
#include "../../../portable/sdl/third_party/stb_vorbis.h"
#define MINIAUDIO_IMPLEMENTATION
#include "../../../portable/sdl/third_party/miniaudio.h"
#include "AudioDevice.hpp"
#include "../game/MusicCatalog.hpp"
#include <cmath>
#include <algorithm>
#include <cstdio>
#include <cerrno>
namespace th11::sdl {
namespace {
u32 word(const u8* p){u32 v;std::memcpy(&v,p,4);return v;}
u16 short_word(const u8* p){u16 v;std::memcpy(&v,p,2);return v;}
float volume(i32 db){return std::pow(10.f,float(db)/2000.f);}
}
struct AudioDevice::Impl {
    struct Voice {ma_decoder decoder{};ma_sound sound{};bool decoded=false,attached=false;~Voice(){if(attached)ma_sound_uninit(&sound);if(decoded)ma_decoder_uninit(&decoder);}};
    ma_engine engine{};SDL_AudioStream* stream=nullptr;bool engine_ready=false,ready=false,suspended=false,refill=true,music_paused=false;
#ifdef TH11_MULTIPLAYER
    bool spectator_silent=false;
#endif
    std::array<std::unique_ptr<Voice>,56> sounds;std::array<std::vector<u8>,46> samples;std::unique_ptr<Voice> music;
    std::vector<u8> formats;i32 track=-1,pending_track=-1,fade=0,fade_total=0;ma_uint64 pending_frames=0;bool music_retry=false;mutable u32 stats[9]{};
    // Impl lifetime, never journaled: version, words, successful/failed starts,
    // last ma_result/handle, invalid handles, fixed capacity; 56 counts per kind.
    std::array<u32,120> sound_stats{1,120,0,0,0,0,0,56};
    ~Impl(){music.reset();for(auto& sound:sounds)sound.reset();if(stream)SDL_DestroyAudioStream(stream);if(engine_ready)ma_engine_uninit(&engine);}
    // TH11 never changes pitch. Skip the unity-rate resampler and its one-PCM-
    // frame history; miniaudio still enables conversion for unequal source rates.
    bool attach(Voice& v){if(ma_sound_init_from_data_source(&engine,&v.decoder,MA_SOUND_FLAG_NO_SPATIALIZATION|MA_SOUND_FLAG_NO_PITCH,nullptr,&v.sound)!=MA_SUCCESS)return false;v.attached=true;return true;}
    Voice* voice(u32 n){return n&&n<=sounds.size()?sounds[n-1].get():nullptr;}
};
AudioDevice::AudioDevice():impl(std::make_unique<Impl>()){}
AudioDevice::~AudioDevice()=default;
void AudioDevice::close(){effects.reset();impl=std::make_unique<Impl>();}
bool AudioDevice::initialize(GameResources& resources){
    if(impl->ready)return true;if(impl->engine_ready)impl=std::make_unique<Impl>();auto& a=*impl;auto engine=ma_engine_config_init();engine.noDevice=MA_TRUE;engine.channels=2;engine.sampleRate=44100;engine.defaultVolumeSmoothTimeInPCMFrames=0;
    if(ma_engine_init(&engine,&a.engine)!=MA_SUCCESS){error="Unable to initialize audio mixer";return false;}a.engine_ready=true;
#ifdef TH11_MULTIPLAYER
    if(a.spectator_silent)ma_engine_set_volume(&a.engine,0.f);
#endif
    SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES,"2048");const SDL_AudioSpec spec{SDL_AUDIO_F32,2,44100};
    if(SDL_InitSubSystem(SDL_INIT_AUDIO))a.stream=SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,&spec,nullptr,nullptr);
    if(!a.stream){error=SDL_GetError();return false;}
    for(u32 n=0;n<a.samples.size();++n)if(!resources.read(sound_samples[n],a.samples[n])){error=std::string("Missing sound: ")+sound_samples[n];return false;}
    for(u32 n=0;n<a.sounds.size();++n){auto voice=std::make_unique<Impl::Voice>();const auto& bytes=a.samples[sound_definitions[n].sample];auto config=ma_decoder_config_init(ma_format_f32,2,44100);
        if(ma_decoder_init_memory(bytes.data(),bytes.size(),&config,&voice->decoder)!=MA_SUCCESS){error="Invalid TH11 WAV sample";return false;}voice->decoded=true;
        if(!a.attach(*voice)){error="Unable to attach sound voice";return false;}effects.buffers[n]=n+1;a.sounds[n]=std::move(voice);
    }
    if(!resources.read("thbgm.fmt",a.formats)){error="Missing TH11 music layout";return false;}SDL_ResumeAudioStreamDevice(a.stream);a.ready=true;error.clear();return true;
}
bool AudioDevice::music(i32 track){
    auto& a=*impl;if(!a.ready)return false;if(track<0){a.pending_track=-1;a.music_retry=false;if(a.music)ma_sound_stop(&a.music->sound);a.track=-1;return true;}
    const char* stem=nullptr;for(const auto& t:music_tracks)if(t.cue==track){stem=t.file;break;}if(!stem){error="Unknown TH11 music cue";return false;}
    const auto filename=std::string(stem)+".wav";const u8* format=nullptr;
    for(u32 n=0;n+52<=a.formats.size();n+=52)if(std::strncmp(reinterpret_cast<const char*>(a.formats.data()+n),filename.c_str(),16)==0){format=a.formats.data()+n;break;}
    if(!format){error="Music layout entry not found";return false;}
    if(a.track==track&&a.music){a.fade=0;a.music_paused=false;ma_sound_seek_to_pcm_frame(&a.music->sound,0);ma_sound_set_volume(&a.music->sound,music_enabled?volume(SoundEffects::adjusted_volume(0,music_volume,true)):0.f);ma_sound_start(&a.music->sound);return true;}
    auto next=std::make_unique<Impl::Voice>();const auto path=std::string("/music/")+stem+".ogg";const u32 rate=word(format+36),align=short_word(format+44),channels=short_word(format+34),total=word(format+28),intro=word(format+24);
    if(!align||!rate||!channels||intro>=total){error="Invalid music loop";return false;}
    // Match TH08/TH10's progressive OGG contract: missing music is silence,
    // not a fatal game error. Only the resource-write notification retries it.
    if(auto* file=std::fopen(path.c_str(),"rb"))std::fclose(file);
    else if(errno==ENOENT){
        a.music.reset();a.track=track;a.pending_track=track;a.pending_frames=0;
        a.music_retry=false;a.fade=0;a.music_paused=false;error.clear();return true;
    }
    auto config=ma_decoder_config_init(ma_format_f32,channels,rate);
    if(ma_decoder_init_file(path.c_str(),&config,&next->decoder)!=MA_SUCCESS){error="Unable to decode "+path;return false;}next->decoded=true;
    ma_uint64 length=0;if(ma_decoder_get_length_in_pcm_frames(&next->decoder,&length)!=MA_SUCCESS||length!=total/align){error="Music PCM frame count differs from original: "+path;return false;}
    ma_data_source_set_loop_point_in_pcm_frames(&next->decoder,intro/align,total/align);
    if(!a.attach(*next)){error="Unable to attach music voice";return false;}ma_sound_set_looping(&next->sound,MA_TRUE);ma_sound_set_volume(&next->sound,music_enabled?volume(SoundEffects::adjusted_volume(0,music_volume,true)):0.f);
    a.music=std::move(next);a.track=track;a.pending_track=-1;a.music_retry=false;a.fade=0;a.music_paused=false;ma_sound_start(&a.music->sound);return true;
}
bool AudioDevice::music_file(const char* name){std::string file=name?name:"";const auto slash=file.find_last_of("/\\");if(slash!=std::string::npos)file.erase(0,slash+1);const auto dot=file.find_last_of('.');if(dot!=std::string::npos)file.erase(dot);for(const auto& track:music_tracks)if(file==track.file)return music(track.cue);error="Unknown music filename: "+file;return false;}
void AudioDevice::fade_music(i32 frames){auto& a=*impl;a.fade=a.fade_total=std::max(0,frames);if(!frames){a.pending_track=-1;a.music_retry=false;if(a.music)ma_sound_stop(&a.music->sound);}}
void AudioDevice::pause_music(bool paused){auto& a=*impl;if(a.music_paused==paused)return;a.music_paused=paused;if(!a.music)return;if(paused)ma_sound_stop(&a.music->sound);else ma_sound_start(&a.music->sound);}
void AudioDevice::refresh_volume(){auto& a=*impl;if(a.music)ma_sound_set_volume(&a.music->sound,music_enabled?volume(SoundEffects::adjusted_volume(a.fade?a.fade*5000/a.fade_total-5000:0,music_volume,true)):0.f);}
void AudioDevice::resource_changed(){if(impl->pending_track>=0)impl->music_retry=true;}
bool AudioDevice::update(){
    auto& a=*impl;effects.process();
    if(a.pending_track>=0&&a.music_retry){
        const auto track=a.pending_track;const auto cursor=a.pending_frames;const auto fade=a.fade,total=a.fade_total;const bool paused=a.music_paused;
        if(!music(track))return false;
        if(a.pending_track<0&&a.music){
            const char* stem=nullptr;for(const auto& t:music_tracks)if(t.cue==track){stem=t.file;break;}
            const auto name=std::string(stem)+".wav";const u8* f=nullptr;
            for(u32 n=0;n+52<=a.formats.size();n+=52)if(std::strncmp(reinterpret_cast<const char*>(a.formats.data()+n),name.c_str(),16)==0){f=a.formats.data()+n;break;}
            if(!f)return false;const auto align=short_word(f+44);const ma_uint64 intro=word(f+24)/align,end=word(f+28)/align;
            const auto source_cursor=cursor*word(f+36)/44100;const auto frame=source_cursor<end?source_cursor:intro+(source_cursor-intro)%(end-intro);
            ma_sound_seek_to_pcm_frame(&a.music->sound,frame);
        }
        a.pending_frames=cursor;a.fade=fade;a.fade_total=total;pause_music(paused);
    }
    if((a.music||a.pending_track>=0)&&a.fade&&!a.music_paused){--a.fade;if(!a.fade){if(a.music)ma_sound_stop(&a.music->sound);a.pending_track=-1;}}
    refresh_volume();return true;
}
bool AudioDevice::mix(float* pcm,u32 frames){
    if(impl->pending_track>=0&&music_enabled&&!impl->music_paused)impl->pending_frames+=frames;
    ma_uint64 read=0;if(!impl->ready||!pcm||ma_engine_read_pcm_frames(&impl->engine,pcm,frames,&read)!=MA_SUCCESS||read!=frames)return false;
    for(u32 n=0;n<frames*2;++n)pcm[n]=std::clamp(pcm[n],-1.f,1.f);return true;
}
const double* AudioDevice::pump_profile(int enabled){
    if(enabled>=0)pump_profile_[2]=enabled!=0;return pump_profile_.data();
}
void AudioDevice::begin_pump_profile_frame(){
    auto& p=pump_profile_;if(!p[2])return;const double callback=p[3]+1;
    p.fill(0);p[0]=1;p[1]=18;p[2]=1;p[3]=callback;p[14]=p[15]=-1;
}
void AudioDevice::pump(){
    if(!pump_profile_[2]){
        // Keep the default path free of clocks and additional queue reads.
        auto& a=*impl;if(!a.ready||!a.stream||a.suspended)return;i32 queued=std::max(0,SDL_GetAudioStreamQueued(a.stream))/8;
        if(!a.refill&&queued<4096)a.refill=true;if(!a.refill)return;if(queued>=6144){a.refill=false;return;}
        for(i32 n=0;n<6&&queued<6144;++n){float pcm[2048]{};if(!mix(pcm,1024)){a.stats[3]=1;return;}if(!SDL_PutAudioStreamData(a.stream,pcm,sizeof(pcm))){a.stats[3]=2;return;}++a.stats[1];a.stats[2]+=1024;queued+=1024;}if(queued>=6144)a.refill=false;
        return;
    }
    auto& a=*impl;auto& p=pump_profile_;const auto now=[](){return double(SDL_GetTicksNS())/1000000.;};
    const double started=now();++p[4];
    const auto queue=[&](){const double began=now();const i32 frames=std::max(0,SDL_GetAudioStreamQueued(a.stream))/8;p[12]+=now()-began;++p[13];return frames;};
    const auto finish=[&](){
        // Only diagnostics read the real queue after the unchanged refill loop.
        // This added read is included in queue time and total pump time.
        if(a.ready&&a.stream&&!a.suspended)p[15]=queue();
        p[9]+=now()-started;
    };
    if(!a.ready||!a.stream||a.suspended){finish();return;}
    i32 queued=queue();if(p[14]<0)p[14]=queued;
    if(!a.refill&&queued<4096)a.refill=true;if(!a.refill){finish();return;}if(queued>=6144){a.refill=false;finish();return;}
    for(i32 n=0;n<6&&queued<6144;++n){
        float pcm[2048]{};const double mixAt=now();++p[5];const bool mixed=mix(pcm,1024);p[10]+=now()-mixAt;
        if(!mixed){++p[16];a.stats[3]=1;finish();return;}p[7]+=1024;
        const double putAt=now();const bool submitted=SDL_PutAudioStreamData(a.stream,pcm,sizeof(pcm));p[11]+=now()-putAt;
        if(!submitted){++p[17];a.stats[3]=2;finish();return;}++p[6];p[8]+=1024;
        ++a.stats[1];a.stats[2]+=1024;queued+=1024;
    }
    if(queued>=6144)a.refill=false;finish();
}
void AudioDevice::suspend(bool paused){auto& a=*impl;a.suspended=paused;if(a.stream){if(paused)SDL_PauseAudioStreamDevice(a.stream);else SDL_ResumeAudioStreamDevice(a.stream);}}
#ifdef TH11_MULTIPLAYER
void AudioDevice::spectator_silent(bool quiet){
    auto& a=*impl;if(a.spectator_silent==quiet)return;a.spectator_silent=quiet;
    if(quiet)for(auto& voice:a.sounds)if(voice)ma_sound_stop(&voice->sound);
    if(a.stream)SDL_ClearAudioStream(a.stream);
    if(a.engine_ready)ma_engine_set_volume(&a.engine,quiet?0.f:1.f);
}
#endif
void AudioDevice::sound_stop(u32 n){if(auto* v=impl->voice(n))ma_sound_stop(&v->sound);}
void AudioDevice::sound_position(u32 n,u32 frame){if(auto* v=impl->voice(n))ma_sound_seek_to_pcm_frame(&v->sound,frame);}
void AudioDevice::sound_pan(u32 n,i32 value){if(auto* v=impl->voice(n)){ma_sound_set_pan_mode(&v->sound,ma_pan_mode_balance);ma_sound_set_pan(&v->sound,value>=0?1.f-std::pow(10.f,-float(value)/2000.f):std::pow(10.f,float(value)/2000.f)-1.f);}}
void AudioDevice::sound_volume(u32 n,i32 value){if(auto* v=impl->voice(n))ma_sound_set_volume(&v->sound,volume(value));}
void AudioDevice::sound_play(u32 n){
    auto& a=*impl;
#ifdef TH11_MULTIPLAYER
    if(a.spectator_silent)return;
#endif
    if(auto* v=a.voice(n)){
        const auto result=ma_sound_start(&v->sound); // Same single call and playback semantics.
        if(result==MA_SUCCESS){++a.sound_stats[2];++a.sound_stats[8+n-1];}
        else{++a.sound_stats[3];++a.sound_stats[64+n-1];a.sound_stats[4]=u32(result);a.sound_stats[5]=n;}
    }else ++a.sound_stats[6];
}
const u32* AudioDevice::sound_statistics()const{return impl->sound_stats.data();}
const u32* AudioDevice::statistics()const{auto& a=*impl;a.stats[0]=a.ready;a.stats[4]=a.stream?std::max(0,SDL_GetAudioStreamQueued(a.stream))/8:0;a.stats[5]=u32(a.track);a.stats[6]=a.fade;a.stats[7]=a.music?u32(std::round(ma_sound_get_volume(&a.music->sound)*1000000.f)):0;a.stats[8]=a.music_paused;return a.stats;}
}
