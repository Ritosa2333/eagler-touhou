#include "frame_pacing.hpp"
#include "hiscore.hpp"
#include "hud_renderer.hpp"
#include "replay_session.hpp"
#include <emscripten/emscripten.h>
#include <memory>

namespace {
constexpr unsigned width = 640, height = 400;
const std::filesystem::path assets = "/content";

struct Runtime {
    std::unique_ptr<soew::EncounterScene> scene;
    std::optional<soew::ReplaySession> session;
    std::optional<soew::NativeReplay> replay;
    th03::assets::BfntSheet font;
    std::size_t cursor = 0;
    std::vector<std::uint32_t> pixels = std::vector<std::uint32_t>(width * height);

    void start(soew::NativeReplayConfig config, std::optional<soew::NativeReplay> run = {}) {
        if (run && run->practice) throw std::runtime_error("This entry plays Story replays");
        const auto bytes = soew::StageScene::read(assets / "MIKOFT.BFT");
        auto decoded = th03::assets::decode_bfnt(std::span(bytes).first(8224),
                                                 th03::assets::BfntConsumerPolicy::gaiji);
        if (!decoded) throw std::runtime_error("MIKOFT.BFT: " + decoded.error().message);
        scene = std::make_unique<soew::EncounterScene>(soew::load_replay_scene(assets, config));
        font = std::move(*decoded);
        replay = std::move(run);
        if (replay) session.emplace(*replay);
        else session.emplace(config);
        cursor = 0;
    }

    int step(std::uint16_t input, bool group, bool fast) {
        const auto result = replay ? session->step_playback(*scene, *replay, cursor)
            : session->tick(*scene, soew::normalize_replay_tick(input, group, fast));
        if (result.status == soew::ReplayStepStatus::Failed) throw std::runtime_error(result.error);
        return result.status == soew::ReplayStepStatus::Finished ? 0 : 1;
    }

    void render() {
        const auto &s = *scene;
        s.render(pixels, true, true);
        soew::mask_gameplay_tram(pixels);
        soew::draw_original_hud(pixels, font, {
            s.score_core.score, s.score_core.score, s.continues_used, 0,
            s.life.lives, s.life.bombs, int(s.pickups.power), s.rank,
        });
        if (s.continue_prompt()) {
            namespace glyphs = soew::score_gaiji;
            soew::EncounterScene::apply_mima_tone(pixels, 50);
            const auto line = [&](const auto &codes, int x, int y, std::uint32_t color) {
                for (const auto code : codes) {
                    draw_glyph(pixels, font, code, x, y, color);
                    x += 16;
                }
            };
            line(glyphs::continue_prompt, 18 * 8, 12 * 16, 0xffffff);
            line(glyphs::yes, 24 * 8, 14 * 16, s.continue_yes ? 0xffff00 : 0xffffff);
            line(glyphs::no, 25 * 8, 15 * 16, s.continue_yes ? 0xffffff : 0xffff00);
            line(glyphs::credit, 18 * 8, 20 * 16, 0x00ff00);
            draw_glyph(pixels, font, glyphs::zero + 3 - s.continues_used, 32 * 8, 20 * 16, 0x00ff00);
        }
        // Native XRGB words become RGBA bytes in Wasm's little-endian memory.
        for (auto &p : pixels) p = 0xff000000u | ((p & 0xff) << 16) | (p & 0xff00) | ((p >> 16) & 0xff);
    }
};

Runtime runtime;
std::string error;

template<class F> int call(F fn) {
    try {
        return fn();
    } catch (const std::exception &e) {
        error = e.what();
        return -1;
    }
}
}

extern "C" {
EMSCRIPTEN_KEEPALIVE int soew_start(std::uint32_t seed) {
    return call([&] {
        soew::NativeReplayConfig config;
        config.seed = seed;
        config.stage5_mainline = true;
        runtime.start(config);
        return 1;
    });
}
EMSCRIPTEN_KEEPALIVE int soew_open_replay() {
    return call([] {
        auto run = soew::load_native_replay("/input.rep");
        const auto config = run.config;
        runtime.start(config, std::move(run));
        return 1;
    });
}
EMSCRIPTEN_KEEPALIVE int soew_step(unsigned input, bool group, bool fast) {
    return call([&] { return runtime.step(input, group, fast); });
}
EMSCRIPTEN_KEEPALIVE int soew_render() {
    return call([] { runtime.render(); return 1; });
}
EMSCRIPTEN_KEEPALIVE unsigned soew_dialogue_line(unsigned line) {
    const auto &scene = *runtime.scene;
    const auto &d = scene.dialogue;
    using Phase = soew::StageDialogue::Phase;
    if ((scene.stage == 4 && scene.mima_delay) ||
        (d.phase != Phase::Text && d.phase != Phase::KeyDelay)) return 0;
    const auto &text = d.boxes.at(d.current).lines[line].text;
    unsigned end = 0, limit = d.reveal.bytes(line);
    while (end < limit) {
        const auto c = std::to_integer<unsigned>(text[end]);
        const auto size = (c >= 0x81 && c <= 0x9f) || (c >= 0xe0 && c <= 0xfc) ? 2u : 1u;
        if (end + size > limit) break;
        end += size;
    }
    return end;
}
EMSCRIPTEN_KEEPALIVE const std::byte *soew_dialogue_bytes(unsigned line) {
    const auto &d = runtime.scene->dialogue;
    return d.boxes.at(d.current).lines[line].text.data();
}
EMSCRIPTEN_KEEPALIVE const std::uint32_t *soew_pixels() { return runtime.pixels.data(); }
EMSCRIPTEN_KEEPALIVE const char *soew_error() { return error.c_str(); }
EMSCRIPTEN_KEEPALIVE std::uint32_t soew_tick_count() { return runtime.session->ticks(); }
EMSCRIPTEN_KEEPALIVE std::uint64_t soew_digest() { return soew::replay_state_digest(*runtime.scene); }
EMSCRIPTEN_KEEPALIVE int soew_player_x() { return runtime.scene->player.x; }
EMSCRIPTEN_KEEPALIVE unsigned soew_slowdown() { return runtime.scene->presentation_slowdown(); }
EMSCRIPTEN_KEEPALIVE double soew_retrace_ms() {
    return 1000.0 * soew::pc98_refresh_den / soew::pc98_refresh_num;
}
}
