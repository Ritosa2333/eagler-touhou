#pragma once
#include "../game/GameApplication.hpp"
#include "../game/MusicControl.hpp"
#include "../game/MusicCommands.hpp"
#include "../game/SoundEffects.hpp"
#include "../game/MidiPlayer.hpp"
namespace th08 {
class BrowserRuntime;
// Owns game music selection, effect voices, fades and MIDI sequencing.
// The SoundDevice owns only platform playback resources.
class GameAudioManager:public AudioSink,public MusicControlActions,public MusicActions,public MidiDeviceSink {
    BrowserRuntime& host;
    BgmFormats formats;const BgmFormat* current_format=nullptr;SoundEffects effects{*this};AudioFades fades{*this};
    MusicCommands commands{*this};std::unique_ptr<MusicControl> control;bool music_loop=false;
    MidiPlayer midi{*this};u32 midi_clock=0;bool midi_failed=false;
    void audio_context();bool open_music(const BgmFormat*);
    std::vector<u8> read(const char*);u32 milliseconds();
public:
    explicit GameAudioManager(BrowserRuntime&);
    bool prepare_formats();bool prepare_samples();void shutdown();
    void sound(i32,i32,float,bool);bool play_music(i32,i32);void load_music(i32,const char*);void play_audio(const char*,i32);
    void stop_audio();void fade_music(float);void menu_music(MenuMusic,float);void midi_reset();void start_bgm();
    void process_sounds();void update_audio_fades();void apply_volume(const GameConfiguration&);bool audio_tick(u32);
    i32 stop(u32)override;i32 position(u32,u32)override;i32 pan(u32,i32)override;i32 volume(u32,i32)override;i32 play(u32,u32,u32)override;
    void command(i32,i32,const char*)override;void midi_load(i32,const char*)override;void midi_stop()override;
    void midi_play(i32)override;void midi_file(const char*)override;void midi_start()override;void midi_fade(i32)override;
    void set_volume(i32)override;void stop_all()override;i32 preload(i32,const char*)override;i32 load(i32)override;
    i32 reset()override;i32 fill(bool)override;void play()override;void stop()override;void recreate_buffers()override;
    void reopen(const char*)override;void request_thread_stop()override{};bool thread_stopped()override{return true;}
    void close_music()override;void fade_out(float)override;void pause()override;void unpause()override;
    void open()override;void close()override;void short_message(u8,u8,u8)override;void long_message(const u8*,u32)override;
};
}
