#include "game/BulletExtension16.hpp"
#include "game/BulletRules.hpp"
#include "../ecl-fixture.hpp"
#include <cmath>
#include <cstdio>
#include <exception>

using namespace ecl_fixture;
using namespace th128::bullet;

namespace {
ExtensionRaw mbossRecord() {
    ExtensionRaw record;
    record.control = 0;
    record.type = 16;
    record.parameterA = 60;
    record.parameterB = 1;
    record.parameterC = 0;
    record.parameterD = 0;
    record.parameterR = 2.094395160675049f;
    record.parameterS = 0.5f;
    record.parameterT = -999999.f;
    record.parameterU = -999999.f;
    return record;
}
}

int main() {
    try {
        const auto record = mbossRecord();
        const auto initialized = initializeExtension16(record, 3.f, .5f);
        require(initialized.has_value(), "actual C0/D0 type16 record must initialize");
        require(initialized->active && initialized->mode == 0 && initialized->angleMode == 0 &&
                initialized->period == 60 && initialized->limit == 1 &&
                initialized->targetSpeed == .5f && initialized->angleOffset == record.parameterR,
                "actual MBoss record fields must initialize unchanged");
        {
            auto state = *initialized;
            state.time = 0;
            state.fraction = 0;
            const auto elapsed = stepExtension16(state, 3.f, .5f, {1, 2, 0}, {0, 0, .1f}, {0, 400, 0}, -1);
            require(elapsed.has_value() && elapsed->speed == 3.f && elapsed->angle == .5f &&
                    std::abs(elapsed->velocity.x - 3.f * std::cos(.5f)) < 1e-6f &&
                    std::abs(elapsed->velocity.y - 3.f * std::sin(.5f)) < 1e-6f,
                    "mode0 pre-boundary must retain angle and interpolate base speed");
            auto late = *initialized;
            late.time = 59;
            late.fraction = 59;
            const auto almost = stepExtension16(late, 3.f, .5f, {1, 2, 0}, {0, 0, .1f}, {0, 400, 0}, -1);
            require(almost.has_value() && almost->speed == 3.f &&
                    std::abs(almost->velocity.x - .05f * std::cos(.5f)) < 1e-6f &&
                    std::abs(almost->velocity.y - .05f * std::sin(.5f)) < 1e-6f,
                    "mode0 last pre-boundary velocity must use base*59/60 while retaining base speed");
        }
        {
            auto boundary = *initialized;
            boundary.time = 60;
            boundary.fraction = 60;
            const auto step = stepExtension16(boundary, 3.f, .5f, {1, 2, 0}, {0, 0, .1f}, {0, 400, 0}, -1);
            const float expectedAngle = .5f + record.parameterR;
            require(step.has_value() && std::abs(step->angle - expectedAngle) < 1e-6f && step->speed == .5f &&
                    step->next.count == 1 && step->finished && !step->next.active &&
                    std::abs(step->velocity.x - .5f * std::cos(expectedAngle)) < 1e-6f &&
                    std::abs(step->velocity.y - .5f * std::sin(expectedAngle)) < 1e-6f,
                    "mode0 boundary must add R to current angle, restore target speed and finish at limit");
        }
        {
            auto sound = *initialized;
            sound.time = 60;
            sound.fraction = 60;
            const auto step = stepExtension16(sound, 3.f, .5f, {1, 2, 0}, {0, 0, .1f}, {0, 400, 0}, 40);
            require(step.has_value() && step->soundRequired && !step->finished &&
                    step->next.time == sound.time && step->next.count == sound.count,
                    "mode0 boundary must request its sound before changing state");
        }
        {
            auto mode2Record = record;
            mode2Record.parameterC = 2;
            mode2Record.parameterR = 0;
            const auto mode2 = initializeExtension16(mode2Record, 3.f, .5f);
            require(mode2.has_value() && mode2->mode == 2, "existing mode2 record must remain accepted");
            auto boundary = *mode2;
            boundary.time = 60;
            boundary.fraction = 60;
            const auto aim = targetAnglePrefix({0, 0, .1f}, {0, 400, 0});
            const auto step = stepExtension16(boundary, 3.f, .5f, {1, 2, 0}, {0, 0, .1f}, {0, 400, 0}, -1);
            require(aim.has_value() && step.has_value() && std::abs(step->angle - *aim) < 1e-6f && step->speed == .5f,
                    "mode2 boundary must retain targetAnglePrefix behavior");
        }
        {
            auto unknownMode = record;
            unknownMode.parameterC = 1;
            require(!initializeExtension16(unknownMode, 3.f, .5f), "unknown mode1 must remain rejected");
            auto callback = record;
            callback.callback = 1;
            require(!initializeExtension16(callback, 3.f, .5f), "nonzero callback must remain rejected");
            auto angleMode = record;
            angleMode.parameterD = 1;
            require(!initializeExtension16(angleMode, 3.f, .5f), "unknown angleMode1 must remain rejected");
        }
        std::puts("actual MBoss type16 mode0 initialization and boundary passed");
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
