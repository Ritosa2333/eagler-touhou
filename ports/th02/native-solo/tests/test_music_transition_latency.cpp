#include "audio_preview.hpp"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

std::vector<std::byte> read(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary | std::ios::ate);
    if (!input) throw std::runtime_error("Cannot open " + path.string());
    const auto size = input.tellg();
    if (size <= 0) throw std::runtime_error("Empty asset: " + path.string());
    std::vector<std::byte> bytes(static_cast<std::size_t>(size));
    input.seekg(0);
    if (!input.read(reinterpret_cast<char*>(bytes.data()), size)) throw std::runtime_error("Read failed");
    return bytes;
}
int wmain(int argc, wchar_t** argv) {
    try {
        if (argc != 3) throw std::runtime_error("Usage: test <asset directory> <soundfont>");
        const std::filesystem::path root(argv[1]);
        const auto sf = read(argv[2]);
        const auto effects = read(root / "HUUMA.EFC");
        for (bool midi : {false, true}) {
            const auto stage_fm = read(root / "STAGE0.M");
            const auto boss_fm = read(root / "BOSS1.M");
            const auto stage = midi ? read(root / "STAGE0.MMD") : stage_fm;
            const auto boss = midi ? read(root / "BOSS1.MMD") : boss_fm;
            for (int i = 0; i < 5; ++i) {
                AudioPreview audio;
                const auto font = midi ? std::span<const std::byte>(sf) : std::span<const std::byte>{};
                if (!audio.start(stage, true, WAVE_MAPPER, font, effects, stage_fm, false, 0)) throw std::runtime_error(audio.error());
                const auto t0 = std::chrono::steady_clock::now();
                const bool ok = audio.start(boss, true, WAVE_MAPPER, font, effects, boss_fm, false, 0);
                const auto t1 = std::chrono::steady_clock::now();
                if (!ok || !audio.active() || audio.queued() != 4) throw std::runtime_error("Transition failed: " + audio.error());
                std::cout << "mode=" << (midi ? "MIDI" : "FM") << " iteration=" << i << " transition_ms=" << std::chrono::duration<double, std::milli>(t1-t0).count() << " queued=" << audio.queued() << '\n';
            }
        }
        return 0;
    } catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
