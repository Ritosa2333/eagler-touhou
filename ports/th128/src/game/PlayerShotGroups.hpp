#pragma once
#include "PlayerShotBirthRules.hpp"
#include "animation/AnmScene.hpp"
#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>

namespace th128::playershot {

struct PlayerShotPose {
    float x = 0, y = 0, z = 0;
};

struct PlayerShotGroupSnapshot {
    std::uint32_t group = 0, slot = 0, handle = 0, parent = 0;
    int script = -1;
    std::uint16_t state3f8 = 0;
    anm::State state;
    std::optional<anm::SpriteInfo> sprite;
    float x = 0, y = 0, z = 0;
};

enum class NativeCallback : std::uint8_t { Default412b40 };

struct PlayerShotGroupsState {
    bool loaded = false, configured = false, retired = false, failed = false;
    std::uint32_t group = 0, selector = 0;
    std::uint32_t activeGroups = 0, activeHandles = 0, retiredHandles = 0;
    bool nativeDefault = false;
    std::int32_t callbackResult = 0;
};

class PlayerShotGroups {
public:
    using Visitor = std::function<bool(std::uint32_t,anm::VM&,std::string&)>;
    using Lifecycle = std::function<bool(std::uint32_t,anm::VM&,int,std::string&)>;
    bool load(std::span<const std::uint8_t> sht,std::span<const std::uint8_t> animation,std::string& error);
    bool configure(std::uint32_t group,std::uint32_t selector,PlayerShotPose pose,Lifecycle lifecycle,std::string& error);
    bool configureSelection(float cumulativeArea,int difficulty,std::uint32_t selector,
                            PlayerShotPose pose,Lifecycle lifecycle,std::string& error);
    bool configureNativeDefault(std::uint32_t group,std::uint32_t selector,PlayerShotPose pose,std::string& error);
    bool configureNativeDefaultSelection(float cumulativeArea,int difficulty,std::uint32_t selector,
                                         PlayerShotPose pose,std::string& error);
    bool update(std::uint32_t group,std::uint32_t selector,PlayerShotPose pose,std::string& error);
    bool updateSelection(float cumulativeArea,int difficulty,std::uint32_t selector,
                         PlayerShotPose pose,std::string& error);
    bool tick(PlayerShotPose pose,std::string& error);
    bool clear(int code,std::string& error);
    bool walk(std::uint32_t handle,const Visitor& visitor,std::string& error);
    bool retire(std::uint32_t handle,int code,std::string& error);
    bool retireWithChildren(std::uint32_t handle,int code,std::string& error);
    std::uint32_t slot(std::uint32_t group,std::uint32_t index) const;
    anm::VM* vm(std::uint32_t handle) const;
    std::uint16_t nativeState3f8(std::uint32_t handle) const;
    std::vector<PlayerShotGroupSnapshot> snapshots() const;
    PlayerShotGroupsState state() const;
private:
    std::unique_ptr<ShtBank> bank_;
    anm::Scene scene_;
    Lifecycle lifecycle_;
    std::optional<NativeCallback> nativeCallback_;
    std::int32_t callbackResult_=0;
    bool loaded_=false,configured_=false,retired_=false,executing_=false;
    std::uint32_t group_=0,selector_=0,retiredHandles_=0;
    PlayerShotPose pose_;
    std::array<std::array<std::uint32_t,2>,8> roots_{};
    std::array<std::array<std::uint64_t,2>,8> keys_{};
    std::array<std::array<PlayerShotPose,2>,8> relative_{};
    std::unordered_map<std::uint32_t,std::uint16_t> state3f8_;
    std::string failure_;
    void runtime();
    void updatePositions(PlayerShotPose pose);
    bool configureWith(std::uint32_t group,std::uint32_t selector,PlayerShotPose pose,
                       Lifecycle lifecycle,std::optional<NativeCallback> native,std::string& error);
    bool create(std::uint32_t group,std::uint32_t selector,PlayerShotPose pose,std::string& error);
    bool retireRoot(std::uint32_t handle,int code,std::string& error);
    bool visitTree(std::uint32_t handle,const Visitor& visitor,std::string& error);
    void abandon();
    bool fail(std::string message,std::string& output);
};

}
