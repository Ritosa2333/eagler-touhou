#include "game/EnemyRules.hpp"

#include <bit>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>

namespace {
namespace rules = th128::enemy;
std::size_t checks = 0;

void expect(bool condition, const char* message) {
    ++checks;
    if (!condition) {
        std::fprintf(stderr, "enemy rule failed: %s\n", message);
        std::exit(1);
    }
}

void fixedVectors() {
    const auto root = rules::setFlags(0, 0x20);
    expect(root.flags == 0x20 && root.visibility == rules::VisibilityEffect::Hide, "root 402 hides existing animations");
    expect(rules::setFlags(0x20, 0).visibility == rules::VisibilityEffect::Hide, "402 tests resulting flags, not transitions");
    expect(rules::clearFlags(0, 0).visibility == rules::VisibilityEffect::Show, "403 requests show even when unchanged");
    expect(rules::clearFlags(0x60, 0x40).visibility == rules::VisibilityEffect::None, "403 still-hidden branch");
    expect(rules::setFlags(0x80000000u, 2).flags == 0x80000002u, "full unsigned flags preserved");
    rules::Bounds bounds{10, 11};
    expect(rules::setBounds(bounds, 24, 24) && bounds.width == 24 && bounds.height == 24, "400 raw extents, not half sizes");
    expect(rules::setBounds(bounds, -1, 0) && bounds.width == -1, "finite raw negative bound is not clamped");
    expect(!rules::setBounds(bounds, std::numeric_limits<float>::infinity(), 2) && bounds.width == -1, "nonfinite bounds rejected atomically");
    rules::Polar polar{1.75f, 3, 0xff};
    expect(rules::setPolar(polar, rules::UnchangedPolar, rules::UnchangedPolar), "304 keep sentinel accepted");
    expect(polar.angle == 1.75f && polar.speed == 3 && polar.modeFlags == 0xfc, "304 clears motion bits even with both sentinels");
    expect(rules::setPolar(polar, 0, -2) && polar.speed == -2, "304 does not clamp negative speed");
    expect(rules::setPolar(polar, 0, 3, 0x100000) && polar.angle == rules::Pi, "304 mirrored zero angle is native float32 pi");
    rules::PolarInterpolation interpolation;
    polar = {1.75f, 3, 0xff};
    expect(rules::beginPolarInterpolation(polar, interpolation, 100, 0, 0.3926991f, 1), "305 first reached linear setup");
    expect(interpolation.duration == 100 && interpolation.elapsed == 0 && polar.modeFlags == 0xfc, "305 resets timer and clears motion bits");
    expect(rules::stepPolarInterpolation(polar, interpolation) && interpolation.elapsed == 1 && polar.speed < 3, "linear first step is 1/100");
    for (std::size_t step = 1; step < 100; ++step) {
        expect(rules::stepPolarInterpolation(polar, interpolation), "linear subsequent step");
    }
    expect(interpolation.duration == 0 && polar.angle == 0.3926991f && polar.speed == 1, "linear completion uses exact endpoint");
    const auto saved = interpolation;
    expect(!rules::beginPolarInterpolation(polar, interpolation, 100, 1, 0, 0), "unverified easing mode explicitly rejected");
    expect(interpolation.targetAngle == saved.targetAngle && interpolation.duration == saved.duration, "unsupported mode has no partial mutation");
    polar.modeFlags = 3;
    expect(rules::beginPolarInterpolation(polar, interpolation, 0, 123, 0, 0) && polar.modeFlags == 3, "nonpositive duration takes early native stop");
    rules::AnimationState animation;
    rules::setAnimationBank(animation, 1);
    const auto primary = rules::animationSlot(animation, 0, 0, 0x20);
    expect(primary && primary->bank == 1 && primary->slot == 0 && primary->script == 0 && primary->layer == 7 && primary->primary, "262 bank1 slot0 script0 request");
    expect(primary->visibility == rules::VisibilityEffect::Hide, "262 newly born hidden animation request");
    expect(!rules::animationSlot(animation, 16, 0) && !rules::animationSlot(animation, -1, 0), "bounded host slots, not native memory corruption");
    rules::SpawnParent parent{{100, 200, 999}, 0, {}, 0x12345678};
    parent.inheritedWords[11] = 0xaabbccdd;
    const auto absolute = rules::prepareSpawn(257, "GirlA1b", parent, -128, -32, 20, 1000, 2);
    expect(absolute && absolute->position.x == -128 && absolute->position.y == -32 && absolute->position.z == 0, "257 absolute xy, zero z");
    expect(absolute->life == 20 && absolute->score == 1000 && absolute->drop == 2 && absolute->mode == 0, "257 argument order");
    expect(absolute->inheritedWords[11] == 0xaabbccdd && absolute->contextTag == 0x12345678, "spawn opaque inherited values preserved");
    const auto relative = rules::prepareSpawn(256, "GirlA1b", parent, -128, -32, 20, 1000, 2);
    const auto mirrored = rules::prepareSpawn(260, "GirlA1b", parent, -128, -32, 20, 1000, 2);
    expect(relative && relative->position.x == -28 && relative->position.y == 168 && relative->mode == 0, "256 relative to combined parent position");
    expect(mirrored && mirrored->position.x == -28 && mirrored->position.y == 168 && mirrored->mode == 1, "260 relative position and native mode 1");
    parent.flags = 0x8000000;
    expect(!rules::prepareSpawn(256, "GirlA1b", parent, 0, 0, 1, 2, 3), "unrecovered 256 transform branch is unsupported");
    const auto absoluteMode1 = rules::prepareSpawn(261,"GirlA1b",parent,10,20,1,2,3);
    expect(absoluteMode1&&absoluteMode1->position.x==10&&absoluteMode1->position.y==20&&absoluteMode1->mode==1,
        "261 absolute position and native mode1 request");
    expect(!rules::prepareSpawn(257, std::string_view("bad\0name", 8), parent, 0, 0, 1, 2, 3), "embedded NUL name guard");
    rules::Position position{-0.001f, 0.001f, 0};
    rules::Position velocity{9, 9, 9};
    expect(rules::advancePolarPosition(position, velocity, {0, 0, 0}) && position.x == -0.01f && position.y == 0,
           "simple polar position uses floor of float32 hundredths, including negatives");
    expect(velocity.x == 0 && velocity.y == 0 && velocity.z == 0, "native polar velocity output");
    const auto previousPosition = position;
    expect(!rules::advancePolarPosition(position, velocity, {0, 1, 1}) && position.x == previousPosition.x,
           "circular mode is not silently approximated by linear motion");
    expect(!rules::combinePosition(position, velocity, {0, 0, 0}, {0, 0, 0}, 0x40000), "unimplemented native clamp path rejected");
    std::printf("enemy fixed vectors passed: %zu\n", checks);
}
}

int main() {
    fixedVectors();
    std::string identifier;
    std::string operation;
    std::size_t queries = 0;
    while (std::cin >> identifier >> operation) {
        if (operation == "flags") {
            std::uint32_t flags = 0;
            std::uint32_t mask = 0;
            std::uint32_t opcode = 0;
            std::cin >> opcode >> flags >> mask;
            const auto result = opcode == 402 ? rules::setFlags(flags, mask) : rules::clearFlags(flags, mask);
            std::cout << identifier << ' ' << result.flags << ' ' << static_cast<int>(result.visibility) << '\n';
        } else if (operation == "polar") {
            std::uint32_t angle = 0;
            std::uint32_t speed = 0;
            std::uint32_t flags = 0;
            rules::Polar polar;
            std::cin >> angle >> speed >> polar.modeFlags >> flags;
            polar.angle = std::bit_cast<float>(angle);
            polar.speed = std::bit_cast<float>(speed);
            std::cin >> angle >> speed;
            const bool accepted = rules::setPolar(polar, std::bit_cast<float>(angle), std::bit_cast<float>(speed), flags);
            std::cout << identifier << ' ' << accepted << ' ' << std::bit_cast<std::uint32_t>(polar.angle) << ' '
                      << std::bit_cast<std::uint32_t>(polar.speed) << ' ' << polar.modeFlags << '\n';
        } else if (operation == "bounds") {
            std::uint32_t width = 0;
            std::uint32_t height = 0;
            std::cin >> width >> height;
            rules::Bounds bounds{17, 19};
            const bool accepted = rules::setBounds(bounds, std::bit_cast<float>(width), std::bit_cast<float>(height));
            std::cout << identifier << ' ' << accepted << ' ' << std::bit_cast<std::uint32_t>(bounds.width) << ' '
                      << std::bit_cast<std::uint32_t>(bounds.height) << '\n';
        } else if (operation == "bank") {
            std::int32_t bank = 0;
            std::cin >> bank;
            rules::AnimationState animation{17, 0};
            rules::setAnimationBank(animation, bank);
            std::cout << identifier << ' ' << animation.bank << '\n';
        } else if (operation == "animation-request") {
            rules::AnimationState animation;
            std::int32_t slot = 0;
            std::int32_t script = 0;
            std::uint32_t flags = 0;
            std::cin >> animation.bank >> animation.layerOffset >> slot >> script >> flags;
            const auto request = rules::animationSlot(animation, slot, script, flags);
            if (!request) {
                std::cout << identifier << " 0\n";
            } else {
                std::cout << identifier << " 1 " << request->bank << ' ' << request->slot << ' ' << request->script
                          << ' ' << request->layer << ' ' << static_cast<int>(request->visibility) << ' ' << request->primary << '\n';
            }
        } else if (operation == "polar-interpolate") {
            std::uint32_t oldAngle = 0;
            std::uint32_t oldSpeed = 0;
            std::uint32_t angle = 0;
            std::uint32_t speed = 0;
            std::uint32_t flags = 0;
            std::int32_t duration = 0;
            std::int32_t mode = 0;
            rules::Polar polar;
            std::cin >> oldAngle >> oldSpeed >> polar.modeFlags >> flags >> duration >> mode >> angle >> speed;
            polar.angle = std::bit_cast<float>(oldAngle);
            polar.speed = std::bit_cast<float>(oldSpeed);
            rules::PolarInterpolation interpolation{1.75f, 3, 0.3926991f, 1, 19, 0, 0};
            const bool accepted = rules::beginPolarInterpolation(polar, interpolation, duration, mode,
                std::bit_cast<float>(angle), std::bit_cast<float>(speed), flags);
            std::cout << identifier << ' ' << accepted << ' ' << std::bit_cast<std::uint32_t>(polar.angle) << ' '
                      << std::bit_cast<std::uint32_t>(polar.speed) << ' ' << polar.modeFlags << ' '
                      << std::bit_cast<std::uint32_t>(interpolation.startAngle) << ' '
                      << std::bit_cast<std::uint32_t>(interpolation.startSpeed) << ' '
                      << std::bit_cast<std::uint32_t>(interpolation.targetAngle) << ' '
                      << std::bit_cast<std::uint32_t>(interpolation.targetSpeed) << ' '
                      << interpolation.duration << ' ' << interpolation.elapsed << ' '
                      << std::bit_cast<std::uint32_t>(interpolation.elapsedFloat) << '\n';
        } else if (operation == "linear-step") {
            std::uint32_t startAngle = 0;
            std::uint32_t startSpeed = 0;
            std::uint32_t targetAngle = 0;
            std::uint32_t targetSpeed = 0;
            rules::PolarInterpolation interpolation;
            std::cin >> startAngle >> startSpeed >> targetAngle >> targetSpeed >> interpolation.duration >> interpolation.elapsed;
            interpolation.startAngle = std::bit_cast<float>(startAngle);
            interpolation.startSpeed = std::bit_cast<float>(startSpeed);
            interpolation.targetAngle = std::bit_cast<float>(targetAngle);
            interpolation.targetSpeed = std::bit_cast<float>(targetSpeed);
            interpolation.elapsedFloat = static_cast<float>(interpolation.elapsed);
            rules::Polar polar{interpolation.startAngle, interpolation.startSpeed, 0};
            const bool accepted = rules::stepPolarInterpolation(polar, interpolation);
            std::cout << identifier << ' ' << accepted << ' ' << std::bit_cast<std::uint32_t>(polar.angle) << ' '
                      << std::bit_cast<std::uint32_t>(polar.speed) << ' ' << interpolation.duration << ' '
                      << interpolation.elapsed << ' ' << std::bit_cast<std::uint32_t>(interpolation.elapsedFloat) << '\n';
        } else if (operation == "spawn") {
            std::uint16_t opcode = 0;
            std::string subroutine;
            rules::SpawnParent parent;
            std::uint32_t parentX = 0;
            std::uint32_t parentY = 0;
            std::uint32_t positionX = 0;
            std::uint32_t positionY = 0;
            std::int32_t life = 0;
            std::int32_t score = 0;
            std::int32_t drop = 0;
            std::cin >> opcode >> subroutine >> parentX >> parentY >> parent.flags >> positionX >> positionY >> life >> score >> drop;
            for (auto& word : parent.inheritedWords) {
                std::cin >> word;
            }
            std::cin >> parent.contextTag;
            parent.combinedPosition = {std::bit_cast<float>(parentX), std::bit_cast<float>(parentY), 999};
            const auto request = rules::prepareSpawn(opcode, subroutine, parent, std::bit_cast<float>(positionX),
                                                     std::bit_cast<float>(positionY), life, score, drop);
            if (!request) {
                std::cout << identifier << " 0\n";
            } else {
                std::cout << identifier << " 1 " << std::bit_cast<std::uint32_t>(request->position.x) << ' '
                          << std::bit_cast<std::uint32_t>(request->position.y) << ' ' << std::bit_cast<std::uint32_t>(request->position.z)
                          << ' ' << request->life << ' ' << request->score << ' ' << request->drop << ' ' << request->mode;
                for (const auto word : request->inheritedWords) {
                    std::cout << ' ' << word;
                }
                std::cout << ' ' << request->contextTag << '\n';
            }
        } else if (operation == "cartesian" || operation == "motion-frame") {
            auto readPosition = []() {
                std::uint32_t positionX = 0;
                std::uint32_t positionY = 0;
                std::uint32_t positionZ = 0;
                std::cin >> positionX >> positionY >> positionZ;
                return rules::Position{std::bit_cast<float>(positionX), std::bit_cast<float>(positionY), std::bit_cast<float>(positionZ)};
            };
            auto writePosition = [](const rules::Position& position) {
                std::cout << ' ' << std::bit_cast<std::uint32_t>(position.x) << ' '
                          << std::bit_cast<std::uint32_t>(position.y) << ' ' << std::bit_cast<std::uint32_t>(position.z);
            };
            auto absolute = readPosition();
            auto relative = operation == "motion-frame" ? readPosition() : rules::Position{};
            auto combined = operation == "motion-frame" ? readPosition() : rules::Position{};
            std::uint32_t angle = 0;
            std::uint32_t speed = 0;
            std::uint32_t relativeAngle = 0;
            std::uint32_t relativeSpeed = 0;
            std::uint32_t flags = 0;
            std::cin >> angle >> speed;
            if (operation == "motion-frame") {
                std::cin >> relativeAngle >> relativeSpeed >> flags;
            }
            rules::Position absoluteVelocity;
            rules::Position relativeVelocity;
            rules::Position combinedVelocity;
            bool accepted = rules::advancePolarPosition(absolute, absoluteVelocity, {std::bit_cast<float>(angle), std::bit_cast<float>(speed), 0});
            if (operation == "motion-frame") {
                accepted = accepted && rules::advancePolarPosition(relative, relativeVelocity,
                    {std::bit_cast<float>(relativeAngle), std::bit_cast<float>(relativeSpeed), 0});
                accepted = accepted && rules::combinePosition(combined, combinedVelocity, absolute, relative, flags);
            }
            std::cout << identifier << ' ' << accepted;
            writePosition(absolute);
            writePosition(absoluteVelocity);
            if (operation == "motion-frame") {
                writePosition(relative);
                writePosition(relativeVelocity);
                writePosition(combined);
                writePosition(combinedVelocity);
            }
            std::cout << '\n';
        } else {
            std::fprintf(stderr, "unknown enemy query: %s\n", operation.c_str());
            return 2;
        }
        if (!std::cin) {
            return 2;
        }
        ++queries;
    }
    std::printf("enemy evidence queries passed: %zu\n", queries);
    return std::cin.eof() ? 0 : 2;
}
