#include "game/BulletManager.hpp"
#include "game/BulletVisuals.hpp"
#include "game/BulletUpdateHost.hpp"
#include "game/BulletCollisionRules.hpp"
#include "game/CancellationEffects.hpp"
#include "game/IceContact.hpp"
#include "engine/Random.hpp"
#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

using namespace th128::bullet;

static std::vector<unsigned char> read(const char* path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) throw std::runtime_error("missing input");
    return {std::istreambuf_iterator<char>(input), {}};
}
static void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

int main(int argc, char** argv) {
    try {
        require(argc == 3, "usage: type16-pipeline profile.bin bullet.anm");
        const auto profileBytes = read(argv[1]);
        const auto animation = read(argv[2]);
        require(profileBytes.size() == 288, "type16 profile byte length");
        std::string error;
        Profile profile;
        require(profile.loadTyped(16, profileBytes.data(), profileBytes.size(), error), error);
        require(profile.type == 16 && profile.script == 51 && profile.secondaryScript == 0,
                "type16 script/secondary");
        require(profile.tail.hitboxSize == 4.f && profile.tail.auxiliarySize == 16.f,
                "type16 hitbox/auxiliary");
        require(profile.tail.value == 3 && profile.tail.freezePolicy == 0,
                "type16 appearance/freeze");
        require(!profile.identitySpriteRemap(), "type16 color remap identity");
        require(profile.remap(6, 0) == 294 && profile.remap(6, 1) == 326 &&
                profile.remap(6, 2) == 342 && profile.remap(6, 3) == 349,
                "type16 color6 remap");
        require(profile.cancellationScript(0) == 337 && profile.cancellationScript(6) == 349 &&
                profile.cancellationScript(15) == 367,
                "type16 cancellation table");
        {
            auto corrupt = profileBytes;
            corrupt[0] = 42;
            require(!profile.loadTyped(16, corrupt.data(), corrupt.size(), error), "type16 wrong script");
            corrupt = profileBytes;
            corrupt[0x104] = corrupt[0x105] = corrupt[0x106] = corrupt[0x107] = 0;
            require(!profile.loadTyped(16, corrupt.data(), corrupt.size(), error), "type16 wrong hitbox");
            corrupt = profileBytes;
            corrupt[0x10c] = 4;
            require(!profile.loadTyped(16, corrupt.data(), corrupt.size(), error), "type16 wrong appearance");
            corrupt = profileBytes;
            corrupt[0x110] = 8;
            require(!profile.loadTyped(16, corrupt.data(), corrupt.size(), error), "type16 wrong cancellation mode");
            require(!profile.loadTyped(15, profileBytes.data(), profileBytes.size(), error), "type16 type mismatch");
        }

        std::vector<Birth> births;
        Manager manager;
        require(manager.setConstructor([&](const Birth& birth, std::string&) {
            births.push_back(birth);
            return true;
        }), "type16 manager constructor");
        auto emitter = resetEmitter();
        emitter.type = 16;
        emitter.color = 6;
        emitter.count = 1;
        emitter.aimMode = 2;
        emitter.speed = emitter.endSpeed = 1.5f;
        require(manager.emit(emitter, profile, {0, 400, 0}, 0, error), error);
        require(births.size() == 1 && manager.activeCount() == 1, "type16 manager birth");
        auto birth = births.front();
        require(birth.type == 16 && birth.script == 51 && birth.cancellationScript == 349,
                "type16 birth profile fields");
        require(birth.state.hitboxSize == 4.f && birth.state.auxiliarySize == 16.f &&
                birth.state.profileValue == 3 && birth.state.freezePolicy == 0,
                "type16 birth metadata");
        emitter.type = 15;
        require(!manager.emit(emitter, profile, {0, 400, 0}, 0, error), "type16 profile mismatch rejected");

        BulletVisuals visuals;
        require(visuals.load(animation.data(), animation.size(), error), error);
        th128::Random random;
        random.seed = 0x1234;
        visuals.setRandomSources([&] { return random.next32(); }, [&] { return random.next32(); });
        BulletVisuals::CreateInfo info;
        info.type = 16;
        info.color = 6;
        info.script = 51;
        info.birthInterrupt = birth.state.animationInterrupt;
        for (std::size_t index = 0; index < info.spriteRemap.size(); ++index) {
            const auto mapped = profile.remap(6, std::int32_t(index));
            require(mapped.has_value(), "type16 visual remap");
            info.spriteRemap[index] = *mapped;
        }
        auto visual = visuals.create(info, error);
        require(visual.has_value(), error);
        require(visual->primary.state().script == 51 && !visual->hasSecondary,
                "type16 script51 visual/no secondary");

        const auto start = birth.state.position;
        bool removed = false;
        require(updateBulletWithKnownMisses(birth, visuals, *visual, {0, 1000, 0}, 0, removed, error), error);
        require(!removed, "type16 first update removed");
        require(birth.lifeClock == 1 && birth.lifeTime == 1.f && birth.grace == 4,
                "type16 first update life/grace");
        require(birth.state.position.y > start.y && birth.state.position.y - start.y > .5f,
                "type16 first update movement");
        require(birth.state.hitboxSize == 4.f && birth.state.auxiliarySize == 16.f,
                "type16 collision metadata preserved");
        const auto geometry = collision::circleGeometry(
            {birth.state.position.x, birth.state.position.y}, birth.state.hitboxSize,
            {birth.state.position.x, birth.state.position.y}, 0);
        require(geometry == collision::Geometry::PotentialHit, "type16 collision geometry");
        require(th128::ice::canFreezeBullet(1, profile.tail.freezePolicy, birth.lifeClock),
                "type16 freeze policy");
        require(!visual->primary.faulted(), "type16 script51 visual fault");

        th128::effects::CancellationEffects effects;
        require(effects.load(animation.data(), animation.size(), error), error);
        require(effects.setGameRandom([] { return 0x12345678u; }), "type16 cancellation RNG");
        const auto effect = effects.spawn(349, 320.f, 16.f, 0.f, 23, error);
        require(effect.has_value(), error);
        require(effects.rootCount() == 1 && effects.nodeCount() == 3,
                "type16 cancellation 349/348 family");
        std::puts("PASS type16 full Manager/Visual/Update/collision/freeze/cancellation pipeline");
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
