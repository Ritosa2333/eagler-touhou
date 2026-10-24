#pragma once
#include <cstdint>
#include <limits>

namespace th15::multiplayer {
// Host-owned request state. Tokens survive completion until the next request;
// a completed save never holds the simulation while JavaScript syncs IDBFS.
class PendingSave {
public:
    enum Status : int { Stale=-2, Failed=-1, Pending=0, Completed=1 };
    std::uint32_t Request() noexcept {
        if (status_==Pending) return token_;
        if (serial_==std::numeric_limits<std::uint32_t>::max()) return 0;
        token_=++serial_;status_=Pending;return token_;
    }
    Status Poll(std::uint32_t token) const noexcept {
        return token&&token==token_?status_:Stale;
    }
    bool Waiting() const noexcept { return status_==Pending; }
    bool Cancel(std::uint32_t token) noexcept {
        if (Poll(token)!=Pending) return false;
        status_=Failed;return true;
    }
    void Fail() noexcept { if (Waiting()) status_=Failed; }
    void Clear() noexcept { token_=0;status_=Stale; }
    template<class Runtime,class Save>
    Status CompleteForRuntime(bool started,bool beforeSimulation,const Runtime& runtime,
                              std::uint32_t retained,std::uint32_t journalFrames,Save&& save) {
        // The room gate can still be waiting for a peer before frame zero.
        // Only that untouched prefix needs no input confirmation. The owner
        // supplies a sticky pre-simulation flag; rewinding to zero cannot
        // recreate it. Retained game/journal state is still fenced by Complete.
        constexpr auto invalid=std::numeric_limits<std::uint32_t>::max();
        const bool empty=started&&beforeSimulation&&runtime.Configured()&&
            !runtime.ReadOnly()&&!runtime.Retired()&&!*runtime.Error()&&
            !runtime.Correcting()&&runtime.NextFrame()==0&&runtime.CaptureFrame()==0&&
            runtime.LastFrame()==invalid&&runtime.CommittedThrough()==invalid&&
            runtime.RollbackFrame()==invalid;
        return Complete(started&&runtime.AllowsRollback(),runtime.Correcting(),
                        runtime.InputsReconciledBeforeNext()||empty,retained,journalFrames,save);
    }
    template<class Save>
    Status Complete(bool rollback,bool correcting,bool reconciled,
                    std::uint32_t retained,std::uint32_t journalFrames,Save&& save) {
        if (Waiting()&&(!rollback||(!correcting&&reconciled&&!retained&&!journalFrames)))
            status_=save()?Completed:Failed;
        return status_;
    }
private:
    std::uint32_t serial_=0,token_=0;
    Status status_=Stale;
};
}
