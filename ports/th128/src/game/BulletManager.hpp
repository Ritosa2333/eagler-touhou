#pragma once

#include "BulletRules.hpp"
#include "BulletExtension800000.hpp"
#include "BulletExtension8.hpp"
#include "BulletExtension16.hpp"
#include "BulletExtension4.hpp"
#include "BulletExtension1.hpp"
#include "BulletExtension64.hpp"
#include "BulletExtension256.hpp"
#include "Extension4096.hpp"
#include <array>
#include <cstddef>
#include <initializer_list>
#include <cstdint>
#include <functional>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace th128::bullet {

struct Profile {
    std::int32_t type = 8;
    std::int32_t script = -1;
    std::int32_t secondaryScript = 0;
    SpawnPrefixProfile tail;
    bool load(const void* bytes, std::size_t size, std::string& error);
    bool loadTyped(int selectedType,const void* bytes,std::size_t size,std::string& error);
    std::optional<std::int32_t> remap(std::int32_t color, std::int32_t sprite) const;
    std::optional<std::int32_t> cancellationScript(std::int32_t color) const;
    bool identitySpriteRemap() const noexcept;
private:
    std::array<std::uint32_t, 72> words_{};
    std::int32_t cancellationMode_ = 0;
};

struct Birth {
    std::uint32_t id = 0;
    std::uint32_t slot = 0;
    std::int32_t type = 0;
    std::int32_t color = 0;
    std::int32_t script = -1;
    SpawnTailPrefix state;
    std::int32_t cancellationScript=-1;
    std::int32_t age = 0;
    std::int32_t lifeClock = 0;
    float lifeTime = 0;
    std::int32_t grace = 5;
    std::int32_t cooldown = 0;
    bool extensionListEmpty = true;
    float angle=0;
    float speed=0;
    Extension16 extension16{};
    Extension4 extension4{};
    Extension1 extension1{};
    Extension8 extension8{};
    Extension64 extension64{};
    Extension256 extension256{};
    Extension4096 extension4096{};
    Extension800000Snapshot extension800000{};
    int delayRemaining=0;
    std::string extensionFailure{};
    bool updateExecuting=false;
};

bool initializeBirthExtension1048576(Birth& birth, Position player,
    const std::function<bool(int, std::string&)>& sound, std::string& error);

struct CallbackSnapshot {
    std::size_t slot = 0;
    std::uint32_t id = 0;
    std::int32_t type = 0;
    std::uint16_t motionState = 0;
    Position position;
    float angle = 0;
    float speed = 0;
    Position velocity;
};

struct CallbackMutation {
    std::size_t slot = 0;
    std::uint32_t id = 0;
    float angle = 0;
    Position velocity;
    std::optional<float> speed={};
};

struct SoundRequest {
    std::int32_t identifier = -1;
    float positionX = 0;
    std::int32_t operation = 0;
};

struct NestedEmissionRequest {
    Emitter emitter;
    Profile profile;
    Position player;
    int birthStartIndex = 0;
};

class Manager {
public:
    static constexpr std::size_t Capacity = 2000;
    using Constructor = std::function<bool(Birth&, std::string&)>;
    using NestedEmitter = std::function<bool(Birth&, const NestedEmissionRequest&, std::string&)>;
    using Updater = std::function<bool(Birth&,bool&,std::string&,const NestedEmitter&)>;
    using LegacyUpdater = std::function<bool(Birth&,bool&,std::string&)>;
    using SoundSink=std::function<bool(const SoundRequest&,std::string&)>;
    void reset();
    bool setConstructor(Constructor constructor);
    bool setUpdater(Updater updater);
    bool setUpdater(LegacyUpdater updater);
    bool setUpdater(std::nullptr_t)=delete;
    bool setUpdater(std::initializer_list<std::nullptr_t>)=delete;
    bool setSoundSink(SoundSink sink);
    bool hasUpdater() const;
    bool isCurrentUpdatingBirth(const Birth& birth) const;
    bool tick(std::string& error);
    using Cancellation=std::function<bool(Birth&,std::string&)>;
    bool cancelAll(Cancellation cancellation,std::string& error);
    bool cancelRadius(Position center,float radius,Cancellation cancellation,std::string& error);
    bool emit(const Emitter& emitter, const Profile& profile, const Position& player,
              float exclusionDistance, std::string& error);
    bool release(std::size_t slot);
    bool consume(std::size_t slot,std::uint32_t identifier,std::string& error);
    std::optional<CallbackSnapshot> callbackSnapshot(std::size_t slot) const;
    bool applyCallbackMutations(std::span<const CallbackMutation> mutations,std::string& error);
    bool convertCallbackType25(std::size_t slot,std::uint32_t identifier,const Profile& profile,Constructor visualOwner,std::string& error);
    std::uint32_t activeCount() const;
    std::uint32_t birthCount() const;
    std::size_t cursor() const;
    bool saturated() const;
    std::vector<Birth> snapshots() const;
    const std::vector<SoundRequest>& sounds() const;
private:
    std::vector<std::optional<Birth>> slots_ = std::vector<std::optional<Birth>>(Capacity);
    std::vector<SoundRequest> sounds_;
    Constructor constructor_;
    Updater updater_;
    SoundSink soundSink_;
    std::size_t cursor_ = 0;
    std::uint32_t nextId_ = 1;
    std::uint32_t births_ = 0;
    bool saturated_ = false;
    bool executing_ = false;
    bool nestedActive_ = false;
    bool nestedConstruction_ = false;
    std::size_t nestedParentSlot_ = Capacity;
    std::uint32_t nestedParentId_ = 0;
    std::uint64_t nestedEpoch_ = 0;
    std::string cancellationFailure_;
    bool emitInternal(const Emitter& emitter, const Profile& profile, const Position& player,
                      float exclusionDistance, bool nested, int birthStartIndex, std::string& error);
    bool emitNestedLocked(const NestedEmissionRequest& request, std::string& error);
};

}
