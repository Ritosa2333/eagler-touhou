#include "EnemyBulletCallbackRules.hpp"
#include <cmath>
#include <cstdio>
#include <exception>
#include <limits>
#include <vector>

using namespace th128::enemybullet;

namespace {
void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}
BulletState bullet(float x, float y, float angle, bool active = true, std::uint16_t motion = 1) {
    BulletState value;
    value.active = active;
    value.type = 13;
    value.motionState = motion;
    value.position = {x, y};
    value.speed = 2;
    value.angle = angle;
    return value;
}
}

int main() {
    try {
        {
            EntityContext entity;
            entity.variable4 = .1f;
            entity.variable5 = .2f;
            std::vector<BulletState> bullets{bullet(0, -10, .5f), bullet(0, 10, -2.f),
                                              bullet(0, 10, -1.5f), bullet(96, 0, .5f),
                                              bullet(0, 0, .5f, false), bullet(0, 0, .5f, true, 2)};
            const auto plan = evaluate(Selector::Curve, entity, bullets);
            require(plan.supported && !plan.clearCallback && plan.callbackReturn == 0 &&
                    plan.nextVariable4 == .3f && plan.effects.size() == bullets.size(),
                    "selector2 plan shape");
            require(plan.effects[0].matched && plan.effects[0].changed &&
                    std::abs(plan.effects[0].angle - .6f) < 1e-6f &&
                    std::abs(plan.effects[0].velocity.x - 2.f * std::cos(.6f)) < 1e-6f &&
                    std::abs(plan.effects[0].velocity.y - 2.f * std::sin(.6f)) < 1e-6f,
                    "below entity angle plus variable4 and fsincos velocity");
            require(plan.effects[1].matched && std::abs(plan.effects[1].angle - (-2.1f)) < 1e-6f,
                    "above entity positive delta subtracts variable4");
            require(plan.effects[2].matched && std::abs(plan.effects[2].angle - (-1.4f)) < 1e-6f,
                    "above entity negative delta adds variable4");
            require(!plan.effects[3].matched && !plan.effects[3].changed &&
                    plan.effects[3].angle == .5f, "distance equal to 96^2 must be excluded");
            require(!plan.effects[4].matched && !plan.effects[4].changed, "inactive bullet exclusion");
            require(!plan.effects[5].matched && !plan.effects[5].changed, "motionState!=1 exclusion");
        }
        {
            EntityContext entity;
            entity.variable4 = .1f;
            entity.variable5 = .2f;
            std::vector<BulletState> center{bullet(0, 0, 0)};
            const auto plan = evaluate(Selector::Curve, entity, center);
            require(plan.effects[0].matched && std::abs(plan.effects[0].angle - .1f) < 1e-6f,
                    "zero angle difference at equal y uses native plus-variable4 branch");
        }
        {
            EntityContext entity;
            entity.variable4 = .5f;
            const float target = float(std::atan2(-10.0, 0.0));
            std::vector<BulletState> above{bullet(0, 10, target)};
            const auto plan = evaluate(Selector::Curve, entity, above);
            require(plan.supported && plan.effects[0].matched &&
                    std::abs(plan.effects[0].angle - (target - .5f)) < 1e-6f,
                    "above-entity zero delta must subtract variable4");
        }
        {
            EntityContext entity;
            entity.variable4 = .5f;
            std::vector<BulletState> hugeAngle{bullet(0, -10, 1e6f)};
            const auto plan = evaluate(Selector::Curve, entity, hugeAngle);
            require(!plan.supported && plan.reason.find("normalization domain") != std::string::npos,
                    "out-of-domain normalization must not return successful unchanged effects");
            std::vector<BulletState> invalidSpeed{bullet(0, -10, .5f)};
            invalidSpeed[0].speed = std::numeric_limits<float>::infinity();
            const auto invalid = evaluate(Selector::Curve, entity, invalidSpeed);
            require(!invalid.supported && invalid.reason.find("finite implemented domain") != std::string::npos,
                    "nonfinite active bullet must be explicitly unsupported");
            std::vector<BulletState> tooMany(2001);
            const auto capacity = evaluate(Selector::Curve, entity, tooMany);
            require(!capacity.supported && capacity.reason.find("2000-slot capacity") != std::string::npos,
                    "span capacity contract");
            entity.variable4 = std::numeric_limits<float>::max();
            entity.variable5 = std::numeric_limits<float>::max();
            const auto overflow = evaluate(Selector::Curve, entity, {});
            require(!overflow.supported && overflow.reason.find("variable4 update") != std::string::npos,
                    "nextVariable4 overflow must be rejected before supported=true");
        }
        {
            EntityContext entity;
            entity.variable4 = -1.f;
            entity.variable5 = .5f;
            std::vector<BulletState> bullets{bullet(95.999f, 0, -2.f)};
            const auto plan = evaluate(Selector::Curve, entity, bullets);
            require(plan.effects[0].matched && plan.nextVariable4 == -.5f,
                    "inside threshold boundary and single variable4 update");
            auto two = bullets;
            two.push_back(bullet(0, 1, 2.f));
            const auto multiple = evaluate(Selector::Curve, entity, two);
            require(multiple.nextVariable4 == -.5f && multiple.effects.size() == 2,
                    "variable4 advances once after the whole bullet pass");
            for (const auto& effect : multiple.effects)
                require(!effect.changed || std::isfinite(effect.angle), "finite action");
        }
        {
            EntityContext entity;
            const auto clear = evaluate(Selector::Clear, entity, {});
            require(clear.supported && clear.clearCallback && clear.effects.empty() &&
                    clear.nextVariable4 == entity.variable4, "selector0 real clear rule");
            const auto conversion = evaluate(Selector::Type25Conversion, entity, {});
            require(!conversion.supported && !conversion.clearCallback &&
                    conversion.reason.find("in-place type25 conversion owner") != std::string::npos,
                    "selector3 explicit conversion-owner dependency");
            const auto unknown = evaluate(static_cast<Selector>(7), entity, {});
            require(!unknown.supported && !unknown.clearCallback &&
                    unknown.reason.find("unknown 529 selector") != std::string::npos,
                    "unknown selector explicit rejection");
        }
        std::puts("PASS enemy bullet callback selector2/0 typed rules");
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
