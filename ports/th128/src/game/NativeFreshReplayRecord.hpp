#pragma once
#include "NativeReplayInput.hpp"
#include <algorithm>
#include <array>
#include <bit>
#include <cstdint>
#include <span>
#include <stdexcept>

namespace th128::record {

struct NativeFreshReplayContext {
    unsigned mode;
    unsigned stage;
    unsigned rank;
    unsigned seed;
    unsigned newStage;
    unsigned shotGroup;
    std::int32_t score;
    std::int32_t pointValue;
    std::int32_t motivation;
    std::int32_t perfectCharge;
    float frozenArea;
};

struct NativeOrdinaryFreshReplayContext {
    NativeFreshReplayContext record;
    unsigned physicalRoute;
};

class NativeFreshReplayRecord {
    std::array<std::uint8_t, 292> bytes_{};
    NativeReplayInput input_;
    unsigned mode_;
    unsigned stage_;
    unsigned route_=6;
    bool active_ = false;

    static void store16(std::span<std::uint8_t> bytes, std::size_t offset, std::uint16_t value) {
        bytes[offset] = static_cast<std::uint8_t>(value);
        bytes[offset + 1] = static_cast<std::uint8_t>(value >> 8);
    }

    static void store32(std::span<std::uint8_t> bytes, std::size_t offset, std::uint32_t value) {
        for (unsigned byte = 0; byte < 4; ++byte)
            bytes[offset + byte] = static_cast<std::uint8_t>(value >> (byte * 8));
    }

    static std::array<std::uint8_t, 60> originalDefaultConfiguration() {
        std::array<std::uint8_t, 60> configuration{};
        store32(configuration, 0, 0x128001);
        const std::array<std::uint16_t, 9> mapping{0, 1, 3, 2, 4, 65535, 65535, 65535, 65535};
        for (std::size_t index = 0; index < mapping.size(); ++index)
            store16(configuration, 4 + index * 2, mapping[index]);
        store32(configuration, 0x16, 0x2580258);
        store32(configuration, 0x1a, 0x10100);
        store16(configuration, 0x1f, 0x6402);
        configuration[0x21] = 0x50;
        store16(configuration, 0x22, 0x200);
        store32(configuration, 0x28, 0x80000000);
        store32(configuration, 0x2c, 0x80000000);
        store32(configuration, 0x38, 0x100);
        return configuration;
    }

    void initialize(const NativeFreshReplayContext& context) {
        store32(bytes_, 0, 0x72383231);
        store16(bytes_, 4, 2);
        store32(bytes_, 0x10, 0x100);
        const auto configuration = originalDefaultConfiguration();
        std::copy(configuration.begin(), configuration.end(), bytes_.begin() + 36 + 0x18);
        store32(bytes_, 36 + 0x5c, route_);
        store32(bytes_, 36 + 0x64, context.rank);
        constexpr std::size_t stage = 36 + 112;
        store16(bytes_, stage, static_cast<std::uint16_t>(context.stage));
        store16(bytes_, stage + 2, static_cast<std::uint16_t>(context.seed));
        store32(bytes_, stage + 0xc, std::bit_cast<std::uint32_t>(context.score));
        store16(bytes_, stage + 0x10, static_cast<std::uint16_t>(context.shotGroup));
        store32(bytes_, stage + 0x14, std::bit_cast<std::uint32_t>(context.pointValue));
        std::uint32_t initialWord = 0;
        for (unsigned index = 0; index < 20; ++index) {
            store32(bytes_, stage + 0x30 + index * 4, initialWord);
            initialWord -= 0x21522153u;
        }
        store32(bytes_, stage + 0x80, std::bit_cast<std::uint32_t>(context.motivation));
        store32(bytes_, stage + 0x84, std::bit_cast<std::uint32_t>(context.perfectCharge));
        store32(bytes_, stage + 0x88, std::bit_cast<std::uint32_t>(context.frozenArea));
        store32(bytes_, stage + 0x8c, context.newStage & 1u);
    }

public:
    explicit NativeFreshReplayRecord(const NativeFreshReplayContext& context)
        : mode_(context.mode), stage_(context.stage) {
        if (context.mode != 0 || context.stage != 16 || context.rank != 4
            || context.seed > 65535 || context.newStage != 1 || context.shotGroup > 15
            || !std::isfinite(context.frozenArea) || context.frozenArea < 0)
            throw std::runtime_error("fresh replay owner outside original owned EX domain");
        initialize(context);
    }

    explicit NativeFreshReplayRecord(const NativeOrdinaryFreshReplayContext& ordinary)
        :mode_(ordinary.record.mode),stage_(ordinary.record.stage),route_(ordinary.physicalRoute) {
        static constexpr std::array<unsigned,6> terminalStages{3,5,8,10,13,15};
        const auto& context=ordinary.record;
        if(route_>=terminalStages.size()||context.stage!=terminalStages[route_]||context.rank>=4
            ||context.mode!=0||context.seed>65535||context.newStage!=1||context.shotGroup>15
            ||!std::isfinite(context.frozenArea)||context.frozenArea<0)
            throw std::runtime_error("ordinary fresh replay route/stage/rank owner rejected");
        initialize(context);
    }

    void activate(std::int32_t horizontal, std::int32_t vertical, bool focused) {
        if (active_) throw std::runtime_error("fresh replay input stage already active");
        input_.beginStageBuffers(stage_);
        constexpr std::size_t stage = 36 + 112;
        store32(bytes_, stage + 0x18, std::bit_cast<std::uint32_t>(horizontal));
        store32(bytes_, stage + 0x1c, std::bit_cast<std::uint32_t>(vertical));
        store32(bytes_, stage + 0x24, focused ? 1u : 0u);
        active_ = true;
    }

    bool record(const NativeReplayInputContext& context) {
        if (!active_) throw std::runtime_error("fresh replay input stage is not active");
        return input_.append(context);
    }

    void finish(std::uint64_t timestamp,std::int32_t score,double numerator,double denominator,
                bool completed,unsigned currentStage) {
        if(mode_!=0||timestamp>0x793406fffull||!std::isfinite(numerator)||numerator<0
            ||!std::isfinite(denominator)||denominator<=0||currentStage>255)
            throw std::runtime_error("original replay finish context rejected");
        const auto ratio=static_cast<float>(numerator/denominator);
        const auto slowdown=static_cast<float>(100.0-100.0*static_cast<double>(ratio));
        if(!std::isfinite(slowdown))throw std::runtime_error("original replay slowdown is not finite");
        for(unsigned byte=0;byte<8;++byte)bytes_[36+0xc+byte]=std::uint8_t(timestamp>>(byte*8));
        store32(bytes_,36+0x14,std::bit_cast<std::uint32_t>(score));
        store32(bytes_,36+0x54,std::bit_cast<std::uint32_t>(slowdown));
        store32(bytes_,36+0x68,completed?route_+17u:currentStage);
    }

    unsigned physicalRoute() const noexcept { return route_; }
    unsigned mode() const noexcept { return mode_; }
    unsigned stage() const noexcept { return stage_; }
    bool active() const noexcept { return active_; }
    const NativeReplayInput& input() const noexcept { return input_; }
    const std::array<std::uint8_t, 292>& bytes() const noexcept { return bytes_; }
};

}
