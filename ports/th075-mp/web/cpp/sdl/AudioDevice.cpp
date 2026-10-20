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
#include "FileResources.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <cstdio>
#include <cmath>
namespace th075::sdl {
namespace {float gain(i32 level,i32 volume){level+=(volume-100)*50;if(volume==0||level< -10000)level=-10000;if(level>0)level=0;return std::pow(10.f,float(level)/2000.f);}bool read_file(const std::string& path,std::vector<u8>& bytes){return FileResources::read_file(path,bytes);}}
struct AudioDevice::Impl {
 struct Voice {std::vector<u8> bytes;ma_decoder decoder{};ma_sound sound{};bool decoded=false,attached=false;~Voice(){if(attached)ma_sound_uninit(&sound);if(decoded)ma_decoder_uninit(&decoder);}};
 ma_engine engine{};SDL_AudioStream* stream=nullptr;bool engineReady=false,ready=false,refill=true,suspended=false;
 std::array<std::vector<std::unique_ptr<Voice>>,3> banks;std::unique_ptr<Voice> current,pending;std::vector<u8> musicCatalog;
 i16 currentCue=-1,pendingCue=-1;i32 level=0,target=-10000,step=-160;u8 stopWhenDone=1,startWhenDone=0;bool fading=false;u32 lastFade=0;
 ~Impl(){current.reset();pending.reset();for(auto& bank:banks)bank.clear();if(stream)SDL_DestroyAudioStream(stream);if(engineReady)ma_engine_uninit(&engine);}
 bool attach(Voice& v){if(ma_sound_init_from_data_source(&engine,&v.decoder,MA_SOUND_FLAG_NO_SPATIALIZATION,nullptr,&v.sound)!=MA_SUCCESS)return false;v.attached=true;return true;}
 bool bank(u8 index,const SoundPack& pack){std::vector<std::unique_ptr<Voice>> next(pack.samples.size());for(u32 i=0;i<pack.samples.size();i++){if(!pack.samples[i].present)continue;auto voice=std::make_unique<Voice>();voice->bytes=pack.samples[i].wave_file();auto config=ma_decoder_config_init(ma_format_f32,2,44100);if(ma_decoder_init_memory(voice->bytes.data(),voice->bytes.size(),&config,&voice->decoder)!=MA_SUCCESS)return false;voice->decoded=true;if(!attach(*voice))return false;next[i]=std::move(voice);}banks[index]=std::move(next);return true;}
 void play(u8 bank,i16 id,i32 volume){if(id<0||u32(id)>=banks[bank].size()||!banks[bank][id])return;auto& v=*banks[bank][id];ma_sound_stop(&v.sound);ma_sound_seek_to_pcm_frame(&v.sound,0);ma_sound_set_volume(&v.sound,gain(0,volume));ma_sound_start(&v.sound);}
 bool playing()const{return current&&ma_sound_is_playing(&current->sound);}
};
AudioDevice::AudioDevice():impl(std::make_unique<Impl>()){}AudioDevice::~AudioDevice()=default;void AudioDevice::close(){impl=std::make_unique<Impl>();}
bool AudioDevice::initialize(ResourceStore& resources){auto& a=*impl;if(a.ready)return true;auto cfg=ma_engine_config_init();cfg.noDevice=MA_TRUE;cfg.channels=2;cfg.sampleRate=44100;cfg.defaultVolumeSmoothTimeInPCMFrames=0;if(ma_engine_init(&cfg,&a.engine)!=MA_SUCCESS){error="Unable to initialize audio mixer";return false;}a.engineReady=true;SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES,"2048");const SDL_AudioSpec spec{SDL_AUDIO_F32,2,44100};if(SDL_InitSubSystem(SDL_INIT_AUDIO))a.stream=SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,&spec,nullptr,nullptr);if(!a.stream){error=SDL_GetError();return false;}std::vector<u8> data;SoundPack system;if(!resources.read(ResourceEdition::Base,"wave/se.dat",data)||!system.load(data.data(),data.size())||!a.bank(0,system)){error="Invalid system sound pack";return false;}if(!read_file("/music/catalog.bin",a.musicCatalog)){error="Missing music catalog";return false;}SDL_ResumeAudioStreamDevice(a.stream);a.ready=true;return true;}
bool AudioDevice::character_bank(u8 side,const SoundPack& pack){if(side>1||!impl->ready)return false;if(!impl->bank(side+1,pack)){error="Unable to decode character sounds";return false;}return true;}
void AudioDevice::release_character(u8 side){if(side<2)impl->banks[side+1].clear();}
void AudioDevice::sound(i16 id){impl->play(0,id,soundVolume);}void AudioDevice::character_sound(u8 side,i16 id){if(side<2)impl->play(side+1,id,soundVolume);}
void AudioDevice::stop_sound(u8 bank,i16 id){if(bank<impl->banks.size()&&id>=0&&u32(id)<impl->banks[bank].size()&&impl->banks[bank][id])ma_sound_stop(&impl->banks[bank][id]->sound);}
bool AudioDevice::select_music(i16 cue){if(!musicEnabled)return true;auto& a=*impl;if(!a.ready||cue<0)return false;const u8* entry=nullptr;for(u32 i=0;i+16<=a.musicCatalog.size();i+=16){i32 id;std::memcpy(&id,a.musicCatalog.data()+i,4);if(id==cue){entry=a.musicCatalog.data()+i;break;}}if(!entry){error="Unknown music cue "+std::to_string(cue);return false;}u32 start,end,loop;std::memcpy(&start,entry+4,4);std::memcpy(&end,entry+8,4);std::memcpy(&loop,entry+12,4);const auto original=music_resource_path(cue);const auto slash=original.find_last_of('/');const auto filename=original.substr(slash+1);const auto path="/music/"+filename.substr(0,filename.size()-4)+".ogg";std::vector<u8> available;if(!FileResources::read_file(path,available)){error="Missing music "+path;return false;}auto next=std::make_unique<Impl::Voice>();auto cfg=ma_decoder_config_init(ma_format_f32,2,44100);if(ma_decoder_init_file(path.c_str(),&cfg,&next->decoder)!=MA_SUCCESS){error="Unable to decode "+path;return false;}next->decoded=true;ma_uint64 frames=0;if(ma_decoder_get_length_in_pcm_frames(&next->decoder,&frames)!=MA_SUCCESS||frames!=end||start>=end){error="Music loop or sample count mismatch";return false;}if(loop)ma_data_source_set_loop_point_in_pcm_frames(&next->decoder,start,end);if(!a.attach(*next)){error="Unable to attach music";return false;}ma_sound_set_looping(&next->sound,loop?MA_TRUE:MA_FALSE);a.pending=std::move(next);a.pendingCue=cue;return true;}
void AudioDevice::stop_music(){auto& a=*impl;if(a.current)ma_sound_stop(&a.current->sound);a.fading=false;}
void AudioDevice::start_music(){auto& a=*impl;if(a.pending){a.current=std::move(a.pending);a.currentCue=a.pendingCue;a.pendingCue=-1;}if(!a.current)return;a.level=0;a.fading=false;ma_sound_seek_to_pcm_frame(&a.current->sound,0);ma_sound_set_volume(&a.current->sound,gain(a.level,musicEnabled?musicVolume:0));ma_sound_start(&a.current->sound);}
void AudioDevice::music_level(i32 level){impl->level=level;if(impl->current)ma_sound_set_volume(&impl->current->sound,gain(level,musicEnabled?musicVolume:0));}
void AudioDevice::fade_music(i32 target,i32 step,u8 stopWhenDone,u8 startWhenDone){auto& a=*impl;if(!a.playing()){a.fading=false;if(startWhenDone)start_music();return;}a.target=target;a.step=step;a.stopWhenDone=stopWhenDone;a.startWhenDone=startWhenDone;a.fading=true;a.lastFade=u32(SDL_GetTicks());}
void AudioDevice::update(u32 now){auto& a=*impl;if(a.fading&&a.playing()&&u32(now-a.lastFade)>=80){a.lastFade+=80;if(a.current)ma_sound_set_volume(&a.current->sound,gain(a.level,musicEnabled?musicVolume:0));a.level=increment_wrapped(a.level,a.step*4);if(a.step<0){if(a.level<a.target||a.level< -9999)a.fading=false;}else if(a.level<a.target||a.level>=0){if(a.level>=0)a.level=0;a.fading=false;}if(!a.fading&&a.stopWhenDone){stop_music();if(a.startWhenDone)start_music();}}if(a.current&&!a.fading)ma_sound_set_volume(&a.current->sound,gain(a.level,musicEnabled?musicVolume:0));}
bool AudioDevice::mix(float* pcm,u32 frames){ma_uint64 read=0;if(!impl->ready||!pcm||ma_engine_read_pcm_frames(&impl->engine,pcm,frames,&read)!=MA_SUCCESS||read!=frames)return false;for(u32 i=0;i<frames*2;i++)pcm[i]=std::clamp(pcm[i],-1.f,1.f);return true;}
// Bound PCM work per browser callback. A six-block refill becomes a long
// main-thread blackout on constrained devices and delays input/network events.
#if TH075_MULTIPLAYER
static constexpr i32 refillBlocks=2;
#else
static constexpr i32 refillBlocks=6;
#endif
void AudioDevice::pump(){auto& a=*impl;if(!a.ready||!a.stream||a.suspended)return;i32 queued=std::max(0,SDL_GetAudioStreamQueued(a.stream))/8;if(!a.refill&&queued<4096)a.refill=true;if(!a.refill)return;if(queued>=6144){a.refill=false;return;}for(i32 i=0;i<refillBlocks&&queued<6144;i++){float pcm[2048]{};if(!mix(pcm,1024))return;if(!SDL_PutAudioStreamData(a.stream,pcm,sizeof(pcm)))return;queued+=1024;}if(queued>=6144)a.refill=false;}
const u32* AudioDevice::statistics(){static u32 state[5];auto& a=*impl;state[0]=a.ready;state[1]=a.stream?u32(std::max(0,SDL_GetAudioStreamQueued(a.stream))/8):0;state[2]=u32(a.currentCue);state[3]=a.suspended;state[4]=musicEnabled;return state;}
void AudioDevice::suspend(bool suspend){auto& a=*impl;a.suspended=suspend;if(a.stream){if(suspend)SDL_PauseAudioStreamDevice(a.stream);else SDL_ResumeAudioStreamDevice(a.stream);}}
}
