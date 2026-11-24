#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <cstdio>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "game/AnmDrawSchedule.hpp"
#include "game/AnmManager.hpp"
#include "game/AnmRenderer.hpp"
#include "game/AsciiFrame.hpp"
#include "game/AsciiText.hpp"
#include "game/DrawCapture.hpp"
#include "game/EclResource.hpp"
#include "game/GameInput.hpp"
#include "game/PlayfieldTargets.hpp"
#include "game/ReplayFile.hpp"
#include "game/ReplaySession.hpp"
#include "game/Rng.hpp"
#include "game/RunScene.hpp"
#include "game/ScreenViews.hpp"
#include "game/StageBattle.hpp"
#include "game/StagePresentation.hpp"
#include "game/StageSim.hpp"
#include "game/TitleFrame.hpp"
#include "game/TitleScene.hpp"

namespace {

using th13::i32;
using th13::u32;

int rng_smoke() {
    th13::Rng rng(1);
    std::printf("th13-harness rng");
    for (int i = 0; i < 8; i++) {
        std::printf(" %d", rng.next());
    }
    std::printf("\n");
    return 0;
}

int stage1(const char* resource_dir, const char* event_path, unsigned frames, const char* stage) {
    th13::StageSim sim;
    sim.anm_effects_noop = true;
    if (!sim.load(resource_dir, event_path, stage)) {
        std::printf("stage1 load failed: %s\n", sim.error().c_str());
        return 1;
    }
    bool ok;
    if (std::getenv("TH13_LIFE_PROBE")) {
        ok = sim.start();
        for (unsigned f = 1; ok && f < frames && sim.error().empty(); ++f) {
            sim.sink.set_frame(f);
            if (!sim.step_frame()) break;
            for (const auto& enemy : sim.enemies)
                std::printf("LIFE f=%u e=%d life=%d pos=(%.9g,%.9g) hb=(%.9g,%.9g) "
                            "flags=%08x\n",
                            f, int(enemy->serial), int(enemy->life), double(enemy->position[0]),
                            double(enemy->position[1]), double(enemy->hurtbox[0]), double(enemy->hurtbox[1]),
                            enemy->flags);
        }
        sim.sink.close();
        ok = ok && sim.error().empty() && sim.sink.ok();
    } else {
        ok = sim.run(frames);
    }
    std::printf("stage1 frames=%u enemies=%u events=%llu ok=%d\n", frames, sim.spawn_count(),
                static_cast<unsigned long long>(sim.event_count()), ok ? 1 : 0);
    if (!ok) {
        std::printf("stage1 error: %s\n", sim.error().empty() ? "event sink failure" : sim.error().c_str());
        return 1;
    }
    return 0;
}

bool read_whole_file(const std::string& path, std::vector<th13::u8>& out) {
    std::FILE* file = std::fopen(path.c_str(), "rb");
    if (!file) return false;
    std::fseek(file, 0, SEEK_END);
    const long size = std::ftell(file);
    std::fseek(file, 0, SEEK_SET);
    out.resize(size_t(size < 0 ? 0 : size));
    const size_t got = out.empty() ? 1 : std::fread(out.data(), 1, out.size(), file);
    std::fclose(file);
    return got == out.size();
}

void state_keys(th13::u16 state, std::string& out) {
    static constexpr std::pair<th13::u16, int> kBits[] = {{0x01, 0x5A}, {0x02, 0x58}, {0x08, 0x10}, {0x10, 0x26},
                                                          {0x20, 0x28}, {0x40, 0x25}, {0x80, 0x27}};
    std::vector<int> keys;
    for (const auto& bit : kBits)
        if (state & bit.first) keys.push_back(bit.second);
    std::sort(keys.begin(), keys.end());
    out.clear();
    for (size_t i = 0; i < keys.size(); i++) {
        out += i ? "," : "";
        out += std::to_string(keys[i]);
    }
}

void write_state_row(std::FILE* log, int frame, const th13::StageBattle& battle, const th13::SessionState& progress,
                     const th13::HudPlayerState& player, const th13::HudScoreState& score, th13::u16 state) {
    std::string keys;
    state_keys(state, keys);
    std::fprintf(log,
                 "{\"schema\":\"th13-state-raw-v1\",\"frame\":%d,"
                 "\"player_position\":[%.9g,%.9g],"
                 "\"power\":%d,\"power_step\":%d,\"spell_id\":null,"
                 "\"keys\":[%s],"
                 "\"raw_globals\":{\"004be7c0\":%d,\"004be7c8\":%d,"
                 "\"004be810\":%d,\"004be814\":%d},"
                 "\"raw_enemy_manager\":null}\n",
                 frame, double(battle.player_x), double(battle.player_y), player.power, player.power_step, keys.c_str(),
                 score.score, progress.continues, progress.high_score, progress.high_score_extra);
}

class RunSceneHost final : public th13::RunSceneServices {
public:
    std::string dir;
    th13::AnmManager* animations = nullptr;
    th13::DrawCapture* graphics = nullptr;
    th13::AsciiText* captions = nullptr;
    i32 destination = 0;
    bool game_over = false;
    bool hud_text(const th13::HudTextDraw& value) override {
        return captions ? captions->enqueue(value, 0, value.style.shadow) : true;
    }
    bool hud_sound(i32) override { return true; }
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool camera(th13::DrawCamera, bool) override { return true; }
    bool request_pause() override { return true; }
    bool request_game_over() override {
        game_over = true;
        return true;
    }
    bool request_destination(i32 raw) override {
        destination = raw;
        return true;
    }
    std::vector<std::tuple<i32, i32, std::string>> music_requests;
    bool queue_music(i32 code, i32 value, const std::string& name) override {
        music_requests.emplace_back(code, value, name);
        return true;
    }
    bool read_asset(const std::string& name, std::vector<th13::u8>& bytes, bool required) override {
        if (read_whole_file(dir + "/" + name, bytes)) return true;
        bytes.clear();
        return !required;
    }
    th13::ZunGraphics& stage_graphics() override { return *graphics; }
    bool preload_scenery_animation(i32 bank, const std::string& name) override {
        std::vector<th13::u8> bytes;
        return animations && read_whole_file(dir + "/" + name, bytes) &&
               animations->load(bank, bytes.data(), th13::u32(bytes.size()));
    }
};

struct ReplayRunSession {
    th13::GameRandom visual;
    th13::GameRandom game_random;
    th13::AnmEnvironment environment;
    th13::AnmManager animations{visual, environment};
    th13::DrawCapture capture;
    th13::AnmRenderer renderer{capture};
    th13::ScreenViews views{renderer, environment};
    th13::AsciiText captions{animations, environment};
    th13::FrameScheduler scheduler;
    th13::SessionState progress;
    th13::HudPlayerState player;
    th13::HudScoreState score;
    RunSceneHost host;
    std::unique_ptr<th13::RunScene> run;
    std::unique_ptr<th13::PlayfieldTargets> playfield;
    std::unique_ptr<th13::AnmDrawSchedule> draw_schedule;
    std::unique_ptr<th13::AsciiFrame> caption_frame;
    std::array<th13::FrameCallback, 2> animation_updates{};
    std::string error;
    bool fail(const std::string& why) {
        if (error.empty()) error = why;
        return false;
    }
    void configure_environment() {
        environment.screen_width = 640;
        environment.screen_height = 480;
        environment.raster_scale = 1;
        environment.resolution_scale = 1;
        environment.render_target_has_alpha = true;
        environment.screen_offsets = {32, 16, 320, 16};
        environment.playfield_origin = {32, 16};
        environment.camera_origin = {0, 0, 1000};
        environment.shared_game_rng = &game_random;
    }
    bool read_bank(const char* name, th13::i32 bank, th13::i32 identity_slot = -100) {
        std::vector<th13::u8> bytes;
        if (!read_whole_file(host.dir + "/" + name, bytes) ||
            !animations.load(bank, bytes.data(), th13::u32(bytes.size())))
            return fail(std::string("ANM load failed: ") + name + ": " + animations.error);
        if (identity_slot != -100)
            if (auto* file = animations.resource(bank))
                capture.register_texture(file, 0, th13::u32(file->textures.size()), identity_slot);
        return true;
    }
    bool load_run_banks(const th13::ReplayFile& replay_file) {
        static constexpr std::pair<const char*, th13::i32> init_banks[7] = {
            {"text.anm", 0},   {"sig.anm", 1},      {"ascii.anm", 2}, {"effect.anm", 8},
            {"title.anm", 16}, {"title_v.anm", 17}, {"help.anm", 19}};
        for (const auto& bank : init_banks) {
            if (!read_bank(bank.first, bank.second, bank.second)) return false;
            if (bank.second == 2) environment.fallback_sprite_resource = animations.resource(2);
        }
        if (!read_bank("front.anm", 3, 5)) return false;
        std::vector<th13::u8> ecl, bytes;
        char stage_ecl[16];
        std::snprintf(stage_ecl, sizeof stage_ecl, "st%02d.ecl", int(progress.stage));
        if (!read_whole_file(std::string(host.dir) + "/" + stage_ecl, ecl))
            return fail(std::string("Stage ECL missing: ") + stage_ecl);
        th13::EclResource head;
        if (!head.open(ecl.data(), th13::u32(ecl.size())) || head.animations.empty())
            return fail("Stage ECL ANIM list unavailable");
        th13::AnmResource merged;
        for (const auto& name : head.animations) {
            if (!read_whole_file(host.dir + "/" + name, bytes)) return fail("Stage ANM missing: " + name);
            if (!merged.append(bytes.data(), th13::u32(bytes.size()))) return fail("Stage ANM append failed: " + name);
        }
        if (!animations.adopt(4, std::move(merged))) return fail("Stage ANM bank failed: " + animations.error);
        if (auto* file = animations.resource(4)) capture.register_texture(file, 0, th13::u32(file->textures.size()), 6);
        if (!read_bank("bullet.anm", 5, 7)) return false;
        if (!read_bank("astral.anm", 25, 25)) return false;
        char player_anm[16];
        std::snprintf(player_anm, sizeof player_anm, "pl%02d.anm", replay_file.character & 3);
        if (!read_bank(player_anm, 6, 9)) return false;
        char stage_logo[20];
        std::snprintf(stage_logo, sizeof stage_logo, "st%02dlogo.anm", int(progress.stage));
        if (!read_bank(stage_logo, 27)) return false;
        return true;
    }
    bool register_animation_pools() {
        animation_updates[0].owner = animation_updates[1].owner = this;
        animation_updates[0].enabled = animation_updates[1].enabled = true;
        animation_updates[0].run = [](void* p) {
            return static_cast<ReplayRunSession*>(p)->animations.update(true) ? th13::i32(th13::FrameAction::Continue)
                                                                              : th13::i32(th13::FrameAction::Error);
        };
        animation_updates[1].run = [](void* p) {
            return static_cast<ReplayRunSession*>(p)->animations.update(false) ? th13::i32(th13::FrameAction::Continue)
                                                                               : th13::i32(th13::FrameAction::Error);
        };
        if (scheduler.add(animation_updates[0], th13::FramePass::Update, 9) < 0 ||
            scheduler.add(animation_updates[1], th13::FramePass::Update, 34) < 0)
            return fail("animation pool registration failed");
        return true;
    }
    bool assemble(const th13::ReplayFile& replay_file, u32 stage_index, const th13::ReplayStage& stage,
                  const char* event_path = nullptr) {
        host.animations = &animations;
        host.graphics = &capture;
        configure_environment();
        progress.stage = i32(stage.number);
        progress.difficulty = replay_file.rank & 3;
        progress.character = replay_file.character;
        th13::ReplayRestoreLog restore_log;
        std::string restore_error;
        const th13::ReplayRestoreTarget restore_target{&progress, &player, &score, &game_random, &visual};
        if (!th13::ReplaySession::restore_stage(replay_file, stage_index, restore_target, restore_log, restore_error)) {
            error = "replay restore failed: " + restore_error;
            return false;
        }
        th13::ReplaySession::emit_log(restore_log);
        progress.stage = i32(stage.number);
        environment.game_rng.reseed(restore_log.seed);
        if (!load_run_banks(replay_file)) return false;
        char std_name[16], sht_file[16];
        std::snprintf(std_name, sizeof std_name, "st%02d.std", int(stage.number));
        std::snprintf(sht_file, sizeof sht_file, "pl%02d%c.sht", replay_file.character,
                      replay_file.shot_type == 0 ? 'a' : 'b');
        run = std::make_unique<th13::RunScene>(progress, player, score, animations, renderer, environment, host,
                                               scheduler, th13::StagePresentation::Banks{4, 5, 6, 8}, 3, 2, 7, std_name,
                                               &game_random);
        run->hud.stage_logo_bank = 27;
        run->battle_sht = sht_file;
        if (event_path && !run->battle.open_event_sink(event_path))
            return fail("event sink open failed: " + run->battle.error());
        if (!register_animation_pools()) return false;
        if (!run->initialize()) return fail(run->error);
        if (event_path && !run->battle.open_event_sink(event_path))
            return fail("event sink reaffirm failed: " + run->battle.error());
        return true;
    }
};

static std::FILE* g_rng_trace = nullptr;
static std::FILE* g_spirit_probe = nullptr;
static unsigned g_probe_frame = 0;
static const th13::GameRandom* g_probe_game_random = nullptr;

static void probe_game_draw(const th13::GameRandom* rng, uint32_t before, uint32_t after) {
    if (g_rng_trace && rng == g_probe_game_random)
        std::fprintf(g_rng_trace, "{\"frame\":%u,\"before\":%u,\"after\":%u}\n", g_probe_frame, unsigned(before),
                     unsigned(after));
}

int replay_runscene(const char* replay_path, const char* resource_dir, const char* event_path, unsigned frames,
                    unsigned stage_index) {
    std::vector<th13::u8> bytes;
    if (!read_whole_file(replay_path, bytes)) {
        std::printf("replay read failed: %s\n", replay_path);
        return 1;
    }
    th13::ReplayFile replay_file;
    if (!replay_file.decode(bytes.data(), bytes.size())) {
        std::printf("replay-runscene decode failed: %s\n", replay_file.error().c_str());
        return 1;
    }
    if (stage_index >= replay_file.stages.size()) {
        std::printf("replay stage index %u out of %u\n", stage_index, unsigned(replay_file.stages.size()));
        return 1;
    }
    const th13::ReplayStage& stage = replay_file.stages[stage_index];
    std::printf("replay-runscene name=%s rank=%d chara=%d shot=%d stages=%u\n", replay_file.player_name.c_str(),
                replay_file.rank, replay_file.character, replay_file.shot_type, unsigned(replay_file.stages.size()));
    const auto session = new ReplayRunSession();
    session->host.dir = resource_dir;
    if (const char* rng_path = std::getenv("TH13_RNG_TRACE")) {
        g_rng_trace = std::fopen(rng_path, "wb");
        if (!g_rng_trace) {
            std::printf("rng trace open failed: %s\n", rng_path);
            return 1;
        }
        g_probe_game_random = &session->game_random;
        th13::GameRandom::instance_draw_observer = probe_game_draw;
    }
    if (const char* spirit_path = std::getenv("TH13_SPIRIT_PROBE")) {
        g_spirit_probe = std::fopen(spirit_path, "wb");
        if (!g_spirit_probe) {
            std::printf("spirit probe open failed: %s\n", spirit_path);
            return 1;
        }
    }
    if (std::getenv("TH13_DAMAGE_PROBE")) th13::damage_probe_enabled = true;
    if (!session->assemble(replay_file, stage_index, stage, event_path)) {
        std::printf("replay-runscene setup failed: %s\n",
                    session->error.empty() ? session->run->error.c_str() : session->error.c_str());
        return 1;
    }
    const bool no_ecl_slot_anm = std::getenv("TH13_NO_ECL_SLOT_ANM") != nullptr;
    if (no_ecl_slot_anm) session->run->battle.sim.ecl_slot_anm = false;
    unsigned freeze_at = 0;
    if (const char* at = std::getenv("TH13_FREEZE_AT")) freeze_at = unsigned(std::atoi(at));

    std::FILE* state_log = nullptr;
    if (const char* state_path = std::getenv("TH13_STATE_LOG")) {
        state_log = std::fopen(state_path, "wb");
        if (!state_log) {
            std::printf("replay state log open failed: %s\n", state_path);
            return 1;
        }
        write_state_row(state_log, -1, session->run->battle, session->progress, session->player, session->score, 0);
    }
    th13::ReplayInputCursor cursor(stage);
    th13::GameInput input;
    const u32 total = frames ? th13::u32(frames) : stage.frame_count;
    u32 consumed = 0, stepped = 0, frozen = 0;
    unsigned max_with_handles = 0;
    i32 boss_frame = -1, nonspell_frame = -1;
    bool boss_seen = false, nonspell_seen = false, terminal = false;
    u32 stop_after = 0xFFFFFFFFu;
    while (consumed < total && cursor.valid() && !terminal) {
        const th13::ReplayFrame frame = cursor.next();
        g_probe_frame = consumed;
        input.update(frame.state);
        session->run->controls(input);
        if (freeze_at && consumed + 1 >= freeze_at) session->progress.scene_flags |= 0x10u;
        const i32 frame_before = session->progress.stage_frame;
        if (session->scheduler.update() < 0) {
            std::printf("replay-runscene error at frame %u: %s\n", consumed,
                        session->run->error.empty() ? session->animations.error.c_str() : session->run->error.c_str());
            if (state_log) std::fclose(state_log);
            session->run->battle.sim.sink.close();
            return 1;
        }
        ++consumed;
        if (state_log)
            write_state_row(state_log, i32(consumed) - 1, session->run->battle, session->progress, session->player,
                            session->score, frame.state);
        if (g_spirit_probe) {
            unsigned alive = 0;
            for (const auto& spirit : session->run->battle.item_manager.spirits())
                if (spirit.alive) ++alive;
            std::fprintf(g_spirit_probe, "{\"frame\":%u,\"spirits\":%u,\"index\":%u,\"seed\":%u}\n", g_probe_frame,
                         alive, unsigned(session->game_random.index()), unsigned(session->game_random.seed()));
        }
        if (session->progress.stage_frame == frame_before) {
            ++frozen;
        } else {
            ++stepped;
            unsigned live_handles = 0;
            for (const auto& enemy : session->run->battle.sim.enemies)
                for (auto handle : enemy->anm_slot_handle)
                    if (handle) live_handles++;
            max_with_handles = std::max(max_with_handles, live_handles);
            const bool boss = session->run->battle.sim.boss() != nullptr;
            if (boss && !boss_seen) {
                boss_seen = true;
                boss_frame = i32(stepped) - 1;
            }
            if (session->run->battle.sim.boss_nonspell_frame >= 0 && !nonspell_seen) {
                nonspell_seen = true;
                nonspell_frame = session->run->battle.sim.boss_nonspell_frame;
                if (!frames && !std::getenv("TH13_REPLAY_FULL")) {
                    stop_after = consumed + 300;
                }
            }
        }
        if (session->host.game_over || session->host.destination) terminal = true;
        if (consumed >= stop_after) break;
        if (consumed % 600 == 0)
            std::printf("replay-runscene frame=%u held=%04x x=%.1f y=%.1f stage_frame=%u "
                        "enemies=%u slow=%u\n",
                        consumed, frame.state, session->run->battle.player_x, session->run->battle.player_y,
                        session->progress.stage_frame, unsigned(session->run->battle.sim.enemies.size()),
                        cursor.current_slow_rate());
    }
    if (state_log) std::fclose(state_log);
    if (g_spirit_probe) {
        std::fclose(g_spirit_probe);
        g_spirit_probe = nullptr;
    }
    if (g_rng_trace) {
        std::fclose(g_rng_trace);
        g_rng_trace = nullptr;
        th13::GameRandom::instance_draw_observer = nullptr;
    }
    session->run->battle.sim.sink.close();
    const bool sink_ok = session->run->battle.sim.sink.ok();
    const unsigned long long events = static_cast<unsigned long long>(session->run->battle.sim.event_count());
    std::printf("replay-runscene done consumed=%u/%u stepped=%u frozen=%u events=%llu "
                "sink_ok=%d x=%.1f y=%.1f deaths=%d score=%d hi=%d boss_frame=%d "
                "nonspell_frame=%d game_over=%d destination=%d\n",
                consumed, unsigned(stage.frame_count), stepped, frozen, events, int(sink_ok),
                session->run->battle.player_x, session->run->battle.player_y, session->player.deaths,
                session->score.score, session->progress.high_score, boss_frame, nonspell_frame,
                int(session->host.game_over), int(session->host.destination));
    if (!sink_ok) {
        std::printf("replay-runscene FAIL event sink write error\n");
        return 1;
    }
    int failures = 0;
    if (freeze_at) {
        const u32 expect_frozen = total >= freeze_at ? total - freeze_at + 1 : 0;
        std::printf("SELFTEST-FREEZE freeze_at=%u consumed=%u stepped=%u frozen=%u "
                    "expect_frozen=%u rows_per_consumed_frame=1\n",
                    freeze_at, consumed, stepped, frozen, expect_frozen);
        if (consumed != total || frozen != expect_frozen || stepped != total - expect_frozen) {
            std::printf("SELFTEST-FREEZE FAIL\n");
            failures++;
        } else {
            std::printf("SELFTEST-FREEZE PASS\n");
        }
    }
    {
        const bool drained = session->run->battle.sim.anm_effects.empty();
        std::printf("SELFTEST-ANMSYNC ecl_slot_anm=%d effects_drained=%d "
                    "max_with_handles=%u\n",
                    int(session->run->battle.sim.ecl_slot_anm), int(drained), max_with_handles);
        if (!drained) {
            std::printf("SELFTEST-ANMSYNC FAIL effects not drained\n");
            failures++;
        }
        if (no_ecl_slot_anm ? max_with_handles != 0 : max_with_handles == 0) {
            std::printf("SELFTEST-ANMSYNC FAIL slot handles\n");
            failures++;
        } else {
            std::printf("SELFTEST-ANMSYNC PASS\n");
        }
    }
    return failures ? 1 : 0;
}

constexpr th13::i32 kDrawsStageBank = 4, kDrawsBulletBank = 5, kDrawsPlayerBank = 6, kDrawsEffectBank = 8;

class DrawsProvider final : public th13::EclResourceProvider {
public:
    std::string dir;
    th13::StageSim* sim = nullptr;
    bool read(const std::string& name, std::vector<th13::u8>& data) override {
        return read_whole_file(dir + "/" + name, data);
    }
    bool animation(u32, const std::string& name) override {
        std::vector<th13::u8> probe;
        if (!read_whole_file(dir + "/" + name, probe)) return false;
        if (sim) sim->anim_list.push_back(name);
        return true;
    }
};

class DrawsHost final : public th13::StageBattleServices {
public:
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override {
        game_over = true;
        return true;
    }
    bool hud_sound(i32) override { return true; }
    th13::AnmManager* retire_animations = nullptr;
    void enemy_retire(u32 serial) override {
        if (!retire_animations) return;
        for (const auto& enemy : session_sim->enemies)
            if (enemy->serial == serial) {
                for (auto& handle : enemy->anm_slot_handle)
                    if (handle) retire_animations->retire(handle);
                return;
            }
    }
    th13::StageSim* session_sim = nullptr;
    bool game_over = false;
};

struct DrawsSession {
    struct StageFileRange {
        th13::u32 first = 0, count = 0;
        th13::i32 slot = -1;
    };
    th13::DrawCapture capture;
    th13::AnmEnvironment environment;
    th13::GameRandom anm_random;
    th13::StageBattle battle;
    th13::AnmManager animations{anm_random, environment};
    th13::AnmRenderer renderer{capture};
    th13::ScreenViews views{renderer, environment};
    std::unique_ptr<th13::StagePresentation> presentation;
    std::vector<StageFileRange> stage_files;
    std::string error;

    bool fail(const std::string& why) {
        if (error.empty()) error = why;
        return false;
    }
    void configure_environment() {
        environment.screen_width = 640;
        environment.screen_height = 480;
        environment.raster_scale = 1;
        environment.resolution_scale = 1;
        environment.render_target_has_alpha = true;
        environment.screen_offsets = {32, 16, 320, 16};
        environment.playfield_origin = {32, 16};
        environment.camera_origin = {0, 0, 1000};
    }
    bool load_banks(DrawsProvider& provider, const char* player_anm);
    bool render_frame(unsigned frame, th13::u32 held);
};

bool DrawsSession::load_banks(DrawsProvider& provider, const char* player_anm) {
    std::vector<th13::u8> bytes;
    if (!read_whole_file(provider.dir + "/bullet.anm", bytes) ||
        !animations.load(kDrawsBulletBank, bytes.data(), th13::u32(bytes.size())))
        return fail("bullet.anm load failed: " + animations.error);
    if (auto* file = animations.resource(kDrawsBulletBank))
        capture.register_texture(file, 0, th13::u32(file->textures.size()), 7);
    environment.fallback_sprite_resource = animations.resource(kDrawsBulletBank);
    if (!read_whole_file(provider.dir + "/effect.anm", bytes) ||
        !animations.load(kDrawsEffectBank, bytes.data(), th13::u32(bytes.size())))
        return fail("effect.anm load failed: " + animations.error);
    if (auto* file = animations.resource(kDrawsEffectBank))
        capture.register_texture(file, 0, th13::u32(file->textures.size()), 8);
    if (!read_whole_file(provider.dir + "/astral.anm", bytes) ||
        !animations.load(25, bytes.data(), th13::u32(bytes.size())))
        return fail("astral.anm load failed: " + animations.error);
    if (auto* file = animations.resource(25)) capture.register_texture(file, 0, th13::u32(file->textures.size()), 25);
    std::vector<th13::u8> ecl;
    if (!read_whole_file(provider.dir + "/st01.ecl", ecl)) return fail("st01.ecl missing");
    th13::EclResource head;
    if (!head.open(ecl.data(), th13::u32(ecl.size())) || head.animations.empty())
        return fail("st01.ecl ANIM list unavailable");
    th13::AnmResource merged;
    for (const auto& name : head.animations) {
        if (!read_whole_file(provider.dir + "/" + name, bytes)) return fail("stage ANM missing: " + name);
        th13::AnmResource single;
        if (!single.open(bytes.data(), th13::u32(bytes.size()))) return fail("stage ANM invalid: " + name);
        StageFileRange range;
        range.first = th13::u32(merged.textures.size());
        range.count = th13::u32(single.textures.size());
        range.slot = th13::i32(10 + stage_files.size());
        if (!merged.append(bytes.data(), th13::u32(bytes.size()))) return fail("stage ANM append failed: " + name);
        stage_files.push_back(range);
    }
    if (!animations.adopt(kDrawsStageBank, std::move(merged)))
        return fail("stage ANM bank failed: " + animations.error);
    if (auto* file = animations.resource(kDrawsStageBank))
        for (const auto& range : stage_files)
            capture.register_texture(file, range.first, range.count, range.slot);
    if (!read_whole_file(provider.dir + "/" + std::string(player_anm), bytes) ||
        !animations.load(kDrawsPlayerBank, bytes.data(), th13::u32(bytes.size())))
        return fail(std::string("player ANM load failed: ") + player_anm);
    if (auto* file = animations.resource(kDrawsPlayerBank))
        capture.register_texture(file, 0, th13::u32(file->textures.size()), -1);
    return true;
}

bool DrawsSession::render_frame(unsigned frame, th13::u32 held) {
    if (!error.empty()) return false;
    capture.begin_frame(frame);
    if (!animations.update(true)) return fail("fore pool update: " + animations.error);
    if (!presentation->sync(battle, held)) return fail("presentation sync: " + presentation->error());
    if (!animations.update(false)) return fail("main pool update: " + animations.error);
    if (const char* dump_at = std::getenv("TH13_DUMPCHECK")) {
        const unsigned at = unsigned(std::atoi(dump_at));
        if (frame == at) {
            const auto as_bits = [](float value) { return th13::u32(*reinterpret_cast<const th13::u32*>(&value)); };
            for (th13::AnmVm* vm : animations.registry.layer(2)) {
                const auto& ch = vm->interpolators.position;
                const auto& col = vm->interpolators.color;
                const auto& al = vm->interpolators.alpha;
                std::printf("[f%u] s4aa=%d x3c=%.9g bits=%08x start=%08x,%08x,%08x "
                            "end=%08x,%08x,%08x dur=%08x mode=%d frac=%08x "
                            "color=%08x cdur=%d ctimer=%d alpha=%08x adur=%d "
                            "atimer=%d astart=%.9g aend=%.9g\n",
                            frame, vm->source_script, double(vm->variables.position.x),
                            as_bits(vm->variables.position.x), as_bits(ch.start[0]), as_bits(ch.start[1]),
                            as_bits(ch.start[2]), as_bits(ch.end[0]), as_bits(ch.end[1]), as_bits(ch.end[2]),
                            as_bits(float(ch.duration)), ch.mode, as_bits(ch.timer.fractional), vm->visual.color,
                            int(col.duration), col.timer.current, vm->visual.color, int(al.duration), al.timer.current,
                            double(al.start[0]), double(al.end[0]));
            }
        }
    }
    if (!views.camera(th13::DrawCamera::Fullscreen, false)) return fail("full-screen camera unavailable");
    std::vector<th13::AnmVm*> hidden;
    for (th13::u32 layer = 0; layer < 33; layer++)
        for (auto* vm : animations.registry.layer(layer))
            if (vm && vm->resource == animations.resource(kDrawsPlayerBank) && (vm->visual.flags & 1u)) {
                vm->visual.flags &= ~1u;
                hidden.push_back(vm);
            }
    for (th13::u32 layer = 0; layer < 33; layer++)
        if (!renderer.draw_layer(animations.registry.layer(layer)))
            return fail("animation layer draw: " + renderer.error);
    for (auto* vm : hidden)
        vm->visual.flags |= 1u;
    for (th13::u32 pass = 0; pass < 4; pass++)
        if (!presentation->draw(pass)) return fail("presentation draw: " + presentation->error());
    capture.set_stale_layer(56);
    renderer.flush();
    capture.clear_device_texture();
    if (!capture.end_frame()) return fail("draws write: " + capture.error());
    return true;
}

int run_draws(const char* resource_dir, const char* draws_path, unsigned frames, const char* stage) {
    DrawsSession session;
    session.configure_environment();
    DrawsProvider provider;
    provider.dir = resource_dir;
    provider.sim = &session.battle.sim;
    DrawsHost host;
    th13::SessionState progress;
    th13::HudPlayerState player;
    th13::HudScoreState score;
    th13::draw_capture = &session.capture;
    const std::string stage_file = stage ? stage : "st01.ecl";
    if (!session.battle.load(provider, stage_file.c_str(), 1 << 3, host, progress, player, score, nullptr, nullptr)) {
        std::printf("draws battle load failed: %s\n", session.battle.error().c_str());
        return 1;
    }
    if (!session.load_banks(provider, "pl00.anm")) {
        std::printf("draws banks failed: %s\n", session.error.c_str());
        return 1;
    }
    session.presentation = std::make_unique<th13::StagePresentation>(
        session.animations, session.renderer,
        th13::StagePresentation::Banks{kDrawsStageBank, kDrawsBulletBank, kDrawsPlayerBank, kDrawsEffectBank}, 31);
    session.presentation->enemies_from_ecl = true;
    session.battle.sim.ecl_slot_anm = true;
    host.retire_animations = &session.animations;
    host.session_sim = &session.battle.sim;
    th13::StagePresentation::debug_pass_mask = 4;
    session.battle.player_x = 192.0f;
    session.battle.player_y = 420.0f;
    if (!session.capture.open_output(draws_path)) {
        std::printf("draws output failed: %s\n", session.capture.error().c_str());
        return 1;
    }
    if (!session.render_frame(0, 0)) {
        std::printf("draws frame 0 failed: %s\n", session.error.c_str());
        return 1;
    }
    for (unsigned frame = 1; frame < frames; frame++) {
        if (!session.battle.sim.step_frame()) {
            std::printf("draws error at frame %u: %s\n", frame, session.battle.sim.error().c_str());
            return 1;
        }
        if (!session.render_frame(frame, 0)) {
            std::printf("draws error at frame %u: %s\n", frame, session.error.c_str());
            return 1;
        }
        if (frame % 600 == 0)
            std::printf("draws frame=%u enemies=%u ops=%llu\n", frame, unsigned(session.battle.sim.enemies.size()),
                        static_cast<unsigned long long>(session.capture.quads() + session.capture.ups()));
    }
    session.capture.close_output();
    std::printf("port-draws frames=%llu quads=%llu ups=%llu ops=%llu zero=%u\n",
                static_cast<unsigned long long>(session.capture.frames_out()),
                static_cast<unsigned long long>(session.capture.quads()),
                static_cast<unsigned long long>(session.capture.ups()),
                static_cast<unsigned long long>(session.capture.quads() + session.capture.ups()),
                session.capture.zero_op_frames());
    return 0;
}

int run_draws_replay(const char* replay_path, const char* resource_dir, const char* draws_path, unsigned frames,
                     unsigned stage_index) {
    std::vector<th13::u8> bytes;
    if (!read_whole_file(replay_path, bytes)) {
        std::printf("draws-replay read failed: %s\n", replay_path);
        return 1;
    }
    th13::ReplayFile replay_file;
    if (!replay_file.decode(bytes.data(), bytes.size())) {
        std::printf("draws-replay decode failed: %s\n", replay_file.error().c_str());
        return 1;
    }
    if (stage_index >= replay_file.stages.size()) {
        std::printf("draws-replay stage index %u out of %u\n", stage_index, unsigned(replay_file.stages.size()));
        return 1;
    }
    const th13::ReplayStage& stage = replay_file.stages[stage_index];
    std::printf("draws-replay-runscene name=%s rank=%d chara=%d shot=%d stages=%u\n", replay_file.player_name.c_str(),
                replay_file.rank, replay_file.character, replay_file.shot_type, unsigned(replay_file.stages.size()));
    const auto session = new ReplayRunSession();
    session->host.dir = resource_dir;
    session->host.captions = &session->captions;
    if (!session->assemble(replay_file, stage_index, stage)) {
        std::printf("draws-replay setup failed: %s\n",
                    session->error.empty() ? session->run->error.c_str() : session->error.c_str());
        return 1;
    }
    th13::draw_capture = &session->capture;
    session->views.configure(session->environment);
    if (!session->captions.initialize(2)) {
        std::printf("draws-replay captions failed: %s\n", session->captions.error.c_str());
        return 1;
    }
    session->draw_schedule = std::make_unique<th13::AnmDrawSchedule>(
        session->scheduler, session->animations, session->renderer, session->capture, session->host);
    if (!session->draw_schedule->error.empty()) {
        std::printf("draws-replay schedule failed: %s\n", session->draw_schedule->error.c_str());
        return 1;
    }
    session->caption_frame =
        std::make_unique<th13::AsciiFrame>(session->scheduler, session->captions, session->renderer, session->host);
    if (!session->caption_frame->error.empty()) {
        std::printf("draws-replay caption frame failed: %s\n", session->caption_frame->error.c_str());
        return 1;
    }
    session->playfield = std::make_unique<th13::PlayfieldTargets>(session->scheduler, session->animations,
                                                                  session->renderer, session->capture, session->views);
    if (!session->playfield->initialize(0)) {
        std::printf("draws-replay playfield failed: %s\n", session->playfield->error.c_str());
        return 1;
    }
    if (!session->capture.open_output(draws_path)) {
        std::printf("draws-replay output failed: %s\n", session->capture.error().c_str());
        return 1;
    }
    const auto render_production_frame = [&](unsigned frame) -> bool {
        if (!session->error.empty()) return false;
        session->capture.begin_frame(frame);
        if (session->scheduler.draw() < 0) {
            session->fail("draw callbacks failed: " +
                          (!session->draw_schedule->error.empty()   ? session->draw_schedule->error
                           : !session->caption_frame->error.empty() ? session->caption_frame->error
                                                                    : session->run->error));
            return false;
        }
        session->renderer.flush();
        session->capture.clear_device_texture();
        return session->capture.end_frame();
    };
    const unsigned total = frames && frames < stage.frame_count ? frames : unsigned(stage.frame_count);
    th13::ReplayInputCursor cursor(stage);
    th13::GameInput input;
    if (!render_production_frame(0)) {
        std::printf("draws-replay frame 0 failed: %s\n", session->error.c_str());
        return 1;
    }
    unsigned drawn = 0;
    bool terminal = false;
    for (unsigned frame = 1; frame <= total && !terminal; frame++) {
        if (!cursor.valid()) {
            std::printf("draws-replay input exhausted at frame %u (window %u)\n", frame, total);
            session->capture.close_output();
            return 1;
        }
        input.update(cursor.next().state);
        session->run->controls(input);
        if (session->scheduler.update() < 0) {
            std::printf("draws-replay update failed at frame %u: %s\n", frame,
                        session->run->error.empty() ? session->animations.error.c_str() : session->run->error.c_str());
            session->capture.close_output();
            return 1;
        }
        if (session->host.game_over || session->host.destination) {
            render_production_frame(frame);
            session->capture.close_output();
            std::printf("draws-replay game over at frame %u (window %u) destination=%d\n", frame, total,
                        int(session->host.destination));
            return 1;
        }
        if (!render_production_frame(frame)) {
            std::printf("draws-replay render failed at frame %u: %s\n", frame,
                        session->error.empty() ? session->capture.error().c_str() : session->error.c_str());
            session->capture.close_output();
            return 1;
        }
        drawn = frame;
        if (frame % 600 == 0)
            std::printf("draws-replay frame=%u stage_frame=%u enemies=%u ops=%llu\n", frame,
                        session->progress.stage_frame, unsigned(session->run->battle.sim.enemies.size()),
                        static_cast<unsigned long long>(session->capture.quads() + session->capture.ups()));
    }
    session->capture.close_output();
    std::printf("port-draws-replay frames=%llu drawn=%u/%u quads=%llu ups=%llu ops=%llu "
                "zero=%u\n",
                static_cast<unsigned long long>(session->capture.frames_out()), drawn, total,
                static_cast<unsigned long long>(session->capture.quads()),
                static_cast<unsigned long long>(session->capture.ups()),
                static_cast<unsigned long long>(session->capture.quads() + session->capture.ups()),
                session->capture.zero_op_frames());
    return 0;
}

class TitleHost final : public th13::TitleScenePlatform {
public:
    bool clear_title_overlay() override { return true; }
    bool queue_title_music(i32, i32, const std::string&) override { return true; }
    bool clear_current_wave() override { return true; }
    bool title_exit(i32 value) override {
        exit_destination = value;
        return true;
    }
    bool fade_out_title_music() override { return true; }
    bool sound(i32) override { return true; }
    bool begin_transition(u32& animation) override {
        animation = 0;
        return true;
    }
    bool start_game(i32 stage) override {
        requested_stage = stage;
        run_requested = true;
        return true;
    }
    bool hud_text(const th13::HudTextDraw&) override { return true; }
    i32 exit_destination = -1;
    i32 requested_stage = 0;
    bool run_requested = false;
};

struct TitleSession {
    th13::DrawCapture capture;
    th13::GameRandom loading_random;
    th13::AnmEnvironment loading_environment;
    th13::AnmManager loading_animations{loading_random, loading_environment};
    th13::GameRandom anm_random;
    th13::AnmEnvironment environment;
    th13::AnmManager animations{anm_random, environment};
    th13::AnmRenderer renderer{capture};
    th13::ScreenViews views{renderer, environment};
    th13::FrameScheduler scheduler;
    TitleHost host;
    th13::SessionState progress;
    th13::TitleSelectionSettings selection;
    std::unique_ptr<th13::AnmDrawSchedule> draw_schedule;
    std::unique_ptr<th13::TitleScene> title;
    std::array<th13::FrameCallback, 2> animation_updates{};
    th13::u32 loading_signature = 0;
    unsigned loading_frames = 0;
    std::string dir;
    std::string error;

    bool fail(const std::string& why) {
        if (error.empty()) error = why;
        return false;
    }
    bool read_bank(th13::AnmManager& manager, const char* name, th13::i32 bank) {
        std::vector<th13::u8> bytes;
        if (!read_whole_file(dir + "/" + name, bytes) || !manager.load(bank, bytes.data(), th13::u32(bytes.size())))
            return fail(std::string("ANM load failed: ") + name + ": " + manager.error);
        if (auto* file = manager.resource(bank))
            capture.register_texture(file, 0, th13::u32(file->textures.size()), bank);
        return true;
    }
    bool prepare_loading() {
        loading_environment.resolution_scale = 1;
        loading_environment.raster_scale = 1;
        if (!read_bank(loading_animations, "sig.anm", 1) || !read_bank(loading_animations, "ascii.anm", 2) ||
            !read_bank(loading_animations, "text.anm", 0))
            return false;
        loading_signature = loading_animations.create(1, 0);
        if (!loading_signature) return fail("loading signature create: " + loading_animations.error);
        if (auto* boot = loading_animations.registry.find(loading_signature))
            if (auto* right = loading_animations.registry.find_child_script(*boot, 1, 0)) right->visual.layer = 26;
        loading_frames = 0;
        return render_loading_frame(0);
    }
    bool render_loading_frame(unsigned frame) {
        capture.begin_frame(frame);
        renderer.invalidate();
        const auto factor_kept = capture.pipeline().textureFactor;
        capture.pipeline() = touhou::graphics::PipelineState{};
        capture.pipeline().textureFactor = factor_kept;
        th13::ScreenViews loading_views{renderer, loading_environment};
        if (!loading_views.camera(th13::DrawCamera::Fullscreen, false)) return fail("loading camera unavailable");
        if (frame != 0) th13::emit_title_side_strips(loading_animations);
        if (frame < 180)
            for (th13::u32 layer = 0; layer < 33; layer++)
                if (!renderer.draw_layer(loading_animations.registry.layer(layer)))
                    return fail("loading layer draw: " + renderer.error);
        renderer.flush();
        capture.clear_device_texture();
        return capture.end_frame();
    }
    bool advance_loading(unsigned frame) {
        if (!loading_animations.update(false) || !loading_animations.update(true))
            return fail("loading tick: " + loading_animations.error);
        if (frame == 1 && !loading_animations.update(false)) return fail("loading tick: " + loading_animations.error);
        ++loading_frames;
        return render_loading_frame(frame);
    }
    bool prepare_title() {
        environment.raster_scale = 1;
        environment.render_target_has_alpha = true;
        environment.screen_offsets = {32, 16, 320, 16};
        environment.playfield_origin = {32, 16};
        constexpr std::pair<const char*, th13::i32> banks[7] = {
            {"text.anm", 0},   {"sig.anm", 1},      {"ascii.anm", 2}, {"effect.anm", 8},
            {"title.anm", 16}, {"title_v.anm", 17}, {"help.anm", 19}};
        for (const auto& bank : banks) {
            if (!read_bank(animations, bank.first, bank.second)) return false;
            if (bank.second == 2) environment.fallback_sprite_resource = animations.resource(2);
        }
        if (const auto* text = animations.resource(0)) {
            if (text->textures.size() < 4) return fail("text.anm @R entries missing");
            std::printf("title rt identities: (0,2,%s) (0,3,%s)\n", text->textures[2].name.c_str(),
                        text->textures[3].name.c_str());
            if (text->textures[2].name != "@R" || text->textures[3].name != "@R")
                return fail("text.anm entry2/3 not @R: contract identity mismatch");
        } else {
            return fail("text.anm bank unavailable");
        }
        animation_updates[0].owner = animation_updates[1].owner = this;
        animation_updates[0].enabled = animation_updates[1].enabled = true;
        animation_updates[0].run = [](void* p) {
            return static_cast<TitleSession*>(p)->animations.update(true) ? th13::i32(th13::FrameAction::Continue)
                                                                          : th13::i32(th13::FrameAction::Error);
        };
        animation_updates[1].run = [](void* p) {
            return static_cast<TitleSession*>(p)->animations.update(false) ? th13::i32(th13::FrameAction::Continue)
                                                                           : th13::i32(th13::FrameAction::Error);
        };
        if (scheduler.add(animation_updates[0], th13::FramePass::Update, 9) < 0 ||
            scheduler.add(animation_updates[1], th13::FramePass::Update, 34) < 0)
            return fail("animation callback registration failed");
        draw_schedule = std::make_unique<th13::AnmDrawSchedule>(scheduler, animations, renderer, capture, views);
        if (!draw_schedule->error.empty()) return fail(draw_schedule->error);
        title = std::make_unique<th13::TitleScene>(progress, selection, animations, host, scheduler,
                                                   th13::TitleSceneResources{});
        if (!title->error.empty()) return fail(title->error);
        title->state.return_reason = 0;
        return true;
    }
    bool title_step(unsigned frame, th13::u32 held, th13::u32 pressed) {
        title->controls({held, pressed, 0, 0});
        if (scheduler.update() < 0)
            return fail(title->error.empty() ? (title->frame.error.empty() ? animations.error : title->frame.error)
                                             : title->error);
        capture.begin_frame(frame);
        if (scheduler.draw() < 0)
            return fail(!draw_schedule->error.empty() ? draw_schedule->error
                        : title->error.empty()        ? "title draw failed"
                                                      : title->error);
        renderer.flush();
        capture.clear_device_texture();
        return capture.end_frame();
    }
    void teardown() {
        title.reset();
        for (auto& callback : animation_updates)
            scheduler.remove(callback);
        draw_schedule.reset();
        capture.close_output();
    }
};

struct TitleWindowCut {
    unsigned prefix = 0, frames = 0, min_ops = 0;
    unsigned long long max_ops = 0, total_ops = 0;
    bool ok = false;
};

unsigned title_line_ops(const std::string& line) {
    unsigned ops = 0;
    for (size_t at = line.find("\"seq\":"); at != std::string::npos; at = line.find("\"seq\":", at + 1))
        ops++;
    return ops;
}

TitleWindowCut cut_title_window(const char* path, unsigned want, bool fixed_window) {
    TitleWindowCut out;
    std::vector<th13::u8> bytes;
    if (!read_whole_file(path, bytes)) return out;
    std::vector<std::string> lines;
    std::string current;
    for (th13::u8 c : bytes) {
        if (c == '\n') {
            if (!current.empty()) lines.push_back(current);
            current.clear();
        } else {
            current.push_back(char(c));
        }
    }
    if (!current.empty()) lines.push_back(current);
    const auto drawn = [](const std::string& line) { return line.find("\"ops\":[]") == std::string::npos; };
    size_t first = 0;
    while (first < lines.size() && !drawn(lines[first]))
        first++;
    if (first >= lines.size()) return out;
    size_t end = lines.size();
    if (fixed_window) {
        end = first + want;
        if (end > lines.size()) return out;
    } else {
        while (end > first && !drawn(lines[end - 1]))
            end--;
    }
    std::FILE* file = std::fopen(path, "wb");
    if (!file) return out;
    unsigned index = 0;
    for (size_t i = first; i < end; i++) {
        std::string& line = lines[i];
        const size_t at = line.find("\"frame\":");
        const size_t digits = line.find(',', at + 8);
        if (at == std::string::npos || digits == std::string::npos) {
            std::fclose(file);
            return out;
        }
        line.replace(at + 8, digits - (at + 8), std::to_string(index));
        const unsigned ops = title_line_ops(line);
        if (index == 0 || ops < out.min_ops) out.min_ops = ops;
        if (ops > out.max_ops) out.max_ops = ops;
        out.total_ops += ops;
        line.push_back('\n');
        std::fwrite(line.data(), 1, line.size(), file);
        index++;
    }
    std::fclose(file);
    out.prefix = unsigned(first);
    out.frames = index;
    out.ok = index > 0;
    return out;
}

std::map<unsigned, th13::u32> title_input_script(bool window_b) {
    if (window_b) return {{470, 0x01}, {530, 0x20}, {590, 0x01}, {650, 0x80}, {710, 0x01}};
    return {{380, 0x20}, {420, 0x10}, {470, 0x01}};
}

int run_title(const char* resource_dir, const char* draws_path, bool window_b) {
    std::printf("title mode start dir=%s out=%s window=%c\n", resource_dir, draws_path, window_b ? 'b' : 'a');
    std::fflush(stdout);
    constexpr unsigned kBootFrames = 180;
    const std::unique_ptr<TitleSession> session(new TitleSession());
    session->dir = resource_dir;
    th13::draw_capture = &session->capture;
    if (!session->capture.open_output(draws_path)) {
        std::printf("title output failed: %s\n", session->capture.error().c_str());
        return 1;
    }
    if (!session->prepare_loading()) {
        std::printf("title boot failed: %s\n", session->error.c_str());
        return 1;
    }
    std::printf("title boot prepared sig=%u\n", session->loading_signature);
    std::fflush(stdout);
    for (unsigned frame = 1; frame <= kBootFrames; frame++) {
        if (!session->advance_loading(frame)) {
            std::printf("title boot failed at frame %u: %s\n", frame, session->error.c_str());
            return 1;
        }
        if (frame % 60 == 0) {
            std::printf("title boot frame=%u ops=%llu\n", frame,
                        static_cast<unsigned long long>(session->capture.quads() + session->capture.ups()));
            std::fflush(stdout);
        }
    }
    if (!session->prepare_title()) {
        std::printf("title load failed: %s\n", session->error.c_str());
        return 1;
    }
    std::printf("title scene ready\n");
    std::fflush(stdout);
    const std::map<unsigned, th13::u32> script = title_input_script(window_b);
    const unsigned first_step = kBootFrames + 1;
    const unsigned cap = window_b ? 900 : first_step + 429;
    unsigned transition_frame = 0;
    for (unsigned frame = first_step; frame < cap; frame++) {
        const auto at = script.find(frame);
        const th13::u32 bits = at == script.end() ? 0 : at->second;
        if (!session->title_step(frame, bits, bits)) {
            std::printf("title error at frame %u: %s\n", frame, session->error.c_str());
            return 1;
        }
        if (window_b && session->host.run_requested) {
            transition_frame = frame;
            break;
        }
        if (frame % 60 == 0) {
            std::printf("title frame=%u screen=%d substate=%d ops=%llu\n", frame, i32(session->title->state.screen),
                        session->title->state.substate,
                        static_cast<unsigned long long>(session->capture.quads() + session->capture.ups()));
            std::fflush(stdout);
        }
    }
    if (window_b && transition_frame == 0) {
        std::printf("title-b: no game start request within %u frames\n", cap);
        return 1;
    }
    const unsigned long long quads = session->capture.quads(), ups = session->capture.ups();
    const unsigned long long raw_frames = session->capture.frames_out();
    session->teardown();
    const TitleWindowCut cut = cut_title_window(draws_path, window_b ? 0 : 600, !window_b);
    if (!cut.ok) {
        std::printf("title window cut failed (raw frames=%llu)\n", raw_frames);
        return 1;
    }
    std::printf("port-title-draws window=%c prefix=%u frames=%u min_ops=%u max_ops=%llu "
                "total_ops=%llu transition=%u stage=%d quads=%llu ups=%llu raw=%llu\n",
                window_b ? 'b' : 'a', cut.prefix, cut.frames, cut.min_ops, cut.max_ops, cut.total_ops, transition_frame,
                session->host.requested_stage, quads, ups, raw_frames);
    return 0;
}

}

int main(int argc, char** argv) {
    if (argc >= 4 && std::strcmp(argv[1], "stage1") == 0)
        return stage1(argv[2], argv[3], argc >= 5 ? unsigned(std::atoi(argv[4])) : 3600u,
                      argc >= 6 ? argv[5] : nullptr);
    if (argc >= 5 && std::strcmp(argv[1], "replay") == 0)
        return replay_runscene(argv[2], argv[3], argv[4], argc >= 6 ? unsigned(std::atoi(argv[5])) : 0u,
                               argc >= 7 ? unsigned(std::atoi(argv[6])) : 0u);
    if (argc >= 4 && std::strcmp(argv[1], "draws") == 0)
        return run_draws(argv[2], argv[3], argc >= 5 ? unsigned(std::atoi(argv[4])) : 3600u,
                         argc >= 6 ? argv[5] : nullptr);
    if (argc >= 5 && std::strcmp(argv[1], "draws-replay") == 0)
        return run_draws_replay(argv[2], argv[3], argv[4], argc >= 6 ? unsigned(std::atoi(argv[5])) : 0u,
                                argc >= 7 ? unsigned(std::atoi(argv[6])) : 0u);
    if (argc >= 4 && (std::strcmp(argv[1], "title") == 0 || std::strcmp(argv[1], "title-b") == 0))
        return run_title(argv[2], argv[3], std::strcmp(argv[1], "title-b") == 0);
    return rng_smoke();
}
