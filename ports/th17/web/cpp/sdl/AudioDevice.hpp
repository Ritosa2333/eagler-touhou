#pragma once
#include "../game/StageAssets.hpp"
#include "../game/SoundEffects.hpp"
#include "../game/MusicLayout.hpp"
#include "../game/MusicCommands.hpp"
#include <SDL3/SDL.h>
#include <memory>
namespace th17::sdl {
// Same SDL3 float-stereo stream and miniaudio mixer as the delivered ports;
// TH17 owns its original voice definitions and PCM music loop boundaries.
class AudioDevice final:public SoundOutput {
 struct Impl;std::unique_ptr<Impl> impl;
public:
 std::string error;SoundEffects effects{*this};i32 music_volume=100;bool music_enabled=true;std::string music_directory="/music";
 AudioDevice();~AudioDevice();bool initialize(AssetSource&,bool device=true);void close();
 bool prepare_music(const std::string&);bool music_file(const std::string&);bool music(i32);void stop_music();void fade_music(i32 frames=240);void pause_music(bool);bool seek_music(double seconds);bool current_music(std::string&,double&);
 bool queue_music(i32 code,i32 value,const std::string& text="");bool finish_music_requests();const MusicCommands& music_commands()const;
 bool queue_music_track(i32 slot,const std::string& stem);void clear_current_wave();
 void refresh_volume();void update();void pump();void suspend(bool);bool mix(float*,u32 frames);
 bool sound_available(u32)const override;bool sound_playing(u32)override;
 void sound_stop(u32)override;void sound_position(u32,u32)override;void sound_pan(u32,i32)override;void sound_volume(u32,i32)override;void sound_play(u32,u32)override;
 const u32* statistics()const;
};
}
