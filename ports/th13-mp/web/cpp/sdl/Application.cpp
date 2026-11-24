#include "ApplicationState.hpp"
#include "LanguagePack.hpp"
#include "StageTransitionClock.hpp"
#include "game/ReplayFastForward.hpp"
#include "game/TitleFrame.hpp"
#include "game/VisualBootstrap.hpp"
#include "multiplayer/Session.hpp"
#include "multiplayer/StageEntry.hpp"
#include <eagler/input/LocalPlayerCue.hpp>

#include <SDL3/SDL.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace th13 {
extern bool damage_probe_enabled;
}
namespace th13::sdl {

namespace {
constexpr std::pair<const char*, i32> kBanks[7] = {
    {"text.anm", 0},   {"sig.anm", 1},      {"ascii.anm", 2}, {"effect.anm", 8},
    {"title.anm", 16}, {"title_v.anm", 17}, {"help.anm", 19},
};
constexpr i32 kFrontBank = 3;
constexpr i32 kStageBank = 4, kBulletBank = 5, kPlayerBank = 6, kSceneryBank = 7;
constexpr std::array<i32, 2> kTeammateBanks{101, 102};
constexpr i32 kSpiritBank = 25;
constexpr i32 kStageLogoBank = 27;

void sync_mp_prediction_barrier(ApplicationState& app) {
    if (!multiplayer::session.Active()) return;
    const bool transition = app.stage_transition_loading || app.stage_transition_active || app.pending_destination >= 0;
    const bool paused = app.pause && app.pause->active();
    multiplayer::session.SetPredictionBarrier(transition || paused);
}
}

Application::Application() : state(std::make_unique<ApplicationState>()) {}
Application::~Application() = default;
bool Application::initialize(bool device) { return state->initialize(device); }
bool Application::step(const GameInput& input, float fps, bool focus) { return state->step(input, fps, focus); }
const std::string& Application::error() const { return state->failure; }
const i32* Application::projection() const { return state->projection(); }

bool ApplicationState::fail(const std::string& why) {
    if (failure.empty()) failure = why.empty() ? "Application operation failed" : why;
    return false;
}

bool ApplicationState::read(const std::string& name, std::vector<u8>& bytes, bool required) {
    const auto at = name.find_last_of("/\\");
    const auto base = name.substr(at == std::string::npos ? 0 : at + 1);
    if (base.empty() || base == "." || base == "..") return fail("Invalid game asset name");
    const auto found = resource_cache.find(base);
    if (found != resource_cache.end()) {
        bytes = found->second;
        return true;
    }
    size_t size = 0;
    auto* data = static_cast<u8*>(nullptr);
    if (language_pack_active() && language_asset_allowed(base)) {
        std::vector<u8> patched;
        if (read_language_file(language_asset_path(base), patched)) {
            bytes = std::move(patched);
            if (bytes.size() <= 1024 * 1024 && base.size() >= 4 && base.substr(base.size() - 4) != ".wav" &&
                base.substr(base.size() - 4) != ".anm")
                resource_cache.emplace(base, bytes);
            return true;
        }
    }
    data = static_cast<u8*>(SDL_LoadFile(("/assets/" + base).c_str(), &size));
    if (!data) return required ? fail("Missing game asset: " + base) : false;
    bytes.assign(data, data + size);
    SDL_free(data);
    if (bytes.size() <= 1024 * 1024 && base.size() >= 4 && base.substr(base.size() - 4) != ".wav" &&
        base.substr(base.size() - 4) != ".anm")
        resource_cache.emplace(base, bytes);
    return true;
}

bool ApplicationState::prepare_platform() {
    if (platform_prepared) return true;
    if (!graphics.initialize()) return fail(graphics.error);
#ifdef __EMSCRIPTEN__
    // clang-format off
    const int override = EM_ASM_INT({ return window.__th13_boot_seed ?? -1; });
    // clang-format on
    boot_seed = override >= 0 ? u16(override) : u16(SDL_GetTicks());
#else
    boot_seed = u16(SDL_GetTicks());
#endif
    visual.reseed(boot_seed);
    game_random.reseed(boot_seed);
    environment.shared_game_rng = &game_random;
    loading_environment.shared_game_rng = &game_random;
    platform_prepared = true;
    return true;
}

#ifdef __EMSCRIPTEN__
extern "C" void th13_browser_replay_request();
#endif

i32 ApplicationState::replay_count() const { return i32(replay_slots.size()); }

const ReplayFile* ApplicationState::replay_entry(i32 index) const {
    if (index < 0 || index >= i32(replay_slots.size())) return nullptr;
    const ReplayCatalogSlot& slot = replay_slots[u32(index)];
    return slot.decoded ? slot.decoded.get() : nullptr;
}

std::string ApplicationState::replay_filename(i32 index) const {
    if (index < 0 || index >= i32(replay_slots.size())) return {};
    return replay_slots[u32(index)].name;
}

bool ApplicationState::request_replay_file() {
#ifdef __EMSCRIPTEN__
    th13_browser_replay_request();
    return true;
#else
    replay_import_error_ = "Replay import needs the browser runtime";
    return false;
#endif
}

std::string ApplicationState::replay_import_error() const { return replay_import_error_; }

bool ApplicationState::start_replay(i32 file_index, i32 stage_index) {
    if (file_index < 0 || file_index >= i32(replay_slots.size()) || !replay_slots[u32(file_index)].decoded)
        return fail("Replay start on empty slot: " + std::to_string(file_index));
    if (stage_index < 0) return fail("Replay start with negative stage index");
    if (!begin_run_transition()) return false;
    pending_replay_file = file_index;
    replay_catalog_index = file_index;
    pending_replay_stage_slot = stage_index;
    pending_destination = i32(Destination::Run);
    return true;
}

bool ApplicationState::import_replay_bytes(const u8* data, size_t size, const std::string& name) {
    auto decoded = std::make_shared<ReplayFile>();
    if (!decoded->decode(data, size)) {
        replay_import_error_ = name + ": " + decoded->error();
        return false;
    }
    i32 slot = -1;
    if (name.size() >= 11 && name.compare(0, 5, "th13_") == 0 && name.compare(7, 4, ".rpy") == 0 &&
        std::all_of(name.begin() + 5, name.begin() + 7, [](char c) { return c >= '0' && c <= '9'; })) {
        const i32 number = (name[5] - '0') * 10 + (name[6] - '0');
        if (number >= 1 && number <= 25) slot = number - 1;
    }
    if (slot < 0) {
        for (i32 user = 25; user < i32(replay_slots.size()) - 1; user++) {
            if (!replay_slots[u32(user)].decoded) {
                slot = user;
                break;
            }
        }
        if (slot < 0) {
            replay_import_error_ = name + ": replay slots full";
            return false;
        }
    }
    ReplayCatalogSlot& target = replay_slots[u32(slot)];
    target.name = name;
    target.bytes.assign(data, data + size);
    target.decoded = std::move(decoded);
    replay_import_error_.clear();
    if (title) title->state.replay_import_hint = slot;
    return true;
}

bool ApplicationState::prepare_loading() {
    if (!prepare_platform()) return false;
    std::vector<u8> bytes;
    loading_environment.resolution_scale = 1;
    loading_environment.raster_scale = float(graphics.render_scale);
    constexpr std::pair<const char*, i32> loading_banks[3] = {{"sig.anm", 1}, {"ascii.anm", 2}, {"text.anm", 0}};
    for (const auto& bank : loading_banks) {
        if (!loading_animations.resource(bank.second) &&
            (!read(bank.first, bytes) || !loading_animations.load(bank.second, bytes.data(), u32(bytes.size())) ||
             !graphics.preload(*loading_animations.resource(bank.second))))
            return fail("Loading artwork: " + loading_animations.error + graphics.error);
    }
    return draw_loading(true);
}

bool ApplicationState::loading_waiting() const { return loading_startup || pending_load != 0; }

bool ApplicationState::draw_loading(bool signature) {
    graphics.presentation.reset();
    if (!loading_animations.retire(loading_signature) || !loading_animations.retire(loading_prayer))
        return fail(loading_animations.error);
    if (signature) {
        loading_signature = loading_animations.create(1, 0);
        if (!loading_signature) return fail(loading_animations.error);
        if (auto* boot = loading_animations.registry.find(loading_signature))
            if (auto* right = loading_animations.registry.find_child_script(*boot, 1, 0)) right->visual.layer = 26;
    }
    loading_prayer = loading_animations.create(2, 17, -1, 0, {480, 392, 0});
    if (!loading_prayer) return fail(loading_animations.error);
    loading_startup = signature;
    loading_frames = 0;
    loading_debt = 0;
    return render_loading();
}

bool ApplicationState::render_loading() {
    renderer.invalidate();
    const auto factor_kept = graphics.backend.pipeline().textureFactor;
    graphics.backend.pipeline() = touhou::graphics::PipelineState{};
    graphics.backend.pipeline().textureFactor = factor_kept;
    if (!graphics.select_target(nullptr, 0) || !graphics.clear_target(0xff000000, nullptr)) return fail(graphics.error);
    ScreenViews loading_views{renderer, loading_environment};
    if (!loading_views.camera(DrawCamera::Fullscreen, false)) return fail("Loading camera unavailable");
    emit_title_side_strips(loading_animations);
    if (loading_frames < 180) {
        const auto* credit = loading_animations.registry.find(loading_signature);
        for (u32 layer = 0; layer < 33; ++layer) {
            if (!renderer.draw_layer(loading_animations.registry.layer(layer))) return fail(renderer.error);
            if (credit && layer == u32(credit->visual.layer)) {
                renderer.flush();
                graphics.draw_startup_branding(credit->visual.color);
            }
        }
    }
    renderer.flush();
    graphics.present();
    return true;
}

bool ApplicationState::advance_loading_caption() {
    if (!loading_animations.update(false) || !loading_animations.update(true)) return fail(loading_animations.error);
    return true;
}

bool ApplicationState::commit_loading_battle_caption() {
    if (!run || !loading_prayer) return true;
    if (!loading_animations.registry.find(loading_prayer)) return true;
    const i32 battle_frame = progress.stage_frame - 1;
    if (battle_frame < 0 || battle_frame > 10) return true;
    if (battle_frame == 10) {
        if (!loading_animations.retire(loading_prayer)) return fail(loading_animations.error);
        return true;
    }
    if (battle_frame >= 1) {
        if (!loading_animations.update(true)) return fail(loading_animations.error);
    }
    return true;
}

bool ApplicationState::draw_loading_battle() {
    if (!run || !loading_prayer) return true;
    if (!loading_animations.registry.find(loading_prayer)) return true;
    const i32 battle_frame = progress.stage_frame - 1;
    if (battle_frame < 0 || battle_frame > 9) return true;
    struct SavedColor {
        AnmVm* vm;
        u32 color;
        u32 secondary;
    };
    std::vector<SavedColor> saved;
    auto& fore = loading_animations.registry.layer(31);
    saved.reserve(fore.size());
    const u32 fade = u32(30 - battle_frame);
    for (auto* vm : fore) {
        saved.push_back({vm, vm->visual.color, vm->visual.secondary_color});
        u32 alpha;
        switch (vm->source_script) {
        case 14:
            alpha = u32(u8(0x9Fu * fade / 30u));
            break;
        case 15:
            alpha = 0;
            break;
        case 16:
            alpha = u32(u8(0x66u * (10u - u32(battle_frame)) / 10u));
            break;
        default:
            continue;
        }
        vm->visual.color = (vm->visual.color & 0x00ffffffu) | (alpha << 24);
        vm->visual.secondary_color = (vm->visual.secondary_color & 0x00ffffffu) | (alpha << 24);
    }
    renderer.invalidate();
    bool drawn = views.loading_camera();
    if (drawn) {
        graphics.set_fog(false);
        graphics.set_depth_mask(false);
        graphics.set_depth_compare(touhou::graphics::Compare::Always);
        renderer.offset = {};
        drawn = renderer.draw_layer(fore);
    }
    for (const auto& item : saved) {
        item.vm->visual.color = item.color;
        item.vm->visual.secondary_color = item.secondary;
    }
    if (!drawn) return fail(renderer.error.empty() ? "Loading caption camera unavailable" : renderer.error);
    return true;
}

bool ApplicationState::draw_fps_text() {
    if (!run) return true;
    const float color_fps = fps_color_override_ >= 0 ? fps_color_override_ : last_fps_;
    if (replay_active) {
        i32 integer_fps = i32(last_fps_);
        if (replay_cursor) {
            if (const ReplayStage* stage = replay_cursor->stage()) {
                const u32 tick = (replay_consumed + 28) / 30;
                if (tick < stage->events.size()) integer_fps = i32(stage->events[tick]);
            }
        }
        HudTextStyle integer_style;
        integer_style.font = 0;
        integer_style.color = integer_fps < 30 ? 0xff5050ffu : integer_fps < 50 ? 0xffa0a0ffu : 0xffffffffu;
        char integer_text[16];
        std::snprintf(integer_text, sizeof integer_text, "%3d", integer_fps);
        if (!captions.enqueue({{383.0f, 450.0f, 0}, integer_style, integer_text})) return fail(captions.error);
    }
    HudTextStyle style;
    style.font = 1;
    style.color = color_fps < 30 ? 0xff5050ffu : color_fps < 40 ? 0xffa0a0ffu : 0xffffffffu;
    char text[32];
    std::snprintf(text, sizeof text, "%2.1ffps", color_fps);
    if (!captions.enqueue({{588, 470, 0}, style, text})) return fail(captions.error);
    return true;
}

bool ApplicationState::flush_animation_captures() {
    auto captures = animations.take_captures();
    for (const auto& capture : captures) {
        if (!capture.resource) continue;
        (void)graphics.capture_region_to_texture(*capture.resource, capture.texture, capture.source,
                                                 capture.destination);
    }
    return true;
}

bool ApplicationState::advance_loading(double delta) {
    loading_debt += std::clamp(delta, 0., .1);
    while (loading_debt + 1e-9 >= 1. / 60. && loading_frames < 180) {
        loading_debt = std::max(0., loading_debt - 1. / 60.);
        if (!advance_loading_caption()) return false;
        ++loading_frames;
    }
    if (!render_loading()) return false;
    if (loading_frames >= 180) {
        if (!loading_animations.retire(loading_signature) || !loading_animations.retire(loading_prayer))
            return fail(loading_animations.error);
        loading_startup = false;
        loading_debt = 0;
    }
    return true;
}

bool ApplicationState::initialize(bool device) {
    if (initialized) return fail("Application already initialized");
    if (!prepare_platform()) return false;
    if (!loading_animations.resource(2) && !prepare_loading()) return false;
    if (!fonts.initialize()) fonts_failure = fonts.error;
    visual_font_noise_table = visual_font_noise(visual);
    if (!audio_device.initialize(device)) return fail(audio_device.error);
    environment.raster_scale = float(graphics.render_scale);
    environment.render_target_has_alpha = true;
    environment.screen_offsets = {32, 16, 320, 16};
    environment.playfield_origin = {32, 16};
    if (!files.initialize()) return fail(files.error);
    audio_device.music_volume = files.bgm_volume;
    audio_device.effect_volume = files.effect_volume;
    load_music_room_catalog();
    connect_record_menus();
    animations.resource_release = [this](const AnmResource& resource) { graphics.unload(resource); };
    std::vector<u8> bytes;
    for (const auto& bank : kBanks) {
        if (!read(bank.first, bytes) || !animations.load(bank.second, bytes.data(), u32(bytes.size())))
            return fail("Application ANM load: " + std::string(bank.first) + ": " + animations.error);
        animations.name_resource(bank.second, bank.first);
        if (bank.second == 2) environment.fallback_sprite_resource = animations.resource(2);
        if (!graphics.preload(*animations.resource(bank.second))) return fail(graphics.error);
    }
    if (fonts.ready()) {
        const AnmResource* text = animations.resource(0);
        if (!text || text->scripts.size() < 4 || text->sprites.size() < 4 || text->textures.empty() ||
            text->textures[0].format != 5 || text->textures[0].kind == AnmTexture::Kind::External ||
            text->textures[0].kind == AnmTexture::Kind::RenderTarget)
            fonts_failure = "TH13 text.anm texture0 dialogue layout unavailable";
        else
            fonts_failure.clear();
    }
    display =
        std::make_unique<SceneDisplay>(scheduler, animations, environment, graphics, renderer, views, captions, 2);
    if (!display->initialize()) return fail(display->error);
    display->before_present = [this] { return flush_animation_captures(); };
    for (u32 index = 0; index < animation_updates.size(); index++) {
        auto& callback = animation_updates[index];
        callback.owner = this;
        callback.enabled = true;
        callback.run = index == 0
            ? [](void* p) -> i32 { return static_cast<ApplicationState*>(p)->animations.update(true) ? 1 : 5; }
        : [](void* p) -> i32 { return static_cast<ApplicationState*>(p)->animations.update(false) ? 1 : 5; };
        if (scheduler.add(callback, FramePass::Update, index == 0 ? 9 : 34) < 0)
            return fail("Application animation callback registration failed");
    }
    loading_battle_draw.owner = this;
    loading_battle_draw.enabled = true;
    loading_battle_draw.run = [](void* p) -> i32 {
        return static_cast<ApplicationState*>(p)->draw_loading_battle() ? 1 : 5;
    };
    if (scheduler.add(loading_battle_draw, FramePass::Draw, 72) < 0)
        return fail("Loading caption draw callback registration failed");
    fps_text_draw.owner = this;
    fps_text_draw.enabled = true;
    fps_text_draw.run = [](void* p) -> i32 { return static_cast<ApplicationState*>(p)->draw_fps_text() ? 1 : 5; };
    if (scheduler.add(fps_text_draw, FramePass::Draw, 77) < 0)
        return fail("FPS text draw callback registration failed");
    replay_fast_forward.owner = this;
    replay_fast_forward.enabled = true;
    replay_fast_forward.run = [](void* p) -> i32 {
        auto& app = *static_cast<ApplicationState*>(p);
        if (!app.run || !app.replay_active || !app.physical_input || app.pending_destination >= 0) return 1;
        app.restart_update = replay_fast_forward_restart(
            app.physical_input->held, app.replay_consumed, app.replay_consumed != app.replay_pass_start,
            app.environment.paused, app.progress.scene_flags, app.replay_finished);
        return app.restart_update ? 3 : 1;
    };
    if (scheduler.add(replay_fast_forward, FramePass::Update, 30) < 0)
        return fail("Replay fast-forward callback registration failed");
    ending_update.owner = this;
    ending_update.enabled = true;
    ending_update.run = [](void* p) -> i32 {
        auto& app = *static_cast<ApplicationState*>(p);
        if (!app.ending || !app.physical_input) return 1;
        if (!app.ending->update(*app.physical_input)) return 5;
        app.restart_update = app.pending_destination < 0 && app.ending->should_restart(*app.physical_input);
        return app.restart_update ? 3 : 1;
    };
    if (scheduler.add(ending_update, FramePass::Update, 31) < 0)
        return fail("Ending update callback registration failed");
    initialized = true;
    return begin_title(0);
}

ApplicationState::~ApplicationState() {
    clear_ending_windows();
    (void)release_run_transition();
    title.reset();
    if (ending) {
        ending->teardown();
        ending.reset();
    }
    release_run();
    scheduler.remove(loading_battle_draw);
    scheduler.remove(fps_text_draw);
    scheduler.remove(replay_fast_forward);
    scheduler.remove(ending_update);
    for (auto& callback : animation_updates)
        scheduler.remove(callback);
    display.reset();
    animations.resource_release = nullptr;
}

void ApplicationState::release_run(bool preserve_hud) {
    const bool transition_alive = run_transition.active();
    stage_assets.discard();
    audio_device.stop(-1);
    graphics.presentation.reset();
    pause.reset();
    playfield_targets.reset();
    if (multiplayer::session.Active()) {
        multiplayer::session.BindWorld(nullptr, nullptr, nullptr);
        multiplayer::session.capture_host = {};
        multiplayer::session.digest_host = {};
    }
    run.reset();
    if (!preserve_hud) {
        run_hud.reset();
        run_player.reset();
    }
    dialogue_text.release(animations);
    for (const auto bank : {kFrontBank, kStageBank, kBulletBank, kPlayerBank, kSceneryBank, 26, kStageLogoBank,
                            kSpiritBank, kTeammateBanks[0], kTeammateBanks[1]}) {
        if (preserve_hud && bank == kFrontBank) continue;
        animations.unload(bank);
    }
    if (!preserve_hud)
        for (const auto bank : {0, 2, 8})
            if (!(transition_alive && bank == 8)) animations.retire_resource(bank);
    animations.retire(transition_prayer);
    transition_prayer = 0;
    transition_scenery_pending = false;
    transition_scenery_bank = -1;
    transition_scenery_std.clear();
    animations.collect_resources();
    for (auto& callback : animation_updates)
        callback.enabled = true;
    environment.paused = false;
    environment.screen_translation = {};
    environment.background_delta = {};
    environment.screen_offsets = {32, 16, 320, 16};
    environment.playfield_origin = {32, 16};
    views.configure(environment);
    renderer.invalidate();
    const auto factor_kept = graphics.backend.pipeline().textureFactor;
    graphics.backend.pipeline() = touhou::graphics::PipelineState{};
    graphics.backend.pipeline().textureFactor = factor_kept;
    if (display) display->detach();
}

bool ApplicationState::preload_scenery_animation(i32 bank, const std::string& name) {
    std::vector<u8> bytes;
    if (animations.resource(bank)) return true;
    if (adopt_staged_bank(bank, name)) {
        animations.name_resource(bank, name);
        return true;
    }
    if (!read(name, bytes) || !animations.load(bank, bytes.data(), u32(bytes.size())))
        return fail("Scenery ANM load: " + name + ": " + animations.error);
    animations.name_resource(bank, name);
    if (!graphics.preload(*animations.resource(bank)))
        return fail("Scenery ANM preload: " + name + ": " + graphics.error);
    return true;
}

bool ApplicationState::adopt_staged_bank(i32 bank, const std::string& key) {
    auto staged = stage_assets.take(key);
    if (!staged) return false;
    const AnmResource* source = staged.get();
    if (!animations.adopt(bank, std::move(*staged))) {
        graphics.unload(*source);
        return false;
    }
    graphics.rebind(*source, *animations.resource(bank));
    return graphics.preload(*animations.resource(bank));
}

bool ApplicationState::begin_title(i32 reason) {
    if (run && multiplayer::session.Simulating() && !multiplayer::session.FinishWorldSimulation())
        return fail("Unconfirmed multiplayer title handoff");
    if (!release_run_transition()) return false;
    if (mp_seat_count < 2 && (run || ordinary_records_dirty)) {
        if (!(run ? files.save_score() : files.save())) return fail(files.error);
        ordinary_records_dirty = false;
    }
    if (replay_active && reason == 1) reason = 2;
    if (returns_to_practice_selection(progress, reason, replay_active))
        reason = 4;
    else if (returns_to_spell_practice_selection(progress, reason, replay_active))
        reason = 5;
    if (ending) {
        ending->teardown();
        ending.reset();
    }
    clear_ending_windows();
    stage_transition_active = stage_transition_loading = false;
    stage_startup_age = -1;
    pending_replay_stage = nullptr;
    pending_replay_stage_index = 0;
    title.reset();
    release_run();
    replay_active = false;
    replay_finished = false;
    replay_cursor.reset();
    if (!audio_device.queue_music(2, -1, reason == 3 ? "th128_08.wav" : "th13_00.wav")) return fail(audio_device.error);
    current_destination = i32(Destination::Title);
    title = std::make_unique<TitleScene>(progress, selection, animations, *this, scheduler, TitleSceneResources{});
    title->state.return_reason = reason;
    if (reason == 2) title->state.replay_index = replay_catalog_index;
    return title->error.empty() || fail(title->error);
}

bool ApplicationState::begin_run_transition() {
    if (!animations.resource(8)) return fail("Run transition effect bank unavailable");
    if (!run_transition.create())
        return fail(run_transition.error.empty() ? "Run transition effect create failed" : run_transition.error);
    return true;
}

bool ApplicationState::stage_start_run_transition() {
    if (!run_transition.stage_start())
        return fail(run_transition.error.empty() ? "Run transition stage start failed" : run_transition.error);
    return true;
}

bool ApplicationState::release_run_transition() {
    if (!run_transition.release())
        return fail(run_transition.error.empty() ? "Run transition effect retire failed" : run_transition.error);
    return true;
}

bool ApplicationState::begin_run() {
    const bool new_run = progress.new_run;
    const bool preserve_hud = run && !new_run;
    const bool continuing = stage_transition_active && !new_run;
    const bool carry_multiplayer = continuing && run && run->battle.seat_count >= 2;
    multiplayer::StageEntry carry;
    const u32 carry_death_generation = carry_multiplayer ? run->battle.death_trance_generation : 0;
    if (carry_multiplayer) {
        carry.Capture(run->battle, progress, selection_score, game_random.seed(), game_random.index(), visual.seed(),
                      visual.index());
        selection_player = run->battle.seat_huds[0];
        carry.PrepareStageClear(selection_player, run->battle);
    }
    std::unique_ptr<RunScene> previous_run;
    const bool replay_session = pending_replay_stage != nullptr;
    if (mp_seat_count < 2 && run) {
        if (!files.save_score()) return fail(files.error);
        ordinary_records_dirty = false;
    }
    if (title) {
        const u32 title_effect = title->state.start_effect;
        title->state.start_effect = 0;
        if (!run_transition.adopt(title_effect))
            return fail(run_transition.error.empty() ? "Run transition adopt failed" : run_transition.error);
    }
    title.reset();
    if (continuing) {
        pause.reset();
        previous_run = std::move(run);
        for (i32 bank : {kStageBank, kBulletBank, kSpiritBank, kStageLogoBank})
            animations.unload(bank);
        animations.collect_resources();
    } else {
        release_run(preserve_hud);
        next_scenery_bank = 26;
    }
    if (mp_seat_count >= 2 && multiplayer::session.Active() && multiplayer::session.SimFrame() == 0 && !continuing) {
        if (!release_run_transition()) return false;
        clear_ending_windows();
        animations.registry = AnmRegistry{};
        animations.collect_resources();
        loading_animations.registry = AnmRegistry{};
        loading_signature = loading_prayer = 0;
        loading_animations.collect_resources();
        mp_transition_handle_ = 0;
        if (!begin_run_transition()) return false;
    }
    if (!ecl_difficulty_mask(progress.difficulty)) return fail("Invalid run difficulty");
    run_entry_reset_points(continuing, progress.difficulty, selection_score.score, progress.max_point,
                           progress.max_point_cap, progress.graze);
    const u32 difficulty = u32(progress.difficulty);
    spell_time_stage = progress.stage;
    spell_time_packed = 0;
    spell_time_clock.reset();
    progress.chapter = progress.chapter_frame = 0;
    for (auto& callback : animation_updates)
        callback.enabled = true;
    if (new_run) {
        static constexpr i32 continues_by_difficulty[] = {3, 3, 6, 9, 0, 0};
        progress.continue_stock = continues_by_difficulty[difficulty];
        progress.stage = selected_stage;
        progress.starting_stage = selected_stage;
    }
    progress.stage_frame = 0;
    progress.scene_flags = 0;
    progress.rate = 1;
    progress.new_run = false;
    replay_active = replay_session;
    const bool cold_replay = pending_replay_stage && !continuing;
    std::vector<u8> bytes;
    if (!animations.resource(kFrontBank) &&
        (!read("front.anm", bytes) || !animations.load(kFrontBank, bytes.data(), u32(bytes.size())) ||
         !graphics.preload(*animations.resource(kFrontBank))))
        return fail("Run front asset: " + animations.error + graphics.error);
    if (progress.stage < 1 || progress.stage > 7)
        return fail("Stage number out of 0x4bb4f0 table range: " + std::to_string(progress.stage));
    char stage_ecl[16];
    std::snprintf(stage_ecl, sizeof stage_ecl, "st%02d.ecl", progress.stage);
    std::vector<u8> ecl;
    if (!read(stage_ecl, ecl)) return fail(std::string("Stage ECL missing: ") + stage_ecl);
    EclResource head;
    if (!head.open(ecl.data(), u32(ecl.size())) || head.animations.empty())
        return fail("Stage ECL ANIM list unavailable");
    if (!stage_assets.staged_for(progress.stage)) stage_assets.discard();
    if (!adopt_staged_bank(kStageBank, StageAssetStager::kStageKey)) {
        AnmResource stage;
        for (const auto& name : head.animations) {
            if (!read(name, bytes)) return fail("Stage ANM missing: " + name);
            if (!stage.append(bytes.data(), u32(bytes.size()))) return fail("Stage ANM invalid: " + name);
        }
        if (!animations.adopt(kStageBank, std::move(stage)) || !graphics.preload(*animations.resource(kStageBank)))
            return fail("Stage ANM bank: " + animations.error + graphics.error);
    }
    if (!adopt_staged_bank(kBulletBank, "bullet.anm") &&
        (!read("bullet.anm", bytes) || !animations.load(kBulletBank, bytes.data(), u32(bytes.size())) ||
         !graphics.preload(*animations.resource(kBulletBank))))
        return fail("Bullet ANM bank: " + animations.error + graphics.error);
    if (!adopt_staged_bank(kSpiritBank, "astral.anm") &&
        (!read("astral.anm", bytes) || !animations.load(kSpiritBank, bytes.data(), u32(bytes.size())) ||
         !graphics.preload(*animations.resource(kSpiritBank))))
        return fail("Spirit ANM bank: " + animations.error + graphics.error);
    animations.name_resource(kSpiritBank, "astral.anm");
    const char* player_files[4] = {"pl00.anm", "pl01.anm", "pl02.anm", "pl03.anm"};
    const i32 character = progress.character & 3;
    if (!animations.resource(kPlayerBank) &&
        (!read(player_files[character], bytes) || !animations.load(kPlayerBank, bytes.data(), u32(bytes.size())) ||
         !graphics.preload(*animations.resource(kPlayerBank))))
        return fail("Player ANM bank: " + animations.error + graphics.error);
    StagePresentation::Banks banks{kStageBank, kBulletBank, kPlayerBank, 8, kSpiritBank, kFrontBank};
    for (i32 seat = 1; seat < mp_seat_count && seat < 3; ++seat) {
        const auto teammate = mp_characters[seat] & 3;
        const auto bank = teammate == character ? kPlayerBank : kTeammateBanks[seat - 1];
        if (!animations.resource(bank) &&
            (!read(player_files[teammate], bytes) || !animations.load(bank, bytes.data(), u32(bytes.size())) ||
             !graphics.preload(*animations.resource(bank))))
            return fail("Teammate ANM bank: " + animations.error + graphics.error);
        banks.teammate_player[seat - 1] = bank;
    }
    char stage_std[16];
    std::snprintf(stage_std, sizeof stage_std, "st%02d.std", progress.stage);
    if (!run_hud)
        run_hud = std::make_unique<GameHud>(animations, progress, selection_player, selection_score, kFrontBank, 2);
    char stage_logo[20];
    std::snprintf(stage_logo, sizeof stage_logo, "st%02dlogo.anm", progress.stage);
    if (!preload_scenery_animation(kStageLogoBank, stage_logo)) return false;
    run_hud->stage_logo_bank = kStageLogoBank;
    if (!run_player) run_player = std::make_unique<StagePlayerState>();
    run = std::make_unique<RunScene>(progress, selection_player, selection_score, animations, renderer, environment,
                                     *this, scheduler, banks, kFrontBank, 2, kSceneryBank, stage_std, &game_random,
                                     run_hud.get(), run_player.get());
    char sht[16];
    std::snprintf(sht, sizeof sht, "pl%02da.sht", character);
    run->battle_sht = sht;
    if (cold_replay) {
        pending_replay_stage = nullptr;
        run->restore_after_player_prepare = [this] {
            ReplayRestoreLog log;
            std::string restore_error;
            const ReplayRestoreTarget target{&progress, &selection_player, &selection_score, &game_random, &visual};
            if (!ReplaySession::restore_stage(replay_file, pending_replay_stage_index, target, log, restore_error,
                                              ReplayRestorePhase::Load))
                return fail(restore_error);
            ReplaySession::emit_log(log);
            return true;
        };
    }
    if (previous_run) {
        run->preserve_presentation_from(*previous_run);
        previous_run.reset();
    }
    if (!replay_session) {
        if (new_run && !recorder.start(ReplayRecorder::top_snapshot(progress)))
            return fail("Replay recording initialization failed");
        if (new_run && replay_top_pin_enabled_ && replay_top_pin_.size() == 0x74) recorder.start(replay_top_pin_);
        if (!begin_record_stage()) return false;
    }
    if (!run->initialize(HudEntryContext{0, new_run, title_flow_ran})) return fail(run->error);
    run->battle.challenge = mp_challenge;
    run->battle.keep_bullets_on_miss = mp_keep_bullets_on_miss;
    run->battle.item_manager.challenge = mp_challenge;
    run->hud.challenge = mp_challenge;
    if (mp_seat_count >= 2 && !run->enable_multiplayer(mp_seat_count, mp_characters))
        return fail(run->error.empty() ? "Multiplayer seat enable failed" : run->error);
    run->battle.sim.mp_boss_mitigation_legacy = mp_legacy_boss_mitigation;
    run->battle.item_manager.legacy_pickup_rewards = mp_legacy_pickup_rewards;
    if (carry_multiplayer) {
        auto& battle = run->battle;
        carry.RestoreBattleCarry(battle);
        if (!mp_legacy_stage_positions) carry.RestoreStageClearPositions(battle);
        battle.death_trance_generation = carry_death_generation;
        battle.revive_all_on_stage_clear();
        selection_player = battle.seat_huds[0];
    }
    if (mp_challenge) run->battle.apply_challenge_locks();
    if (mp_seat_count >= 2) {
        for (i32 seat = 0; seat < mp_seat_count; ++seat) {
            const HudPlayerState* seat_state = run->battle.seat_hud(seat);
            if (!seat_state) return fail("Multiplayer seat HUD state missing");
            const i32 lives = std::max(0, seat_state->extra_lives);
            if (!life_hud_seat(seat, lives, seat_state->life_pieces) ||
                !bomb_hud_seat(seat, seat_state->bombs, seat_state->bomb_pieces))
                return false;
        }
    }
    run->battle.mp_local_seat = mp_local_seat;
    if (mp_seat_count >= 2 && multiplayer::session.Active()) {
        multiplayer::session.capture_host = [this] { return capture_multiplayer_host(); };
        multiplayer::session.digest_host = [this] { return digest_multiplayer_host(); };
        multiplayer::session.BindWorld(&run->battle, &visual, run->presentation.get(), run.get(),
                                       &mp_transition_handle_);
        run->playback_input = [this]() -> const GameInput* {
            return mp_history_playback ? &run->battle.seat_inputs[0] : multiplayer::session.LocalDelayedInput();
        };
        wire_multiplayer_audio();
    }
    if (continuing) {
        if (!animations.retire(transition_prayer))
            return fail("Next-stage loading caption release: " + animations.error);
    }
    if (!stage_start_run_transition()) return false;
    if (!replay_session)
        run->record_input = [this](const GameInput& input) {
            const u8 slow = u8(std::min(255.0f, std::max(0.0f, slow_window_fps_ + 0.5f)));
            const bool edge_reset = record_edge_reset_pending_;
            record_edge_reset_pending_ = false;
            if (!recorder.capture_input(input, slow, edge_reset)) fail("Replay input recording failed");
        };
    if (replay_session) {
        int restore_player_first = 1;
        run->playback_input = [this, restore_player = true, restore_player_first]() mutable -> const GameInput* {
            if (replay_finished) return nullptr;

            if (restore_player) {
                std::string error;
                const auto* stage = replay_cursor ? replay_cursor->stage() : nullptr;
                if (!stage || !ReplaySession::restore_player_start(*stage, run->battle, error)) {
                    fail(error.empty() ? "Replay starting stage unavailable" : error);
                    return nullptr;
                }
                restore_player = false;
                restore_player_first = 1;
            }
            const u32 recorded = replay_take_input();
            if (replay_finished) return nullptr;
            replay_input.update(recorded);
            if (::th13::damage_probe_enabled)
                std::printf("PB stage=%d f=%d first=%d rec=%u\n", progress.stage, progress.stage_frame,
                            int(restore_player_first), recorded);
            restore_player_first = 0;
            return &replay_input;
        };
    }
    if (!playfield_targets) {
        playfield_targets = std::make_unique<PlayfieldTargets>(scheduler, animations, renderer, graphics, views);
        if (!playfield_targets->initialize(0)) return fail(playfield_targets->error);
    }
    if (display) display->playfield_active = true;
    pause = std::make_unique<RunPause>(progress, animations, environment, *this, scheduler, kFrontBank);
    if (!pause->prepare()) return fail(pause->error);
    static constexpr const char* kStageMusic[7] = {"th13_01", "th13_04", "th13_07", "th13_10",
                                                   "th13_12", "th13_16", "th13_20"};
    static constexpr const char* kStageBossMusic[7] = {"th13_03", "th13_06", "th13_08", "th13_11",
                                                       "th13_15", "th13_17", "th13_22"};
    if (progress.stage >= 1 && progress.stage <= 7) {
        if (!audio_device.queue_music(1, 0, std::string(kStageMusic[progress.stage - 1]) + ".wav") ||
            !audio_device.queue_music(1, 1, std::string(kStageBossMusic[progress.stage - 1]) + ".wav") ||
            !audio_device.queue_music(2, -1, std::string(kStageMusic[progress.stage - 1]) + ".wav"))
            return fail(audio_device.error);
    }
    if (mp_seat_count < 2 && records_practice_stage(progress, replay_session) &&
        !files.practice_stage_played(progress.character, progress.difficulty, progress.stage)) {
        const auto previous = files.practice_played;
        files.mark_practice_stage_played(progress.character, progress.difficulty, progress.stage);
        if (!files.save()) {
            files.practice_played = previous;
            return fail(files.error);
        }
    }
    if (mp_seat_count < 2 && new_run && !replay_session && progress.practice_mode == 0) {
        if (!files.record_run_start(progress.character) || !files.save())
            return fail(files.error.empty() ? "Invalid play start record" : files.error);
        ordinary_records_dirty = false;
    }
    return true;
}

bool ApplicationState::begin_record_stage() {
    reset_slow_rate();
    record_edge_reset_pending_ = true;
    auto header = ReplayRecorder::stage_snapshot(
        progress, selection_player, selection_score, game_random.seed(), run_player ? run_player->fixed_x : 0,
        run_player ? run_player->fixed_y : 51200, run_player && run_player->focus);
    if (header.size() >= 0x38) {
        const u32 point_items = u32(progress.point_items);
        for (int i = 0; i < 4; ++i)
            header[0x34 + i] = u8(point_items >> (8 * i));
    }
    if (spell_time_pin_enabled_ && header.size() >= 0xC4 && progress.stage >= 1 && progress.stage <= 7) {
        const auto& slot = spell_time_pin_[u32(progress.stage)];
        for (int k = 0; k < 21; ++k)
            for (int b = 0; b < 4; ++b)
                header[size_t(0x70 + k * 4 + b)] = u8(u32(slot[k]) >> (8 * b));
    }
    return recorder.begin_stage(header) || fail("Replay stage recording failed");
}

bool ApplicationState::prepare_next_stage() {
    if (!run) return fail("Stage transition has no run");
    if (progress.high_score < selection_score.score) progress.high_score = selection_score.score;
    stage_transition_loading = false;
    stage_transition_active = true;
    stage_startup_age = -1;
    transition_gate_active = true;
    transition_gate_frames = 0;
    progress.scene_flags |= 4u;
    if (mp_seat_count < 2 && !files.save_score()) return fail(files.error);
    run_hud->prepare_stage_assets();
    if (!transition_prayer) {
        transition_prayer = animations.create(2, 17, -1, 0, {480, 392, 0});
        if (!transition_prayer) return fail("Next-stage loading caption: " + animations.error);
    }
    char name[16];
    std::snprintf(name, sizeof name, "st%02d.std", progress.stage);
    const i32 bank = next_scenery_bank;
    next_scenery_bank = bank == 7 ? 26 : 7;
    for (auto& enemy : run->battle.sim.enemies)
        if (run->battle.sim.retire_sink) run->battle.sim.retire_sink(enemy->serial);
    run->battle.sim.enemies.clear();
    run->battle.sim.bullets.clear();
    run->battle.item_manager.reset_items();
    for (auto& spirit : run->battle.item_manager.spirits())
        spirit = Spirit{};
    run->presentation->release_stage();
    run->transition_wait = true;
    transition_scenery_pending = true;
    transition_scenery_bank = bank;
    transition_scenery_std = name;
    stage_assets.begin(progress.stage, &graphics,
                       [this](const std::string& asset, std::vector<u8>& bytes) { return read(asset, bytes, false); });
    if (!run->presentation->options_hidden() && !run->presentation->hide_player_options())
        return fail(run->presentation->error());
    if (replay_active) {
        const i32 index = ReplaySession::stage_index(replay_file, progress.stage);
        if (index < 0) return fail("Replay next stage missing");
        pending_replay_stage_index = u32(index);
        pending_replay_stage = &replay_file.stages[u32(index)];
        pending_transition_restore = true;
    } else {
    }
    return true;
}

bool ApplicationState::activate_next_stage() {
    if (replay_active) {
        const ReplayStage& stage = replay_file.stages[pending_replay_stage_index];
        replay_cursor = std::make_unique<ReplayInputCursor>(stage);
        replay_consumed = 0;
        replay_input = {};
        replay_finished = false;
    }
    progress.new_run = false;
    if (!begin_run()) return false;
    stage_assets.discard();
    pending_replay_stage = nullptr;
    stage_transition_active = false;
    stage_startup_age = -1;
    transition_gate_active = false;
    transition_gate_frames = 0;
    return true;
}

bool ApplicationState::begin_replay_run(const u8* data, size_t size, u32 stage_index) {
    if (!replay_file.decode(data, size)) return fail("Replay decode: " + replay_file.error());
    if (stage_index >= replay_file.stages.size()) return fail("Replay stage index out of range");
    return begin_replay_stage(stage_index);
}

bool ApplicationState::begin_replay_stage(u32 stage_index, bool stage_transition) {
    if (stage_index >= replay_file.stages.size()) return fail("Replay stage index out of range");
    const ReplayStage& stage = replay_file.stages[stage_index];
    replay_cursor = std::make_unique<ReplayInputCursor>(stage);
    replay_consumed = 0;
    replay_input = {};
    replay_finished = false;
    selected_stage = stage.number > 0 ? stage.number : 1;
    progress.difficulty = replay_file.rank;
    progress.character = replay_file.character & 3;
    const bool spell_replay = replay_file.top_header.size() > 10 && (replay_file.top_header[10] & 2u) != 0;
    progress.run_mode_flags = (progress.run_mode_flags & ~0x30u) | (spell_replay ? 0x20u : 0u);
    progress.practice_mode = 1;
    progress.selected_spell_id = -1;
    if (spell_replay && replay_file.top_header.size() >= 0x74)
        std::memcpy(&progress.selected_spell_id, replay_file.top_header.data() + 0x70, 4);
    selection_player = HudPlayerState{};
    selection_score = HudScoreState{};
    pending_replay_stage = &stage;
    pending_replay_stage_index = stage_index;
    progress.new_run = !stage_transition;
    loading_startup = false;
    loading_debt = 0;
    pending_load = 0;
    pending_destination = -1;
    if (!begin_run()) return false;
    replay_active = true;
    current_destination = i32(Destination::Run);
    return true;
}

u32 ApplicationState::replay_take_input() {
    if (!replay_active || replay_finished || !replay_cursor || !replay_cursor->valid()) return 0;
    const ReplayFrame frame = replay_cursor->next();
    if (frame.eof()) {
        replay_finished = true;
        if (!pause || !pause->open(PauseEntrance::PracticeEnd))
            fail(pause ? pause->error : "Replay end has no overlay");
        return 0;
    }
    ++replay_consumed;
    return frame.state;
}

bool ApplicationState::apply_destination() {
    if (pending_destination < 0) return true;
    const auto next = pending_destination;
    pending_destination = -1;
    if (next == i32(Destination::Run)) {
        current_destination = next;
        if (pending_replay_file >= 0) {
            const i32 file_index = pending_replay_file;
            const i32 stage_index = pending_replay_stage_slot;
            pending_replay_file = pending_replay_stage_slot = -1;
            if (file_index < 0 || file_index >= i32(replay_slots.size()) || !replay_slots[u32(file_index)].decoded)
                return fail("Replay launch slot unavailable: " + std::to_string(file_index));
            const ReplayCatalogSlot& slot = replay_slots[u32(file_index)];
            return begin_replay_run(slot.bytes.data(), slot.bytes.size(), u32(stage_index));
        }
        if (replay_active) return begin_replay_stage(pending_replay_stage_index);
        return begin_run();
    }
    if (next == i32(Destination::NextStage)) {
        current_destination = i32(Destination::Run);
        stage_transition_loading = true;
        return true;
    }
    if (next == i32(Destination::Ending)) {
        if (multiplayer::session.Simulating() && !multiplayer::session.FinishWorldSimulation(true))
            return fail("Unconfirmed multiplayer ending handoff");
        if (mp_seat_count < 2 && !replay_active) {
            if (progress.continues == 0)
                files.set_cleared(progress.character & 3, std::clamp(progress.difficulty, 0, kSaveDifficulties - 1));
            if (progress.difficulty > 0 && progress.continues == 0) files.set_extra_unlocked(progress.character & 3);
        }
        if (mp_seat_count < 2 && !files.save_score()) return fail(files.error);
        ordinary_records_dirty = false;
        current_destination = next;
        release_run();
        ending = std::make_unique<EndingScene>(progress, *this, mp_seat_count >= 2);
        if (!ending->initialize()) return fail(ending->error);
        return initialize_ending_text();
    }
    if (next == i32(Destination::AfterEnding)) {
        if (ending) {
            ending->teardown();
            ending.reset();
        }
        if (mp_seat_count < 2 && !replay_active && !files.save_score()) return fail(files.error);
        return begin_title(3);
    }
    if (next == i32(Destination::Title) || next == i32(Destination::TitleFromGameOver) ||
        next == i32(Destination::TitleFromReplay)) {
        return begin_title(1);
    }
    if (next == i32(Destination::Quit)) {
        return quit();
    }
    return fail("Unrecognized application destination: " + std::to_string(next));
}

bool ApplicationState::quit() {
    if (exiting) return true;
    if (mp_seat_count < 2 && !files.save_score()) return fail(files.error);
    if (!files.advance_pending_writes(~0u)) return fail(files.error);
    ordinary_records_dirty = false;
    exiting = true;
    return true;
}

bool ApplicationState::complete_loading() {
    if (pending_load) {
        const unsigned task = pending_load;
        pending_load = 0;
        if (task == 1) {
            if (!apply_destination()) return false;
        }
    }
    return true;
}

bool ApplicationState::update_scene_pass(const GameInput& input) {
    bool loaded_transition = false;
    if (stage_transition_loading) {
        if (!prepare_next_stage()) return false;
        loaded_transition = true;
    }
    const bool gate_player_update = transition_gate_player_update_runs(environment.paused, progress.scene_flags);
    if (transition_gate_active && stage_transition_active && !loaded_transition &&
        transition_gate_should_interrupt(transition_gate_frames, environment.paused, progress.scene_flags) && run &&
        run->presentation) {
        if (!run->presentation->interrupt_player_options()) return fail(run->presentation->error());
    }
    if (stage_transition_active && !loaded_transition && !environment.paused && !(progress.scene_flags & 0x70u)) {
        if (stage_startup_age < 0) {
            if (!(run->hud.flags & 0x100u) || run->hud.clear_age.current >= 120) {
                if (transition_scenery_pending) {
                    if (!run->prepare_transition(transition_scenery_bank, transition_scenery_std))
                        return fail(run->error);
                    transition_scenery_pending = false;
                }
                if (!run->start_transition()) return false;
                progress.scene_flags = stage_loader_completed_scene_flags(progress.scene_flags);
                stage_startup_age = 0;
            }
        } else if (stage_startup_age == 30) {
            if (!activate_next_stage()) return false;
        }
        if (stage_transition_active && stage_startup_age >= 0) ++stage_startup_age;
    }
    if (transition_gate_active && stage_transition_active && gate_player_update) ++transition_gate_frames;
    if (title) {
        title_flow_ran = true;
        title->controls({input.held, input.pressed, input.repeated, 0});
        if (scheduler.update() < 0)
            return fail(title->error.empty() ? title->frame.error.empty() ? animations.error : title->frame.error
                                             : title->error);
    } else if (run) {
        const i32 previous_stage_frame = progress.stage_frame;
        run->controls(input);
        if (scheduler.update() < 0) return fail(run->error.empty() ? animations.error : run->error);
        if (!replay_active && progress.stage_frame != previous_stage_frame) ++pending_play_frames_;
    } else if (ending) {
        if (scheduler.update() < 0) return fail(ending->error.empty() ? animations.error : ending->error);
    } else {
        return fail("Application has no active scene");
    }
    if (pending_transition_restore) {
        pending_transition_restore = false;
        ReplayRestoreLog log;
        std::string error;
        if (!ReplaySession::restore_stage(replay_file, pending_replay_stage_index,
                                          {&progress, &selection_player, &selection_score, &game_random, &visual}, log,
                                          error))
            return fail(error);
    }
    return true;
}

bool ApplicationState::update(const GameInput& input, bool lost_focus) {
    if (!initialized || exiting || !failure.empty()) return false;
    if (multiplayer::session.Active() && mp_transition_handle_ != run_transition.handle()) {
        if (!run_transition.adopt(mp_transition_handle_))
            return fail(run_transition.error.empty() ? "Run transition adopt failed" : run_transition.error);
    }
    mp_transition_handle_ = run_transition.handle();
    recorder.capture_frame = frames;
    if (!complete_loading()) return false;
    const bool local_pause_allowed = !multiplayer::session.Active();
    const bool deterministic_pause = multiplayer::session.Simulating() || mp_history_playback;
    const bool focus_pause = local_pause_allowed && lost_focus && focus_pause_enabled;
    if (run && pause) {
        if (pause->active())
            pause->controls({input.pressed, input.repeated, focus_pause});
        else if (((local_pause_allowed || deterministic_pause) && (input.pressed & 0x100u)) || focus_pause)
            if (!pause->open(PauseEntrance::Pause)) return fail(pause->error);
    }
    physical_input = &input;
    do {
        restart_update = false;
        replay_pass_start = replay_consumed;
        if (!update_scene_pass(input)) {
            physical_input = nullptr;
            return false;
        }
    } while (restart_update && pending_destination < 0);
    physical_input = nullptr;
    ending_fades.erase(std::remove_if(ending_fades.begin(), ending_fades.end(),
                                      [this](const auto& fade) { return !scheduler.contains(fade->update); }),
                       ending_fades.end());
    return true;
}

bool ApplicationState::commit_pending_replay_records() {
    while (!recorder.pending_inputs.empty()) {
        const PendingReplayRecorder::PendingInput pending = recorder.pending_inputs.front();
        recorder.pending_inputs.erase(recorder.pending_inputs.begin());
        GameInput record = pending.input;
        if (pending.edge_reset) {
            record.pressed = record.held;
            record.released = 0;
        }
        if (!recorder.stream.record(record, pending.slow)) return fail("Replay input recording failed");
    }
    while (!recorder.pending_spell_times.empty()) {
        const PendingReplayRecorder::PendingSpellTime pending = recorder.pending_spell_times.front();
        recorder.pending_spell_times.erase(recorder.pending_spell_times.begin());
        if (!recorder.stream.record_spell_time(pending.index, pending.packed))
            return fail("Replay spell time array unavailable");
    }
    return true;
}

bool ApplicationState::commit_pending_file_events() {
    while (!pending_file_events_.empty()) {
        const PendingFileEvent entry = pending_file_events_.front();
        pending_file_events_.erase(pending_file_events_.begin());
        if (mp_seat_count >= 2 && entry.kind != PendingFileEvent::Kind::PauseOption &&
            entry.kind != PendingFileEvent::Kind::PersistOptions)
            continue;
        switch (entry.kind) {
        case PendingFileEvent::Kind::SpellEvent: {
            bool changed = false;
            if (entry.spell_event.kind == SpellEventKind::Begin)
                changed = files.record_spell_attempt(entry.character, entry.spell_event.state.id,
                                                     entry.spell_event.state.name, entry.spell_practice);
            else
                changed = files.record_spell_capture(entry.character, entry.spell_event.state.id, entry.spell_practice);
            if (!(changed && files.save()))
                return fail(files.error.empty() ? "Invalid spell record event" : files.error);
            break;
        }
        case PendingFileEvent::Kind::SpellPracticeScore:
            if (!(files.record_spell_practice_score(entry.character, entry.spell_id, entry.score) && files.save()))
                return fail(files.error);
            break;
        case PendingFileEvent::Kind::StageClear:
            if (!files.record_run_clear(entry.character, entry.difficulty, entry.no_continue) || !files.save())
                return fail(files.error.empty() ? "Invalid clear record" : files.error);
            ordinary_records_dirty = false;
            break;
        case PendingFileEvent::Kind::PauseOption: {
            i32& value = entry.row == 0 ? files.bgm_volume : files.effect_volume;
            value = std::max(0, std::min(100, value + entry.delta));
            apply_volumes(files.bgm_volume, files.effect_volume);
            if (!files.save()) return fail(files.error);
            break;
        }
        case PendingFileEvent::Kind::ResultEntry:
            files.record_high_score(entry.character, entry.difficulty, entry.result);
            files.remember_name(entry.result.name);
            if (!files.save_score()) return fail(files.error);
            break;
        case PendingFileEvent::Kind::MusicUnlocked:
            files.set_music_heard(entry.track);
            if (!files.save()) return fail(files.error);
            break;
        case PendingFileEvent::Kind::MarkHeard:
            files.set_music_heard(entry.track);
            (void)files.save();
            break;
        case PendingFileEvent::Kind::PersistOptions: {
            files.bgm_volume = entry.bgm;
            files.effect_volume = entry.effect;
            if (!files.save() && title) {
                TitleOptionsMenu& menu = title->option_menu();
                if (menu.error.empty()) menu.error = "Title option persistence unavailable";
            }
            break;
        }
        }
    }
    return true;
}

void ApplicationState::wire_multiplayer_audio() {
    if (mp_audio_wired_ || mp_seat_count < 2 || !multiplayer::session.Active()) return;
    mp_audio_wired_ = true;
    audio_device.defer_output = true;
    audio_debug_observer_ = audio_device.request_observer;
    audio_device.request_observer = [this](const AudioDevice::RequestObservation& event) {
        if (multiplayer::session.RecordingAudio())
            multiplayer::session.ObserveAudio(event);
        else
            audio_device.apply_request(event);
        if (audio_debug_observer_) audio_debug_observer_(event);
    };
}

bool ApplicationState::commit_pending_multiplayer_audio() {
    if (!audio_device.defer_output) return true;
    auto& session = multiplayer::session;
    const auto apply = [this, &session](const std::vector<multiplayer::Session::AudioRequest>& requests) {
        std::size_t applied = 0;
        for (; applied < requests.size(); ++applied) {
            const auto& request = requests[applied];
            if (!audio_device.apply_request(request.observation)) break;
            session.NoteApplied(request.frame, request.observation);
        }
        return applied;
    };
    const std::size_t deferred = apply(session.AudioOut());
    session.ConsumeAudioOut(deferred);
    if (!audio_device.error.empty()) return false;
    const std::size_t voices = apply(session.VoiceOut());
    session.ConsumeVoiceOut(voices);
    return audio_device.error.empty();
}

void ApplicationState::commit_pending_voice_pans() {
    while (!pending_voice_pans_.empty()) {
        const PendingVoicePan pending = pending_voice_pans_.front();
        pending_voice_pans_.erase(pending_voice_pans_.begin());
        audio_device.apply_voice_pan(pending.id, pending.pan);
    }
}

bool ApplicationState::commit_pending_play_frames() {
    if (pending_play_frames_ == 0) return true;
    const u32 count = pending_play_frames_;
    pending_play_frames_ = 0;
    if (mp_seat_count >= 2) return true;
    for (u32 i = 0; i < count; ++i)
        if (!files.record_play_frame(progress.character)) return fail("Invalid play time record");
    ordinary_records_dirty = true;
    return true;
}

bool ApplicationState::capture_multiplayer_host_state(multiplayer::HostState& state) {
    auto& fields = state.state_fields;
    fields.frames = frames;
    fields.pending_destination = pending_destination;
    fields.pending_load = pending_load;
    fields.record_edge_reset_pending = record_edge_reset_pending_;
    fields.spell_time_stage = spell_time_stage;
    fields.spell_clock_active = spell_time_clock.active();
    fields.spell_clock_index = spell_time_clock.index();
    fields.stage_transition_loading = stage_transition_loading;
    fields.stage_transition_active = stage_transition_active;
    fields.stage_startup_age = stage_startup_age;
    fields.transition_gate_active = transition_gate_active;
    fields.transition_gate_frames = transition_gate_frames;
    fields.selection_player = selection_player;
    fields.game_over_music = game_over_music;
    fields.loading_environment = loading_environment;
    fields.loading_animation_rate = loading_animations.rate;
    fields.loading_signature = loading_signature;
    fields.loading_prayer = loading_prayer;
    fields.loading_registry = std::make_shared<AnmRegistry::SnapshotData>();
    if (!loading_animations.registry.capture(*fields.loading_registry)) return fail(loading_animations.registry.error);
    fields.has_pause = pause != nullptr;
    fields.pause = pause ? pause->state : PauseState{};
    fields.pause_frame = pause ? pause->frame.capture_rollback() : PauseFrame::RollbackState{};
    fields.fades.clear();
    for (const auto& fade : ending_fades)
        fields.fades.push_back({fade->kind, fade->duration, fade->time, fade->alpha, fade->playfield_only});
    auto& local = state.local_fields;
    local.spell_time_clock = spell_time_clock;
    local.spell_time_packed = spell_time_packed;
    local.game_over_music_cursor = game_over_music_cursor;
    local.last_fps = last_fps_;
    local.slow_window_time = slow_window_time_;
    local.slow_window_frames = slow_window_frames_;
    local.slow_window_fps = slow_window_fps_;
    local.slow_actual = slow_actual_;
    local.slow_budget = slow_budget_;
    local.slow_num = slow_num_;
    local.slow_den = slow_den_;
    state.dialogue_digest = digest_dialogue_rollback();
    return true;
}

u64 ApplicationState::digest_multiplayer_host() {
    multiplayer::HostState state;
    return capture_multiplayer_host_state(state) ? multiplayer::WorldState::digest_host(state) : 0;
}

std::function<bool()> ApplicationState::capture_multiplayer_host() {
    multiplayer::HostState state;
    if (!capture_multiplayer_host_state(state)) return [] { return false; };
    const auto restore_dialogue = capture_dialogue_rollback();
    return [this, state = std::move(state), restore_dialogue] {
        const auto& fields = state.state_fields;
        frames = fields.frames;
        pending_destination = fields.pending_destination;
        pending_load = fields.pending_load;
        record_edge_reset_pending_ = fields.record_edge_reset_pending;
        spell_time_stage = fields.spell_time_stage;
        stage_transition_loading = fields.stage_transition_loading;
        stage_transition_active = fields.stage_transition_active;
        stage_startup_age = fields.stage_startup_age;
        transition_gate_active = fields.transition_gate_active;
        transition_gate_frames = fields.transition_gate_frames;
        selection_player = fields.selection_player;
        game_over_music = fields.game_over_music;
        loading_environment = fields.loading_environment;
        loading_animations.rate = fields.loading_animation_rate;
        loading_signature = fields.loading_signature;
        loading_prayer = fields.loading_prayer;
        const auto& local = state.local_fields;
        spell_time_clock = local.spell_time_clock;
        spell_time_packed = local.spell_time_packed;
        game_over_music_cursor = local.game_over_music_cursor;
        last_fps_ = local.last_fps;
        slow_window_time_ = local.slow_window_time;
        slow_window_frames_ = local.slow_window_frames;
        slow_window_fps_ = local.slow_window_fps;
        slow_actual_ = local.slow_actual;
        slow_budget_ = local.slow_budget;
        slow_num_ = local.slow_num;
        slow_den_ = local.slow_den;
        if (!loading_animations.registry.restore(*fields.loading_registry))
            return fail(loading_animations.registry.error);
        if (pause) {
            pause->state = fields.pause;
            pause->frame.restore_rollback(fields.pause_frame);
        }
        if (!restore_dialogue()) return false;
        clear_ending_windows();
        for (const auto& fade : fields.fades) {
            if (!open_game_fade(fade.kind == 0, fade.duration, fade.playfield_only)) return false;
            ending_fades.back()->time = fade.time;
            ending_fades.back()->alpha = fade.alpha;
        }
        sync_mp_prediction_barrier(*this);
        return true;
    };
}

void ApplicationState::queue_multiplayer_outputs() {
    auto inputs = std::move(recorder.pending_inputs);
    auto spells = std::move(recorder.pending_spell_times);
    auto events = std::move(pending_file_events_);
    auto pans = std::move(pending_voice_pans_);
    auto play_frames = pending_play_frames_;
    recorder.pending_inputs.clear();
    recorder.pending_spell_times.clear();
    pending_file_events_.clear();
    pending_voice_pans_.clear();
    pending_play_frames_ = 0;
    multiplayer::session.SetConfirmedOutput([this, inputs = std::move(inputs), spells = std::move(spells),
                                             events = std::move(events), pans = std::move(pans),
                                             play_frames]() mutable {
        recorder.pending_inputs.swap(inputs);
        recorder.pending_spell_times.swap(spells);
        pending_file_events_.swap(events);
        pending_voice_pans_.swap(pans);
        std::swap(pending_play_frames_, play_frames);
        const bool ok = commit_pending_replay_records() && commit_pending_file_events();
        if (ok) commit_pending_voice_pans();
        const bool committed = ok && commit_pending_play_frames();
        recorder.pending_inputs.swap(inputs);
        recorder.pending_spell_times.swap(spells);
        pending_file_events_.swap(events);
        pending_voice_pans_.swap(pans);
        std::swap(pending_play_frames_, play_frames);
        return committed;
    });
}

bool ApplicationState::commit(float presentation_fps) {
    if (multiplayer::session.RecordingAudio()) {
        queue_multiplayer_outputs();
    } else {
        if (!commit_pending_replay_records()) return false;
        if (!commit_pending_file_events()) return false;
        commit_pending_voice_pans();
        if (!commit_pending_play_frames()) return false;
    }
    if (!commit_pending_multiplayer_audio()) return fail(audio_device.error);
    if (stage_transition_active && !stage_assets.done()) stage_assets.step();
    if (!files.advance_pending_writes()) return fail(files.error);
    audio_device.update();
    if (!audio_device.error.empty()) return fail(audio_device.error);
    frames++;
    if (run) {
        last_fps_ = presentation_fps;
        if (presentation_fps > 0.0f) slow_window_time_ += 1.0 / presentation_fps;
        ++slow_window_frames_;
        if (slow_window_time_ >= 1.0) {
            const float window_fps = float(double(slow_window_frames_) / slow_window_time_);
            slow_window_fps_ = window_fps;
            if (!environment.paused && !(progress.scene_flags & 0x14u) && !loading_waiting()) {
                slow_budget_ += 60.0;
                slow_actual_ += window_fps <= 57.0f ? double(window_fps) : 60.0;
                slow_num_ += window_fps <= 57.0f ? double(window_fps) : 60.0;
                slow_den_ += 60.0;
            }
            slow_window_time_ = 0.0;
            slow_window_frames_ = 0;
        }
    }
    sync_mp_prediction_barrier(*this);
    if (!commit_loading_battle_caption()) return false;
    if (run && run->presentation)
        run->presentation->prepare_player_opacity();
    else
        renderer.player_opacity.fill(255);
    if (run && run->battle.seat_count >= 2) {
        const i32 seat = run->battle.mp_local_seat;
        if (seat >= 0 && seat < run->battle.seat_count) {
            const auto& state = run->battle.seats[size_t(seat)];
            eagler::presentation::local_player_cue(unsigned(seat), state.x + 320.f, state.y + 16.f);
        }
    }
    if (ending && (!graphics.select_target(nullptr, 0) || !graphics.clear_target(0xff000000, nullptr)))
        return fail(graphics.error);
    if (pause && !pause->draw_overlay(static_cast<RunSceneServices&>(*this))) return fail(pause->error);
    return display->prepare() || fail(display->error);
}

bool ApplicationState::present() {
    if (!display->present()) return fail(display->error);
    if (multiplayer::session.Simulating() && pause &&
        (pause->state.screen == PauseScreen::ExtraEnd || pause->state.screen == PauseScreen::GameOver ||
         pause->state.screen == PauseScreen::BGameOver) &&
        multiplayer::session.AllFramesCommitted()) {
        multiplayer::session.FinishWorldSimulation(pause->state.screen == PauseScreen::ExtraEnd);
        if (run) run->playback_input = {};
    }
    if (pending_destination >= 0 && multiplayer::session.Simulating() && !multiplayer::session.AllFramesCommitted())
        return true;
    if (pending_destination == i32(Destination::Run) || pending_destination == i32(Destination::Ending)) {
        if (!draw_loading(false)) return false;
        pending_load = 1;
        return true;
    }
    const bool applied = apply_destination();
    sync_mp_prediction_barrier(*this);
    return applied && (graphics.error.empty() || fail(graphics.error));
}

bool ApplicationState::step(const GameInput& input, float fps, bool lost_focus) {
    return update(input, lost_focus) && commit(fps) && present();
}

const i32* ApplicationState::projection() {
    projected.fill(0);
    projected[0] = title ? 0 : run ? 1 : ending ? 2 : -1;
    projected[1] = current_destination;
    projected[2] = progress.stage;
    projected[3] = progress.difficulty;
    projected[4] = progress.character;
    projected[5] = progress.stage_frame;
    projected[6] = i32(progress.scene_flags);
    projected[7] = i32(frames);
    if (title) {
        projected[8] = i32(title->state.screen);
        projected[9] = title->state.substate;
        projected[10] = title->state.menu.cursor;
        projected[11] = title->state.menu.count;
        projected[12] = i32(selection_player.mode_flags);
    } else if (run) {
        projected[8] = selection_score.score;
        projected[9] = selection_player.extra_lives;
        projected[10] = selection_player.bombs;
        projected[11] = selection_player.power;
        projected[12] = i32(selection_player.mode_flags);
        projected[13] = selection_player.deaths;
        projected[14] = pause ? i32(pause->state.screen) : 0;
    } else if (ending) {
        projected[8] = ending->ending_index();
        projected[9] = ending->line();
        projected[10] = ending->frames();
        projected[11] = i32(ending->scene_flags());
    }
    projected[15] = fonts_failure.empty() ? 0 : 1;
    return projected.data();
}

EndingSaveBytes* ApplicationState::save_bytes() {
    ending_save_view.bytes = mp_seat_count >= 2 ? nullptr : &files.ending_seen_bytes;
    return &ending_save_view;
}

bool ApplicationState::fade_out_music(i32 frames) {
    return audio_device.queue_music(5, frames / 60, "") || fail(audio_device.error);
}

bool ApplicationState::record_music_unlocked(i32 index) {
    if (index < 0 || index >= 32) return fail("Ending music index out of range");
    PendingFileEvent entry;
    entry.kind = PendingFileEvent::Kind::MusicUnlocked;
    entry.capture_frame = recorder.capture_frame;
    entry.track = index;
    pending_file_events_.push_back(entry);
    return true;
}

bool ApplicationState::advance_sound() { return sound(0); }

bool ApplicationState::load_ending_anm(i32 slot, const std::string& name) {
    std::vector<u8> bytes;
    if (!read(name, bytes)) return false;
    if (animations.resource(slot)) animations.unload(slot);
    if (!animations.load(slot, bytes.data(), u32(bytes.size())))
        return fail("Ending ANM load: " + name + ": " + animations.error);
    animations.name_resource(slot, name);
    if (!graphics.preload(*animations.resource(slot)))
        return fail("Ending ANM preload: " + name + ": " + graphics.error);
    return true;
}

bool ApplicationState::cue_ending_anm(i32 vm_slot, i32 bank_slot, i32 script) {
    if (vm_slot < 0 || vm_slot >= i32(ending_anm_vms.size())) return fail("Ending ANM VM slot out of range");
    u32& vm = ending_anm_vms[u32(vm_slot)];
    if (!animations.retire(vm)) return fail(animations.error);
    vm = animations.create(bank_slot, script, -1, 0);
    if (!vm) return fail("Ending ANM cue failed: " + animations.error);
    return true;
}

void ApplicationState::release_ending_anm(i32 slot) {
    animations.unload(slot);
    if (slot == 0x17) {
        for (u32& vm : ending_anm_vms)
            animations.retire(vm);
    }
}

bool ApplicationState::initialize_ending_text() {
    for (u32 row = 0; row < ending_text_vms.size(); ++row) {
        if (animations.registry.find(ending_text_vms[row])) continue;
        ending_text_vms[row] = animations.create(0, i32(68 + row), -1, 0);
        if (!ending_text_vms[row]) {
            release_ending_page();
            return fail("Ending text VM creation: " + animations.error);
        }
        AnmVm* vm = animations.registry.find(ending_text_vms[row]);
        if (!vm) {
            release_ending_page();
            return fail("Ending text VM unavailable");
        }
        vm->visual.render_flags |= 0x1000;
    }
    return true;
}

void ApplicationState::release_ending_page() {
    for (u32& vm : ending_text_vms)
        animations.retire(vm);
}

bool ApplicationState::ending_page_reset() {
    if (!initialize_ending_text()) return false;
    for (u32 row = 0; row < ending_text_vms.size(); ++row)
        if (!ending_page_line(i32(row), 0x00ffffff, " ")) return false;
    return ending_page_clear();
}

bool ApplicationState::ending_page_line(i32 row, u32 color, const std::string& cp932) {
    if (row < 0 || row >= i32(ending_text_vms.size())) return fail("Ending text row out of range");
    AnmVm* vm = animations.registry.find(ending_text_vms[u32(row)]);
    if (!vm || !vm->resource || !vm->sprite) return fail("Ending text VM unavailable");
    const AnmSprite& sprite = *vm->sprite;
    const u32 handle = graphics.texture(*vm->resource, sprite.texture);
    TextureImage* target = handle ? graphics.pixels(handle) : nullptr;
    if (!target || target->format != touhou::graphics::PixelFormat::Argb4444 || target->pitch < target->width * 2 ||
        target->pixels.size() < size_t(target->pitch) * target->height || sprite.x < 0 || sprite.y < 0 ||
        sprite.width != 512 || sprite.height != 20 || sprite.x + sprite.width > float(target->width) ||
        sprite.y + sprite.height > float(target->height))
        return fail("Ending text texture unavailable");
    TextureImage scratch;
    scratch.width = 1024;
    scratch.height = 40;
    scratch.pitch = 2048;
    scratch.packed_format = 5;
    scratch.format = touhou::graphics::PixelFormat::Argb4444;
    scratch.pixels.assign(size_t(scratch.pitch) * scratch.height, 0);
    if (!fonts.draw_text(scratch, 0, 2, 0, color, cp932)) return fail(fonts.error);
    const u32 x = u32(sprite.x), y = u32(sprite.y);
    for (u32 py = 0; py < 20; ++py) {
        u8* destination = target->pixels.data() + size_t(y + py) * target->pitch + size_t(x) * 2;
        const float center_y = (float(py) + .5f) * 1.8f - .5f;
        for (u32 px = 0; px < 512; ++px) {
            const float center_x = (float(px) + .5f) * 2.f - .5f;
            float sum[4] = {}, total = 0;
            for (i32 sy = i32(std::ceil(center_y - 1.8f)); sy <= i32(std::floor(center_y + 1.8f)); ++sy) {
                const float wy = std::max(0.f, 1.f - std::fabs(float(sy) - center_y) / 1.8f);
                for (i32 sx = i32(std::ceil(center_x - 2.f)); sx <= i32(std::floor(center_x + 2.f)); ++sx) {
                    const float weight = wy * std::max(0.f, 1.f - std::fabs(float(sx) - center_x) / 2.f);
                    total += weight;
                    if (sx < 0 || sx >= 1024 || sy < 0 || sy >= 36) continue;
                    u16 pixel = 0;
                    std::memcpy(&pixel, scratch.pixels.data() + size_t(sy) * scratch.pitch + size_t(sx) * 2, 2);
                    for (u32 c = 0; c < 4; ++c)
                        sum[c] += float((pixel >> (12 - c * 4)) & 15u) * weight;
                }
            }
            u16 pixel = 0;
            for (float component : sum)
                pixel = u16((pixel << 4) | u16(std::clamp(i32(std::floor(component / total + .5f)), 0, 15)));
            std::memcpy(destination + px * 2, &pixel, 2);
        }
    }
    graphics.changed(handle, {x, y, x + 512, y + 20});
    return animations.interrupt(ending_text_vms[u32(row)], 2, false) || fail(animations.error);
}

bool ApplicationState::ending_page_clear() {
    for (u32 vm : ending_text_vms)
        if (!animations.interrupt(vm, 3, false)) return fail(animations.error);
    return true;
}

bool ApplicationState::request_stage6_fade() { return open_game_fade(false, 200, false); }

bool ApplicationState::open_game_fade(bool entering, i32 duration, bool playfield_only) {
    if (!open_ending_window(entering ? 0 : 5, duration)) return false;
    auto& fade = *ending_fades.back();
    fade.playfield_only = playfield_only;
    if (playfield_only) {
        const auto draw_function = fade.draw.run;
        scheduler.remove(fade.draw);
        fade.draw.run = draw_function;
        if (scheduler.add(fade.draw, FramePass::Draw, 11) < 0) return fail("Game fade registration failed");
    }
    return true;
}

bool ApplicationState::open_ending_window(i32 kind, i32 duration) {
    if ((kind != 0 && kind != 5) || duration < 0) return fail("Ending fade parameters invalid");
    auto fade = std::make_unique<EndingFade>();
    fade->owner = this;
    fade->kind = kind;
    fade->duration = duration;
    fade->alpha = kind == 0 ? 255 : 0;
    auto* item = fade.get();
    item->update.owner = item;
    item->update.enabled = true;
    item->update.run = [](void* owner) -> i32 {
        auto& fade = *static_cast<EndingFade*>(owner);
        if (fade.owner->environment.paused) return i32(FrameAction::Continue);
        if (fade.kind == 5) {
            if (fade.duration)
                fade.alpha = fade.time >= fade.duration ? 255 : i32(double(fade.time) * 255.0 / double(fade.duration));
            if (fade.time >= fade.duration + 2) {
                fade.owner->scheduler.remove(fade.draw);
                return i32(FrameAction::Remove);
            }
        } else {
            if (fade.duration) fade.alpha = std::max(0, i32(255.0 - double(fade.time) * 255.0 / double(fade.duration)));
            if (fade.time >= fade.duration) {
                fade.owner->scheduler.remove(fade.draw);
                return i32(FrameAction::Remove);
            }
        }
        if (!stage_loading_holds_game_fade(fade.kind, fade.owner->progress.scene_flags)) ++fade.time;
        return i32(FrameAction::Continue);
    };
    item->draw.owner = item;
    item->draw.enabled = true;
    item->draw.run = [](void* owner) -> i32 {
        auto& fade = *static_cast<EndingFade*>(owner);
        auto& app = *fade.owner;
        app.renderer.invalidate();
        app.renderer.set_viewport({0, 0, 640, 480, 0, 1});
        auto& pipeline = app.graphics.pipeline();
        pipeline.blend = true;
        pipeline.sourceBlend = touhou::graphics::BlendFactor::SourceAlpha;
        pipeline.destinationBlend = touhou::graphics::BlendFactor::InverseSourceAlpha;
        pipeline.blendEquation = touhou::graphics::BlendEquation::Add;
        pipeline.depthWrite = false;
        pipeline.color.operation = pipeline.alpha.operation = touhou::graphics::ColorOperation::First;
        pipeline.color.first = pipeline.alpha.first = {touhou::graphics::ArgumentSource::Diffuse};
        app.graphics.set_layout(touhou::graphics::VertexLayout::ScreenColor);
        struct Vertex {
            Vec3 position;
            float reciprocal_w;
            u32 color;
        };
        const u32 color = u32(fade.alpha) << 24;
        const float x0 = fade.playfield_only ? 128.f : 0.f;
        const float x1 = fade.playfield_only ? 512.f : 640.f;
        const float y0 = fade.playfield_only ? 16.f : 0.f;
        const float y1 = fade.playfield_only ? 464.f : 480.f;
        const Vertex quad[] = {
            {{x0, y0, 0}, 1, color}, {{x1, y0, 0}, 1, color}, {{x0, y1, 0}, 1, color}, {{x1, y1, 0}, 1, color}};
        app.graphics.primitives(touhou::graphics::Topology::Strip, 2, quad, sizeof(Vertex));
        app.graphics.flush();
        pipeline.color.operation = pipeline.alpha.operation = touhou::graphics::ColorOperation::Multiply;
        pipeline.color.first = pipeline.alpha.first = {touhou::graphics::ArgumentSource::Texture};
        pipeline.color.second = pipeline.alpha.second = {touhou::graphics::ArgumentSource::Diffuse};
        return i32(FrameAction::Continue);
    };
    if (scheduler.add(item->update, FramePass::Update, 16) < 0 || scheduler.add(item->draw, FramePass::Draw, 75) < 0) {
        scheduler.remove(item->update);
        scheduler.remove(item->draw);
        return fail("Ending fade registration failed");
    }
    ending_fades.push_back(std::move(fade));
    return true;
}

void ApplicationState::clear_ending_windows() {
    for (auto& fade : ending_fades) {
        scheduler.remove(fade->update);
        scheduler.remove(fade->draw);
    }
    ending_fades.clear();
}

}
