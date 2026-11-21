#include "BulletRules.hpp"

#include <cstddef>
#include <bit>
#include <cmath>
#include <limits>

namespace th128::bullet {
namespace {

std::size_t emitterDifficulty(std::int32_t difficulty) noexcept {
    return difficulty == 0 ? 0 : difficulty == 1 ? 1 : difficulty == 2 ? 2 : 3;
}

std::int16_t signedWord(std::int32_t value) noexcept {
    const auto word = static_cast<std::uint32_t>(value) & 0xffffU;
    return static_cast<std::int16_t>(word < 0x8000U ? static_cast<std::int32_t>(word)
                                                 : static_cast<std::int32_t>(word) - 0x10000);
}

float normalizePrefixAngle(float angle) noexcept {
    constexpr double pi = 3.1415927410125732421875;
    constexpr double tau = 6.283185482025146484375;
    auto normalized = static_cast<float>(static_cast<double>(angle) + 0.0);
    std::int32_t iterations = 0;
    while (normalized > pi) {
        normalized = static_cast<float>(static_cast<double>(normalized) - tau);
        if (iterations++ > 32) break;
    }
    while (normalized < -pi) {
        normalized = static_cast<float>(static_cast<double>(normalized) + tau);
        if (iterations++ > 32) break;
    }
    return normalized;
}

bool bounded(float value) noexcept {
    return std::isfinite(value) && std::abs(value) <= 1e20f;
}

bool bounded(const Position& position) noexcept {
    return bounded(position.x) && bounded(position.y) && bounded(position.z);
}

}

std::optional<std::int32_t> chooseDifficulty(
    std::int32_t difficulty, const std::array<std::int32_t, 4>& values) noexcept {
    if (difficulty < 0 || difficulty > 4) return std::nullopt;
    return values[difficulty < 3 ? static_cast<std::size_t>(difficulty) : 3];
}

Emitter resetEmitter() noexcept {
    Emitter emitter;
    emitter.speed = 2;
    emitter.count = 1;
    emitter.layers = 1;
    emitter.flags = 35;
    emitter.shotSound = 22;
    emitter.transformSound = 40;
    return emitter;
}

void setTypeColor(Emitter& emitter, std::int32_t type, std::int32_t color) noexcept {
    emitter.type = type;
    emitter.color = color;
}
bool setSpawnRadius(Emitter& emitter,float radius) noexcept {
    if(!bounded(radius))return false;emitter.spawnRadius=radius;return true;
}
bool setEmissionOffset(Emitter& emitter,float x,float y) noexcept {
    if(!bounded(x)||!bounded(y))return false;emitter.offsetX=x;emitter.offsetY=y;return true;
}
bool setPolarEmissionOffset(Emitter& emitter,float angle,float radius) noexcept {
    if(!bounded(angle)||!bounded(radius))return false;
    return setEmissionOffset(emitter,float(std::cos(double(angle))*radius),float(std::sin(double(angle))*radius));
}
bool setAbsoluteEmissionOrigin(Emitter& emitter,float x,float y) noexcept {
    if(!bounded(x)||!bounded(y))return false;emitter.absoluteX=x;emitter.absoluteY=y;emitter.absoluteOrigin=x>= -990.f;return true;
}
Position emissionOrigin(const Emitter& emitter,Position enemy) noexcept {
    return {float(double(emitter.offsetX)+(emitter.absoluteOrigin?emitter.absoluteX:enemy.x)),float(double(emitter.offsetY)+(emitter.absoluteOrigin?emitter.absoluteY:enemy.y)),emitter.absoluteOrigin?0:enemy.z};
}
std::optional<Position> spawnPositionPrefix(const Emitter& emitter,const EmissionPrefix& geometry) noexcept {
    if(!bounded(emitter.spawnRadius)||!bounded(emitter.positionX)||!bounded(emitter.positionY)||!std::isfinite(geometry.angle)||std::abs(geometry.angle)>8192)return std::nullopt;
    Position result{emitter.positionX,emitter.positionY,.1f};
    if(emitter.spawnRadius!=0){
        const float horizontal=float(std::cos(double(geometry.angle))*emitter.spawnRadius),vertical=float(std::sin(double(geometry.angle))*emitter.spawnRadius);
        result.x=float(double(result.x)+horizontal);result.y=float(double(result.y)+vertical);
    }
    if(!bounded(result))return std::nullopt;return result;
}

void setAimMode(Emitter& emitter, std::int32_t mode) noexcept {
    emitter.aimMode = static_cast<std::uint16_t>(mode);
}

void setAngles(Emitter& emitter, float angle, float spacing) noexcept {
    emitter.angle = angle;
    emitter.spacing = spacing;
}

void setDifficultySpeeds(Emitter& emitter, std::int32_t difficulty,
                         const std::array<float, 4>& speeds,
                         const std::array<float, 4>& endSpeeds) noexcept {
    const auto index = emitterDifficulty(difficulty);
    emitter.speed = speeds[index];
    emitter.endSpeed = endSpeeds[index];
}

void setDifficultyCounts(Emitter& emitter, std::int32_t difficulty,
                         const std::array<std::int32_t, 4>& counts,
                         const std::array<std::int32_t, 4>& layers) noexcept {
    const auto index = emitterDifficulty(difficulty);
    emitter.count = signedWord(counts[index]);
    emitter.layers = signedWord(layers[index]);
}

std::optional<std::uint32_t> prefixCount(const Emitter& emitter, std::uint32_t budget) noexcept {
    if (emitter.count <= 0 || emitter.layers <= 0) return 0;
    const auto count = static_cast<std::uint32_t>(emitter.count) * static_cast<std::uint32_t>(emitter.layers);
    if (count > budget) return std::nullopt;
    return count;
}

bool setFirstExtensionRaw(Emitter& emitter, std::int32_t extensionIndex,
                           const ExtensionRaw& extension) noexcept {
    return extensionIndex==0&&setLegacyExtension(emitter,0,extension);
}
std::optional<ExtensionRaw> extensionRecord(const Emitter& emitter,int index) noexcept {
    if(index<0||index>=18)return std::nullopt;
    return index==0?emitter.extension0:emitter.laterExtensions[std::size_t(index-1)];
}
bool setExtensionRecord(Emitter& emitter,int index,const ExtensionRaw& extension) noexcept {
    if(index<0||index>=18)return false;
    if(index==0)emitter.extension0=extension;else emitter.laterExtensions[std::size_t(index-1)]=extension;
    return true;
}
bool setExtendedExtension(Emitter& emitter,int index,const ExtensionRaw& extension) noexcept {
    auto prior=extensionRecord(emitter,index);
    if(!prior||!std::isfinite(extension.parameterR)||!std::isfinite(extension.parameterS)||!std::isfinite(extension.parameterT)||!std::isfinite(extension.parameterU))return false;
    auto next=extension;next.callback=prior->callback;next.callbackName=prior->callbackName;return setExtensionRecord(emitter,index,next);
}
bool setLegacyExtension(Emitter& emitter,int index,const ExtensionRaw& extension) noexcept {
    auto prior=extensionRecord(emitter,index);
    if(!prior||!std::isfinite(extension.parameterR)||!std::isfinite(extension.parameterS))return false;
    prior->parameterR=extension.parameterR;prior->parameterS=extension.parameterS;
    prior->parameterA=extension.parameterA;prior->parameterB=extension.parameterB;prior->type=extension.type;prior->control=extension.control;
    return setExtensionRecord(emitter,index,*prior);
}
std::array<std::uint32_t,11> serializeExtension(const ExtensionRaw& extension) noexcept {
    return {std::bit_cast<std::uint32_t>(extension.parameterR),std::bit_cast<std::uint32_t>(extension.parameterS),
        std::bit_cast<std::uint32_t>(extension.parameterT),std::bit_cast<std::uint32_t>(extension.parameterU),
        std::uint32_t(extension.parameterA),std::uint32_t(extension.parameterB),std::uint32_t(extension.parameterC),std::uint32_t(extension.parameterD),
        std::uint32_t(extension.type),std::uint32_t(extension.control),extension.callback};
}
bool nextExtensionEmpty(const SpawnTailPrefix& state) noexcept {
    if(state.extensionIndex<0||state.extensionIndex>18)return false;
    if(state.extensionIndex==18)return true;
    return state.extensionIndex==0?state.extension0.type==0:state.laterExtensions[std::size_t(state.extensionIndex-1)].type==0;
}

std::optional<EmissionPrefix> emissionPrefix(const Emitter& emitter, std::int32_t bulletIndex,
                                             std::int32_t layerIndex, float aimAngle) noexcept {
    if ((emitter.aimMode > 3 && emitter.aimMode != 5) || bulletIndex < 0 || bulletIndex >= emitter.count || layerIndex < 0
        || layerIndex >= emitter.layers || !std::isfinite(emitter.angle) || !std::isfinite(emitter.spacing)
        || !std::isfinite(emitter.speed) || !std::isfinite(emitter.endSpeed) || !std::isfinite(aimAngle)) {
        return std::nullopt;
    }
    float rawAngle=0;
    if(emitter.aimMode==5){
        const float halfStep=float(3.1415927410125732421875/emitter.count);
        const float ring=float(6.283185482025146484375*bulletIndex/emitter.count+halfStep);
        rawAngle=float(double(ring)+double(emitter.spacing)*layerIndex+emitter.angle);
    }else if(emitter.aimMode>=2){
        const float ring=float(6.283185482025146484375*bulletIndex/emitter.count+(emitter.aimMode==2?double(aimAngle):0.));
        rawAngle=float(double(ring)+double(emitter.spacing)*layerIndex+emitter.angle);
    }else{
        const auto pair = (emitter.count & 1) != 0 ? (bulletIndex + 1) / 2 : bulletIndex / 2;
        const double halfOffset = (emitter.count & 1) != 0 ? 0.0 : static_cast<double>(emitter.spacing) * .5;
        auto offset = static_cast<float>(static_cast<double>(pair) * emitter.spacing + halfOffset);
        if ((bulletIndex & 1) != 0) offset = -offset;
        if (emitter.aimMode == 0) offset = static_cast<float>(static_cast<double>(offset) + aimAngle);
        rawAngle = static_cast<float>(static_cast<double>(emitter.angle) + offset);
    }
    if (!std::isfinite(rawAngle) || std::abs(rawAngle) > 8192) return std::nullopt;
    const auto speed = emitter.layers <= 1 ? emitter.speed
        : static_cast<float>(static_cast<double>(emitter.speed)
            - (static_cast<double>(emitter.speed) - emitter.endSpeed) * layerIndex / (emitter.layers - 1));
    return EmissionPrefix{rawAngle, normalizePrefixAngle(rawAngle), speed,
                          static_cast<float>(std::cos(static_cast<double>(rawAngle)) * speed),
                          static_cast<float>(std::sin(static_cast<double>(rawAngle)) * speed)};
}

std::optional<float> targetAnglePrefix(const Position& position, const Position& player) noexcept {
    if (!bounded(position) || !bounded(player)) return std::nullopt;
    const auto horizontal = static_cast<float>(static_cast<double>(player.x) - position.x);
    const auto vertical = static_cast<float>(static_cast<double>(player.y) - position.y);
    if (horizontal == 0 && vertical == 0) return 1.57079637050628662109375f;
    return static_cast<float>(std::atan2(static_cast<double>(vertical), static_cast<double>(horizontal)));
}
std::optional<float> directionalDrawAngle(float bulletAngle,float animationAngle,std::uint32_t animationFlags) noexcept {
    if(!std::isfinite(animationAngle))return std::nullopt;
    if(!(animationFlags&0x40000000u))return animationAngle;
    if(!std::isfinite(bulletAngle)||std::abs(bulletAngle)>8192)return std::nullopt;
    return normalizePrefixAngle(float(double(bulletAngle)+1.5707963705062866));
}

std::optional<SpawnTailPrefix> spawnTailPrefix(const Emitter& emitter, const SpawnPrefixProfile& profile,
                                              const Position& position, const Position& velocity,
                                              std::uint32_t postAnimationFlags) noexcept {
    if ((emitter.type != 4&&emitter.type != 6&&emitter.type != 8&&emitter.type!=9&&emitter.type!=10&&emitter.type!=13&&emitter.type!=16&&emitter.type!=18&&emitter.type!=20&&emitter.type!=21&&emitter.type!=23&&emitter.type!=24&&emitter.type!=25&&emitter.type!=29&&emitter.type!=30&&emitter.type!=33) || profile.type != emitter.type || emitter.color < 0 || emitter.color > 15
        || !bounded(position) || !bounded(velocity) || !bounded(profile.hitboxSize) || !bounded(profile.auxiliarySize)
        || !std::isfinite(emitter.extension0.parameterR) || !std::isfinite(emitter.extension0.parameterS)) {
        return std::nullopt;
    }
    SpawnTailPrefix result;
    result.position = position;
    result.velocity = velocity;
    result.hitboxSize = profile.hitboxSize;
    result.auxiliarySize = profile.auxiliarySize;
    result.flags = postAnimationFlags | 0x10U;
    result.emitterFlags = emitter.flags;
    result.transformSound = emitter.transformSound;
    result.profileValue = profile.value;
    result.freezePolicy = profile.freezePolicy;
    result.extension0 = emitter.extension0;
    result.laterExtensions=emitter.laterExtensions;
    if (emitter.extension0.type == 2) {
        result.motionState = 2;
        result.animationInterrupt = static_cast<std::uint16_t>(
            (static_cast<std::uint32_t>(emitter.extension0.parameterA) + 7U) & 0xffffU);
        result.extensionIndex = 1;
        const auto backshift = [](float coordinate, float speed) {
            const auto offset = static_cast<float>(static_cast<double>(speed) * 4.0);
            return static_cast<float>(static_cast<double>(coordinate) - offset);
        };
        result.position = {backshift(position.x, velocity.x), backshift(position.y, velocity.y),
                           backshift(position.z, velocity.z)};
    }
    if (!bounded(result.position)) return std::nullopt;
    return result;
}

std::optional<Position> motionPrefix(const Position& position, const Position& velocity,
                                     std::uint16_t motionState, float timeScale) noexcept {
    if ((motionState != 1 && motionState != 2) || !bounded(position) || !bounded(velocity) || !bounded(timeScale)) {
        return std::nullopt;
    }
    for (const auto speed : std::array{velocity.x, velocity.y, velocity.z}) {
        const auto product = static_cast<double>(speed) * timeScale;
        if (!std::isfinite(product) || std::abs(product) > std::numeric_limits<float>::max()) return std::nullopt;
    }
    const auto advance = [motionState, timeScale](float coordinate, float speed) {
        auto offset = static_cast<float>(static_cast<double>(speed) * timeScale);
        if (motionState == 2) offset = static_cast<float>(static_cast<double>(offset) * .5);
        return static_cast<float>(static_cast<double>(coordinate) + offset);
    };
    const Position result{advance(position.x, velocity.x), advance(position.y, velocity.y),
                           advance(position.z, velocity.z)};
    if (!bounded(result)) return std::nullopt;
    return result;
}

}
