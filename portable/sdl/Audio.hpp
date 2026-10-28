// Sound effects and music: upstream src/sound-player.mjs and music.mjs on
// miniaudio + SDL3 queued audio. Effects are
// the original WAVs inside th12.dat, indexed by th12.exe's sound table;
// music streams out of thbgm.dat with thbgm.fmt's loop points.
#pragma once
#include "ExeTables.hpp"
#include <memory>

namespace th12host {
class Archive;
struct MusicTrack {std::string name;u32 offset=0,loop_start=0,length=0,channels=2,rate=44100,align=4;};
std::vector<MusicTrack> parse_music_layout(const std::vector<u8>& fmt);
// DirectSound volume rules (4544b0 / 4663e0): effects use a cube, music a square.
float native_sound_gain(i32 base,i32 percent);
float native_music_gain(i32 percent);
class Audio {
public:
    Audio();~Audio();
    std::string error;i32 se_volume=80,bgm_volume=100;
    bool initialize(Archive& archive,const ExeTables& tables);
    // One core sound event: restart (or stop) that id's voice.
    void effect(i32 id,i32 pan,bool stop);
    void pause_effects();void resume_effects();void stop_effects();
    i32 track_count()const;i32 track_index(const std::string& name)const;
    void play_music(i32 index);
    void retry_music();
    void play_music(const std::string& name){play_music(track_index(name));}
    std::string music_name()const; // current track, e.g. "th12_01" ("" when none)
    bool music_paused()const;
    // music.mjs fade: 4666c0 ramps the volume to -5000 dB, then stops.
    void fade_music(double seconds);
    void set_volumes(i32 bgm,i32 se);
    void update(double seconds); // per displayed frame: fades
    void stop_music();void pause_music(bool paused);
    void pump();void suspend(bool paused);
    bool mix(float* pcm,u32 frames); // host tests
private:
    struct Impl;std::unique_ptr<Impl> impl;
};
}
