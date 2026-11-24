#pragma once

#include <cstdint>
#include <optional>

namespace th128::bullet::collision {

inline constexpr float MinimumNonzeroMagnitude = 0x1p-16f;
inline constexpr float MaximumMagnitude = 65536.0f;

struct Vec2 {
    float horizontal;
    float vertical;
};

enum class Geometry : std::uint8_t { Miss, PotentialHit, PotentialGraze };

struct RectBounds {
    float minX;
    float minY;
    float maxX;
    float maxY;
};

struct PlayerSnapshot {
    Vec2 position;
    RectBounds bounds;
    float profileHitRadius;
    std::int32_t stateF78;
    std::uint32_t flags438ec;
    std::int32_t counter438dc;
};

struct ControlSnapshot {
    bool present;
    std::int32_t counter5a1c;
};

struct BulletSnapshot {
    Vec2 center;
    Vec2 fullSize;
    std::uint32_t flags;
    std::int32_t age1188;
    float scale1180;
};

enum class HelperOutcome : std::uint8_t {
    Miss, Suppressed, HitReturn, GrazeReturn, DamageCallRequired
};

struct Decision {
    HelperOutcome outcome;
    std::optional<std::int32_t> helperReturn;
    std::uint32_t callSite;
    std::uint32_t callTarget;
};

struct RectangleDecision {
    Decision decision;
    RectBounds hitBounds;
    std::optional<RectBounds> grazeBounds;
};

struct CircleDecision {
    Decision decision;
    float distanceSquared;
    std::optional<float> dividedGrazeSize;
    std::optional<float> grazeSize;
};

enum class UpdateSite : std::uint8_t { HalfStep, FullStep };
enum class HelperRoute : std::uint8_t { None, Rectangle, Circle };
enum class UpdateIntent : std::uint8_t {
    NoHelper, HelperReturned, DamageCallRequired, HitEffectsRequired, GrazeCallRequired
};

struct UpdateDecision {
    HelperRoute route;
    std::optional<Decision> helper;
    UpdateIntent intent;
    std::optional<Vec2> effectiveSize;
    std::uint32_t callSite;
    std::uint32_t callTarget;
    std::optional<std::uint16_t> pendingMotionState;
    std::uint32_t pendingFlagsSetAfterCall;
};

std::optional<Geometry> circleGeometry(Vec2 bulletPosition, float hitboxSize,
                                      Vec2 playerPosition, float profileHitRadius) noexcept;
std::optional<bool> circleSweepHit(Vec2 bulletPosition, float hitboxSize, Vec2 from,
                                  Vec2 to, float profileHitRadius) noexcept;
std::optional<RectangleDecision> rectangleDecision(Vec2 center, Vec2 fullSize,
                                                  PlayerSnapshot player, ControlSnapshot control) noexcept;
std::optional<CircleDecision> circleDecision(Vec2 center, float hitboxSize,
                                            PlayerSnapshot player, ControlSnapshot control) noexcept;
std::optional<UpdateDecision> updateCollisionPrefix(UpdateSite site, BulletSnapshot bullet,
                                                   PlayerSnapshot player, ControlSnapshot control) noexcept;

}
