#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Logical audio events are multiplayer-only
#endif
#include "../game/AudioManager.hpp"
#include <eagler/netplay/NetplayProtocol.hpp>
#include <array>
#include <vector>

namespace th10::multiplayer {

// A title command outbox, not another mixer/player. Only surviving confirmed
// logical frames reach the native AudioManager, in original event order.
// Rewind replaces uncommitted records; the already emitted cursor never rewinds.
class AudioEvents final:public AudioCommandSink {
public:
    static constexpr std::size_t capacity=16,events_per_frame=1024;
    bool BeginFrame(u32 frame);
    bool EndFrame();
    bool DiscardFrom(u32 frame);
    void Reset();
    bool CommitThrough(u32 confirmed,u32 last,AudioManager& output,
                       bool (*finish_frame)(void*),void* context);
    bool Failed()const{return failed_;}
    bool IsOpen()const{return open_!=Netplay::INVALID_FRAME;}
    u32 NextCommit()const{return next_commit_;}
    u32 EffectsCommitted()const{return effects_;}
    u32 StopsCommitted()const{return stops_;}
    u32 MusicCommitted()const{return music_;}
    u32 Digest()const{return digest_;}
    u32 PendingFrames()const;
    void capture_effect(i32 effect,i32 pan,const SoundDefinition*) noexcept override;
    void capture_stop(i32 effect) noexcept override;
    void capture_music(i32 kind,i32 argument,const char* filename) override;
private:
    enum class Kind:u32 {Effect,Stop,Music};
    struct Event {
        Kind kind{};i32 first=0,second=0;
        const SoundDefinition* definitions=nullptr;
        char filename[256]{};
    };
    struct Frame {u32 number=Netplay::INVALID_FRAME;bool closed=false;std::vector<Event> events;};
    std::array<Frame,capacity> frames_{};
    u32 open_=Netplay::INVALID_FRAME,next_commit_=0;
    u32 effects_=0,stops_=0,music_=0,digest_=2166136261u;
    bool failed_=false;
    void append(const Event& event);
    void word(u32 value);
};
}
