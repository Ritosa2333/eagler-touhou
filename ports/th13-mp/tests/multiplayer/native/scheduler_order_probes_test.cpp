#include "../../harness/test_framework.hpp"

#include "game/GameInput.hpp"
#include "sdl/ApplicationState.hpp"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

namespace {

using th13::i32;
using th13::u32;
using th13::u8;
using App = th13::sdl::ApplicationState;
using th13::FramePass;

constexpr float kFps = 60.0f;

const char* repo_root() {
    const char* root = std::getenv("TH13_NATIVE_REPO_ROOT");
    return root && *root ? root : "../../..";
}

bool read_repo_file(const std::string& relative, std::vector<u8>& out) {
    std::FILE* file = std::fopen((std::string(repo_root()) + "/" + relative).c_str(), "rb");
    if (!file) return false;
    std::fseek(file, 0, SEEK_END);
    const long size = std::ftell(file);
    std::fseek(file, 0, SEEK_SET);
    out.resize(size_t(size < 0 ? 0 : size));
    const size_t got = out.empty() ? 1 : std::fread(out.data(), 1, out.size(), file);
    std::fclose(file);
    return got == out.size();
}

bool pin_boot(App& app, const char* save_name) {
    ::setenv("TH13_SAVE_PATH", save_name, 1);
    const char* asset_root = std::getenv("TH13_NATIVE_ASSET_ROOT");
    if (!asset_root || !*asset_root) {
        std::printf("  失败 boot: TH13_NATIVE_ASSET_ROOT 未注入\n");
        return false;
    }
    if (!app.audio_device.initialize_for_test(
            asset_root, std::string(repo_root()) + "/games/th13/thbgmogg.dat")) {
        std::printf("  失败 boot: audio initialize_for_test: %s\n", app.audio_device.error.c_str());
        return false;
    }
    if (!app.initialize(false)) {
        std::printf("  失败 boot: initialize: %s\n", app.failure.c_str());
        return false;
    }
    return true;
}

bool boot_replay_battle(App& app, const std::vector<u8>& rpy, const char* save_name) {
    if (!pin_boot(app, save_name)) return false;
    if (!app.draw_loading(false)) {
        std::printf("  失败 boot: draw_loading: %s\n", app.failure.c_str());
        return false;
    }
    for (unsigned frame = 0; frame < App::kDebugLoadingFrames; ++frame)
        if (!app.advance_loading_caption()) {
            std::printf("  失败 boot: advance_loading_caption: %s\n", app.failure.c_str());
            return false;
        }
    if (!app.begin_replay_run(rpy.data(), rpy.size(), 0)) {
        std::printf("  失败 boot: begin_replay_run: %s\n", app.failure.c_str());
        return false;
    }
    return app.run != nullptr && app.failure.empty();
}

bool boot_live_run(App& app, const char* save_name) {
    if (!pin_boot(app, save_name)) return false;
    if (!app.start_game(1)) {
        std::printf("  失败 boot: start_game: %s\n", app.failure.c_str());
        return false;
    }
    return app.pending_destination == 13 && app.failure.empty();
}

struct Obs {
    int stage_frame = 0;
    u32 vis_seed = 0, vis_index = 0;
    u32 game_seed = 0, game_index = 0;
    u32 scenery_frames = 0;
    i32 enemy_vm_timer = -1;
    u32 enemy_vm_handle = 0;
    const void* enemy_vm_ptr = nullptr;
    int items_alive = 0;
    double item_x = 0, item_y = 0, item_anm_time = 0;
    int spirits_alive = 0;
    double spirit_anm = 0;

    bool same_world(const Obs& o) const {
        return stage_frame == o.stage_frame && vis_seed == o.vis_seed &&
               vis_index == o.vis_index && game_seed == o.game_seed &&
               game_index == o.game_index && scenery_frames == o.scenery_frames &&
               enemy_vm_timer == o.enemy_vm_timer && enemy_vm_handle == o.enemy_vm_handle &&
               enemy_vm_ptr == o.enemy_vm_ptr && items_alive == o.items_alive &&
               item_x == o.item_x && item_y == o.item_y &&
               item_anm_time == o.item_anm_time && spirits_alive == o.spirits_alive &&
               spirit_anm == o.spirit_anm;
    }
};

Obs observe(App& app) {
    Obs o;
    o.stage_frame = app.progress.stage_frame;
    o.vis_seed = app.visual.seed();
    o.vis_index = app.visual.index();
    o.game_seed = app.game_random.seed();
    o.game_index = app.game_random.index();
    if (app.run) {
        if (app.run->scenery) o.scenery_frames = app.run->scenery->frames;
        for (const auto& enemy : app.run->battle.sim.enemies) {
            if (enemy->dead) continue;
            if (const th13::AnmVm* vm = app.animations.registry.find(enemy->anm_slot_handle[0])) {
                o.enemy_vm_handle = enemy->anm_slot_handle[0];
                o.enemy_vm_ptr = vm;
                o.enemy_vm_timer = vm->timer.current;
                break;
            }
        }
        for (const auto& item : app.run->battle.item_manager.items()) {
            if (!item.alive) continue;
            ++o.items_alive;
            o.item_x += item.x;
            o.item_y += item.y;
            o.item_anm_time += item.anm_time;
        }
        for (const auto& spirit : app.run->battle.item_manager.spirits()) {
            if (!spirit.alive) continue;
            ++o.spirits_alive;
            o.spirit_anm += spirit.anm_frame;
        }
    }
    return o;
}

constexpr int kBrackets[] = {8, 10, 12, 14, 15, 17, 19, 21, 23, 27, 29, 33, 35};
constexpr int kBracketCount = 13;

std::vector<std::pair<int, Obs>>* g_probe_log = nullptr;

struct UpdateProbe {
    th13::FrameCallback callback{};
    App* app = nullptr;
    th13::FrameScheduler* attached_scheduler = nullptr;
    int priority = 0;
    static i32 run(void* p) {
        auto& probe = *static_cast<UpdateProbe*>(p);
        g_probe_log->push_back({probe.priority, observe(*probe.app)});
        return i32(th13::FrameAction::Continue);
    }
    bool attach(App& target, int priority_) {
        app = &target;
        priority = priority_;
        callback.owner = this;
        callback.enabled = true;
        callback.run = &UpdateProbe::run;
        if (target.scheduler.add(callback, FramePass::Update, priority) < 0) return false;
        attached_scheduler = &target.scheduler;
        return true;
    }
    void detach() {
        if (attached_scheduler) attached_scheduler->remove(callback);
        attached_scheduler = nullptr;
        app = nullptr;
    }
    ~UpdateProbe() { detach(); }
};

bool attach_all(App& app, std::vector<UpdateProbe*>& probes) {
    for (int priority : kBrackets) {
        probes.push_back(new UpdateProbe());
        if (!probes.back()->attach(app, priority)) return false;
    }
    return true;
}

void detach_all(std::vector<UpdateProbe*>& probes) {
    for (UpdateProbe* probe : probes) {
        probe->detach();
        delete probe;
    }
    probes.clear();
}

struct FrameProbes {
    std::vector<std::pair<int, Obs>> entries;
};

const Obs* at(const FrameProbes& frame, int priority) {
    for (const auto& entry : frame.entries)
        if (entry.first == priority) return &entry.second;
    return nullptr;
}

template <class Getter>
bool constant_across(const FrameProbes& frame, Getter getter, const int* priorities,
                     int count, const char* what, int frame_index) {
    bool ok = true;
    const decltype(getter(*at(frame, priorities[0]))) base = getter(*at(frame, priorities[0]));
    for (int i = 1; i < count; ++i) {
        const Obs* o = at(frame, priorities[i]);
        if (!o) continue;
        if (!(getter(*o) == base)) {
            std::printf("  失败 %s f%d: bracket %d 观测偏离（应在区间外持平）\n", what,
                        frame_index, priorities[i]);
            ok = false;
        }
    }
    return ok;
}

const int kBefore20[] = {8, 10, 12, 14, 15, 17, 19};
const int kAfter20[] = {21, 23, 27, 29, 33, 35};
const int kBefore13[] = {8, 10, 12};
const int kAfter13[] = {14, 15, 17, 19, 21, 23, 27, 29, 33, 35};

void assert_frame_order(const FrameProbes& frame, int frame_index, bool expect_sf_advance) {
    CHECK_EQ(frame.entries.size(), size_t(kBracketCount));
    for (size_t i = 0; i < frame.entries.size() && i < size_t(kBracketCount); ++i)
        CHECK_EQ(frame.entries[i].first, kBrackets[i]);

    const Obs* first = at(frame, kBrackets[0]);
    if (first && at(frame, 21)) {
        CHECK(constant_across(frame, [](const Obs& o) { return o.stage_frame; },
                              kBefore20, 7, "stage_frame", frame_index));
        CHECK_EQ(at(frame, 19)->stage_frame + (expect_sf_advance ? 1 : 0),
                 at(frame, 21)->stage_frame);
        CHECK(constant_across(frame, [](const Obs& o) { return o.stage_frame; },
                              kAfter20, 6, "stage_frame", frame_index));
    }
    if (at(frame, 14)) {
        CHECK(constant_across(frame, [](const Obs& o) { return o.scenery_frames; },
                              kBefore13, 3, "scenery_frames", frame_index));
        CHECK_EQ(at(frame, 12)->scenery_frames + 1, at(frame, 14)->scenery_frames);
        CHECK(constant_across(frame, [](const Obs& o) { return o.scenery_frames; },
                              kAfter13, 10, "scenery_frames", frame_index));
    }
}

}

TEST(scheduler_order_bracket_monotonic_replay) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    if (rpy.empty()) return;
    App app;
    std::vector<UpdateProbe*> probes;
    g_probe_log = new std::vector<std::pair<int, Obs>>();
    CHECK(attach_all(app, probes));
    CHECK(boot_replay_battle(app, rpy, "t5so-replay-save.bin"));
    if (!app.failure.empty() || !app.run) {
        detach_all(probes);
        delete g_probe_log;
        g_probe_log = nullptr;
        return;
    }
    th13::GameInput input;
    constexpr int kFrames = 100;
    for (int F = 0; F < kFrames; ++F) {
        const size_t begin = g_probe_log->size();
        input.update(0);
        CHECK(app.update(input, false));
        CHECK(app.commit(kFps));
        CHECK(app.present());
        if (!app.failure.empty()) {
            std::printf("  失败 drive f%d: %s\n", F, app.failure.c_str());
            break;
        }
        FrameProbes frame;
        for (size_t i = begin; i < g_probe_log->size(); ++i)
            frame.entries.push_back((*g_probe_log)[i]);
        const int battle_frame = app.progress.stage_frame - 1;
        assert_frame_order(frame, battle_frame, true);

        const Obs* pre19 = at(frame, 19);
        const Obs* post21 = at(frame, 21);
        const Obs* pre33 = at(frame, 33);
        const Obs* post35 = at(frame, 35);
        if (pre19 && post21 && pre33 && post35 && post21->enemy_vm_timer >= 0) {
            const bool born_this_frame = pre19->enemy_vm_ptr != post21->enemy_vm_ptr;
            const bool same_vm_at_tail = post21->enemy_vm_ptr != nullptr &&
                                         post21->enemy_vm_ptr == pre33->enemy_vm_ptr &&
                                         pre33->enemy_vm_ptr == post35->enemy_vm_ptr;
            if (same_vm_at_tail) {
                CHECK_EQ(pre33->enemy_vm_timer, post21->enemy_vm_timer);
                if (post35->enemy_vm_timer == pre33->enemy_vm_timer) {
                    std::printf("  失败 evt f%d: 主链@34 tick 未改变槽 VM 计时器\n",
                                app.progress.stage_frame - 1);
                }
                CHECK(post35->enemy_vm_timer != pre33->enemy_vm_timer);
            }
            if (born_this_frame) {
                CHECK(post21->enemy_vm_timer >= 0);
            }
        }
        const Obs* pre23 = at(frame, 23);
        const Obs* post27 = at(frame, 27);
        if (pre23 && post27 && pre19 && post21) {
            CHECK_EQ(post21->items_alive, pre23->items_alive);
            CHECK_EQ(post21->item_x, pre23->item_x);
            CHECK_EQ(post21->item_y, pre23->item_y);
            CHECK_EQ(post21->item_anm_time, pre23->item_anm_time);
            CHECK_EQ(post21->spirits_alive, pre23->spirits_alive);
            CHECK_EQ(post21->spirit_anm, pre23->spirit_anm);
            const bool item_activity =
                pre23->items_alive != post27->items_alive || pre23->item_x != post27->item_x ||
                pre23->item_y != post27->item_y || pre23->item_anm_time != post27->item_anm_time;
            const bool spirit_activity =
                pre23->spirits_alive != post27->spirits_alive ||
                pre23->spirit_anm != post27->spirit_anm;
            if (pre23->items_alive > 0) CHECK(item_activity);
            if (pre23->spirits_alive > 0) CHECK(spirit_activity);
        }
        const Obs* pre21_19 = at(frame, 19);
        const Obs* post21_21 = at(frame, 21);
        const Obs* first_obs = at(frame, 8);
        const Obs* last_obs = at(frame, 35);
        if (pre21_19 && post21_21 && first_obs && last_obs) {
            const u32 vis_total = last_obs->vis_index - first_obs->vis_index;
            const u32 vis_19_21 = post21_21->vis_index - pre21_19->vis_index;
            CHECK(vis_19_21 <= vis_total);
            if (battle_frame == 8 || battle_frame == 16 || battle_frame == 24 ||
                battle_frame == 64 || battle_frame == 96 || battle_frame == 98) {
                CHECK(vis_19_21 > 0);
            }
            if (battle_frame == 60 || battle_frame == 90 || battle_frame == 98) {
                CHECK(post21_21->game_index - pre21_19->game_index > 0);
            }
        }
    }
    detach_all(probes);
    delete g_probe_log;
    g_probe_log = nullptr;
}

TEST(scheduler_order_bomb_shake_bracket16_live) {
    App app;
    std::vector<UpdateProbe*> probes;
    g_probe_log = new std::vector<std::pair<int, Obs>>();
    CHECK(attach_all(app, probes));
    CHECK(boot_live_run(app, "t5so-live-save.bin"));
    if (!app.failure.empty()) {
        detach_all(probes);
        delete g_probe_log;
        g_probe_log = nullptr;
        return;
    }
    th13::GameInput input;
    constexpr u32 kZ = 0x01, kX = 0x02;
    for (int barrier = 0; barrier < 2 && !app.run; ++barrier) {
        input.update(0);
        CHECK(app.step(input, kFps, false));
        if (!app.failure.empty()) {
            std::printf("  失败 live barrier step %d: %s\n", barrier, app.failure.c_str());
            detach_all(probes);
            delete g_probe_log;
            g_probe_log = nullptr;
            return;
        }
    }
    if (!app.failure.empty() || !app.run) {
        std::printf("  失败 live boot: 对局未建立\n");
        detach_all(probes);
        delete g_probe_log;
        g_probe_log = nullptr;
        return;
    }
    int shake_frames = 0;
    for (int F = 1; F <= 400; ++F) {
        const u32 held = (F >= 1 ? kZ : 0u) | (F == 66 || F == 67 ? kX : 0u);
        const size_t begin = g_probe_log->size();
        input.update(held);
        CHECK(app.update(input, false));
        CHECK(app.commit(kFps));
        CHECK(app.present());
        if (!app.failure.empty()) {
            std::printf("  失败 live drive f%d: %s\n", F, app.failure.c_str());
            break;
        }
        FrameProbes frame;
        for (size_t i = begin; i < g_probe_log->size(); ++i)
            frame.entries.push_back((*g_probe_log)[i]);
        assert_frame_order(frame, app.progress.stage_frame - 1, true);
        const Obs* pre15 = at(frame, 15);
        const Obs* post17 = at(frame, 17);
        if (pre15 && post17 && post17->vis_index != pre15->vis_index) {
            ++shake_frames;
            CHECK_EQ(pre15->stage_frame, post17->stage_frame);
        }
    }
    detach_all(probes);
    delete g_probe_log;
    g_probe_log = nullptr;
    CHECK_EQ(shake_frames, 7);
}

TEST(scheduler_order_replay_fast_forward_cut_30) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    if (rpy.empty()) return;
    App app;
    std::vector<UpdateProbe*> probes;
    g_probe_log = new std::vector<std::pair<int, Obs>>();
    CHECK(attach_all(app, probes));
    CHECK(boot_replay_battle(app, rpy, "t5so-ff-save.bin"));
    if (!app.failure.empty() || !app.run) {
        detach_all(probes);
        delete g_probe_log;
        g_probe_log = nullptr;
        return;
    }
    th13::GameInput input;
    for (int F = 0; F < 8; ++F) {
        input.update(0);
        CHECK(app.update(input, false));
        CHECK(app.commit(kFps));
        CHECK(app.present());
        if (!app.failure.empty()) {
            std::printf("  失败 ff warmup f%d: %s\n", F, app.failure.c_str());
            break;
        }
    }
    for (int F = 8; F <= 11; ++F) {
        const int sf0 = app.progress.stage_frame;
        const size_t begin = g_probe_log->size();
        input.update(0x01);
        CHECK(app.update(input, false));
        if (!app.failure.empty()) {
            std::printf("  失败 ff drive f%d: %s\n", F, app.failure.c_str());
            break;
        }
        std::vector<FrameProbes> passes;
        for (size_t i = begin; i < g_probe_log->size(); ++i) {
            const int priority = (*g_probe_log)[i].first;
            if (priority == 8 || passes.empty()) passes.push_back(FrameProbes{});
            passes.back().entries.push_back((*g_probe_log)[i]);
        }
        CHECK(passes.size() >= 2);
        CHECK_EQ(app.progress.stage_frame, sf0 + i32(passes.size()));
        size_t complete_passes = 0;
        for (const FrameProbes& pass : passes) {
            const bool complete = pass.entries.size() == size_t(kBracketCount);
            if (complete) {
                ++complete_passes;
                CHECK_EQ(pass.entries.back().first, 35);
                for (size_t i = 0; i < pass.entries.size(); ++i)
                    CHECK_EQ(pass.entries[i].first, kBrackets[i]);
            } else {
                CHECK(pass.entries.size() < size_t(kBracketCount));
                bool saw_late = false;
                for (const auto& entry : pass.entries)
                    if (entry.first >= 33) saw_late = true;
                if (saw_late) {
                    std::printf("  失败 ff f%d: 截断 pass 触发了 >=33 bracket（快进未在 30 截断）\n", F);
                }
                CHECK(!saw_late);
            }
        }
        CHECK_EQ(complete_passes, size_t(1));
        CHECK(app.commit(kFps));
        CHECK(app.present());
    }
    detach_all(probes);
    delete g_probe_log;
    g_probe_log = nullptr;
}

int main() { return th13test::run_all("m1.unit_scheduler_order_probes"); }
