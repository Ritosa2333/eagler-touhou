#include "BulletCollisionRules.hpp"

#include <algorithm>
#include <bit>
#include <cmath>
#include <limits>

namespace th128::bullet::collision {
namespace {

static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559
              && std::numeric_limits<float>::digits == 24);
static_assert(std::numeric_limits<double>::digits >= 53);

using Wide = unsigned __int128;

bool bounded(float value) noexcept {
    return std::isfinite(value) && std::abs(value) <= MaximumMagnitude
           && (value == 0 || std::abs(value) >= MinimumNonzeroMagnitude);
}

bool bounded(Vec2 value) noexcept {
    return bounded(value.horizontal) && bounded(value.vertical);
}
bool coordinate(Vec2 value) noexcept {
    return std::isfinite(value.horizontal)&&std::isfinite(value.vertical)
        &&std::abs(value.horizontal)<=MaximumMagnitude&&std::abs(value.vertical)<=MaximumMagnitude;
}

bool nonnegative(Vec2 value) noexcept {
    return bounded(value) && value.horizontal >= 0 && value.vertical >= 0;
}

bool bounded(PlayerSnapshot player) noexcept {
    const auto bounds = player.bounds;
    return bounded(player.position) && bounded(bounds.minX) && bounded(bounds.minY)
           && bounded(bounds.maxX) && bounded(bounds.maxY)
           && bounds.minX <= bounds.maxX && bounds.minY <= bounds.maxY
           && bounded(player.profileHitRadius) && player.profileHitRadius >= 0;
}

unsigned bitWidth(Wide value) noexcept {
    const auto high = static_cast<std::uint64_t>(value >> 64);
    return high ? 64U + std::bit_width(high) : std::bit_width(static_cast<std::uint64_t>(value));
}

Wide rounded(Wide value, unsigned digits) noexcept {
    const auto width = bitWidth(value);
    if (width <= digits) return value;
    const auto shift = width - digits;
    auto significand = value >> shift;
    const auto remainder = value & ((Wide{1} << shift) - 1);
    const auto halfway = Wide{1} << (shift - 1);
    if (remainder > halfway || (remainder == halfway && (significand & 1))) ++significand;
    return significand << shift;
}

float storedFloat(Wide magnitude, unsigned fractionBits, bool negative = false) noexcept {
    const auto sign = negative ? 0x80000000U : 0U;
    if (magnitude == 0) return std::bit_cast<float>(sign);
    const auto stored = rounded(magnitude, 24);
    const auto width = bitWidth(stored);
    const auto exponent = static_cast<int>(width) - 1 - static_cast<int>(fractionBits) + 127;
    const auto significand = width > 24 ? stored >> (width - 24) : stored << (24 - width);
    const auto bits = sign | (static_cast<std::uint32_t>(exponent) << 23)
                      | (static_cast<std::uint32_t>(significand) & 0x7fffffU);
    return std::bit_cast<float>(bits);
}

std::int64_t fixed(float value) noexcept {
    const auto bits = std::bit_cast<std::uint32_t>(value);
    if ((bits & 0x7fffffffU) == 0) return 0;
    const auto exponent = static_cast<unsigned>((bits >> 23) & 0xffU);
    const auto magnitude = std::uint64_t{(bits & 0x7fffffU) | 0x800000U} << (exponent - 110);
    return bits & 0x80000000U ? -static_cast<std::int64_t>(magnitude)
                              : static_cast<std::int64_t>(magnitude);
}

Wide square(std::int64_t value) noexcept {
    const auto magnitude = static_cast<std::uint64_t>(value < 0 ? -value : value);
    return Wide{magnitude} * magnitude;
}

Wide distanceAtScale(float value) noexcept {
    const auto bits = std::bit_cast<std::uint32_t>(value);
    if (bits == 0) return 0;
    const Wide significand = (bits & 0x7fffffU) | 0x800000U;
    const auto shift = static_cast<int>((bits >> 23) & 0xffU) - 70;
    return shift < 0 ? significand >> -shift : significand << shift;
}

float bound(float center, float size, bool upper) noexcept {
    const auto coordinate = fixed(center);
    const auto halfSize = fixed(size) / 2;
    const auto result = upper ? coordinate + halfSize : coordinate - halfSize;
    const bool negativeZero = center == 0 && size == 0 && std::signbit(center)
                              && (upper ? std::signbit(size) : !std::signbit(size));
    return storedFloat(static_cast<std::uint64_t>(result < 0 ? -result : result), 40,
                       result < 0 || (result == 0 && negativeZero));
}

RectBounds boundsFor(Vec2 center, Vec2 size) noexcept {
    return {bound(center.horizontal, size.horizontal, false), bound(center.vertical, size.vertical, false),
            bound(center.horizontal, size.horizontal, true), bound(center.vertical, size.vertical, true)};
}

bool overlaps(RectBounds first, RectBounds second) noexcept {
    return first.maxX >= second.minX && first.maxY >= second.minY
           && first.minX <= second.maxX && first.minY <= second.maxY;
}

Decision returned(HelperOutcome outcome, std::int32_t value) noexcept {
    return {outcome, value, 0, 0};
}

Decision hitDecision(PlayerSnapshot player, ControlSnapshot control, std::uint32_t callSite) noexcept {
    if ((control.present && control.counter5a1c != 0)
        || player.stateF78 == 2 || player.stateF78 == 3 || player.stateF78 == 4
        || (player.flags438ec & 2U)) {
        return returned(HelperOutcome::Suppressed, 0);
    }
    if (player.counter438dc > 0) return returned(HelperOutcome::HitReturn, 1);
    return {HelperOutcome::DamageCallRequired, std::nullopt, callSite, 0x43d0c0};
}

struct CircleGeometryDetails {
    Geometry geometry;
    float distanceSquared;
    std::optional<float> dividedGrazeSize;
    std::optional<float> grazeSize;
};
long double extended(long double value) noexcept {
    if(value==0)return value;
    int exponent=0;const auto fraction=std::frexp(value,&exponent);
    return std::ldexp(std::nearbyint(std::ldexp(fraction,64)),exponent-64);
}
CircleGeometryDetails smallCoordinateCircle(Vec2 bullet,float size,Vec2 player,float radius) noexcept {
    static_assert(std::numeric_limits<long double>::digits>=64);
    const auto horizontal=extended(static_cast<long double>(player.horizontal)-bullet.horizontal);
    const auto vertical=extended(static_cast<long double>(player.vertical)-bullet.vertical);
    const float distance=float(extended(extended(horizontal*horizontal)+extended(vertical*vertical)));
    const auto sizeSquared=static_cast<long double>(size)*size;
    const auto threshold=extended(static_cast<long double>(radius)*radius+sizeSquared);
    if(static_cast<long double>(distance)<threshold)return {Geometry::PotentialHit,distance,std::nullopt,std::nullopt};
    const float dividedSize=float(double(size)/2.5),grazeSize=std::max(dividedSize,40.f);
    const auto grazeRadius=extended(static_cast<long double>(radius)+grazeSize);
    const auto grazeThreshold=extended(extended(grazeRadius*grazeRadius)+sizeSquared);
    return {static_cast<long double>(distance)<grazeThreshold?Geometry::PotentialGraze:Geometry::Miss,distance,dividedSize,grazeSize};
}

std::optional<CircleGeometryDetails> circleGeometryDetails(Vec2 bulletPosition, float hitboxSize,
                                                          Vec2 playerPosition, float profileHitRadius) noexcept {
    if (!coordinate(bulletPosition) || !coordinate(playerPosition) || !bounded(hitboxSize) || hitboxSize < 0
        || !bounded(profileHitRadius) || profileHitRadius < 0) {
        return std::nullopt;
    }
    if(!bounded(bulletPosition)||!bounded(playerPosition))return smallCoordinateCircle(bulletPosition,hitboxSize,playerPosition,profileHitRadius);
    const auto deltaX = fixed(playerPosition.horizontal) - fixed(bulletPosition.horizontal);
    const auto deltaY = fixed(playerPosition.vertical) - fixed(bulletPosition.vertical);
    const auto distance = rounded(rounded(square(deltaX), 64) + rounded(square(deltaY), 64), 64);
    const auto distanceSquared = storedFloat(distance, 80);
    const auto storedDistance = distanceAtScale(distanceSquared);
    const auto sizeSquared = square(fixed(hitboxSize));
    const auto hitThreshold = rounded(square(fixed(profileHitRadius)) + sizeSquared, 64);
    if (storedDistance < hitThreshold) {
        return CircleGeometryDetails{Geometry::PotentialHit, distanceSquared, std::nullopt, std::nullopt};
    }
    const auto dividedSize = static_cast<float>(static_cast<double>(hitboxSize) / 2.5);
    const auto grazeSize = dividedSize < 40.0f ? 40.0f : dividedSize;
    const auto grazeRadius = fixed(profileHitRadius) + fixed(grazeSize);
    const auto grazeThreshold = rounded(rounded(square(grazeRadius), 64) + sizeSquared, 64);
    const auto geometry = storedDistance < grazeThreshold ? Geometry::PotentialGraze : Geometry::Miss;
    return CircleGeometryDetails{geometry, distanceSquared, dividedSize, grazeSize};
}

}

std::optional<Geometry> circleGeometry(Vec2 bulletPosition, float hitboxSize,
                                      Vec2 playerPosition, float profileHitRadius) noexcept {
    const auto evaluated = circleGeometryDetails(bulletPosition, hitboxSize, playerPosition, profileHitRadius);
    if (!evaluated) return std::nullopt;
    return evaluated->geometry;
}

std::optional<RectangleDecision> rectangleDecision(Vec2 center, Vec2 fullSize,
                                                  PlayerSnapshot player, ControlSnapshot control) noexcept {
    if (!bounded(center) || !nonnegative(fullSize) || !bounded(player)) return std::nullopt;
    const auto hitBounds = boundsFor(center, fullSize);
    if (overlaps(hitBounds, player.bounds)) {
        return RectangleDecision{hitDecision(player, control, 0x43c4e5), hitBounds, std::nullopt};
    }
    const auto grazeBounds = boundsFor(center, {48, 48});
    const auto graze = overlaps(grazeBounds, player.bounds);
    return RectangleDecision{returned(graze ? HelperOutcome::GrazeReturn : HelperOutcome::Miss, graze ? 2 : 0),
                             hitBounds, grazeBounds};
}

std::optional<CircleDecision> circleDecision(Vec2 center, float hitboxSize,
                                            PlayerSnapshot player, ControlSnapshot control) noexcept {
    auto supportedPlayer=player;supportedPlayer.position={0,0};
    if (!coordinate(player.position)||!bounded(supportedPlayer)) return std::nullopt;
    const auto evaluated = circleGeometryDetails(center, hitboxSize, player.position, player.profileHitRadius);
    if (!evaluated) return std::nullopt;
    const auto decision = evaluated->geometry == Geometry::PotentialHit ? hitDecision(player, control, 0x43c648)
        : returned(evaluated->geometry == Geometry::PotentialGraze ? HelperOutcome::GrazeReturn : HelperOutcome::Miss,
                   evaluated->geometry == Geometry::PotentialGraze ? 2 : 0);
    return CircleDecision{decision, evaluated->distanceSquared, evaluated->dividedGrazeSize, evaluated->grazeSize};
}

std::optional<UpdateDecision> updateCollisionPrefix(UpdateSite site, BulletSnapshot bullet,
                                                   PlayerSnapshot player, ControlSnapshot control) noexcept {
    if ((site != UpdateSite::HalfStep && site != UpdateSite::FullStep)
        || !bounded(bullet.center) || !nonnegative(bullet.fullSize) || !bounded(bullet.scale1180)
        || bullet.scale1180 < 0 || !bounded(player)) {
        return std::nullopt;
    }
    UpdateDecision result{HelperRoute::None, std::nullopt, UpdateIntent::NoHelper, std::nullopt,
                          0, 0, std::nullopt, 0};
    if ((site == UpdateSite::HalfStep && bullet.age1188 < 8) || !(bullet.flags & 2U)) return result;
    const bool circle = (bullet.flags & 0x10U) != 0;
    auto size = circle ? Vec2{bullet.fullSize.horizontal, bullet.fullSize.horizontal} : bullet.fullSize;
    if (bullet.flags & 0x40U) {
        size.horizontal = static_cast<float>(static_cast<double>(size.horizontal) * bullet.scale1180);
        size.vertical = circle ? size.horizontal
                               : static_cast<float>(static_cast<double>(size.vertical) * bullet.scale1180);
    }
    if (!nonnegative(size)) return std::nullopt;
    result.route = circle ? HelperRoute::Circle : HelperRoute::Rectangle;
    result.effectiveSize = size;
    if (circle) {
        const auto evaluated = circleDecision(bullet.center, size.horizontal, player, control);
        if (!evaluated) return std::nullopt;
        result.helper = evaluated->decision;
    } else {
        const auto evaluated = rectangleDecision(bullet.center, size, player, control);
        if (!evaluated) return std::nullopt;
        result.helper = evaluated->decision;
    }
    const auto helper = *result.helper;
    result.intent = UpdateIntent::HelperReturned;
    if (helper.outcome == HelperOutcome::DamageCallRequired) {
        result.intent = UpdateIntent::DamageCallRequired;
        result.callSite = helper.callSite;
        result.callTarget = helper.callTarget;
    } else if (helper.helperReturn == 1) {
        result.intent = UpdateIntent::HitEffectsRequired;
        result.callSite = site == UpdateSite::HalfStep ? 0x40882b : 0x408abe;
        result.callTarget = 0x40d2a0;
        result.pendingMotionState = 3;
    } else if (helper.helperReturn == 2 && !(bullet.flags & 4U)) {
        result.intent = UpdateIntent::GrazeCallRequired;
        result.callSite = site == UpdateSite::HalfStep ? 0x408883 : 0x408b67;
        result.callTarget = 0x43e090;
        result.pendingFlagsSetAfterCall = 4;
    }
    return result;
}

}
