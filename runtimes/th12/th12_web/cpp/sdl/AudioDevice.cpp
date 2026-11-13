#define MA_NO_DEVICE_IO
#define MA_NO_RESOURCE_MANAGER
#define MA_NO_MP3
#if PORT_TITLE != 14
#define MA_NO_FLAC
#endif
#define MA_NO_ENCODING
#define MA_NO_THREADING
#include "../../../portable/sdl/third_party/stb_vorbis.h"
#define MINIAUDIO_IMPLEMENTATION
#include "../../../portable/sdl/third_party/miniaudio.h"
#include "AudioDevice.hpp"
#include "../game/MusicCatalog.hpp"
#include "../game/Th14MusicFade.hpp"
#include <cmath>
#include <algorithm>
#if PORT_TITLE == 14
#include "Th14MusicStream.hpp"
#endif
namespace th11::sdl {
namespace {
u32 word(const u8* p){u32 v;std::memcpy(&v,p,4);return v;}
u16 short_word(const u8* p){u16 v;std::memcpy(&v,p,2);return v;}
float volume(i32 db){return std::pow(10.f,float(db)/2000.f);}
}
struct AudioDevice::Impl {
#if PORT_TITLE == 14
    struct MusicTime {
        double started=0,paused=0,pause_started=0;
        u32 rate=0,total=0,intro=0,channels=0,bits=0;
        double position(double now)const {
            // Whole retail44beb0: preserve the double operation order.
            double value=now-(paused+started);
            const double end=double(total)/(double(rate)*0.125)/double(bits)/double(channels);
            const double loop=double(total-intro)/double(rate)/(double(bits)*0.125)/double(channels);
            while(value>=end)value-=loop;
            return value;
        }
    };
    double suspend_started=0,suspended_time=0,saved_position=0;
    double clock()const {
        const double now=double(SDL_GetTicksNS())/1000000000.;
        return (suspended?suspend_started:now)-suspended_time;
    }
#endif
#if PORT_TITLE == 14
    struct MusicPcm {std::string filename;const u8* format=nullptr;std::vector<u8> wave;};
    std::array<std::shared_ptr<MusicPcm>,2> preloaded_music{};
    std::array<std::string,2> preloaded_names{};
    Th14MusicQueue music_requests;
    bool native_direct_sound=false;
    u32 preload_calls=0,preload_cache_hits=0,initial_request_calls=0;
#endif
#if PORT_TITLE == 14
    // DirectSound SetPan keeps one channel at unity and attenuates the other
    // by the supplied hundredths of a dB. Keep that gain directly: mapping it
    // through miniaudio's normalized pan loses precision in 1-(1-gain).
    struct Balance {
        ma_node_base base{};
        float left=1,right=1;
        bool initialized=false;
        static void process(ma_node* node,const float** input,ma_uint32* in_count,
                            float** output,ma_uint32* out_count){
            auto& self=*static_cast<Balance*>(node);
            const ma_uint32 count=std::min(*in_count,*out_count);
            for(ma_uint32 i=0;i<count;++i){
                output[0][i*2]=float(input[0][i*2]*self.left);
                output[0][i*2+1]=float(input[0][i*2+1]*self.right);
            }
            *in_count=*out_count=count;
        }
        bool initialize(ma_engine& engine){
            static const ma_node_vtable table{process,nullptr,1,1,0};
            const ma_uint32 channels=2;
            auto config=ma_node_config_init();config.vtable=&table;
            config.pInputChannels=&channels;config.pOutputChannels=&channels;
            auto* graph=ma_engine_get_node_graph(&engine);
            if(ma_node_init(graph,&config,nullptr,&base)!=MA_SUCCESS)return false;
            initialized=true;
            return ma_node_attach_output_bus(&base,0,ma_node_graph_get_endpoint(graph),0)==MA_SUCCESS;
        }
        void pan(i32 value){
            if(value<-10000||value>10000)return;
            const float gain=volume(value>0?-value:value);
            left=value>0?gain:1.f;right=value<0?gain:1.f;
        }
        ~Balance(){if(initialized)ma_node_uninit(&base,nullptr);}
    };
#endif
    struct Voice {
#if PORT_TITLE == 14
        MusicTime time;
        Balance balance;bool use_balance=false;
        std::shared_ptr<MusicPcm> cached_pcm;
        std::unique_ptr<Th14MusicStream> music_stream;
#endif
        ma_decoder decoder{};ma_sound sound{};bool decoded=false,attached=false;~Voice(){if(attached)ma_sound_uninit(&sound);
#if PORT_TITLE == 14
        music_stream.reset();
#endif
        if(decoded)ma_decoder_uninit(&decoder);}};
    ma_engine engine{};SDL_AudioStream* stream=nullptr;bool engine_ready=false,ready=false,suspended=false,refill=true,music_paused=false;
    std::array<std::unique_ptr<Voice>,sound_voice_count> sounds;std::array<std::vector<u8>,96> samples;std::unique_ptr<Voice> music,saved_music;
    std::vector<u8> formats;i32 track=-1,saved_track=-1,fade=0,fade_total=0;mutable u32 stats[14]{};
#if PORT_TITLE == 14
    Th14MusicFade music_fade;
    std::array<bool,sound_voice_count> paused_sounds{};bool sounds_paused=false;
#endif
    ~Impl(){music.reset();saved_music.reset();for(auto& sound:sounds)sound.reset();if(stream)SDL_DestroyAudioStream(stream);if(engine_ready)ma_engine_uninit(&engine);}
    bool attach(Voice& v){
        ma_uint32 flags=MA_SOUND_FLAG_NO_SPATIALIZATION;
#if PORT_TITLE == 14
        // Retail plays fixed-rate PCM;avoid the mixer's optional pitch path.
        flags|=MA_SOUND_FLAG_NO_PITCH;
#endif
        ma_data_source* source=&v.decoder;
#if PORT_TITLE == 14
        if(v.music_stream)source=&v.music_stream->base;
#endif
        if(ma_sound_init_from_data_source(&engine,source,flags,nullptr,&v.sound)!=MA_SUCCESS)return false;v.attached=true;
#if PORT_TITLE == 14
        if(v.use_balance){
            if(!v.balance.initialize(engine)||ma_node_attach_output_bus(&v.sound,0,&v.balance.base,0)!=MA_SUCCESS)return false;
        }
#endif
        return true;
    }
#if PORT_TITLE == 14
    bool seek_music(Voice& v,ma_uint64 frame,bool subframe=false){
        // Retail481ee0 recreates its streaming buffer. Recreate the mixer node
        // as well: ma_sound_seek alone retains already decoded cache frames.
        if(v.attached){ma_sound_stop(&v.sound);ma_sound_uninit(&v.sound);v.attached=false;}
        if(subframe)v.music_stream->begin_subframe();
        else if(ma_data_source_seek_to_pcm_frame(&v.music_stream->base,frame)!=MA_SUCCESS)return false;
        if(!attach(v))return false;ma_sound_set_looping(&v.sound,MA_TRUE);return true;
    }
#endif
    Voice* voice(u32 n){return n&&n<=sounds.size()?sounds[n-1].get():nullptr;}
};
AudioDevice::AudioDevice():impl(std::make_unique<Impl>()){}
AudioDevice::~AudioDevice()=default;
void AudioDevice::close(){effects.reset();impl=std::make_unique<Impl>();}
bool AudioDevice::initialize(GameResources& resources){
    if(impl->ready)return true;if(impl->engine_ready)impl=std::make_unique<Impl>();auto& a=*impl;auto engine=ma_engine_config_init();engine.noDevice=MA_TRUE;engine.channels=2;engine.sampleRate=44100;engine.defaultVolumeSmoothTimeInPCMFrames=0;
    if(ma_engine_init(&engine,&a.engine)!=MA_SUCCESS){error="Unable to initialize audio mixer";return false;}a.engine_ready=true;
    SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES,"2048");const SDL_AudioSpec spec{SDL_AUDIO_F32,2,44100};
    if(SDL_InitSubSystem(SDL_INIT_AUDIO))a.stream=SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,&spec,nullptr,nullptr);
    if(!a.stream){error=SDL_GetError();return false;}
    // The count is the title's, not the array's: TH12 loads 49 samples and TH13 loads 60, and a bound taken from
    // the buffer's capacity would either read past a shorter list or leave a longer one unloaded.
    for(u32 n=0;n<sound_sample_count;++n)if(!resources.read(sound_samples[n],a.samples[n])){error=std::string("Missing sound: ")+sound_samples[n];return false;}
    for(u32 n=0;n<a.sounds.size();++n){auto voice=std::make_unique<Impl::Voice>();const auto& bytes=a.samples[sound_definitions[n].sample];auto config=ma_decoder_config_init(ma_format_f32,2,44100);
        if(ma_decoder_init_memory(bytes.data(),bytes.size(),&config,&voice->decoder)!=MA_SUCCESS){error="Invalid TH11 WAV sample";return false;}voice->decoded=true;
#if PORT_TITLE == 14
        voice->use_balance=true;
#endif
        if(!a.attach(*voice)){error="Unable to attach sound voice";return false;}
#if PORT_TITLE == 14
        ma_sound_set_looping(&voice->sound,(sound_definitions[n].play_flags&1)!=0);
#endif
        effects.buffers[n]=n+1;a.sounds[n]=std::move(voice);
    }
    if(!resources.read("thbgm.fmt",a.formats)){error="Missing TH11 music layout";return false;}SDL_ResumeAudioStreamDevice(a.stream);a.ready=true;error.clear();return true;
}
#if PORT_TITLE == 14
bool AudioDevice::initialize_device(){
    if(impl->stream)return true;
    SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES,"2048");const SDL_AudioSpec spec{SDL_AUDIO_F32,2,44100};
    if(SDL_InitSubSystem(SDL_INIT_AUDIO))impl->stream=SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,&spec,nullptr,nullptr);
    if(!impl->stream){error=SDL_GetError();return false;}error.clear();return true;
}
bool AudioDevice::prepare_cpu(GameResources& resources,const std::function<bool()>& cancelled){
    // This private object has never owned an SDL stream. No live Main mixer,
    // queue, decoder, statistics or volume field is written by this worker.
    if(impl->stream||impl->engine_ready){error="Audio preparation owner already initialized";return false;}
    auto& a=*impl;auto engine=ma_engine_config_init();engine.noDevice=MA_TRUE;engine.channels=2;engine.sampleRate=44100;engine.defaultVolumeSmoothTimeInPCMFrames=0;
    if(ma_engine_init(&engine,&a.engine)!=MA_SUCCESS){error="Unable to initialize audio mixer";return false;}a.engine_ready=true;
    for(u32 n=0;n<sound_sample_count;++n){
        if(cancelled())return false;
        if(!resources.read(sound_samples[n],a.samples[n])){error=std::string("Missing sound: ")+sound_samples[n];return false;}
    }
    for(u32 n=0;n<a.sounds.size();++n){
        if(cancelled())return false;
        auto voice=std::make_unique<Impl::Voice>();const auto& bytes=a.samples[sound_definitions[n].sample];auto config=ma_decoder_config_init(ma_format_f32,2,44100);
        if(ma_decoder_init_memory(bytes.data(),bytes.size(),&config,&voice->decoder)!=MA_SUCCESS){error="Invalid TH11 WAV sample";return false;}voice->decoded=true;
#if PORT_TITLE == 14
        voice->use_balance=true;
#endif
        if(!a.attach(*voice)){error="Unable to attach sound voice";return false;}
        ma_sound_set_looping(&voice->sound,(sound_definitions[n].play_flags&1)!=0);
        effects.buffers[n]=n+1;a.sounds[n]=std::move(voice);
    }
    error.clear();return true;
}
void AudioDevice::prepare_formats(std::vector<u8> formats){impl->formats=std::move(formats);}
bool AudioDevice::adopt_prepared(AudioDevice& prepared){
    if(this==&prepared||!prepared.impl->engine_ready||prepared.impl->stream||prepared.impl->ready||prepared.impl->formats.empty()){
        error="Invalid prepared audio owner";return false;
    }
    if(!initialize_device())return false;
    // The heap-resident engine and its voice nodes keep their addresses.
    // Only Main's stream crosses into the completed private owner.
    auto& next=*prepared.impl;next.stream=impl->stream;impl->stream=nullptr;
    next.suspended=impl->suspended;next.suspend_started=impl->suspend_started;next.suspended_time=impl->suspended_time;
    impl.swap(prepared.impl);
    std::copy(std::begin(prepared.effects.buffers),std::end(prepared.effects.buffers),std::begin(effects.buffers));
    if(!impl->suspended)SDL_ResumeAudioStreamDevice(impl->stream);
    impl->ready=true;error.clear();return true;
}
#endif
#if PORT_TITLE == 14
Th14MusicQueue& AudioDevice::music_requests()noexcept{return impl->music_requests;}
bool AudioDevice::consume_initial_music_request(bool native_direct_sound){
    auto& a=*impl;a.native_direct_sound=native_direct_sound;if(!a.ready)return true; // 46e60b leaves the queue intact.
    using Result=Th14MusicQueue::Result;
    return a.music_requests.consume_one([&](const Th14MusicQueue::Request& request){
        ++a.initial_request_calls;
        if(request.command==3||request.command==4){
            if(!a.music)return Result::Done;
            if(!request.progress){ma_sound_stop(&a.music->sound);return Result::Advance;}
            // This backend has no Win32 music-thread handle. Preserve the
            // original no-handle stop phase; device streaming stays on Main.
            return Result::Done;
        }
        if(request.command!=1||request.argument>=2||request.progress){error="Unsupported initial music request";return Result::Failed;}
        const u32 slot=request.argument;
        // 46de40 retains an existing identical slot. With bit10 clear the
        // original stores its filename but does not read the PCM file.
        if(native_direct_sound){a.music.reset();a.track=-1;a.music_paused=false;a.music_fade={};}
        if(a.preloaded_music[slot]&&a.preloaded_names[slot]==request.file)return Result::Done;
        a.preloaded_names[slot]=request.file;++a.preload_calls;
        if(!native_direct_sound)return Result::Done;
        a.preloaded_music[slot].reset();
        const u8* format=nullptr;
        for(u32 n=0;n+52<=a.formats.size();n+=52)if(std::strncmp(reinterpret_cast<const char*>(a.formats.data()+n),request.file,16)==0){format=a.formats.data()+n;break;}
        if(!format){error="Initial music layout entry not found";return Result::Failed;}
        const u32 rate=word(format+36),align=short_word(format+44),channels=short_word(format+34),bits=short_word(format+46);
        const u32 bytes=word(format+20),total=word(format+28);
        if(!align||!rate||!channels||bits!=16||bytes%align||bytes!=total){error="Unsupported initial music PCM layout";return Result::Failed;}
        std::string stem=request.file;const auto dot=stem.find_last_of('.');if(dot!=std::string::npos)stem.erase(dot);
        const auto path=std::string("/music/")+stem+".flac";
        ma_decoder decoder{};const auto config=ma_decoder_config_init(ma_format_s16,channels,rate);
        if(ma_decoder_init_file(path.c_str(),&config,&decoder)!=MA_SUCCESS){error="Unable to preload "+path;return Result::Failed;}
        auto pcm=std::make_shared<Impl::MusicPcm>();pcm->filename=request.file;pcm->format=format;pcm->wave.resize(44+bytes);
        ma_uint64 length=0,read=0;const bool ok=ma_decoder_get_length_in_pcm_frames(&decoder,&length)==MA_SUCCESS&&length==total/align&&
            ma_decoder_read_pcm_frames(&decoder,pcm->wave.data()+44,bytes/align,&read)==MA_SUCCESS&&read==bytes/align;
        ma_decoder_uninit(&decoder);
        if(!ok){error="Initial music PCM length differs from original: "+path;return Result::Failed;}
        // A 44-byte WAV view lets the existing streaming/Continue decoder
        // read this exact s16 PCM without keeping a second payload copy.
        auto* w=pcm->wave.data();const auto put32=[&](u32 offset,u32 value){std::memcpy(w+offset,&value,4);};
        const auto put16=[&](u32 offset,u16 value){std::memcpy(w+offset,&value,2);};
        std::memcpy(w,"RIFF",4);put32(4,36+bytes);std::memcpy(w+8,"WAVEfmt ",8);put32(16,16);put16(20,1);put16(22,u16(channels));
        put32(24,rate);put32(28,rate*align);put16(32,u16(align));put16(34,u16(bits));std::memcpy(w+36,"data",4);put32(40,bytes);
        a.preloaded_music[slot]=std::move(pcm);return Result::Done;
    });
}
#endif
bool AudioDevice::music(i32 track){
    auto& a=*impl;if(!a.ready)return false;if(track<0){if(a.music)ma_sound_stop(&a.music->sound);a.track=-1;return true;}
    const char* stem=nullptr;for(const auto& t:music_tracks)if(t.cue==track){stem=t.file;break;}if(!stem){error="Unknown TH11 music cue";return false;}
    const auto filename=std::string(stem)+".wav";const u8* format=nullptr;
    for(u32 n=0;n+52<=a.formats.size();n+=52)if(std::strncmp(reinterpret_cast<const char*>(a.formats.data()+n),filename.c_str(),16)==0){format=a.formats.data()+n;break;}
    if(!format){error="Music layout entry not found";return false;}
#if PORT_TITLE == 14
    std::shared_ptr<Impl::MusicPcm> cached_music;
    if(a.native_direct_sound)for(u32 slot=0;slot<2;++slot)if(a.preloaded_music[slot]&&a.preloaded_names[slot]==filename){cached_music=a.preloaded_music[slot];format=cached_music->format;break;}
#endif

    if(a.track==track&&a.music){a.fade=0;a.music_paused=false;
#if PORT_TITLE == 14
        a.music_fade={};
#endif
#if PORT_TITLE == 14
        if(!a.seek_music(*a.music,0)){error="Unable to restart TH14 music";return false;}
#else
        ma_sound_seek_to_pcm_frame(&a.music->sound,0);
#endif
#if PORT_TITLE == 14
        a.music->time.started=a.clock();a.music->time.paused=0;a.music->time.pause_started=0;
#endif
        ma_sound_set_volume(&a.music->sound,music_enabled?volume(SoundEffects::adjusted_volume(0,music_volume,true)):0.f);ma_sound_start(&a.music->sound);return true;}
    auto next=std::make_unique<Impl::Voice>();
#if PORT_TITLE == 14
    const auto path=std::string("/music/")+stem+".flac";
#else
    const auto path=std::string("/music/")+stem+".ogg";
#endif
    const u32 rate=word(format+36),align=short_word(format+44),channels=short_word(format+34),total=word(format+28),intro=word(format+24);
    if(!align||!rate||!channels||intro>=total){error="Invalid music loop";return false;}
    auto config=ma_decoder_config_init(ma_format_f32,channels,rate);
#if PORT_TITLE == 14
    next->cached_pcm=std::move(cached_music);
    const auto decoded=next->cached_pcm?ma_decoder_init_memory(next->cached_pcm->wave.data(),next->cached_pcm->wave.size(),&config,&next->decoder):ma_decoder_init_file(path.c_str(),&config,&next->decoder);
    if(decoded!=MA_SUCCESS){error="Unable to decode "+path;return false;}next->decoded=true;
    if(next->cached_pcm)++a.preload_cache_hits;
#else
    if(ma_decoder_init_file(path.c_str(),&config,&next->decoder)!=MA_SUCCESS){error="Unable to decode "+path;return false;}next->decoded=true;
#endif
    ma_uint64 length=0;if(ma_decoder_get_length_in_pcm_frames(&next->decoder,&length)!=MA_SUCCESS||length!=total/align){error="Music PCM frame count differs from original: "+path;return false;}
    ma_data_source_set_loop_point_in_pcm_frames(&next->decoder,intro/align,total/align);
#if PORT_TITLE == 14
    next->music_stream=std::make_unique<Th14MusicStream>(next->decoder,a.formats,format);
    if(!next->music_stream->initialized){error="Unable to initialize TH14 music stream";return false;}
    next->time.rate=rate;next->time.total=total;next->time.intro=intro;
    next->time.channels=channels;next->time.bits=short_word(format+46);
#endif
    if(!a.attach(*next)){error="Unable to attach music voice";return false;}ma_sound_set_looping(&next->sound,MA_TRUE);ma_sound_set_volume(&next->sound,music_enabled?volume(SoundEffects::adjusted_volume(0,music_volume,true)):0.f);
    a.music=std::move(next);a.track=track;a.fade=0;a.music_paused=false;
#if PORT_TITLE == 14
    a.music_fade={};
#endif
#if PORT_TITLE == 14
    a.music->time.started=a.clock();
#endif
    ma_sound_start(&a.music->sound);return true;
}
i32 AudioDevice::current_music()const noexcept{return impl->track;}
bool AudioDevice::music_file(const char* name){std::string file=name?name:"";const auto slash=file.find_last_of("/\\");if(slash!=std::string::npos)file.erase(0,slash+1);const auto dot=file.find_last_of('.');if(dot!=std::string::npos)file.erase(dot);for(const auto& track:music_tracks)if(file==track.file)return music(track.cue);error="Unknown music filename: "+file;return false;}
void AudioDevice::fade_music(i32 frames){auto& a=*impl;
#if PORT_TITLE == 14
    a.music_fade.begin(std::max(0,frames));
#else
    a.fade=a.fade_total=std::max(0,frames);if(!frames&&a.music)ma_sound_stop(&a.music->sound);
#endif
}
#if PORT_TITLE == 14
void AudioDevice::pause_sounds(bool paused){auto& a=*impl;if(a.sounds_paused==paused)return;a.sounds_paused=paused;
    if(paused){
        // Retail4373b0 clears only the queue sentinel, preserving its metadata.
        effects.state.indices[0]=-1;
        for(u32 n=0;n<sound_voice_count;++n){auto* voice=a.sounds[n].get();
            a.paused_sounds[n]=voice&&ma_sound_is_playing(&voice->sound);
            if(voice)ma_sound_stop(&voice->sound);
        }
    }else{
        // Retail449020 resumes saved playing voices without rewinding them.
        // Menu sounds can reuse a voice while paused; its current cursor wins.
        for(u32 n=0;n<sound_voice_count;++n)if(a.paused_sounds[n]&&a.sounds[n])ma_sound_start(&a.sounds[n]->sound);
        a.paused_sounds.fill(false);
    }
}
void AudioDevice::discard_paused_sounds(){impl->paused_sounds.fill(false);impl->sounds_paused=false;}
#endif
void AudioDevice::pause_music(bool paused){auto& a=*impl;if(!a.music||a.music_paused==paused)return;a.music_paused=paused;
#if PORT_TITLE == 14
    if(paused)a.music->time.pause_started=a.clock();
    else a.music->time.paused+=a.clock()-a.music->time.pause_started;
#endif
    if(paused)ma_sound_stop(&a.music->sound);else ma_sound_start(&a.music->sound);}
bool AudioDevice::save_music(){
    auto& a=*impl;discard_saved_music();if(!a.music||a.track<0)return false;
#if PORT_TITLE == 14
    // Retail449e98 obtains wrapped clock time, independently of decoder read-ahead.
    a.saved_position=a.music->time.position(a.clock());
#endif
    ma_sound_stop(&a.music->sound);a.saved_music=std::move(a.music);a.saved_track=a.track;
    a.track=-1;a.fade=a.fade_total=0;a.music_paused=false;
#if PORT_TITLE == 14
    a.music_fade={};
#endif
    return true;
}
bool AudioDevice::restore_music(){
    auto& a=*impl;if(!a.saved_music)return false;
#if PORT_TITLE == 14
    // Retail481f3c..481f55 truncates sample time and subtracts one PCM frame.
    const double sample_time=a.saved_position*double(a.saved_music->time.rate);
    if(sample_time>=1&&sample_time<2147483648.){
        const u64 frame=u64(i32(sample_time)-1);
        if(!a.seek_music(*a.saved_music,frame)){error="Unable to restore TH14 music time";return false;}
    }
    else if(sample_time>=0&&sample_time<1&&a.saved_music->music_stream->restore_supported()){
        if(!a.seek_music(*a.saved_music,0,true)){error="Unable to restore TH14 subframe music";return false;}
    }
#endif
    if(a.music)ma_sound_stop(&a.music->sound);a.music=std::move(a.saved_music);a.track=a.saved_track;a.saved_track=-1;
    a.fade=a.fade_total=0;a.music_paused=false;
#if PORT_TITLE == 14
    a.music_fade={};
#endif
#if PORT_TITLE == 14
    a.music->time.started=a.clock()-a.saved_position;
    a.music->time.paused=0;a.music->time.pause_started=0;
#endif
    refresh_volume();
    return ma_sound_start(&a.music->sound)==MA_SUCCESS;
}
void AudioDevice::discard_saved_music(){impl->saved_music.reset();impl->saved_track=-1;}
void AudioDevice::refresh_volume(){auto& a=*impl;
#if PORT_TITLE == 14
    const i32 base=a.music_fade.volume;
#else
    const i32 base=a.fade?a.fade*5000/a.fade_total-5000:0;
#endif
    if(a.music)ma_sound_set_volume(&a.music->sound,music_enabled?volume(SoundEffects::adjusted_volume(base,music_volume,true)):0.f);
}
void AudioDevice::update(){auto& a=*impl;effects.process();
#if PORT_TITLE == 14
    if(a.music&&!a.music_paused){const u32 action=a.music_fade.tick();if(action==2)ma_sound_stop(&a.music->sound);else if(action==1)refresh_volume();}
#else
    if(a.music&&a.fade&&!a.music_paused){--a.fade;if(!a.fade)ma_sound_stop(&a.music->sound);}refresh_volume();
#endif
}
bool AudioDevice::mix(float* pcm,u32 frames){
    ma_uint64 read=0;if(!impl->ready||!pcm||ma_engine_read_pcm_frames(&impl->engine,pcm,frames,&read)!=MA_SUCCESS||read!=frames)return false;
    for(u32 n=0;n<frames*2;++n)pcm[n]=std::clamp(pcm[n],-1.f,1.f);return true;
}
void AudioDevice::pump(){
    auto& a=*impl;if(!a.ready||!a.stream||a.suspended)return;i32 queued=std::max(0,SDL_GetAudioStreamQueued(a.stream))/8;
    if(!a.refill&&queued<4096)a.refill=true;if(!a.refill)return;if(queued>=6144){a.refill=false;return;}
    for(i32 n=0;n<6&&queued<6144;++n){float pcm[2048]{};if(!mix(pcm,1024)){a.stats[3]=1;return;}if(!SDL_PutAudioStreamData(a.stream,pcm,sizeof(pcm))){a.stats[3]=2;return;}++a.stats[1];a.stats[2]+=1024;queued+=1024;}if(queued>=6144)a.refill=false;
}
void AudioDevice::suspend(bool paused){auto& a=*impl;
#if PORT_TITLE == 14
    if(a.suspended!=paused){const double now=double(SDL_GetTicksNS())/1000000000.;if(paused)a.suspend_started=now;else a.suspended_time+=now-a.suspend_started;}
#endif
    a.suspended=paused;if(a.stream){if(paused)SDL_PauseAudioStreamDevice(a.stream);else SDL_ResumeAudioStreamDevice(a.stream);}}
void AudioDevice::sound_stop(u32 n){if(auto* v=impl->voice(n))ma_sound_stop(&v->sound);}
void AudioDevice::sound_position(u32 n,u32 frame){if(auto* v=impl->voice(n))ma_sound_seek_to_pcm_frame(&v->sound,frame);}
void AudioDevice::sound_pan(u32 n,i32 value){if(auto* v=impl->voice(n)){
#if PORT_TITLE == 14
    v->balance.pan(value);
#else
    ma_sound_set_pan_mode(&v->sound,ma_pan_mode_balance);ma_sound_set_pan(&v->sound,value>=0?1.f-std::pow(10.f,-float(value)/2000.f):std::pow(10.f,float(value)/2000.f)-1.f);
#endif
}}
void AudioDevice::sound_volume(u32 n,i32 value){
#if PORT_TITLE == 14
    if(value<-10000||value>0)return;
#endif
    if(auto* v=impl->voice(n))ma_sound_set_volume(&v->sound,volume(value));
}
void AudioDevice::sound_play(u32 n){if(auto* v=impl->voice(n))ma_sound_start(&v->sound);}
const u32* AudioDevice::statistics()const{auto& a=*impl;a.stats[0]=a.ready;a.stats[4]=a.stream?std::max(0,SDL_GetAudioStreamQueued(a.stream))/8:0;a.stats[5]=u32(a.track);a.stats[6]=
#if PORT_TITLE == 14
    u32(std::max(0,a.music_fade.remaining));
#else
    a.fade;
#endif
    a.stats[7]=a.music?u32(std::round(ma_sound_get_volume(&a.music->sound)*1000000.f)):0;a.stats[8]=a.music_paused;ma_uint64 cursor=0,saved=0;if(a.music)ma_sound_get_cursor_in_pcm_frames(&a.music->sound,&cursor);if(a.saved_music)ma_sound_get_cursor_in_pcm_frames(&a.saved_music->sound,&saved);a.stats[9]=u32(cursor);a.stats[10]=u32(cursor>>32);a.stats[11]=u32(a.saved_track);a.stats[12]=u32(saved);a.stats[13]=u32(saved>>32);return a.stats;}
}
