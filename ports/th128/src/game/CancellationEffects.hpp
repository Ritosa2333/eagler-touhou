#pragma once

#include "animation/AnmScene.hpp"
#include <optional>

namespace th128::effects {

class CancellationEffects {
public:
    using Handle = uint32_t;
    static constexpr int BankId = 0;
    static constexpr int CancelScript = 357;
    static constexpr int ChildScript = 356;
    static constexpr size_t MaxNodes = 4096;
    static constexpr size_t InstructionBudget = 4096;

    struct Pose {
        float x = 0, y = 0, z = 0, sx = 1, sy = 1, rotation = 0;
    };
    struct Snapshot {
        Handle handle = 0;
        uint32_t node = 0, parent = 0;
        anm::Membership membership = anm::Membership::Primary;
        anm::State state;
        std::optional<anm::SpriteInfo> sprite;
        Pose world;
    };

    CancellationEffects() = default;
    CancellationEffects(const CancellationEffects&) = delete;
    CancellationEffects& operator=(const CancellationEffects&) = delete;
    CancellationEffects(CancellationEffects&&) = delete;
    CancellationEffects& operator=(CancellationEffects&&) = delete;

    bool load(const void* bytes, size_t size, std::string& error);
    bool setGameRandom(std::function<uint32_t()> random);
    std::optional<Handle> spawn(int script, float x, float y, float z, int layer, std::string& error,bool head=false);
    bool tick(std::string& error);
    bool tickPrimary(std::string& error);
    bool setRootPositionInterpolation(Handle handle, float x, float y, float z,
                                      int duration, int mode, std::string& error);
    bool initializeNativeDrift(Handle handle, const std::array<float, 3>& velocity, float clock,
                               const std::array<float, 3>& globalStart, std::string& error);
    bool requestNativeDrift(Handle handle, std::string& error);
    bool remove(Handle handle);
    bool clear();
    std::vector<Snapshot> snapshots() const;
    std::optional<anm::SpriteInfo> spriteInfo(int sprite) const;
    size_t rootCount() const { return roots_.size(); }
    size_t nodeCount() const { return scene_.nodes().size(); }

private:
    struct Root {
        size_t reservation = 1;
    };
    anm::Scene scene_{anm::Scene::AllocationPolicy::Native4096PoolWithHeapFallback};
    std::unordered_map<Handle, Root> roots_;
    std::function<uint32_t()> gameRandom32_;
    std::string failure_;
    Handle nextHandle_ = 1;
    size_t reservedNodes_ = 0;
    std::unordered_map<int, size_t> nodesPerScript_;
    bool executing_ = false;
    bool unvalidatedMutation_ = false;

    void configureRuntime();
    uint32_t random32();
    bool ready(std::string& error) const;
    bool fail(std::string& error);
    bool reconcile(std::string& error);
    bool reconcileSpawned(Handle handle, size_t beforeNodes, bool& accepted);
    bool phase(std::string& error);
    bool worldPose(const anm::Node& node, Pose& pose, size_t depth = 0) const;
    static bool validatePrograms(const anm::Registry& registry, std::unordered_map<int, size_t>& reservations,
                                  std::string& error);
    static bool validState(const anm::State& state);
};

}
