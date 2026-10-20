#pragma once
#include "Types.hpp"
namespace th075 {
struct WaveFormat {u16 tag=1,channels=1;u32 sampleRate=44100,bytesPerSecond=88200;u16 blockAlign=2,bitsPerSample=16,extraSize=0;};
struct SoundSample {bool present=false;WaveFormat format;std::vector<u8> pcm;std::vector<u8> wave_file()const;};
class SoundPack {
public:
 std::vector<SoundSample> samples;std::string error;
 bool load(const void*,std::size_t);
};
struct MusicLoop {u32 endBytes=0,lengthBytes=0;};
MusicLoop original_music_loop(const void*,std::size_t,i16 cue);
std::string music_resource_path(i16);
float sound_volume_gain(i32 volume);
}
