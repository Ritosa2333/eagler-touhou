#include "GameAudioManager.hpp"
#include "BrowserRuntime.hpp"
#include "PlatformDevices.hpp"
namespace th08 {
#ifdef TH_SDL3
extern "C" u32 sdl_music_source_mode();
#endif
GameAudioManager::GameAudioManager(BrowserRuntime& runtime):host(runtime){control=std::make_unique<MusicControl>(host.app.session.display_config,host.app.session.statistics,*this);}
std::vector<u8> GameAudioManager::read(const char* p){return host.read(p);}
u32 GameAudioManager::milliseconds(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // MIDI start/reset and audio_tick must use the same non-rewindable clock.
    // BrowserRuntime::milliseconds is the admitted simulation clock in MP.
    return file_device().milliseconds();
#else
    return host.milliseconds();
#endif
}
bool GameAudioManager::prepare_formats(){const auto fmt=read("thbgm.fmt");return formats.load(fmt.data(),fmt.size());}
bool GameAudioManager::prepare_samples(){
    const auto init_midi=read("init.mid");if(!midi.load(30,init_midi.data(),init_midi.size()))return false;
    for(u32 i=0;i<46;++i){const auto bytes=read(sound_samples[sound_definitions[i].sample]);PcmWave wave;if(!wave.load(bytes.data(),bytes.size()))return false;if(sound_device().create_pcm(i+1,wave.format,wave.samples.data(),wave.samples.size()))return false;effects.buffers[i]=i+1;}return true;
}
void GameAudioManager::shutdown(){stop_all();midi.stop();}
void GameAudioManager::audio_context(){
    control->game_flags=host.app.in_game()?host.app.game.globals.game_flags:0;control->frame_rate=host.app.animations.timing.rate;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // MusicControl still owns logical unlocks and command selection. Physical
    // queue settings instead come from the confirmed event's captured context.
    if(event_sink||draining_events)return;
#endif
    commands.context.wav=host.app.session.display_config.music==1;commands.context.preload=host.app.session.display_config.options&8192;
}
void GameAudioManager::sound(i32 i,i32 m,float x,bool p){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(capture(multiplayer::AudioEventKind::Sound,i,m,x,"",p))return;
#endif
    if(p)effects.positioned(i,x);else effects.enqueue(i,m);
}
bool GameAudioManager::play_music(i32 i,i32 s){audio_context();return control->play(i,s);}
void GameAudioManager::load_music(i32 i,const char* p){audio_context();control->load(i,p);}
void GameAudioManager::play_audio(const char* p,i32 s){audio_context();control->audio(p,s);}
void GameAudioManager::stop_audio(){audio_context();control->stop();}
void GameAudioManager::fade_music(float s){audio_context();control->fade(s);}
void GameAudioManager::menu_music(MenuMusic m,float s){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(event_sink&&!draining_events){
        audio_context();switch(m){
        case MenuMusic::Pause:command(6,0,"dummy");break;
        case MenuMusic::Resume:command(7,0,"dummy");break;
        case MenuMusic::Stop:stop_audio();break;
        case MenuMusic::FadeIn:capture(multiplayer::AudioEventKind::Fade,2,0,s);break;
        case MenuMusic::PartialFadeOut:capture(multiplayer::AudioEventKind::Fade,4,0,s);break;
        case MenuMusic::PartialFadeIn:capture(multiplayer::AudioEventKind::Fade,3,0,s);break;
        }return;
    }
#endif
    audio_context();switch(m){case MenuMusic::Pause:commands.enqueue(6,0,"dummy");break;case MenuMusic::Resume:commands.enqueue(7,0,"dummy");break;case MenuMusic::Stop:stop_audio();break;case MenuMusic::FadeIn:fades.fade(2,s);break;case MenuMusic::PartialFadeOut:fades.fade(4,s);break;case MenuMusic::PartialFadeIn:fades.fade(3,s);break;}
}
void GameAudioManager::midi_reset(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(capture(multiplayer::AudioEventKind::MidiReset))return;
#endif
    midi.stop();midi_failed|=!midi.parse(30)||!midi.play();midi_clock=milliseconds();
}
void GameAudioManager::start_bgm(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(capture(multiplayer::AudioEventKind::StartBgm))return;
#endif
    audio_context();stop_all();open_music(formats.get(0));
}
void GameAudioManager::process_sounds(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(capture(multiplayer::AudioEventKind::Process))return;
#endif
    audio_context();commands.process();effects.process();
}
void GameAudioManager::update_audio_fades(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(capture(multiplayer::AudioEventKind::TickFades))return;
#endif
    fades.update_all();
}
void GameAudioManager::apply_volume(const GameConfiguration& c){
#ifdef TH_SDL3
    // The Launcher selects the source independently of imported config files.
    // OGG uses the WAV-shaped owner; none mutes BGM without disabling SFX.
    host.app.session.display_config.music=sdl_music_source_mode();
#endif
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(capture(multiplayer::AudioEventKind::Volumes))return;
#endif
    effects.enabled=c.sounds==1;effects.master_volume=c.sound_volume;fades.master_volume=c.music_volume;commands.context.master_volume=c.music_volume;commands.enqueue(8,0,"dummy");audio_context();}
i32 GameAudioManager::stop(u32 b){return sound_device().stop(b);}
i32 GameAudioManager::position(u32 b,u32 p){return sound_device().position(b,p);}
i32 GameAudioManager::pan(u32 b,i32 v){return sound_device().pan(b,v);}
i32 GameAudioManager::volume(u32 b,i32 v){return sound_device().volume(b,v);}
i32 GameAudioManager::play(u32 b,u32 p,u32 f){return sound_device().play(b,p,f);}
void GameAudioManager::command(i32 o,i32 a,const char* p){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(capture(multiplayer::AudioEventKind::WaveCommand,o,a,0,p))return;
#endif
    commands.enqueue(o,a,p);
}
void GameAudioManager::midi_load(i32 i,const char* p){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(capture(multiplayer::AudioEventKind::MidiLoad,i,0,0,p))return;
#endif
    const auto b=read(p);midi_failed|=!midi.load(i,b.data(),b.size());
}
void GameAudioManager::midi_stop(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(capture(multiplayer::AudioEventKind::MidiStop))return;
#endif
    midi.stop();
}
void GameAudioManager::midi_play(i32 i){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(capture(multiplayer::AudioEventKind::MidiPlay,i))return;
#endif
    midi_failed|=!midi.parse(i);
}
void GameAudioManager::midi_file(const char* p){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(capture(multiplayer::AudioEventKind::MidiFile,0,0,0,p))return;
#endif
    const auto b=read(p);midi_failed|=!midi.load_file(b.data(),b.size());
}
void GameAudioManager::midi_start(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(capture(multiplayer::AudioEventKind::MidiStart))return;
#endif
    midi_failed|=!midi.play();midi_clock=milliseconds();
}
void GameAudioManager::midi_fade(i32 t){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(capture(multiplayer::AudioEventKind::MidiFade,t))return;
#endif
    midi.fade(u32(t));
}
bool GameAudioManager::audio_tick(u32 now){if(!midi.active()){midi_clock=now;return !midi_failed;}if(now-midi_clock>10000)midi_clock=now;while(midi_clock!=now){++midi_clock;if(!midi.tick()){midi_failed=true;break;}}return !midi_failed;}
void GameAudioManager::open(){sound_device().midi_open();}
void GameAudioManager::close(){sound_device().midi_close(midi_clock);}
void GameAudioManager::short_message(u8 s,u8 a,u8 b){const u8 packet[]{s,a,b};sound_device().midi_message(packet,(s&0xf0)==0xc0||(s&0xf0)==0xd0?2:3,midi_clock);}
void GameAudioManager::long_message(const u8* p,u32 n){sound_device().midi_message(p,n,midi_clock);}
void GameAudioManager::set_volume(i32 v){fades.master_volume=v;fades.volume(0);}
void GameAudioManager::stop_all(){if(fades.buffer){fades.stop();sound_device().release(fades.buffer);}fades.buffer=0;commands.context.has_music=commands.context.has_thread=false;current_format=nullptr;}
bool GameAudioManager::open_music(const BgmFormat* f){if(!f)return false;current_format=f;fades.buffer=1000;fades.state={};commands.context.has_music=commands.context.has_thread=true;commands.context.total_length=f->total;music_loop=false;return sound_device().music_format(1000,*f,false)==0;}
i32 GameAudioManager::preload(i32 i,const char* p){if(u32(i)>=16||!p||std::strlen(p)>=256)return -1;std::strcpy(commands.filenames[i],p);return 0;}
i32 GameAudioManager::load(i32 i){if(u32(i)>=16||!commands.filenames[i][0])return -1;return open_music(formats.get(formats.find(commands.filenames[i])))?0:-1;}
i32 GameAudioManager::reset(){return fades.buffer?position(fades.buffer,0):-1;}
i32 GameAudioManager::fill(bool loop){if(!current_format)return -1;music_loop=loop;return sound_device().music_format(1000,*current_format,loop);}
void GameAudioManager::play(){fades.play(0,1);}
void GameAudioManager::stop(){fades.stop();}
void GameAudioManager::recreate_buffers(){if(fades.buffer)sound_device().release(fades.buffer);}
void GameAudioManager::reopen(const char* p){open_music(formats.get(formats.find(p)));}
void GameAudioManager::close_music(){stop_all();}
void GameAudioManager::fade_out(float s){fades.fade(1,s);}
void GameAudioManager::pause(){fades.pause();}
void GameAudioManager::unpause(){fades.unpause();}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
bool GameAudioManager::bind_audio_events(multiplayer::AudioEvents* value){
    if(draining_events||(event_sink&&value&&event_sink!=value))return false;
    event_sink=value;return true;
}
bool GameAudioManager::capture(multiplayer::AudioEventKind kind,i32 first,i32 second,float value,const char* path,bool flag){
    if(!event_sink||draining_events)return false;
    multiplayer::AudioEvent event;event.kind=kind;event.first=first;event.second=second;event.value=value;event.flag=flag;
    const auto& config=host.app.session.display_config;
    event.settings={config.music==1,bool(config.options&8192),config.sounds==1,config.sound_volume,config.music_volume};
    if(path){u32 size=0;while(size<sizeof(event.text)&&path[size])++size;std::memcpy(event.text,path,size);}
    else std::memset(event.text,1,sizeof(event.text));
    event_sink->Append(event);return true; // Never bypass a failed queue to device output.
}
bool GameAudioManager::commit_audio_events(multiplayer::AudioEvents& events,u32 confirmed,u32 simulated){
    return event_sink==&events&&!draining_events&&events.CommitThrough(confirmed,simulated,*this);
}
bool GameAudioManager::apply_audio_event(const multiplayer::AudioEvent& event){
    if(draining_events)return false;draining_events=true;
    commands.context.wav=event.settings.wav;commands.context.preload=event.settings.preload;
    commands.context.master_volume=fades.master_volume=event.settings.music_volume;
    effects.enabled=event.settings.sounds;effects.master_volume=event.settings.sound_volume;
    using Kind=multiplayer::AudioEventKind;
    switch(event.kind){
    case Kind::Sound:sound(event.first,event.second,event.value,event.flag);break;
    case Kind::WaveCommand:command(event.first,event.second,event.text);break;
    case Kind::MidiLoad:midi_load(event.first,event.text);break;
    case Kind::MidiStop:midi_stop();break;
    case Kind::MidiPlay:midi_play(event.first);break;
    case Kind::MidiFile:midi_file(event.text);break;
    case Kind::MidiStart:midi_start();break;
    case Kind::MidiFade:midi_fade(event.first);break;
    case Kind::MidiReset:midi_reset();break;
    case Kind::StartBgm:start_bgm();break;
    case Kind::Fade:fades.fade(event.first,event.value);break;
    case Kind::Process:process_sounds();break;
    case Kind::TickFades:update_audio_fades();break;
    case Kind::Volumes:commands.enqueue(8,0,"dummy");break;
    }
    draining_events=false;return !midi_failed;
}
#endif
#ifdef TH_MULTIPLAYER_FIXTURES
u32 GameAudioManager::diagnostic_device_fingerprint()const{
    u32 hash=2166136261u;
    const auto part=[&](const auto& value){const auto* bytes=reinterpret_cast<const u8*>(&value);
        for(std::size_t i=0;i<sizeof(value);++i){hash^=bytes[i];hash*=16777619u;}};
    part(effects.state);part(effects.enabled);part(effects.master_volume);
    part(commands.commands);part(commands.filenames);part(commands.context);
    part(fades.state);part(fades.buffer);part(fades.master_volume);part(current_format);
    part(music_loop);part(midi_clock);part(midi_failed);part(midi.file_index);
    part(midi.elapsed);part(midi.base_ticks);part(midi.channels);part(midi.fading);
    const bool active=midi.active(),invalid=midi.invalid();part(active);part(invalid);return hash;
}
#endif
}
