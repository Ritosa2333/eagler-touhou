// Device/mixer integration fixture; not a playable-game application.
#include "../../cpp/sdl/AudioDevice.hpp"
#include <fstream>
#include <memory>
using namespace th15;
namespace {
struct Files final:AssetSource {bool read(const std::string& name,std::vector<u8>& out)override{std::ifstream f("/assets/"+name,std::ios::binary|std::ios::ate);if(!f)return false;const auto size=f.tellg();if(size<0)return false;out.resize(u32(size));f.seekg(0);return bool(f.read(reinterpret_cast<char*>(out.data()),out.size()));}}files;
std::unique_ptr<sdl::AudioDevice> audio;std::vector<float> pcm;std::string current;
}
extern "C" {
int audio_initialize(int device){audio=std::make_unique<sdl::AudioDevice>();return audio->initialize(files,device!=0);}
const char* audio_error(){return audio?audio->error.c_str():"No audio fixture";}
int audio_enqueue(int id,float x,int positioned){return positioned?audio->effects.positioned(id,x):audio->effects.enqueue(id);}
void audio_update(){audio->update();}void audio_pump(){audio->pump();}
int audio_queue_music(i32 code,i32 value,const char* text){return audio->queue_music(code,value,text);}
int audio_finish_requests(){return audio->finish_music_requests();}
const i32* audio_queue_state(){return &audio->music_commands().requests[0].code;}
const float* audio_mix(u32 frames){if(frames>1000000)return nullptr;pcm.resize(frames*2);return audio->mix(pcm.data(),frames)?pcm.data():nullptr;}
u32 audio_playing(u32 id){return audio->sound_playing(id);}void audio_effects_stop(int id){audio->effects.stop(id);audio->effects.process();}
void audio_effects_suspend(){audio->effects.suspend();}void audio_effects_resume(){audio->effects.resume();}
int audio_music_prepare(const char* file){return audio->prepare_music(file);}int audio_music_file(const char* file){return audio->music_file(file);}void audio_music_stop(){audio->stop_music();}
void audio_music_pause(u32 value){audio->pause_music(value!=0);}void audio_music_fade(i32 frames){audio->fade_music(frames);}int audio_music_seek(double seconds){return audio->seek_music(seconds);}
const char* audio_current_music(){double seconds=0;audio->current_music(current,seconds);return current.c_str();}
double audio_current_seconds(){double seconds=0;audio->current_music(current,seconds);return seconds;}
void audio_volume(i32 effect,i32 music){audio->effects.master_volume=effect;audio->music_volume=music;audio->refresh_volume();}
const u32* audio_statistics(){return audio->statistics();}void audio_device_suspend(u32 value){audio->suspend(value!=0);}
}
