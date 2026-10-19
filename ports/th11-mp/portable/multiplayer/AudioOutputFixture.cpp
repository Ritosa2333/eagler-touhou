// Linked only by audio-output.test.mjs, never by the Runtime builder.
#include "../../web/cpp/sdl/AudioDevice.hpp"
#include <memory>
namespace {
std::unique_ptr<th11::GameResources> resources;
std::unique_ptr<th11::sdl::AudioDevice> audio;
}
extern "C" __attribute__((export_name("audio_fixture_initialize")))
unsigned audio_fixture_initialize(){
    size_t size=0;auto* bytes=static_cast<th11::u8*>(SDL_LoadFile("/th11.dat",&size));
    resources=std::make_unique<th11::GameResources>();
    const bool opened=bytes&&resources->open_archive(bytes,unsigned(size));SDL_free(bytes);
    if(!opened)return 0;audio=std::make_unique<th11::sdl::AudioDevice>();
    return audio->initialize(*resources)?1:0;
}
extern "C" __attribute__((export_name("audio_fixture_command")))
void audio_fixture_command(unsigned command,unsigned value){
    if(command==0)audio->sound_play(value);
    if(command==1)audio->spectator_silent(value!=0);
    if(command==2)audio->suspend(value!=0);
    if(command==3){audio.reset();resources.reset();}
}
