#pragma once
#include "AudioManager.hpp"
#include "ScoreData.hpp"
namespace th10 {
struct PracticeState;
struct AudioGame {
    AudioManager& manager;ScoreData** scores;const u32* display_flags;const float* rate;
#ifdef TH_ENABLE_THPRAC
    PracticeState* practice=nullptr;
#endif
    i32 prepare(i32 slot,const char* name);
    void play(i32 slot,i32 track,bool stage_start=false);
    void play_file(const char* name,i32 track,bool stage_start=false);
    void stop();
    void fade(float seconds);
    static i32 enabled(u8 mode) noexcept {return mode==1||mode==2?0:-1;}
private:
    static bool wave_name(char* output,const char* name);
    void unlock(i32 track);
#ifdef TH_ENABLE_THPRAC
    // F7 everlasting-BGM filter (thprac ElBgmTest, thprac_th10.cpp:2270). Returns
    // true to drop the command. Commands: 0 play, 1 stop, 2 pause, 3 resume,
    // anything else is upstream's default arm (no state change). stage_start
    // distinguishes the 0x420b10 play whose caller is 0x4183e6; only that call
    // site may arm the BGM lock (ElBgmTest's caller==caller_addr guard).
    bool filter(i32 command,i32 song,bool stage_start);
#endif
};
#ifdef TH_ENABLE_THPRAC
// Free form of the th10 ElBgmTest command classification. The upstream switch
// keys on the return address of the BGM-manager call; the port maps that to its
// own command set: play/play_file = 0x420b42, stop = 0x4180a7, pause = the
// Results pause music command (0x422be1), resume = the Results resume command
// (0x422c51). Fade/other commands match no arm and only observe the lock.
// stage_start mirrors ElBgmTest's `caller == 0x4183e6` play guard.
bool practice_bgm_filter(PracticeState& practice,i32 command,i32 song,bool stage_start);
// Parity with the th08 fix for the same restart+everlasting-BGM bug: upstream
// routes the stage-teardown BGM stop through the same hooked BGM-manager call
// as ElBgmTest, so the lock swallows it. Returns true when the teardown stop
// must be skipped to keep the locked song playing across a thprac practice
// restart instead of hard-stopping it and then swallowing the next play.
inline bool practice_bgm_stop(PracticeState& practice){return practice_bgm_filter(practice,1,0,false);}
#endif
}
