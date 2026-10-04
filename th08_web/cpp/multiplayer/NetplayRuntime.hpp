#pragma once
#include "SessionSetup.hpp"
#include <eagler/netplay/NetplayCore.hpp>
#include <eagler/netplay/NetplaySession.hpp>
#include <eagler/netplay/AdonisTiming.hpp>
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
    bool ApplyMeasuredTiming(unsigned delay,unsigned prediction);
    bool PreparingWorld()const{return configured_&&setup_.version>=6&&setup_.adonis_mode!=0;}
    void Clear() noexcept;
    bool Configured()const{return configured_;}
    bool CanStart()const{return configured_&&!retired_&&(spectator_?spectator_timing_ready_:playback_||gate_.CanStart());}
    const Netplay::SessionConfig& Config()const{return gate_.Config();}
    const SessionSetup& Setup()const{return setup_;}
    std::uint32_t NextFrame()const{return next_;}
    std::uint32_t NextCaptureFrame()const{return next_capture_;}
    std::uint32_t InputDelay()const{return setup_.input_delay;}
    Netplay::AdonisMode Mode()const{return Netplay::AdonisMode(setup_.adonis_mode);}
    bool AllowsRollback()const{return !ReadOnly()&&Mode()!=Netplay::AdonisMode::Delay;}
    // One physical sample per forward simulation tick, including delayed
    // sessions. Sampling again while that tick waits queues stale controls
    // ahead of the negotiated delay and turns a transient stall into latency.
    bool CanCapture()const{return !ReadOnly()&&CanStart()&&!Correcting()&&next_capture_==next_;}
    bool HasCapture(std::uint32_t frame)const{return core_.HasLocalCapture(frame);}
    bool HasLocalFrame(std::uint32_t frame)const{return core_.InputPresent(std::uint8_t(setup_.local_player),frame);}
    std::uint32_t LastFrame()const{return core_.LastSimulatedFrame();}
    std::uint32_t Generation()const{return generation_;}
    bool Retired()const{return retired_;}
    bool Spectator()const{return spectator_;}
    bool Playback()const{return playback_;}
    bool ReadOnly()const{return spectator_||playback_;}
    bool BeginPlayback();
    bool FeedPlayback(std::uint32_t frame,const std::array<Netplay::FrameInput,Netplay::MAX_PLAYERS>&);
    bool BeginSpectator();
    // Only the validated receive-only transport calls this. There is no
    // browser export for injecting an observer frame into the simulation.
    bool FeedSpectator(const Netplay::SpectatorFramePacket&);

    Netplay::SessionPacket SessionPacket(Netplay::SessionPhase phase)const{return gate_.BuildPacket(phase);}
    Netplay::SessionPacketResult ApplySession(const Netplay::SessionPacket& packet){
        return ReadOnly()?Netplay::SessionPacketResult::InvalidPeer:gate_.Apply(packet);
    }
    bool MarkReady();
    bool Ready()const{return !ReadOnly()&&gate_.LocalReady();}

    bool CaptureLocal(std::uint32_t frame,const Netplay::FrameInput&,std::uint8_t route=255);
    bool CaptureLeadInBootstrap(const Netplay::FrameInput&);
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
    bool BeginCorrection(std::uint32_t first,std::uint32_t checkpointSpan=1);
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
    bool apply_spectator_timing(unsigned delay,unsigned prediction,std::uint32_t abi);
    bool receive_frame(std::uint32_t frame)const;
    bool FeedAuthoritative(std::uint32_t frame,const std::array<Netplay::FrameInput,Netplay::MAX_PLAYERS>&);
    Netplay::SessionGate gate_;
    Netplay::RollbackCore core_;
    SessionSetup setup_{};
    std::uint64_t base_session_id_=0;
    std::uint32_t next_=0,next_capture_=0,generation_=0,correction_end_=Netplay::INVALID_FRAME;
    bool configured_=false,retired_=false,world_ready_=false,spectator_=false,playback_=false;
    bool spectator_timing_ready_=true;
};
}
