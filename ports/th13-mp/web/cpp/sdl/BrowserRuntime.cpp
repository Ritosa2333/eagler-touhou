#include "../../../portable/input/TouchController.hpp"
#include "../../../portable/sdl/FrameCadence.hpp"
#include "../../../portable/sdl/FrameRateMeter.hpp"
#include "../game/GameInput.hpp"
#include "../game/GameKeyboard.hpp"
#include "../game/ReplayEntityState.hpp"
#include "../multiplayer/FrameSchedule.hpp"
#include "../multiplayer/HostState.hpp"
#include "../multiplayer/ReplayArchive.hpp"
#include "../multiplayer/Session.hpp"
#include "../multiplayer/SpectatorJournal.hpp"
#include "../multiplayer/StageEntry.hpp"
#include "../multiplayer/TerminalHandoff.hpp"
#include "../multiplayer/WorldState.hpp"
#include "ApplicationState.hpp"
#include <eagler/input/BrowserGamepad.hpp>
#include <eagler/replay/Workspace.hpp>
#include <limits>
#include <locale>
#include <sstream>

#include <GLES3/gl3.h>
#include <SDL3/SDL.h>
#include <deque>
#include <emscripten.h>
#include <emscripten/html5.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <memory>
#include <vector>

namespace th13 {
using AnmRandomObserver = void (*)(const AnmVm&, i32, i32, i32, u32, u32, u32, u32, bool);
extern AnmRandomObserver anm_random_observer;
extern const AnmVm* anm_tick_current_vm;
extern void (*anm_tick_observer)(const AnmVm&, float);
}

// clang-format off
EM_JS(void, th13_browser_frame, (int ok, double milliseconds, unsigned ticks), {
    Module["onGameFrame"]?.(ok, milliseconds, ticks);
});
// clang-format on
// clang-format off
EM_JS(void, th13_browser_work_frame,
      (double sequence, double begin, double end, int stage, int end_stage,
       int phase, unsigned source, unsigned simulated, unsigned rollback,
       unsigned restores, unsigned presentations, double presentation_at, int ok), {
    const callback = Module["onGameWorkFrame"];
    if (!callback) return;
    try {
        callback({sequence, begin, end, milliseconds: end - begin, stage,
                  endStage: end_stage, phase: ["active", "menu", "loading", "suspended", "paused"][phase],
                  source, simulated, rollback, restores, presentations,
                  presentationAt: presentation_at < 0 ? null : presentation_at, ok: !!ok});
    } catch (error) { console.error("onGameWorkFrame callback failed", error); }
});
// clang-format on

// clang-format off
EM_JS(void, th13_browser_replay_request, (), {
    Module["onReplayImportRequest"]?.();
});
// clang-format on

namespace th13::sdl {
namespace {
std::unique_ptr<ApplicationState> app;
double work_sequence = 0;
struct LoadingWork {
    double begin = emscripten_get_now(), sequence = ++work_sequence;
    ApplicationState* before = app.get();
    int stage = app ? app->progress.stage : 0;
    unsigned presentations = app ? app->graphics.backend.stats.presentations : 0;
    unsigned source, retired_presentations = 0;
    void replacing_app() {
        if (app) {
            const unsigned now = app->graphics.backend.stats.presentations;
            retired_presentations += before == app.get() && now >= presentations ? now - presentations : now;
        }
        before = nullptr;
        presentations = 0;
    }
    explicit LoadingWork(unsigned value) : source(value) {}
    int finish(int result, bool ok) const {
        const int end_stage = app ? app->progress.stage : stage;
        const unsigned after = app ? app->graphics.backend.stats.presentations : 0;
        const unsigned submitted =
            retired_presentations +
            (app && app.get() == before && after >= presentations ? after - presentations : after);
        const double end = emscripten_get_now();
        th13_browser_work_frame(sequence, begin, end, stage, end_stage, 2, source, 0, 0, 0, submitted, -1, ok);
        return result;
    }
};
std::string last_error;
std::string audio_observations;
unsigned audio_sequence = 0;
std::string anm_random_observations;
int anm_random_first = 0, anm_random_last = -1;
std::unique_ptr<th13::multiplayer::ReplayArchive> mp_replay_loaded;
bool mp_playback_pending = false;
bool mp_started = false;
bool mp_room_preview = false;
bool mp_spectator_mode = false;
std::uint32_t mp_spectator_stage = 0;
std::uint32_t mp_playback_frame = 0;
std::uint32_t mp_playback_chapter = 0;
bool mp_playback_paused = false;
std::uint32_t mp_playback_seek_target = Netplay::INVALID_FRAME;
unsigned mp_playback_rate = 1;
void observe_anm_random(const AnmVm& vm, i32 opcode, i32 variable, i32 argument, u32 before, u32 seed_before, u32 after,
                        u32 seed_after, bool game_stream) {
    if (!app || !app->run) return;
    const int frame = app->progress.stage_frame - 1;
    if (frame < anm_random_first || frame > anm_random_last) return;
    anm_random_observations +=
        "{\"frame\":" + std::to_string(frame) +
        ",\"bank\":" + std::to_string(app->animations.resource_id(vm.resource)) +
        ",\"script\":" + std::to_string(vm.source_script) +
        ",\"handle\":" + std::to_string(app->animations.registry.handle(vm)) +
        ",\"pc\":" + std::to_string(vm.instruction_offset) + ",\"opcode\":" + std::to_string(opcode) +
        ",\"variable\":" + std::to_string(variable) + ",\"argument\":" + std::to_string(argument) +
        ",\"consumed\":" + std::to_string(app->replay_consumed) +
        ",\"stage_frame\":" + std::to_string(app->progress.stage_frame) +
        ",\"sim_frame\":" + std::to_string(app->run ? app->run->battle.sim.frame : -1) +
        ",\"vm_age\":" + std::to_string(vm.age.current) + ",\"vm_timer\":" + std::to_string(vm.timer.current) +
        ",\"vm\":" + std::to_string(reinterpret_cast<uintptr_t>(&vm)) + ",\"stream\":\"" +
        (game_stream ? "game" : "visual") + "\"" + ",\"before\":" + std::to_string(before) +
        ",\"seed_before\":" + std::to_string(seed_before) + ",\"after\":" + std::to_string(after) +
        ",\"seed_after\":" + std::to_string(seed_after) + "}\n";
}
struct Key {
    const char* code;
    const char* sdl;
    u32 scan, vk;
    bool hosted = false;
    SDL_Scancode native = SDL_SCANCODE_UNKNOWN;
    bool managed = false;
    std::deque<bool> pending;
};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#include "../../../portable/input/KeyboardMap.inc"
#pragma GCC diagnostic pop

GameInput controls;
touhou::input::TouchController gestures;

u32 timing_float_bits(float value) {
    u32 bits;
    std::memcpy(&bits, &value, sizeof bits);
    return bits;
}

void trace_applied_touch() {
    auto& session = multiplayer::session;
    if (!app || !app->run || !session.Simulating()) return;
    const auto& touch = app->run->battle.seats[session.LocalSeat()].touch;
    session.TraceTiming("apply-touch", session.SimFrame(), touch.mode);
    session.TraceTiming("apply-touch-xy", session.SimFrame(), timing_float_bits(touch.x), timing_float_bits(touch.y));
}

touhou::input::TouchState touch_state() {
    touhou::input::TouchState state;
    if (!app || !app->run || (app->pause && app->pause->active())) return state;
    if (app->replay_active) {
        state.context = 3;
        return state;
    }
    const auto& battle = app->run->battle;
    state.context = app->run->dialog_active() ? 2 : 1;
    state.instance = app->progress.stage;
    const auto& player = battle.seat_count >= 2 ? battle.seats[size_t(battle.mp_local_seat)] : battle.player_state();
    const bool available = !player.dead && !player.ghost;
    state.ready = available && player.phase == StagePlayerState::Phase::Active;
    state.retain_motion = available && player.phase == StagePlayerState::Phase::Deathbomb;
    state.x = player.x;
    state.y = player.y;
    state.fast = float(player.speed_orth) / 128;
    state.slow = float(player.speed_orth_focus) / 128;
    state.min_x = -184;
    state.max_x = 184;
    state.min_y = 32;
    state.max_y = 432;
    return state;
}
SDL_Joystick* controller = nullptr;
touhou::sdl::FrameCadence cadence;
touhou::sdl::FrameRateMeter frame_meter;
bool frame_meter_live = false;
bool running = false, suspended = false, lost_focus = false;
u32 render_scale = 1, loop_epoch = 0;
double previous_frame = -1;
double presentation_window_start = -1;
u32 presentation_count = 0;
float presentation_fps = 60;
constexpr double kDisplayStaleMs = 250.;
double last_display_callback_ms = -1;

void reset_presentation_window() {
    presentation_window_start = -1;
    presentation_count = 0;
    presentation_fps = 60;
    last_display_callback_ms = -1;
    frame_meter.reset();
}

void reset_cadence() {
    cadence.reset();
    cadence.debt = touhou::sdl::FrameCadence::interval / 2;
    frame_meter.restart();
}

void sample_presentation_rate(double now_ms) {
    if (presentation_window_start < 0) {
        presentation_window_start = now_ms;
        return;
    }
    const double elapsed_ms = now_ms - presentation_window_start;
    if (elapsed_ms + 1.e-6 < 1000.) return;
    presentation_fps = float(1000. * presentation_count / elapsed_ms);
    presentation_window_start = now_ms;
    presentation_count = 0;
}

void add_controller(SDL_JoystickID id) {
    if (controller) return;
    controller = SDL_OpenJoystick(id);
}

void initialize_controller() {
    if (controller) {
        SDL_CloseJoystick(controller);
        controller = nullptr;
    }
    int count = 0;
    auto* ids = SDL_GetJoysticks(&count);
    if (ids) {
        for (int i = 0; i < count; ++i)
            add_controller(ids[i]);
        SDL_free(ids);
    }
}

u32 sample_controller(u32& raw_buttons) {
    raw_buttons = 0;
    if (!controller || !app) return 0;
    const int num_buttons = std::min(32, SDL_GetNumJoystickButtons(controller));
    for (int i = 0; i < num_buttons; ++i) {
        if (SDL_GetJoystickButton(controller, i)) raw_buttons |= 1u << i;
    }
    u32 keys = 0;
    constexpr u32 actions[5] = {0x1, 0x2, 0x800, 0x8, 0x100};
    const auto& bindings = app->pad_bindings();
    for (size_t i = 0; i < 5; ++i) {
        const i16 btn = bindings[i];
        if (btn >= 0 && btn < 32 && (raw_buttons & (1u << btn))) keys |= actions[i];
    }
    if (SDL_GetNumJoystickAxes(controller) > 0) {
        const float x = float(SDL_GetJoystickAxis(controller, 0)) / 32768.f;
        if (x < -0.5f)
            keys |= 0x40;
        else if (x > 0.5f)
            keys |= 0x80;
    }
    if (SDL_GetNumJoystickAxes(controller) > 1) {
        const float y = float(SDL_GetJoystickAxis(controller, 1)) / 32768.f;
        if (y < -0.5f)
            keys |= 0x10;
        else if (y > 0.5f)
            keys |= 0x20;
    }
    if (SDL_GetNumJoystickHats(controller) > 0) {
        const auto hat = SDL_GetJoystickHat(controller, 0);
        if (hat & SDL_HAT_UP) keys |= 0x10;
        if (hat & SDL_HAT_DOWN) keys |= 0x20;
        if (hat & SDL_HAT_LEFT) keys |= 0x40;
        if (hat & SDL_HAT_RIGHT) keys |= 0x80;
    }
    return keys;
}

void clear_inputs() {
    if (multiplayer::session.Simulating())
        multiplayer::session.TraceTiming("device-clear", multiplayer::session.CaptureFrame());
    gestures.cancel();
    if (app && app->run) app->run->battle.touch = {};
    for (auto& key : keyboard_map) {
        key.hosted = false;
        key.managed = false;
        key.pending.clear();
    }
    SDL_ResetKeyboard();
    controls = {};
}

struct SampledFrame {
    u32 held = 0;
    StagePlayerState::TouchMotion touch;
    bool focus = false;
    bool skip = false;
};

SampledFrame sample_input() {
    SampledFrame sampled;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_WINDOW_FOCUS_LOST) {
            clear_inputs();
            lost_focus = true;
        } else if (event.type == SDL_EVENT_QUIT) {
            app->quit();
        } else if (event.type == SDL_EVENT_JOYSTICK_ADDED) {
            add_controller(event.jdevice.which);
        } else if (event.type == SDL_EVENT_JOYSTICK_REMOVED && controller &&
                   SDL_GetJoystickID(controller) == event.jdevice.which) {
            SDL_CloseJoystick(controller);
            controller = nullptr;
            initialize_controller();
        }
    }
    if (app->exiting) {
        sampled.skip = true;
        return sampled;
    }
    bool keys[256]{};
    bool hosted_keys[256]{};
    bool physical_hosted_keys[256]{};
    std::vector<std::pair<u32, bool>> hosted_edges;
    const bool live_mp_input = multiplayer::session.Simulating();
    const auto* physical = SDL_GetKeyboardState(nullptr);
    for (auto& key : keyboard_map) {
        if (key.hosted) {
            if (key.vk < 256) physical_hosted_keys[key.vk] = true;
            if (key.vk >= 160 && key.vk <= 165) physical_hosted_keys[16 + (key.vk - 160) / 2] = true;
            if (key.scan == 28 || key.scan == 156) physical_hosted_keys[13] = true;
        }
        bool hosted = key.hosted;
        if (live_mp_input && !key.pending.empty()) {
            hosted = key.pending.front();
            key.pending.pop_front();
            hosted_edges.emplace_back(key.scan, hosted);
        }
        if (hosted) {
            if (key.vk < 256) hosted_keys[key.vk] = true;
            if (key.vk >= 160 && key.vk <= 165) hosted_keys[16 + (key.vk - 160) / 2] = true;
            if (key.scan == 28 || key.scan == 156) hosted_keys[13] = true;
        }
        if (hosted ||
            ((!live_mp_input || !key.managed) && key.native != SDL_SCANCODE_UNKNOWN && physical[key.native])) {
            if (key.vk < 256) keys[key.vk] = true;
            if (key.vk >= 160 && key.vk <= 165) keys[16 + (key.vk - 160) / 2] = true;
            if (key.scan == 28 || key.scan == 156) keys[13] = true;
        }
    }
    const int mapped_pad = eagler::input::BrowserGamepadState();
    eagler::input::ApplyGamepadVirtualKeys(keys, mapped_pad);
    u32 raw_buttons = 0;
    const u32 pad_keys = mapped_pad < 0 ? sample_controller(raw_buttons) : 0;
    app->raw_pad_buttons = raw_buttons;
    const auto touch = gestures.sample(touch_state(), SDL_GetTicks(), keys[16] || (pad_keys & 8),
                                       keys[37] || keys[38] || keys[39] || keys[40] || (pad_keys & 0xf0));
    for (u32 i = 0; i < 256; ++i)
        keys[i] = keys[i] || touch.keys[i];
    sampled.touch = {touch.motion, touch.x, touch.y};
    if (app->run && app->run->battle.seat_count < 2) app->run->battle.touch = sampled.touch;
    u32 held = keyboard_keys(keys) | pad_keys;
    controls.update(held);
    sampled.held = held;
    sampled.focus = app->replay_active ? false : lost_focus;
    if (multiplayer::session.Simulating()) {
        const auto frame = multiplayer::session.CaptureFrame();
        multiplayer::session.TraceTiming("read", frame, held, touch.motion);
        for (const auto& edge : hosted_edges)
            multiplayer::session.TraceTiming("read-hosted-edge", frame, edge.first, edge.second);
        multiplayer::session.TraceTiming("read-hosted-physical", frame, keyboard_keys(physical_hosted_keys));
        multiplayer::session.TraceTiming("read-hosted", frame, keyboard_keys(hosted_keys));
        multiplayer::session.TraceTiming("read-touch-xy", frame, timing_float_bits(touch.x),
                                         timing_float_bits(touch.y));
    }
    lost_focus = false;
    return sampled;
}

bool sample_and_tick(float fps = 60.f, unsigned* simulated = nullptr) {
    if (!app) return false;
    if (mp_room_preview && !multiplayer::session.PumpTerminal(static_cast<std::uint64_t>(emscripten_get_now())))
        return false;
    const SampledFrame sampled = sample_input();
    if (sampled.skip) return true;
    if (simulated) ++*simulated;
    return app->update(mp_room_preview ? GameInput{} : controls, mp_room_preview ? false : sampled.focus) &&
           app->commit(fps) && app->present();
}

multiplayer::FrameSchedule mp_schedule;
multiplayer::FrameClock mp_clock;
multiplayer::SlowdownGovernor mp_governor;
bool mp_slowdown_enabled = true;
double mp_step_cost_ms = 0;
double mp_display_interval_ms = 0, mp_display_last_ms = -1;
bool mp_work_queued = false;
u32 mp_work_ticket = 0;
unsigned mp_tasks_remaining = 0;
bool mp_network_wait = false;
void queue_mp_work(bool received = false);

bool mp_restore_history_root(const multiplayer::SessionSetup& setup, unsigned stage,
                             const multiplayer::StageEntry* entry, bool legacy_pickup_rewards = false) {
    const std::array<th13::i32, 3> characters = {th13::i32(setup.selections[0]), th13::i32(setup.selections[1]),
                                                 th13::i32(setup.selections[2])};
    if (!app->begin_title(0) || !app->configure_multiplayer(th13::i32(setup.player_count), characters, 0)) {
        last_error = "TH13 multiplayer replay seat configuration rejected";
        return false;
    }
    app->mp_history_playback = true;
    app->mp_challenge = setup.challenge;
    app->mp_keep_bullets_on_miss = setup.keep_bullets_on_miss;
    app->mp_legacy_pickup_rewards = legacy_pickup_rewards;
    app->progress = th13::SessionState{};
    app->pending_load = 0;
    app->transition_gate_active = false;
    app->transition_gate_frames = 0;
    app->progress.run_mode_flags = 0;
    app->progress.practice_mode = 0;
    app->progress.selected_spell_id = -1;
    if (!app->start_game(th13::i32(stage))) {
        last_error = app->failure;
        return false;
    }
    app->pending_destination = -1;
    app->progress.character = th13::i32(setup.selections[0]);
    app->progress.difficulty = th13::i32(setup.difficulty);
    app->progress.stage = app->progress.starting_stage = th13::i32(stage);
    app->selected_stage = th13::i32(stage);
    app->frames = 0;
    app->game_random.reseed_restore(static_cast<std::uint16_t>(setup.seed));
    app->visual.reseed_restore(static_cast<std::uint16_t>(setup.seed));
    if (!app->begin_run()) {
        last_error = app->failure.empty() ? "TH13 multiplayer replay begin_run failed" : app->failure;
        return false;
    }
    if (entry) {
        std::uint16_t gameSeed = 0, visualSeed = 0;
        std::uint32_t gameIndex = 0, visualIndex = 0;
        entry->Restore(app->run->battle, app->progress, app->run->score_view(), gameSeed, gameIndex, visualSeed,
                       visualIndex);
        app->game_random.reseed_restore(gameSeed);
        app->game_random.set_index(gameIndex);
        app->visual.reseed_restore(visualSeed);
        app->visual.set_index(visualIndex);
        for (unsigned seat = 0; seat < entry->seat_count && seat < 3; ++seat)
            app->run->battle.seat_inputs[seat] = entry->inputs[seat];
    }
    if (app->mp_challenge && app->run) app->run->battle.apply_challenge_locks();
    return true;
}

bool mp_tick_spectator(float fps, unsigned& simulated, unsigned& attempted) {
    auto& runtime = multiplayer::session.Runtime();
    auto& playback = runtime.StagePlayback();
    const auto begin = std::chrono::steady_clock::now();
    const bool fast = playback.FastForwarding();
    app->audio_device.suspend(playback.Paused() || playback.Seeking());
    const bool ok = playback.Step(
        [&](const Netplay::SpectatorStageEvent& checkpoint) {
            const auto& setup = multiplayer::session.Setup();
            const unsigned stage = setup.difficulty == 4 ? 7u : 1u;
            multiplayer::StageEntry entry;
            if (checkpoint.frame != 0 || checkpoint.stage != stage ||
                checkpoint.game != multiplayer::SpectatorJournal::kGame ||
                checkpoint.playerCount != setup.player_count || checkpoint.sessionId != setup.session_id ||
                checkpoint.gameplayAbi != multiplayer::GameplayAbi(setup) ||
                checkpoint.schema != multiplayer::SpectatorStageSchema ||
                !multiplayer::StageEntry::Decode(checkpoint.state.data(), checkpoint.state.size(), entry) ||
                entry.stage != stage || entry.chapter_frame != 0 || entry.seat_count != setup.player_count)
                return false;
            if (!mp_restore_history_root(setup, stage, &entry)) return false;
            mp_spectator_stage = stage;
            return true;
        },
        [&](const Netplay::SpectatorFramePacket& packet, bool seeking) {
            if (!app->run) return false;
            const unsigned count = std::min<unsigned>(packet.playerCount, 3);
            auto& battle = app->run->battle;
            const auto menu = multiplayer::InputLanes::AdvanceHistory(battle, packet.inputs.data(), count);
            app->run->playback_input = []() -> const th13::GameInput* { return &app->run->battle.seat_inputs[0]; };
            ++attempted;
            if (!app->update(menu, false) || !app->commit(fps)) return false;
            ++simulated;
            return true;
        },
        [&] {
            return app->pending_load || app->pending_destination >= 0 ||
                   std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - begin).count() >= 6.0;
        },
        fast ? 120 : 1);
    if (!ok) {
        last_error = "TH13 spectator stage restoration failed";
        return false;
    }
    return app->present();
}

bool mp_tick_playback(float fps, unsigned& simulated, unsigned& attempted) {
    const auto* archive = mp_replay_loaded.get();
    if (!archive || !archive->Loaded()) return false;
    const bool seeking = mp_playback_seek_target != Netplay::INVALID_FRAME;
    app->audio_device.suspend(seeking || mp_playback_paused);
    if (mp_playback_paused && !seeking) return app->present();
    const auto begin = std::chrono::steady_clock::now();
    const auto limit = seeking ? mp_playback_seek_target : archive->FrameCount();
    unsigned steps = 0;
    while (steps < (seeking ? 120u : mp_playback_rate) && mp_playback_frame < limit) {
        if (!app->run) {
            last_error = "TH13 multiplayer replay world ended before its input history";
            return false;
        }
        const auto* frame = archive->FrameAt(mp_playback_frame);
        if (!frame) {
            last_error = "TH13 multiplayer replay frame is missing";
            return false;
        }
        const unsigned count = archive->Setup().player_count;
        auto& battle = app->run->battle;
        const auto menu = multiplayer::InputLanes::AdvanceHistory(battle, frame->data(), count);
        app->run->playback_input = []() -> const th13::GameInput* { return &app->run->battle.seat_inputs[0]; };
        ++attempted;
        if (!app->update(menu, false) || !app->commit(fps)) return false;
        ++mp_playback_frame;
        ++steps;
        ++simulated;
        if (app->exiting) break;
        if (app->pending_load || app->pending_destination >= 0) break;
        if (seeking &&
            std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - begin).count() >= 6.0)
            break;
    }
    if (seeking && mp_playback_frame >= mp_playback_seek_target) mp_playback_seek_target = Netplay::INVALID_FRAME;
    return app->present();
}

bool mp_tick(float fps, unsigned& simulated, unsigned& attempted, unsigned& rollback) {
    if (!app) return false;
    const auto tick_started = std::chrono::steady_clock::now();
    const auto elapsed_ms = [&]() {
        return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - tick_started).count();
    };
    const double display_ms = mp_display_interval_ms > 0 ? mp_display_interval_ms : 16.7;
    const double kMpCallbackBudgetMs = mp_slowdown_enabled ? std::clamp(display_ms * .75, 11.5, 500.) : 11.5;
    const double kMpCatchupBudgetMs = kMpCallbackBudgetMs * (8.5 / 11.5);
    constexpr unsigned kMpMaxCatchupSteps = 120;
    struct LogicalDrawReset {
        GraphicsDevice& graphics;
        ~LogicalDrawReset() { graphics.logical_drawing = false; }
    } reset_logical_draw{app->graphics};
    const auto can_logical_draw = [&]() {
        return !(app->pause && app->pause->active()) && !app->graphics.capture_active();
    };
    const std::uint64_t now_ms = static_cast<std::uint64_t>(emscripten_get_now());
    if (!multiplayer::session.Pump(now_ms)) return false;
    if (multiplayer::session.Spectator()) return mp_tick_spectator(fps, simulated, attempted);
    if (mp_playback_pending && mp_started) return mp_tick_playback(fps, simulated, attempted);
    const auto terminal = multiplayer::complete_terminal_handoff(*app, multiplayer::session);
    if (terminal != multiplayer::TerminalHandoff::NotReady) return terminal == multiplayer::TerminalHandoff::Finished;
    if (!multiplayer::session.Simulating()) {
        const SampledFrame sampled = sample_input();
        if (sampled.skip) return true;
        ++attempted;
        if (!(app->update(controls, sampled.focus) && app->commit(fps))) return false;
        ++simulated;
        return app->present();
    }
    const auto capture_due_input = [&]() {
        if (!mp_schedule.owed()) return true;
        const bool correcting = multiplayer::session.Correcting();
        const auto frontier =
            correcting ? multiplayer::session.Runtime().CorrectionEnd() : multiplayer::session.SimFrame();
        const auto capture = multiplayer::session.CaptureFrame();
        const bool transition = !correcting && (app->stage_transition_loading || app->stage_transition_active ||
                                                app->pending_destination == i32(Destination::NextStage));
        if (capture > frontier && (!transition || capture - frontier >= multiplayer::SessionSetup::MaxRollbackFrames))
            return true;
        const auto sampled = sample_input();
        if (sampled.skip) return true;
        if (!multiplayer::session.CaptureLocal(sampled.held, static_cast<std::uint64_t>(emscripten_get_now()),
                                               sampled.touch)) {
            last_error = "MP input capture rejected controls " + std::to_string(sampled.held);
            return false;
        }
        multiplayer::session.AdvanceCapturePhase();
        mp_schedule.complete();
        return true;
    };
    if (!capture_due_input()) return false;
    if (app->exiting) return true;
    mp_network_wait = false;
    while (multiplayer::session.Correcting()) {
        if (!multiplayer::CompleteConfirmedStageHandoff(
                app->pending_destination == i32(Destination::NextStage), multiplayer::session.Committed(),
                multiplayer::session.SimFrame(), [] { return app->apply_destination(); }))
            return false;
        if (!multiplayer::session.CanSimulate()) {
            mp_network_wait = true;
            mp_schedule.network_wait();
            break;
        }
        const std::uint32_t before = multiplayer::session.SimFrame();
        const std::uint32_t corr_end = multiplayer::session.Runtime().CorrectionEnd();
        const bool more_replay = before + 1 < corr_end;
        const bool will_run_forward =
            !more_replay && corr_end < multiplayer::session.CaptureFrame() && elapsed_ms() < kMpCallbackBudgetMs;
        trace_applied_touch();
        ++attempted;
        ++rollback;
        if (!app->update(multiplayer::session.MenuInput(), false)) return false;
        app->graphics.logical_drawing = can_logical_draw() && (more_replay || will_run_forward);
        const bool committed = app->commit(fps);
        app->graphics.logical_drawing = false;
        if (!committed) return false;
        multiplayer::session.OnFrameSimulated();
        if (multiplayer::session.SimFrame() == before) {
            last_error = "MP frame did not commit";
            return false;
        }
        ++simulated;
        if (app->exiting) break;
        if (multiplayer::session.Correcting() && elapsed_ms() >= kMpCallbackBudgetMs) break;
    }
    if (multiplayer::session.Correcting() && !mp_network_wait && !mp_tasks_remaining) mp_tasks_remaining = 1;
    if (app->pending_destination >= 0 && !multiplayer::session.Correcting()) {
        mp_schedule.cpu_yield();
        return app->present();
    }
    const unsigned max_steps = mp_slowdown_enabled ? kMpMaxCatchupSteps : 2;
    unsigned steps = 0;
    bool promised = false;
    while (steps < max_steps && !multiplayer::session.Correcting() && !app->exiting) {
        if (steps == 0 && simulated > 0 && elapsed_ms() >= kMpCallbackBudgetMs) break;
        const double step_started_ms = elapsed_ms();
        if (steps >= 1 && (!promised || (!mp_slowdown_enabled && elapsed_ms() >= kMpCatchupBudgetMs))) break;
        if (!capture_due_input()) return false;
        if (app->exiting) break;
        if (multiplayer::session.SimFrame() >= multiplayer::session.CaptureFrame()) break;
        if (!multiplayer::session.CanSimulate()) {
            mp_network_wait = true;
            mp_schedule.network_wait();
            break;
        }
        const std::uint32_t before = multiplayer::session.SimFrame();
        const bool can_run_second =
            steps + 1 < max_steps && (before + 1 < multiplayer::session.CaptureFrame() || mp_schedule.owed() > 0) &&
            step_started_ms < kMpCatchupBudgetMs &&
            (!mp_slowdown_enabled || step_started_ms + 2. * mp_step_cost_ms <= kMpCallbackBudgetMs);
        promised = can_run_second;
        trace_applied_touch();
        ++attempted;
        if (!app->update(multiplayer::session.MenuInput(), false)) return false;
        app->graphics.logical_drawing =
            can_logical_draw() && can_run_second && app->pending_load == 0 && app->pending_destination < 0;
        const bool committed = app->commit(fps);
        app->graphics.logical_drawing = false;
        if (!committed) return false;
        multiplayer::session.OnFrameSimulated();
        if (multiplayer::session.SimFrame() == before) {
            last_error = "MP frame did not commit";
            return false;
        }
        ++steps;
        ++simulated;
        mp_step_cost_ms += (elapsed_ms() - step_started_ms - mp_step_cost_ms) * .25;
        if (app->pending_load || app->pending_destination >= 0) break;
    }
    mp_schedule.cpu_yield();
    {
        auto& runtime = multiplayer::session.Runtime();
        if (!multiplayer::session.Spectator())
            runtime.PublishSpectator(multiplayer::session.ConfirmedThrough(), multiplayer::session.SimFrame()
                                                                                  ? multiplayer::session.SimFrame() - 1
                                                                                  : Netplay::INVALID_FRAME);
    }
    return simulated && !multiplayer::session.Correcting() ? app->present() : true;
}

}

bool drive_one_frame(double delta, unsigned source = 0) {
    const double begin = emscripten_get_now();
    const double current_sequence = ++work_sequence;
    const int stage = app->progress.stage;
    const int phase = suspended                            ? 3
                      : app->loading_waiting()             ? 2
                      : app->pause && app->pause->active() ? 4
                      : app->run                           ? 0
                                                           : 1;
    const auto before_presentations = app->graphics.backend.stats.presentations;
    const auto before_restores = multiplayer::session.Restores();
    unsigned simulated = 0, attempted = 0, rollback = 0;
    double presentation_at = -1;
    const auto finish = [&](bool result, bool ok) {
        const auto presentations = app->graphics.backend.stats.presentations - before_presentations;
        const auto after_restores = multiplayer::session.Restores();
        const unsigned restores = after_restores >= before_restores ? after_restores - before_restores : after_restores;
        const int end_stage = app->progress.stage;
        const double end = emscripten_get_now();
        th13_browser_work_frame(current_sequence, begin, end, stage, end_stage, phase, source, attempted, rollback,
                                restores, presentations, presentation_at, ok);
        return result;
    };
    if (suspended) {
        reset_cadence();
        return finish(true, true);
    }
    if (app->loading_waiting()) {
        reset_cadence();
        reset_presentation_window();
        if (!(app->loading_startup ? app->advance_loading(delta) : app->complete_loading())) {
            running = false;
            th13_browser_frame(0, emscripten_get_now() - begin, 0);
            return finish(false, false);
        }
        app->audio_device.pump();
        return finish(true, true);
    }
    const bool live_mp = multiplayer::session.Active() && multiplayer::session.Simulating() &&
                         !multiplayer::session.Spectator() && !mp_playback_pending;
    if (live_mp != frame_meter_live) {
        frame_meter_live = live_mp;
        mp_schedule.take_completed();
        frame_meter.restart();
    }
    const bool display_callback = source == 1;
    if (display_callback) last_display_callback_ms = begin;
    const bool presents =
        display_callback ||
        (source == 3 && (last_display_callback_ms < 0 || begin - last_display_callback_ms > kDisplayStaleMs));
    if (presents) {
        sample_presentation_rate(begin);
        frame_meter.sample(live_mp ? mp_schedule.credit() : cadence.debt);
    }
    frame_meter.elapse(delta);
    unsigned ticks;
    if (live_mp) {
        if (source == 1) {
            if (mp_display_last_ms >= 0) {
                const double gap = std::min(begin - mp_display_last_ms, 1000.);
                mp_display_interval_ms =
                    mp_display_interval_ms > 0 ? mp_display_interval_ms + (gap - mp_display_interval_ms) * .25 : gap;
            }
            mp_display_last_ms = begin;
        }
        multiplayer::session.TraceTiming("callback", multiplayer::session.SimFrame(), mp_schedule.owed(), source);
        double scale = multiplayer::session.IntervalScale();
        if (!(std::isfinite(scale) && scale > 0)) scale = 1.;
        if (mp_slowdown_enabled)
            scale = std::max(scale, mp_governor.update(multiplayer::SlowdownGovernor::depth_of(
                                        multiplayer::session.SimFrame(), multiplayer::session.ConfirmedThrough())));
        if (mp_slowdown_enabled)
            mp_schedule.advance_gap(delta / scale);
        else
            mp_schedule.advance(delta / scale);
        multiplayer::session.TraceTiming("clock-due", multiplayer::session.SimFrame(), mp_schedule.owed(),
                                         multiplayer::session.CaptureFrame());
        ticks = 1;
    } else {
        ticks = cadence.advance(delta);
    }
    bool ok = true;
    const bool mp_active = multiplayer::session.Active();
    app->graphics.backend.defer = true;
    unsigned executed = 0;
    for (; executed < ticks && ok; executed++) {
        ok = mp_active ? mp_tick(frame_meter.fps, simulated, attempted, rollback)
                       : sample_and_tick(frame_meter.fps, &attempted);
        if (app->exiting || app->pending_load) {
            ++executed;
            break;
        }
    }
    frame_meter.frame(live_mp ? mp_schedule.take_completed() : executed);
    const bool fresh = app->graphics.backend.pending_presentation();
    if (ok && presents && app->graphics.backend.commit(display_callback)) {
        presentation_at = emscripten_get_now();
        if (fresh) ++presentation_count;
        if (live_mp && multiplayer::session.SimFrame())
            multiplayer::session.TraceTiming("present", multiplayer::session.SimFrame() - 1);
    }
    if (app->graphics.backend.error()[0]) ok = false;
    app->graphics.backend.defer = false;
    app->audio_device.pump();
    th13_browser_frame(ok ? 1 : 0, emscripten_get_now() - begin, mp_active ? simulated : ticks);
    if (live_mp)
        multiplayer::session.TraceTiming("callback-end", multiplayer::session.SimFrame(), simulated,
                                         mp_schedule.owed());
    if (!ok || app->exiting) running = false;
    if (live_mp) queue_mp_work();
    return finish(running, ok);
}

namespace {
void queue_mp_work(bool received) {
    if (!running || suspended || !app || app->exiting || app->pending_load || app->loading_waiting() ||
        !multiplayer::session.Simulating() || mp_playback_pending || multiplayer::session.Spectator() ||
        mp_work_queued ||
        (!received && (mp_network_wait || (!mp_schedule.has_work() && !multiplayer::session.Correcting() &&
                                           multiplayer::session.SimFrame() >= multiplayer::session.CaptureFrame()))) ||
        (!received && !mp_tasks_remaining))
        return;
    mp_work_queued = true;
    const auto ticket = ++mp_work_ticket;
    multiplayer::session.TraceTiming("wake-queued", multiplayer::session.SimFrame(), ticket, received ? 1u : 0u);
    emscripten_async_call(
        [](void* value) {
            if (u32(uintptr_t(value)) != mp_work_ticket) return;
            mp_work_queued = false;
            if (!running || suspended || !app || app->exiting || app->pending_load || app->loading_waiting() ||
                !multiplayer::session.Active() || !multiplayer::session.Simulating() ||
                multiplayer::session.Spectator() || mp_playback_pending)
                return;
            if (mp_tasks_remaining) --mp_tasks_remaining;
            drive_one_frame(0, 2);
        },
        reinterpret_cast<void*>(uintptr_t(ticket)), 0);
}
}

int mp_run_frame(double delta_ms) {
    if (!running || !app) return 0;
    if (!std::isfinite(delta_ms) || delta_ms < 0.0) return 0;
    mp_clock.elapsed(emscripten_get_now());
    return drive_one_frame(delta_ms / 1000.0, 3) ? 1 : 0;
}

void reset_mp_accumulator() {
    mp_schedule.reset();
    mp_schedule.align_phase();
    mp_schedule.take_completed();
    frame_meter.restart();
    mp_governor.reset();
    mp_step_cost_ms = 0;
    mp_display_interval_ms = 0;
    mp_display_last_ms = -1;
    mp_clock.reset();
    ++mp_work_ticket;
    mp_work_queued = false;
    mp_tasks_remaining = 0;
    mp_network_wait = false;
    for (auto& key : keyboard_map) {
        key.pending.clear();
        key.managed = false;
    }
}
}

namespace th13::multiplayer::detail {
std::string mp_base64_encode(const std::uint8_t* data, std::size_t size) {
    static const char kDigits[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    out.reserve((size + 2) / 3 * 4);
    for (std::size_t at = 0; at < size; at += 3) {
        const std::uint32_t b0 = data[at];
        const std::uint32_t b1 = at + 1 < size ? data[at + 1] : 0;
        const std::uint32_t b2 = at + 2 < size ? data[at + 2] : 0;
        out.push_back(kDigits[b0 >> 2]);
        out.push_back(kDigits[(b0 & 3) << 4 | b1 >> 4]);
        out.push_back(at + 1 < size ? kDigits[(b1 & 15) << 2 | b2 >> 6] : '=');
        out.push_back(at + 2 < size ? kDigits[b2 & 63] : '=');
    }
    return out;
}
std::vector<std::uint8_t> mp_base64_decode(const char* text) {
    static const auto value = [](char c) -> int {
        if (c >= 'A' && c <= 'Z') return c - 'A';
        if (c >= 'a' && c <= 'z') return c - 'a' + 26;
        if (c >= '0' && c <= '9') return c - '0' + 52;
        if (c == '+') return 62;
        if (c == '/') return 63;
        return -1;
    };
    std::vector<std::uint8_t> out;
    if (!text) return out;
    int buffer = 0, bits = 0;
    for (const char* p = text; *p; ++p) {
        if (*p == '=' || *p == '\n' || *p == '\r') continue;
        const int digit = value(*p);
        if (digit < 0) {
            out.clear();
            return out;
        }
        buffer = buffer << 6 | digit;
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            out.push_back(std::uint8_t(buffer >> bits));
        }
    }
    return out;
}
}

extern "C" {

EMSCRIPTEN_KEEPALIVE int th13_prepare_loading() {
    using namespace th13::sdl;
    LoadingWork work(4);
    const int result = [&]() -> int {
        if (running) return 0;
        work.replacing_app();
        app = std::make_unique<ApplicationState>();
        app->graphics.render_scale = render_scale;
        return app->prepare_loading();
    }();
    return work.finish(result, result != 0);
}

EMSCRIPTEN_KEEPALIVE int th13_draw_startup(unsigned frames) {
    using namespace th13::sdl;
    LoadingWork work(6);
    const int result = [&]() -> int {
        if (!app || running) return 0;
        for (unsigned frame = 0; frame < std::min(frames, 120u); ++frame) {
            if (!app->loading_animations.update(false) || !app->loading_animations.update(true)) return 0;
            ++app->loading_frames;
        }
        return app->render_loading();
    }();
    return work.finish(result, result != 0);
}

EMSCRIPTEN_KEEPALIVE int th13_initialize() {
    using namespace th13::sdl;
    LoadingWork work(5);
    const int result = [&]() -> int {
        if (running) return 1;
        clear_inputs();
        if (!SDL_InitSubSystem(SDL_INIT_JOYSTICK)) {
            last_error = SDL_GetError();
            return 0;
        }
        initialize_controller();
        if (!app || !app->platform_prepared || app->initialized) {
            work.replacing_app();
            app = std::make_unique<ApplicationState>();
            app->graphics.render_scale = render_scale;
        }
        if (!app->initialize(true)) {
            last_error = app->failure;
            work.replacing_app();
            app.reset();
            return 0;
        }
        app->graphics.release_startup_branding();
        for (auto& key : keyboard_map)
            key.native = SDL_GetScancodeFromName(key.sdl);
        previous_frame = -1;
        reset_cadence();
        return 1;
    }();
    return work.finish(result, result != 0);
}

EMSCRIPTEN_KEEPALIVE void th13_always_hitbox(unsigned enabled) {
    if (th13::sdl::app) th13::sdl::app->renderer.always_hitbox = enabled != 0;
}

EMSCRIPTEN_KEEPALIVE void th13_music_enabled(unsigned enabled) {
    if (th13::sdl::app) th13::sdl::app->audio_device.music_enabled = enabled != 0;
}

EMSCRIPTEN_KEEPALIVE void th13_render_scale(unsigned scale) {
    if (!th13::sdl::running) th13::sdl::render_scale = scale >= 2 ? 2 : 1;
}

EMSCRIPTEN_KEEPALIVE const char* th13_error() {
    using namespace th13::sdl;
    if (app && app->graphics.backend.error()[0]) return app->graphics.backend.error();
    if (app && !app->graphics.submission_trace.good()) return app->graphics.submission_trace.error().c_str();
    if (app && !app->graphics.error.empty()) return app->graphics.error.c_str();
    return app ? app->failure.c_str() : last_error.c_str();
}

EMSCRIPTEN_KEEPALIVE void th13_debug_gl_checks(int enabled) { sdl_gl_checks(enabled); }

EMSCRIPTEN_KEEPALIVE unsigned th13_phase() {
    using namespace th13::sdl;
    return !app ? 4 : app->exiting ? 4 : app->title ? 0 : app->pause && app->pause->active() ? 2 : 1;
}

EMSCRIPTEN_KEEPALIVE int th13_room_preview() {
    using namespace th13::sdl;
    LoadingWork work(7);
    const int result = [&]() -> int {
        if (running || !app || !app->initialized || !app->title || mp_started) return 0;
        clear_inputs();
        mp_room_preview = true;
        return app->commit(60.f) && app->present();
    }();
    return work.finish(result, result != 0);
}

EMSCRIPTEN_KEEPALIVE int th13_room_return_ready() {
    using namespace th13::sdl;
    return app && mp_started && !mp_spectator_mode && !th13::multiplayer::session.Simulating() &&
           (app->exiting ||
            (!app->run && !app->ending && app->title && app->title->state.screen == th13::TitleScreen::Main));
}

EMSCRIPTEN_KEEPALIVE int th13_room_return() {
    using namespace th13::sdl;
    LoadingWork work(9);
    const int result = [&]() -> int {
        if (running || !app || !app->initialized || th13::multiplayer::session.Simulating()) return 0;
        th13::multiplayer::session.Stop(true);
        mp_started = false;
        clear_inputs();
        app->exiting = false;
        app->pending_destination = -1;
        if (!app->begin_title(1)) return 0;
        mp_room_preview = true;
        return 1;
    }();
    return work.finish(result, result != 0);
}

EMSCRIPTEN_KEEPALIVE float th13_debug_presentation_fps() { return th13::sdl::presentation_fps; }

EMSCRIPTEN_KEEPALIVE float th13_debug_frame_rate() { return th13::sdl::frame_meter.fps; }

EMSCRIPTEN_KEEPALIVE unsigned th13_frame() { return th13::sdl::app ? th13::sdl::app->frames : 0; }
EMSCRIPTEN_KEEPALIVE unsigned th13_boot_seed() { return th13::sdl::app ? th13::sdl::app->boot_seed : 0; }

EMSCRIPTEN_KEEPALIVE void th13_debug_fps(float fps) {
    using namespace th13::sdl;
    if (!app) return;
    if (!std::isfinite(fps) || fps < 0.f || fps > 10000.f) return;
    app->set_fps_color_override(fps);
}

EMSCRIPTEN_KEEPALIVE unsigned th13_run_mode_flags() {
    return th13::sdl::app ? unsigned(th13::sdl::app->progress.run_mode_flags) : 0u;
}

EMSCRIPTEN_KEEPALIVE int th13_practice_mode() { return th13::sdl::app ? th13::sdl::app->progress.practice_mode : 0; }

EMSCRIPTEN_KEEPALIVE const int* th13_probe_state() {
    using namespace th13::sdl;
    return app ? app->projection() : nullptr;
}

EMSCRIPTEN_KEEPALIVE void th13_key(unsigned scan, unsigned down) {
    if (th13::multiplayer::session.Simulating())
        th13::multiplayer::session.TraceTiming("device", th13::multiplayer::session.CaptureFrame(), scan, down);
    for (auto& key : th13::sdl::keyboard_map) {
        if (key.scan != scan) continue;
        if (th13::multiplayer::session.Simulating()) {
            key.managed = true;
            if (key.hosted != (down != 0)) key.pending.push_back(down != 0);
        }
        key.hosted = down != 0;
        if (th13::multiplayer::session.Simulating()) {
            bool keys[256]{};
            if (key.vk < 256) keys[key.vk] = true;
            if (key.vk >= 160 && key.vk <= 165) keys[16 + (key.vk - 160) / 2] = true;
            if (key.scan == 28 || key.scan == 156) keys[13] = true;
            th13::multiplayer::session.TraceTiming("device-map", th13::multiplayer::session.CaptureFrame(),
                                                   th13::keyboard_keys(keys), (scan << 1) | !!down);
        }
    }
}

EMSCRIPTEN_KEEPALIVE int th13_debug_mp_seats(int count, int c0, int c1, int c2, int local_seat) {
    using namespace th13::sdl;
    if (!app || !app->initialized) return 0;
    if (count != 0 && (count < 2 || count > 3)) return 0;
    const int characters[3] = {c0, c1, c2};
    for (int i = 0; i < count; ++i)
        if (characters[i] < 0 || characters[i] > 3) return 0;
    if (!app->configure_multiplayer(count, {c0, c1, c2}, local_seat)) return 0;
    return 1;
}

EMSCRIPTEN_KEEPALIVE int th13_debug_mp_seat_input(int seat, unsigned held) {
    using namespace th13::sdl;
    if (!app || !app->run || seat <= 0 || seat >= int(app->run->battle.seat_inputs.size())) return 0;
    app->run->battle.seat_inputs[size_t(seat)].update(held);
    return 1;
}

EMSCRIPTEN_KEEPALIVE int th13_debug_mp_trance_slot(int value) {
    using namespace th13::sdl;
    if (!app || !app->run) return -1;
    auto& progress = app->run->battle.sim.session ? *app->run->battle.sim.session : app->progress;
    if (value >= 0) progress.ending_points = value;
    return progress.ending_points;
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_mp_trance_state() {
    using namespace th13::sdl;
    static std::string report;
    if (!app || !app->run) {
        report = "{}";
        return report.c_str();
    }
    auto& battle = app->run->battle;
    char buffer[192];
    std::snprintf(buffer, sizeof buffer,
                  "{\"mode\":%d,\"owner\":%d,\"generation\":%u,\"slot\":%d,"
                  "\"active\":%d,\"seat_count\":%d,\"ghosts\":%d}",
                  int(battle.trance_mode), battle.trance_owner_seat, battle.trance_generation,
                  battle.sim.session ? battle.sim.session->ending_points : 0, battle.team_trance_active() ? 1 : 0,
                  battle.seat_count, battle.ghost_count());
    report = buffer;
    return report.c_str();
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_mp_seat_state(int seat) {
    using namespace th13::sdl;
    static std::string report;
    if (!app || !app->run || seat < 0 || seat >= int(app->run->battle.seats.size())) {
        report = "{}";
        return report.c_str();
    }
    auto& battle = app->run->battle;
    const bool window_seat = battle.seat_count == 0 && seat == 0;
    const auto& state = window_seat ? battle.player_state() : battle.seats[size_t(seat)];
    const auto* hud = window_seat ? nullptr : &battle.seat_huds[size_t(seat)];
    char buffer[320];
    std::snprintf(buffer, sizeof buffer,
                  "{\"seat\":%d,\"x\":%.6f,\"y\":%.6f,\"phase\":%d,\"phase_frame\":%d,"
                  "\"invulnerable\":%d,\"dead\":%d,\"shots\":%zu,\"bombs\":%d,"
                  "\"extra_lives\":%d,\"power\":%d,\"character\":%d,\"seat_count\":%d,"
                  "\"ghost\":%d,\"deaths\":%d}",
                  seat, state.x, state.y, int(state.phase), state.phase_frame, state.invulnerable, int(state.dead),
                  state.shots.size(), hud ? hud->bombs : -1, hud ? hud->extra_lives : -1, hud ? hud->power : -1,
                  state.character, battle.seat_count, int(state.ghost), hud ? hud->deaths : 0);
    report = buffer;
    return report.c_str();
}

EMSCRIPTEN_KEEPALIVE int th13_debug_mp_net_configure(const char* words_json) {
    using namespace th13::sdl;
    if (!words_json) return 0;
    std::vector<unsigned> words;
    const char* p = words_json;
    while (*p) {
        while (*p == ' ' || *p == ',' || *p == '[' || *p == ']')
            ++p;
        if (!*p) break;
        char* end = nullptr;
        const unsigned long value = std::strtoul(p, &end, 10);
        if (end == p) return 0;
        words.push_back(unsigned(value));
        p = end;
    }
    if (!th13::multiplayer::session.Configure(words.data(), words.size())) return 0;
    th13::sdl::reset_mp_accumulator();
    if (app && app->run && app->mp_seat_count >= 2) {
        th13::multiplayer::session.capture_host = [] { return app->capture_multiplayer_host(); };
        th13::multiplayer::session.TryBindWorld(&app->run->battle, &app->visual, app->run->presentation.get(),
                                                app->run.get(), &app->mp_transition_handle_);
        app->run->playback_input = []() -> const th13::GameInput* {
            return th13::multiplayer::session.LocalDelayedInput();
        };
        app->wire_multiplayer_audio();
    }
    return 1;
}

EMSCRIPTEN_KEEPALIVE int th13_debug_mp_net_connect(const char* relay_url) {
    using namespace th13::sdl;
    if (!relay_url) return 0;
    return th13::multiplayer::session.Connect(relay_url) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE int th13_debug_mp_net_pump(double now_ms) {
    using namespace th13::sdl;
    return th13::multiplayer::session.Pump(std::uint64_t(now_ms)) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE int th13_debug_mp_net_ready() {
    using namespace th13::sdl;
    th13::multiplayer::session.MarkLocalReady();
    return th13::multiplayer::session.Start() ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE int th13_debug_mp_net_capture(unsigned held, double now_ms) {
    using namespace th13::sdl;
    if (!th13::multiplayer::session.CaptureLocal(held, std::uint64_t(now_ms))) return 0;
    th13::multiplayer::session.AdvanceCapturePhase();
    return 1;
}

EMSCRIPTEN_KEEPALIVE int th13_debug_mp_net_can_simulate() {
    using namespace th13::sdl;
    return th13::multiplayer::session.CanSimulate() ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE void th13_debug_mp_net_on_simulated() {
    using namespace th13::sdl;
    th13::multiplayer::session.OnFrameSimulated();
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_mp_net_rollback() {
    using namespace th13::sdl;
    static std::string report;
    const auto& session = th13::multiplayer::session;
    const auto digest_stats = th13::multiplayer::NetplayRuntime::DebugDigestStats();
    char buffer[1400];
    std::snprintf(
        buffer, sizeof buffer,
        "{\"restores\":%u,\"resimulated\":%u,\"checkpoints\":%u,"
        "\"committed\":%u,\"maxDistance\":%u,\"rollback_pending\":%d,"
        "\"records\":%zu,\"audio_pending\":%zu,\"last_error\":%d,"
        "\"sfx_suppressed\":%llu,\"music_deferred\":%llu,"
        "\"music_dropped\":%llu,\"voice_captured\":%llu,"
        "\"music_captured\":%llu,\"voice_applied\":%llu,"
        "\"music_applied\":%llu,"
        "\"desync_detected\":%llu,\"digests_sent\":%llu,"
        "\"digests_received\":%llu,"
        "\"capture_us_max\":%llu,\"capture_us_sum\":%llu,"
        "\"capture_us_count\":%llu,"
        "\"restore_us_max\":%llu,\"restore_us_sum\":%llu,"
        "\"update_us_max\":%llu,\"update_us_sum\":%llu,"
        "\"update_us_count\":%llu,\"resim_us_sum\":%llu,"
        "\"rollback_distance_sum\":%llu,\"stall_frames\":%llu}",
        session.Restores(), session.Resimulated(), session.Checkpoints(), session.Committed(),
        session.MaxRollbackDistance(), session.HasRollbackRequest() ? 1 : 0, session.RecordCount(),
        session.AudioOut().size(), session.LastRollbackFailed() ? 1 : 0,
        static_cast<unsigned long long>(session.SfxSuppressed()),
        static_cast<unsigned long long>(session.MusicDeferred()),
        static_cast<unsigned long long>(session.MusicDropped()),
        static_cast<unsigned long long>(session.VoiceCaptured()),
        static_cast<unsigned long long>(session.MusicCaptured()),
        static_cast<unsigned long long>(session.VoiceApplied()),
        static_cast<unsigned long long>(session.MusicApplied()), static_cast<unsigned long long>(digest_stats.desync),
        static_cast<unsigned long long>(digest_stats.sent), static_cast<unsigned long long>(digest_stats.received),
        static_cast<unsigned long long>(session.CaptureUsMax()),
        static_cast<unsigned long long>(session.CaptureUsSum()),
        static_cast<unsigned long long>(session.CaptureUsCount()),
        static_cast<unsigned long long>(session.RestoreUsMax()),
        static_cast<unsigned long long>(session.RestoreUsSum()), static_cast<unsigned long long>(session.UpdateUsMax()),
        static_cast<unsigned long long>(session.UpdateUsSum()),
        static_cast<unsigned long long>(session.UpdateUsCount()), static_cast<unsigned long long>(session.ResimUsSum()),
        static_cast<unsigned long long>(session.RollbackDistanceSum()),
        static_cast<unsigned long long>(session.StallFrames()));
    report = buffer;
    return report.c_str();
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_mp_host_state() {
    using namespace th13::sdl;
    static std::string report;
    th13::multiplayer::HostState state;
    report = app && app->capture_multiplayer_host_state(state)
                 ? th13::multiplayer::WorldState::diagnostic_host_detail(state)
                 : "";
    return report.c_str();
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_mp_net_audio() {
    using namespace th13::sdl;
    static std::string report;
    report = "[";
    const auto& audio = th13::multiplayer::session.AudioOut();
    for (size_t index = 0; index < audio.size(); ++index) {
        const auto& event = audio[index].observation;
        const char* kind = event.kind == AudioDevice::RequestObservation::Kind::Voice  ? "voice"
                           : event.kind == AudioDevice::RequestObservation::Kind::Stop ? "stop"
                           : event.kind == AudioDevice::RequestObservation::Kind::Pan  ? "pan"
                                                                                       : "music";
        if (index) report += ",";
        report += "{\"kind\":\"" + std::string(kind) + "\",\"id\":" + std::to_string(event.id) +
                  ",\"frame\":" + std::to_string(audio[index].frame) + "}";
    }
    report += "]";
    return report.c_str();
}

EMSCRIPTEN_KEEPALIVE void th13_debug_mp_net_audio_clear() {
    using namespace th13::sdl;
    th13::multiplayer::session.ClearAudioOut();
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_mp_net_audio_applied() {
    using namespace th13::sdl;
    static std::string report;
    report = "[";
    const auto& applied = th13::multiplayer::session.AppliedAudio();
    bool first = true;
    for (const auto& item : applied) {
        const auto& event = item.observation;
        const char* kind = event.kind == AudioDevice::RequestObservation::Kind::Voice  ? "voice"
                           : event.kind == AudioDevice::RequestObservation::Kind::Stop ? "stop"
                           : event.kind == AudioDevice::RequestObservation::Kind::Pan  ? "pan"
                                                                                       : "music";
        if (!first) report += ",";
        first = false;
        report += "{\"frame\":" + std::to_string(item.frame) + ",\"confirmed\":" + std::to_string(item.confirmed) +
                  ",\"kind\":\"" + std::string(kind) + "\",\"id\":" + std::to_string(event.id) +
                  ",\"value\":" + std::to_string(event.value) +
                  ",\"positional\":" + (event.positional ? "true" : "false") + ",\"x\":" + std::to_string(event.x) +
                  ",\"text\":\"" + event.text + "\"}";
    }
    report += "]";
    return report.c_str();
}

EMSCRIPTEN_KEEPALIVE void th13_debug_mp_net_audio_applied_clear() {
    using namespace th13::sdl;
    th13::multiplayer::session.ClearAppliedAudio();
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_mp_net_state() {
    using namespace th13::sdl;
    static std::string report;
    const auto& session = th13::multiplayer::session;
    char buffer[512];
    std::snprintf(buffer, sizeof buffer,
                  "{\"active\":%d,\"sim_frame\":%u,\"confirmed\":%u,"
                  "\"local_seat\":%u,\"players\":%u,\"can_start\":%d,"
                  "\"connected\":%d,\"simulating\":%d,\"capture_frame\":%u,"
                  "\"stretch\":%.3f,\"run_released\":%d,\"error\":\"%s\"}",
                  session.Active() ? 1 : 0, session.SimFrame(), session.ConfirmedThrough(),
                  unsigned(session.LocalSeat()), session.PlayerCount(), session.CanStart() ? 1 : 0,
                  session.Connected() ? 1 : 0, session.Simulating() ? 1 : 0, session.CaptureFrame(),
                  mp_governor.stretch(), session.RunReleased() ? 1 : 0, session.Error());
    report = buffer;
    return report.c_str();
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_mp_terminal() {
    static std::string report;
    const auto t = th13::multiplayer::session.Runtime().Terminal();
    char buffer[320];
    std::snprintf(buffer, sizeof buffer,
                  "{\"last_simulated\":%u,\"confirmed\":%u,\"acknowledged\":%u,"
                  "\"channel_active\":%d,\"retiring\":%d,\"released\":%d,"
                  "\"spectators_settled\":%d,\"started_ms\":%llu,\"release_ms\":%llu}",
                  t.last_simulated, t.confirmed, t.acknowledged, t.channel_active ? 1 : 0, t.retiring ? 1 : 0,
                  t.released ? 1 : 0, t.spectators_settled ? 1 : 0, static_cast<unsigned long long>(t.started_ms),
                  static_cast<unsigned long long>(t.release_ms));
    report = buffer;
    return report.c_str();
}

namespace {
std::unique_ptr<th13::multiplayer::WorldState> mp_world_snapshot;
th13::multiplayer::WorldState::ReplayView mp_replay_view;
bool mp_replay_captured = false;
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_mp_snapshot_capture() {
    using namespace th13::sdl;
    static std::string report;
    if (!app || !app->run) {
        report = "{\"ok\":0,\"error\":\"run absent\"}";
        return report.c_str();
    }
    if (!mp_world_snapshot) mp_world_snapshot = std::make_unique<th13::multiplayer::WorldState>();
    th13::multiplayer::WorldState::ReplayView replay;
    const bool is_replay = app->replay_active && app->replay_cursor;
    if (is_replay) {
        replay.consumed = app->replay_consumed;
        replay.cursor_eof = false;
        app->replay_cursor->capture_cursor(replay.cursor_index, replay.cursor_eof);
        replay.finished = app->replay_finished;
        replay.input = app->replay_input;
    }
    if (!mp_world_snapshot->capture(app->run->battle, app->visual, is_replay ? &replay : nullptr,
                                    app->run->presentation.get())) {
        report = "{\"ok\":0,\"error\":\"capture failed\"}";
        return report.c_str();
    }
    if (is_replay) {
        mp_replay_view = replay;
        mp_replay_captured = true;
    } else {
        mp_replay_captured = false;
    }
    char buffer[96];
    std::snprintf(buffer, sizeof buffer, "{\"ok\":1,\"digest\":\"%016llx\",\"retained\":%zu}",
                  (unsigned long long)mp_world_snapshot->digest(), mp_world_snapshot->retained_bytes());
    report = buffer;
    return report.c_str();
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_mp_snapshot_restore() {
    using namespace th13::sdl;
    static std::string report;
    if (!app || !app->run || !mp_world_snapshot || !mp_world_snapshot->ready()) {
        report = "{\"ok\":0,\"error\":\"snapshot absent\"}";
        return report.c_str();
    }
    if (mp_replay_captured && app->replay_active && app->replay_cursor) {
        app->replay_consumed = mp_replay_view.consumed;
        app->replay_cursor->restore_cursor(mp_replay_view.cursor_index, mp_replay_view.cursor_eof);
        app->replay_finished = mp_replay_view.finished;
        app->replay_input = mp_replay_view.input;
    }
    if (!mp_world_snapshot->restore(app->run->battle, app->visual, mp_replay_captured ? &mp_replay_view : nullptr,
                                    app->run->presentation.get())) {
        report = "{\"ok\":0,\"error\":\"restore failed\"}";
        return report.c_str();
    }
    th13::multiplayer::WorldState verify;
    const bool live_ok = verify.capture(app->run->battle, app->visual, nullptr, app->run->presentation.get());
    char buffer[96];
    std::snprintf(buffer, sizeof buffer, "{\"ok\":1,\"digest\":\"%016llx\",\"verified\":%d}",
                  live_ok ? (unsigned long long)verify.digest() : 0ull, live_ok ? 1 : 0);
    report = buffer;
    return report.c_str();
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_mp_snapshot_digest() {
    using namespace th13::sdl;
    static std::string report;
    if (!app || !app->run || !mp_world_snapshot || !mp_world_snapshot->ready()) {
        report = "{\"ok\":0,\"error\":\"snapshot absent\"}";
        return report.c_str();
    }
    char buffer[96];
    std::snprintf(buffer, sizeof buffer, "{\"ok\":1,\"digest\":\"%016llx\"}",
                  (unsigned long long)mp_world_snapshot->digest());
    report = buffer;
    return report.c_str();
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_mp_snapshot_digest_core() {
    using namespace th13::sdl;
    static std::string report;
    if (!app || !app->run || !mp_world_snapshot || !mp_world_snapshot->ready()) {
        report = "{\"ok\":0,\"error\":\"snapshot absent\"}";
        return report.c_str();
    }
    char buffer[96];
    std::snprintf(buffer, sizeof buffer, "{\"ok\":1,\"digest\":\"%016llx\"}",
                  (unsigned long long)mp_world_snapshot->digest_core());
    report = buffer;
    return report.c_str();
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_mp_snapshot_digest_domains() {
    using namespace th13::sdl;
    static std::string report;
    if (!app || !app->run || !mp_world_snapshot || !mp_world_snapshot->ready()) {
        report = "{\"ok\":0,\"error\":\"snapshot absent\"}";
        return report.c_str();
    }
    char buffer[256];
    buffer[0] = 0;
    mp_world_snapshot->digest_domains(buffer, sizeof buffer);
    report = std::string("{\"ok\":1,\"domains\":\"") + buffer + "\"}";
    return report.c_str();
}

EMSCRIPTEN_KEEPALIVE void th13_debug_mp_snapshot_drop() {
    if (mp_world_snapshot) mp_world_snapshot->discard();
}

namespace {
std::unique_ptr<th13::multiplayer::ReplayArchive> mp_replay_archive;
th13::multiplayer::StageEntry mp_stage_entry;
bool mp_stage_entry_valid = false;
th13::multiplayer::SpectatorJournal mp_spectator;
std::vector<std::uint8_t> mp_spectator_wire;
bool mp_spectator_publish_ready = false;
std::string mp_report;
}
using th13::sdl::mp_playback_chapter;
using th13::sdl::mp_playback_frame;
using th13::sdl::mp_playback_paused;
using th13::sdl::mp_playback_pending;
using th13::sdl::mp_playback_rate;
using th13::sdl::mp_playback_seek_target;
using th13::sdl::mp_replay_loaded;
using th13::sdl::mp_spectator_mode;
using th13::sdl::mp_started;

EMSCRIPTEN_KEEPALIVE int th13_debug_mp_replay_begin(double timestamp) {
    using namespace th13::sdl;
    if (!app) return 0;
    if (!mp_replay_archive) mp_replay_archive = std::make_unique<th13::multiplayer::ReplayArchive>();
    th13::multiplayer::SessionSetup setup;
    if (th13::multiplayer::session.Active()) {
        setup = th13::multiplayer::session.Setup();
    } else {
        setup.version = th13::multiplayer::SessionSetup::Version;
        setup.mode = 2;
        setup.session_id = 0x9001'0000'0000'0001ull;
        setup.player_count = 2;
        setup.local_player = 0;
        setup.difficulty = 2;
        setup.seed = 47932;
        setup.automatic = false;
        setup.requested_delay = setup.input_delay = 2;
        setup.selections = {0, 0, 0};
        setup.build = {1, 2, 3, 4};
    }
    return mp_replay_archive->Begin(setup, std::uint64_t(timestamp)) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE int th13_debug_mp_replay_append(unsigned frame, unsigned stage, unsigned held0, unsigned held1,
                                                     unsigned held2) {
    using namespace th13;
    if (!mp_replay_archive) return 0;
    Netplay::FrameInput inputs[3];
    const unsigned held[3] = {held0, held1, held2};
    const unsigned count = mp_replay_archive->Setup().player_count;
    if (count < 2 || count > 3) return 0;
    for (unsigned seat = 0; seat < count; ++seat)
        if (!multiplayer::InputLanes::Capture(held[seat], false, inputs[seat])) return 0;
    return mp_replay_archive->Append(frame, stage, inputs, count) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE int th13_debug_mp_replay_stage_entry(unsigned stage) {
    using namespace th13::sdl;
    if (!app || !app->run || !mp_replay_archive) return 0;
    th13::multiplayer::StageEntry entry;
    entry.Capture(app->run->battle, app->progress, app->run->score_view(), app->game_random.seed(),
                  app->game_random.index(), app->visual.seed(), app->visual.index());
    return mp_replay_archive->AppendStageEntry(stage, entry.Encode()) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_mp_replay_encode(const char* name, unsigned score, unsigned completed) {
    using namespace th13::sdl;
    if (!mp_replay_archive) {
        mp_report = "{\"ok\":0}";
        return mp_report.c_str();
    }
    std::vector<std::uint8_t> wire;
    if (!mp_replay_archive->Encode(wire, name ? name : "mp", score, completed != 0)) {
        mp_report = "{\"ok\":0}";
        return mp_report.c_str();
    }
    mp_report = "{\"ok\":1,\"b64\":\"" + th13::multiplayer::detail::mp_base64_encode(wire.data(), wire.size()) +
                "\",\"frames\":" + std::to_string(mp_replay_archive->FrameCount()) + "}";
    return mp_report.c_str();
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_mp_replay_load(const char* b64) {
    using namespace th13;
    const auto wire = th13::multiplayer::detail::mp_base64_decode(b64);
    if (!mp_replay_loaded) mp_replay_loaded = std::make_unique<multiplayer::ReplayArchive>();
    if (wire.empty() || !mp_replay_loaded->Load(wire.data(), wire.size())) {
        mp_report = "{\"ok\":0}";
        return mp_report.c_str();
    }
    const auto& setup = mp_replay_loaded->Setup();
    mp_report = "{\"ok\":1,\"frames\":" + std::to_string(mp_replay_loaded->FrameCount()) +
                ",\"chapters\":" + std::to_string(mp_replay_loaded->ChapterCount()) +
                ",\"completed\":" + (mp_replay_loaded->Completed() ? "1" : "0") +
                ",\"score\":" + std::to_string(mp_replay_loaded->Score()) +
                ",\"session\":" + std::to_string(setup.session_id) + "}";
    return mp_report.c_str();
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_mp_replay_frame(unsigned frame) {
    using namespace th13;
    if (!mp_replay_loaded) {
        mp_report = "{\"ok\":0}";
        return mp_report.c_str();
    }
    const auto* decoded = mp_replay_loaded->FrameAt(frame);
    if (!decoded) {
        mp_report = "{\"ok\":0}";
        return mp_report.c_str();
    }
    mp_report = "{\"ok\":1,\"held\":[" + std::to_string(decoded->at(0).buttons) + "," +
                std::to_string(decoded->at(1).buttons) + "," + std::to_string(decoded->at(2).buttons) + "]}";
    return mp_report.c_str();
}

EMSCRIPTEN_KEEPALIVE int th13_debug_mp_stage_entry_capture() {
    using namespace th13::sdl;
    if (!app || !app->run) return 0;
    mp_stage_entry.Capture(app->run->battle, app->progress, app->run->score_view(), app->game_random.seed(),
                           app->game_random.index(), app->visual.seed(), app->visual.index());
    mp_stage_entry_valid = true;
    return 1;
}

EMSCRIPTEN_KEEPALIVE int th13_debug_mp_stage_entry_restore() {
    using namespace th13::sdl;
    if (!app || !app->run || !mp_stage_entry_valid) return 0;
    std::uint16_t gameSeed = 0, visualSeed = 0;
    std::uint32_t gameIndex = 0, visualIndex = 0;
    mp_stage_entry.Restore(app->run->battle, app->progress, app->run->score_view(), gameSeed, gameIndex, visualSeed,
                           visualIndex);
    app->game_random.reseed_restore(gameSeed);
    app->game_random.set_index(gameIndex);
    app->visual.reseed_restore(visualSeed);
    app->visual.set_index(visualIndex);
    return 1;
}

EMSCRIPTEN_KEEPALIVE int th13_debug_mp_spectator_begin_publish() {
    using namespace th13::sdl;
    if (!app) return 0;
    th13::multiplayer::SessionSetup setup;
    if (th13::multiplayer::session.Active()) {
        setup = th13::multiplayer::session.Setup();
    } else {
        setup.version = th13::multiplayer::SessionSetup::Version;
        setup.mode = 2;
        setup.session_id = 0x9001'0000'0000'0002ull;
        setup.player_count = 2;
        setup.local_player = 0;
        setup.difficulty = 2;
        setup.seed = 47932;
        setup.automatic = false;
        setup.requested_delay = setup.input_delay = 2;
        setup.selections = {0, 0, 0};
        setup.build = {1, 2, 3, 4};
    }
    return mp_spectator.BeginPublish(setup) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE int th13_debug_mp_spectator_begin_viewing() {
    using namespace th13::sdl;
    if (!app) return 0;
    th13::multiplayer::SessionSetup setup;
    if (th13::multiplayer::session.Active()) {
        setup = th13::multiplayer::session.Setup();
    } else {
        setup.version = th13::multiplayer::SessionSetup::Version;
        setup.mode = 2;
        setup.session_id = 0x9001'0000'0000'0002ull;
        setup.player_count = 2;
        setup.local_player = 1;
        setup.difficulty = 2;
        setup.seed = 47932;
        setup.automatic = false;
        setup.requested_delay = setup.input_delay = 2;
        setup.selections = {0, 0, 0};
        setup.build = {1, 2, 3, 4};
    }
    return mp_spectator.BeginViewing(setup) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE int th13_debug_mp_spectator_record(unsigned frame, unsigned stage, unsigned held0, unsigned held1,
                                                        int with_entry) {
    using namespace th13::sdl;
    if (!app || !app->run) return 0;
    Netplay::FrameInput inputs[3];
    const unsigned held[3] = {held0, held1, 0};
    const unsigned count = 2;
    for (unsigned seat = 0; seat < 2; ++seat)
        if (!th13::multiplayer::InputLanes::Capture(held[seat], false, inputs[seat])) return 0;
    const std::vector<std::uint8_t>* entry = nullptr;
    if (with_entry) {
        th13::multiplayer::StageEntry capture;
        capture.Capture(app->run->battle, app->progress, app->run->score_view(), app->game_random.seed(),
                        app->game_random.index(), app->visual.seed(), app->visual.index());
        static std::vector<std::uint8_t> entryBytes;
        entryBytes = capture.Encode();
        entry = &entryBytes;
    }
    return mp_spectator.RecordFrame(frame, stage, inputs, count, entry) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_mp_spectator_publish(unsigned confirmed, unsigned simulated) {
    using namespace th13;
    mp_spectator_wire.clear();
    const auto send = [&](const std::vector<std::uint8_t>& packet) {
        const std::uint32_t length = std::uint32_t(packet.size());
        mp_spectator_wire.push_back(std::uint8_t(length));
        mp_spectator_wire.push_back(std::uint8_t(length >> 8));
        mp_spectator_wire.push_back(std::uint8_t(length >> 16));
        mp_spectator_wire.push_back(std::uint8_t(length >> 24));
        mp_spectator_wire.insert(mp_spectator_wire.end(), packet.begin(), packet.end());
        return true;
    };
    const auto inputsAt = [&](std::uint32_t frame, std::array<Netplay::FrameInput, 3>& out) { return true; };
    const bool ok = mp_spectator.Publish(confirmed, simulated, inputsAt, send);
    mp_report = "{\"ok\":" + std::string(ok ? "1" : "0") +
                ",\"published\":" + std::to_string(mp_spectator.Published()) + ",\"failure\":\"" +
                mp_spectator.PublishFailureName() + "\"" + ",\"b64\":\"" +
                th13::multiplayer::detail::mp_base64_encode(mp_spectator_wire.data(), mp_spectator_wire.size()) + "\"}";
    return mp_report.c_str();
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_mp_spectator_receive(const char* b64) {
    using namespace th13;
    const auto wire = th13::multiplayer::detail::mp_base64_decode(b64);
    if (wire.empty()) {
        mp_report = "{\"ok\":0}";
        return mp_report.c_str();
    }
    unsigned received = 0, events = 0;
    std::size_t at = 0;
    while (at + 4 <= wire.size()) {
        const std::uint32_t length = std::uint32_t(wire[at]) | std::uint32_t(wire[at + 1]) << 8 |
                                     std::uint32_t(wire[at + 2]) << 16 | std::uint32_t(wire[at + 3]) << 24;
        at += 4;
        if (!length || at + length > wire.size()) break;
        if (Netplay::IsSpectatorStageEvent(wire.data() + at, length)) {
            Netplay::SpectatorStageEvent event;
            if (Netplay::DecodeSpectatorStageEvent(wire.data() + at, length, &event) &&
                mp_spectator.ReceiveEvent(event))
                ++events;
        } else {
            Netplay::SpectatorFramePacket packet;
            if (Netplay::DecodeSpectatorFramePacket(wire.data() + at, length, &packet) &&
                mp_spectator.ReceiveFrame(packet))
                ++received;
        }
        at += length;
    }
    mp_report = "{\"ok\":1,\"events\":" + std::to_string(events) + ",\"frames\":" + std::to_string(received) +
                ",\"received\":" + std::to_string(mp_spectator.Received()) + "}";
    return mp_report.c_str();
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_mp_spectator_state() {
    using namespace th13;
    mp_report = "{\"publishing\":" + std::string(mp_spectator.Publishing() ? "1" : "0") +
                ",\"viewing\":" + std::string(mp_spectator.Viewing() ? "1" : "0") +
                ",\"published\":" + std::to_string(mp_spectator.Publishing() ? mp_spectator.Published() : 0) +
                ",\"received\":" + std::to_string(mp_spectator.Received()) +
                ",\"position\":" + std::to_string(mp_spectator.Position()) +
                ",\"seeking\":" + std::string(mp_spectator.Seeking() ? "1" : "0") +
                ",\"paused\":" + std::string(mp_spectator.Paused() ? "1" : "0") +
                ",\"ended\":" + std::string(mp_spectator.HistoryEnded() ? "1" : "0") + "}";
    return mp_report.c_str();
}

namespace {
bool mp_recording = false;
std::uint32_t mp_recorded_through = 0;
unsigned mp_saved_slot = 0;
std::string mp_saved_name;
int save_mp_replay(unsigned slot, const char* name);

#ifdef __EMSCRIPTEN__
// clang-format off
EM_JS(bool, th13_mp_persist_replay, (const unsigned char* bytes, int size,
                                    const char* name), {
    try {
        const data = HEAPU8.slice(bytes, bytes + size);
        return Module.onReplaySaved?.(UTF8ToString(name), data) !== false;
    } catch (_) { return false; }
});
// clang-format on
#endif
bool mp_start_playback();

struct MpBenchStart {
    th13::i32 stage = 0;
    th13::i32 spell_slot = -1;
    bool protect_all_seats = false;
} mp_bench_start;
}

EMSCRIPTEN_KEEPALIVE void th13_debug_mp_bench_start(unsigned stage, int spell_slot, unsigned protect_all_seats) {
    mp_bench_start = {th13::i32(stage), th13::i32(spell_slot), protect_all_seats != 0};
}

EMSCRIPTEN_KEEPALIVE void th13_debug_mp_slowdown(unsigned enabled) {
    th13::sdl::mp_slowdown_enabled = enabled != 0;
    th13::sdl::mp_governor.reset();
}

EMSCRIPTEN_KEEPALIVE int th13_mp_configure(const unsigned* words, unsigned count) {
    using namespace th13::sdl;
    if (!app || !words || !count) return 0;
    if (!th13::multiplayer::session.Configure(words, count)) return 0;
    reset_mp_accumulator();
    mp_started = mp_playback_pending = mp_spectator_mode = mp_recording = false;
    mp_recorded_through = 0;
    mp_saved_slot = 0;
    mp_saved_name.clear();
    app->multiplayer_replay_available = {};
    app->multiplayer_replay_save = {};
    mp_spectator_stage = 0;
    last_error.clear();
    return 1;
}

EMSCRIPTEN_KEEPALIVE int th13_mp_connect(const char* relay_url) {
    if (!relay_url) return 0;
    return th13::multiplayer::session.Connect(relay_url) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE int th13_mp_spectator_connect(const char* relay_url, const char* id) {
    if (!relay_url || !id || !*id) return 0;
    if (!th13::multiplayer::session.ConnectSpectator(relay_url, id)) return 0;
    mp_spectator_mode = true;
    return 1;
}

EMSCRIPTEN_KEEPALIVE int th13_mp_pump(double now_ms) {
    if (!std::isfinite(now_ms) || now_ms < 0) return 0;
    return th13::multiplayer::session.Pump(std::uint64_t(now_ms)) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE int th13_mp_start() {
    using namespace th13::sdl;
    LoadingWork work(8);
    bool waiting = false;
    const int result = [&]() -> int {
        if (!app || !app->initialized || running) return 0;
        auto& session = th13::multiplayer::session;
        if (!session.Active()) {
            last_error = "TH13 multiplayer session is not configured";
            return 0;
        }
        if (mp_started) return 1;
        if (mp_playback_pending) {
            if (!mp_start_playback()) return 0;
            mp_started = true;
            return 1;
        }
        if (mp_spectator_mode) {
            mp_started = true;
            return 1;
        }
        if (!session.Pump(std::uint64_t(emscripten_get_now()))) {
            last_error = session.Error();
            return 0;
        }
        session.MarkLocalReady();
        if (!session.CanStart()) {
            const char* error = session.Error();
            waiting = (!error || !*error) && last_error.empty() && app->failure.empty();
            if (waiting) work.source = 10;
            return 0;
        }
        if (!session.Start()) return 0;
        mp_room_preview = false;
        session.diagnostic_host = [] {
            th13::multiplayer::HostState state;
            return app->capture_multiplayer_host_state(state)
                       ? th13::multiplayer::WorldState::diagnostic_host_detail(state)
                       : std::string{};
        };
        const auto& setup = session.Setup();
        const std::array<th13::i32, 3> characters = {th13::i32(setup.selections[0]), th13::i32(setup.selections[1]),
                                                     th13::i32(setup.selections[2])};
        if (!app->configure_multiplayer(th13::i32(setup.player_count), characters, th13::i32(setup.local_player))) {
            last_error = "TH13 multiplayer seat configuration rejected";
            return 0;
        }
        app->mp_challenge = setup.challenge;
        app->mp_keep_bullets_on_miss = setup.keep_bullets_on_miss;
        app->progress.run_mode_flags = 0;
        app->progress.practice_mode = 0;
        app->progress.selected_spell_id = -1;
        const th13::i32 starting_stage = mp_bench_start.stage > 0 ? mp_bench_start.stage
                                         : setup.difficulty == 4  ? 7
                                                                  : 1;
        app->progress.character = th13::i32(setup.selections[0]);
        app->progress.difficulty = th13::i32(setup.difficulty);
        if (mp_bench_start.spell_slot >= 0) {
            const auto* card =
                app->spell_catalog.variant(starting_stage, mp_bench_start.spell_slot, th13::i32(setup.difficulty));
            if (!card) {
                last_error = "TH13 multiplayer bench spell card unavailable";
                return 0;
            }
            app->progress.selected_spell_id = card->id;
            app->progress.run_mode_flags = (app->progress.run_mode_flags & ~0x70u) | 0x20u;
        }
        if (!app->start_game(starting_stage)) {
            last_error = app->failure;
            return 0;
        }
        app->pending_destination = -1;
        app->progress.character = th13::i32(setup.selections[0]);
        app->progress.difficulty = th13::i32(setup.difficulty);
        app->progress.stage = app->progress.starting_stage = starting_stage;
        app->selected_stage = starting_stage;
        app->frames = 0;
        app->game_random.reseed_restore(static_cast<std::uint16_t>(setup.seed));
        app->visual.reseed_restore(static_cast<std::uint16_t>(setup.seed));
        if (!app->begin_run()) {
            last_error = app->failure.empty() ? "TH13 multiplayer begin_run failed" : app->failure;
            return 0;
        }
        if (app->run) app->run->battle.protect_all_seats = mp_bench_start.protect_all_seats;
        if (!mp_replay_archive) mp_replay_archive = std::make_unique<th13::multiplayer::ReplayArchive>();
        mp_replay_archive->Clear();
        if (!mp_replay_archive->Begin(setup, std::uint64_t(emscripten_date_now()))) {
            last_error = "TH13 multiplayer replay begin failed";
            return 0;
        }
        mp_recorded_through = 0;
        mp_recording = true;
        mp_saved_slot = 0;
        mp_saved_name.clear();
        app->multiplayer_replay_available = [] { return mp_replay_archive && mp_replay_archive->FrameCount() != 0; };
        app->multiplayer_replay_save = [](th13::i32 slot, const std::string& name) {
            return save_mp_replay(unsigned(slot), name.c_str()) != 0;
        };
        session.stage_source = [] { return std::uint32_t(std::max(1, app->progress.stage)); };
        session.confirmed_sink = [](std::uint32_t frame, std::uint32_t stage,
                                    const std::array<Netplay::FrameInput, 3>& inputs) {
            if (!mp_recording || !mp_replay_archive) return;
            if (frame < mp_replay_archive->FrameCount()) return;
            if (!mp_replay_archive->Append(frame, stage, inputs.data(), mp_replay_archive->Setup().player_count))
                return;
            mp_recorded_through = frame;
        };
        session.spectator_sink = [](std::uint32_t frame, const std::array<Netplay::FrameInput, 3>& inputs) {
            auto& runtime = th13::multiplayer::session.Runtime();
            if (runtime.Spectator() || runtime.SpectatorPublishFailed()) return true;
            if (!th13::sdl::app || !th13::sdl::app->run) return true;
            const std::uint32_t stage = std::uint32_t(std::max(1, th13::sdl::app->progress.stage));
            const std::vector<std::uint8_t>* initializer = nullptr;
            static std::vector<std::uint8_t> entryBytes;
            if (mp_recording && mp_replay_archive && !mp_replay_archive->StageEntryAt(stage)) {
                th13::multiplayer::StageEntry capture;
                capture.Capture(app->run->battle, app->progress, app->run->score_view(), app->game_random.seed(),
                                app->game_random.index(), app->visual.seed(), app->visual.index());
                if (!mp_replay_archive->AppendStageEntry(stage, capture.Encode())) return false;
            }
            const auto need = runtime.SpectatorCheckpointNeeded(frame, stage);
            if (need == Netplay::SpectatorStagePublisher::CheckpointRequirement::Required) {
                th13::multiplayer::StageEntry capture;
                capture.Capture(th13::sdl::app->run->battle, th13::sdl::app->progress,
                                th13::sdl::app->run->score_view(), th13::sdl::app->game_random.seed(),
                                th13::sdl::app->game_random.index(), th13::sdl::app->visual.seed(),
                                th13::sdl::app->visual.index());
                entryBytes = capture.Encode();
                if (entryBytes.empty()) return false;
                initializer = &entryBytes;
            } else if (need == Netplay::SpectatorStagePublisher::CheckpointRequirement::MissingPredecessor) {
                return false;
            }
            return runtime.RecordSpectatorFrame(frame, stage, inputs.data(), th13::multiplayer::session.PlayerCount(),
                                                initializer);
        };
        mp_started = true;
        return 1;
    }();
    return work.finish(result, result != 0 || waiting);
}

bool mp_start_playback() {
    using namespace th13::sdl;
    auto& session = th13::multiplayer::session;
    const auto* archive = mp_replay_loaded.get();
    if (!archive || !archive->Loaded() || !archive->FrameCount()) {
        last_error = "TH13 multiplayer replay is not loaded";
        return false;
    }
    const auto& setup = archive->Setup();
    const auto stage = archive->Info().chapters[0].label;
    th13::multiplayer::StageEntry entry;
    const auto* bytes = archive->StageEntryAt(stage);
    if (bytes && !th13::multiplayer::StageEntry::Decode(bytes->data(), bytes->size(), entry)) {
        last_error = "TH13 multiplayer replay stage entry is invalid";
        return false;
    }
    if (!mp_restore_history_root(setup, stage, bytes ? &entry : nullptr, archive->UsesLegacyPickupRewards()))
        return false;
    app->mp_legacy_stage_positions = archive->UsesLegacyStagePositions();
    app->mp_legacy_boss_mitigation = archive->UsesLegacyBossMitigation();
    mp_playback_frame = 0;
    if (mp_playback_chapter == 0) mp_playback_chapter = stage;
    mp_recording = false;
    return true;
}

EMSCRIPTEN_KEEPALIVE const unsigned* th13_mp_status() {
    static std::array<unsigned, 24> status{};
    status.fill(0);
    auto& session = th13::multiplayer::session;
    const auto& setup = session.Setup();
    status[0] = 1;
    status[1] = session.Active() ? 1u : 0u;
    status[2] = session.CanStart() ? 1u : 0u;
    status[3] = session.SimFrame();
    status[4] = session.ConfirmedThrough();
    status[5] = session.PlayerCount();
    status[6] = session.LocalSeat();
    status[7] = setup.input_delay;
    status[8] = setup.prediction_limit;
    status[9] = session.Connected() ? 1u : 0u;
    status[10] = mp_spectator_mode ? 1u : 0u;
    status[11] = mp_playback_pending && mp_started ? 1u : 0u;
    status[12] = 0;
    status[13] = 0;
    status[14] = mp_spectator_mode ? session.Runtime().SpectatorBacklog() : 0u;
    status[15] = session.PacketsSent();
    status[16] = session.PacketsReceived();
    status[17] = session.RepairsSent();
    status[18] = session.PacketsIgnored();
    status[19] = session.TransportOpen() ? 1u : 0u;
    status[20] = session.RouteCode();
    status[21] = 0;
    status[22] = session.Runtime().SpectatorPublishFailed() ? 1u : 0u;
    {
        const char* error = session.Error();
        status[23] = error && *error ? 1u : 0u;
    }
    return status.data();
}

EMSCRIPTEN_KEEPALIVE const unsigned* th13_mp_rollback_status() {
    using namespace th13::sdl;
    static std::array<unsigned, 9> out{};
    const auto& session = th13::multiplayer::session;
    const auto& setup = session.Setup();
    out[0] = 1;
    out[1] = setup.mode == 2 ? 1u : 0u;
    out[2] = session.Restores();
    out[3] = session.Resimulated();
    out[4] = session.LastRollbackDistance();
    out[5] = session.MaxRollbackDistance();
    out[6] = session.HasRollbackRequest() ? 1u : 0u;
    out[7] = session.Correcting() ? session.CorrectionEnd() : 0u;
    out[8] = unsigned(std::lround(presentation_fps * 100.f));
    return out.data();
}

EMSCRIPTEN_KEEPALIVE const char* th13_mp_error() {
    const char* error = th13::multiplayer::session.Error();
    if (error && *error) return error;
    if (th13::sdl::app && !th13::sdl::app->failure.empty()) return th13::sdl::app->failure.c_str();
    if (!th13::sdl::last_error.empty()) return th13::sdl::last_error.c_str();
    return error ? error : "";
}

EMSCRIPTEN_KEEPALIVE const char* th13_mp_diagnostic_report() {
    static std::string report;
    const auto& app = th13::sdl::app;
    const auto* battle = app && app->run ? &app->run->battle : nullptr;
    report = th13::multiplayer::session.DiagnosticJson();
    report.pop_back();
    std::ostringstream out;
    out.imbue(std::locale::classic());
    out << ",\"runtime\":{\"running\":" << (th13::sdl::running ? "true" : "false")
        << ",\"suspended\":" << (th13::sdl::suspended ? "true" : "false")
        << ",\"exiting\":" << (app && app->exiting ? "true" : "false")
        << ",\"intervalScale\":" << th13::multiplayer::session.IntervalScale()
        << ",\"clockOwed\":" << th13::sdl::mp_schedule.owed() << '}'
        << ",\"presentationSeat\":" << (battle ? battle->active_seat + 1 : 0) << ",\"seatsAtStop\":[";
    if (battle)
        for (int i = 0; i < battle->seat_count && i < 3; ++i) {
            if (i) out << ',';
            const auto& player = battle->seats[i];
            out << "{\"seat\":" << i + 1 << ",\"character\":" << battle->seat_characters[i] << ",\"x\":";
            if (std::isfinite(player.x))
                out << player.x;
            else
                out << "null";
            out << ",\"y\":";
            if (std::isfinite(player.y))
                out << player.y;
            else
                out << "null";
            out << ",\"initialized\":" << (player.initialized ? "true" : "false")
                << ",\"ghost\":" << (player.ghost ? "true" : "false")
                << ",\"dead\":" << (player.dead ? "true" : "false") << ",\"phase\":" << int(player.phase)
                << ",\"held\":" << battle->seat_inputs[i].held << ",\"shots\":" << player.shots.size()
                << ",\"power\":" << battle->seat_huds[i].power << '}';
        }
    out << "]}";
    report += out.str();
    return report.c_str();
}

EMSCRIPTEN_KEEPALIVE void th13_mp_audit(unsigned enabled) { th13::multiplayer::session.SetAudit(enabled != 0); }
EMSCRIPTEN_KEEPALIVE const char* th13_mp_audit_take() {
    static std::string report;
    report = th13::multiplayer::session.TakeAuditJson();
    return report.c_str();
}
EMSCRIPTEN_KEEPALIVE const char* th13_mp_timing_take() {
    static std::string report;
    report = th13::multiplayer::session.TakeTimingJson();
    return report.c_str();
}

EMSCRIPTEN_KEEPALIVE void th13_mp_close() {
    using namespace th13::sdl;
    reset_mp_accumulator();
    th13::multiplayer::session.confirmed_sink = nullptr;
    th13::multiplayer::session.spectator_sink = nullptr;
    th13::multiplayer::session.Stop();
    if (app) {
        app->multiplayer_replay_available = {};
        app->multiplayer_replay_save = {};
        app->mp_seat_count = 0;
        app->mp_characters = {0, 0, 0};
        app->mp_local_seat = 0;
    }
    mp_started = mp_playback_pending = mp_spectator_mode = mp_recording = false;
    mp_recorded_through = 0;
    mp_playback_frame = mp_playback_chapter = 0;
    last_error.clear();
}

EMSCRIPTEN_KEEPALIVE const char* th13_mp_spectator_state() {
    if (!mp_spectator_mode) return nullptr;
    return th13::multiplayer::session.Runtime().SpectatorState();
}

EMSCRIPTEN_KEEPALIVE int th13_mp_spectator_control(const char* run, const char* action, unsigned frame) {
    if (!mp_spectator_mode || !run || !action) return 0;
    return th13::multiplayer::session.Runtime().SpectatorControl(run, action, frame) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE int th13_mp_spectator_result(const unsigned char* bytes, unsigned size) {
    using namespace th13::sdl;
    auto& runtime = th13::multiplayer::session.Runtime();
    if (!mp_spectator_mode || !bytes || !size) return 0;
    auto parsed = std::make_unique<th13::multiplayer::ReplayArchive>();
    if (!parsed->Load(bytes, size)) return 0;
    const auto& setup = parsed->Setup();
    const auto& sessionSetup = th13::multiplayer::session.Setup();
    for (std::size_t index = 0; index < 4; ++index)
        if (setup.build[index] != sessionSetup.build[index]) return 0;
    mp_replay_loaded = std::move(parsed);
    return 1;
}

EMSCRIPTEN_KEEPALIVE int th13_mp_spectator_save_replay() {
    using namespace th13::sdl;
    if (!mp_spectator_mode) return 0;
    auto archive = std::make_unique<th13::multiplayer::ReplayArchive>();
    if (!th13::multiplayer::session.Runtime().StagePlaybackReady()) return 0;
    auto& playback = th13::multiplayer::session.Runtime().StagePlayback();
    const auto& segments = playback.History().Segments();
    if (segments.empty()) return 0;
    const auto& setup = th13::multiplayer::session.Setup();
    if (!archive->Begin(setup, std::uint64_t(emscripten_date_now()))) return 0;
    for (const auto& segment : segments) {
        const std::vector<std::uint8_t>* initializer = nullptr;
        if (!segment.checkpoint.state.empty()) initializer = &segment.checkpoint.state;
        if (initializer && !archive->AppendStageEntry(segment.checkpoint.stage, *initializer)) return 0;
        for (const auto& packet : segment.inputs)
            if (!archive->Append(packet.frame, segment.checkpoint.stage, packet.inputs.data(), packet.playerCount))
                return 0;
    }
    std::vector<std::uint8_t> wire;
    std::uint32_t score = 0;
    if (app) score = std::uint32_t(app->run ? app->run->score_view().score : app->selection_score.score);
    const std::string name = "SPECTATOR";
    if (!archive->Encode(wire, name.c_str(), score, playback.History().Ended())) return 0;
    SDL_CreateDirectory("/save");
    SDL_CreateDirectory("/save/replay");
    char path[64]{}, file_name[32]{};
    for (unsigned slot = 1; slot <= 99; ++slot) {
        std::snprintf(file_name, sizeof file_name, "th13mp_%02u.rpy", slot);
        std::snprintf(path, sizeof path, "/save/replay/%s", file_name);
        SDL_IOStream* existing = SDL_IOFromFile(path, "rb");
        if (!existing) break;
        SDL_CloseIO(existing);
        path[0] = 0;
    }
    if (!path[0]) {
        std::snprintf(file_name, sizeof file_name, "th13mp_01.rpy");
        std::snprintf(path, sizeof path, "/save/replay/%s", file_name);
    }
    SDL_IOStream* file = SDL_IOFromFile(path, "wb");
    if (!file) return 0;
    const bool written = SDL_WriteIO(file, wire.data(), wire.size()) == wire.size();
    const bool closed = SDL_CloseIO(file);
    if (!written || !closed) return 0;
#ifdef __EMSCRIPTEN__
    const bool persisted = th13_mp_persist_replay(wire.data(), int(wire.size()), file_name);
    return persisted ? 1 : 0;
#else
    return 1;
#endif
}

EMSCRIPTEN_KEEPALIVE const char* eagler_replay_inspect(const unsigned char* bytes, unsigned size) {
    using namespace th13::sdl;
    static std::string report;
    if (!bytes || !size) return nullptr;
    th13::multiplayer::ReplayArchive archive;
    if (!archive.Load(bytes, size)) return nullptr;
    eagler::replay::Description description;
    description.encoded_name(archive.Name().c_str(), unsigned(archive.Name().size()));
    const auto& setup = archive.Setup();
    description.number("character", setup.selections[0]);
    if (setup.player_count >= 2 && setup.player_count <= 3) {
        std::string characters = "[";
        for (unsigned seat = 0; seat < setup.player_count; ++seat)
            characters += (seat ? "," : "") + std::to_string(setup.selections[seat]);
        description.field("characters", characters + "]");
    }
    description.number("difficulty", setup.difficulty);
    description.number("score", double(archive.Score()) * 10);
    description.number("timestamp", std::floor(double(archive.Timestamp()) / 1000.));
    const auto& info = archive.Info();
    for (unsigned index = 0; index < info.chapterCount; ++index) {
        const auto stage = info.chapters[index].label;
        const auto number = stage % 8;
        description.stage_without_score(stage, 0, number == 7 ? "EXTRA" : "STAGE " + std::to_string(number));
    }
    report = description.finish();
    return report.c_str();
}

EMSCRIPTEN_KEEPALIVE const unsigned* eagler_replay_state() {
    using namespace th13::sdl;
    static std::array<unsigned, 8> state{};
    state.fill(0);
    const auto* archive = mp_replay_loaded.get();
    if (!mp_playback_pending || !mp_started || !archive || !archive->Loaded()) return state.data();
    state[0] = mp_playback_frame;
    state[1] = archive->FrameCount();
    state[2] = mp_playback_frame >= archive->FrameCount() ? 1u : 0u;
    state[3] = app ? unsigned(std::max(1, app->progress.stage)) : 1u;
    state[4] = mp_playback_chapter;
    for (unsigned i = 0; i < archive->Info().chapterCount; ++i)
        if (archive->Info().chapters[i].label == mp_playback_chapter) state[5] = archive->Info().chapters[i].firstFrame;
    state[6] = mp_playback_seek_target;
    state[7] = mp_playback_seek_target != Netplay::INVALID_FRAME ? 1u : 0u;
    return state.data();
}

EMSCRIPTEN_KEEPALIVE int th13_mp_replay_pause(int paused) {
    if (!mp_playback_pending || !mp_started) return 0;
    mp_playback_paused = paused != 0;
    if (mp_playback_paused) mp_playback_seek_target = Netplay::INVALID_FRAME;
    return 1;
}

EMSCRIPTEN_KEEPALIVE int th13_mp_replay_rate(unsigned frames_per_tick) {
    if (!mp_playback_pending || !mp_started) return 0;
    if (frames_per_tick > 8) return 0;
    mp_playback_rate = frames_per_tick == 0 ? 1 : frames_per_tick;
    return 1;
}

EMSCRIPTEN_KEEPALIVE int th13_mp_replay_seek_stage(unsigned stage) {
    using namespace th13::sdl;
    if (!mp_playback_pending || !mp_started) return 0;
    const auto* archive = mp_replay_loaded.get();
    if (!archive || !archive->Loaded()) return 0;
    const auto& info = archive->Info();
    for (unsigned index = 0; index < info.chapterCount; ++index)
        if (info.chapters[index].label == stage) {
            mp_playback_chapter = stage;
            if (!mp_start_playback()) return 0;
            const unsigned target = info.chapters[index].firstFrame;
            mp_playback_seek_target = target ? target : Netplay::INVALID_FRAME;
            return 1;
        }
    return 0;
}

EMSCRIPTEN_KEEPALIVE int th13_mp_replay_seek(unsigned frame) {
    using namespace th13::sdl;
    if (!mp_playback_pending || !mp_started || !mp_replay_loaded || frame > mp_replay_loaded->FrameCount()) return 0;
    const auto& info = mp_replay_loaded->Info();
    if (!info.chapterCount) return 0;
    unsigned chapter = info.chapters[0].label;
    for (unsigned i = 0; i < info.chapterCount; ++i)
        if (info.chapters[i].firstFrame <= frame) chapter = info.chapters[i].label;
    if (!th13_mp_replay_seek_stage(chapter)) return 0;
    mp_playback_seek_target = frame ? frame : Netplay::INVALID_FRAME;
    return 1;
}

namespace {
int save_mp_replay(unsigned requested_slot, const char* requested_name) {
    using namespace th13::sdl;
    if (mp_started && !th13::multiplayer::session.Spectator() && !th13::multiplayer::session.Simulating())
        th13::multiplayer::session.Runtime().FinishSpectator(th13::multiplayer::session.SimFrame());
    if (!mp_replay_archive || !mp_replay_archive->FrameCount()) return 1;
    std::vector<std::uint8_t> bytes;
    std::uint32_t score = 0;
    if (app) score = std::uint32_t(app->run ? app->run->score_view().score : app->selection_score.score);
    const bool completed = th13::multiplayer::session.WorldCompleted();
    const std::string name = requested_name                      ? requested_name
                             : !mp_saved_name.empty()            ? mp_saved_name
                             : mp_replay_archive->Name().empty() ? "PLAYER"
                                                                 : mp_replay_archive->Name();
    if (!mp_replay_archive->Encode(bytes, name.c_str(), score, completed)) return 0;
    SDL_CreateDirectory("/save");
    SDL_CreateDirectory("/save/replay");
    char path[64]{}, file_name[32]{};
    const unsigned saved_slot = requested_slot ? requested_slot : mp_saved_slot;
    for (unsigned slot = saved_slot ? saved_slot : 1; slot <= 99; ++slot) {
        std::snprintf(file_name, sizeof file_name, "th13mp_%02u.rpy", slot);
        std::snprintf(path, sizeof path, "/save/replay/%s", file_name);
        if (saved_slot) break;
        SDL_IOStream* existing = SDL_IOFromFile(path, "rb");
        if (!existing) break;
        SDL_CloseIO(existing);
        path[0] = 0;
    }
    if (!path[0]) {
        std::snprintf(file_name, sizeof file_name, "th13mp_01.rpy");
        std::snprintf(path, sizeof path, "/save/replay/%s", file_name);
    }
    SDL_IOStream* file = SDL_IOFromFile(path, "wb");
    if (!file) return 0;
    const bool written = SDL_WriteIO(file, bytes.data(), bytes.size()) == bytes.size();
    const bool closed = SDL_CloseIO(file);
    if (!written || !closed) return 0;
    mp_saved_slot = unsigned(std::strtoul(file_name + 7, nullptr, 10));
    mp_saved_name = name;
#ifdef __EMSCRIPTEN__
    const bool persisted = th13_mp_persist_replay(bytes.data(), int(bytes.size()), file_name);
    if (persisted && mp_started && !th13::multiplayer::session.Spectator() &&
        th13::multiplayer::session.LocalSeat() == 0)
        Netplay::BrowserPeerTransport::ShareSpectatorReplay(bytes.data(), bytes.size(), true);
    return persisted ? 1 : 0;
#else
    return 1;
#endif
}

}
EMSCRIPTEN_KEEPALIVE int th13_mp_replay_save() { return save_mp_replay(0, nullptr); }

EMSCRIPTEN_KEEPALIVE int th13_mp_replay_load(const unsigned char* bytes, unsigned size, const unsigned* build) {
    using namespace th13::sdl;
    if (!app || !bytes || !size || !build) return 0;
    auto parsed = std::make_unique<th13::multiplayer::ReplayArchive>();
    if (!parsed->Load(bytes, size)) {
        last_error = "TH13 multiplayer replay is invalid";
        return 0;
    }
    const auto& setup = parsed->Setup();
    for (std::size_t index = 0; index < 4; ++index) {
        if (setup.build[index] != build[index]) {
            last_error = "TH13 multiplayer replay build mismatch";
            return 0;
        }
    }
    const auto words = th13::multiplayer::EncodeSessionSetup(setup);
    if (!th13::multiplayer::session.Configure(words.data(), words.size())) {
        last_error = "TH13 multiplayer replay session is invalid";
        return 0;
    }
    reset_mp_accumulator();
    mp_replay_loaded = std::move(parsed);
    mp_playback_pending = true;
    mp_started = false;
    mp_playback_frame = 0;
    mp_playback_chapter = 0;
    mp_playback_paused = false;
    mp_playback_seek_target = Netplay::INVALID_FRAME;
    mp_playback_rate = 1;
    last_error.clear();
    return 1;
}

EMSCRIPTEN_KEEPALIVE void th13_mp_wake() {
    using namespace th13::sdl;
    if (th13::multiplayer::session.Simulating())
        th13::multiplayer::session.TraceTiming("wake-received", th13::multiplayer::session.SimFrame(),
                                               mp_work_queued ? 1u : 0u, mp_schedule.owed());
    queue_mp_work(true);
}

EMSCRIPTEN_KEEPALIVE void th13_touch_options(unsigned enabled, unsigned mode, float sensitivity, unsigned two_finger,
                                             unsigned double_tap) {
    auto& touch = th13::sdl::gestures;
    if (touch.enabled != (enabled != 0) || touch.mode != int(mode)) touch.cancel();
    touch.enabled = enabled != 0;
    touch.mode = mode <= 3 ? int(mode) : 0;
    touch.unlimited = touch.mode == 1;
    touch.sensitivity = std::isfinite(sensitivity) ? std::clamp(sensitivity, 1.f, 3.f) : 1.f;
    touch.two_finger = two_finger != 0;
    touch.double_tap = double_tap != 0;
}

EMSCRIPTEN_KEEPALIVE void th13_touch_controls(unsigned enabled, unsigned fire, unsigned focus, unsigned bomb,
                                              unsigned escape, float x, float y) {
    auto& touch = th13::sdl::gestures;
    if (touch.enabled != (enabled != 0)) touch.cancel();
    touch.enabled = enabled != 0;
    touch.controls(fire != 0, focus != 0, bomb, escape, std::isfinite(x) ? x : 0, std::isfinite(y) ? y : 0);
}

EMSCRIPTEN_KEEPALIVE void th13_touch(unsigned type, int id, float x, float y) {
    using namespace th13::sdl;
    if (type > 2 || !std::isfinite(x) || !std::isfinite(y)) return;
    if (th13::multiplayer::session.Simulating()) {
        const auto frame = th13::multiplayer::session.CaptureFrame();
        th13::multiplayer::session.TraceTiming("touch-device", frame, type, th13::u32(id));
        th13::multiplayer::session.TraceTiming("touch-device-xy", frame, timing_float_bits(x), timing_float_bits(y));
    }
    gestures.pointer(int(type), id, x, y, SDL_GetTicks(), touch_state(), (controls.held & 8) != 0);
}

EMSCRIPTEN_KEEPALIVE void th13_touch_cancel() {
    using namespace th13::sdl;
    gestures.cancel();
    if (app && app->run) app->run->battle.touch = {};
}

EMSCRIPTEN_KEEPALIVE void th13_keys_clear() { th13::sdl::clear_inputs(); }

EMSCRIPTEN_KEEPALIVE void th13_loop_stop() {
    using namespace th13::sdl;
    running = false;
    ++loop_epoch;
    previous_frame = -1;
    reset_cadence();
    reset_presentation_window();
    reset_mp_accumulator();
    clear_inputs();
    if (controller) {
        SDL_CloseJoystick(controller);
        controller = nullptr;
    }
    if (app) app->audio_device.suspend(true);
}

EMSCRIPTEN_KEEPALIVE void th13_debug_audio_resume() {
    using namespace th13::sdl;
    if (app) app->audio_device.suspend(false);
}

EMSCRIPTEN_KEEPALIVE void th13_loop_pause(unsigned enabled) {
    using namespace th13::sdl;
    suspended = enabled != 0;
    previous_frame = -1;
    reset_cadence();
    reset_presentation_window();
    reset_mp_accumulator();
    if (app) app->audio_device.suspend(suspended);
}

EMSCRIPTEN_KEEPALIVE void th13_loop_start() {
    using namespace th13::sdl;
    if (running || !app || !app->initialized) return;
    if (app->loading_frames >= 120) {
        app->loading_startup = false;
        app->loading_debt = 0;
    }
    running = true;
    suspended = false;
    previous_frame = -1;
    reset_cadence();
    reset_presentation_window();
    mp_clock.reset();
    clear_inputs();
    initialize_controller();
    app->audio_device.suspend(false);
    emscripten_request_animation_frame_loop(
        [](double time, void* epoch) -> EM_BOOL {
            if (!running || uintptr_t(epoch) != loop_epoch) return EM_FALSE;
            const double delta = previous_frame < 0 ? 0 : (time - previous_frame) / 1000.;
            previous_frame = time;
            mp_tasks_remaining = 2;
            const double elapsed = mp_clock.elapsed(emscripten_get_now());
            return drive_one_frame(th13::multiplayer::session.Active() || mp_room_preview ? elapsed : delta, 1)
                       ? EM_TRUE
                       : EM_FALSE;
        },
        reinterpret_cast<void*>(uintptr_t(++loop_epoch)));
}

EMSCRIPTEN_KEEPALIVE int th13_replay_import(const unsigned char* data, unsigned size, const char* name) {
    using namespace th13::sdl;
    if (!app || !data || size == 0 || !name) return 0;
    return app->import_replay_bytes(data, size_t(size), std::string(name)) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE int th13_replay_import_hint() {
    using namespace th13::sdl;
    return app && app->title ? app->title->state.replay_import_hint : -1;
}

EMSCRIPTEN_KEEPALIVE const char* th13_replay_import_error() {
    using namespace th13::sdl;
    static std::string error;
    error = app ? app->replay_import_error() : std::string{};
    return error.c_str();
}

EMSCRIPTEN_KEEPALIVE unsigned th13_debug_replay(const char* name, unsigned stage_index) {
    using namespace th13::sdl;
    if (running || !app || !app->initialized || !name) return 0;
    std::vector<th13::u8> bytes;
    if (!app->read(name, bytes)) {
        last_error = app->failure;
        return 0;
    }
    app->visual.reseed_restore(app->boot_seed);
    for (int step = 0; step < 256; ++step)
        (void)app->visual.step16();
    (void)app->visual.next32();
    (void)app->visual.next32();
    if (!app->draw_loading(false)) {
        last_error = app->failure;
        return 0;
    }
    for (unsigned frame = 0; frame < th13::sdl::ApplicationState::kDebugLoadingFrames; ++frame)
        if (!app->advance_loading_caption()) {
            last_error = app->failure;
            return 0;
        }
    if (!app->begin_replay_run(bytes.data(), bytes.size(), stage_index)) {
        last_error = app->failure;
        return 0;
    }
    clear_inputs();
    reset_cadence();
    return app->replay_cursor ? app->replay_cursor->stage() ? app->replay_cursor->stage()->frame_count : 0 : 0;
}

EMSCRIPTEN_KEEPALIVE int th13_debug_staff_texture() {
    using namespace th13::sdl;
    if (!app || !app->initialized) return 0;
    std::vector<th13::u8> bytes;
    if (!app->read("staff.anm", bytes)) {
        last_error = "staff.anm read failed";
        return 0;
    }
    if (app->animations.resource(21)) app->animations.unload(21);
    if (!app->animations.load(21, bytes.data(), th13::u32(bytes.size()))) {
        last_error = "staff.anm load failed";
        return 0;
    }
    if (!app->graphics.preload(*app->animations.resource(21))) {
        last_error = "staff.anm preload failed";
        return 0;
    }
    const th13::u32 handle = app->animations.create(21, 5, -1, 0);
    if (!handle) {
        last_error = "script5 cue failed";
        return 0;
    }
    return 1;
}

EMSCRIPTEN_KEEPALIVE int th13_debug_ending(int character, int difficulty) {
    using namespace th13::sdl;
    if (running || !app || !app->initialized) return 0;
    app->progress.character = character;
    app->progress.difficulty = difficulty;
    app->progress.continues = 0;
    app->progress.practice_mode = 0;
    app->progress.stage = 6;
    app->title.reset();
    app->pending_destination = th13::i32(Destination::Ending);
    if (!app->apply_destination()) {
        last_error = app->failure;
        return 0;
    }
    clear_inputs();
    reset_cadence();
    return 1;
}

EMSCRIPTEN_KEEPALIVE int th13_debug_game_over() {
    using namespace th13::sdl;
    if (!app || !app->initialized || !app->run) return 0;
    if (!app->request_game_over()) {
        last_error = app->failure;
        return 0;
    }
    return 1;
}

EMSCRIPTEN_KEEPALIVE int th13_debug_start_run(int stage, int character, int difficulty, int spell_slot) {
    using namespace th13::sdl;
    if (!app || !app->initialized || app->run || th13::multiplayer::session.Active()) return 0;
    app->progress.character = character;
    app->progress.difficulty = difficulty;
    app->progress.run_mode_flags = 0;
    app->progress.practice_mode = 0;
    app->progress.selected_spell_id = -1;
    if (spell_slot >= 0) {
        const auto* card = app->spell_catalog.variant(stage, spell_slot, difficulty);
        if (!card) {
            last_error = "Spell card slot unavailable";
            return 0;
        }
        app->progress.selected_spell_id = card->id;
        app->progress.run_mode_flags = 0x20u;
    }
    app->title.reset();
    if (!app->start_game(stage) || !app->apply_destination()) {
        last_error = app->failure;
        return 0;
    }
    clear_inputs();
    reset_cadence();
    return 1;
}

EMSCRIPTEN_KEEPALIVE int th13_debug_stage_clear() {
    using namespace th13::sdl;
    if (!app || !app->run || app->stage_transition_active || app->stage_transition_loading) return 0;
    app->run->open_dialog(-2);
    if (!app->run->error.empty()) {
        last_error = app->run->error;
        return 0;
    }
    return 1;
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_transition_state() {
    using namespace th13::sdl;
    static char report[256];
    if (!app) {
        std::snprintf(report, sizeof report, "{\"run\":false}");
        return report;
    }
    const int clear_age = app->run && (app->run->hud.flags & 0x100u) ? app->run->hud.clear_age.current : -1;
    std::snprintf(report, sizeof report,
                  "{\"run\":%s,\"stage\":%d,\"stage_frame\":%d,\"loading\":%s,\"transition\":%s,"
                  "\"startup_age\":%d,\"clear_age\":%d,\"pending_destination\":%d,\"pending_load\":%u,"
                  "\"loading_waiting\":%s}",
                  app->run ? "true" : "false", app->progress.stage, app->progress.stage_frame,
                  app->stage_transition_loading ? "true" : "false", app->stage_transition_active ? "true" : "false",
                  app->stage_startup_age, clear_age, app->pending_destination, app->pending_load,
                  app->loading_waiting() ? "true" : "false");
    return report;
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_pause_state() {
    using namespace th13::sdl;
    static char report[512];
    if (!app || !app->pause) {
        report[0] = 0;
        return report;
    }
    const auto& state = app->pause->state;
    std::snprintf(report, sizeof report,
                  "{\"screen\":%d,\"phase\":%d,\"age\":%d,\"cursor\":%d,\"menu_count\":%d,"
                  "\"can_continue\":%d,\"can_save\":%d,\"needs_name\":%d,\"name_is_result\":%d,"
                  "\"replay_slot\":%d,\"name_cursor\":%d,\"name\":\"%s\"}",
                  int(state.screen), state.phase, state.age.current, state.menu.cursor, state.menu.count,
                  int(state.can_continue), int(state.can_save_replay), int(state.result_needs_name),
                  int(state.name_is_result), state.replay_slot, state.name_cursor, state.replay_name.c_str());
    return report;
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_result_state() {
    using namespace th13::sdl;
    static std::string report;
    report.clear();
    if (!app || !app->initialized) return report.c_str();
    th13::TitleResultScreenData data;
    if (!app->result_screen_data(data)) return report.c_str();
    char row[256];
    std::snprintf(row, sizeof row,
                  "{\"stage\":%d,\"slowdown\":%.9g,\"score\":%d,\"rank\":%d,"
                  "\"has_run\":%s,\"slow_actual\":%.17g,\"slow_budget\":%.17g}",
                  data.current.stage, double(data.current.progress_percent), data.current.score, data.rank,
                  app->run ? "true" : "false", app->slow_actual_, app->slow_budget_);
    report = row;
    return report.c_str();
}

EMSCRIPTEN_KEEPALIVE int th13_debug_frame_present(unsigned present) {
    using namespace th13::sdl;
    if (!app) return 0;
    if (!app->failure.empty()) return 0;
    if (app->loading_waiting()) {
        if (!(app->loading_startup ? app->advance_loading(1. / 60.) : app->complete_loading())) return 0;
        return 1;
    }
    const double begin = emscripten_get_now();
    app->graphics.backend.defer = true;
    const bool ok = sample_and_tick();
    if (present) app->graphics.backend.commit();
    app->graphics.backend.defer = false;
    app->audio_device.pump();
    th13_browser_frame(ok ? 1 : 0, emscripten_get_now() - begin, 1);
    return ok ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE int th13_debug_frame() { return th13_debug_frame_present(1); }

EMSCRIPTEN_KEEPALIVE unsigned th13_debug_replay_frame() { return th13::sdl::app ? th13::sdl::app->replay_consumed : 0; }

EMSCRIPTEN_KEEPALIVE const char* th13_debug_replay_progress() {
    using namespace th13::sdl;
    static std::string report;
    if (!app) return "";
    const auto* cursor = app->replay_cursor.get();
    const auto* stage = cursor ? cursor->stage() : nullptr;
    int slot = -1;
    th13::u64 position = 0;
    for (size_t i = 0; i < app->replay_file.stages.size(); ++i) {
        if (&app->replay_file.stages[i] == stage) {
            slot = int(i);
            break;
        }
        position += app->replay_file.stages[i].frame_count;
    }
    if (slot >= 0) position += cursor->frame_index();
    report =
        "{\"active\":" + std::string(app->replay_active ? "true" : "false") +
        ",\"finished\":" + (app->replay_finished ? "true" : "false") +
        ",\"cursor_valid\":" + (cursor && cursor->valid() ? "true" : "false") +
        ",\"slot_index\":" + std::to_string(slot) + ",\"slot_number\":" + std::to_string(stage ? stage->number : -1) +
        ",\"slot_frame\":" + std::to_string(cursor ? cursor->frame_index() : 0) +
        ",\"slot_frames\":" + std::to_string(stage ? stage->frame_count : 0) +
        ",\"recorded_position\":" + std::to_string(slot >= 0 ? position : 0) +
        ",\"consumed\":" + std::to_string(app->replay_consumed) +
        ",\"current_stage\":" + std::to_string(app->progress.stage) + ",\"host_frame\":" + std::to_string(app->frames) +
        ",\"loading\":" + (app->loading_waiting() ? "true" : "false") +
        ",\"transition\":" + (app->stage_transition_active ? "true" : "false") +
        ",\"startup_age\":" + std::to_string(app->stage_startup_age) +
        ",\"overlay\":" + std::to_string(app->pause ? th13::i32(app->pause->state.screen) : 0) +
        ",\"phase\":" + std::to_string(th13_phase()) + ",\"scene_flags\":" + std::to_string(app->progress.scene_flags) +
        ",\"environment_paused\":" + (app->environment.paused ? "true" : "false") +
        ",\"stage_frame\":" + std::to_string(app->progress.stage_frame) + "}";
    return report.c_str();
}

EMSCRIPTEN_KEEPALIVE void th13_debug_damage_probe(unsigned enabled) { th13::damage_probe_enabled = enabled != 0; }

EMSCRIPTEN_KEEPALIVE void th13_debug_damage_probe_window(int first, int last) {
    th13::damage_probe_first_frame = first;
    th13::damage_probe_last_frame = last;
}

EMSCRIPTEN_KEEPALIVE unsigned th13_debug_stage_frame() {
    return th13::sdl::app && th13::sdl::app->run ? unsigned(th13::sdl::app->progress.stage_frame) : 0;
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_layer_report() {
    using namespace th13::sdl;
    static std::string report;
    if (!app) return "";
    report.clear();
    char line[256];
    std::snprintf(line, sizeof line, "registry vms=%u\n", app->animations.registry.count());
    report += line;
    for (th13::u32 layer = 0; layer < 33; layer++) {
        const auto& vms = app->animations.registry.layer(layer);
        if (vms.empty()) continue;
        std::snprintf(line, sizeof line, "layer %u: %zu vm(s)\n", layer, vms.size());
        report += line;
        for (const auto* vm : vms) {
            if (!vm) continue;
            const auto* source = vm->sprite_resource ? vm->sprite_resource : vm->resource;
            const char* texture = source && !source->textures.empty() ? source->textures[0].name.c_str() : "?";
            const th13::i32 sprite = vm->visual.sprite;
            const char* sprite_texture =
                source && vm->texture_sprite() ? source->textures[vm->texture_sprite()->texture].name.c_str() : "-";
            std::snprintf(line, sizeof line, "  script=%d sprite=%d pos=(%.0f,%.0f) flags=0x%x tex=%s/%s\n",
                          vm->source_script, sprite, vm->visual.translation.x, vm->visual.translation.y,
                          vm->visual.render_flags, texture, sprite_texture);
            report += line;
            std::snprintf(
                line, sizeof line,
                "    visual=%08x color=%08x mode=%u scale=(%.5g,%.5g) size=(%.5g,%.5g) origin=(%.5g,%.5g,%.5g) pc=%d\n",
                vm->visual.flags, vm->visual.color, vm->visual.draw_mode(), vm->visual.scale.x, vm->visual.scale.y,
                vm->visual.sprite_size.x, vm->visual.sprite_size.y, vm->variables.position.x, vm->variables.position.y,
                vm->variables.position.z, vm->instruction_offset);
            report += line;
        }
    }
    return report.c_str();
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_anm_state() {
    using namespace th13::sdl;
    static std::string result;
    result = "{\"schema\":\"th13-anm-state-v1\",\"vms\":[";
    if (!app) {
        result += "]}";
        return result.c_str();
    }
    const auto bits = [](float f) {
        th13::u32 value;
        std::memcpy(&value, &f, sizeof value);
        return value;
    };
    const auto vector3 = [&](const th13::Vec3& v) {
        return "[" + std::to_string(bits(v.x)) + "," + std::to_string(bits(v.y)) + "," + std::to_string(bits(v.z)) +
               "]";
    };

    const auto json_string = [](const std::string& text) {
        std::string escaped = "\"";
        for (const unsigned char value : text) {
            switch (value) {
            case '"':
                escaped += "\\\"";
                break;
            case '\\':
                escaped += "\\\\";
                break;
            case '\b':
                escaped += "\\b";
                break;
            case '\f':
                escaped += "\\f";
                break;
            case '\n':
                escaped += "\\n";
                break;
            case '\r':
                escaped += "\\r";
                break;
            case '\t':
                escaped += "\\t";
                break;
            default:
                if (value < 0x20) {
                    char code[7];
                    std::snprintf(code, sizeof code, "\\u%04x", unsigned(value));
                    escaped += code;
                } else {
                    escaped += char(value);
                }
            }
        }
        escaped += '"';
        return escaped;
    };
    bool first = true;
    auto& registry = app->animations.registry;
    for (th13::u32 bucket = 0; bucket < 33; ++bucket) {
        for (const auto* vm : registry.layer(bucket)) {
            if (!vm) continue;
            if (!first) result += ',';
            first = false;
            const auto* parent = vm->creation_parent;
            const auto* texture_sprite = vm->texture_sprite();
            const auto* texture_source = vm->sprite_resource ? vm->sprite_resource : vm->resource;
            const th13::i32 texture_index = texture_sprite ? th13::i32(texture_sprite->texture) : -1;
            const std::string texture_name =
                texture_source && texture_index >= 0 && th13::u32(texture_index) < texture_source->textures.size()
                    ? texture_source->textures[th13::u32(texture_index)].name
                    : std::string();
            result += "{\"bank\":" + std::to_string(app->animations.resource_id(vm->resource)) +
                      ",\"script\":" + std::to_string(vm->source_script) +
                      ",\"handle\":" + std::to_string(registry.handle(*vm)) +
                      ",\"parent\":" + std::to_string(parent ? registry.handle(*parent) : 0) +
                      ",\"bucket\":" + std::to_string(bucket) + ",\"layer\":" + std::to_string(vm->visual.layer) +
                      ",\"sprite\":" + std::to_string(vm->visual.sprite) +
                      ",\"color\":" + std::to_string(vm->visual.color) +
                      ",\"flags\":" + std::to_string(vm->visual.flags) +
                      ",\"render_flags\":" + std::to_string(vm->visual.render_flags) +
                      ",\"pending\":" + std::to_string(vm->pending_interrupt) +
                      ",\"pc\":" + std::to_string(vm->instruction_offset) +
                      ",\"timer_previous\":" + std::to_string(vm->timer.previous) +
                      ",\"timer_current\":" + std::to_string(vm->timer.current) +
                      ",\"timer_fraction_bits\":" + std::to_string(bits(vm->timer.fractional)) +
                      ",\"position_bits\":" + vector3(vm->variables.position) +
                      ",\"translation_bits\":" + vector3(vm->visual.translation) + ",\"scale_bits\":[" +
                      std::to_string(bits(vm->visual.scale.x)) + "," + std::to_string(bits(vm->visual.scale.y)) + "]" +
                      ",\"rotation_bits\":" + vector3(vm->variables.rotation) +
                      ",\"texture_index\":" + std::to_string(texture_index) +
                      ",\"texture_name\":" + json_string(texture_name) + "}";
        }
    }
    result += "]";
    result += ",\"dialogue\":{\"entry\":" + std::to_string(app->run ? app->run->dialog_entry() : -1) +
              ",\"active\":" + (app->run && app->run->dialog_active() ? "true" : "false") +
              ",\"ecl_released\":" + (app->run && app->run->dialog_wait_released() ? "true" : "false") + "}}";
    return result.c_str();
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_mp_focus_state() {
    using namespace th13::sdl;
    static std::string result;
    if (!app || !app->run) return "";
    const auto& battle = app->run->battle;
    auto& registry = app->animations.registry;
    result = "{\"stageFrame\":" + std::to_string(app->progress.stage_frame) +
             ",\"simFrame\":" + std::to_string(battle.sim.frame) +
             ",\"seatCount\":" + std::to_string(battle.seat_count) +
             ",\"localSeat\":" + std::to_string(battle.mp_local_seat) + ",\"seats\":[";
    char row[512];
    for (int seat = 0; seat < battle.seat_count && seat < 3; ++seat) {
        const auto& player = battle.seats[size_t(seat)];
        if (seat) result += ',';
        std::snprintf(row, sizeof row,
                      "{\"seat\":%d,\"character\":%d,\"x\":%.6f,\"y\":%.6f,\"focus\":%s,\"phase\":%d,\"ghost\":%s}",
                      seat, player.character, double(player.x), double(player.y), player.focus ? "true" : "false",
                      int(player.phase), player.ghost ? "true" : "false");
        result += row;
    }
    result += "],\"vms\":[";
    bool first = true;
    for (th13::u32 bucket = 0; bucket < 33; ++bucket) {
        for (const auto* vm : registry.layer(bucket)) {
            if (!vm || !vm->shot_effect_owner || (vm->source_script != 21 && vm->source_script != 22)) continue;
            if (!first) result += ',';
            first = false;
            const auto* parent = vm->creation_parent;
            std::snprintf(row, sizeof row,
                          "{\"bank\":%d,\"script\":%d,\"handle\":%u,\"parent\":%u,\"owner\":%u,\"bucket\":%u,\"layer\":"
                          "%u,\"sprite\":%d,\"color\":%u,\"pending\":%d,\"timer\":%d,\"position\":[%.9g,%.9g,%.9g],"
                          "\"translation\":[%.9g,%.9g,%.9g]}",
                          app->animations.resource_id(vm->resource), vm->source_script, registry.handle(*vm),
                          parent ? registry.handle(*parent) : 0u, unsigned(vm->shot_effect_owner), bucket,
                          unsigned(vm->visual.layer), vm->visual.sprite, vm->visual.color, vm->pending_interrupt,
                          vm->timer.current, double(vm->variables.position.x), double(vm->variables.position.y),
                          double(vm->variables.position.z), double(vm->visual.translation.x),
                          double(vm->visual.translation.y), double(vm->visual.translation.z));
            result += row;
        }
    }
    result += "]}";
    return result.c_str();
}

EMSCRIPTEN_KEEPALIVE void th13_debug_anm_random_observe(int first, int last) {
    using namespace th13::sdl;
    anm_random_observations.clear();
    anm_random_first = first;
    anm_random_last = last;
    th13::anm_random_observer = first <= last ? observe_anm_random : nullptr;
}

namespace {
std::string rng_site_rows;
int rng_site_first = 0, rng_site_last = -1;
void observe_rng_site(unsigned before, unsigned after) {
    using namespace th13::sdl;
    if (!app) return;
    const int frame = app->progress.stage_frame - 1;
    if (frame < rng_site_first || frame > rng_site_last) return;
    const th13::AnmVm* vm = th13::anm_tick_current_vm;
    rng_site_rows += "{\"frame\":" + std::to_string(frame) + ",\"before\":" + std::to_string(before) +
                     ",\"after\":" + std::to_string(after) + ",\"vm\":" +
                     (vm && vm->resource ? "{\"bank\":" + std::to_string(app->animations.resource_id(vm->resource)) +
                                               ",\"script\":" + std::to_string(vm->source_script) +
                                               ",\"handle\":" + std::to_string(app->animations.registry.handle(*vm)) +
                                               ",\"pc\":" + std::to_string(vm->instruction_offset) +
                                               ",\"ptr\":" + std::to_string(reinterpret_cast<uintptr_t>(vm)) +
                                               ",\"t\":" + std::to_string(vm->timer.current) + "}"
                                         : "null") +
                     "}\n";
}
}

EMSCRIPTEN_KEEPALIVE void th13_debug_rng_site_observe(int first, int last) {
    rng_site_rows.clear();
    rng_site_first = first;
    rng_site_last = last;
    th13::GameRandom::draw_observer = first <= last ? observe_rng_site : nullptr;
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_rng_site_take() {
    static std::string result;
    result.swap(rng_site_rows);
    rng_site_rows.clear();
    return result.c_str();
}
namespace {
std::string opt_spawn_rows;
int opt_spawn_first = 0, opt_spawn_last = -1;
void observe_opt_spawn(const th13::AnmVm& parent, th13::i32 child_script, th13::u32 child_handle) {
    using namespace th13::sdl;
    if (!app) return;
    const int frame = app->progress.stage_frame - 1;
    if (frame < opt_spawn_first || frame > opt_spawn_last) return;
    const th13::i32 bank = app->animations.resource_id(parent.resource);
    opt_spawn_rows += "{\"frame\":" + std::to_string(frame) + ",\"bank\":" + std::to_string(bank) +
                      ",\"parent_script\":" + std::to_string(parent.source_script) +
                      ",\"parent_handle\":" + std::to_string(app->animations.registry.handle(parent)) +
                      ",\"child_script\":" + std::to_string(child_script) +
                      ",\"child_handle\":" + std::to_string(child_handle) + "}\n";
}
}
EMSCRIPTEN_KEEPALIVE void th13_debug_opt_spawn_observe(int first, int last) {
    opt_spawn_rows.clear();
    opt_spawn_first = first;
    opt_spawn_last = last;
    th13::anm_spawn_observer = first <= last ? observe_opt_spawn : nullptr;
}
EMSCRIPTEN_KEEPALIVE const char* th13_debug_opt_spawn_take() {
    static std::string result;
    result.swap(opt_spawn_rows);
    opt_spawn_rows.clear();
    return result.c_str();
}
namespace {
std::string dialog_rows;
int dialog_first = 0, dialog_last = -1;
void observe_dialog(th13::i32 entry) {
    using namespace th13::sdl;
    if (!app) return;
    const int frame = app->progress.stage_frame - 1;
    if (frame < dialog_first || frame > dialog_last) return;
    dialog_rows += "{\"frame\":" + std::to_string(frame) + ",\"entry\":" + std::to_string(entry) + "}\n";
}
}
EMSCRIPTEN_KEEPALIVE void th13_debug_dialog_observe(int first, int last) {
    dialog_rows.clear();
    dialog_first = first;
    dialog_last = last;
    th13::run_dialog_observer = first <= last ? observe_dialog : nullptr;
}
EMSCRIPTEN_KEEPALIVE const char* th13_debug_dialog_take() {
    static std::string result;
    result.swap(dialog_rows);
    dialog_rows.clear();
    return result.c_str();
}

namespace {
std::string anm_tick_rows;
int anm_tick_first = 0, anm_tick_last = -1;
void observe_anm_tick(const th13::AnmVm& vm, float rate) {
    using namespace th13::sdl;
    if (!app) return;
    const th13::i32 script = vm.source_script;
    if (script != 166 && script != 167 && script != 40 && script != 170 && script != 171 && script != 200 &&
        script != 201)
        return;
    const int frame = app->progress.stage_frame - 1;
    if (frame < anm_tick_first || frame > anm_tick_last) return;
    anm_tick_rows += "{\"frame\":" + std::to_string(frame) + ",\"script\":" + std::to_string(vm.source_script) +
                     ",\"ptr\":" + std::to_string(reinterpret_cast<uintptr_t>(&vm)) +
                     ",\"timer\":" + std::to_string(vm.timer.current) +
                     ",\"pc\":" + std::to_string(vm.instruction_offset) + ",\"rate\":" + std::to_string(rate) + "}\n";
}
}

EMSCRIPTEN_KEEPALIVE void th13_debug_anm_tick_observe(int first, int last) {
    anm_tick_rows.clear();
    anm_tick_first = first;
    anm_tick_last = last;
    th13::anm_tick_observer = first <= last ? observe_anm_tick : nullptr;
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_anm_tick_take() {
    static std::string result;
    result.swap(anm_tick_rows);
    anm_tick_rows.clear();
    return result.c_str();
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_anm_random_take() {
    using namespace th13::sdl;
    static std::string result;
    result.swap(anm_random_observations);
    anm_random_observations.clear();
    return result.c_str();
}

EMSCRIPTEN_KEEPALIVE void th13_debug_item_trace(int enable) { th13::set_item_trace(enable != 0); }

EMSCRIPTEN_KEEPALIVE const char* th13_debug_item_trace_take() {
    static std::string result;
    result = th13::take_item_trace();
    return result.c_str();
}

namespace {
std::string anm_chain_row;
int anm_chain_first = 0, anm_chain_last = -1;
bool anm_chain_enabled = false;
}
EMSCRIPTEN_KEEPALIVE void th13_debug_anm_chain_observe(int first, int last) {
    anm_chain_row.clear();
    anm_chain_first = first;
    anm_chain_last = last;
    anm_chain_enabled = first <= last;
}
EMSCRIPTEN_KEEPALIVE const char* th13_debug_anm_chain_take() {
    static std::string result;
    result.swap(anm_chain_row);
    anm_chain_row.clear();
    return result.c_str();
}
EMSCRIPTEN_KEEPALIVE const char* th13_debug_anm_chain() {
    using namespace th13::sdl;
    static std::string result;
    result.clear();
    if (!app || !anm_chain_enabled) return result.c_str();
    const int frame = app->progress.stage_frame - 1;
    if (frame < anm_chain_first || frame > anm_chain_last) return result.c_str();
    auto& registry = app->animations.registry;
    const th13::u32 count = registry.ordered_count(false);
    result = "{\"frame\":" + std::to_string(frame) + ",\"count\":" + std::to_string(count) + ",\"nodes\":[";
    bool first = true;
    for (th13::u32 index = 0; index < count; ++index) {
        const th13::u32 handle = registry.ordered_handle(false, index);
        const th13::AnmVm* vm = registry.find(handle);
        if (!vm) continue;
        if (!first) result += ',';
        first = false;
        result +=
            "{\"index\":" + std::to_string(index) + ",\"handle\":" + std::to_string(handle) +
            ",\"bank\":" + std::to_string(app->animations.resource_id(vm->resource)) +
            ",\"script\":" + std::to_string(vm->source_script) + ",\"layer\":" + std::to_string(vm->visual.layer) +
            ",\"pc\":" + std::to_string(vm->instruction_offset) + ",\"timer\":" + std::to_string(vm->timer.current) +
            ",\"visible\":" + (vm->visible ? "true" : "false") + ",\"flags\":" + std::to_string(vm->visual.flags) +
            ",\"render_flags\":" + std::to_string(vm->visual.render_flags) + "}";
    }
    result += "]}\n";
    anm_chain_row += result;
    return result.c_str();
}
}
namespace {
std::string chain_walk_rows;
int chain_walk_first = 0, chain_walk_last = -1;
std::string build_chain_walk_json() {
    using namespace th13::sdl;
    if (!app) return std::string();
    const int frame = app->progress.stage_frame - 1;
    auto& registry = app->animations.registry;
    const th13::u32 count = registry.ordered_count(false);
    std::string result =
        "{\"frame\":" + std::to_string(frame) + ",\"entry\":true,\"count\":" + std::to_string(count) + ",\"nodes\":[";
    bool first = true;
    for (th13::u32 index = 0; index < count; ++index) {
        const th13::u32 handle = registry.ordered_handle(false, index);
        const th13::AnmVm* vm = registry.find(handle);
        if (!vm) continue;
        if (!first) result += ',';
        first = false;
        result +=
            "{\"index\":" + std::to_string(index) + ",\"handle\":" + std::to_string(handle) +
            ",\"bank\":" + std::to_string(app->animations.resource_id(vm->resource)) +
            ",\"script\":" + std::to_string(vm->source_script) + ",\"layer\":" + std::to_string(vm->visual.layer) +
            ",\"pc\":" + std::to_string(vm->instruction_offset) + ",\"timer\":" + std::to_string(vm->timer.current) +
            ",\"visible\":" + (vm->visible ? "true" : "false") + ",\"flags\":" + std::to_string(vm->visual.flags) +
            ",\"render_flags\":" + std::to_string(vm->visual.render_flags) + "}";
    }
    result += "]}\n";
    return result;
}
void observe_chain_walk(const th13::AnmRegistry&, bool alternate) {
    using namespace th13::sdl;
    if (alternate || !app) return;
    const int frame = app->progress.stage_frame - 1;
    if (frame < chain_walk_first || frame > chain_walk_last) return;
    chain_walk_rows += build_chain_walk_json();
}
}
extern "C" {
EMSCRIPTEN_KEEPALIVE void th13_debug_anm_chain_walk_observe(int first, int last) {
    chain_walk_rows.clear();
    chain_walk_first = first;
    chain_walk_last = last;
    th13::chain_entry_observer = first <= last ? observe_chain_walk : nullptr;
}
EMSCRIPTEN_KEEPALIVE const char* th13_debug_anm_chain_walk_take() {
    static std::string result;
    result.swap(chain_walk_rows);
    chain_walk_rows.clear();
    return result.c_str();
}

namespace {
int bullet_queue_first = 0, bullet_queue_last = -1;
bool bullet_queue_enabled = false;
}
EMSCRIPTEN_KEEPALIVE void th13_debug_bullet_queue_observe(int first, int last) {
    bullet_queue_first = first;
    bullet_queue_last = last;
    bullet_queue_enabled = first <= last;
}
EMSCRIPTEN_KEEPALIVE const char* th13_debug_bullet_queue() {
    using namespace th13::sdl;
    static std::string result;
    result.clear();
    if (!app || !app->run || !bullet_queue_enabled) return result.c_str();
    const int frame = app->progress.stage_frame - 1;
    if (frame < bullet_queue_first || frame > bullet_queue_last) return result.c_str();
    result = app->run->battle.sim.debug_bullet_queue();
    return result.c_str();
}

namespace {
std::string ecl_sprite_rows;
int ecl_sprite_first = 0, ecl_sprite_last = -1;
void observe_ecl_sprite(th13::i32 frame, th13::u32 serial, th13::i32 slot, th13::i32 sprite, th13::i32 subtype) {
    if (frame < ecl_sprite_first || frame > ecl_sprite_last) return;
    ecl_sprite_rows += "{\"frame\":" + std::to_string(frame) + ",\"serial\":" + std::to_string(serial) +
                       ",\"slot\":" + std::to_string(slot) + ",\"sprite\":" + std::to_string(sprite) +
                       ",\"subtype\":" + std::to_string(subtype) + "}\n";
}
}
EMSCRIPTEN_KEEPALIVE void th13_debug_ecl_sprite_observe(int first, int last) {
    ecl_sprite_rows.clear();
    ecl_sprite_first = first;
    ecl_sprite_last = last;
    th13::ecl_sprite_observer = first <= last ? observe_ecl_sprite : nullptr;
}
EMSCRIPTEN_KEEPALIVE const char* th13_debug_ecl_sprite_take() {
    static std::string result;
    result.swap(ecl_sprite_rows);
    ecl_sprite_rows.clear();
    return result.c_str();
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_item_report() {
    using namespace th13::sdl;
    static std::string report;
    report.clear();
    if (!app || !app->run) return "";
    auto& battle = app->run->battle;
    report = "rate=" + std::to_string(battle.sim.time_rate()) + "\n";
    const auto& items = battle.item_manager.items();
    for (size_t slot = 0; slot < items.size(); ++slot) {
        const auto& item = items[slot];
        if (!item.alive) continue;
        char row[256];
        std::snprintf(
            row, sizeof row,
            "item slot=%zu state=%d type=%d pos=(%.9g,%.9g,%.9g) velocity=(%.9g,%.9g,%.9g) homing=%.9g time=%.9g\n",
            slot, item.state, item.type, item.x, item.y, item.z, item.vx, item.vy, item.vz, item.homing_speed,
            item.anm_time);
        report += row;
    }
    return report.c_str();
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_stage_report() {
    using namespace th13::sdl;
    static std::string report;
    if (!app || !app->run || !app->run->presentation) return "";
    report = app->run->presentation->debug_report();
    report += "dialogue active=" + std::to_string(app->run->dialog_active()) +
              " ecl_released=" + std::to_string(app->run->dialog_wait_released()) + "\n";
    const auto& sim = app->run->battle.sim;
    {
        const auto& spell = sim.spell.state();
        report +=
            "spell id=" + std::to_string(spell.id) + " name=" +
            th13::cp932_to_utf8(reinterpret_cast<const th13::u8*>(spell.name.data()), th13::u32(spell.name.size())) +
            " bytes=";
        for (const unsigned char value : spell.name) {
            char hex[3];
            std::snprintf(hex, sizeof hex, "%02x", value);
            report += hex;
        }
        report += "\n";
        if (auto* text = app->animations.resource(0)) {
            const auto handle = app->graphics.texture(*text, 0);
            const auto* image = app->graphics.pixels(handle);
            if (image)
                for (th13::u32 index : {0u, 1u, 9u}) {
                    if (index >= text->sprites.size()) continue;
                    const auto& sprite = text->sprites[index];
                    th13::u32 count = 0, left = image->width, right = 0;
                    for (th13::u32 y = th13::u32(sprite.y); y < sprite.y + sprite.height; ++y)
                        for (th13::u32 x = th13::u32(sprite.x); x < sprite.x + sprite.width; ++x) {
                            th13::u16 value = 0;
                            std::memcpy(&value, image->pixels.data() + size_t(y) * image->pitch + x * 2, 2);
                            if (!(value >> 12)) continue;
                            ++count;
                            left = std::min(left, x);
                            right = std::max(right, x);
                        }
                    report += "text sprite=" + std::to_string(index) + " ink=" + std::to_string(count) +
                              " bounds=" + std::to_string(left) + "," + std::to_string(right) + "\n";
                }
        }
    }
    {
        const auto& animation_random = app->animations.random_source();
        const auto& distortion_random = app->environment.game_rng;
        char line[192];
        std::snprintf(line, sizeof line, "rng game=(%u,%u) animation=(%u,%u) distortion=(%u,%u)\n", sim.random.seed(),
                      sim.random.index(), animation_random.seed(), animation_random.index(), distortion_random.seed(),
                      distortion_random.index());
        report += line;
    }
    for (const auto& enemy : sim.enemies) {
        if (enemy->dead) continue;
        char line[320];
        std::snprintf(line, sizeof line,
                      "  enemy id=%d serial=%u pos=(%.9g,%.9g) life=%.9g age=%.9g flags=%u invulnerable=%d "
                      "spell_life=%d phase_life=%d\n",
                      enemy->script_id, enemy->serial, enemy->position[0], enemy->position[1], double(enemy->life),
                      double(enemy->age), enemy->flags, enemy->invulnerable, enemy->spell_life,
                      enemy->phase_life_remaining);
        report += line;
    }
    for (const auto& bullet : sim.bullets) {
        if (!bullet.alive) continue;
        char line[320];
        std::snprintf(line, sizeof line,
                      "  projectile generation=%u type=%u state=%d pos=(%.9g,%.9g) hit=(%.9g,%.9g) scale=%.9g flags=%u "
                      "grazed=%d ready=%d\n",
                      bullet.generation, bullet.sprite, bullet.state, bullet.x, bullet.y, bullet.hit_size[0],
                      bullet.hit_size[1], bullet.hit_scale, bullet.flags, bullet.grazed, bullet.collision_ready);
        report += line;
    }
    {
        const auto& battle = app->run->battle;
        char line[160];
        std::snprintf(line, sizeof line, "combat player=(%.9g,%.9g) phase=%d invulnerable=%d\n", battle.player_x,
                      battle.player_y, int(battle.player_phase), battle.invulnerable);
        report += line;
        if (battle.bomb_age >= 0) {
            report += "bomb age=" + std::to_string(battle.bomb_age) + "\n";
            for (size_t i = 0; i < battle.bomb_orbs.size(); ++i) {
                const auto& orb = battle.bomb_orbs[i];
                char row[256];
                std::snprintf(row, sizeof row,
                              "  orb i=%zu active=%d pos=(%.9g,%.9g) angle=%.9g speed=%.9g radius=%.9g "
                              "velocity=(%.9g,%.9g) damage=%.9g\n",
                              i, orb.active, orb.x, orb.y, orb.angle, orb.speed, orb.radius, orb.velocity_x,
                              orb.velocity_y, orb.accumulated_damage);
                report += row;
            }
        }
        for (const auto& shot : battle.shots) {
            if (!shot.alive) continue;
            char details[256];
            std::snprintf(details, sizeof details,
                          "  player_shot gen=%u age=%d pos=(%.9g,%.9g) angle=%.9g speed=%.9g target=%u script=%d\n",
                          shot.generation, shot.age, shot.x, shot.y, shot.angle, shot.speed, shot.target_serial,
                          shot.script);
            report += details;
        }
        for (const auto& bullet : sim.bullets) {
            if (!bullet.alive) continue;
            std::snprintf(line, sizeof line,
                          "  hazard pos=(%.9g,%.9g) velocity=(%.9g,%.9g) size=(%.9g,%.9g) flags=%u\n", bullet.x,
                          bullet.y, bullet.vel[0], bullet.vel[1], bullet.hit_size[0], bullet.hit_size[1], bullet.flags);
            report += line;
            std::snprintf(line, sizeof line,
                          "    bullet gen=%u type=%u color=%u state=%d age=%d ex=%d behavior=%u bounce=%d/%d mode=%u\n",
                          bullet.generation, bullet.sprite, bullet.subtype, bullet.state, bullet.state_age,
                          bullet.ex_cursor, bullet.behavior_flags, bullet.ex40_bounces, bullet.ex40_limit,
                          bullet.ex40_mode);
            report += line;
        }
    }
    report +=
        "lasers=" + std::to_string(sim.lasers.size()) + " births=" + std::to_string(sim.laser_serial - 0x10000u) + "\n";
    for (const auto& laser : sim.lasers) {
        char line[240];
        std::snprintf(line, sizeof line,
                      "  laser gen=%u id=%d phase=%d time=%d pos=(%.9g,%.9g) angle=%.9g length=%.9g width=%.9g\n",
                      laser.generation, laser.id, laser.phase, laser.time, laser.origin[0], laser.origin[1],
                      laser.angle, laser.length, laser.width);
        report += line;
    }
    return report.c_str();
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_state_report() {
    using namespace th13::sdl;
    static char report[768];
    if (!app || !app->run) return "";
    const auto& battle = app->run->battle;
    const auto& progress = app->progress;
    const auto& player = app->selection_player;
    std::string keys;
    constexpr std::pair<unsigned, unsigned> mapping[] = {{8, 16},  {64, 37}, {16, 38}, {128, 39},
                                                         {32, 40}, {2, 88},  {1, 90}};
    const unsigned held = app->replay_active ? app->replay_input.held : controls.held;
    for (const auto& pair : mapping)
        if (held & pair.first) {
            if (!keys.empty()) keys += ',';
            keys += std::to_string(pair.second);
        }
    std::snprintf(report, sizeof report,
                  "{\"schema\":\"th13-state-raw-v1\",\"frame\":%d,"
                  "\"player_position\":[%.9g,%.9g],\"power\":%d,\"power_step\":%d,"
                  "\"spell_id\":null,\"keys\":[%s],"
                  "\"raw_globals\":{\"004be7c0\":%d,\"004be7c8\":%d,\"004be7d0\":%d,\"004be7dc\":%d,"
                  "\"004be810\":%d,\"004be814\":%d},\"raw_enemy_manager\":null}",
                  int(app->replay_active ? app->replay_consumed : progress.stage_frame) - 1, double(battle.player_x),
                  double(battle.player_y), player.power, player.power_step, keys.c_str(), app->selection_score.score,
                  progress.continues, progress.graze, progress.max_point, progress.high_score,
                  progress.high_score_extra);
    return report;
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_replay_core() {
    using namespace th13::sdl;
    using th13::u32;
    static std::string report;
    if (!app || !app->run || !app->replay_active) return "";
    const auto& p = app->progress;
    const auto& h = app->selection_player;
    const auto& battle = app->run->battle;
    const auto bits = [](float value) {
        unsigned raw;
        std::memcpy(&raw, &value, sizeof raw);
        return raw;
    };
    report = "{\"schema\":\"th13-replay-core-v2\",\"frame\":" + std::to_string(int(app->replay_consumed) - 1) +
             ",\"stage\":" + std::to_string(p.stage) +
             ",\"input\":{\"held\":" + std::to_string(app->replay_input.held) +
             ",\"pressed\":" + std::to_string(app->replay_input.pressed) +
             ",\"released\":" + std::to_string(app->replay_input.released) + "},\"globals\":{";
    const std::pair<const char*, u32> values[] = {
        {"004be7b8", u32(p.character)},       {"004be7c0", u32(app->selection_score.score)},
        {"004be7c4", u32(p.difficulty)},      {"004be7c8", u32(p.continues)},
        {"004be7d0", u32(p.graze)},           {"004be7d4", u32(p.selected_spell_id)},
        {"004be7dc", u32(p.max_point)},       {"004be7e4", u32(p.max_point_cap)},
        {"004be7e8", u32(h.power)},           {"004be7ec", u32(h.max_power)},
        {"004be7f0", u32(h.power_step)},      {"004be7f4", u32(h.extra_lives)},
        {"004be7f8", u32(h.life_pieces)},     {"004be7fc", u32(p.life_piece_threshold_index)},
        {"004be800", u32(h.bombs)},           {"004be804", u32(h.bomb_pieces)},
        {"004be808", u32(p.ending_points)},   {"004be810", u32(p.high_score)},
        {"004be814", u32(p.high_score_extra)}};
    bool first = true;
    for (const auto& value : values) {
        if (!first) report += ',';
        first = false;
        report += '"';
        report += value.first;
        report += "\":" + std::to_string(value.second);
    }
    const auto& game = app->game_random;
    const auto& visual = app->animations.random_source();
    report += "},\"player\":{\"x_bits\":" + std::to_string(bits(battle.player_x)) +
              ",\"y_bits\":" + std::to_string(bits(battle.player_y)) +
              "},\"rng\":{\"game_seed\":" + std::to_string(game.seed()) +
              ",\"game_index\":" + std::to_string(game.index()) + ",\"visual_seed\":" + std::to_string(visual.seed()) +
              ",\"visual_index\":" + std::to_string(visual.index()) + "}}";
    return report.c_str();
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_replay_entities() {
    static std::string report;
    const auto& app = th13::sdl::app;
    if (!app || !app->run) return "";
    auto state = th13::sample_replay_entities(app->run->battle);
    state.frame = app->replay_active ? app->replay_consumed - 1 : app->progress.stage_frame - 1;
    report = state.json();
    report.pop_back();
    report += ",\"boss_main_serial\":" + std::to_string(app->run->battle.sim.boss_main_serial) + "}";
    return report.c_str();
}

EMSCRIPTEN_KEEPALIVE int th13_debug_mp_laser_contact(float x, float y, float radius) {
    const auto& app = th13::sdl::app;
    if (!app || !app->run) return 0;
    for (const auto& laser : app->run->battle.sim.lasers)
        if (laser.active && laser.collision_enabled() && laser.player_contact(x, y, radius, radius) == 1) return 1;
    return 0;
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_mp_phase_state() {
    static std::string report;
    const auto& app = th13::sdl::app;
    if (!app || !app->run) {
        report = "{\"available\":false}";
        return report.c_str();
    }
    const auto& sim = app->run->battle.sim;
    const auto& registry = app->animations.registry;
    constexpr size_t enemy_limit = 1024, thread_limit = 64, name_limit = 256;
    std::ostringstream out;
    out.imbue(std::locale::classic());
    out.precision(std::numeric_limits<double>::max_digits10);
    const auto number = [&](double value) {
        if (std::isfinite(value))
            out << value;
        else
            out << "null";
    };
    const auto name = [&](const std::string& value) {
        static constexpr char hex[] = "0123456789abcdef";
        out << '"';
        for (size_t i = 0; i < std::min(value.size(), name_limit); ++i) {
            const auto c = static_cast<unsigned char>(value[i]);
            if (c == '"' || c == '\\')
                out << '\\' << char(c);
            else if (c < 0x20 || c >= 0x7f)
                out << "\\u00" << hex[c >> 4] << hex[c & 15];
            else
                out << char(c);
        }
        out << '"';
    };
    out << "{\"available\":true,\"stage\":" << app->progress.stage << ",\"stage_frame\":" << app->progress.stage_frame
        << ",\"sim_frame\":" << sim.frame << ",\"boss_main_serial\":" << sim.boss_main_serial
        << ",\"enemy_count\":" << sim.enemies.size() << ",\"enemy_limit\":" << enemy_limit
        << ",\"enemies_truncated\":" << (sim.enemies.size() > enemy_limit ? "true" : "false") << ",\"enemies\":[";
    for (size_t index = 0; index < std::min(sim.enemies.size(), enemy_limit); ++index) {
        if (index) out << ',';
        const auto& ptr = sim.enemies[index];
        if (!ptr) {
            out << "null";
            continue;
        }
        const auto& enemy = *ptr;
        out << "{\"serial\":" << enemy.serial << ",\"spawn_routine\":";
        name(enemy.routine);
        out << ",\"spawn_routine_truncated\":" << (enemy.routine.size() > name_limit ? "true" : "false")
            << ",\"life\":";
        number(enemy.life);
        out << ",\"flags\":" << enemy.flags << ",\"dead\":" << (enemy.dead ? "true" : "false") << ",\"age\":";
        number(enemy.age);
        out << ",\"position\":[";
        number(enemy.position[0]);
        out << ',';
        number(enemy.position[1]);
        out << ',';
        number(enemy.position[2]);
        out << "],\"mirror_dir\":" << enemy.mirror_dir << ",\"mirror_velocity_x\":";
        number(enemy.mirror_velocity_x);
        out << ",\"anm_file\":" << enemy.anm_file << ",\"anm_base_script\":" << enemy.anm_base_script
            << ",\"anm_slot0_bank\":" << enemy.anm_slot0_bank
            << ",\"anm_slot0_script_offset\":" << enemy.anm_slot0_script_offset;
        const size_t count = enemy.threads ? enemy.threads->count() : 0;
        out << ",\"thread_count\":" << count << ",\"thread_limit\":" << thread_limit
            << ",\"threads_truncated\":" << (count > thread_limit ? "true" : "false") << ",\"threads\":[";
        for (size_t i = 0; i < std::min(count, thread_limit); ++i) {
            if (i) out << ',';
            const auto* thread = enemy.threads->at(static_cast<th13::u32>(i));
            if (!thread) {
                out << "null";
                continue;
            }
            const auto* program = thread->program;
            const bool valid = program && thread->routine >= 0 && size_t(thread->routine) < program->definitions.size();
            out << "{\"index\":" << i << ",\"routine\":" << thread->routine
                << ",\"routine_valid\":" << (valid ? "true" : "false") << ",\"routine_name\":";
            if (valid)
                name(program->subroutine(static_cast<th13::u32>(thread->routine)).name);
            else
                out << "null";
            out << ",\"routine_name_truncated\":"
                << (valid && program->subroutine(static_cast<th13::u32>(thread->routine)).name.size() > name_limit
                        ? "true"
                        : "false")
                << ",\"instruction_offset\":" << thread->instruction_offset << ",\"time\":";
            number(thread->time);
            out << ",\"id\":" << thread->thread_id << ",\"state\":" << thread->thread_state
                << ",\"flags\":" << thread->thread_flags << '}';
        }
        out << "],\"slots\":[";
        for (int i = 0; i < th13::StageEnemy::kAnimationSlots; ++i) {
            if (i) out << ',';
            const auto handle = enemy.anm_slot_handle[i];
            const auto* vm = registry.find(handle);
            out << "{\"index\":" << i << ",\"script\":" << enemy.anm_script[size_t(i)] << ",\"handle\":" << handle
                << ",\"resolved\":" << (vm ? "true" : "false");
            if (vm)
                out << ",\"source_script\":" << vm->source_script << ",\"pending_interrupt\":" << vm->pending_interrupt
                    << ",\"timer\":" << vm->timer.current << ",\"age\":" << vm->age.current;
            out << '}';
        }
        out << "]}";
    }
    out << "]}";
    report = out.str();
    return report.c_str();
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_mp_hazards() {
    static std::string report;
    const auto& app = th13::sdl::app;
    if (!app || !app->run) return "";
    const auto& sim = app->run->battle.sim;
    std::ostringstream out;
    out.imbue(std::locale::classic());
    out.precision(std::numeric_limits<double>::max_digits10);
    out << "{\"dialogue\":" << (app->run->dialog_active() ? "true" : "false")
        << ",\"boss_main_serial\":" << sim.boss_main_serial << ",\"enemies\":[";
    bool comma = false;
    for (const auto& slot : sim.enemies) {
        if (!slot || slot->dead) continue;
        const auto& e = *slot;
        if (comma) out << ',';
        comma = true;
        out << "{\"serial\":" << e.serial << ",\"flags\":" << e.flags << ",\"life\":" << e.life
            << ",\"dead\":false,\"body_invulnerable\":" << e.body_invulnerable << ",\"x\":" << e.position[0]
            << ",\"y\":" << e.position[1] << ",\"dx\":" << e.motion_delta[0] << ",\"dy\":" << e.motion_delta[1]
            << ",\"width\":" << e.hitbox[0] << ",\"height\":" << e.hitbox[1] << ",\"angle\":" << e.body_angle << '}';
    }
    out << "],\"bullets\":[";
    comma = false;
    for (std::size_t index = 0; index < sim.bullets.size(); ++index) {
        const auto& b = sim.bullets[index];
        if (!b.alive) continue;
        if (comma) out << ',';
        comma = true;
        out << "{\"index\":" << index << ",\"flags\":" << b.flags
            << ",\"collision_ready\":" << (b.collision_ready ? "true" : "false") << ",\"alive\":true,\"x\":" << b.x
            << ",\"y\":" << b.y << ",\"vx\":" << b.vel[0] << ",\"vy\":" << b.vel[1] << ",\"width\":" << b.hit_size[0]
            << ",\"height\":" << b.hit_size[1] << ",\"scale\":" << b.hit_scale << '}';
    }
    out << "],\"items\":[";
    comma = false;
    for (const auto& item : app->run->battle.item_manager.items()) {
        if (!item.alive) continue;
        if (comma) out << ',';
        comma = true;
        out << "{\"kind\":" << item.type << ",\"x\":" << item.x << ",\"y\":" << item.y << '}';
    }
    out << "],\"spirits\":[";
    comma = false;
    for (const auto& spirit : app->run->battle.item_manager.spirits()) {
        if (!spirit.alive) continue;
        if (comma) out << ',';
        comma = true;
        out << "{\"kind\":" << spirit.kind << ",\"x\":" << spirit.x << ",\"y\":" << spirit.y << '}';
    }
    out << "]}";
    report = out.str();
    return report.c_str();
}

EMSCRIPTEN_KEEPALIVE void th13_debug_audio_observe(unsigned enable) {
    using namespace th13::sdl;
    if (!app) return;
    audio_observations.clear();
    audio_sequence = 0;
    app->audio_device.request_observer = {};
    if (!enable) return;
    app->audio_device.request_observer = [](const AudioDevice::RequestObservation& event) {
        const char* kind = event.kind == AudioDevice::RequestObservation::Kind::Voice  ? "voice"
                           : event.kind == AudioDevice::RequestObservation::Kind::Stop ? "stop"
                           : event.kind == AudioDevice::RequestObservation::Kind::Pan  ? "pan"
                                                                                       : "music";
        unsigned x_bits;
        std::memcpy(&x_bits, &event.x, sizeof x_bits);
        const int frame = app->replay_active ? int(app->replay_consumed) - 1 : app->progress.stage_frame - 1;
        const bool voice = event.kind == AudioDevice::RequestObservation::Kind::Voice;
        const std::string id_text = voice ? std::to_string(unsigned(event.id)) : std::to_string(event.id);
        audio_observations += "{\"frame\":" + std::to_string(frame) +
                              ",\"sequence\":" + std::to_string(audio_sequence++) + ",\"kind\":\"" + kind +
                              "\",\"id\":" + id_text + ",\"value\":" + std::to_string(event.value) +
                              ",\"positional\":" + (event.positional ? "true" : "false") +
                              ",\"x_bits\":" + std::to_string(x_bits) + "}\n";
    };
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_audio_take() {
    using namespace th13::sdl;
    static std::string result;
    result.swap(audio_observations);
    audio_observations.clear();
    return result.c_str();
}

EMSCRIPTEN_KEEPALIVE void th13_debug_stage_pass(unsigned mask) { th13::StagePresentation::debug_pass_mask = mask; }

EMSCRIPTEN_KEEPALIVE void th13_debug_draw_probe(int enable) {
    th13::anm_draw_probe.reset();
    th13::anm_draw_probe.enabled = enable != 0;
}

EMSCRIPTEN_KEEPALIVE const unsigned* th13_debug_stats() {
    return th13::sdl::app ? reinterpret_cast<const unsigned*>(&th13::sdl::app->graphics.backend.stats) : nullptr;
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_pending() {
    using namespace th13::sdl;
    static std::string report;
    if (!app) return "{}";
    report = "{\"pending_destination\":" + std::to_string(app->pending_destination) +
             ",\"pending_load\":" + std::to_string(app->pending_load) +
             ",\"loading_startup\":" + (app->loading_startup ? "true" : "false") +
             ",\"loading_frames\":" + std::to_string(app->loading_frames) +
             ",\"pending_replay_file\":" + std::to_string(app->pending_replay_file) +
             ",\"pending_replay_stage_slot\":" + std::to_string(app->pending_replay_stage_slot) +
             ",\"replay_active\":" + (app->replay_active ? "true" : "false") +
             ",\"has_run\":" + (app->run ? "true" : "false") + ",\"has_title\":" + (app->title ? "true" : "false") +
             ",\"failure\":" + (app->failure.empty() ? "null" : "\"" + app->failure + "\"");
    if (app->title) {
        const auto& state = app->title->state;
        report += ",\"title_screen\":" + std::to_string(th13::i32(state.screen)) +
                  ",\"title_substate\":" + std::to_string(state.substate) +
                  ",\"title_age\":" + std::to_string(state.age.current) +
                  ",\"replay_index\":" + std::to_string(state.replay_index);
    }
    report += "}";
    return report.c_str();
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_draw_report() {
    using namespace th13::sdl;
    static std::string report;
    const auto& probe = th13::anm_draw_probe;
    report.clear();
    char line[128];
    std::snprintf(line, sizeof line, "calls=%u quads=%u shapes=%u strips=%u glyphs=%u other=%u\n", probe.calls,
                  probe.quads, probe.shapes_drawn, probe.strips, probe.glyphs, probe.other);
    report += line;
    for (th13::u32 i = 0; i < th13::AnmDrawProbe::kSlots; i++) {
        if (!probe.sprite_counts[i]) break;
        std::snprintf(line, sizeof line, "  sprite=%d count=%u size=%ux%u\n", probe.sprite_ids[i],
                      probe.sprite_counts[i], probe.sprite_sizes[i][0], probe.sprite_sizes[i][1]);
        report += line;
    }
    return report.c_str();
}
EMSCRIPTEN_KEEPALIVE int th13_debug_capture_begin(const char* path) {
    using namespace th13::sdl;
    if (!app || !path) return 0;
    if (!app->graphics.capture_begin(path)) {
        last_error = app->graphics.error.empty() ? "draws capture open failed" : app->graphics.error;
        return 0;
    }
    return 1;
}

EMSCRIPTEN_KEEPALIVE unsigned th13_debug_capture_end() {
    using namespace th13::sdl;
    if (!app) {
        last_error = "capture end without application";
        return ~0u;
    }
    if (!app->graphics.capture_finish()) {
        last_error = app->graphics.error;
        return ~0u;
    }
    return unsigned(app->graphics.submission_trace.frames_out());
}

EMSCRIPTEN_KEEPALIVE double th13_debug_capture_tell() {
    using namespace th13::sdl;
    return app ? double(app->graphics.submission_trace.output_position()) : -1;
}

EMSCRIPTEN_KEEPALIVE unsigned char* th13_debug_read_pixels(int x, int y, int w, int h) {
    static std::vector<unsigned char> buffer;
    if (w <= 0 || h <= 0 || w > 4096 || h > 4096) return nullptr;
    buffer.resize(size_t(w) * size_t(h) * 4);
    GLint read = 0, draw = 0, pack = 0, row = 0, rows = 0, pixels = 0, pbo = 0;
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &read);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &draw);
    glGetIntegerv(GL_PACK_ALIGNMENT, &pack);
    glGetIntegerv(GL_PACK_ROW_LENGTH, &row);
    glGetIntegerv(GL_PACK_SKIP_ROWS, &rows);
    glGetIntegerv(GL_PACK_SKIP_PIXELS, &pixels);
    glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &pbo);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ROW_LENGTH, 0);
    glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
    glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());
    glBindFramebuffer(GL_READ_FRAMEBUFFER, GLuint(read));
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GLuint(draw));
    glBindBuffer(GL_PIXEL_PACK_BUFFER, GLuint(pbo));
    glPixelStorei(GL_PACK_ALIGNMENT, pack);
    glPixelStorei(GL_PACK_ROW_LENGTH, row);
    glPixelStorei(GL_PACK_SKIP_ROWS, rows);
    glPixelStorei(GL_PACK_SKIP_PIXELS, pixels);
    return buffer.data();
}
}
