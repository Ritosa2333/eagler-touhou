#pragma once
#include <eagler/netplay/SessionChannel.hpp>

namespace th09::multiplayer {
// Network state never belongs to a world checkpoint. Restoring gameplay must
// preserve received input, local capture ownership, ACKs and transport clocks.
class RollbackSession : private Netplay::PeerTransport {
public:
    static constexpr unsigned History = 8;
    explicit RollbackSession(Netplay::PeerTransport& transport): transport_(transport), channel_(*this) {}
    bool Begin(const Netplay::SessionConfig&, std::uint64_t now);
    void Clear();
    bool Verify(std::uint32_t frame,std::uint32_t hash);
    bool Pump(std::uint64_t now, bool expectsInput = true);
    bool NeedsCapture() const;
    bool Capture(const Netplay::FrameInput&, std::uint64_t now);
    Netplay::FrameDecision Prepare() const;
    bool Complete(const Netplay::FrameDecision&);
    // Call only AFTER the title restored the state before RollbackFrame().
    // Resimulation uses Prepare/Complete and cannot invoke Capture.
    bool Restored(std::uint32_t frame);
    bool Ready() const { return configured_ && gate_.CanStart() && !Failed(); }
    bool Failed() const override { return failed_ || invalidInput_ || transport_.Failed(); }
    const char* Error() const { return error_; }
    std::uint32_t Frame() const { return next_; }
    std::uint32_t RollbackFrame() const { return core_.RollbackFrame(); }
    std::uint32_t ConfirmedThrough() const;
    bool ConfirmedInputs(std::uint32_t frame, std::array<Netplay::FrameInput, Netplay::MAX_PLAYERS>* out) const;
    bool InputsReconciledBeforeNext() const { const auto c=core_.ConfirmedThroughAllRemotes();return !core_.HasRollbackRequest()&&(next_==0||(c!=Netplay::INVALID_FRAME&&c>=next_-1)); }
    void FinishExactBoundary(){if(InputsReconciledBeforeNext())replayEnd_=next_;}
    bool CanRetire() const;
    bool Retire(std::uint64_t now);
    double IntervalScale() const { return channel_.IntervalScale(); }
    const Netplay::SessionChannel& Channel() const { return channel_; }
    std::uint32_t Captures() const { return captures_; }
    std::uint32_t Corrections() const { return corrections_; }
    std::uint32_t Resimulated() const { return resimulated_; }
    static bool ValidInput(const Netplay::FrameInput&);
private:
    struct HashSlot {std::uint32_t frame=Netplay::INVALID_FRAME,local=0,remote=0;bool hasLocal=false,hasRemote=false;};
    std::array<HashSlot,64> hashes_{};
    Netplay::PeerTransport& transport_;
    Netplay::RollbackCore core_;
    Netplay::SessionGate gate_;
    Netplay::SessionChannel channel_;
    std::uint32_t next_ = 0, replayEnd_ = 0, captures_ = 0, corrections_ = 0, resimulated_ = 0;
    bool configured_ = false, failed_ = false, invalidInput_ = false;
    const char* error_ = "";
    bool Fail(const char* error) { failed_ = true; error_ = error; return false; }
    bool IsOpen() const override { return transport_.IsOpen(); }
    bool SendTo(std::uint8_t p, const std::uint8_t* b, std::size_t n) override { return transport_.SendTo(p,b,n); }
    bool SendRepairTo(std::uint8_t p, const std::uint8_t* b, std::size_t n) override { return transport_.SendRepairTo(p,b,n); }
    bool SendControl(const std::uint8_t* b, std::size_t n) override { return transport_.SendControl(b,n); }
    bool Poll(std::vector<std::uint8_t>*) override;
    std::size_t BufferedAmount() const override { return transport_.BufferedAmount(); }
};
}
