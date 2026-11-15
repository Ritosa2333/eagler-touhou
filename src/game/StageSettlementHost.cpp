#include "StageSettlementHost.hpp"
#include <bit>
#include <limits>
#include <utility>

namespace th128::stage {
namespace {

constexpr std::uint64_t kFrontKey = 1;
constexpr std::uint32_t kFrontOwner = 1;
constexpr std::uint32_t kManagerField5a14 = 0x4b6f58u;
constexpr std::uint32_t kManagerField5a08Initial = 0xfff0bdc1u;
constexpr std::uint32_t kManagerField5a08Final = 0xffffffffu;
constexpr std::uint32_t kManagerFlagStageClear = 0x100u;

constexpr std::array<std::int32_t, 24> kStageBaseScoreTable = {
    0, 1000000, 2000000, 3000000, 2000000, 3000000, 1000000, 2000000,
    3000000, 2000000, 3000000, 1000000, 2000000, 3000000, 2000000, 3000000,
    3000000, 1000, 1000, 1000, 1000, 1000, 10000, 10000};

struct Guard {
    bool& state;
    explicit Guard(bool& value) : state(value) { state = true; }
    ~Guard() { state = false; }
};

std::int32_t wrapped(std::int64_t value) {
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(static_cast<std::uint64_t>(value)));
}

}

bool StageSettlementHost::supportedStage(int stage) noexcept {
    switch (stage) {
    case 1:
    case 2:
    case 4:
    case 6:
    case 7:
    case 9:
    case 11:
    case 12:
    case 14:
        return true;
    default:
        return false;
    }
}

std::int32_t StageSettlementHost::stageBaseScore(int stage) noexcept {
    return stage >= 0 && stage < int(kStageBaseScoreTable.size())
        ? kStageBaseScoreTable[std::size_t(stage)]
        : 0;
}

bool StageSettlementHost::fail(std::string reason, std::string& error) {
    if (failure_.empty()) failure_ = std::move(reason);
    error = failure_;
    return false;
}

void StageSettlementHost::runtime() {
    scene_.runtime = {};
    scene_.runtime.instructionBudget = 4096;
    scene_.runtime.cameraPosition.fill(std::numeric_limits<float>::quiet_NaN());
    scene_.runtime.cameraEyeOffset.fill(std::numeric_limits<float>::quiet_NaN());
    scene_.runtime.gameRandom32 = [this] {
        if (failure_.empty()) failure_ = "unproved stage-clear game RNG operation";
        return 0u;
    };
    scene_.runtime.displayRandom32 = [this] {
        if (failure_.empty()) failure_ = "unproved stage-clear display RNG operation";
        return 0u;
    };
    scene_.runtime.unsupported = [this](int, int script, int opcode, const char* reason) {
        if (failure_.empty())
            failure_ = "front82 script=" + std::to_string(script) + " opcode="
                + std::to_string(opcode) + ": " + reason;
    };
}

void StageSettlementHost::abandon() {
    scene_.removeOwner(kFrontOwner);
    scene_.collect();
}

bool StageSettlementHost::checked(std::string& error) {
    if (!failure_.empty()) return fail(failure_, error);
    const auto bank = scene_.registry.bank(5);
    for (const auto& [id, node] : scene_.nodes()) {
        if (node->vm.faulted() || node->detached || node->customGeometry)
            return fail("unimplemented stage-clear Scene geometry/callback", error);
        const auto& state = node->vm.state();
        if (state.sprite >= 0
            && (!bank || std::size_t(state.sprite) >= bank->sprites.size() || state.spriteBank != 5))
            return fail("stage-clear sprite binding unavailable", error);
        if (state.ended || state.deleteRequested) node->alive = false;
    }
    scene_.collect();
    return true;
}

bool StageSettlementHost::load(std::span<const std::uint8_t> bytes, std::string& error) {
    error.clear();
    if (loaded_ || begun_) {
        error = "stage-clear front resource replacement requires a fresh host";
        return false;
    }
    if (bytes.empty()) {
        error = "stage-clear front resource is empty";
        return false;
    }
    anm::Registry candidate;
    if (!candidate.loadBank(5, bytes.data(), bytes.size(), error)) return false;
    const auto bank = candidate.bank(5);
    if (!bank || bank->scripts.size() <= std::size_t(kStageSettlementAnmId)) {
        error = "actual front82 stage-clear script is required";
        return false;
    }
    scene_.registry = std::move(candidate);
    loaded_ = true;
    runtime();
    return true;
}

bool StageSettlementHost::begin(const StageSettlementContext& context,
                                const StageSettlementCallbacks& callbacks, std::string& error) {
    return beginScoped(context, callbacks, false, error);
}

bool StageSettlementHost::beginTerminal(const StageSettlementContext& context,
                                        const StageSettlementCallbacks& callbacks, std::string& error) {
    if(context.stage==16){error="extra terminal settlement requires its explicit owner";return false;}
    return beginScoped(context, callbacks, true, error);
}

bool StageSettlementHost::beginExtraTerminal(const StageSettlementContext& context,
                                             const StageSettlementCallbacks& callbacks, std::string& error) {
    if(context.stage!=16){error="extra terminal settlement requires stage16";return false;}
    return beginScoped(context, callbacks, true, error);
}

bool StageSettlementHost::beginScoped(const StageSettlementContext& context,
                                      const StageSettlementCallbacks& callbacks, bool terminal,
                                      std::string& error) {
    error.clear();
    if (!failure_.empty()) return fail(failure_, error);
    if (!loaded_) {
        error = "stage-clear front resource is not loaded";
        return false;
    }
    if (begun_) {
        error = "stage-clear settlement already started";
        return false;
    }
    if (executing_) {
        error = "stage-clear settlement re-entry";
        return false;
    }
    const bool terminalStage = context.stage == 3 || context.stage == 5 || context.stage == 8
        || context.stage == 10 || context.stage == 13 || context.stage == 15 || context.stage == 16;
    if (terminal ? !terminalStage : !supportedStage(context.stage)) {
        error = "stage-clear input stage is outside the supported ordinary rows";
        return false;
    }
    if (!callbacks.setClearFlag || !callbacks.slot || !callbacks.finish
        || (!callbacks.retireTree && (!callbacks.walk || !callbacks.retire))) {
        error = "stage-clear owner callbacks are incomplete";
        return false;
    }
    Guard guard(executing_);
    auto* root = scene_.bind(kFrontKey, kFrontOwner, 5, kStageSettlementAnmId,
                             anm::Membership::Primary, 0, 0, {}, false, nullptr, 224, 16,
                             kStageSettlementLayer);
    if (!root) {
        abandon();
        return fail("actual front82 stage-clear binding failed", error);
    }
    begun_ = true;
    presentationBound_ = true;
    frontId_ = root->id;
    frontSpawnLayer_ = kStageSettlementLayer;
    context_ = context;
    context_.manager.handle58cc = frontId_;
    if (!checked(error)) {
        abandon();
        return fail(error.empty() ? "front82 stage-clear execution stopped" : error, error);
    }
    context_.manager.baseScore5a38 = stageBaseScore(context_.stage);
    context_.score = wrapped(std::int64_t(context_.score)
                             + std::int64_t(context_.manager.baseScore5a38 / 10));
    if (context_.score >= std::int32_t(kStageSettlementScoreClamp))
        context_.score = std::int32_t(kStageSettlementScoreCap);
    context_.manager.flags5a04 |= kManagerFlagStageClear;
    if ((context_.manager.field5a18 & 1u) == 0) {
        context_.manager.field5a18 |= 1u;
        context_.manager.field5a10 = 0.0f;
        context_.manager.field5a0c = 0;
        context_.manager.field5a08 = kManagerField5a08Initial;
        context_.manager.field5a14 = kManagerField5a14;
    }
    context_.manager.field5a10 = 0.0f;
    context_.manager.field5a0c = 0;
    context_.manager.field5a08 = kManagerField5a08Final;
    scalarApplied_ = true;
    std::string serviceError;
    if (!callbacks.setClearFlag(serviceError)) {
        abandon();
        return fail(serviceError.empty() ? "stage-clear owner flag write failed" : serviceError, error);
    }
    for (std::uint32_t group = 0; group < kStageSettlementOwnerGroupCount; ++group) {
        for (std::uint32_t slot = 0; slot < kStageSettlementOwnerSlotsPerGroup; ++slot) {
            ++ownerSlotsVisited_;
            const auto entity = callbacks.slot(group, slot);
            if (entity == 0) continue;
            if (callbacks.retireTree) {
                std::uint32_t retired = 0;
                if (!callbacks.retireTree(group, slot, 3, retired, serviceError)) {
                    abandon();
                    return fail(serviceError.empty() ? "stage-clear owner tree retirement failed" : serviceError,
                                error);
                }
                retiredEntities_ += retired;
                continue;
            }
            std::string retireError, walkError;
            bool rejected = false;
            const StageSettlementCallbacks::Visit visit = [&](std::uint32_t id) {
                if (!callbacks.retire(id, retireError)) {
                    rejected = true;
                    return false;
                }
                ++retiredEntities_;
                return true;
            };
            if (!callbacks.walk(group, slot, visit, walkError) || rejected) {
                std::string reason = retireError.empty() ? walkError : retireError;
                if (reason.empty()) reason = "stage-clear owner lifecycle walk failed";
                abandon();
                return fail(std::move(reason), error);
            }
        }
    }
    if (!callbacks.finish(serviceError)) {
        abandon();
        return fail(serviceError.empty() ? "stage-clear owner chain finish failed" : serviceError, error);
    }
    cleanupCalled_ = true;
    return true;
}

bool StageSettlementHost::interrupt(int code, std::string& error) {
    error.clear();
    if (!failure_.empty()) return fail(failure_, error);
    if (!begun_) {
        error = "stage-clear settlement is not started";
        return false;
    }
    if (presentationComplete_) return true;
    if (executing_) {
        error = "stage-clear settlement re-entry";
        return false;
    }
    Guard guard(executing_);
    scene_.interrupt(kFrontKey, code, false);
    return true;
}

bool StageSettlementHost::tick(std::string& error) {
    error.clear();
    if (!loaded_) {
        error = "stage-clear front resource is not loaded";
        return false;
    }
    if (!failure_.empty()) return fail(failure_, error);
    if (!begun_) {
        error = "stage-clear settlement is not started";
        return false;
    }
    if (presentationComplete_) return true;
    if (executing_) {
        error = "stage-clear settlement re-entry";
        return false;
    }
    Guard guard(executing_);
    scene_.tickPrimary();
    ++presentationTicks_;
    if (!checked(error)) {
        abandon();
        return fail(error.empty() ? "front82 stage-clear tick stopped" : error, error);
    }
    if (frontId_ != 0 && !scene_.find(kFrontKey)) {
        scene_.removeOwner(kFrontOwner);
        scene_.collect();
        presentationComplete_ = true;
    }
    return true;
}

StageSettlementState StageSettlementHost::state() const {
    StageSettlementState result;
    result.loaded = loaded_;
    result.begun = begun_;
    result.failed = !failure_.empty();
    result.scalarApplied = scalarApplied_;
    result.presentationBound = presentationBound_;
    result.cleanupCalled = cleanupCalled_;
    result.presentationComplete = presentationComplete_;
    result.ownerSlotsVisited = ownerSlotsVisited_;
    result.retiredEntities = retiredEntities_;
    result.presentationTicks = presentationTicks_;
    result.frontSpawnLayer = frontSpawnLayer_;
    result.context = context_;
    return result;
}

std::vector<StageSettlementSnapshot> StageSettlementHost::snapshots() const {
    std::vector<StageSettlementSnapshot> result;
    const auto bank = scene_.registry.bank(5);
    if (!bank) return result;
    for (const auto membership : {anm::Membership::Primary, anm::Membership::Secondary}) {
        for (const auto* node : scene_.chain(membership)) {
            const auto& state = node->vm.state();
            std::optional<anm::SpriteInfo> sprite;
            if (state.sprite >= 0 && std::size_t(state.sprite) < bank->sprites.size())
                sprite = bank->sprites[std::size_t(state.sprite)];
            float x = state.x + state.engineX + state.offsetX;
            float y = state.y + state.engineY + state.offsetY;
            float z = state.z + state.engineZ + state.offsetZ;
            if (node->parent) {
                const auto parent = scene_.nodes().find(node->parent);
                if (parent != scene_.nodes().end()) {
                    const auto& owner = parent->second->vm.state();
                    x += owner.x + owner.engineX + owner.offsetX;
                    y += owner.y + owner.engineY + owner.offsetY;
                    z += owner.z + owner.engineZ + owner.offsetZ;
                }
            }
            result.push_back({node->id, node->parent, state, sprite, x, y, z});
        }
    }
    return result;
}

}
