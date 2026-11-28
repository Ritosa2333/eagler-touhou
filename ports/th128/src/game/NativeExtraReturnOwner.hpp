#pragma once
// PROPOSED NEW FILE - integration target: src/game/NativeExtraReturnOwner.hpp
// The single issuer of th128::record::NativeExReturn.  It is the EX analogue of
// th128::record::NativeOrdinaryTerminalOwner (binding + sealing + one-shot
// retirement) and of th128::ending::Runtime15 (the only ordinary minter), but
// it is deliberately NOT a free factory: it has no Context/boolean overload,
// it must first be bound to the actual objects, mint() refuses to run unless
// the live th128::stage::Control is still at the 7/16 pre-transition state,
// commit() refuses to seal unless the very same Control reports the terminal16
// transition Applied with current==4 && pending==4 && menuMode==3, and any
// failure latches failed_ so the credential can never be minted afterwards.
#include "NativeExReturn.hpp"
#include "NativePostrunRecord.hpp"
#include "StageControl.hpp"
#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>

namespace th128::record {

// Bound once, after the EX terminal23 record exists, from the same shared
// objects the World owns (postrun record + full-run history + fresh envelope).
struct ExtraTerminalBinding {
    ExtraTerminalIdentity identity{};
    std::shared_ptr<NativePostrunRecord> record;
    std::shared_ptr<NativeRunReplay> replay;
    std::shared_ptr<NativeFreshReplayRecord> envelope;
    std::int32_t score = 0;
    double numerator = 0;
    double denominator = 0;
};

class NativeExtraReturnOwner {
    bool bound_ = false, minted_ = false, sealed_ = false, failed_ = false, retired_=false, retiring_=false;
    const th128::stage::Control* control_=nullptr;
    ExtraTerminalIdentity identity_{};
    std::shared_ptr<NativePostrunRecord> record_;
    std::shared_ptr<NativeRunReplay> replay_;
    std::shared_ptr<NativeFreshReplayRecord> envelope_;
    std::int32_t score_ = 0;
    double numerator_ = 0, denominator_ = 0;
    std::string failure_;

    static std::uint32_t word(const std::array<std::uint8_t, 292>& bytes, unsigned offset) noexcept {
        return std::uint32_t(bytes[offset])
            | (std::uint32_t(bytes[offset + 1]) << 8)
            | (std::uint32_t(bytes[offset + 2]) << 16)
            | (std::uint32_t(bytes[offset + 3]) << 24);
    }

    // Re-derives the whole EX domain from the shared actual objects.  Called by
    // bind(), mint() (before and after the control probe) and commit().
    bool actual() const noexcept {
        if (!record_ || !replay_ || !envelope_) return false;
        if (!identity_.generation || !identity_.epoch
            || identity_.stage != NativeExReturn::kStage || identity_.route != NativeExReturn::kRoute
            || identity_.rank != NativeExReturn::kRank || identity_.settledSelector != NativeExReturn::kSelector
            || identity_.continues != 0 || !identity_.nativeFrames)
            return false;
        if (!record_->bound() || !record_->acknowledged() || record_->serial() != 1
            || record_->digest() != identity_.recordDigest
            || record_->profile().size() != NativeExReturn::kProfileBytes
            || identity_.worldFrame!=identity_.nativeFrames || envelope_->input().frame()!=int(identity_.nativeFrames))
            return false;
        if (!replay_->configuredFor(NativeExReturn::kStage) || replay_->stages() != 1
            || replay_->route() != NativeExReturn::kRoute || replay_->rank() != NativeExReturn::kRank
            || replay_->frames() != identity_.nativeFrames)
            return false;
        if (envelope_->mode() != 0 || envelope_->stage() != NativeExReturn::kStage
            || envelope_->physicalRoute() != NativeExReturn::kRoute || !envelope_->active())
            return false;
        const auto& raw = envelope_->bytes();
        if (!std::isfinite(std::bit_cast<float>(word(raw, 36 + 0x54)))) return false;
        return score_ >= 0 && score_ <= 999999999
            && std::isfinite(numerator_) && numerator_ >= 0
            && std::isfinite(denominator_) && denominator_ > 0;
    }

    void latch(std::string& error, const char* reason) {
        failure_ = reason ? reason : "extra return owner failed";
        failed_ = true;
        error = failure_;
    }

public:
    // World-side binding.  Must be called after th128_world_debug_postrun_record
    // op1 (record prepared) + op2 (record ACKed) and after the EX full history
    // and fresh envelope exist, but before th128_world_debug_postrun_retire.
    bool bind(const ExtraTerminalBinding& binding, const th128::stage::Control& control, std::string& error) {
        error.clear();
        if (bound_ || minted_ || sealed_ || failed_) {
            error = "extra return owner is already bound or spent";
            return false;
        }
        if(!control.initialized()||!control.sessionConstructed()||control.gateSet()||control.current()!=7||control.pending()!=16){error="EX binding lost actual terminal controller";return false;}
        control_=&control;
        identity_ = binding.identity;
        record_ = binding.record;
        replay_ = binding.replay;
        envelope_ = binding.envelope;
        score_ = binding.score;
        numerator_ = binding.numerator;
        denominator_ = binding.denominator;
        if (!actual()) {
            record_.reset();
            replay_.reset();
            envelope_.reset();
            latch(error, "extra return binding lacks its acknowledged terminal23 record/history/envelope");
            return false;
        }
        bound_ = true;
        return true;
    }

    // Step 1: called by the World immediately before
    // stageControl.consumeTerminal16(retire, construct).  The returned credential
    // is only a candidate; it is consumed by the Menu ctor and only becomes a
    // proven return once commit() observes the Applied transition.
    bool retire(const th128::stage::Control& control,const std::function<bool()>& release,std::string& error){
        error.clear();if(retiring_){failed_=true;error="EX retirement reentered";return false;}
        if(!bound_||minted_||retired_||failed_||control_!=&control||control.current()!=7||control.pending()!=16||!release||!actual()){error="EX native retirement owner changed";return false;}
        retiring_=true;bool ok=false;try{ok=release();}catch(...){retiring_=false;failed_=true;throw;}
        retiring_=false;if(!ok||failed_||control.current()!=7||control.pending()!=16||!actual()){failed_=true;error="EX actual BattleResources retirement did not settle";return false;}
        retired_=true;return true;
    }
    std::unique_ptr<NativeExReturn> mint(const th128::stage::Control& control, std::string& error) {
        error.clear();
        if (!bound_ || minted_ || sealed_ || failed_ || !retired_ || control_!=&control) {
            error = "extra return owner cannot mint in its current state";
            return {};
        }
        if (!control.initialized() || !control.sessionConstructed() || control.gateSet()
            || control.current() != 7 || control.pending() != 4 || control.menuMode() != 3) {
            latch(error, "extra return mint requires the actual controller7/terminal16 pre-transition owner");
            return {};
        }
        if (!actual()) {
            latch(error, "extra return mint no longer matches its acknowledged terminal23 record");
            return {};
        }
        minted_ = true;
        auto permit = std::unique_ptr<NativeExReturn>(new NativeExReturn(
            identity_, record_, replay_, envelope_, record_->profile(), score_, numerator_, denominator_));
        if (!permit->ready()) {
            permit.reset();
            latch(error, "extra return credential failed its own native proof");
            return {};
        }
        return permit;
    }

    // Step 2: called by the World after consumeTerminal16 returns.  Seals the
    // owner and proves the transition really was the terminal16 -> postrun-menu
    // commit for this same control and these same shared objects.
    bool commit(const th128::stage::Control& control, const th128::stage::CommitResult& result,
                std::string& error) {
        error.clear();
        if (!minted_ || sealed_ || failed_ || !retired_ || control_!=&control) {
            error = "extra return owner has no minted credential to commit";
            return false;
        }
        if (result.status != th128::stage::CommitStatus::Applied
            || result.action.kind != th128::stage::ActionKind::PostrunMenu
            || control.current() != 4 || control.pending() != 4
            || control.menuMode() != 3 || control.gateSet()) {
            latch(error, "extra return commit lacks the applied terminal16 postrun-menu transition");
            return false;
        }
        if (!actual()) {
            latch(error, "extra return commit lost its acknowledged terminal23 record");
            return false;
        }
        sealed_ = true;
        return true;
    }

    bool bound() const noexcept { return bound_; }
    bool minted() const noexcept { return minted_; }
    bool sealed() const noexcept { return sealed_; }
    bool failed() const noexcept { return failed_; }
    const std::string& failure() const noexcept { return failure_; }
    const ExtraTerminalIdentity& identity() const {
        if (!bound_) throw std::runtime_error("extra terminal data owner absent");
        return identity_;
    }
    // The single admissible profile source for the EX Menu Context: the same
    // 52232 bytes the durable record ACKed (prefix profile + real playtime).
    const std::vector<std::uint8_t>& profile() const {
        if (!bound_) throw std::runtime_error("extra terminal data owner absent");
        return record_->profile();
    }
    const std::shared_ptr<NativeRunReplay>& replay() const noexcept { return replay_; }
    const std::shared_ptr<NativeFreshReplayRecord>& envelope() const noexcept { return envelope_; }
};

} // namespace th128::record
