#include "game/BulletManager.hpp"
#include "game/BulletVisuals.hpp"
#include "game/BulletUpdateHost.hpp"
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <vector>

static std::vector<unsigned char> readFile(const char* path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) { std::fprintf(stderr, "missing %s\n", path); std::exit(2); }
    return {std::istreambuf_iterator<char>(file), {}};
}

int main(int argc, char** argv) {
    try {
        if (argc != 4) return 2;
        using namespace th128::bullet;
        const auto type8Bytes = readFile(argv[1]);
        const auto type25Bytes = readFile(argv[2]);
        const auto animation = readFile(argv[3]);
        std::string error;
        Profile type8, type25;
        if (!type8.loadTyped(8, type8Bytes.data(), type8Bytes.size(), error)
            || !type25.loadTyped(25, type25Bytes.data(), type25Bytes.size(), error)) {
            std::fprintf(stderr, "profile: %s\n", error.c_str());
            return 1;
        }
        if (type8.cancellationScript(10) != 357 || type25.cancellationScript(10) != 371) {
            std::fprintf(stderr, "unexpected cancellation profiles\n");
            return 1;
        }
        BulletVisuals visuals;
        if (!visuals.load(animation.data(), animation.size(), error)) return 1;
        visuals.setRandomSources([] { return 0x80000000u; }, [] { return 0u; });
        std::unordered_map<std::uint32_t, BulletVisuals::Visual> animations;
        Manager manager;
        unsigned cancellationCalls = 0;
        bool convertedReady = false;
        int consumedScript = -1;
        int consumedType = -1;
        if (!manager.setConstructor([&](const Birth& birth, std::string& reason) {
            BulletVisuals::CreateInfo info;
            info.type = 8;
            info.color = 10;
            info.script = 43;
            info.birthInterrupt = birth.state.animationInterrupt;
            for (std::size_t index = 0; index < info.spriteRemap.size(); ++index) {
                const auto mapped = type8.remap(10, std::int32_t(index));
                if (!mapped) { reason = "source remap"; return false; }
                info.spriteRemap[index] = *mapped;
            }
            auto visual = visuals.create(info, reason);
            if (!visual) return false;
            return animations.emplace(birth.id, std::move(*visual)).second;
        })) return 1;
        if (!manager.setUpdater([&](Birth& birth, bool& removed, std::string& reason) {
            if (!convertedReady) return true;
            const auto visual = animations.find(birth.id);
            if (visual == animations.end()) { reason = "missing converted visual"; return false; }
            return updateBulletWithKnownMisses(birth, visuals, visual->second, {0, 400}, 2, removed, reason, {},
                [](const Birth&, std::string&) -> std::optional<int> { return 1; },
                [&](const Birth& hit, bool, std::string&) {
                    ++cancellationCalls;
                    consumedScript = hit.cancellationScript;
                    consumedType = hit.type;
                    return true;
                });
        })) return 1;
        auto emitter = resetEmitter();
        emitter.type = 8;
        emitter.color = 10;
        emitter.speed = 1;
        emitter.endSpeed = 1;
        emitter.positionX = .125f;
        emitter.positionY = 399;
        emitter.extension0.type = 0;
        emitter.extension0.parameterA = 1;
        emitter.extension0.control = 1;
        if (!manager.emit(emitter, type8, {0, 400}, 0, error)) {
            std::fprintf(stderr, "emit: %s\n", error.c_str());
            return 1;
        }
        const auto source = manager.snapshots().front();
        if (source.cancellationScript != 357 || source.type != 8) {
            std::fprintf(stderr, "source cancellation cache=%d\n", source.cancellationScript);
            return 1;
        }
        if (!manager.tick(error)) {
            std::fprintf(stderr, "source tick: %s\n", error.c_str());
            return 1;
        }
        BulletVisuals::CreateInfo convertedInfo;
        convertedInfo.type = 25;
        convertedInfo.color = 10;
        convertedInfo.script = 102;
        convertedInfo.secondaryScript = 103;
        convertedInfo.identitySpriteRemap = true;
        auto convertedVisual = visuals.createConverted(convertedInfo, error);
        if (!convertedVisual) {
            std::fprintf(stderr, "createConverted: %s\n", error.c_str());
            return 1;
        }
        const Manager::Conversion request{source.slot, source.id};
        if (!manager.commitConversion(request, error)) {
            std::fprintf(stderr, "commitConversion: %s\n", error.c_str());
            return 1;
        }
        animations[source.id] = std::move(*convertedVisual);
        convertedReady = true;
        const auto converted = manager.snapshots().front();
        if (converted.type != 25 || converted.script != 102 || converted.cancellationScript != 357) {
            std::fprintf(stderr, "converted cache/type mismatch script=%d type=%d\n",
                         converted.cancellationScript, converted.type);
            return 1;
        }
        for (int tick = 0; tick < 20 && cancellationCalls == 0; ++tick) {
            if (!manager.tick(error)) {
                std::fprintf(stderr, "tick %d: %s\n", tick, error.c_str());
                return 1;
            }
        }
        if (cancellationCalls != 1 || consumedScript != 357 || consumedType != 25) {
            std::fprintf(stderr, "cancellation consumer calls=%u script=%d type=%d\n",
                         cancellationCalls, consumedScript, consumedType);
            return 1;
        }
        std::puts("PASS converted type25 cancellation consumer keeps raw +0xa1c 357, not target 371");
        return 0;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "FAIL %s\n", error.what());
        return 1;
    }
}
