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
namespace th09::sdl {
namespace {
u32 word(const u8* p){u32 v;std::memcpy(&v,p,4);return v;}
u16 short_word(const u8* p){u16 v;std::memcpy(&v,p,2);return v;}
float volume(i32 db){return std::pow(10.f,float(db)/2000.f);}
}
struct AudioDevice::Impl {
    struct Voice {ma_decoder decoder{};ma_sound sound{};bool decoded=false,attached=false;~Voice(){if(attached)ma_sound_uninit(&sound);if(decoded)ma_decoder_uninit(&decoder);}};
    ma_engine engine{};SDL_AudioStream* stream=nullptr;bool engine_ready=false,ready=false,suspended=false,refill=true,music_paused=false;
    std::array<std::unique_ptr<Voice>,54> sounds;std::array<std::vector<u8>,39> samples;std::unique_ptr<Voice> music;
    std::vector<u8> formats;i32 track=-1,fade=0,fade_total=0;mutable u32 stats[8]{};
    ~Impl(){music.reset();for(auto& sound:sounds)sound.reset();if(stream)SDL_DestroyAudioStream(stream);if(engine_ready)ma_engine_uninit(&engine);}
    bool attach(Voice& v){if(ma_sound_init_from_data_source(&engine,&v.decoder,MA_SOUND_FLAG_NO_SPATIALIZATION,nullptr,&v.sound)!=MA_SUCCESS)return false;v.attached=true;return true;}
    Voice* voice(u32 n){return n&&n<=sounds.size()?sounds[n-1].get():nullptr;}
};
AudioDevice::AudioDevice():impl(std::make_unique<Impl>()){}
AudioDevice::~AudioDevice()=default;
bool AudioDevice::initialize(ResourceReader& resources){
    if(impl->ready)return true;if(impl->engine_ready)impl=std::make_unique<Impl>();auto& a=*impl;auto engine=ma_engine_config_init();engine.noDevice=MA_TRUE;engine.channels=2;engine.sampleRate=44100;engine.defaultVolumeSmoothTimeInPCMFrames=0;
    if(ma_engine_init(&engine,&a.engine)!=MA_SUCCESS){error="Unable to initialize audio mixer";return false;}a.engine_ready=true;
    SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES,"2048");const SDL_AudioSpec spec{SDL_AUDIO_F32,2,44100};
    if(SDL_InitSubSystem(SDL_INIT_AUDIO))a.stream=SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,&spec,nullptr,nullptr);
    if(!a.stream){error=SDL_GetError();return false;}
    for(u32 n=0;n<a.samples.size();++n)if(!resources.read(sound_samples[n],a.samples[n])){error=std::string("Missing sound: ")+sound_samples[n];return false;}
    for(u32 n=0;n<a.sounds.size();++n){auto voice=std::make_unique<Impl::Voice>();const auto& bytes=a.samples[sound_definitions[n].sample];auto config=ma_decoder_config_init(ma_format_f32,2,44100);
        if(ma_decoder_init_memory(bytes.data(),bytes.size(),&config,&voice->decoder)!=MA_SUCCESS){error="Invalid TH09 WAV sample";return false;}voice->decoded=true;
        if(!a.attach(*voice)){error="Unable to attach sound voice";return false;}effects.buffers[n]=n+1;a.sounds[n]=std::move(voice);
    }
    if(!resources.read("thbgm.fmt",a.formats)){error="Missing TH09 music layout";return false;}SDL_ResumeAudioStreamDevice(a.stream);a.ready=true;error.clear();return true;
}
bool AudioDevice::music(i32 track){
    auto& a=*impl;if(!a.ready)return false;if(track<0){if(a.music)ma_sound_stop(&a.music->sound);a.track=-1;return true;}
    // A muted host transfers no OGG at all, so selecting the track must not open
    // the decoder; otherwise the title and every match fail on a missing file.
    if(!music_enabled){if(a.music){ma_sound_stop(&a.music->sound);a.music.reset();}a.track=track;a.fade=0;a.music_paused=false;error.clear();return true;}
    const char* stem=nullptr;for(const auto& t:music_tracks)if(t.cue==track){stem=t.file;break;}if(!stem){error="Unknown TH09 music cue";return false;}
    const auto filename=std::string(stem)+".wav";const u8* format=nullptr;
    for(u32 n=0;n+52<=a.formats.size();n+=52)if(std::strncmp(reinterpret_cast<const char*>(a.formats.data()+n),filename.c_str(),16)==0){format=a.formats.data()+n;break;}
    if(!format){error="Music layout entry not found";return false;}
    if(a.track==track&&a.music){a.fade=0;a.music_paused=false;ma_sound_seek_to_pcm_frame(&a.music->sound,0);ma_sound_set_volume(&a.music->sound,music_enabled?volume(SoundEffects::adjusted_volume(0,music_volume,true)):0.f);ma_sound_start(&a.music->sound);return true;}
    auto next=std::make_unique<Impl::Voice>();const auto path=std::string("/music/")+stem+".ogg";const u32 rate=word(format+36),align=short_word(format+44),channels=short_word(format+34),total=word(format+28),intro=word(format+24);
    if(!align||!rate||!channels||intro>=total){error="Invalid music loop";return false;}
    auto config=ma_decoder_config_init(ma_format_f32,channels,rate);
    if(ma_decoder_init_file(path.c_str(),&config,&next->decoder)!=MA_SUCCESS){error="Unable to decode "+path;return false;}next->decoded=true;
    ma_uint64 length=0;if(ma_decoder_get_length_in_pcm_frames(&next->decoder,&length)!=MA_SUCCESS||length!=total/align){error="Music PCM frame count differs from original: "+path;return false;}
    ma_data_source_set_loop_point_in_pcm_frames(&next->decoder,intro/align,total/align);
    if(!a.attach(*next)){error="Unable to attach music voice";return false;}ma_sound_set_looping(&next->sound,MA_TRUE);ma_sound_set_volume(&next->sound,music_enabled?volume(SoundEffects::adjusted_volume(0,music_volume,true)):0.f);
    a.music=std::move(next);a.track=track;a.fade=0;a.music_paused=false;ma_sound_start(&a.music->sound);return true;
}
bool AudioDevice::music_file(const char* name){std::string file=name?name:"";const auto slash=file.find_last_of("/\\");if(slash!=std::string::npos)file.erase(0,slash+1);const auto dot=file.find_last_of('.');if(dot!=std::string::npos)file.erase(dot);for(const auto& track:music_tracks)if(file==track.file)return music(track.cue);error="Unknown music filename: "+file;return false;}
void AudioDevice::fade_music(i32 frames){auto& a=*impl;a.fade=a.fade_total=std::max(0,frames);if(!frames&&a.music)ma_sound_stop(&a.music->sound);}
void AudioDevice::pause_music(bool paused){auto& a=*impl;if(!a.music||a.music_paused==paused)return;a.music_paused=paused;if(paused)ma_sound_stop(&a.music->sound);else ma_sound_start(&a.music->sound);}
void AudioDevice::refresh_volume(){auto& a=*impl;if(a.music)ma_sound_set_volume(&a.music->sound,music_enabled?volume(SoundEffects::adjusted_volume(a.fade?a.fade*5000/a.fade_total-5000:0,music_volume,true)):0.f);}
void AudioDevice::update(){auto& a=*impl;effects.process();if(a.music&&a.fade&&!a.music_paused){--a.fade;if(!a.fade)ma_sound_stop(&a.music->sound);}refresh_volume();}
void AudioDevice::pump(){
    auto& a=*impl;if(!a.ready||!a.stream||a.suspended)return;i32 queued=std::max(0,SDL_GetAudioStreamQueued(a.stream))/8;
    if(!a.refill&&queued<4096)a.refill=true;if(!a.refill)return;if(queued>=6144){a.refill=false;return;}
    for(i32 n=0;n<6&&queued<6144;++n){float pcm[2048]{};ma_uint64 frames=0;if(ma_engine_read_pcm_frames(&a.engine,pcm,1024,&frames)!=MA_SUCCESS){a.stats[3]=1;return;}for(auto& v:pcm)v=std::clamp(v,-1.f,1.f);if(!SDL_PutAudioStreamData(a.stream,pcm,sizeof(pcm))){a.stats[3]=2;return;}++a.stats[1];a.stats[2]+=1024;queued+=1024;}if(queued>=6144)a.refill=false;
}
void AudioDevice::suspend(bool paused){auto& a=*impl;a.suspended=paused;if(a.stream){if(paused)SDL_PauseAudioStreamDevice(a.stream);else SDL_ResumeAudioStreamDevice(a.stream);}}
void AudioDevice::sound_stop(u32 n){if(auto* v=impl->voice(n))ma_sound_stop(&v->sound);}
void AudioDevice::sound_position(u32 n,u32 frame){if(auto* v=impl->voice(n))ma_sound_seek_to_pcm_frame(&v->sound,frame);}
void AudioDevice::sound_pan(u32 n,i32 value){if(auto* v=impl->voice(n)){ma_sound_set_pan_mode(&v->sound,ma_pan_mode_balance);ma_sound_set_pan(&v->sound,value>=0?1.f-std::pow(10.f,-float(value)/2000.f):std::pow(10.f,float(value)/2000.f)-1.f);}}
void AudioDevice::sound_volume(u32 n,i32 value){if(auto* v=impl->voice(n))ma_sound_set_volume(&v->sound,volume(value));}
void AudioDevice::sound_play(u32 n){if(auto* v=impl->voice(n))ma_sound_start(&v->sound);}
const u32* AudioDevice::statistics()const{auto& a=*impl;a.stats[0]=a.ready;a.stats[4]=a.stream?std::max(0,SDL_GetAudioStreamQueued(a.stream))/8:0;a.stats[5]=u32(a.track);a.stats[6]=a.fade;a.stats[7]=a.music?u32(std::round(ma_sound_get_volume(&a.music->sound)*1000000.f)):0;return a.stats;}
}
