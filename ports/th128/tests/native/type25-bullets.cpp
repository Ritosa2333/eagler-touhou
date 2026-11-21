#include "game/BulletManager.hpp"
#include "game/BulletRules.hpp"
#include "game/BulletTickRules.hpp"
#include "game/BulletVisuals.hpp"
#include "game/BulletUpdateHost.hpp"
#include "game/IceContact.hpp"
#include "animation/AnmLogic.hpp"
#include "engine/Random.hpp"
#include <bit>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

using namespace th128::bullet;

static std::vector<unsigned char> read(const char* path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("missing input");
    return {std::istreambuf_iterator<char>(file), {}};
}
static void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

int main(int argc, char** argv) {
    try {
        require(argc == 3, "usage: profile bullet.anm");
        const auto profileBytes = read(argv[1]);
        const auto animation = read(argv[2]);
        std::string error;
        Profile profile;
        require(profile.loadTyped(25, profileBytes.data(), profileBytes.size(), error), error);
        require(profile.type == 25 && profile.script == 102 && profile.secondaryScript == 103,
                "profile script/secondary");
        require(profile.tail.hitboxSize == 6.f && profile.tail.auxiliarySize == 22.f,
                "profile size");
        require(profile.tail.value == 2 && profile.tail.freezePolicy == 1, "profile appearance/freeze");
        require(profile.identitySpriteRemap(), "type25 identity sprite sentinel");
        require(profile.remap(0, 457) == 457 && profile.remap(15, 0) == 0, "identity remap");
        require(profile.cancellationScript(0) == 0x173 && profile.cancellationScript(15) == 0x173,
                "type25 cancellation ANM");
        {
            auto corrupt = profileBytes;
            const auto copy = profile;
            corrupt[0] = 101;
            require(!profile.loadTyped(25, corrupt.data(), corrupt.size(), error) && profile.script == copy.script,
                    "corrupt primary script");
            corrupt = profileBytes;
            corrupt[0x11c] = 0;
            require(!profile.loadTyped(25, corrupt.data(), corrupt.size(), error), "missing secondary");
            corrupt = profileBytes;
            corrupt[0x110] = 6;
            require(!profile.loadTyped(25, corrupt.data(), corrupt.size(), error), "wrong selector");
            corrupt = profileBytes;
            corrupt[0x118] = 0;
            require(!profile.loadTyped(25, corrupt.data(), corrupt.size(), error), "wrong freeze policy");
            corrupt = profileBytes;
            corrupt[0x10c] = 3;
            require(!profile.loadTyped(25, corrupt.data(), corrupt.size(), error), "wrong appearance");
            corrupt = profileBytes;
            corrupt[4] = corrupt[5] = corrupt[6] = corrupt[7] = 0;
            require(!profile.loadTyped(25, corrupt.data(), corrupt.size(), error), "missing identity sentinel");
            require(!profile.loadTyped(25, profileBytes.data(), profileBytes.size() - 1, error), "size");
            require(!profile.loadTyped(24, profileBytes.data(), profileBytes.size(), error), "type");
        }
        std::vector<Birth> births;
        Manager manager;
        require(manager.setConstructor([&](const Birth& birth, std::string&) { births.push_back(birth); return true; }), "constructor");
        auto emitter = resetEmitter();
        emitter.type = 25; emitter.color = 0; emitter.count = 4; emitter.aimMode = 2;
        emitter.speed = emitter.endSpeed = 1.5f;
        emitter.extension0.type = 2; emitter.extension0.parameterA = 1;
        require(manager.emit(emitter, profile, {0, 400, 0}, 0, error), error);
        require(births.size() == 4 && manager.activeCount() == 4, "type25 birth count");
        for (const auto& birth : births) {
            require(birth.type == 25 && birth.script == 102 && birth.cancellationScript == 0x173,
                    "birth type/script/cancellation");
            require(birth.state.hitboxSize == 6.f && birth.state.auxiliarySize == 22.f,
                    "birth size");
            require(birth.state.profileValue == 2 && birth.state.freezePolicy == 1,
                    "birth appearance/freeze");
            require(birth.state.motionState == 2 && birth.state.animationInterrupt == 8,
                    "type2 extension birth state");
        }
        emitter.type = 8;
        require(!manager.emit(emitter, profile, {0, 400, 0}, 0, error), "profile type mismatch");
        require(!th128::ice::canFreezeBullet(0x13u, profile.tail.freezePolicy, 0) &&
                !th128::ice::canFreezeBullet(0x13u, profile.tail.freezePolicy, 60) &&
                !th128::ice::canFreezeBullet(0x13u, profile.tail.freezePolicy, 1000),
                "policy 1 remains unfreezable");
        BulletVisuals visuals;
        require(visuals.load(animation.data(), animation.size(), error), error);
        th128::Random gameRandom, displayRandom;
        gameRandom.seed = displayRandom.seed = 0x1234;
        visuals.setRandomSources([&] { return gameRandom.next32(); }, [&] { return displayRandom.next32(); });
        BulletVisuals::CreateInfo info;
        info.type = 25; info.color = 0; info.script = 102; info.secondaryScript = 103;
        info.identitySpriteRemap = true;
        auto visual = visuals.create(info, error);
        require(visual.has_value(), error);
        require(visual->primary.state().script == 102 && visual->secondary.state().script == 103,
                "primary/secondary ANM scripts");
        require(visual->hasSecondary && visual->primary.state().sprite == 457,
                "identity-mapped real primary sprite");
        {
            auto irqInfo = info;
            irqInfo.birthInterrupt = 8;
            auto irqVisual = visuals.create(irqInfo, error);
            require(irqVisual.has_value(), error);
            require(irqVisual->primary.state().clock >= 8 && irqVisual->hasSecondary,
                    "type2 birth interrupt reaches the primary only");
        }
        {
            auto noSecondary = info; noSecondary.secondaryScript.reset();
            require(!visuals.create(noSecondary, error), "type25 requires secondary");
            auto noIdentity = info; noIdentity.identitySpriteRemap = false;
            noIdentity.spriteRemap = {0, 1, 2, 3};
            require(!visuals.create(noIdentity, error), "type25 identity is explicit");
        }
        {
            th128::anm::Registry bank;
            require(bank.loadBank(0, animation.data(), animation.size(), error), error);
            th128::anm::VM primaryReference, secondaryReference;
            th128::anm::Runtime runtime;
            runtime.remapSprite = [](int source) { return source; };
            th128::Random referenceGame, referenceDisplay;
            referenceGame.seed = referenceDisplay.seed = 0x1234;
            runtime.gameRandom32 = [&] { return referenceGame.next32(); };
            runtime.displayRandom32 = [&] { return referenceDisplay.next32(); };
            require(primaryReference.bind(bank, 0, 102, runtime) &&
                    secondaryReference.bind(bank, 0, 103, runtime), "reference bind");
            primaryReference.tick(runtime);
            secondaryReference.tick(runtime);
            const auto compare = [&](const th128::anm::State& expected, const th128::anm::State& actual,
                                     const char* label) {
                require(expected.script == actual.script && expected.sprite == actual.sprite &&
                        expected.clock == actual.clock && expected.ticks == actual.ticks &&
                        expected.integers == actual.integers && expected.sx == actual.sx &&
                        expected.sy == actual.sy && expected.flags == actual.flags &&
                        expected.held == actual.held && expected.ended == actual.ended,
                        label);
            };
            compare(primaryReference.state(), visual->primary.state(), "primary create state");
            compare(secondaryReference.state(), visual->secondary.state(), "secondary create state");
            for (int tick = 0; tick < 90; ++tick) {
                require(visuals.advance(*visual, 1, error), error);
                primaryReference.tick(runtime);
                if (!primaryReference.state().ended && !primaryReference.state().deleteRequested)
                    secondaryReference.tick(runtime);
                compare(primaryReference.state(), visual->primary.state(), "primary tick state");
                compare(secondaryReference.state(), visual->secondary.state(), "secondary tick state");
            }
        }
        auto tailEmitter = resetEmitter();
        tailEmitter.type = 25; tailEmitter.color = 0; tailEmitter.aimMode = 2;
        tailEmitter.speed = tailEmitter.endSpeed = 1.5f;
        const auto tail = spawnTailPrefix(tailEmitter, profile.tail, {-128, 100, .1f}, {.5f, 1.5f, 0}, 0x13);
        require(tail.has_value(), "tail");
        Birth bullet;
        bullet.id = 1; bullet.slot = 0; bullet.type = 25; bullet.color = 0; bullet.script = 102;
        bullet.state = *tail; bullet.cancellationScript = 0x173; bullet.grace = 5;
        BulletVisuals updateVisuals;
        require(updateVisuals.load(animation.data(), animation.size(), error), error);
        th128::Random updateGame, updateDisplay;
        updateGame.seed = updateDisplay.seed = 0x5678;
        updateVisuals.setRandomSources([&] { return updateGame.next32(); }, [&] { return updateDisplay.next32(); });
        BulletVisuals::CreateInfo updateInfo = info;
        auto updateVisual = updateVisuals.create(updateInfo, error);
        require(updateVisual.has_value(), error);
        unsigned ticks = 0;
        bool removed = false;
        const GrazeCallback graze = [](const Birth&, std::string&) { return true; };
        const HitCallback hit = [](const Birth&, std::string&) { return std::optional<int>{0}; };
        const CancellationCallback cancellation = [](const Birth&, bool, std::string&) { return true; };
        const auto initialSecondaryClock = updateVisual->secondary.state().clock;
        while (ticks < 40 && !removed) {
            require(updateBulletWithKnownMisses(bullet, updateVisuals, *updateVisual, {0, 400, 0}, 0,
                    removed, error, graze, hit, cancellation), error);
            ++ticks;
        }
        require(!removed && ticks == 40, "no unproven lifetime removal is invented");
        require(updateVisual->secondary.state().clock >= initialSecondaryClock + 39,
                "secondary ANM ticks while primary remains active");
        {
            auto hitBullet = bullet;
            hitBullet.id = 4;
            hitBullet.state.position = {0, 100, 0};
            hitBullet.state.velocity = {0, 0, 0};
            auto hitVisual = updateVisuals.create(updateInfo, error);
            require(hitVisual.has_value(), error);
            bool cancelled = false;
            const HitCallback directHit = [](const Birth&, std::string&) { return std::optional<int>{1}; };
            const CancellationCallback directCancellation =
                [&](const Birth& birth, bool, std::string&) {
                    require(birth.cancellationScript == 0x173, "cancellation owner script");
                    cancelled = true;
                    return true;
                };
            bool hitRemoved = false;
            require(updateBulletWithKnownMisses(hitBullet, updateVisuals, *hitVisual, {0, 100, 0}, 0,
                    hitRemoved, error, graze, directHit, directCancellation), error);
            require(cancelled && !hitRemoved, "type25 hit cancellation keeps the owned two-VM visual");
        }
        {
            auto offscreen = bullet;
            offscreen.id = 2;
            offscreen.state.position = {500, 0, 0};
            offscreen.state.velocity = {0, 0, 0};
            offscreen.state.motionState = 1;
            offscreen.grace = 0;
            auto offscreenVisual = updateVisuals.create(updateInfo, error);
            require(offscreenVisual.has_value(), error);
            const auto clocks = offscreenVisual->secondary.state().clock;
            bool gone = false;
            require(updateBulletWithKnownMisses(offscreen, updateVisuals, *offscreenVisual, {0, 400, 0}, 0,
                    gone, error, graze, hit, cancellation), error);
            require(gone && offscreenVisual->secondary.state().clock == clocks,
                    "offscreen removal drops the whole two-VM owner without a secondary tick");
        }
        std::puts("PASS type25 profile, generic birth, identity ANM, secondary tick, freeze and removal");
        return 0;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "FAIL type25 bullets: %s\n", error.what());
        return 1;
    }
}
