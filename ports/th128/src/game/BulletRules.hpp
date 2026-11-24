#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <memory>
#include <string>

namespace th128::bullet {

struct ExtensionRaw {
    float parameterR = 0;
    float parameterS = 0;
    std::int32_t parameterA = 0;
    std::int32_t parameterB = 0;
    std::int32_t type = 0;
    std::int32_t control = 0;
    float parameterT=0,parameterU=0;
    std::int32_t parameterC=0,parameterD=0;
    std::uint32_t callback=0;
    std::shared_ptr<const std::string> callbackName;
};

struct Emitter {
    std::int32_t type = 0;
    std::int32_t color = 0;
    float positionX = 0;
    float positionY = 0;
    float positionZ = 0;
    float laserWidth = 0;
    std::array<std::int32_t,5> laserTimers{};
    float offsetX=0,offsetY=0,absoluteX=0,absoluteY=0;
    bool absoluteOrigin=false;
    float angle = 0;
    float spacing = 0;
    float speed = 0;
    float endSpeed = 0;
    float spawnRadius = 0;
    std::int16_t count = 0;
    std::int16_t layers = 0;
    std::uint16_t aimMode = 0;
    std::uint32_t flags = 0;
    std::int32_t shotSound = 0;
    std::int32_t transformSound = 0;
    ExtensionRaw extension0;
    std::array<ExtensionRaw,17> laterExtensions{};
};

struct EmissionPrefix {
    float rawAngle = 0;
    float angle = 0;
    float speed = 0;
    float velocityX = 0;
    float velocityY = 0;
};

struct Position {
    float x = 0;
    float y = 0;
    float z = 0;
};

struct SpawnPrefixProfile {
    std::int32_t type = 0;
    float hitboxSize = 0;
    float auxiliarySize = 0;
    std::int32_t value = 0;
    std::int32_t freezePolicy = 0;
};

struct SpawnTailPrefix {
    Position position;
    Position velocity;
    float hitboxSize = 0;
    float auxiliarySize = 0;
    std::uint32_t flags = 0;
    std::uint32_t activeExtensions = 0;
    std::uint32_t emitterFlags = 0;
    std::int32_t transformSound = 0;
    std::int32_t profileValue = 0;
    std::int32_t freezePolicy = 0;
    std::int32_t extensionIndex = 0;
    std::uint16_t motionState = 1;
    std::uint16_t animationInterrupt = 2;
    ExtensionRaw extension0;
    std::array<ExtensionRaw,17> laterExtensions{};
};

std::optional<std::int32_t> chooseDifficulty(
    std::int32_t difficulty, const std::array<std::int32_t, 4>& values) noexcept;
Emitter resetEmitter() noexcept;
bool setSpawnRadius(Emitter& emitter,float radius) noexcept;
bool setEmissionOffset(Emitter& emitter,float x,float y) noexcept;
bool setPolarEmissionOffset(Emitter& emitter,float angle,float radius) noexcept;
bool setAbsoluteEmissionOrigin(Emitter& emitter,float x,float y) noexcept;
Position emissionOrigin(const Emitter& emitter,Position enemy) noexcept;
std::optional<Position> spawnPositionPrefix(const Emitter& emitter,const EmissionPrefix& geometry) noexcept;
void setTypeColor(Emitter& emitter, std::int32_t type, std::int32_t color) noexcept;
void setAimMode(Emitter& emitter, std::int32_t mode) noexcept;
void setAngles(Emitter& emitter, float angle, float spacing) noexcept;
void setDifficultySpeeds(Emitter& emitter, std::int32_t difficulty,
                         const std::array<float, 4>& speeds,
                         const std::array<float, 4>& endSpeeds) noexcept;
void setDifficultyCounts(Emitter& emitter, std::int32_t difficulty,
                         const std::array<std::int32_t, 4>& counts,
                         const std::array<std::int32_t, 4>& layers) noexcept;
bool setFirstExtensionRaw(Emitter& emitter, std::int32_t extensionIndex,
                           const ExtensionRaw& extension) noexcept;
std::optional<ExtensionRaw> extensionRecord(const Emitter& emitter,int index) noexcept;
bool setExtensionRecord(Emitter& emitter,int index,const ExtensionRaw& extension) noexcept;
bool setExtendedExtension(Emitter& emitter,int index,const ExtensionRaw& extension) noexcept;
bool setLegacyExtension(Emitter& emitter,int index,const ExtensionRaw& extension) noexcept;
std::array<std::uint32_t,11> serializeExtension(const ExtensionRaw& extension) noexcept;
bool nextExtensionEmpty(const SpawnTailPrefix& state) noexcept;
std::optional<std::uint32_t> prefixCount(const Emitter& emitter,
                                        std::uint32_t budget = 4096) noexcept;
std::optional<EmissionPrefix> emissionPrefix(const Emitter& emitter, std::int32_t bulletIndex,
                                             std::int32_t layerIndex, float aimAngle) noexcept;
std::optional<float> targetAnglePrefix(const Position& position, const Position& player) noexcept;
std::optional<float> directionalDrawAngle(float bulletAngle,float animationAngle,std::uint32_t animationFlags) noexcept;
std::optional<SpawnTailPrefix> spawnTailPrefix(const Emitter& emitter, const SpawnPrefixProfile& profile,
                                              const Position& position, const Position& velocity,
                                              std::uint32_t postAnimationFlags) noexcept;
std::optional<Position> motionPrefix(const Position& position, const Position& velocity,
                                     std::uint16_t motionState, float timeScale) noexcept;

}
