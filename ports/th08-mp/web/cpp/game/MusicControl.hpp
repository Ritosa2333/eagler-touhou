#pragma once
#include "GameplaySession.hpp"
namespace th08 {
struct MusicControlActions {
    virtual ~MusicControlActions()=default;
    virtual void command(i32 opcode,i32 argument,const char* path)=0;
    virtual void midi_load(i32 slot,const char* path)=0;
    virtual void midi_stop()=0;
    virtual void midi_play(i32 slot)=0;
    virtual void midi_file(const char* path)=0;
    virtual void midi_start()=0;
    virtual void midi_fade(i32 milliseconds)=0;
};
// Original supervisor audio routing and Music Room unlock side effects.
class MusicControl {
    GameConfiguration& config;PlayRecord& records;MusicControlActions& actions;
    void unlock(i32 song);static bool wave_path(const char*,char (&out)[260]);
public:
    u32 game_flags=0;float frame_rate=1;bool midi_available=true;
    MusicControl(GameConfiguration& c,PlayRecord& p,MusicControlActions& a):config(c),records(p),actions(a){}
    i32 load(i32 slot,const char* path);i32 play(i32 slot,i32 song);i32 audio(const char* path,i32 song);i32 stop();i32 fade(float seconds);
};
}
