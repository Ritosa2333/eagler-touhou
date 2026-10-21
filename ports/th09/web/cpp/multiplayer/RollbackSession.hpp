#pragma once
#include <eagler/netplay/SessionChannel.hpp>
#include <eagler/netplay/AdonisStartup.hpp>
#include <deque>
#include "ChargeHistory.hpp"

namespace th09::multiplayer {
// Network state never belongs to a world checkpoint. Restoring gameplay must
// preserve received input, local capture ownership, ACKs and transport clocks.
class RollbackSession : private Netplay::PeerTransport {
public:
    static constexpr unsigned History = 8;
    explicit RollbackSession(Netplay::PeerTransport& transport): transport_(transport), channel_(*this) {}
    bool Begin(const Netplay::SessionConfig&, std::uint64_t now, std::uint8_t inputDelay = 0,
               Netplay::AdonisMode mode = Netplay::AdonisMode::Rollback);
    bool BeginMeasured(const Netplay::SessionConfig&,std::uint64_t now,std::uint32_t requestedDelay,
                       Netplay::AdonisMode mode,unsigned predictionReserve=2);
    const Netplay::AdonisStartup& Startup() const {return startup_;}
    void Clear();
    bool Verify(std::uint32_t frame,std::uint32_t hash);
    bool Pump(std::uint64_t now, bool expectsInput = true, std::uint64_t measurementNowUs = 0,
              bool deferOrdinaryInput = false);
    bool FlushDeferredInputs(std::uint64_t now);
    bool NeedsCapture() const;
    bool Capture(const Netplay::FrameInput&, std::uint64_t now);
    bool SetLocalChargePriority(bool enabled){if(configured_||measuring_)return false;chargeRequested_=enabled;return true;}
    bool LocalChargePriority()const{return charges_.Enabled();}
    const ChargeSample* ChargeInput(u32 side,u32 frame)const{return charges_.Get(side,frame);}
    bool CaptureLocalCharge(u32 frame,const ChargeSample& sample);
    bool CommitChargeFrame(u32 frame,const std::array<ChargeSample,2>& samples);
    // The opt-in visible pass may advance once on an unreconciled base, as in
    // Higan's live_tick. It remains speculative until post-Present recovery.
    Netplay::FrameDecision Prepare(bool beforeRecovery=false) const;
    bool Complete(const Netplay::FrameDecision&,bool beforeRecovery=false);
    // Call only AFTER the title restored the state before RollbackFrame().
    // Resimulation uses Prepare/Complete and cannot invoke Capture.
    bool Restored(std::uint32_t frame);
    bool Ready() const { return configured_ && gate_.CanStart() && !Failed(); }
    bool Failed() const override { return failed_ || invalidInput_ || transport_.Failed(); }
    const char* Error() const { return error_; }
    std::uint32_t Frame() const { return next_; }
    std::uint32_t RollbackFrame() const { const auto a=core_.RollbackFrame(),b=charges_.RollbackFrame();return a<b?a:b; }
    std::uint32_t ConfirmedThrough() const;
    bool ConfirmedInputs(std::uint32_t frame, std::array<Netplay::FrameInput, Netplay::MAX_PLAYERS>* out) const;
    bool InputsReconciledBeforeNext() const { const auto c=core_.ConfirmedThroughAllRemotes();return !HasRollback()&&(!charges_.Enabled()||charges_.ConfirmedCount()>=next_)&&(next_==0||(c!=Netplay::INVALID_FRAME&&c>=next_-1)); }
    void FinishExactBoundary(){if(InputsReconciledBeforeNext())replayEnd_=next_;}
    bool CanRetire() const;
    bool Retire(std::uint64_t now);
    double IntervalScale() const;
    // Keep the diagnostic slot stable. Missing input now uses the hard history
    // bound without slowing every fresh local sample before that bound.
    double NetworkIntervalScale() const { return 1; }
    std::uint32_t PredictionFrontier() const { return next_ > replayEnd_ ? next_ : replayEnd_; }
    std::uint32_t RemoteInputFrontier() const;
    std::uint32_t MissingRemoteInputs() const;
    double PacedElapsedMs(double elapsedMs);
    Netplay::AdonisMode Mode() const { return mode_; }
    unsigned InputDelay() const { return inputDelay_; }
    const Netplay::SessionChannel& Channel() const { return channel_; }
    std::uint32_t Captures() const { return captures_; }
    std::uint32_t Corrections() const { return corrections_; }
    std::uint32_t Resimulated() const { return resimulated_; }
    static bool ValidInput(const Netplay::FrameInput&);
private:
    ChargeHistory charges_;bool chargeRequested_=false;
    std::uint64_t chargeRepairAt_=0;
    bool HasRollback()const{return RollbackFrame()!=Netplay::INVALID_FRAME;}
    bool Configure(const Netplay::SessionConfig&,std::uint64_t now,std::uint8_t delay,
                   Netplay::AdonisMode mode,unsigned predictionReserve=0);
    bool PumpStartup(std::uint64_t now, std::uint64_t measurementNowUs);
    Netplay::AdonisStartup startup_;
    Netplay::SessionConfig startupSession_{};
    std::uint64_t startupNowUs_=0;
    bool measuring_=false,measured_=false;
    std::deque<std::vector<std::uint8_t>> startupPending_;
    struct HashSlot {std::uint32_t frame=Netplay::INVALID_FRAME,local=0,remote=0;bool hasLocal=false,hasRemote=false;};
    std::array<HashSlot,64> hashes_{};
    Netplay::PeerTransport& transport_;
    Netplay::RollbackCore core_;
    Netplay::SessionGate gate_;
    Netplay::SessionChannel channel_;
    std::uint32_t next_ = 0, replayEnd_ = 0, captures_ = 0, corrections_ = 0, resimulated_ = 0;
    bool configured_ = false, failed_ = false, invalidInput_ = false;
    Netplay::AdonisMode mode_ = Netplay::AdonisMode::Rollback;
    std::uint8_t inputDelay_ = 0;
    double phaseDebtMs_ = 0;
    const char* error_ = "";
    bool Fail(const char* error) { failed_ = true; error_ = error; return false; }
    bool IsOpen() const override { return transport_.IsOpen(); }
    bool Recovering() const override { return transport_.Recovering(); }
    bool Disconnected() const override { return transport_.Disconnected(); }
    bool CalibrationSuspended() const override { return transport_.CalibrationSuspended(); }
    bool SendTo(std::uint8_t p, const std::uint8_t* b, std::size_t n) override { return transport_.SendTo(p,b,n); }
    bool SendRepairTo(std::uint8_t p, const std::uint8_t* b, std::size_t n) override { return transport_.SendRepairTo(p,b,n); }
    bool SendControl(const std::uint8_t* b, std::size_t n) override { return transport_.SendControl(b,n); }
    bool Poll(std::vector<std::uint8_t>*) override;
    std::size_t BufferedAmount() const override { return transport_.BufferedAmount(); }
};
}
