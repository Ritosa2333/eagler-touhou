#define MINIAUDIO_IMPLEMENTATION
#include "MiniaudioConfig.hpp"
#include "Audio.hpp"
#include "RuntimeOptions.hpp"
#include "Archive.hpp"
#include "BgmStream.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <eagler/audio/WebSfx.hpp>
namespace web_sfx = eagler::audio::web_sfx;
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace th12host {
namespace {
u32 word(const u8* p){u32 v;std::memcpy(&v,p,4);return v;}
u16 half(const u8* p){u16 v;std::memcpy(&v,p,2);return v;}
float native_gain(i32 base,i32 percent,i32 power){
    const double value=std::clamp(percent,0,100)/100.0;if(!value)return 0;
    const double db=std::trunc((base+5000)*(1-std::pow(1-value,power)))-5000;
    return float(std::pow(10.0,std::clamp(db,-10000.,0.)/2000));
}
}
float native_sound_gain(i32 base,i32 percent){return native_gain(base,percent,3);}
float native_music_gain(i32 percent){return native_gain(0,percent,2);}
std::vector<MusicTrack> parse_music_layout(const std::vector<u8>& fmt){
    std::vector<MusicTrack> out;
    for(size_t p=0;p+52<=fmt.size();p+=52){
        const u8* r=fmt.data()+p;std::string name;for(int i=0;i<16&&r[i];++i)name+=char(r[i]);if(name.empty())continue;
        MusicTrack t;t.name=name;t.offset=word(r+16);t.loop_start=word(r+24);t.length=word(r+28);t.channels=half(r+34);t.rate=word(r+36);t.align=half(r+44);
        if(half(r+32)!=1||half(r+46)!=16||!t.channels||!t.rate||t.align!=t.channels*2||t.loop_start>=t.length)continue;
        out.push_back(t);
    }
    return out;
}

struct Audio::Impl {
    struct Voice {ma_decoder decoder{};ma_sound sound{};bool decoded=false,attached=false,paused=false,playing=false;
        ~Voice(){web_sfx::erase(&sound);if(attached)ma_sound_uninit(&sound);if(decoded)ma_decoder_uninit(&decoder);}};
    struct Music {host::PcmStream stream;ma_sound sound{};ma_decoder decoder{};ma_audio_buffer_ref buffer{};std::vector<float> pcm;bool decoded=false,initialized=false,buffer_ready=false,attached=false;
        ~Music(){if(attached)ma_sound_uninit(&sound);if(initialized)ma_data_source_uninit(&stream.base);if(buffer_ready)ma_audio_buffer_ref_uninit(&buffer);if(decoded)ma_decoder_uninit(&decoder);}};
    ma_engine engine{};SDL_AudioStream* device=nullptr;bool engine_ready=false,ready=false,suspended=false,refill=true,music_paused=false;
    std::vector<std::vector<u8>> samples;std::array<std::unique_ptr<Voice>,64> voices;std::array<SoundRecord,64> records{};std::array<bool,64> known{};
    std::unique_ptr<Music> music;FILE* bgm=nullptr;std::vector<char> bgm_buffer;std::unique_ptr<host::BgmReader> reader;std::vector<MusicTrack> tracks;i32 track=-1;
    double fade_total=0,fade_elapsed=0;bool fading=false;
    ~Impl(){music.reset();for(auto& v:voices)v.reset();if(device)SDL_DestroyAudioStream(device);if(engine_ready)ma_engine_uninit(&engine);reader.reset();if(bgm)std::fclose(bgm);}
    u32 queued()const{return device?SDL_GetAudioStreamQueued(device)/8:0;}
};
Audio::Audio():impl(std::make_unique<Impl>()){}
Audio::~Audio()=default;
bool Audio::initialize(Archive& archive,const ExeTables& tables){
    auto& a=*impl;auto config=ma_engine_config_init();config.noDevice=MA_TRUE;config.channels=2;config.sampleRate=44100;config.defaultVolumeSmoothTimeInPCMFrames=0;
    if(ma_engine_init(&config,&a.engine)!=MA_SUCCESS){error="unable to initialise the audio mixer";return false;}a.engine_ready=true;
    if(SDL_InitSubSystem(SDL_INIT_AUDIO)){
        SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES,"1024");
        SDL_AudioSpec want{SDL_AUDIO_F32,2,44100};
        a.device=SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,&want,nullptr,nullptr);
    }
    if(!a.device)host::log("audio: no output device (%s); running silent",SDL_GetError());
    // Effects: each sound record's WAV, decoded once; one voice per id.
    std::vector<std::string> files;
    for(const auto& r:tables.sounds){if(r.id<0||r.id>=64)continue;a.records[size_t(r.id)]=r;a.known[size_t(r.id)]=true;
        size_t sample=std::find(files.begin(),files.end(),r.file)-files.begin();
        if(sample==files.size()){std::vector<u8> bytes;if(!archive.read(r.file,bytes)){host::log("audio: %s missing",r.file.c_str());continue;}files.push_back(r.file);a.samples.push_back(std::move(bytes));}
        auto v=std::make_unique<Impl::Voice>();const auto& bytes=a.samples[sample];auto dc=ma_decoder_config_init(ma_format_f32,2,44100);
        if(ma_decoder_init_memory(bytes.data(),bytes.size(),&dc,&v->decoder)!=MA_SUCCESS){host::log("audio: %s is not a PCM WAV",r.file.c_str());continue;}v->decoded=true;
        if(ma_sound_init_from_data_source(&a.engine,&v->decoder,MA_SOUND_FLAG_NO_SPATIALIZATION,nullptr,&v->sound)!=MA_SUCCESS)continue;v->attached=true;
        ma_sound_set_looping(&v->sound,r.loop?MA_TRUE:MA_FALSE);ma_sound_set_pan_mode(&v->sound,ma_pan_mode_balance);
#ifdef __EMSCRIPTEN__
        if(a.device){ma_uint64 frames=0,read=0;
            if(ma_decoder_get_length_in_pcm_frames(&v->decoder,&frames)==MA_SUCCESS&&frames&&frames<=UINT32_MAX/8){
                std::vector<float> pcm(size_t(frames)*2);
                if(ma_decoder_read_pcm_frames(&v->decoder,pcm.data(),frames,&read)==MA_SUCCESS&&read==frames)
                    web_sfx::upload(&v->sound,pcm.data(),unsigned(frames),2,44100);
                ma_decoder_seek_to_pcm_frame(&v->decoder,0);
            }
        }
#endif
        a.voices[size_t(r.id)]=std::move(v);
    }
    // Music: thbgm.fmt (inside th12.dat) + thbgm.dat (optional).
    std::vector<u8> fmt;if(archive.read("thbgm.fmt",fmt))a.tracks=parse_music_layout(fmt);else host::log("audio: thbgm.fmt missing, music disabled");
    a.bgm=host::open_file(host::paths().data_file("thbgm.dat"),"rb");
    if(a.bgm){a.bgm_buffer.resize(64*1024);std::setvbuf(a.bgm,a.bgm_buffer.data(),_IOFBF,a.bgm_buffer.size());a.reader=std::make_unique<host::BgmReader>(a.bgm);}
    else host::log("audio: using per-track OGG files");
    web_sfx::suspend(false);if(a.device)SDL_ResumeAudioStreamDevice(a.device);a.ready=true;return true;
}
void Audio::effect(i32 id,i32 pan,bool stop){
    auto& a=*impl;if(id<0||id>=64||!a.voices[size_t(id)])return;auto& v=*a.voices[size_t(id)];
    web_sfx::stop(&v.sound);ma_sound_stop(&v.sound);v.playing=false;v.paused=false;if(stop)return;
    // DirectSound attenuates only the channel away from the source.
    const i32 p=std::clamp(pan,-10000,10000);
    const float balance=p>0?1.f-std::pow(10.f,-float(p)/2000.f):p<0?std::pow(10.f,float(p)/2000.f)-1.f:0.f;
    const float gain=native_sound_gain(a.records[size_t(id)].volume,se_volume);
    web_sfx::pan(&v.sound,balance);ma_sound_set_pan(&v.sound,balance);
    web_sfx::volume(&v.sound,gain);ma_sound_set_volume(&v.sound,gain);
    web_sfx::seek(&v.sound,0,44100);ma_sound_seek_to_pcm_frame(&v.sound,0);
    if(!web_sfx::play(&v.sound,a.records[size_t(id)].loop))ma_sound_start(&v.sound);v.playing=true;
}
// 423020 stops every active buffer and remembers which were playing.
void Audio::pause_effects(){for(auto& v:impl->voices)if(v&&v->playing&&(web_sfx::contains(&v->sound)?web_sfx::playing(&v->sound):ma_sound_is_playing(&v->sound))){web_sfx::stop(&v->sound);ma_sound_stop(&v->sound);v->paused=true;}}
void Audio::resume_effects(){for(size_t id=0;id<impl->voices.size();++id)if(auto& v=impl->voices[id];v&&v->paused){v->paused=false;if(!web_sfx::play(&v->sound,impl->records[id].loop))ma_sound_start(&v->sound);}}
void Audio::stop_effects(){for(auto& v:impl->voices)if(v){web_sfx::stop(&v->sound);ma_sound_stop(&v->sound);v->playing=v->paused=false;}}
i32 Audio::track_count()const{return i32(impl->tracks.size());}
i32 Audio::track_index(const std::string& name)const{
    for(size_t i=0;i<impl->tracks.size();++i){const auto& n=impl->tracks[i].name;if(n==name||n==name+".wav")return i32(i);}return -1;
}
void Audio::music_failure(const char* reason){
    error=reason;const auto track=music_name();host::log("audio: %s: %s",track.c_str(),reason);
#ifdef __EMSCRIPTEN__
    EM_ASM({Module.th12Error?.(UTF8ToString($0),"audio-resource",UTF8ToString($1));},reason,track.c_str());
#endif
}
void Audio::play_music(i32 index){
    auto& a=*impl;if(!a.ready)return;
    if(index<0||index>=i32(a.tracks.size())){stop_music();return;}
    a.track=index;a.music_paused=false;a.fading=false;error.clear();
    if(!runtime_options.music){a.music.reset();if(a.reader)a.reader->stop();if(a.device)SDL_ClearAudioStream(a.device);return;}
    if(!a.bgm){
        const auto& t=a.tracks[size_t(index)];std::string stem=t.name;const auto dot=stem.rfind('.');if(dot!=std::string::npos)stem.resize(dot);
        const auto filename=host::paths().data_file(("music/"+stem+".ogg").c_str());
        a.music.reset();auto next=std::make_unique<Impl::Music>();auto config=ma_decoder_config_init(ma_format_f32,2,44100);
        if(ma_decoder_init_file(filename.c_str(),&config,&next->decoder)!=MA_SUCCESS){
            // The browser fetches an absent track asynchronously, then retries.
            // Existing files have already arrived and must decode successfully.
            if(host::exists(filename))music_failure("cannot decode music resource");
            else host::log("audio: %s awaiting resource",filename.c_str());
            return;
        }next->decoded=true;
        ma_data_source* source=&next->decoder;
        if(runtime_options.full_music){
            // Decode this track before attaching it to the mixer. Keep one PCM
            // owner, released on track change; no unbounded all-track cache.
            constexpr ma_uint64 MaxFrames=256ull*1024*1024/(sizeof(float)*2);
            ma_uint64 length=0;if(ma_decoder_get_length_in_pcm_frames(&next->decoder,&length)!=MA_SUCCESS||!length||length>MaxFrames){music_failure("OGG decoded length is invalid or exceeds the 256 MiB track limit");return;}
            next->pcm.resize(size_t(length*2));ma_uint64 done=0;
            while(done<length){ma_uint64 read=0;const auto result=ma_decoder_read_pcm_frames(&next->decoder,next->pcm.data()+done*2,std::min<ma_uint64>(16384,length-done),&read);done+=read;if(result!=MA_SUCCESS||!read)break;}
            if(done!=length){music_failure("OGG full decode did not produce the complete track");return;}
            next->pcm.resize(size_t(done*2));ma_decoder_uninit(&next->decoder);next->decoded=false;
            if(ma_audio_buffer_ref_init(ma_format_f32,2,next->pcm.data(),done,&next->buffer)!=MA_SUCCESS){music_failure("cannot initialize decoded music buffer");return;}
            next->buffer_ready=true;source=&next->buffer;
        }
        ma_data_source_set_loop_point_in_pcm_frames(source,t.loop_start/t.align,t.length/t.align);
        if(ma_sound_init_from_data_source(&a.engine,source,MA_SOUND_FLAG_NO_SPATIALIZATION,nullptr,&next->sound)!=MA_SUCCESS){music_failure("cannot attach music to mixer");return;}
        next->attached=true;ma_sound_set_looping(&next->sound,MA_TRUE);ma_sound_set_volume(&next->sound,native_music_gain(bgm_volume));
        a.music=std::move(next);ma_sound_start(&a.music->sound);return;
    }
    const auto& t=a.tracks[size_t(index)];a.music.reset();auto next=std::make_unique<Impl::Music>();auto& s=next->stream;
    s.reader=a.reader.get();s.offset=t.offset;s.frames=t.length/t.align;s.loop_start=t.loop_start/t.align;s.channels=t.channels;s.rate=t.rate;
    auto config=ma_data_source_config_init();config.vtable=&host::pcm_vtable;
    if(ma_data_source_init(&config,&s.base)!=MA_SUCCESS){music_failure("cannot open PCM music source");return;}next->initialized=true;
    a.reader->start(s.offset,s.frames,s.loop_start,s.channels,0);
    if(host::pcm_seek(&s,0)!=MA_SUCCESS){music_failure("cannot seek thbgm.dat");return;}
    ma_data_source_set_loop_point_in_pcm_frames(&s,s.loop_start,s.frames);
    if(ma_sound_init_from_data_source(&a.engine,&s,MA_SOUND_FLAG_NO_SPATIALIZATION,nullptr,&next->sound)!=MA_SUCCESS){music_failure("cannot attach PCM music to mixer");return;}next->attached=true;
    ma_sound_set_looping(&next->sound,MA_TRUE);ma_sound_set_volume(&next->sound,native_music_gain(bgm_volume));
    a.music=std::move(next);ma_sound_start(&a.music->sound);
}
void Audio::stop_music(){error.clear();auto& a=*impl;if(a.music)ma_sound_stop(&a.music->sound);a.music.reset();if(a.reader)a.reader->stop();a.track=-1;a.music_paused=false;a.fading=false;}
void Audio::retry_music(){auto& a=*impl;if(!runtime_options.music){a.music.reset();if(a.reader)a.reader->stop();if(a.device)SDL_ClearAudioStream(a.device);return;}if(!a.music&&a.track>=0){const bool paused=a.music_paused;play_music(a.track);pause_music(paused);}}
std::string Audio::music_name()const{const auto& a=*impl;if(a.track<0||a.track>=i32(a.tracks.size()))return "";std::string n=a.tracks[size_t(a.track)].name;const auto dot=n.find('.');return dot==std::string::npos?n:n.substr(0,dot);}
bool Audio::music_paused()const{return impl->music_paused;}
void Audio::fade_music(double seconds){auto& a=*impl;if(!a.music)return;a.fading=true;a.fade_total=std::max(seconds,1e-6);a.fade_elapsed=0;}
void Audio::set_volumes(i32 bgm,i32 se){
    se_volume=std::clamp(se,0,100);bgm_volume=std::clamp(bgm,0,100);auto& a=*impl;
    if(a.music&&!a.fading)ma_sound_set_volume(&a.music->sound,native_music_gain(bgm_volume));
    for(i32 id=0;id<64;++id)if(a.voices[size_t(id)]){const float gain=native_sound_gain(a.records[size_t(id)].volume,se_volume);web_sfx::volume(&a.voices[size_t(id)]->sound,gain);ma_sound_set_volume(&a.voices[size_t(id)]->sound,gain);}
}
void Audio::update(double seconds){
    auto& a=*impl;if(!a.fading||!a.music||a.music_paused)return;a.fade_elapsed+=seconds;
    // Exponential in the decibel ramp, down to -50 dB, then silence.
    const double t=std::min(1.0,a.fade_elapsed/a.fade_total),from=native_music_gain(bgm_volume);
    if(t>=1||from<=0){stop_music();return;}
    ma_sound_set_volume(&a.music->sound,float(from*std::pow(std::pow(10.0,-2.5)/from,t)));
}
void Audio::pause_music(bool paused){auto& a=*impl;if(a.music_paused==paused)return;a.music_paused=paused;if(paused&&a.device)SDL_ClearAudioStream(a.device);if(!a.music)return;if(paused)ma_sound_stop(&a.music->sound);else ma_sound_start(&a.music->sound);}
bool Audio::mix(float* pcm,u32 frames){
    ma_uint64 read=0;if(!impl->ready||ma_engine_read_pcm_frames(&impl->engine,pcm,frames,&read)!=MA_SUCCESS||read!=frames)return false;
    for(u32 n=0;n<frames*2;++n)pcm[n]=std::clamp(pcm[n],-1.f,1.f);return true;
}
void Audio::pump(){
    auto& a=*impl;if(!a.ready||!a.device||a.suspended)return;i32 queued=i32(a.queued());
    if(!a.refill&&queued<4096)a.refill=true;if(!a.refill)return;if(queued>=6144){a.refill=false;return;}
    for(i32 n=0;n<6&&queued<6144;++n){float pcm[2048]{};if(!mix(pcm,1024))return;if(!SDL_PutAudioStreamData(a.device,pcm,sizeof(pcm)))return;queued+=1024;}
    if(queued>=6144)a.refill=false;
}
void Audio::suspend(bool paused){auto& a=*impl;a.suspended=paused;web_sfx::suspend(paused);if(a.device)(paused?SDL_PauseAudioStreamDevice(a.device):SDL_ResumeAudioStreamDevice(a.device));}
}

#undef STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.h"
