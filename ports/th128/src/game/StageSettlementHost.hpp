#pragma once
#include "animation/AnmScene.hpp"
#include <array>
#include <cstdint>
#include <functional>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace th128::stage {

inline constexpr int kStageSettlementAnmId = 0x52;
inline constexpr int kStageSettlementLayer = 0x17;
inline constexpr std::uint32_t kStageSettlementScoreCap = 0x3b9ac9ffu;
inline constexpr std::uint32_t kStageSettlementScoreClamp = 0x3b9aca00u;
inline constexpr std::uint32_t kStageSettlementManagerFlags = 0x5a04u;
inline constexpr std::uint32_t kStageSettlementManagerBase = 0x5a38u;
inline constexpr std::uint32_t kStageSettlementManagerHandle = 0x58ccu;
inline constexpr std::uint32_t kStageSettlementOwnerFlagOffset = 0x438ecu;
inline constexpr std::uint32_t kStageSettlementOwnerFlagMask = 0x10u;
inline constexpr std::uint32_t kStageSettlementOwnerFinishOffset = 0x438f0u;
inline constexpr std::uint32_t kStageSettlementOwnerGroupCount = 8;
inline constexpr std::uint32_t kStageSettlementOwnerSlotsPerGroup = 2;

struct StageSettlementManager {
    std::uint32_t handle58cc = 0;
    std::uint32_t flags5a04 = 0;
    std::uint32_t field5a08 = 0;
    std::uint32_t field5a0c = 0;
    float field5a10 = 0;
    std::uint32_t field5a14 = 0;
    std::uint32_t field5a18 = 0;
    std::int32_t baseScore5a38 = 0;
};

struct StageSettlementContext {
    std::int32_t stage = 0;
    std::int32_t score = 0;
    StageSettlementManager manager;
};

struct StageSettlementCallbacks {
    using Visit = std::function<bool(std::uint32_t)>;
    using RetireTree = std::function<bool(std::uint32_t,std::uint32_t,int,std::uint32_t&,std::string&)>;
    std::function<bool(std::string&)> setClearFlag;
    std::function<std::uint32_t(std::uint32_t, std::uint32_t)> slot;
    RetireTree retireTree;
    std::function<bool(std::uint32_t, std::uint32_t, const Visit&, std::string&)> walk;
    std::function<bool(std::uint32_t, std::string&)> retire;
    std::function<bool(std::string&)> finish;
};

struct StageSettlementState {
    bool loaded = false;
    bool begun = false;
    bool failed = false;
    bool scalarApplied = false;
    bool presentationBound = false;
    bool cleanupCalled = false;
    bool presentationComplete = false;
    bool stageTransitionClaimed = false;
    std::uint32_t ownerSlotsVisited = 0;
    std::uint32_t retiredEntities = 0;
    std::uint32_t presentationTicks = 0;
    std::uint32_t frontSpawnLayer = 0;
    StageSettlementContext context;
};

struct StageSettlementSnapshot {
    std::uint32_t id = 0;
    std::uint32_t parent = 0;
    anm::State state;
    std::optional<anm::SpriteInfo> sprite;
    float x = 0, y = 0, z = 0;
};

class StageSettlementHost {
public:
    static bool supportedStage(int stage) noexcept;
    static std::int32_t stageBaseScore(int stage) noexcept;

    bool load(std::span<const std::uint8_t> bytes, std::string& error);
    bool begin(const StageSettlementContext& context, const StageSettlementCallbacks& callbacks,
               std::string& error);
    bool beginTerminal(const StageSettlementContext& context, const StageSettlementCallbacks& callbacks,
                       std::string& error);
    bool beginExtraTerminal(const StageSettlementContext& context, const StageSettlementCallbacks& callbacks,
                            std::string& error);
    bool interrupt(int code, std::string& error);
    bool tick(std::string& error);
    StageSettlementState state() const;
    std::vector<StageSettlementSnapshot> snapshots() const;

private:
    anm::Scene scene_;
    StageSettlementContext context_;
    bool loaded_ = false, begun_ = false, executing_ = false;
    bool scalarApplied_ = false, presentationBound_ = false, cleanupCalled_ = false;
    bool presentationComplete_ = false;
    std::uint32_t ownerSlotsVisited_ = 0, retiredEntities_ = 0, presentationTicks_ = 0;
    std::uint32_t frontId_ = 0, frontSpawnLayer_ = 0;
    std::string failure_;
    bool beginScoped(const StageSettlementContext& context, const StageSettlementCallbacks& callbacks,
                     bool terminal, std::string& error);
    void runtime();
    bool fail(std::string reason, std::string& error);
    bool checked(std::string& error);
    void abandon();
};

}
