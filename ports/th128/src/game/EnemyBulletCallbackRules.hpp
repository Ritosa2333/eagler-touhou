#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace th128::enemybullet {

struct Position {
    float x = 0;
    float y = 0;
};

struct EntityContext {
    float x = 0;
    float y = 0;
    float variable4 = 0;
    float variable5 = 0;
};

struct BulletState {
    bool active = false;
    std::int32_t type = 0;
    std::uint16_t motionState = 0;
    Position position;
    float speed = 0;
    float angle = 0;
    Position velocity;
};

struct BulletEffect {
    std::size_t index = 0;
    bool matched = false;
    bool changed = false;
    std::int32_t type = 0;
    std::uint16_t motionState = 0;
    float angle = 0;
    Position velocity;
    float speed=0;
};

enum class Selector : std::int32_t {
    Clear = 0,
    Curve = 2,
    Type25Conversion = 3,
    Accelerate = 4,
};

struct Plan {
    Selector selector = Selector::Curve;
    bool supported = false;
    bool clearCallback = false;
    std::int32_t callbackReturn = 0;
    float nextVariable4 = 0;
    std::vector<BulletEffect> effects;
    std::string reason;
};

inline constexpr float CurveMaximumDistanceSquared = 9216.0f;
inline constexpr int NativeBulletCapacity = 2000;

Plan evaluate(Selector selector, const EntityContext& entity,
              std::span<const BulletState> bullets);

}
