#include "game/StageWorld.hpp"
#include "game/BulletVisuals.hpp"
#include "game/Type25TransactionHost.hpp"
#include "../ecl-fixture.hpp"
#include <algorithm>
#include <bit>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>
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
        && (a.state.flags & ~0x10u) == (b.state.flags & ~0x10u);
}
}

int main(int argc, char** argv) {
    try {
        require(argc == 4, "pass type8 profile, type25 profile and bullet.anm");
        const auto type8Bytes = read(argv[1]);
        const auto type25Bytes = read(argv[2]);
        const auto animation = read(argv[3]);
        std::string error;
        th128::bullet::Profile type8, type25;
        require(type8.loadTyped(8, type8Bytes.data(), type8Bytes.size(), error), error);
        require(type25.loadTyped(25, type25Bytes.data(), type25Bytes.size(), error), error);
        th128::bullet::BulletVisuals visuals;
        require(visuals.load(animation.data(), animation.size(), error), error);
        unsigned randomCalls = 0;
        std::uint32_t randomState = 0x2468aceu;
        visuals.setRandomSources(
            [&] { ++randomCalls; randomState ^= randomState << 13; randomState ^= randomState >> 17; randomState ^= randomState << 5; return randomState; },
            [&] { ++randomCalls; return 9u; });
        std::unordered_map<std::uint32_t, th128::bullet::BulletVisuals::Visual> animations;

        const auto emit = [](float x, float y) {
            return join({instruction(500, {0}), instruction(502, {0, 8, 10}),
                instruction(503, {0, floatBits(x), floatBits(y)}),
                instruction(505, {0, floatBits(0), floatBits(0)}),
                instruction(506, {0, 1, 1}), instruction(501, {0})});
        };
        const auto data = program({{"main", join({
            instruction(44, {floatBits(.25f)}), instruction(45, {floatBits(-9981)}, 0, 1),
            instruction(44, {floatBits(.5f)}), instruction(45, {floatBits(-9980)}, 0, 1),
            instruction(529, {3}),
            emit(0.0f, 0.0f), emit(96.0f, 0.0f), emit(96.0001f, 0.0f),
            instruction(304, {floatBits(-9981), floatBits(-9980)}, 0, 3),
            instruction(0, {}, 1),
            instruction(529, {3}), instruction(0, {}, 1),
            instruction(529, {3}), instruction(0, {}, 1),
            instruction(529, {0}), instruction(0, {}, 100)})}});
        th128::StageWorld world;
        require(world.load("selector3.ecl", data.data(), data.size(), error), error);
        require(world.start("selector3.ecl", 1, 0), error);
        require(world.setAppearanceBoundsProvider([](int, int) -> std::optional<th128::enemy::Bounds> {
            return th128::enemy::Bounds{1, 1};
        }, error), error);
        require(world.configureBullets(type8Bytes.data(), type8Bytes.size(), {0, 400, 0},
            [&](const th128::bullet::Birth& birth, std::string& reason) {
                th128::bullet::BulletVisuals::CreateInfo info;
                info.type = 8; info.color = birth.color; info.script = type8.script;
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
        th128::bullet::Type25TransactionHost host(
            [&](std::size_t slot) { return world.liveBulletBirth(slot); },
            [&](const th128::bullet::Manager::Conversion& request, std::string& reason) {
                return world.preflightBulletConversion(request, reason);
            },
            [&](const th128::bullet::Manager::Conversion& request, std::string& reason) {
                return world.commitBulletConversion(request, reason);
            }, visuals, type25, animations);
        require(world.setBulletConversionHost([&](th128::bullet::Birth& live, std::string& reason) {
            return host.convert(live, reason);
        }, error), error);
        th128::bullet::BulletVisuals::CreateInfo normalInfo;
        normalInfo.type = 25; normalInfo.color = 10; normalInfo.script = 102;
        normalInfo.secondaryScript = 103; normalInfo.identitySpriteRemap = true;
        auto normal = visuals.create(normalInfo, error);
        require(bool(normal), error);
        const auto normalPrimaryClock = normal->primary.state().clock;
        const auto normalSecondaryClock = normal->secondary.state().clock;
        require(world.configureBulletUpdater([](th128::bullet::Birth&, bool&, std::string&) { return true; }, error), error);
        const auto beforeCreate = visuals.createCount();
        require(world.enableLocalEnemyPasses(error), error);
        auto live = snapshots(world.bullets());
        require(live.size() == 3, "selector3 must see all three live bullets");
        int type25Count = 0;
        for (const auto& bullet : live) {
            require(bullet.id != 0 && bullet.slot < th128::bullet::Manager::Capacity, "stable slot/id");
            type25Count += bullet.type == 25;
            if (bullet.type == 25) {
                const auto found = animations.find(bullet.id);
                require(found != animations.end() && found->second.hasSecondary,
                        "converted bullet owns primary/secondary Visual");
                require(found->second.primary.state().script == 102
                        && found->second.secondary.state().script == 103,
                        "converted primary102 + secondary103");
                require(found->second.primary.diagnostics().pendingInterrupt == 0
                        && found->second.secondary.diagnostics().pendingInterrupt == 0,
                        "fresh converted VMs discard the old pending interrupt");
                require(found->second.primary.state().clock + 1 == normalPrimaryClock
                        && found->second.secondary.state().clock + 1 == normalSecondaryClock,
                        "conversion has no normal-birth extra tick");
            }
        }
        std::string debug;
        for (const auto& bullet : live) debug += "(" + std::to_string(bullet.position.x) + ","
            + std::to_string(bullet.position.y) + ",t" + std::to_string(bullet.type) + ")";
        require(type25Count == 2, "exact 96 boundary included; 97 outside excluded; got "
            + std::to_string(type25Count) + " bullets at " + debug);
        require(visuals.createCount() > beforeCreate, "eligible sources create converted visuals");
        const auto afterFirstCreate = visuals.createCount();
        const auto firstVariables = world.callbackVariables(world.entities().at(0).id);
        require(firstVariables.has_value(), "entity callback variables");
        require(std::bit_cast<float>(firstVariables->at(0)) == .75f,
                "selector3 adds variable5 to variable4 exactly once");
        require(std::bit_cast<float>(firstVariables->at(1)) == .5f, "variable5 retained");

        require(world.runEnemyCallbackForTest(world.entities().at(0).id, error), error);
        live = snapshots(world.bullets());
        require(live.size() == 3 && std::count_if(live.begin(), live.end(), [](const auto& b) { return b.type == 25; }) == 2,
                "already-converted type25 bullets are skipped on the next selector3");
        require(visuals.createCount() == afterFirstCreate, "already25 must not create a new visual");
        require(std::bit_cast<float>(world.callbackVariables(world.entities().at(0).id)->at(0)) == 1.25f,
                "second selector3 still increments variable4 once; got "
                + std::to_string(std::bit_cast<float>(world.callbackVariables(world.entities().at(0).id)->at(0))));

        require(world.cancelAllBullets([](th128::bullet::Birth&, std::string&) { return true; }, error), error);
        require(world.runEnemyCallbackForTest(world.entities().at(0).id, error), error);
        live = snapshots(world.bullets());
        require(live.size() == 3 && std::count_if(live.begin(), live.end(), [](const auto& b) { return b.type == 25; }) == 2,
                "inactive motionState bullets are skipped");
        require(visuals.createCount() == afterFirstCreate, "inactive bullets must not create a new visual");
        require(std::bit_cast<float>(world.callbackVariables(world.entities().at(0).id)->at(0)) == 1.75f,
                "inactive pass still increments variable4 once");

        for (int tick = 0; tick < 8 && world.callbackSelector(world.entities().at(0).id) != 0; ++tick)
            require(world.tick(), error);
        require(world.callbackSelector(world.entities().at(0).id) == 0, "selector0 clears the callback");
        require(std::bit_cast<float>(world.callbackVariables(world.entities().at(0).id)->at(0)) == 1.75f,
                "selector0 does not increment variable4");

        {
            const auto preserveData = program({{"main", join({
                instruction(44, {floatBits(.25f)}), instruction(45, {floatBits(-9981)}, 0, 1),
                instruction(44, {floatBits(.5f)}), instruction(45, {floatBits(-9980)}, 0, 1),
                emit(0.0f, 0.0f), emit(96.0f, 0.0f), emit(96.0001f, 0.0f),
                instruction(304, {floatBits(-9981), floatBits(-9980)}, 0, 3),
                instruction(0, {}, 100)})}});
            th128::bullet::BulletVisuals preserveVisuals;
            require(preserveVisuals.load(animation.data(), animation.size(), error), error);
            std::unordered_map<std::uint32_t, th128::bullet::BulletVisuals::Visual> preserveAnimations;
            th128::StageWorld preserve;
            require(preserve.load("preserve.ecl", preserveData.data(), preserveData.size(), error), error);
            require(preserve.start("preserve.ecl", 1, 0), error);
            require(preserve.setAppearanceBoundsProvider([](int, int) -> std::optional<th128::enemy::Bounds> {
                return th128::enemy::Bounds{1, 1};
            }, error), error);
            require(preserve.configureBullets(type8Bytes.data(), type8Bytes.size(), {0, 400, 0},
                [&](const th128::bullet::Birth& birth, std::string& reason) {
                    th128::bullet::BulletVisuals::CreateInfo info;
                    info.type = 8; info.color = birth.color; info.script = type8.script;
                    info.identitySpriteRemap = type8.identitySpriteRemap();
                    for (std::size_t index = 0; index < info.spriteRemap.size(); ++index) {
                        const auto mapped = type8.remap(birth.color, std::int32_t(index));
                        if (!mapped) { reason = "preserve source remap"; return false; }
                        info.spriteRemap[index] = *mapped;
                    }
                    auto visual = preserveVisuals.create(info, reason);
                    if (!visual) return false;
                    return preserveAnimations.emplace(birth.id, std::move(*visual)).second;
                }, error), error);
            require(preserve.addBulletProfile(25, type25Bytes.data(), type25Bytes.size(), error), error);
            th128::bullet::Type25TransactionHost preserveHost(
                [&](std::size_t slot) { return preserve.liveBulletBirth(slot); },
                [&](const th128::bullet::Manager::Conversion& request, std::string& reason) {
                    return preserve.preflightBulletConversion(request, reason);
                },
                [&](const th128::bullet::Manager::Conversion& request, std::string& reason) {
                    return preserve.commitBulletConversion(request, reason);
                }, preserveVisuals, type25, preserveAnimations);
            require(preserve.setBulletConversionHost([&](th128::bullet::Birth& live, std::string& reason) {
                return preserveHost.convert(live, reason);
            }, error), error);
            require(preserve.configureBulletUpdater([](th128::bullet::Birth&, bool&, std::string&) { return true; }, error), error);
            require(preserve.enableLocalEnemyPasses(error), error);
            const auto before = preserve.bullets().snapshots();
            require(before.size() == 3, "preserve source snapshots");
            auto forged = before[0];
            forged.color = (forged.color + 1) & 15;
            const auto beforeStaleCreate = preserveVisuals.createCount();
            require(!preserveHost.convert(forged, error)
                    && error.find("stale or not the authoritative") != std::string::npos,
                    "stale caller/live mismatch is rejected");
            forged.type = 7;
            require(!preserveHost.convert(forged, error),
                    "stale caller type mismatch is rejected");
            auto staleScript = before[0];
            staleScript.script += 1;
            require(!preserveHost.convert(staleScript, error),
                    "stale caller script mismatch is rejected");
            require(preserveVisuals.createCount() == beforeStaleCreate,
                    "stale caller rejection is before new bind");
            th128::bullet::Type25TransactionHost commitFailHost(
                [&](std::size_t slot) { return preserve.liveBulletBirth(slot); },
                [&](const th128::bullet::Manager::Conversion& request, std::string& reason) {
                    return preserve.preflightBulletConversion(request, reason);
                },
                [](const th128::bullet::Manager::Conversion&, std::string& reason) {
                    reason = "injected transaction commit failure";
                    return false;
                }, preserveVisuals, type25, preserveAnimations);
            require(!commitFailHost.convert(before[0], error) && commitFailHost.faulted(),
                    "commit failure after bind becomes sticky");
            require(!commitFailHost.convert(before[0], error) && error == commitFailHost.fault(),
                    "sticky transaction refuses a second attempt");
            require(preserve.bullets().snapshots()[0].type == 8,
                    "failed commit leaves the authoritative slot unchanged");
            th128::bullet::Type25TransactionHost postCommitFailHost(
                [&](std::size_t slot) { return preserve.liveBulletBirth(slot); },
                [&](const th128::bullet::Manager::Conversion& request, std::string& reason) {
                    return preserve.preflightBulletConversion(request, reason);
                },
                [&](const th128::bullet::Manager::Conversion& request, std::string& reason) {
                    return preserve.commitBulletConversion(request, reason);
                }, preserveVisuals, type25, preserveAnimations);
            postCommitFailHost.setFailAfterCommitForTest(true);
            require(!postCommitFailHost.convert(before[1], error) && postCommitFailHost.faulted(),
                    "post-commit failure becomes sticky");
            require(preserve.bullets().snapshots()[1].type == 25,
                    "post-commit failure leaves the committed live type25");
            require(preserve.setCallbackSelectorForTest(preserve.entities().at(0).id, 3), "preserve selector3");
            require(preserve.runEnemyCallbackForTest(preserve.entities().at(0).id, error), error);
            const auto after = preserve.bullets().snapshots();
            for (const auto& converted : after) {
                if (converted.type != 25) continue;
                const auto original = std::find_if(before.begin(), before.end(), [&](const auto& birth) {
                    return birth.id == converted.id;
                });
                require(original != before.end() && samePreserved(*original, converted),
                        "selector3 preserves motion/life/velocity/extensions/color/id/slot");
            }
        }

        {
            const auto faultData = program({{"main", join({
                instruction(529, {3}), emit(0.0f, 0.0f), instruction(0, {}, 100)})}});
            th128::bullet::BulletVisuals faultVisuals;
            require(faultVisuals.load(animation.data(), animation.size(), error), error);
            std::unordered_map<std::uint32_t, th128::bullet::BulletVisuals::Visual> faultAnimations;
            th128::StageWorld faultWorld;
            require(faultWorld.load("fault.ecl", faultData.data(), faultData.size(), error), error);
            require(faultWorld.start("fault.ecl", 1, 0), error);
            require(faultWorld.configureBullets(type8Bytes.data(), type8Bytes.size(), {0, 400, 0},
                [&](const th128::bullet::Birth& birth, std::string& reason) {
                    th128::bullet::BulletVisuals::CreateInfo info;
                    info.type = 8; info.color = birth.color; info.script = type8.script;
                    info.identitySpriteRemap = type8.identitySpriteRemap();
                    for (std::size_t index = 0; index < info.spriteRemap.size(); ++index) {
                        const auto mapped = type8.remap(birth.color, std::int32_t(index));
                        if (!mapped) { reason = "fault source remap"; return false; }
                        info.spriteRemap[index] = *mapped;
                    }
                    auto visual = faultVisuals.create(info, reason);
                    if (!visual) return false;
                    return faultAnimations.emplace(birth.id, std::move(*visual)).second;
                }, error), error);
            require(faultWorld.addBulletProfile(25, type25Bytes.data(), type25Bytes.size(), error), error);
            th128::bullet::Type25TransactionHost faultHost(
                [&](std::size_t slot) { return faultWorld.liveBulletBirth(slot); },
                [&](const th128::bullet::Manager::Conversion& request, std::string& reason) {
                    return faultWorld.preflightBulletConversion(request, reason);
                },
                [](const th128::bullet::Manager::Conversion&, std::string& reason) {
                    reason = "injected StageWorld transaction commit failure";
                    return false;
                }, faultVisuals, type25, faultAnimations);
            require(faultWorld.setBulletConversionHost([&](th128::bullet::Birth& live, std::string& reason) {
                return faultHost.convert(live, reason);
            }, error), error);
            require(faultWorld.configureBulletUpdater([](th128::bullet::Birth&, bool&, std::string&) { return true; }, error), error);
            const auto beforeFaultCreate = faultVisuals.createCount();
            require(!faultWorld.enableLocalEnemyPasses(error)
                    && error.find("injected StageWorld transaction commit failure") != std::string::npos,
                    "StageWorld selector3 fails closed on commit failure after bind");
            require(faultVisuals.createCount() == beforeFaultCreate + 2 && faultHost.faulted(),
                    "bind occurred before the injected commit failure and latched the host");
            const auto firstFault = error;
            require(!faultWorld.tick() && faultWorld.error() == firstFault,
                    "StageWorld remains faulted on repeated tick after commit failure");
        }

        {
            const auto resetData = program({{"main", join({instruction(0, {}, 100)})}});
            th128::StageWorld lifecycle;
            require(lifecycle.load("reset.ecl", resetData.data(), resetData.size(), error)
                    && lifecycle.start("reset.ecl", 1, 0), error);
            require(lifecycle.setBulletConversionHost([](th128::bullet::Birth&, std::string&) { return true; }, error),
                    "first conversion host");
            lifecycle.reset();
            require(lifecycle.load("reset.ecl", resetData.data(), resetData.size(), error)
                    && lifecycle.start("reset.ecl", 1, 0), error);
            require(lifecycle.setBulletConversionHost([](th128::bullet::Birth&, std::string&) { return true; }, error),
                    "reset clears the stored conversion host lifetime");
        }

        {
            const auto bad = program({{"main", join({instruction(529, {4}), instruction(257, stringWords("child"), 0, 0, 255, 6),
                instruction(0, {}, 100)})}, {"child", join({instruction(0, {}, 100)})}});
            th128::StageWorld rejected;
            require(rejected.load("bad.ecl", bad.data(), bad.size(), error) && rejected.start("bad.ecl", 1, 0), error);
            require(!rejected.enableLocalEnemyPasses(error), "unknown selector must fail explicitly");
            require(error.find("unsupported native enemy-update callback selector") != std::string::npos, error);
        }
        std::puts("PASS real StageWorld/Manager ECL529 selector3 transaction");
        return 0;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "FAIL %s\n", error.what());
        return 1;
    }
}
