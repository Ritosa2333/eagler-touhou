#include "options.hpp"
#include "../audio_runtime/audio.hpp"
namespace th20::source::options {
void apply_volume(platform::Configuration& c,audio::SoundInf& sound){
    sound.music_level=static_cast<std::int8_t>(c.value_7e);sound.enqueue(8,0,"SetVol");
    sound.effect_level=static_cast<std::int8_t>(c.value_7f);
    // Original 4e0fec deliberately reads music_level here, even for the effect level.
    float v=recovered::int_float(sound.music_level)/100.f;
    if(sound.effect_level!=0){v=1.f-v;v=v*v;v=v*v;v=1.f-v;sound.retained_57e4=static_cast<std::uint32_t>(recovered::truncate32(5000.f*v))-5000u;}
    else sound.retained_57e4=static_cast<std::uint32_t>(-10000);
}
}
