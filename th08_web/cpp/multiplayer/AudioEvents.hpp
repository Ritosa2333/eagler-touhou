#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Confirmed audio events are multiplayer-only
#endif
#include "../game/Types.hpp"
#include <array>
#include <vector>

namespace th08::multiplayer {
enum class AudioEventKind:u8 {
    Sound,WaveCommand,MidiLoad,MidiStop,MidiPlay,MidiFile,MidiStart,MidiFade,
    MidiReset,StartBgm,Fade,Process,TickFades,Volumes
};
struct AudioSettings {
    bool wav=false,preload=false,sounds=true;
    i32 sound_volume=100,music_volume=100;
};
struct AudioEvent {
    AudioEventKind kind=AudioEventKind::Process;
    i32 first=0,second=0;float value=0;bool flag=false;
    AudioSettings settings;
    char text[256]{};
};
struct AudioEventOutput {
    virtual ~AudioEventOutput()=default;
    virtual bool apply_audio_event(const AudioEvent&)=0;
};
// A bounded logical command outbox. TH08 MusicControl continues to run at
// simulation time (including music unlocks); its commands and native SFX/fade
// processing reach GameAudioManager only after corrected inputs are confirmed.
// Committed output is irreversible and its cursor is never part of WorldJournal.
class AudioEvents {
public:
    static constexpr u32 invalid_frame=~u32(0),capacity=16,events_per_frame=4096;
    void Reset(u32 first=0);
    bool BeginFrame(u32 frame);
    bool Append(const AudioEvent&);
    bool EndFrame();
    bool DiscardFrom(u32 frame);
    bool CommitThrough(u32 confirmed,u32 simulated,AudioEventOutput&);
    bool Failed()const{return failed;}
    bool IsOpen()const{return open!=invalid_frame;}
    u32 NextCommit()const{return next_commit;}
    u32 PendingFrames()const{return next_append-next_commit+u32(IsOpen());}
    u32 CommittedEvents()const{return committed_events;}
    u32 CommittedDigest()const{return committed_digest;}
    u32 FrameDigest(u32 frame)const;
    u32 FrameEventCount(u32 frame)const;
private:
    struct Frame {u32 number=invalid_frame;bool closed=false;std::vector<AudioEvent> events;};
    std::array<Frame,capacity> frames;
    u32 open=invalid_frame,next_append=0,next_commit=0,committed_events=0;
    u32 committed_digest=2166136261u;
    bool failed=false,draining=false;
    bool Fail(){failed=true;return false;}
    static bool Valid(const AudioEvent&);
    static u32 DigestEvent(const AudioEvent&,u32 hash);
};
}
