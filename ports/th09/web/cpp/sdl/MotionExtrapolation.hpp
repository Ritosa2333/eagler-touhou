#pragma once
#include "../game/PlayerMotion.hpp"
#include <eagler/presentation/Extrapolation.hpp>

namespace th09::sdl {
// Samples belong to rendered poses. Missing frames, new rounds and lifecycle
// changes start a fresh pair; large jumps are shown at the authoritative pose.
class ShipExtrapolation {
    const void* owner = nullptr;
    i32 stage = 0, round = 0, frame = 0, state = 0;
    Vec3 position{};
    Vec2 velocity{};
public:
    void reset() { owner = nullptr; velocity = {}; }
    void observe(const void* next, i32 nextStage, i32 nextRound, i32 nextFrame,
                 i32 nextState, Vec3 current) {
        velocity = {};
        if (owner == next && stage == nextStage && round == nextRound &&
            nextFrame > 0 && frame == nextFrame - 1 && state == nextState) {
            const float dx = current.x - position.x, dy = current.y - position.y;
            if (dx*dx + dy*dy <= 16.f*16.f) velocity = {dx,dy};
        }
        owner = next; stage = nextStage; round = nextRound; frame = nextFrame;
        state = nextState; position = current;
    }
    bool moving() const { return velocity.x || velocity.y; }
    Vec2 offset(double phase, const PlayfieldLimits& limits) const {
        if (!moving()) return {};
        const float t = eagler::presentation::ExtrapolationWeight(phase) - 1.f;
        return {
            std::clamp(position.x + velocity.x*t, limits.origin.x, limits.origin.x+limits.extent.x) - position.x,
            std::clamp(position.y + velocity.y*t, limits.origin.y, limits.origin.y+limits.extent.y) - position.y,
        };
    }
};
}
