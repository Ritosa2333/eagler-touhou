#pragma once
#include "../game/GameResources.hpp"
#include <SDL3/SDL.h>
#include "../game/SoundEffects.hpp"
#include <memory>
#if PORT_TITLE == 14
#include <functional>
#include "../game/Th14MusicQueue.hpp"
#endif
namespace th11::sdl {
class AudioDevice final:public SoundOutput {
    struct Impl;std::unique_ptr<Impl> impl;
#if PORT_TITLE == 14
    friend struct AudioDeviceProbeTh14;
#endif
public:
    std::string error;SoundEffects effects{*this};i32 music_volume=100;bool music_enabled=true;
    AudioDevice();~AudioDevice();
    bool initialize(GameResources&);
#if PORT_TITLE == 14
    // Device APIs stay on Main. CPU preparation owns an entirely separate
    // AudioDevice until the resource worker has joined its sample loader.
    bool initialize_device();
    bool prepare_cpu(GameResources&,const std::function<bool()>& cancelled);
    void prepare_formats(std::vector<u8>);
    bool adopt_prepared(AudioDevice&);
    Th14MusicQueue& music_requests()noexcept;
    bool consume_initial_music_request(bool native_direct_sound);

#endif
    void close();
    bool music(i32 track);bool music_file(const char*);void fade_music(i32 frames=240);void pause_music(bool);
    i32 current_music()const noexcept;
    bool save_music();bool restore_music();void discard_saved_music();
#if PORT_TITLE == 14
    void pause_sounds(bool);void discard_paused_sounds();
#endif
    void refresh_volume();void update();void pump();void suspend(bool);
    bool mix(float*,u32 frames);
    void sound_stop(u32)override;void sound_position(u32,u32)override;
    void sound_pan(u32,i32)override;void sound_volume(u32,i32)override;void sound_play(u32)override;
    const u32* statistics()const;
};
}
