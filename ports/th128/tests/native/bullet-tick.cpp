#include "game/BulletTickRules.hpp"
#include <bit>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>

using namespace th128::bullet;

void check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

float readFloat() {
    std::uint32_t value = 0;
    std::cin >> value;
    return std::bit_cast<float>(value);
}

Position readPosition() {
    return {readFloat(), readFloat(), readFloat()};
}

void printFloat(float value) {
    std::cout << ' ' << std::bit_cast<std::uint32_t>(value);
}

void printPosition(const Position& position) {
    printFloat(position.x);
    printFloat(position.y);
    printFloat(position.z);
}

int main() {
    TickInput input;
    auto invalid = input;
    invalid.clock = .5f;
    check(!preAnimationTick(invalid), "fractional clocks are outside the proved prefix");
    invalid = input;
    invalid.type = 9;
    check(!preAnimationTick(invalid), "unsupported type must fail");
    invalid = input;
    invalid.motionState = 5;
    check(!preAnimationTick(invalid), "unsupported motion state must fail");
    invalid = input;
    invalid.age = 4097;
    check(!preAnimationTick(invalid), "frame budget must be enforced");
    invalid = input;
    invalid.position.x = std::numeric_limits<float>::infinity();
    check(!preAnimationTick(invalid), "nonfinite position must fail");
    invalid = input;
    invalid.velocity.y = std::numeric_limits<float>::quiet_NaN();
    check(!preAnimationTick(invalid), "nonfinite velocity must fail");
    invalid = input;
    invalid.hasSprite = true;
    invalid.spriteWidth = -1;
    check(!preAnimationTick(invalid), "negative sprite dimensions must fail");
    invalid = input;
    invalid.lifeClock = -1;
    check(!preAnimationTick(invalid), "bounded initial life counter is required");
    invalid = input;
    invalid.lifeTime = 16777218;
    check(!preAnimationTick(invalid), "float input budget must be enforced");
    invalid = input;
    invalid.spriteWidth = 0x1p-60f;
    check(!preAnimationTick(invalid), "unproved x87 tiny-extent comparison must fail");
    const auto initial = preAnimationTick(input);
    check(initial && initial->next.age == 0 && initial->next.lifeClock == 1 &&
          initial->next.grace == 4 && initial->continuation == Continuation::NeedsAnimation,
          "body life timer changes; manager-owned age must not increment");
    auto next = input;
    next.position = {-128, -32, .1f};
    next.velocity = {.25f, .75f, 0};
    for (int age = 0; age <= 7; ++age) {
        next.age = age;
        const auto result = preAnimationTick(next);
        check(result && result->continuation == Continuation::NeedsAnimation && result->next.age == age,
              "first eight motion steps reach only the pre-animation boundary");
        next = result->next;
    }
    next.age = 8;
    const auto collision = preAnimationTick(next);
    check(collision && collision->continuation == Continuation::NeedsCollision &&
          collision->boundary == 0x40880f && collision->next.lifeClock == 9,
          "age8 must stop at the real collision call after half movement");
    const auto resumed = resumeAfterCollisionMiss(*collision);
    check(resumed && resumed->continuation == Continuation::NeedsAnimation &&
          resumed->next.lifeClock == collision->next.lifeClock &&
          resumed->next.position.x == collision->next.position.x && resumed->next.age == collision->next.age,
          "collision miss resume must not repeat life timer, half motion or manager age");
    check(!resumeAfterCollisionMiss(*resumed), "non-collision continuation cannot be resumed");
    check(!resumeAfterResolvedHit(*resumed),"resolved hit requires an original collision continuation");
    auto invalidPending = *collision;
    invalidPending.boundary = 0x408d09;
    check(!resumeAfterCollisionMiss(invalidPending), "unknown collision callsite cannot be resumed");
    check(!resumeAfterResolvedHit(invalidPending),"resolved hit rejects an unknown callsite");
    invalidPending = *collision;
    invalidPending.previousLifeClock = -1;
    check(!resumeAfterCollisionMiss(invalidPending), "invalid pending timer provenance cannot be resumed");
    check(!resumeAfterResolvedHit(invalidPending),"resolved hit rejects invalid timer provenance");
    invalidPending = *collision;
    invalidPending.next.flags &= ~2u;
    check(!resumeAfterCollisionMiss(invalidPending), "non-collidable pending state cannot be resumed");
    check(!resumeAfterResolvedHit(invalidPending),"resolved hit rejects a non-collidable state");
    invalidPending = *collision;
    invalidPending.previousLifeClock = std::numeric_limits<std::int32_t>::max();
    check(!resumeAfterCollisionMiss(invalidPending), "malformed counter must reject before signed overflow");
    check(!resumeAfterResolvedHit(invalidPending),"resolved hit rejects malformed counters before arithmetic");
    invalidPending=*collision;
    invalidPending.next.motionState=3;
    check(!resumeAfterResolvedHit(invalidPending),"already changed hit state cannot authenticate an earlier collision");
    PreAnimationTick invalidExtension;invalidExtension.continuation=Continuation::NeedsExtension;invalidExtension.boundary=0x4088a6;
    invalidExtension.next.motionState=1;invalidExtension.previousLifeClock=std::numeric_limits<int>::max();
    check(!resumeAfterResolvedExtension(invalidExtension,{},16,true),"malformed extension timer rejects before signed addition");
    TickInput cancelled;cancelled.type=8;cancelled.motionState=4;cancelled.extensionListEmpty=true;
    cancelled.position={10,100,0};cancelled.velocity={.5f,1.5f,0};cancelled.flags=0x11;
    const auto cancelledTick=preAnimationTick(cancelled);check(bool(cancelledTick),"native cancelled state4 must reach ANM without movement");
    check(cancelledTick->continuation==Continuation::NeedsAnimation&&cancelledTick->next.position.x==10&&cancelledTick->next.position.y==100,
        "native state4 is stationary after the explicit cancellation half step");
    char operation = 0;
    while (std::cin >> operation) {
        PreAnimationTick pending;
        if (operation == 'R'||operation=='H') {
            int continuation = 0;
            std::cin >> continuation >> pending.boundary >> pending.previousLifeClock;
            pending.continuation = static_cast<Continuation>(continuation);
        } else {
            check(operation == 'T', "unknown native fixture operation");
        }
        std::cin >> input.type >> input.motionState >> input.age >> input.lifeClock;
        input.lifeTime = readFloat();
        std::cin >> input.integer0 >> input.flags >> input.activeExtensions >> input.grace >> input.cooldown >> input.hasSprite;
        input.spriteWidth = readFloat();
        input.spriteHeight = readFloat();
        input.clock = readFloat();
        std::cin >> input.extensionListEmpty;
        input.position = readPosition();
        input.velocity = readPosition();
        check(bool(std::cin), "truncated native-golden input");
        pending.next = input;
        const auto result = operation=='H'?resumeAfterResolvedHit(pending):operation == 'R' ? resumeAfterCollisionMiss(pending) : preAnimationTick(input);
        check(bool(result), "native-golden prefix was rejected");
        const auto& state = result->next;
        std::cout << int(result->continuation) << ' ' << result->boundary << ' ' << result->previousLifeClock
                  << ' ' << state.type << ' ' << state.motionState << ' ' << state.age << ' ' << state.lifeClock;
        printFloat(state.lifeTime);
        std::cout << ' ' << state.integer0 << ' ' << state.flags << ' ' << state.activeExtensions
                  << ' ' << state.grace << ' ' << state.cooldown << ' ' << state.hasSprite;
        printFloat(state.spriteWidth);
        printFloat(state.spriteHeight);
        printFloat(state.clock);
        std::cout << ' ' << state.extensionListEmpty;
        printPosition(state.position);
        printPosition(state.velocity);
        std::cout << '\n';
    }
}
