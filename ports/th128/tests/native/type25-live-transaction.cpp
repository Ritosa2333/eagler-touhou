#include "game/Type25TransactionHost.hpp"
#include "game/BulletRules.hpp"
#include "animation/AnmLogic.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace {
using namespace th128::bullet;

std::vector<unsigned char> read(const char* path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) { std::cerr << "missing " << path << '\n'; std::exit(2); }
    return {std::istreambuf_iterator<char>(file), {}};
}

void require(bool condition, const std::string& message) {
    if (condition) return;
    std::cerr << "FAIL " << message << '\n';
    std::exit(1);
}

struct Fixture {
    Manager manager;
    BulletVisuals visuals;
    std::unordered_map<std::uint32_t, BulletVisuals::Visual> animations;
    unsigned randomCalls = 0;
    std::uint32_t randomState = 0x12345678u;

    Fixture(const std::vector<unsigned char>& animation, const Profile& sourceProfile) {
        std::string error;
        require(visuals.load(animation.data(), animation.size(), error), error);
        visuals.setRandomSources(
            [&] { ++randomCalls; randomState ^= randomState << 13; randomState ^= randomState >> 17; randomState ^= randomState << 5; return randomState; },
            [&] { ++randomCalls; return 7u; });
        require(manager.setConstructor([&](const Birth& birth, std::string& reason) {
            BulletVisuals::CreateInfo info;
            info.type = 8;
            info.color = birth.color;
            info.script = sourceProfile.script;
            info.identitySpriteRemap = sourceProfile.identitySpriteRemap();
            for (std::size_t index = 0; index < info.spriteRemap.size(); ++index) {
                const auto mapped = sourceProfile.remap(birth.color, std::int32_t(index));
                if (!mapped) { reason = "source sprite remap"; return false; }
                info.spriteRemap[index] = *mapped;
            }
            auto visual = visuals.create(info, reason);
            if (!visual) return false;
            return animations.emplace(birth.id, std::move(*visual)).second;
        }), "constructor");
        Emitter emitter = resetEmitter();
        emitter.type = 8;
        emitter.color = 3;
        emitter.count = 1;
        emitter.layers = 1;
        emitter.aimMode = 2;
        emitter.speed = emitter.endSpeed = 1.5f;
        emitter.positionX = 12;
        emitter.positionY = 34;
        emitter.positionZ = 5;
        require(manager.emit(emitter, sourceProfile, {0, 400, 0}, 0, error), error);
        randomCalls = 0;
    }

    Birth live() const {
        const auto snapshots = manager.snapshots();
        require(snapshots.size() == 1, "one live source bullet");
        return snapshots.front();
    }
};

Type25TransactionHost makeHost(Fixture& fixture, const Profile& target) {
    return Type25TransactionHost(
        [&fixture](const Manager::Conversion& request, std::string& error) {
            return fixture.manager.preflightConversion(request, error);
        },
        [&fixture](const Manager::Conversion& request, std::string& error) {
            return fixture.manager.commitConversion(request, error);
        },
        fixture.visuals, target, fixture.animations);
}

void checkPreserved(const Birth& before, const Birth& after) {    require(after.type == 25 && after.script == 102, "converted type/script");
    require(after.id == before.id && after.slot == before.slot && after.color == before.color,
            "id/slot/color preserved");
    require(after.state.position.x == before.state.position.x
            && after.state.position.y == before.state.position.y
            && after.state.position.z == before.state.position.z, "position preserved");
    require(after.state.velocity.x == before.state.velocity.x
            && after.state.velocity.y == before.state.velocity.y
            && after.state.velocity.z == before.state.velocity.z, "velocity preserved");
    require(after.angle == before.angle && after.speed == before.speed, "angle/speed preserved");
    require(after.age == before.age && after.lifeClock == before.lifeClock
            && after.lifeTime == before.lifeTime
            && after.grace == before.grace && after.cooldown == before.cooldown
            && after.extensionListEmpty == before.extensionListEmpty,
            "age/life/extension-list preserved");
    require(after.cancellationScript == before.cancellationScript, "cancellation preserved");
    require(after.state.activeExtensions == before.state.activeExtensions
            && after.state.emitterFlags == before.state.emitterFlags
            && after.state.transformSound == before.state.transformSound
            && after.state.motionState == before.state.motionState
            && after.state.animationInterrupt == before.state.animationInterrupt
            && after.state.extensionIndex == before.state.extensionIndex,
            "extension/motion/interrupt fields preserved");
    require(after.state.hitboxSize == 6.0f && after.state.auxiliarySize == 22.0f
            && after.state.profileValue == 2 && after.state.freezePolicy == 1,
            "type25 target fields");
    require((after.state.flags & ~0x10u) == (before.state.flags & ~0x10u),
            "only OR flags bit4 is allowed");
}
}

int main(int argc, char** argv) {
    require(argc == 4, "pass type8 profile, type25 profile and bullet.anm");
    std::string error;
    Profile sourceProfile, targetProfile;
    const auto sourceBytes = read(argv[1]);
    require(sourceProfile.loadTyped(8, sourceBytes.data(), sourceBytes.size(), error), error);
    const auto targetBytes = read(argv[2]);
    require(targetProfile.loadTyped(25, targetBytes.data(), targetBytes.size(), error), error);
    const auto animation = read(argv[3]);

    {
        Fixture fixture(animation, sourceProfile);
        const auto before = fixture.live();
        require(before.state.motionState == 1 && before.state.flags & 1u, "source active flags bit0");
        BulletVisuals::CreateInfo normalInfo;
        normalInfo.type = 25; normalInfo.color = before.color; normalInfo.script = 102;
        normalInfo.secondaryScript = 103; normalInfo.identitySpriteRemap = true;
        for (std::size_t index = 0; index < normalInfo.spriteRemap.size(); ++index) {
            const auto mapped = targetProfile.remap(before.color, std::int32_t(index));
            require(bool(mapped), "target remap"); normalInfo.spriteRemap[index] = *mapped;
        }
        auto normal = fixture.visuals.create(normalInfo, error);
        require(bool(normal), error);
        fixture.randomCalls = 0;
        const auto beforeCreate = fixture.visuals.createCount();
        auto host = makeHost(fixture, targetProfile);
        require(host.convert(before, error), error);
        require(fixture.visuals.createCount() == beforeCreate + 1,
                "conversion creates exactly one new Visual after prevalidation");
        const auto after = fixture.live();
        checkPreserved(before, after);
        require(after.state.flags & 1u, "+9a0 ownership is not state.flags bit0");
        const auto found = fixture.animations.find(before.id);
        require(found != fixture.animations.end() && fixture.animations.size() == 1, "in-place map owner");
        require(found->second.hasSecondary && found->second.secondary.state().script == 103,
                "+9a0 secondary owner is the converted Visual");
        require(found->second.primary.state().script == 102, "new primary owner");
        require(fixture.visuals.animationState(found->second) != nullptr, "new visual generation live");
        require(found->second.primary.state().clock + 1 == normal->primary.state().clock,
                "conversion must not take the normal-birth extra tick");
        require(found->second.secondary.state().clock + 1 == normal->secondary.state().clock,
                "secondary conversion must not take the normal-birth extra tick");
        require(!fixture.visuals.createConverted([&] {
            auto info = normalInfo; info.birthInterrupt = 8; return info;
        }(), error), "conversion forbids birthInterrupt");

        fixture.randomCalls = 0;
        const auto failedCreate = fixture.visuals.createCount();
        require(!host.convert(before, error) && error.find("active non-type25") != std::string::npos,
                "type25/already-converted rejection");
        require(fixture.randomCalls == 0 && fixture.visuals.createCount() == failedCreate,
                "failed preflight must not bind a new Visual");
    }

    {
        Fixture fixture(animation, sourceProfile);
        const auto before = fixture.live();
        fixture.animations.erase(before.id);
        fixture.randomCalls = 0;
        const auto failedCreate = fixture.visuals.createCount();
        auto host = makeHost(fixture, targetProfile);
        require(!host.convert(before, error) && error.find("source visual is missing") != std::string::npos,
                "missing old visual");
        require(fixture.randomCalls == 0 && fixture.visuals.createCount() == failedCreate,
                "old visual validation happens before new bind RNG");
        require(fixture.live().type == 8, "manager remains source on missing old visual");
    }

    {
        Fixture fixture(animation, sourceProfile);
        const auto before = fixture.live();
        require(fixture.manager.cancelAll([](Birth&, std::string&) { return true; }, error), error);
        fixture.randomCalls = 0;
        const auto failedCreate = fixture.visuals.createCount();
        auto host = makeHost(fixture, targetProfile);
        require(!host.convert(before, error) && error.find("active non-type25") != std::string::npos,
                "inactive bullet rejection");
        require(fixture.randomCalls == 0 && fixture.visuals.createCount() == failedCreate,
                "inactive rejection must be pre-RNG");
    }

    {
        Fixture fixture(animation, sourceProfile);
        const auto before = fixture.live();
        BulletVisuals::CreateInfo wrong;
        wrong.type = 8; wrong.color = before.color; wrong.script = 44;
        wrong.identitySpriteRemap = true;
        auto wrongVisual = fixture.visuals.create(wrong, error);
        require(bool(wrongVisual), error);
        fixture.animations.insert_or_assign(before.id, std::move(*wrongVisual));
        fixture.randomCalls = 0;
        const auto failedCreate = fixture.visuals.createCount();
        auto host = makeHost(fixture, targetProfile);
        require(!host.convert(before, error) && error.find("stale, faulted or mismatched") != std::string::npos,
                "old script mismatch rejection");
        require(fixture.randomCalls == 0 && fixture.visuals.createCount() == failedCreate,
                "old script mismatch is pre-RNG");
    }

    {
        Fixture fixture(animation, sourceProfile);
        const auto before = fixture.live();
        require(fixture.visuals.load(animation.data(), animation.size(), error), error);
        fixture.randomCalls = 0;
        const auto failedCreate = fixture.visuals.createCount();
        auto host = makeHost(fixture, targetProfile);
        require(!host.convert(before, error) && error.find("stale, faulted or mismatched") != std::string::npos,
                "stale generation rejection");
        require(fixture.randomCalls == 0 && fixture.visuals.createCount() == failedCreate,
                "stale generation is pre-RNG");
    }

    {
        Fixture fixture(animation, sourceProfile);
        const auto before = fixture.live();
        auto host = makeHost(fixture, targetProfile);
        require(fixture.manager.setUpdater([&](Birth& bullet, bool&, std::string& reason) {
            return host.convert(bullet, reason);
        }), "reentrancy updater");
        const auto beforeCreate = fixture.visuals.createCount();
        require(!fixture.manager.tick(error) && error.find("not reentrant") != std::string::npos,
                "conversion host must be called outside Manager::tick");
        require(fixture.visuals.createCount() == beforeCreate && fixture.live().type == 8,
                "reentrant rejection is pre-bind and preserves the live slot");
        (void)before;
    }

    std::cout << "PASS type25 live transaction preserves live fields, validates old owner pre-RNG, forbids extra tick, models secondary owner, and fails closed\n";
    return 0;
}
