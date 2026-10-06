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
#include <cmath>
#include <algorithm>
namespace th15::sdl {
namespace {float amplitude(i32 db){return std::pow(10.f,float(db)/2000.f);}std::string wave_name(const std::string& value){return value.size()>=4&&value.compare(value.size()-4,4,".wav")==0?value:value+".wav";}}
struct AudioDevice::Impl final:MusicCommandOutput {
 AudioDevice& owner;explicit Impl(AudioDevice& o):owner(o),commands(layout,*this){}
 struct Voice {ma_decoder decoder{};ma_sound sound{};bool decoded=false,attached=false;~Voice(){if(attached)ma_sound_uninit(&sound);if(decoded)ma_decoder_uninit(&decoder);}};
 ma_engine engine{};SDL_AudioStream* stream=nullptr;bool engine_ready=false,ready=false,suspended=false,refill=true,music_paused=false;
 std::array<std::unique_ptr<Voice>,77> sounds;std::array<std::vector<u8>,66> samples;std::vector<std::unique_ptr<Voice>> music_cache;MusicLayout layout;MusicCommands commands;i32 selected_track=0,track=-1,fade=0,fade_total=0;bool command_ready=true;mutable u32 stats[11]{};
 ~Impl(){music_cache.clear();for(auto& voice:sounds)voice.reset();if(stream)SDL_DestroyAudioStream(stream);if(engine_ready)ma_engine_uninit(&engine);}
 bool attach(Voice& v){if(ma_sound_init_from_data_source(&engine,&v.decoder,MA_SOUND_FLAG_NO_SPATIALIZATION,nullptr,&v.sound)!=MA_SUCCESS)return false;v.attached=true;return true;}
 Voice* voice(u32 id){return id<sounds.size()?sounds[id].get():nullptr;}Voice* current(){return track>=0&&u32(track)<music_cache.size()?music_cache[track].get():nullptr;}
 bool music_ready()const override{return owner.music_enabled&&ready&&command_ready;}bool music_busy()const override{return false;}bool music_has_intro()const override{auto* t=layout.track(u32(selected_track));return t&&t->loop_start!=0;}
 bool release_pending()const override{return false;}bool release_waiting()override{return false;}
 void preload_reset()override{owner.pause_music(true);}
 bool prepare_music(i32,const std::string& wave)override{return owner.prepare_music(wave);}
 bool cached_music(i32 slot)override{return slot>=0&&slot<i32(commands.prepared.size())&&owner.music_file(commands.prepared[slot]);}
 void stop_music_stream(bool)override{owner.stop_music();}
 void rewind_music_stream()override{if(auto* v=current())ma_sound_seek_to_pcm_frame(&v->sound,0);}
 bool load_music(const std::string&,i32 index)override{selected_track=index;auto* t=layout.track(u32(index));return t&&owner.prepare_music(t->filename);}
 void select_music(i32 index)override{selected_track=index;}
 i32 fill_music(bool,bool)override{return selected_track>=0&&u32(selected_track)<music_cache.size()&&music_cache[selected_track]?0:-1;}
 void start_music_stream()override{owner.music(selected_track);}
 void signal_music_release()override{}void close_music_stream()override{owner.stop_music();command_ready=false;}
 void fade_music_stream(i32 frames)override{owner.fade_music(frames);}void pause_music_stream(bool value)override{owner.pause_music(value);}
 void refresh_music_volume()override{if(auto* v=current())ma_sound_set_volume(&v->sound,owner.music_enabled?amplitude(adjusted_music_volume(0,owner.music_volume)):0.f);}
 void switch_music_wave(i32 index)override{if(!ready||!command_ready)return;std::string wave;double position=0;if(owner.current_music(wave,position)&&owner.music(index))owner.seek_music(position);}
};
AudioDevice::AudioDevice():impl(std::make_unique<Impl>(*this)){}AudioDevice::~AudioDevice()=default;
void AudioDevice::close(){effects.reset();impl=std::make_unique<Impl>(*this);error.clear();}
bool AudioDevice::initialize(AssetSource& resources,bool device){
 if(impl->ready)return true;if(impl->engine_ready){effects.reset();impl=std::make_unique<Impl>(*this);}auto& a=*impl;auto config=ma_engine_config_init();config.noDevice=MA_TRUE;config.channels=2;config.sampleRate=44100;config.defaultVolumeSmoothTimeInPCMFrames=0;
 if(ma_engine_init(&config,&a.engine)!=MA_SUCCESS){error="Unable to initialize audio mixer";return false;}a.engine_ready=true;
 if(device){SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES,"2048");const SDL_AudioSpec spec{SDL_AUDIO_F32,2,44100};if(SDL_InitSubSystem(SDL_INIT_AUDIO))a.stream=SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,&spec,nullptr,nullptr);if(!a.stream){error=SDL_GetError();return false;}}
 for(u32 n=0;n<a.samples.size();n++)if(!resources.read(sound_samples[n],a.samples[n])){error=std::string("Missing original sound: ")+sound_samples[n];return false;}
 for(u32 id=0;id<a.sounds.size();id++){auto next=std::make_unique<Impl::Voice>();const auto& bytes=a.samples[effects.voices[id].definition->sample];auto decode=ma_decoder_config_init(ma_format_f32,2,44100);if(ma_decoder_init_memory(bytes.data(),bytes.size(),&decode,&next->decoder)!=MA_SUCCESS){error="Invalid TH15 WAV sample";return false;}next->decoded=true;if(!a.attach(*next)){error="Unable to attach sound voice";return false;}a.sounds[id]=std::move(next);}
 std::vector<u8> format;if(!resources.read("thbgm.fmt",format)||!a.layout.open(format.data(),format.size())){error=a.layout.error.empty()?"Missing TH15 music layout":a.layout.error;return false;}a.music_cache.resize(a.layout.count());if(a.stream)SDL_ResumeAudioStreamDevice(a.stream);a.ready=true;error.clear();return true;
}
bool AudioDevice::prepare_music(const std::string& value){
 // No-music launches intentionally install no OGG files; SFX still use the mixer.
 if(!music_enabled)return true;
 auto& a=*impl;if(!a.ready){error="Audio device not initialized";return false;}const i32 index=a.layout.original_index(wave_name(value));if(a.music_cache[index])return true;const auto& track=*a.layout.track(index);auto next=std::make_unique<Impl::Voice>();std::string filename=track.filename;filename.resize(filename.size()-4);const auto path=music_directory+"/"+filename+".ogg";auto config=ma_decoder_config_init(ma_format_f32,track.channels,track.rate);
 if(ma_decoder_init_file(path.c_str(),&config,&next->decoder)!=MA_SUCCESS){error="Unable to decode "+path;return false;}next->decoded=true;ma_uint64 length=0;if(ma_decoder_get_length_in_pcm_frames(&next->decoder,&length)!=MA_SUCCESS||length!=track.frames()){error="Music PCM frame count differs from original: "+path;return false;}ma_data_source_set_loop_point_in_pcm_frames(&next->decoder,track.loop_frame(),track.frames());if(!a.attach(*next)){error="Unable to attach music voice";return false;}ma_sound_set_looping(&next->sound,MA_TRUE);a.music_cache[index]=std::move(next);return true;
}
bool AudioDevice::music_file(const std::string& value){if(!prepare_music(value))return false;return music(impl->layout.original_index(wave_name(value)));}
bool AudioDevice::music(i32 index){
 if(!music_enabled){stop_music();return true;}
 auto& a=*impl;if(!a.ready)return false;if(index<0){stop_music();return true;}const auto* track=a.layout.track(u32(index));if(!track){error="Music index outside TH15 table";return false;}if(!prepare_music(track->filename))return false;if(auto* previous=a.current())ma_sound_stop(&previous->sound);a.track=a.selected_track=index;a.fade=a.fade_total=0;a.music_paused=false;auto* voice=a.current();ma_sound_seek_to_pcm_frame(&voice->sound,0);refresh_volume();ma_sound_start(&voice->sound);return true;
}
void AudioDevice::stop_music(){auto& a=*impl;if(auto* voice=a.current()){ma_sound_stop(&voice->sound);ma_sound_seek_to_pcm_frame(&voice->sound,0);}a.track=-1;a.fade=a.fade_total=0;a.music_paused=false;}
void AudioDevice::fade_music(i32 frames){auto& a=*impl;a.fade=a.fade_total=std::max(0,frames);if(!frames)if(auto* voice=a.current())ma_sound_stop(&voice->sound);}
void AudioDevice::pause_music(bool value){auto& a=*impl;auto* voice=a.current();if(!voice||a.music_paused==value)return;a.music_paused=value;if(value)ma_sound_stop(&voice->sound);else ma_sound_start(&voice->sound);}
bool AudioDevice::seek_music(double seconds){if(!music_enabled)return true;auto& a=*impl;auto* voice=a.current();const auto* track=a.layout.track(u32(a.track));if(!voice||!track||!std::isfinite(seconds)||seconds<0||seconds>double(UINT64_MAX)/track->rate){error="Invalid music seek";return false;}u64 frame=u64(seconds*track->rate);if(frame>=track->frames())frame=track->loop_frame()+(frame-track->loop_frame())%(track->frames()-track->loop_frame());return ma_sound_seek_to_pcm_frame(&voice->sound,frame)==MA_SUCCESS;}
bool AudioDevice::current_music(std::string& wave,double& seconds){auto& a=*impl;auto* voice=a.current();const auto* track=a.layout.track(u32(a.track));if(!voice||!track){wave.clear();seconds=0;return true;}ma_uint64 cursor=0;if(ma_sound_get_cursor_in_pcm_frames(&voice->sound,&cursor)!=MA_SUCCESS)return false;wave=track->filename;seconds=double(cursor)/track->rate;return true;}
void AudioDevice::refresh_volume(){auto& a=*impl;if(auto* voice=a.current()){const i32 base=a.fade_total?i32(i64(a.fade)*5000/a.fade_total)-5000:0;ma_sound_set_volume(&voice->sound,music_enabled?amplitude(adjusted_music_volume(base,music_volume)):0.f);}}
bool AudioDevice::queue_music(i32 code,i32 value,const std::string& text){if(!impl->commands.enqueue(code,value,text)){error=impl->commands.error;return false;}return true;}
bool AudioDevice::queue_music_track(i32 slot,const std::string& stem){if(!impl->commands.preload_track(slot,stem)){error=impl->commands.error;return false;}return true;}
void AudioDevice::clear_current_wave(){impl->commands.current_wave.clear();}
const MusicCommands& AudioDevice::music_commands()const{return impl->commands;}
bool AudioDevice::finish_music_requests(){auto& a=*impl;a.commands.music_mode=music_enabled?1:0;for(u32 n=0;n<1024&&a.commands.pending();n++)if(!a.commands.process()){if(error.empty())error=a.commands.error;return false;}if(a.commands.pending()){error="Music commands did not complete";return false;}effects.process();return true;}
void AudioDevice::update(){auto& a=*impl;a.commands.music_mode=music_enabled?1:0;if(!a.commands.process()&&error.empty())error=a.commands.error;effects.process();if(a.fade&&!a.music_paused){if(--a.fade==0)if(auto* voice=a.current())ma_sound_stop(&voice->sound);}refresh_volume();}
bool AudioDevice::mix(float* pcm,u32 frames){ma_uint64 read=0;if(!impl->ready||!pcm||ma_engine_read_pcm_frames(&impl->engine,pcm,frames,&read)!=MA_SUCCESS||read!=frames)return false;for(u32 n=0;n<frames*2;n++)pcm[n]=std::clamp(pcm[n],-1.f,1.f);return true;}
void AudioDevice::pump(){
 auto& a=*impl;if(!a.ready||!a.stream||a.suspended)return;i32 queued=std::max(0,SDL_GetAudioStreamQueued(a.stream))/8;if(!a.refill&&queued<4096)a.refill=true;if(!a.refill)return;if(queued>=6144){a.refill=false;return;}for(i32 n=0;n<6&&queued<6144;n++){float pcm[2048]{};if(!mix(pcm,1024)){a.stats[3]=1;return;}if(!SDL_PutAudioStreamData(a.stream,pcm,sizeof pcm)){a.stats[3]=2;return;}a.stats[1]++;a.stats[2]+=1024;queued+=1024;}if(queued>=6144)a.refill=false;
}
void AudioDevice::suspend(bool value){auto& a=*impl;a.suspended=value;if(a.stream){if(value)SDL_PauseAudioStreamDevice(a.stream);else SDL_ResumeAudioStreamDevice(a.stream);}}
bool AudioDevice::sound_available(u32 id)const{return id<impl->sounds.size()&&impl->sounds[id]!=nullptr;}
bool AudioDevice::sound_playing(u32 id){auto* voice=impl->voice(id);return voice&&ma_sound_is_playing(&voice->sound);}
void AudioDevice::sound_stop(u32 id){if(auto* voice=impl->voice(id))ma_sound_stop(&voice->sound);}
void AudioDevice::sound_position(u32 id,u32 frame){if(auto* voice=impl->voice(id))ma_sound_seek_to_pcm_frame(&voice->sound,frame);}
void AudioDevice::sound_pan(u32 id,i32 value){if(auto* voice=impl->voice(id)){ma_sound_set_pan_mode(&voice->sound,ma_pan_mode_balance);ma_sound_set_pan(&voice->sound,value>=0?1.f-std::pow(10.f,-float(value)/2000.f):std::pow(10.f,float(value)/2000.f)-1.f);}}
void AudioDevice::sound_volume(u32 id,i32 value){if(auto* voice=impl->voice(id))ma_sound_set_volume(&voice->sound,amplitude(value));}
void AudioDevice::sound_play(u32 id,u32 flags){if(auto* voice=impl->voice(id)){ma_sound_set_looping(&voice->sound,(flags&1)?MA_TRUE:MA_FALSE);ma_sound_start(&voice->sound);}}
const u32* AudioDevice::statistics()const{auto& a=*impl;a.stats[0]=a.ready;a.stats[4]=a.stream?std::max(0,SDL_GetAudioStreamQueued(a.stream))/8:0;a.stats[5]=u32(a.track);a.stats[6]=a.fade;a.stats[7]=a.current()?u32(std::round(ma_sound_get_volume(&a.current()->sound)*1000000.f)):0;a.stats[8]=a.music_paused;ma_uint64 cursor=0;if(a.current())ma_sound_get_cursor_in_pcm_frames(&a.current()->sound,&cursor);a.stats[9]=u32(cursor);a.stats[10]=a.layout.count();return a.stats;}
}
