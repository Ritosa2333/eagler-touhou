#include "game/BulletManager.hpp"
#include "game/BulletVisuals.hpp"
#include "game/BulletUpdateHost.hpp"
#include "game/BulletRules.hpp"
#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

using namespace th128::bullet;

static std::vector<unsigned char> readFile(const char* path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) throw std::runtime_error("missing input");
    return {std::istreambuf_iterator<char>(input), {}};
}
static void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

int main(int argc, char** argv) {
    try {
        require(argc == 3, "usage: type6-update profile.bin bullet.anm");
        const auto profileBytes = readFile(argv[1]);
        const auto animation = readFile(argv[2]);
        std::string error;
        Profile profile;
        require(profile.loadTyped(6, profileBytes.data(), profileBytes.size(), error), error);
        Manager manager;
        require(manager.setConstructor([](const Birth&, std::string&) { return true; }), "constructor");
        Emitter emitter;
        setTypeColor(emitter, 6, 0);
        setAngles(emitter, 0, 0);
        emitter.count = 1; emitter.layers = 1; emitter.aimMode = 2;
        emitter.speed = emitter.endSpeed = 1.5f;
        emitter.extension0.type = 2;
        require(manager.emit(emitter, profile, {0, 400, 0}, 0, error), error);
        auto birth = manager.snapshots().at(0);
        require(birth.type == 6 && birth.state.motionState == 2 && birth.state.extensionIndex == 1,
                "type6 extension2 birth must start in half-motion after the spawn backshift");
        BulletVisuals visuals;
        require(visuals.load(animation.data(), animation.size(), error), error);
        BulletVisuals::CreateInfo info;
        info.type = 6; info.color = 0; info.script = 41; info.birthInterrupt = birth.state.animationInterrupt;
        for (std::size_t index = 0; index < info.spriteRemap.size(); ++index) {
            const auto mapped = profile.remap(0, std::int32_t(index));
            require(mapped.has_value(), "type6 remap");
            info.spriteRemap[index] = *mapped;
        }
        auto visual = visuals.create(info, error);
        require(visual.has_value(), error);
        const auto start = birth.state.position;
        bool removed = false;
        require(updateBulletWithKnownMisses(birth, visuals, *visual, {0, 1000, 0}, 0, removed, error),
                error);
        require(!removed && birth.state.motionState == 2,
                "type6 first update accepts motionState2");
        require(birth.state.position.y > start.y && birth.state.position.y - start.y > .5f,
                "type6 first update applies the half-step movement");
        require(birth.lifeClock == 1 && birth.lifeTime == 1.f && birth.grace == 4,
                "type6 first update advances life/grace exactly once");
        require(birth.state.hitboxSize == 4.f && birth.state.auxiliarySize == 16.f &&
                (birth.state.flags & 0x10U),
                "type6 collision/profile metadata preserved");
        bool appeared = false;
        for (int frame = 1; frame < 16 && !appeared; ++frame) {
            require(updateBulletWithKnownMisses(birth, visuals, *visual, {0, 1000, 0}, 0, removed, error),
                    error);
            appeared = !removed && birth.state.motionState == 1;
        }
        require(appeared, "type6 appearance transition must reach motionState1 after two-scale update");
        require(visuals.animationState(*visual)->script == 41, "type6 script41 visual stays active");
        std::puts("PASS real Manager/BulletUpdateHost type6 first update");
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
