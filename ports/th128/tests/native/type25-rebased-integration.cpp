#include "game/StageWorld.hpp"
#include "game/BulletVisuals.hpp"
#include "game/Type25TransactionHost.hpp"
#include "../ecl-fixture.hpp"
#include <algorithm>
#include <array>
#include <bit>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

using namespace ecl_fixture;

namespace {
std::vector<unsigned char> read(const char* path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) { std::fprintf(stderr, "missing %s\n", path); std::exit(2); }
    return {std::istreambuf_iterator<char>(file), {}};
}
std::vector<th128::bullet::CallbackSnapshot> snapshots(const th128::bullet::Manager& manager) {
    std::vector<th128::bullet::CallbackSnapshot> result;
    for (std::size_t slot = 0; slot < th128::bullet::Manager::Capacity; ++slot)
        if (const auto snapshot = manager.callbackSnapshot(slot)) result.push_back(*snapshot);
    return result;
}
bool samePreserved(const th128::bullet::Birth& a, const th128::bullet::Birth& b) {
    return a.id == b.id && a.slot == b.slot && a.color == b.color
        && a.state.position.x == b.state.position.x && a.state.position.y == b.state.position.y
        && a.state.position.z == b.state.position.z
        && a.state.velocity.x == b.state.velocity.x && a.state.velocity.y == b.state.velocity.y
        && a.state.velocity.z == b.state.velocity.z
        && a.angle == b.angle && a.speed == b.speed && a.age == b.age
        && a.lifeClock == b.lifeClock && a.lifeTime == b.lifeTime && a.grace == b.grace
        && a.cooldown == b.cooldown && a.extensionListEmpty == b.extensionListEmpty
        && a.cancellationScript == b.cancellationScript
        && a.state.activeExtensions == b.state.activeExtensions
        && a.state.emitterFlags == b.state.emitterFlags && a.state.transformSound == b.state.transformSound
        && a.state.motionState == b.state.motionState
        && a.state.animationInterrupt == b.state.animationInterrupt
        && a.state.extensionIndex == b.state.extensionIndex
        && (a.state.flags & ~0x10u) == (b.state.flags & ~0x10u)
        && a.extension64.parameterR == b.extension64.parameterR
        && a.extension64.limit == b.extension64.limit
        && a.extension64.control == b.extension64.control
        && a.extension64.count == b.extension64.count
        && a.extension256.previous == b.extension256.previous
        && a.extension256.remaining == b.extension256.remaining
        && a.extension256.accumulator == b.extension256.accumulator
        && a.extension256.flags == b.extension256.flags
        && a.extension256.mode == b.extension256.mode
        && a.extension800000.speed == b.extension800000.speed
        && a.extension800000.angle == b.extension800000.angle
        && a.extension800000.position.x == b.extension800000.position.x
        && a.extension800000.position.y == b.extension800000.position.y
        && a.extension800000.position.z == b.extension800000.position.z;
}
struct Harness {
    th128::StageWorld world;
    th128::bullet::BulletVisuals visuals;
    th128::bullet::Profile type8, type25;
    std::unordered_map<std::uint32_t, th128::bullet::BulletVisuals::Visual> animations;
    std::vector<th128::bullet::Birth> precommit;
    std::vector<th128::bullet::Birth> committed;
    std::unique_ptr<th128::bullet::Type25TransactionHost> host;

    Harness(const std::vector<unsigned char>& type8Bytes, const std::vector<unsigned char>& type25Bytes,
            const std::vector<unsigned char>& animation, const std::vector<std::uint8_t>& programBytes,
            const char* name) {
        std::string error;
        require(type8.loadTyped(8, type8Bytes.data(), type8Bytes.size(), error), error);
        require(type25.loadTyped(25, type25Bytes.data(), type25Bytes.size(), error), error);
        require(visuals.load(animation.data(), animation.size(), error), error);
        unsigned randomCalls = 0;
        std::uint32_t randomState = 0x12345678u;
        visuals.setRandomSources([&] {
            ++randomCalls;
            randomState ^= randomState << 13;
            randomState ^= randomState >> 17;
            randomState ^= randomState << 5;
            return randomState;
        }, [&] { ++randomCalls; return 7u; });
        require(world.load(name, programBytes.data(), programBytes.size(), error), error);
        require(world.start(name, 1, 0), error);
        require(world.setAppearanceBoundsProvider([](int, int) -> std::optional<th128::enemy::Bounds> {
            return th128::enemy::Bounds{1, 1};
        }, error), error);
        require(world.configureBullets(type8Bytes.data(), type8Bytes.size(), {0, 400, 0},
            [&](const th128::bullet::Birth& birth, std::string& reason) {
                th128::bullet::BulletVisuals::CreateInfo info;
                info.type = 8;
                info.color = birth.color;
                info.script = type8.script;
                info.identitySpriteRemap = type8.identitySpriteRemap();
                for (std::size_t index = 0; index < info.spriteRemap.size(); ++index) {
                    const auto mapped = type8.remap(birth.color, std::int32_t(index));
                    if (!mapped) { reason = "source remap"; return false; }
                    info.spriteRemap[index] = *mapped;
                }
                auto visual = visuals.create(info, reason);
                if (!visual) return false;
                return animations.emplace(birth.id, std::move(*visual)).second;
            }, error), error);
        require(world.addBulletProfile(25, type25Bytes.data(), type25Bytes.size(), error), error);
        require(world.configureBulletUpdater([](th128::bullet::Birth&, bool&, std::string&) { return true; }, error), error);
    }

    void installHost() {
        std::string error;
        host = std::make_unique<th128::bullet::Type25TransactionHost>(
            [&](std::size_t slot) { return world.liveBulletBirth(slot); },
            [&](const th128::bullet::Manager::Conversion& request, std::string& reason) {
                return world.preflightBulletConversion(request, reason);
            },
            [&](const th128::bullet::Manager::Conversion& request, std::string& reason) {
                if (const auto birth = world.liveBulletBirth(request.slot)) precommit.push_back(*birth);
                const bool committedOk = world.commitBulletConversion(request, reason);
                if (committedOk) {
                    if (const auto birth = world.liveBulletBirth(request.slot)) committed.push_back(*birth);
                }
                return committedOk;
            }, visuals, type25, animations);
        require(world.setBulletConversionHost([&](th128::bullet::Birth& live, std::string& reason) {
            return host->convert(live, reason);
        }, error), error);
    }

    void installPerCallFail(const char* message) {
        std::string error;
        require(world.setBulletConversionHost([&](th128::bullet::Birth& live, std::string& reason) {
            th128::bullet::Type25TransactionHost transient(
                [&](std::size_t slot) { return world.liveBulletBirth(slot); },
                [&](const th128::bullet::Manager::Conversion& request, std::string& gateError) {
                    return world.preflightBulletConversion(request, gateError);
                },
                [&](const th128::bullet::Manager::Conversion&, std::string& gateError) {
                    gateError = message;
                    return false;
                }, visuals, type25, animations);
            return transient.convert(live, reason);
        }, error), error);
    }
};
}

int main(int argc, char** argv) {
    try {
        require(argc == 4, "pass type8 profile, type25 profile and bullet.anm");
        const auto type8 = read(argv[1]);
        const auto type25 = read(argv[2]);
        const auto animation = read(argv[3]);
        const auto emit = [](float x, float y) {
            return join({instruction(500, {0}), instruction(502, {0, 8, 10}),
                instruction(503, {0, floatBits(x), floatBits(y)}),
                instruction(505, {0, floatBits(0), floatBits(0)}),
                instruction(506, {0, 1, 1}), instruction(501, {0})});
        };

        {
            const auto data = program({{"main", join({
                instruction(44, {floatBits(.25f)}), instruction(45, {floatBits(-9981)}, 0, 1),
                instruction(44, {floatBits(.5f)}), instruction(45, {floatBits(-9980)}, 0, 1),
                instruction(529, {3}),
                emit(0.0f, 0.0f), emit(96.0f, 0.0f), emit(100.0f, 0.0f),
                instruction(304, {floatBits(-9981), floatBits(-9980)}, 0, 3),
                instruction(0, {}, 4), instruction(0, {}, 100)})}});
            Harness fixture(type8, type25, animation, data, "wait.ecl");
            fixture.installHost();
            std::string error;
            const auto beforeCreate = fixture.visuals.createCount();
            require(fixture.world.enableLocalEnemyPasses(error), error);
            auto live = snapshots(fixture.world.bullets());
            require(live.size() == 3, "three live bullets");
            int converted = 0;
            for (const auto& bullet : live) converted += bullet.type == 25;
            std::string boundaryDebug;
            for (const auto& bullet : live) boundaryDebug += "(" + std::to_string(bullet.type) + ","
                + std::to_string(bullet.position.x) + ")";
            require(converted == 2, "exact 96 included and 100 outside; got "
                + std::to_string(converted) + " " + boundaryDebug + " entity="
                + std::to_string(fixture.world.entities().at(0).x) + ","
                + std::to_string(fixture.world.entities().at(0).y));
            require(fixture.visuals.createCount() == beforeCreate + 5,
                    "three source Visuals and two converted Visuals");
            const auto afterFirstCreate = fixture.visuals.createCount();
            const auto variables = fixture.world.callbackVariables(fixture.world.entities().at(0).id);
            require(variables && std::bit_cast<float>(variables->at(0)) == .75f, "first pass adds once");
            const auto entity = fixture.world.entities().at(0).id;
            require(fixture.world.tick(), error);
            require(std::bit_cast<float>(fixture.world.callbackVariables(entity)->at(0)) == .75f,
                    "post-enable flag-clear tick does not re-add");
            require(fixture.world.tick(), error);
            require(std::bit_cast<float>(fixture.world.callbackVariables(entity)->at(0)) == 1.25f,
                    "second real tick delivers selector3");
            require(fixture.world.tick(), error);
            require(std::bit_cast<float>(fixture.world.callbackVariables(entity)->at(0)) == 1.75f,
                    "third real tick delivers selector3");
            require(fixture.world.tick(), error);
            require(std::bit_cast<float>(fixture.world.callbackVariables(entity)->at(0)) == 2.25f,
                    "fourth real tick delivers selector3");
            std::string liveDebug;
            for (const auto& bullet : snapshots(fixture.world.bullets()))
                liveDebug += "(" + std::to_string(bullet.type) + "," + std::to_string(bullet.position.x) + ")";
            require(fixture.visuals.createCount() == afterFirstCreate,
                    "already25 creates no new Visual; after=" + std::to_string(afterFirstCreate)
                    + " now=" + std::to_string(fixture.visuals.createCount()) + " live=" + liveDebug
                    + " entity=" + std::to_string(fixture.world.entities().at(0).x) + ","
                    + std::to_string(fixture.world.entities().at(0).y));
            require(fixture.world.cancelAllBullets([](th128::bullet::Birth&, std::string&) { return true; }, error), error);
            require(fixture.world.tick(), error);
            require(fixture.visuals.createCount() == afterFirstCreate, "inactive creates no new Visual");
            require(std::bit_cast<float>(fixture.world.callbackVariables(entity)->at(0)) == 2.75f,
                    "inactive real tick still adds once");
        }

        {
            const auto data = program({{"main", join({
                instruction(44, {floatBits(.25f)}), instruction(45, {floatBits(-9981)}, 0, 1),
                instruction(44, {floatBits(.5f)}), instruction(45, {floatBits(-9980)}, 0, 1),
                emit(0.0f, 0.0f), emit(96.0f, 0.0f), emit(100.0f, 0.0f),
                instruction(0, {}, 1), instruction(529, {3}),
                instruction(0, {}, 4), instruction(0, {}, 100)})}});
            Harness fixture(type8, type25, animation, data, "install.ecl");
            std::string error;
            require(fixture.world.enableLocalEnemyPasses(error), error);
            const auto before = fixture.world.bullets().snapshots();
            require(before.size() == 3, "prepass emitted source bullets");
            fixture.installHost();
            auto forged = before[0];
            forged.color = (forged.color + 1) & 15;
            const auto beforeStale = fixture.visuals.createCount();
            require(!fixture.host->convert(forged, error)
                    && error.find("stale or not the authoritative") != std::string::npos,
                    "stale caller is rejected");
            forged.type = 7;
            require(!fixture.host->convert(forged, error), "stale type is rejected");
            auto staleScript = before[0];
            staleScript.script += 1;
            require(!fixture.host->convert(staleScript, error), "stale script is rejected");
            require(fixture.visuals.createCount() == beforeStale, "stale checks are before bind");
            require(fixture.world.tick(), error);
            require(fixture.world.tick(), error);
            const auto after = fixture.world.bullets().snapshots();
            int converted = 0;
            for (const auto& bullet : after) {
                converted += bullet.type == 25;
                if (bullet.type != 25) continue;
                const auto source = std::find_if(fixture.precommit.begin(), fixture.precommit.end(), [&](const auto& birth) {
                    return birth.id == bullet.id;
                });
                const auto committed = std::find_if(fixture.committed.begin(), fixture.committed.end(), [&](const auto& birth) {
                    return birth.id == bullet.id;
                });
                require(source != fixture.precommit.end() && committed != fixture.committed.end()
                        && samePreserved(*source, *committed),
                        "selector3 preserves extension64 and extension800000 fields at commit; "
                        + std::to_string(source->extension64.parameterR) + "/"
                        + std::to_string(committed->extension64.parameterR) + " "
                        + std::to_string(source->extension800000.speed) + "/"
                        + std::to_string(committed->extension800000.speed));
                const auto visual = fixture.animations.find(bullet.id);
                require(visual != fixture.animations.end() && visual->second.hasSecondary
                        && visual->second.primary.diagnostics().pendingInterrupt == 0
                        && visual->second.secondary.diagnostics().pendingInterrupt == 0,
                        "fresh conversion VMs and +0x9a0 hasSecondary");
            }
            require(converted == 2, "post-profile install converts both in-range bullets");
            const auto variables = fixture.world.callbackVariables(fixture.world.entities().at(0).id);
            require(variables && std::bit_cast<float>(variables->at(0)) == .75f,
                    "post-profile install adds variable4 once");
        }

        {
            const auto data = program({{"main", join({
                instruction(529, {3}), emit(0.0f, 0.0f), instruction(0, {}, 100)})}});
            Harness fixture(type8, type25, animation, data, "prepass.ecl");
            std::string error;
            require(!fixture.world.enableLocalEnemyPasses(error)
                    && error.find("requires a bullet conversion host") != std::string::npos,
                    "unhandled selector3 in prepass fails closed");
            require(!fixture.world.tick() && fixture.world.error() == error,
                    "prepass gating fault is sticky");
        }

        {
            const auto data = program({{"main", join({
                instruction(529, {3}), emit(0.0f, 0.0f), instruction(0, {}, 100)})}});
            Harness fixture(type8, type25, animation, data, "commit-fault.ecl");
            fixture.installPerCallFail("injected rebased commit failure");
            std::string error;
            const auto beforeCreate = fixture.visuals.createCount();
            require(!fixture.world.enableLocalEnemyPasses(error)
                    && error.find("injected rebased commit failure") != std::string::npos,
                    "StageWorld commit failure fails closed");
            require(fixture.visuals.createCount() >= beforeCreate + 2,
                    "primary/secondary bind occurred before the StageWorld commit failure; before="
                    + std::to_string(beforeCreate) + " after=" + std::to_string(fixture.visuals.createCount()));
            const auto fault = error;
            require(!fixture.world.tick() && fixture.world.error() == fault,
                    "StageWorld fault is sticky for per-call host");
        }

        {
            const auto data = program({{"main", join({instruction(0, {}, 100)})}});
            th128::StageWorld lifecycle;
            std::string error;
            require(lifecycle.load("reset.ecl", data.data(), data.size(), error)
                    && lifecycle.start("reset.ecl", 1, 0), error);
            require(lifecycle.setBulletConversionHost([](th128::bullet::Birth&, std::string&) { return true; }, error),
                    "first host");
            lifecycle.reset();
            require(lifecycle.load("reset.ecl", data.data(), data.size(), error)
                    && lifecycle.start("reset.ecl", 1, 0), error);
            require(lifecycle.setBulletConversionHost([](th128::bullet::Birth&, std::string&) { return true; }, error),
                    "reset clears host lifetime");
        }

        std::puts("PASS rebased StageWorld/Manager selector3 real-tick install-order");
        return 0;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "FAIL %s\n", error.what());
        return 1;
    }
}
