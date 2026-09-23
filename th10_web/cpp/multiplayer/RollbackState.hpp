#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error RollbackState is multiplayer-only
#endif

#include <eagler/netplay/RollbackJournal.hpp>
#include <cstddef>
#include <cstdint>

namespace th10::browser { struct World; }

namespace th10::multiplayer {

class RollbackState {
public:
    bool Reset();
    void Clear();
    bool BeginFrame(browser::World& world,std::uint32_t frame);
    bool Touch(void* address,std::size_t bytes);
    bool EndFrame();
    bool RestoreTo(std::uint32_t frame,std::uint32_t* replayFrom=nullptr);
    void DiscardBefore(std::uint32_t frame){journal_.DiscardBefore(frame);}
    bool Failed()const{return journal_.Failed();}
    bool IsCapturing()const{return configured_&&journal_.IsFrameOpen();}
    std::uint32_t OpenFrame()const{return journal_.OpenFrame();}
    std::size_t CapturedBytes(std::uint32_t frame)const{return journal_.BytesForFrame(frame);}
    std::size_t LastBytes()const{return last_bytes_;}
    std::size_t PeakBytes()const{return peak_bytes_;}
    std::size_t LiveFrames()const{return journal_.FrameCount();}
    std::uint64_t SnapshotBytes()const{return total_bytes_;}
    std::uint32_t SnapshotFrames()const{return snapshots_;}

private:
    Netplay::RollbackJournal journal_{};
    bool configured_=false;
    // Diagnostics are outside the deterministic rewindable world.
    std::size_t last_bytes_=0,peak_bytes_=0;
    std::uint64_t total_bytes_=0;
    std::uint32_t snapshots_=0;
};

} // namespace th10::multiplayer
