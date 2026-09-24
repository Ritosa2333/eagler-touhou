#pragma once
#include "SessionSetup.hpp"
#include <eagler/netplay/NetplayCore.hpp>
#include <eagler/netplay/NetplaySession.hpp>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace th08::multiplayer {
class NetworkConnection;
// Title admission/frontiers only. Session packets, history, prediction and
// packet encoding stay in eagler-common. Physical sampling and world undo are
// owned by the application, never by packet delivery.
class NetplayRuntime {
    friend class NetworkConnection;
public:
    static constexpr std::uint8_t MaxRollbackFrames=8;
    enum class WireResult {Accepted,IgnoredSession,Malformed,Rejected};

    bool Reset(const SessionSetup&) noexcept;
    void Clear() noexcept;
    bool Configured()const{return configured_;}
    bool CanStart()const{return configured_&&!retired_&&gate_.CanStart();}
    const Netplay::SessionConfig& Config()const{return gate_.Config();}
    const SessionSetup& Setup()const{return setup_;}
    std::uint32_t NextFrame()const{return next_;}
    std::uint32_t LastFrame()const{return core_.LastSimulatedFrame();}
    std::uint32_t Generation()const{return generation_;}
    bool Retired()const{return retired_;}

    Netplay::SessionPacket SessionPacket(Netplay::SessionPhase phase)const{return gate_.BuildPacket(phase);}
    Netplay::SessionPacketResult ApplySession(const Netplay::SessionPacket& packet){return gate_.Apply(packet);}
    bool MarkReady();
    bool Ready()const{return gate_.LocalReady();}

    bool CaptureLocal(std::uint32_t frame,const Netplay::FrameInput&);
    bool HasLocal(std::uint32_t frame)const{return core_.HasLocalCapture(frame);}
    Netplay::RemoteInputResult SubmitRemote(std::uint8_t seat,std::uint32_t frame,const Netplay::FrameInput&);
    Netplay::FrameDecision Prepare(std::uint32_t frame)const;
    bool MarkSimulated(std::uint32_t frame,const Netplay::FrameDecision&);
    std::uint32_t ConfirmedThrough()const{return core_.ConfirmedThroughAllRemotes();}
    bool ConfirmedInputs(std::uint32_t frame,std::array<Netplay::FrameInput,Netplay::MAX_PLAYERS>&)const;
    std::uint32_t RollbackFrame()const{return core_.RollbackFrame();}

    // Prediction is admitted only after the actual title journal is ready.
    // Loading or an unregistered owner cannot silently run speculative ticks.
    bool SetWorldReady(bool ready);
    bool WorldReady()const{return world_ready_;}
    bool BeginCorrection(std::uint32_t first);
    bool EndCorrection(bool lifecycle_boundary=false);
    bool Correcting()const{return correction_end_!=Netplay::INVALID_FRAME;}
    bool CanRetire()const;
    bool Retire();
    bool BeginNextRun(SessionSetup&,std::uint32_t seed);

    WireResult ApplyWire(const std::uint8_t*,std::size_t);
    bool BuildInputWire(std::uint8_t peer,std::uint32_t frame,std::uint32_t sequence,
                        std::uint32_t ack,std::vector<std::uint8_t>&)const;
    static bool ValidInput(const Netplay::FrameInput&) noexcept;

private:
    bool configure(const SessionSetup&) noexcept;
    bool receive_frame(std::uint32_t frame)const;
    Netplay::SessionGate gate_;
    Netplay::RollbackCore core_;
    SessionSetup setup_{};
    std::uint64_t base_session_id_=0;
    std::uint32_t next_=0,generation_=0,correction_end_=Netplay::INVALID_FRAME;
    bool configured_=false,retired_=false,world_ready_=false;
};
}
