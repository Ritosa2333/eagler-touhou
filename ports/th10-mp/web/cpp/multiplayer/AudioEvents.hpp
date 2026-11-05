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
private:
    enum class Kind:u32 {Effect,Stop,Music};
    struct Event {
        Kind kind{};i32 first=0,second=0;
        // Pointer bytes for Effect, copied filename length for Music. Decode
        // through memcpy because music makes subsequent headers unaligned.
        std::array<u8,sizeof(const SoundDefinition*)> payload{};
    };
public:
    // Retain the previous byte budget: 1024 records with a 256-byte inline
    // music filename. Compact ordinary commands rather than dropping any.
    static constexpr std::size_t capacity=16;
    static constexpr std::size_t bytes_per_frame=1024*(sizeof(Event)+256);
    // Maximum count for ordinary commands; music also consumes filename bytes.
    static constexpr std::size_t events_per_frame=bytes_per_frame/sizeof(Event);
    bool BeginFrame(u32 frame);
    bool EndFrame();
    bool DiscardFrom(u32 frame);
    void Reset();
    bool CommitThrough(u32 confirmed,u32 last,AudioManager& output,
                       bool (*finish_frame)(void*),void* context);
    bool Failed()const{return failure_!=Failure::None;}
    u32 FailureCode()const{return static_cast<u32>(failure_);}
    const char* FailureReason()const;
    std::size_t CapturedBytes(u32 frame)const;
    u32 CapturedCommands(u32 frame)const;
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
    struct Frame {u32 number=Netplay::INVALID_FRAME;bool closed=false;std::vector<u8> events;};
    std::array<Frame,capacity> frames_{};
    u32 open_=Netplay::INVALID_FRAME,next_commit_=0;
    u32 effects_=0,stops_=0,music_=0,digest_=2166136261u;
    enum class Failure:u32 {None,CaptureClosed,Capacity,EffectId,Definitions,StopId,Filename,
        CommitFrame,FinishFrame,Record};
    Failure failure_=Failure::None;
    void fail(Failure reason){if(!Failed())failure_=reason;}
    static u32 filename_bytes(const Event& event);
    static bool scan(const Frame& frame,u32& count);
    void append(const Event& event,const char* filename=nullptr);
    void word(u32 value);
};
}
