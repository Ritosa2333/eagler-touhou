#include "EnemyBulletCallbackRules.hpp"

#include <cmath>

namespace th128::enemybullet {
namespace {
constexpr double pi = 3.1415927410125732421875;
constexpr double tau = 6.283185482025146484375;

std::optional<float> normalizedDifference(float target, float angle) {
    if (!std::isfinite(target) || !std::isfinite(angle)) return std::nullopt;
    double value = double(target) - double(angle);
    int loops = 0;
    while (value > pi && loops++ <= 32) value -= tau;
    while (value < -pi && loops++ <= 32) value += tau;
    if (loops > 33 || !std::isfinite(value)) return std::nullopt;
    return float(value);
}

BulletEffect unchanged(std::size_t index, const BulletState& bullet) {
    return {index, false, false, bullet.type, bullet.motionState, bullet.angle, bullet.velocity};
}
}

Plan evaluate(Selector selector, const EntityContext& entity, std::span<const BulletState> bullets) {
    Plan plan;
    plan.selector = selector;
    plan.nextVariable4 = entity.variable4;
    if (selector == Selector::Clear) {
        plan.supported = true;
        plan.clearCallback = true;
        plan.callbackReturn = 0;
        return plan;
    }
    if (selector != Selector::Curve&&selector!=Selector::Type25Conversion&&selector!=Selector::Accelerate) {
        plan.reason = "unknown 529 selector";
        return plan;
    }
    if (bullets.size() > NativeBulletCapacity) {
        plan.reason = "selector2 bullet span exceeds native 2000-slot capacity";
        return plan;
    }
    if (!std::isfinite(entity.x) || !std::isfinite(entity.y) ||
        !std::isfinite(entity.variable4) || !std::isfinite(entity.variable5)) {
        plan.reason = "selector2 entity state is outside the finite implemented domain";
        return plan;
    }
    const float nextVariable4 = float(double(entity.variable4) + double(entity.variable5));
    if (!std::isfinite(nextVariable4)) {
        plan.reason = "selector2 variable4 update exceeds the finite implemented domain";
        return plan;
    }
    plan.supported = true;
    plan.callbackReturn = 0;
    plan.nextVariable4 = nextVariable4;
    plan.effects.reserve(bullets.size());
    for (std::size_t index = 0; index < bullets.size(); ++index) {
        const auto& bullet = bullets[index];
        if (!bullet.active || bullet.motionState != 1||(selector==Selector::Type25Conversion&&bullet.type==25)) {
            plan.effects.push_back(unchanged(index, bullet));
            continue;
        }
        if (!std::isfinite(bullet.position.x) || !std::isfinite(bullet.position.y) ||
            !std::isfinite(bullet.angle) || !std::isfinite(bullet.speed)) {
            plan.supported = false;
            plan.reason = "selector2 active bullet state is outside the finite implemented domain";
            return plan;
        }
        const float dx = float(double(entity.x) - double(bullet.position.x));
        const float dy = float(double(entity.y) - double(bullet.position.y));
        const float distanceSquared = float(double(dx) * double(dx) + double(dy) * double(dy));
        if (!std::isfinite(distanceSquared)) {
            plan.supported = false;
            plan.reason = "selector2 distance exceeds the finite implemented domain";
            return plan;
        }
        if (distanceSquared >= CurveMaximumDistanceSquared) {
            plan.effects.push_back(unchanged(index, bullet));
            continue;
        }
        if(selector==Selector::Type25Conversion){
            plan.effects.push_back({index,true,true,25,bullet.motionState,bullet.angle,bullet.velocity});
            continue;
        }
        if(selector==Selector::Accelerate){
            const float speed=float(double(bullet.speed)+entity.variable4);
            const float horizontal=float(std::cos(double(bullet.angle))*speed),vertical=float(std::sin(double(bullet.angle))*speed);
            if(!std::isfinite(speed)||!std::isfinite(horizontal)||!std::isfinite(vertical)){plan.supported=false;plan.reason="selector4 nonfinite speed/velocity";return plan;}
            plan.effects.push_back({index,true,true,bullet.type,bullet.motionState,bullet.angle,{horizontal,vertical},speed});
            continue;
        }
        const float target = float(std::atan2(double(dy), double(dx)));
        const auto difference = normalizedDifference(target, bullet.angle);
        if (!difference) {
            plan.supported = false;
            plan.reason = "selector2 angle delta exceeds the implemented normalization domain";
            return plan;
        }
        float angle = bullet.angle;
        if (bullet.position.y > entity.y) {
            angle = *difference >= 0.f ? float(double(bullet.angle) - double(entity.variable4))
                                        : float(double(entity.variable4) + double(bullet.angle));
        } else {
            angle = *difference < 0.f ? float(double(bullet.angle) - double(entity.variable4))
                                        : float(double(entity.variable4) + double(bullet.angle));
        }
        const float velocityX = float(std::cos(double(angle)) * double(bullet.speed));
        const float velocityY = float(std::sin(double(angle)) * double(bullet.speed));
        if (!std::isfinite(angle) || !std::isfinite(velocityX) || !std::isfinite(velocityY)) {
            plan.supported = false;
            plan.reason = "selector2 angle/velocity exceeds the finite implemented domain";
            return plan;
        }
        plan.effects.push_back({index, true, true, bullet.type, bullet.motionState, angle,
                                {velocityX, velocityY}});
    }
    return plan;
}
}
