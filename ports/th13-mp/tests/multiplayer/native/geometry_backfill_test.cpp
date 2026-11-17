// m1.unit_geometry_backfill_timing —— M1/T5 玩法几何回填时点探针（workflow
// m1.update_present_equivalence 的本层 unit 件之一）。RunScene::update@20
// （RunScene.cpp:689/724 的 presentation->sync）把敌机模拟位回填进表现层
// VM visual.translation（StagePresentation.cpp:1480-1482 敌机槽 VM 跟随）；
// 本测试经 scheduler.add 公开面在 Update 优先级 19（回填前）与 21（回填后）
// 挂探针，逐帧采样 app.run->battle.sim.enemies 的 position 与槽0 VM 的
// visual.translation，断言：
//   1) 敌机模拟位跨帧冻结：position@19 == position@上一帧@21（sim 只在
//      battle.step@20 内推进——若 sim 泄漏进 22..34/commit/present，此断言红）。
//   2) 回填新鲜：translation@21 == anm_slot_offset@21 + position@21 逐位成立
//      （sync 在 (19,21) 内随本帧步进完成——若 sync 被搬到 Draw/commit，
//      @21 采样仍是上帧值而 position@21 已前移，此断言红）。
//   3) 移动敌机（position@21 != position@19）的回填跟随：translation@21
//      跟踪新位，同时 translation@19 == offset@19 + position@19（上帧 sync 值，
//      实测 ANM 主链@34 不触碰槽0 VM translation——若未来 ANM 脚本开始写
//      translation，此半断言给出明确红灯供人复核，不静默放宽）。
//   4) 出生帧：@19 不存在、@21 出现的敌机（ECL 303/306 于 battle.step@20 同步
//      建槽 VM）其 translation 在 @21 已按出生位回填。
// 探针零生产钩子；窗口=回放 th13_01 battle f55..f100（敌机存活且逐帧移动，
// 含 f98 掉落道具新敌机出生帧）。
#include "../../harness/test_framework.hpp"

#include "game/GameInput.hpp"
#include "sdl/ApplicationState.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

namespace {

using th13::i32;
using th13::u32;
using th13::u8;
using App = th13::sdl::ApplicationState;

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

bool boot_replay_battle(App& app, const std::vector<u8>& rpy, const char* save_name) {
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

// ---- 敌机几何采样（serial/position/槽0 VM translation+offset，位级）----
struct EnemySample {
    u32 serial = 0;
    float position[3] = {};
    float translation[3] = {};
    float offset[3] = {};
    u32 vm_present = 0;
};

std::vector<EnemySample> sample_enemies(App& app) {
    std::vector<EnemySample> out;
    if (!app.run) return out;
    for (const auto& enemy : app.run->battle.sim.enemies) {
        if (enemy->dead) continue;
        EnemySample s;
        s.serial = enemy->serial;
        std::memcpy(s.position, enemy->position, sizeof s.position);
        if (const th13::AnmVm* vm = app.animations.registry.find(enemy->anm_slot_handle[0])) {
            s.vm_present = 1;
            s.translation[0] = vm->visual.translation.x;
            s.translation[1] = vm->visual.translation.y;
            s.translation[2] = vm->visual.translation.z;
            s.offset[0] = enemy->anm_slot_offset[0].x;
            s.offset[1] = enemy->anm_slot_offset[0].y;
            s.offset[2] = enemy->anm_slot_offset[0].z;
        }
        out.push_back(s);
    }
    return out;
}

const EnemySample* find_serial(const std::vector<EnemySample>& v, u32 serial) {
    for (const EnemySample& s : v)
        if (s.serial == serial) return &s;
    return nullptr;
}

bool floats_eq(const float* a, const float* b) {
    return std::memcmp(a, b, sizeof(float) * 3) == 0;
}

// StagePresentation.cpp:1480-1482 的回填式（offset + position，逐位）。
bool translation_synced(const EnemySample& s) {
    return s.vm_present && s.translation[0] == s.offset[0] + s.position[0] &&
           s.translation[1] == s.offset[1] + s.position[1] &&
           s.translation[2] == s.offset[2] + s.position[2];
}

// ---- Update 段 @19/@21 探针 ----
std::vector<std::vector<EnemySample>>* g_at19 = nullptr;
std::vector<std::vector<EnemySample>>* g_at21 = nullptr;

struct GeometryProbe {
    th13::FrameCallback callback{};
    App* app = nullptr;
    th13::FrameScheduler* attached_scheduler = nullptr;  // 登记归属（FrameCallback::scheduler private）
    int priority = 0;
    static i32 run(void* p) {
        auto& probe = *static_cast<GeometryProbe*>(p);
        if (probe.priority == 19) g_at19->push_back(sample_enemies(*probe.app));
        else g_at21->push_back(sample_enemies(*probe.app));
        return i32(th13::FrameAction::Continue);
    }
    bool attach(App& target, int priority_) {
        app = &target;
        priority = priority_;
        callback.owner = this;
        callback.enabled = true;
        callback.run = &GeometryProbe::run;
        if (target.scheduler.add(callback, th13::FramePass::Update, priority) < 0) return false;
        attached_scheduler = &target.scheduler;
        return true;
    }
    void detach() {
        if (attached_scheduler) attached_scheduler->remove(callback);
        attached_scheduler = nullptr;
        app = nullptr;
    }
    ~GeometryProbe() { detach(); }
};

}  // namespace

TEST(geometry_backfill_probe_19_21) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    if (rpy.empty()) return;
    App app;
    GeometryProbe probe19, probe21;
    g_at19 = new std::vector<std::vector<EnemySample>>();
    g_at21 = new std::vector<std::vector<EnemySample>>();
    CHECK(probe19.attach(app, 19));
    CHECK(probe21.attach(app, 21));
    CHECK(boot_replay_battle(app, rpy, "t5gb-save.bin"));
    if (!app.failure.empty() || !app.run) {
        probe19.detach();
        probe21.detach();
        delete g_at19;
        delete g_at21;
        g_at19 = g_at21 = nullptr;
        return;
    }
    th13::GameInput input;
    // warm f0..f54（对话窗，无敌机）；采样窗 f55..f100（敌机存活逐帧移动）。
    for (int F = 0; F < 55; ++F) {
        input.update(0);
        CHECK(app.update(input, false));
        CHECK(app.commit(kFps));
        CHECK(app.present());
        if (!app.failure.empty()) {
            std::printf("  失败 warmup f%d: %s\n", F, app.failure.c_str());
            return;
        }
    }
    constexpr int kSampleFrames = 46;  // battle f55..f100
    g_at19->clear();  // 探针自 battle f0 起随 update 触发；采样窗从 f55 重新计
    g_at21->clear();
    int checked_pairs = 0, checked_sync21 = 0, checked_sync19 = 0, moving_pairs = 0;
    int spawn_checked = 0;
    bool fail_position_freeze = false, fail_sync21 = false, fail_sync19 = false;
    bool saw_moving = false, saw_spawn = false;
    for (int F = 0; F < kSampleFrames; ++F) {
        input.update(0);
        CHECK(app.update(input, false));
        CHECK(app.commit(kFps));
        CHECK(app.present());
        if (!app.failure.empty()) {
            std::printf("  失败 drive f%d: %s\n", F + 55, app.failure.c_str());
            return;
        }
        CHECK_EQ(g_at19->size(), size_t(F + 1));
        CHECK_EQ(g_at21->size(), size_t(F + 1));
        const std::vector<EnemySample>& pre19 = (*g_at19)[size_t(F)];
        const std::vector<EnemySample>& post21 = (*g_at21)[size_t(F)];
        const std::vector<EnemySample>* prev21 =
            F > 0 ? &(*g_at21)[size_t(F - 1)] : nullptr;
        CHECK_EQ(app.progress.stage_frame - 1, 55 + F);  // battle 帧号对齐

        for (const EnemySample& a : pre19) {
            // (1) 位置跨帧冻结（sim 只在 battle.step@20 内推进）。
            if (prev21) {
                if (const EnemySample* pp = find_serial(*prev21, a.serial)) {
                    ++checked_pairs;
                    if (!floats_eq(pp->position, a.position)) {
                        if (!fail_position_freeze) {
                            std::printf("  失败 freeze f%d serial=%u: position@19 与上一帧"
                                        "@21 不等（sim 泄漏出 (19,21)）\n", 55 + F, a.serial);
                        }
                        fail_position_freeze = true;
                    }
                }
            }
            // (3a) @19 的 translation 是上一帧 sync 值（offset+position）。
            if (a.vm_present) {
                ++checked_sync19;
                if (!translation_synced(a)) {
                    if (!fail_sync19) {
                        std::printf("  失败 stale f%d serial=%u: translation@19=%f,%f,%f "
                                    "≠ offset+position@19=%f,%f,%f（上帧 sync 值被改写）\n",
                                    55 + F, a.serial, a.translation[0], a.translation[1],
                                    a.translation[2], a.offset[0] + a.position[0],
                                    a.offset[1] + a.position[1], a.offset[2] + a.position[2]);
                    }
                    fail_sync19 = true;
                }
            }
            // (2)(3b) @21 回填新鲜 + 移动跟随。
            if (const EnemySample* b = find_serial(post21, a.serial)) {
                if (b->vm_present) {
                    ++checked_sync21;
                    if (!translation_synced(*b)) {
                        if (!fail_sync21) {
                            std::printf("  失败 fresh f%d serial=%u: translation@21=%f,%f,%f "
                                        "≠ offset+position@21=%f,%f,%f（sync 不在 (19,21) 或"
                                        "未随步进回填）\n", 55 + F, a.serial, b->translation[0],
                                        b->translation[1], b->translation[2],
                                        b->offset[0] + b->position[0],
                                        b->offset[1] + b->position[1],
                                        b->offset[2] + b->position[2]);
                        }
                        fail_sync21 = true;
                    }
                    if (!floats_eq(a.position, b->position)) {
                        ++moving_pairs;
                        saw_moving = true;
                    }
                }
            }
        }
        // (4) 出生帧：@19 不存在、@21 出现且带槽 VM 的敌机已按出生位回填。
        for (const EnemySample& b : post21) {
            if (!find_serial(pre19, b.serial) && b.vm_present) {
                ++spawn_checked;
                saw_spawn = true;
                if (!translation_synced(b)) {
                    std::printf("  失败 spawn f%d serial=%u: 出生帧 translation 未在 @21 回填\n",
                                55 + F, b.serial);
                    fail_sync21 = true;
                }
            }
        }
    }
    probe19.detach();
    probe21.detach();
    delete g_at19;
    delete g_at21;
    g_at19 = g_at21 = nullptr;
    // 采样面非空转：窗口内必须观察到移动敌机与（f98 掉落）出生 VM。
    std::printf("# geometry probe: pairs=%d sync19=%d sync21=%d moving=%d spawn=%d\n",
                checked_pairs, checked_sync19, checked_sync21, moving_pairs, spawn_checked);
    CHECK(checked_pairs > 0);
    CHECK(checked_sync21 > 0);
    CHECK(saw_moving);
    CHECK(saw_spawn);
    CHECK(!fail_position_freeze);
    CHECK(!fail_sync21);
    CHECK(!fail_sync19);
}

int main() { return th13test::run_all("m1.unit_geometry_backfill_timing"); }
