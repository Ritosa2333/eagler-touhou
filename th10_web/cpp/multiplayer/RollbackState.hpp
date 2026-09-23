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
    std::size_t CapturedBytes(std::uint32_t frame)const{return journal_.BytesForFrame(frame);}

private:
    Netplay::RollbackJournal journal_{};
    bool configured_=false;
};

} // namespace th10::multiplayer
