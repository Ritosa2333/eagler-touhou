#include "GameAudioManager.hpp"
#include "BrowserRuntime.hpp"
#include "PlatformDevices.hpp"
#include "../sdl/AudioAudit.hpp"
namespace th08 {
namespace {GameAudioManager* observed_audio_manager=nullptr;}
#ifdef TH_SDL3
extern "C" u32 sdl_music_source_mode();
#endif
GameAudioManager::GameAudioManager(BrowserRuntime& runtime):host(runtime){observed_audio_manager=this;control=std::make_unique<MusicControl>(host.app.session.display_config,host.app.session.statistics,*this);}
GameAudioManager::~GameAudioManager(){if(observed_audio_manager==this)observed_audio_manager=nullptr;}
std::vector<u8> GameAudioManager::read(const char* p){return host.read(p);}
u32 GameAudioManager::milliseconds(){return host.milliseconds();}
bool GameAudioManager::prepare_formats(){audit::audio_operation(audit::AudioOperation::ManagerOperation);const auto fmt=read("thbgm.fmt");return formats.load(fmt.data(),fmt.size());}
bool GameAudioManager::prepare_samples(){audit::audio_operation(audit::AudioOperation::ManagerOperation);
    const auto init_midi=read("init.mid");if(!midi.load(30,init_midi.data(),init_midi.size()))return false;std::strncpy(midi_paths[30].data(),"init.mid",midi_paths[30].size()-1);
    for(u32 i=0;i<46;++i){const auto bytes=read(sound_samples[sound_definitions[i].sample]);PcmWave wave;if(!wave.load(bytes.data(),bytes.size()))return false;if(sound_device().create_pcm(i+1,wave.format,wave.samples.data(),wave.samples.size()))return false;effects.buffers[i]=i+1;}return true;
}
void GameAudioManager::shutdown(){audit::audio_operation(audit::AudioOperation::ManagerOperation);stop_all();midi.stop();}
void GameAudioManager::audio_context(){audit::audio_operation(audit::AudioOperation::ManagerOperation);control->game_flags=host.app.in_game()?host.app.game.globals.game_flags:0;control->frame_rate=host.app.animations.timing.rate;commands.context.wav=host.app.session.display_config.music==1;commands.context.preload=host.app.session.display_config.options&8192;}
void GameAudioManager::sound(i32 i,i32 m,float x,bool p){audit::audio_operation(audit::AudioOperation::ManagerOperation);
    if(p)effects.positioned(i,x);else effects.enqueue(i,m);
}
bool GameAudioManager::play_music(i32 i,i32 s){audit::audio_operation(audit::AudioOperation::ManagerOperation);last_music_song=s;audio_context();return control->play(i,s);}
void GameAudioManager::load_music(i32 i,const char* p){audit::audio_operation(audit::AudioOperation::ManagerOperation);audio_context();control->load(i,p);}
void GameAudioManager::play_audio(const char* p,i32 s){audit::audio_operation(audit::AudioOperation::ManagerOperation);audio_context();control->audio(p,s);}
void GameAudioManager::stop_audio(){audit::audio_operation(audit::AudioOperation::ManagerOperation);audio_context();control->stop();}
void GameAudioManager::fade_music(float s){audit::audio_operation(audit::AudioOperation::ManagerOperation);audio_context();control->fade(s);}
void GameAudioManager::menu_music(MenuMusic m,float s){audit::audio_operation(audit::AudioOperation::ManagerOperation);audio_context();switch(m){case MenuMusic::Pause:commands.enqueue(6,0,"dummy");break;case MenuMusic::Resume:commands.enqueue(7,0,"dummy");break;case MenuMusic::Stop:stop_audio();break;case MenuMusic::FadeIn:fades.fade(2,s);break;case MenuMusic::PartialFadeOut:fades.fade(4,s);break;case MenuMusic::PartialFadeIn:fades.fade(3,s);break;}}
void GameAudioManager::midi_reset(){audit::audio_operation(audit::AudioOperation::ManagerOperation);midi.stop();std::memcpy(midi_current_path.data(),midi_paths[30].data(),midi_current_path.size());midi_failed|=!midi.parse(30)||!midi.play();midi_clock=milliseconds();}
void GameAudioManager::start_bgm(){audit::audio_operation(audit::AudioOperation::ManagerOperation);audio_context();stop_all();open_music(formats.get(0));}
void GameAudioManager::process_sounds(){audit::audio_operation(audit::AudioOperation::ManagerOperation);audio_context();commands.process();effects.process();}
void GameAudioManager::update_audio_fades(){audit::audio_operation(audit::AudioOperation::ManagerOperation);fades.update_all();}
void GameAudioManager::apply_volume(const GameConfiguration& c){audit::audio_operation(audit::AudioOperation::ManagerOperation);
#ifdef TH_SDL3
    // The Launcher selects the source independently of imported config files.
    // OGG uses the WAV-shaped owner; none mutes BGM without disabling SFX.
    host.app.session.display_config.music=sdl_music_source_mode();
#endif
    effects.enabled=c.sounds==1;effects.master_volume=c.sound_volume;fades.master_volume=c.music_volume;commands.context.master_volume=c.music_volume;commands.enqueue(8,0,"dummy");audio_context();}
i32 GameAudioManager::stop(u32 b){audit::audio_operation(audit::AudioOperation::ManagerOperation);return sound_device().stop(b);}
i32 GameAudioManager::position(u32 b,u32 p){audit::audio_operation(audit::AudioOperation::ManagerOperation);return sound_device().position(b,p);}
i32 GameAudioManager::pan(u32 b,i32 v){audit::audio_operation(audit::AudioOperation::ManagerOperation);return sound_device().pan(b,v);}
i32 GameAudioManager::volume(u32 b,i32 v){audit::audio_operation(audit::AudioOperation::ManagerOperation);return sound_device().volume(b,v);}
i32 GameAudioManager::play(u32 b,u32 p,u32 f){audit::audio_operation(audit::AudioOperation::ManagerOperation);return sound_device().play(b,p,f);}
void GameAudioManager::command(i32 o,i32 a,const char* p){audit::audio_operation(audit::AudioOperation::ManagerOperation);commands.enqueue(o,a,p);}
void GameAudioManager::midi_load(i32 i,const char* p){audit::audio_operation(audit::AudioOperation::ManagerOperation);const auto b=read(p);midi_failed|=!midi.load(i,b.data(),b.size());if(u32(i)<midi_paths.size()){auto& path=midi_paths[u32(i)];path.fill(0);if(p)std::strncpy(path.data(),p,path.size()-1);}}
void GameAudioManager::midi_stop(){audit::audio_operation(audit::AudioOperation::ManagerOperation);midi.stop();}
void GameAudioManager::midi_play(i32 i){audit::audio_operation(audit::AudioOperation::ManagerOperation);if(u32(i)<midi_paths.size())std::memcpy(midi_current_path.data(),midi_paths[u32(i)].data(),midi_current_path.size());midi_failed|=!midi.parse(i);}
void GameAudioManager::midi_file(const char* p){audit::audio_operation(audit::AudioOperation::ManagerOperation);const auto b=read(p);midi_failed|=!midi.load_file(b.data(),b.size());midi_current_path.fill(0);if(p)std::strncpy(midi_current_path.data(),p,midi_current_path.size()-1);}
void GameAudioManager::midi_start(){audit::audio_operation(audit::AudioOperation::ManagerOperation);midi_failed|=!midi.play();midi_clock=milliseconds();}
void GameAudioManager::midi_fade(i32 t){audit::audio_operation(audit::AudioOperation::ManagerOperation);midi.fade(u32(t));}
bool GameAudioManager::audio_tick(u32 now){audit::audio_operation(audit::AudioOperation::ManagerOperation);if(!midi.active()){midi_clock=now;return !midi_failed;}if(now-midi_clock>10000)midi_clock=now;while(midi_clock!=now){++midi_clock;if(!midi.tick()){midi_failed=true;break;}}return !midi_failed;}
void GameAudioManager::open(){audit::audio_operation(audit::AudioOperation::ManagerOperation);++midi_opens;sound_device().midi_open();}
void GameAudioManager::close(){audit::audio_operation(audit::AudioOperation::ManagerOperation);++midi_closes;sound_device().midi_close(midi_clock);}
void GameAudioManager::short_message(u8 s,u8 a,u8 b){audit::audio_operation(audit::AudioOperation::ManagerOperation);++midi_messages;const u8 kind=s&0xf0;if(kind==0x90&&b)++midi_note_ons;else if(kind==0x80||(kind==0x90&&!b))++midi_note_offs;else if(kind==0xb0)++midi_controllers;const u8 packet[]{s,a,b};sound_device().midi_message(packet,kind==0xc0||kind==0xd0?2:3,midi_clock);}
void GameAudioManager::long_message(const u8* p,u32 n){audit::audio_operation(audit::AudioOperation::ManagerOperation);++midi_sysex;sound_device().midi_message(p,n,midi_clock);}
void GameAudioManager::set_volume(i32 v){audit::audio_operation(audit::AudioOperation::ManagerOperation);fades.master_volume=v;fades.volume(0);}
void GameAudioManager::stop_all(){audit::audio_operation(audit::AudioOperation::ManagerOperation);if(fades.buffer){fades.stop();sound_device().release(fades.buffer);}fades.buffer=0;commands.context.has_music=commands.context.has_thread=false;current_format=nullptr;}
bool GameAudioManager::open_music(const BgmFormat* f){audit::audio_operation(audit::AudioOperation::ManagerOperation);if(!f)return false;current_format=f;fades.buffer=1000;fades.state={};commands.context.has_music=commands.context.has_thread=true;commands.context.total_length=f->total;music_loop=false;return sound_device().music_format(1000,*f,false)==0;}
i32 GameAudioManager::preload(i32 i,const char* p){audit::audio_operation(audit::AudioOperation::ManagerOperation);if(u32(i)>=16||!p||std::strlen(p)>=256)return -1;std::strcpy(commands.filenames[i],p);return 0;}
i32 GameAudioManager::load(i32 i){audit::audio_operation(audit::AudioOperation::ManagerOperation);if(u32(i)>=16||!commands.filenames[i][0])return -1;return open_music(formats.get(formats.find(commands.filenames[i])))?0:-1;}
i32 GameAudioManager::reset(){audit::audio_operation(audit::AudioOperation::ManagerOperation);return fades.buffer?position(fades.buffer,0):-1;}
i32 GameAudioManager::fill(bool loop){audit::audio_operation(audit::AudioOperation::ManagerOperation);if(!current_format)return -1;music_loop=loop;return sound_device().music_format(1000,*current_format,loop);}
void GameAudioManager::play(){audit::audio_operation(audit::AudioOperation::ManagerOperation);fades.play(0,1);}
void GameAudioManager::stop(){audit::audio_operation(audit::AudioOperation::ManagerOperation);fades.stop();}
void GameAudioManager::recreate_buffers(){audit::audio_operation(audit::AudioOperation::ManagerOperation);if(fades.buffer)sound_device().release(fades.buffer);}
void GameAudioManager::reopen(const char* p){audit::audio_operation(audit::AudioOperation::ManagerOperation);open_music(formats.get(formats.find(p)));}
void GameAudioManager::close_music(){audit::audio_operation(audit::AudioOperation::ManagerOperation);stop_all();}
void GameAudioManager::fade_out(float s){audit::audio_operation(audit::AudioOperation::ManagerOperation);fades.fade(1,s);}
void GameAudioManager::pause(){audit::audio_operation(audit::AudioOperation::ManagerOperation);fades.pause();}
void GameAudioManager::unpause(){audit::audio_operation(audit::AudioOperation::ManagerOperation);fades.unpause();}
const u32* GameAudioManager::midi_observation()const{
    static u32 out[22]{};std::fill(out,out+22,0);out[0]=1;out[1]=midi.active();out[2]=midi.invalid();out[3]=midi.file_index<0?0xffffffffu:u32(midi.file_index);
    out[4]=u32(midi.tempo);out[5]=u32(midi.divisions);out[6]=u32(midi.tracks.size());for(const auto& track:midi.tracks)out[7]+=track.playing!=0;
    out[8]=u32(midi.elapsed);out[9]=u32(midi.elapsed>>32);out[10]=u32(midi.fading);out[11]=u32(midi.fade_elapsed);out[12]=u32(midi.fade_interval);
    out[13]=midi_messages;out[14]=midi_note_ons;out[15]=midi_note_offs;out[16]=midi_controllers;out[17]=midi_sysex;out[18]=midi_opens;out[19]=midi_closes;out[20]=midi_clock;out[21]=u32(last_music_song);return out;
}
#ifdef __EMSCRIPTEN__
extern "C" __attribute__((export_name("sdl_midi_observation"))) const u32* sdl_midi_observation(){static const u32 unavailable[22]={1};return observed_audio_manager?observed_audio_manager->midi_observation():unavailable;}
extern "C" __attribute__((export_name("sdl_midi_path"))) const char* sdl_midi_path(){static const char empty[]="";return observed_audio_manager?observed_audio_manager->midi_path():empty;}
#endif
}
