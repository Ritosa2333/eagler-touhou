#pragma once
// PROPOSED NEW FILE - integration target: src/game/NativeExReturn.hpp
// Source-ready return-credential for the actual EX terminal23 path.
// It mirrors th128::record::NativeOrdinaryReturn without touching that file.
//
// Proof rules (see NativeExtraReturnOwner.hpp, the only issuer):
//  * private constructor; friends are the single issuer and the consumer Menu;
//  * immutable after construction; no public mutator, no free factory;
//  * ready() re-derives every claim from the shared actual objects, so the
//    credential cannot exist unless the terminal record was ACKed, the EX
//    full history is one stage 16/rank 4 NativeRunReplay, and the 292-byte
//    fresh envelope was finished completed (selector 23) with the same score.
#include "NativeFreshReplayRecord.hpp"
#include "NativePostrunRecord.hpp"
#include "NativeRunReplay.hpp"
#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <memory>
#include <vector>

namespace th128::postrun {class Menu;}

namespace th128::record {

class NativeExtraReturnOwner;

// One actual EX terminal return. settledSelector is 23 (route 6 + 17), the
// value NativeFreshReplayRecord::finish writes at envelope offset 36+0x68
// when completed=true, and the selector the debug terminal requires before
// th128_world_debug_postrun_retire may run.
struct ExtraTerminalIdentity {
    unsigned generation = 0;      // worldGeneration at retirement
    unsigned epoch = 0;           // debug record prefix epoch (caller-owned)
    unsigned stage = 0;           // 16
    unsigned route = 0;           // 6
    unsigned rank = 0;            // 4
    unsigned continues = 0;       // 0 for EX
    unsigned settledSelector = 0; // 23
    unsigned nativeFrames = 0;    // NativeRunReplay::frames()
    unsigned worldFrame = 0;      // world frame at retirement (diagnostic only)
    std::uint32_t recordDigest = 0; // NativePostrunRecord::digest() after ACK
};

class NativeExReturn {
    friend class NativeExtraReturnOwner;
    friend class th128::postrun::Menu;

    ExtraTerminalIdentity identity_{};
    std::shared_ptr<NativePostrunRecord> record_;
    std::shared_ptr<NativeRunReplay> replay_;
    std::shared_ptr<NativeFreshReplayRecord> envelope_;
    std::vector<std::uint8_t> profile_;
    std::int32_t score_ = 0;
    double numerator_ = 0;
    double denominator_ = 0;
    bool consumed_ = false;

    static std::uint32_t word(const std::array<std::uint8_t, 292>& bytes, unsigned offset) noexcept {
        return std::uint32_t(bytes[offset])
            | (std::uint32_t(bytes[offset + 1]) << 8)
            | (std::uint32_t(bytes[offset + 2]) << 16)
            | (std::uint32_t(bytes[offset + 3]) << 24);
    }

    NativeExReturn(ExtraTerminalIdentity identity,
                   std::shared_ptr<NativePostrunRecord> record,
                   std::shared_ptr<NativeRunReplay> replay,
                   std::shared_ptr<NativeFreshReplayRecord> envelope,
                   std::vector<std::uint8_t> profile,
                   std::int32_t score, double numerator, double denominator)
        : identity_(identity), record_(std::move(record)), replay_(std::move(replay)), envelope_(std::move(envelope)),
          profile_(std::move(profile)), score_(score),
          numerator_(numerator), denominator_(denominator) {}

public:
    static constexpr unsigned kStage = 16;
    static constexpr unsigned kRoute = 6;
    static constexpr unsigned kRank = 4;
    static constexpr unsigned kSelector = 23;
    static constexpr unsigned kProfileBytes = 52232;
    static constexpr unsigned kEnvelopeBytes = 292;

    NativeExReturn(const NativeExReturn&) = delete;
    NativeExReturn& operator=(const NativeExReturn&) = delete;
    NativeExReturn(NativeExReturn&&) = default;
    NativeExReturn& operator=(NativeExReturn&&) = default;

    const ExtraTerminalIdentity& identity() const noexcept { return identity_; }
    unsigned generation() const noexcept { return identity_.generation; }
    unsigned epoch() const noexcept { return identity_.epoch; }
    unsigned frames() const noexcept { return identity_.nativeFrames; }
    std::uint32_t recordDigest() const noexcept { return identity_.recordDigest; }
    std::int32_t score() const noexcept { return score_; }
    double numerator() const noexcept { return numerator_; }
    double denominator() const noexcept { return denominator_; }
    const std::vector<std::uint8_t>& profile() const noexcept { return profile_; }
    const std::shared_ptr<NativeRunReplay>& replay() const noexcept { return replay_; }
    const std::shared_ptr<NativeFreshReplayRecord>& envelope() const noexcept { return envelope_; }
    bool consumed() const noexcept { return consumed_; }

    // Everything below is re-derived from the shared actual objects; a forged
    // or partially retained credential cannot satisfy it.
    bool ready() const noexcept {
        if (consumed_ || !identity_.generation || !identity_.epoch
            || identity_.stage != kStage || identity_.route != kRoute || identity_.rank != kRank
            || identity_.settledSelector != kSelector || identity_.continues != 0
            || !identity_.nativeFrames
            || profile_.size() != kProfileBytes || !record_ || !replay_ || !envelope_)
            return false;
        if (!record_->bound() || !record_->acknowledged() || record_->digest()!=identity_.recordDigest
            || record_->profile()!=profile_ || identity_.worldFrame!=identity_.nativeFrames
            || envelope_->input().frame()!=int(identity_.nativeFrames)) return false;
        if (!replay_->configuredFor(kStage) || replay_->stages() != 1
            || replay_->route() != kRoute || replay_->rank() != kRank
            || replay_->frames() != identity_.nativeFrames)
            return false;
        if (envelope_->mode() != 0 || envelope_->stage() != kStage
            || envelope_->physicalRoute() != kRoute || !envelope_->active())
            return false;
        const auto& raw = envelope_->bytes();
        if (!std::isfinite(std::bit_cast<float>(word(raw, 36 + 0x54)))) return false; // slowdown
        return score_ >= 0 && score_ <= 999999999
            && std::isfinite(numerator_) && numerator_ >= 0
            && std::isfinite(denominator_) && denominator_ > 0;
    }
};

} // namespace th128::record
