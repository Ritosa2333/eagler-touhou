#include "game/BulletManager.hpp"
#include "game/BulletRules.hpp"
#include "game/BulletTickRules.hpp"
#include "game/BulletVisuals.hpp"
#include "game/IceContact.hpp"
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
        require(profile.loadTyped(13, profileBytes.data(), profileBytes.size(), error), error);
        require(profile.type == 13 && profile.script == 48 && profile.secondaryScript == 0 &&
                profile.tail.hitboxSize == 2.4f && profile.tail.auxiliarySize == 16.f &&
                profile.tail.value == 4 && profile.tail.freezePolicy == 0,
                "type13 original profile fields");
        require(!profile.identitySpriteRemap(), "type13 must use the color remap table");
        require(profile.remap(6, 0) == 246 && profile.remap(6, 1) == 326 &&
                profile.remap(6, 2) == 342 && profile.remap(6, 3) == 349,
                "type13 color6 sprite remap");
        require(profile.cancellationScript(6) == 349 && profile.cancellationScript(0) == 337,
                "type13 cancellation table");
        {
            auto corrupt = profileBytes;
            corrupt[0] = 47;
            require(!profile.loadTyped(13, corrupt.data(), corrupt.size(), error), "type13 script");
            corrupt = profileBytes;
            corrupt[0x11c] = 1;
            require(!profile.loadTyped(13, corrupt.data(), corrupt.size(), error), "type13 secondary");
            corrupt = profileBytes;
            corrupt[0x10c] = 3;
            require(!profile.loadTyped(13, corrupt.data(), corrupt.size(), error), "type13 appearance");
            corrupt = profileBytes;
            corrupt[0x110] = 8;
            require(!profile.loadTyped(13, corrupt.data(), corrupt.size(), error), "type13 selector");
            corrupt = profileBytes;
            corrupt[0x118] = 1;
            require(!profile.loadTyped(13, corrupt.data(), corrupt.size(), error), "type13 freeze");
            require(!profile.loadTyped(12, profileBytes.data(), profileBytes.size(), error), "type13 type guard");
        }
        std::vector<Birth> births;
        Manager manager;
        require(manager.setConstructor([&](const Birth& birth, std::string&) { births.push_back(birth); return true; }),
                "constructor");
        auto emitter = resetEmitter();
        emitter.type = 13;
        emitter.color = 6;
        emitter.count = 1;
        emitter.aimMode = 1;
        emitter.speed = emitter.endSpeed = 1.5f;
        require(manager.emit(emitter, profile, {0, 400, 0}, 0, error), error);
        require(births.size() == 1 && manager.activeCount() == 1, "type13 birth count");
        const auto& birth = births[0];
        require(birth.type == 13 && birth.script == 48 && birth.cancellationScript == 349,
                "type13 birth type/script/cancellation");
        require(birth.state.hitboxSize == 2.4f && birth.state.auxiliarySize == 16.f &&
                birth.state.profileValue == 4 && birth.state.freezePolicy == 0,
                "type13 birth profile values");
        require(th128::ice::canFreezeBullet(1, birth.state.freezePolicy, 0, false),
                "type13 freeze policy0 must remain eligible");
        BulletVisuals visuals;
        require(visuals.load(animation.data(), animation.size(), error), error);
        BulletVisuals::CreateInfo info;
        info.type = 13;
        info.color = 6;
        info.script = 48;
        for (int sprite = 0; sprite < 4; ++sprite) {
            const auto remapped = profile.remap(6, sprite);
            require(remapped.has_value(), "type13 remap");
            info.spriteRemap[std::size_t(sprite)] = *remapped;
        }
        info.identitySpriteRemap = profile.identitySpriteRemap();
        auto visual = visuals.create(info, error);
        require(visual.has_value(), error);
        require(!visual->hasSecondary && visual->primary.state().script == 48,
                "type13 single primary ANM");
        require(visuals.spriteInfo(246) != nullptr && visuals.spriteInfo(349) != nullptr,
                "type13 remapped sprites exist");
        require(visuals.advance(*visual, 1, error), error);
        TickInput tick;
        tick.type = 13;
        tick.motionState = 2;
        tick.lifeTime = 100;
        tick.flags = 0x13;
        tick.hasSprite = true;
        tick.spriteWidth = 16;
        tick.spriteHeight = 16;
        auto pending = preAnimationTick(tick);
        require(pending.has_value(), "type13 typed tick domain");
        std::puts("PASS type13 profile, birth, remap, single ANM, freeze and tick");
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
