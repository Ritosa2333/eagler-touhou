#pragma once

#include "BulletRules.hpp"

namespace th128::bullet {

enum class Continuation { NeedsAnimation, NeedsCollision, NeedsExtension, Removed };

struct TickInput {
    Position position;
    Position velocity;
    std::int32_t type = 8;
    std::uint16_t motionState = 2;
    std::int32_t age = 0;
    std::int32_t lifeClock = 0;
    float lifeTime = 0;
    std::int32_t integer0 = 0;
    std::uint32_t flags = 0x13;
    std::uint32_t activeExtensions = 0;
    std::int32_t grace = 5;
    std::int32_t cooldown = 0;
    bool hasSprite = false;
    float spriteWidth = 0;
    float spriteHeight = 0;
    float clock = 1;
    bool extensionListEmpty = false;
};

struct PreAnimationTick {
    TickInput next;
    Continuation continuation = Continuation::NeedsAnimation;
    std::uint32_t boundary = 0x408d09;
    std::int32_t previousLifeClock = 0;
    bool extensionResolved=false;
};

std::optional<PreAnimationTick> preAnimationTick(const TickInput& input) noexcept;
std::optional<PreAnimationTick> resumeAfterCollisionMiss(const PreAnimationTick& pending) noexcept;
std::optional<PreAnimationTick> resumeAfterResolvedHit(const PreAnimationTick& pending) noexcept;
std::optional<PreAnimationTick> resumeAfterResolvedExtension(const PreAnimationTick& pending,Position velocity,std::uint32_t activeExtensions,bool nextRecordEmpty) noexcept;
std::optional<PreAnimationTick> resumeAfterRestartedExtension(const PreAnimationTick& pending,std::uint32_t activeExtensions,bool nextRecordEmpty) noexcept;
std::optional<PreAnimationTick> resumeAfterExtension4096(const PreAnimationTick& pending,Position position,std::uint32_t activeExtensions) noexcept;

}
