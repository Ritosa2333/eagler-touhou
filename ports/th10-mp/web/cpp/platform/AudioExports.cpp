#include "Audio.hpp"
#include "AudioData.hpp"
#include <cstdlib>
#include <new>
using namespace th10;
#ifdef TH_SDL3
#define AUDIO_IMPORT(name) extern "C"
#else
#define AUDIO_IMPORT(name) extern "C" __attribute__((import_module("th10_audio"),import_name(name)))
#endif
AUDIO_IMPORT("create") u32 audio_host_create();
AUDIO_IMPORT("call") i32 audio_host_call(u32,u32,const u32*);
AUDIO_IMPORT("event") u32 audio_host_event(u32,u32);
AUDIO_IMPORT("advance") void audio_host_advance(u32);
namespace {
struct Host final:browser::AudioHost {
    u32 create() override{return audio_host_create();}
    i32 call(u32 h,browser::AudioOperation o,const u32* args) override{return audio_host_call(h,static_cast<u32>(o),args);}
    u32 event(u32 op,u32 h) override{return audio_host_event(op,h);}
    void advance(u32 ms) override{audio_host_advance(ms);}
} host;
}
#define AUDIO_EXPORT(name) extern "C" __attribute__((export_name(name)))
AUDIO_EXPORT("audio_create") browser::Audio* audio_create(browser::FileSystem* files){auto* bytes=std::malloc(sizeof(browser::Audio));return bytes?new(bytes)browser::Audio(host,*files):nullptr;}
AUDIO_EXPORT("audio_destroy") void audio_destroy(browser::Audio* audio){if(audio){audio->~Audio();std::free(audio);}}
AUDIO_EXPORT("audio_initialize") i32 audio_initialize(browser::Audio* audio,u32 window){return audio->initialize(window);}
AUDIO_EXPORT("audio_manager") AudioManager* audio_manager(browser::Audio* audio){return &audio->manager;}
AUDIO_EXPORT("audio_configure") void audio_configure(browser::Audio* audio,u32 display,u32 music,u32 effects,i32 music_volume,i32 effects_volume){audio->display_flags=display;audio->music_enabled=music;audio->effects_enabled=effects;audio->configured_music=music_volume;audio->configured_effects=effects_volume;}
AUDIO_EXPORT("audio_formats") i32 audio_formats(browser::Audio* audio,const char* name){return audio->load_formats(name);}
AUDIO_EXPORT("audio_start_file") i32 audio_start_file(browser::Audio* audio,const char* name){return AudioResources{audio->manager,audio->resources}.start_file_music(name);}
AUDIO_EXPORT("audio_update") i32 audio_update(browser::Audio* audio){return audio->update();}
AUDIO_EXPORT("audio_advance") void audio_advance(browser::Audio* audio,u32 milliseconds){audio->advance(milliseconds);}
// Streaming notifications refill one original chunk. Full-buffer fills reset
// to a track entry, whose larger window the browser pins before starting.
AUDIO_EXPORT("audio_read_window") u32 audio_read_window(browser::Audio* audio){return audio->manager.music?audio->manager.music->chunk_bytes:0;}
AUDIO_EXPORT("audio_fades") void audio_fades(browser::Audio* audio){audio->advance_fades();}
AUDIO_EXPORT("audio_queue_effect") void audio_queue_effect(browser::Audio* audio,i32 effect,i32 pan){if(effect>=0&&effect<47)audio->manager.queue_effect(effect,pan,browser::sound_definitions);}
AUDIO_EXPORT("audio_queue_music") void audio_queue_music(browser::Audio* audio,i32 kind,i32 argument,const char* name){audio->manager.queue_music(kind,argument,name);}
AUDIO_EXPORT("audio_stream_operation") i32 audio_stream_operation(browser::Audio* audio,u32 operation,i32 value){
    auto* stream=audio->manager.music;if(!stream)return -1;
    if(operation==0)return stream->wave->select(audio->manager.formats+value,audio->waves);
    if(operation==1)return stream->fill(stream->buffer(0),value!=0,audio->buffers);
    if(operation==2)return stream->wave->reset(value!=0,audio->waves);
    if(operation==3)return stream->update(value!=0,audio->buffers);
    if(operation==4)return stream->set_volume(value,audio->buffers);
    if(operation==5)return stream->reset(audio->buffers);
    if(operation==6)return stream->play(0,value,audio->buffers);
    return -1;
}
AUDIO_EXPORT("audio_tasks") u32 audio_tasks(browser::Audio* audio){u32 count=0;for(const auto& task:audio->tasks)if(task.id)++count;return count;}
AUDIO_EXPORT("audio_call") i32 audio_call(u32 handle,u32 operation,const u32* arguments){return host.call(handle,static_cast<browser::AudioOperation>(operation),arguments);}
