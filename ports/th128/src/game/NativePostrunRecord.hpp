#pragma once
#include "NativeRecordAccounting.hpp"
#include "NativeScoreProfile.hpp"
#include "NativeBstCodec.hpp"
#include <cmath>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <vector>

namespace th128::record {

class NativePostrunRecord {
    std::uint64_t capability_ = 0;
    bool bound_ = false, preparing_ = false, prepared_ = false, acknowledged_ = false;
    std::vector<std::uint8_t> profile_, assembled_, container_;
    std::uint32_t digest_ = 0;
    double refreshedAnchor_ = 0;

    void requireCapability(std::uint64_t capability) const {
        if (!bound_ || !capability || capability != capability_)
            throw std::runtime_error("postrun storage capability is unavailable");
    }

public:
    void bind(std::uint64_t capability, bool debugTainted, bool normalClear, bool readOnly) {
        if (bound_ || !capability || !debugTainted || normalClear || readOnly)
            throw std::runtime_error("postrun storage binding policy rejected");
        capability_ = capability;
        bound_ = true;
    }

    void prepare(std::uint64_t capability, std::span<const std::uint8_t> profile,
                 double anchor, double sampled, double refreshed,
                 std::uint32_t sessionField74, std::int32_t requestedScene) {
        requireCapability(capability);
        if (preparing_ || prepared_ || acknowledged_ || profile.size() != 52232
            || sessionField74 != 0 || (requestedScene != 15 && requestedScene != 16)
            || !std::isfinite(anchor) || !std::isfinite(sampled) || !std::isfinite(refreshed)
            || anchor < 0 || sampled < anchor || refreshed < sampled)
            throw std::runtime_error("postrun record timing or lifecycle rejected");
        preparing_ = true;
        profile_.assign(profile.begin(), profile.end());
        NativeRecordAccounting::addPlaytime(profile_, sampled - anchor,
                                            sessionField74, requestedScene);
        refreshedAnchor_ = refreshed;
        assembled_ = NativeScoreProfile::assemble(profile_);
        const auto compressed = NativeBstCodec::encode(assembled_);
        container_ = NativeScoreContainer::wrap(compressed, static_cast<std::uint32_t>(assembled_.size()));
        digest_ = 2166136261u;
        for (const auto byte : container_) {
            digest_ ^= byte;
            digest_ *= 16777619u;
        }
        prepared_ = true;
        preparing_ = false;
    }

    void acknowledge(std::uint64_t capability, std::uint32_t serial,
                     std::uint32_t bytes, std::uint32_t digest) {
        requireCapability(capability);
        if (!prepared_ || preparing_ || acknowledged_ || serial != 1
            || bytes != container_.size() || digest != digest_)
            throw std::runtime_error("postrun durable acknowledgement identity rejected");
        acknowledged_ = true;
    }

    bool bound() const noexcept { return bound_; }
    bool preparing() const noexcept { return preparing_; }
    bool prepared() const noexcept { return prepared_; }
    bool acknowledged() const noexcept { return acknowledged_; }
    std::uint32_t serial() const noexcept { return prepared_ ? 1 : 0; }
    std::uint32_t digest() const noexcept { return digest_; }
    double refreshedAnchor() const noexcept { return refreshedAnchor_; }
    const std::vector<std::uint8_t>& profile() const noexcept { return profile_; }
    const std::vector<std::uint8_t>& assembled() const noexcept { return assembled_; }
    const std::vector<std::uint8_t>& container() const noexcept { return container_; }
};

}
