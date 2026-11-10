#pragma once
#include <eagler/netplay/NetplayProtocol.hpp>
namespace th15::multiplayer {
// A factory or external effect may defer one predicted frame until its exact
// inputs arrive. Its confirmed execution ends the temporary capture phase.
class PredictionBoundary {
    std::uint32_t frame_=Netplay::INVALID_FRAME;
public:
    void clear()noexcept{frame_=Netplay::INVALID_FRAME;}
    void defer(std::uint32_t frame)noexcept{frame_=frame;}
    void completed(std::uint32_t frame)noexcept{if(frame_==frame)clear();}
    bool waiting()const noexcept{return frame_!=Netplay::INVALID_FRAME;}
    bool blocks(std::uint32_t frame)const noexcept{return frame_==frame;}
    std::uint32_t frame()const noexcept{return frame_;}
};
}
