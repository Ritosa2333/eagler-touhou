#include "GameAudioManager.hpp"
#include "BrowserRuntime.hpp"
#include "PlatformDevices.hpp"
namespace th08 {
#ifdef TH_SDL3
extern "C" u32 sdl_music_source_mode();
#endif
GameAudioManager::GameAudioManager(BrowserRuntime& runtime):host(runtime){control=std::make_unique<MusicControl>(host.app.session.display_config,host.app.session.statistics,*this);}
std::vector<u8> GameAudioManager::read(const char* p){return host.read(p);}
u32 GameAudioManager::milliseconds(){return host.milliseconds();}
bool GameAudioManager::prepare_formats(){const auto fmt=read("thbgm.fmt");return formats.load(fmt.data(),fmt.size());}
bool GameAudioManager::prepare_samples(){
    const auto init_midi=read("init.mid");if(!midi.load(30,init_midi.data(),init_midi.size()))return false;
    for(u32 i=0;i<46;++i){const auto bytes=read(sound_samples[sound_definitions[i].sample]);PcmWave wave;if(!wave.load(bytes.data(),bytes.size()))return false;if(sound_device().create_pcm(i+1,wave.format,wave.samples.data(),wave.samples.size()))return false;effects.buffers[i]=i+1;}return true;
}
void GameAudioManager::shutdown(){stop_all();midi.stop();}
void GameAudioManager::audio_context(){control->game_flags=host.app.in_game()?host.app.game.globals.game_flags:0;control->frame_rate=host.app.animations.timing.rate;commands.context.wav=host.app.session.display_config.music==1;commands.context.preload=host.app.session.display_config.options&8192;}
void GameAudioManager::sound(i32 i,i32 m,float x,bool p){if(p)effects.positioned(i,x);else effects.enqueue(i,m);}
bool GameAudioManager::play_music(i32 i,i32 s){audio_context();return control->play(i,s);}
void GameAudioManager::load_music(i32 i,const char* p){audio_context();control->load(i,p);}
void GameAudioManager::play_audio(const char* p,i32 s){audio_context();control->audio(p,s);}
void GameAudioManager::stop_audio(){audio_context();control->stop();}
void GameAudioManager::fade_music(float s){audio_context();control->fade(s);}
void GameAudioManager::menu_music(MenuMusic m,float s){audio_context();switch(m){case MenuMusic::Pause:commands.enqueue(6,0,"dummy");break;case MenuMusic::Resume:commands.enqueue(7,0,"dummy");break;case MenuMusic::Stop:stop_audio();break;case MenuMusic::FadeIn:fades.fade(2,s);break;case MenuMusic::PartialFadeOut:fades.fade(4,s);break;case MenuMusic::PartialFadeIn:fades.fade(3,s);break;}}
void GameAudioManager::midi_reset(){midi.stop();midi_failed|=!midi.parse(30)||!midi.play();midi_clock=milliseconds();}
void GameAudioManager::start_bgm(){audio_context();stop_all();open_music(formats.get(0));}
void GameAudioManager::process_sounds(){audio_context();commands.process();effects.process();}
void GameAudioManager::update_audio_fades(){fades.update_all();}
void GameAudioManager::apply_volume(const GameConfiguration& c){
#ifdef TH_SDL3
    // The Launcher selects the source independently of imported config files.
    // OGG uses the WAV-shaped owner; none mutes BGM without disabling SFX.
    host.app.session.display_config.music=sdl_music_source_mode();
#endif
    effects.enabled=c.sounds==1;effects.master_volume=c.sound_volume;fades.master_volume=c.music_volume;commands.context.master_volume=c.music_volume;commands.enqueue(8,0,"dummy");audio_context();}
i32 GameAudioManager::stop(u32 b){return sound_device().stop(b);}
i32 GameAudioManager::position(u32 b,u32 p){return sound_device().position(b,p);}
i32 GameAudioManager::pan(u32 b,i32 v){return sound_device().pan(b,v);}
i32 GameAudioManager::volume(u32 b,i32 v){return sound_device().volume(b,v);}
i32 GameAudioManager::play(u32 b,u32 p,u32 f){return sound_device().play(b,p,f);}
void GameAudioManager::command(i32 o,i32 a,const char* p){commands.enqueue(o,a,p);}
void GameAudioManager::midi_load(i32 i,const char* p){const auto b=read(p);midi_failed|=!midi.load(i,b.data(),b.size());}
void GameAudioManager::midi_stop(){midi.stop();}
void GameAudioManager::midi_play(i32 i){midi_failed|=!midi.parse(i);}
void GameAudioManager::midi_file(const char* p){const auto b=read(p);midi_failed|=!midi.load_file(b.data(),b.size());}
void GameAudioManager::midi_start(){midi_failed|=!midi.play();midi_clock=milliseconds();}
void GameAudioManager::midi_fade(i32 t){midi.fade(u32(t));}
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
}
