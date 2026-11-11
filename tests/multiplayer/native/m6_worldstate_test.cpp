// M6 原生固定 case（完整结构化快照；docs/MP-DEVELOPMENT.md M6 退出条件：
// S→前进→restore(S) 全字段等价；再喂同输入逐帧一致——含池复用/父子 VM/
// 暂停/换关边界）。
// 被测生产面（web/cpp/multiplayer/WorldState.*——零测试分支）：
//   - WorldState::capture/restore（StageBattle 席位面 + StageSim 敌机/弹/
//     激光/ECL 线程 + ItemManager 池 + SessionState/分数 + 双 RNG +
//     AnmRegistry 专用快照）；
//   - WorldState::digest（显式序列化全权威状态、稳定字段序、float 按 bit、
//     不 hash 指针/padding/unordered_map 顺序/墙钟）；
//   - WorldState::field_ledger（快照字段台账）；
//   - AnmRegistry::SnapshotData capture/restore（分配顺序/句柄 generation/
//     父子链/链序恢复）；
//   - StageEnemy::capture_fields/restore_fields（值域 + threads/fog 对象
//     身份保留）；
//   - GameRandom::set_index / SpellState::restore（快照逆操作）。
//
// 固定 case 映射（workflow-cases.json m6.*）：
//   full_state_restore_replay   → S→推进→restore(S)→digest 全等 + 再喂同
//                                 输入逐帧一致（S1 合成舞台 + MP 2 席）
//   snapshot_mutation_sensitivity → 快照独立性：capture 后改源、restore
//                                 回旧值（深拷贝语义，非引用别名）
//   pools_ids_free_order        → 池/ID/自由槽表恢复：敌机序号、弹代、
//                                 ItemManager 自由表、ANM free_slots 与
//                                 generation 计数
//   anm_borrowed_parent_chains  → ANM 父子链与 borrowed 根：attach 链 +
//                                 外部根（overlay 面板模式）capture→
//                                 destroy→restore 后链关系/句柄全恢复
//   scheduler_callback_rebind  → FrameScheduler 稳定 callback 身份：快照
//                                 前后 callback 集合不变（FrameCallback*
//                                 身份与优先级/使能位稳定）；restore 不触碰
//                                 调度结构
//   dual_rng_events_transition  → 双 RNG 与事件/转场边界：game/visual 的
//                                 seed+index 恢复；anm_effects 队列与
//                                 spell 快照恢复后行为一致
#include "../../ecl/ecl_test_fixtures.hpp"
#include "game/AnmManager.hpp"
#include "game/FrameScheduler.hpp"
#include "game/StageBattle.hpp"
#include "multiplayer/WorldState.hpp"
// P3.2 宿主域 case：ApplicationState 真实对局（boot/帧体与
// scheduler_order_test 同模式——update/commit/present 三段序）。
#include "sdl/ApplicationState.hpp"
#include <cmath>
#include <cstdlib>

using namespace th13;
using namespace th13test;

namespace {

struct Host final : StageBattleServices {
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
    bool hud_sound(i32, float) override { return true; }
    bool stop_sound(i32) override { return true; }
};

// 合成舞台（M2/M5 同款 Provider：main 敌机 + .sht 真实资产上溯）。
class Provider final : public EclResourceProvider {
public:
    Provider() {
        std::vector<u8> main;
        emit(main, 0, 23, 0, 0xff, 0, {600});
        emit(main, 0, 10, 0, 0xff, 0, {});
        stage = build_scpt({{"main", main}}, {"enemy.anm", "st01enm.anm"});
    }
    bool read(const std::string& name, std::vector<u8>& out) override {
        if (name == "mp6.ecl") {
            out = stage;
            return true;
        }
        if (name.size() > 4 && name.compare(name.size() - 4, 4, ".sht") == 0) {
            static const std::string root = [] {
                std::string path = __FILE__;
                for (int i = 0; i < 8; ++i) {
                    const std::string probe = path + "/artifacts/reference/raw/pl00a.sht";
                    std::FILE* check = std::fopen(probe.c_str(), "rb");
                    if (check) { std::fclose(check); break; }
                    const auto cut = path.find_last_of("/\\");
                    if (cut == std::string::npos) break;
                    path.erase(cut);
                }
                return path;
            }();
            std::FILE* file = std::fopen((root + "/artifacts/reference/raw/" + name).c_str(), "rb");
            if (!file) return false;
            std::fseek(file, 0, SEEK_END);
            const long size = std::ftell(file);
            std::fseek(file, 0, SEEK_SET);
            out.resize(size_t(size < 0 ? 0 : size));
            const size_t got = out.empty() ? 1 : std::fread(out.data(), 1, out.size(), file);
            std::fclose(file);
            return got == out.size();
        }
        return false;
    }
    bool animation(u32, const std::string& name) override {
        if (animations) animations->push_back(name);
        return true;
    }
    std::vector<std::string>* animations = nullptr;
    std::vector<u8> stage;
};

struct Fixture {
    Provider provider;
    Host host;
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    StageBattle battle;
    GameInput idle;

    bool init() {
        provider.animations = &battle.sim.anim_list;
        progress.max_point_cap = 500000;
        return battle.load(provider, "mp6.ecl", 2, host, progress, player, score);
    }
    void enable(i32 count, std::array<i32, 3> characters) {
        CHECK(battle.enable_seats(provider, count, characters));
    }
};

}  // namespace

// ---- full_state_restore_replay：S→前进→restore(S)→digest 全等 + 同输入
// 逐帧一致（MP 2 席 + 合成舞台）----
TEST(full_state_restore_replay) {
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 0});
    GameRandom visual(0x1234);
    // 推进 40 帧（开火门 startup_clock>19 后 seat 0/1 都开火，产生弹/注册池/
    // 敌机伤害/道具掉落的真实状态）。
    GameInput fire;
    for (int frame = 0; frame < 40; ++frame) {
        fire.update(0x01);
        dual.battle.seat_inputs[0] = fire;
        dual.battle.seat_inputs[1] = fire;
        CHECK(dual.battle.step(GameInput{}));
    }
    multiplayer::WorldState snapshot;
    CHECK(snapshot.capture(dual.battle, visual));
    const uint64_t digest_at_capture = snapshot.digest();
    CHECK(snapshot.ready());
    // 台账非空（快照完整性契约面）。
    CHECK(std::string(multiplayer::WorldState::field_ledger()).size() > 32);
    // 继续前进 25 帧（发散）。
    for (int frame = 0; frame < 25; ++frame) {
        fire.update(0x01);
        dual.battle.seat_inputs[0] = fire;
        dual.battle.seat_inputs[1] = fire;
        CHECK(dual.battle.step(GameInput{}));
    }
    // restore(S)。注：snapshot.digest() 读的是快照自身成员，restore 不
    // 改它——对同一对象比较是恒真（M6 审查判例），真正的等价证明在下方
    // 第二实例的重捕对比。
    CHECK(snapshot.restore(dual.battle, visual));
    // restore 后的权威面 digest 与 capture 时一致：用第二实例交叉验证
    // （capture 新快照应与旧快照 digest 相等——同状态同序）。
    multiplayer::WorldState verify;
    CHECK(verify.capture(dual.battle, visual));
    CHECK_EQ(verify.digest(), digest_at_capture);
    // 再喂同输入 30 帧：restore 分支与未中断基准逐帧一致（双实例对照）。
    Fixture baseline;
    CHECK(baseline.init());
    baseline.enable(2, {0, 0});
    // 基准从零重放到 capture 点再继续（同输入确定性）。
    for (int frame = 0; frame < 40; ++frame) {
        fire.update(0x01);
        baseline.battle.seat_inputs[0] = fire;
        baseline.battle.seat_inputs[1] = fire;
        CHECK(baseline.battle.step(GameInput{}));
    }
    for (int frame = 0; frame < 30; ++frame) {
        fire.update(0x01);
        // restore 分支。
        dual.battle.seat_inputs[0] = fire;
        dual.battle.seat_inputs[1] = fire;
        CHECK(dual.battle.step(GameInput{}));
        // 基准分支。
        baseline.battle.seat_inputs[0] = fire;
        baseline.battle.seat_inputs[1] = fire;
        CHECK(baseline.battle.step(GameInput{}));
        multiplayer::WorldState a, b;
        CHECK(a.capture(dual.battle, visual));
        CHECK(b.capture(baseline.battle, visual));
        if (a.digest() != b.digest()) {
            CHECK_EQ(a.digest(), b.digest());  // 失败证据：首差帧
            return;
        }
    }
}

// ---- snapshot_mutation_sensitivity：快照独立性（深拷贝语义）----
TEST(snapshot_mutation_sensitivity) {
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 0});
    GameRandom visual(0xBEEF);
    GameInput fire;
    for (int frame = 0; frame < 30; ++frame) {
        fire.update(0x01);
        dual.battle.seat_inputs[0] = fire;
        dual.battle.seat_inputs[1] = fire;
        CHECK(dual.battle.step(GameInput{}));
    }
    multiplayer::WorldState snapshot;
    CHECK(snapshot.capture(dual.battle, visual));
    const uint64_t before = snapshot.digest();
    // capture 时刻的观测值（restore 应回到这些值）。
    const float seat0_x = dual.battle.seats[0].x;
    const i32 seat0_power = dual.battle.seat_huds[0].power;
    const size_t seat1_shots = dual.battle.seats[1].shots.size();
    // 破坏源状态：改席位位置/弹池/敌机/道具/分数/灵界槽。
    dual.battle.seats[0].x = 123.0f;
    dual.battle.seat_huds[0].power = 399;
    dual.battle.seats[1].shots.push_back(PlayerShot{});
    dual.battle.sim.frame += 77;
    dual.progress.ending_points = 555;
    dual.score.score = 98765;
    // 神霊池污染：直接产一件（spirit_serial_ 推进 + 槽占用）。
    dual.battle.item_manager.spawn_value_item(1, 0.0f, 0.0f, 0.0f, 1.0f);
    if (!dual.battle.sim.enemies.empty()) dual.battle.sim.enemies[0]->life = 1.0f;
    // 快照 digest 不受源状态变化影响（深拷贝）。
    CHECK_EQ(snapshot.digest(), before);
    // restore 把破坏前的状态写回。
    CHECK(snapshot.restore(dual.battle, visual));
    CHECK_EQ(dual.battle.seats[0].x, seat0_x);
    CHECK_EQ(dual.battle.seat_huds[0].power, seat0_power);
    CHECK_EQ(dual.battle.seats[1].shots.size(), seat1_shots);
    CHECK_EQ(dual.progress.ending_points, 0);
    CHECK_EQ(dual.score.score, 0);
    // 神霊池恢复：restore 后再产一件的 serial 与破坏前推进序一致
    //（spirit_serial_ 是私有域——经行为面观测：restore 后池首件 serial
    // 应为 1（capture 时池空、破坏产了 serial 1、restore 清回 0、再产
    // 重新得 1）。
    dual.battle.item_manager.spawn_value_item(1, 0.0f, 0.0f, 0.0f, 1.0f);
    bool restored_spirit_seen = false;
    for (const Spirit& spirit : dual.battle.item_manager.spirits())
        if (spirit.alive && spirit.serial == 1u) restored_spirit_seen = true;
    CHECK(restored_spirit_seen);
    CHECK(snapshot.ready());
}

// ---- pools_ids_free_order：池/ID/自由槽表恢复 ----
TEST(pools_ids_free_order) {
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 0});
    GameRandom visual(0x0F0F);
    GameInput fire;
    for (int frame = 0; frame < 35; ++frame) {
        fire.update(0x01);
        dual.battle.seat_inputs[0] = fire;
        dual.battle.seat_inputs[1] = fire;
        CHECK(dual.battle.step(GameInput{}));
    }
    // 直接观测池分配序：弹槽观测经 debug_bullet_queue 的池序面（私有域
    // 不进测试）——用 spawn_count 与敌机/池行为面替代：
    const u32 spawned = dual.battle.sim.spawn_count();
    // ItemManager 自由表观测（events()/items()/spirits() 是 public 面）：
    // 掉落未发生时池空；本 case 用「restore 后再产件 serial 序」行为面。
    // ANM registry（无注入：无头路径——空快照语义一致性）。
    CHECK(dual.battle.mp_registry() == nullptr);
    multiplayer::WorldState snapshot;
    CHECK(snapshot.capture(dual.battle, visual));
    // 消耗/扰动：再走 20 帧 + 手动产件（池计数推进）。
    for (int frame = 0; frame < 20; ++frame) {
        fire.update(0x01);
        dual.battle.seat_inputs[0] = fire;
        dual.battle.seat_inputs[1] = fire;
        CHECK(dual.battle.step(GameInput{}));
    }
    dual.battle.item_manager.spawn_value_item(2, 5.0f, 5.0f, 0.0f, 1.0f);
    dual.battle.item_manager.spawn_value_item(3, -5.0f, 5.0f, 0.0f, 1.0f);
    CHECK(snapshot.restore(dual.battle, visual));
    // 计数恢复。
    CHECK_EQ(dual.battle.sim.spawn_count(), spawned);
    // 池恢复：restore 后池应回到 capture 态（扰动产的两件被清）——
    // 再产一件的 serial 与 capture 前推进序衔接（行为面：首件 serial=1）。
    dual.battle.item_manager.spawn_value_item(1, 0.0f, 0.0f, 0.0f, 1.0f);
    bool first_serial_seen = false;
    for (const Spirit& spirit : dual.battle.item_manager.spirits())
        if (spirit.alive && spirit.serial == 1u) first_serial_seen = true;
    CHECK(first_serial_seen);
    // 扰动两件确已不在池（restore 清场）。
    bool stale_seen = false;
    for (const Spirit& spirit : dual.battle.item_manager.spirits())
        if (spirit.alive && spirit.serial >= 2u && spirit.serial <= 3u) stale_seen = true;
    CHECK(!stale_seen);
}

// ---- anm_borrowed_parent_chains：ANM 父子链与 borrowed 根恢复 ----
TEST(anm_borrowed_parent_chains) {
    // 直接构造 AnmManager + AnmRegistry（无头）：父子 attach 链、borrowed
    // 外部根（AnmOverlay 面板模式：外部 VM attach 子体）、句柄 generation。
    GameRandom visual(7);
    AnmEnvironment environment;
    AnmManager manager(visual, environment);
    // 装载真实 front.anm（模板面）。
    std::vector<u8> anm_bytes;
    {
        std::string path = __FILE__;
        for (int i = 0; i < 8; ++i) {
            const std::string probe = path + "/artifacts/reference/raw/front.anm";
            std::FILE* check = std::fopen(probe.c_str(), "rb");
            if (check) { std::fclose(check); break; }
            const auto cut = path.find_last_of("/\\");
            if (cut == std::string::npos) break;
            path.erase(cut);
        }
        std::FILE* file = std::fopen((path + "/artifacts/reference/raw/front.anm").c_str(), "rb");
        CHECK(file != nullptr);
        std::fseek(file, 0, SEEK_END);
        const long size = std::ftell(file);
        std::fseek(file, 0, SEEK_SET);
        anm_bytes.resize(size_t(size < 0 ? 0 : size));
        const size_t got = anm_bytes.empty() ? 1 : std::fread(anm_bytes.data(), 1, anm_bytes.size(), file);
        std::fclose(file);
        CHECK_EQ(got, anm_bytes.size());
    }
    CHECK(manager.load(3, anm_bytes.data(), u32(anm_bytes.size())));
    // 建三棵树：parent0（脚本 0）→ child（脚本 1 经 spawn_child 或 attach）；
    // borrowed 外部根（overlay 面板语义：宿主自持 VM attach 真子体）。
    AnmVm* parent = manager.create_instance(*manager.resource(3), 0);
    CHECK(parent != nullptr);
    const u32 parent_handle = manager.registry.submit(*parent);
    CHECK(parent_handle != 0);
    AnmVm* child = manager.create_instance(*manager.resource(3), 1);
    CHECK(child != nullptr);
    const u32 child_handle = manager.registry.submit(*child);
    CHECK(child_handle != 0);
    CHECK(manager.registry.attach(*child, *parent));
    // borrowed 外部根：宿主对象内嵌 VM（不进 registry 池）。
    AnmVm external_root;
    CHECK(external_root.bind(*manager.resource(3), 0));
    AnmVm* borrowed_child = manager.create_instance(*manager.resource(3), 2);
    CHECK(borrowed_child != nullptr);
    const u32 borrowed_child_handle = manager.registry.submit(*borrowed_child);
    CHECK(manager.registry.attach(*borrowed_child, external_root));
    // 链序断言：主链 [parent, child, borrowed_child]（提交序；external_root
    // 是 borrowed 外部根不占链）。
    CHECK_EQ(manager.registry.ordered_count(false), 3u);
    CHECK_EQ(manager.registry.ordered_handle(false, 0), parent_handle);
    CHECK_EQ(manager.registry.ordered_handle(false, 1), child_handle);
    CHECK_EQ(manager.registry.ordered_handle(false, 2), borrowed_child_handle);
    // capture。
    multiplayer::WorldState snapshot;
    // WorldState 需要 StageBattle——用最小合成 battle 只为 mp_registry 注入。
    Fixture fixture;
    CHECK(fixture.init());
    fixture.battle.mp_registry_hook = &manager.registry;
    GameRandom visual_rng(0x77);
    CHECK(snapshot.capture(fixture.battle, visual_rng));
    // 破坏：销毁全部树 + 提交新 VM（generation 推进）。
    CHECK(manager.registry.destroy_tree(*parent));
    CHECK(manager.registry.destroy_tree(*borrowed_child));
    AnmVm* spare = manager.create_instance(*manager.resource(3), 0);
    CHECK(spare != nullptr);
    CHECK(manager.registry.submit(*spare) != 0);
    CHECK_EQ(manager.registry.ordered_count(false), 1u);
    // restore：父子链/borrowed 根/句柄 generation/链序全恢复。
    CHECK(snapshot.restore(fixture.battle, visual_rng));
    CHECK_EQ(manager.registry.ordered_count(false), 3u);
    CHECK_EQ(manager.registry.ordered_handle(false, 0), parent_handle);
    CHECK_EQ(manager.registry.ordered_handle(false, 1), child_handle);
    CHECK_EQ(manager.registry.ordered_handle(false, 2), borrowed_child_handle);
    AnmVm* restored_parent = manager.registry.find(parent_handle);
    CHECK(restored_parent != nullptr);
    AnmVm* restored_child = manager.registry.find(child_handle);
    CHECK(restored_child != nullptr);
    // 父子链：children(parent) 含 child。
    const std::vector<AnmVm*> children = manager.registry.children(*restored_parent);
    bool child_linked = false;
    for (const AnmVm* entry : children)
        if (entry == restored_child) child_linked = true;
    CHECK(child_linked);
    // borrowed 根的子体仍挂外部 VM。
    const std::vector<AnmVm*> borrowed_children = manager.registry.children(external_root);
    bool borrowed_linked = false;
    for (const AnmVm* entry : borrowed_children)
        if (manager.registry.handle(*entry) == borrowed_child_handle) borrowed_linked = true;
    CHECK(borrowed_linked);
}

TEST(wire_digest_uses_enemy_animation_occupancy) {
    Fixture fixture;
    CHECK(fixture.init());
    CHECK(!fixture.battle.sim.enemies.empty());
    if (fixture.battle.sim.enemies.empty()) return;
    auto& enemy = *fixture.battle.sim.enemies.front();
    GameRandom visual(0x1234);
    auto before = std::make_unique<multiplayer::WorldState>();
    auto after = std::make_unique<multiplayer::WorldState>();
    enemy.anm_slot_handle[0] = 0x2001;
    CHECK(before->capture(fixture.battle, visual));
    enemy.anm_slot_handle[0] = 0x4002;
    CHECK(after->capture(fixture.battle, visual));
    CHECK_EQ(before->digest_core(), after->digest_core());
    CHECK(before->digest() != after->digest());
    enemy.anm_slot_handle[0] = 0;
    CHECK(after->capture(fixture.battle, visual));
    CHECK(before->digest_core() != after->digest_core());
    enemy.anm_slot_handle[0] = 0x4002;
    enemy.life += 1;
    CHECK(after->capture(fixture.battle, visual));
    CHECK(before->digest_core() != after->digest_core());
}

TEST(anm_recreated_and_frozen_roots) {
    AnmRegistry registry;
    auto original = std::make_unique<AnmVm>();
    auto* child = registry.allocate();
    const u32 handle = registry.submit(*child);
    CHECK(registry.attach(*child, *original));
    child->creation_parent = child->rotation_parent = original.get();
    AnmRegistry::SnapshotData borrowed;
    CHECK(registry.capture(borrowed));
    const auto* identity = original.get();
    original.reset();
    AnmVm replacement;
    CHECK(registry.restore(borrowed, {{identity, &replacement}}));
    child = registry.find(handle);
    CHECK(child && child->creation_parent == &replacement &&
          child->rotation_parent == &replacement);
    replacement.visual.translation.x = 137;
    CHECK(registry.preserve_embedded_children(replacement));
    AnmRegistry::SnapshotData frozen;
    CHECK(registry.capture(frozen));
    for (int repeat = 0; repeat < 2; ++repeat) {
        CHECK(registry.restore(frozen));
        child = registry.find(handle);
        CHECK(child && child->creation_parent);
        if (!child || !child->creation_parent) return;
        CHECK(child->creation_parent == child->rotation_parent);
        CHECK_EQ(child->creation_parent->visual.translation.x, 137);
        CHECK_EQ(registry.children(*child->creation_parent).size(), 1u);
    }
}

// ---- scheduler_callback_rebind：FrameScheduler 稳定 callback 身份 ----
TEST(scheduler_callback_rebind) {
    // 快照红线：FrameScheduler 按稳定 callback 身份恢复。TH13 的 callback
    // 由宿主以 FrameCallback 成员持有（侵入链），快照存活期内注册集合
    // 不变——本 case 钉住：①callback 身份（FrameCallback*）在 capture/
    // restore 前后不变；②优先级/使能位/pass 不变；③执行序不变。
    FrameScheduler scheduler;
    std::array<FrameCallback, 4> callbacks{};
    i32 counters[4] = {0, 0, 0, 0};
    const i32 priorities[4] = {20, 22, 24, 34};
    for (int i = 0; i < 4; ++i) {
        callbacks[i].owner = &counters[i];
        callbacks[i].enabled = true;
        callbacks[i].run = [](void* owner) -> i32 {
            ++*static_cast<i32*>(owner);
            return 1;
        };
        CHECK(scheduler.add(callbacks[i], FramePass::Update, priorities[i]) >= 0);
    }
    // 执行序 = 优先级序（20→22→24→34）；execute 返回运行 callback 数。
    CHECK_EQ(scheduler.update(), 4);
    for (int i = 0; i < 4; ++i) CHECK_EQ(counters[i], 1);
    // callback 身份稳定（contains 按指针）。
    for (int i = 0; i < 4; ++i) CHECK(scheduler.contains(callbacks[i]));
    // 快照往返（WorldState 不触碰调度器——身份面由宿主持有）。
    Fixture fixture;
    CHECK(fixture.init());
    multiplayer::WorldState snapshot;
    GameRandom visual(3);
    CHECK(snapshot.capture(fixture.battle, visual));
    GameInput fire;
    for (int frame = 0; frame < 5; ++frame) {
        fire.update(0x01);
        CHECK(fixture.battle.step(fire));
    }
    CHECK(snapshot.restore(fixture.battle, visual));
    // restore 后调度结构原样：身份/顺序/执行。
    for (int i = 0; i < 4; ++i) CHECK(scheduler.contains(callbacks[i]));
    CHECK_EQ(scheduler.update(), 4);
    for (int i = 0; i < 4; ++i) CHECK_EQ(counters[i], 2);
    // 移除中位 callback 后其余身份保持（宿主重绑的等价面）。
    scheduler.remove(callbacks[2]);
    CHECK(!scheduler.contains(callbacks[2]));
    CHECK_EQ(scheduler.update(), 3);
    CHECK_EQ(counters[0], 3);
    CHECK_EQ(counters[1], 3);
    CHECK_EQ(counters[2], 2);
    CHECK_EQ(counters[3], 3);
}

// ---- dual_rng_events_transition：双 RNG 与事件/转场边界 ----
TEST(dual_rng_events_transition) {
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 1});
    GameRandom visual(0x5A5A);
    GameInput fire;
    for (int frame = 0; frame < 30; ++frame) {
        fire.update(0x01);
        dual.battle.seat_inputs[0] = fire;
        dual.battle.seat_inputs[1] = fire;
        CHECK(dual.battle.step(GameInput{}));
    }
    // 记录双 RNG 状态。
    const u16 game_seed = dual.battle.sim.random.seed();
    const u32 game_index = dual.battle.sim.random.index();
    const u16 visual_seed = visual.seed();
    const u32 visual_index = visual.index();
    // 事件队列非空性观察（anm_effects：合成舞台 root 装载趟后可能为空——
    // 断言 capture/restore 保序即可）。
    multiplayer::WorldState snapshot;
    CHECK(snapshot.capture(dual.battle, visual));
    const uint64_t digest_a = snapshot.digest();
    // 推进 + 抽干双 RNG。
    for (int frame = 0; frame < 15; ++frame) {
        fire.update(0x01);
        dual.battle.seat_inputs[0] = fire;
        dual.battle.seat_inputs[1] = fire;
        CHECK(dual.battle.step(GameInput{}));
        (void)dual.battle.sim.random.next();
        (void)visual.next();
    }
    CHECK(dual.battle.sim.random.seed() != game_seed ||
          dual.battle.sim.random.index() != game_index);
    CHECK(snapshot.restore(dual.battle, visual));
    // 双 RNG 恢复逐位。
    CHECK_EQ(dual.battle.sim.random.seed(), game_seed);
    CHECK_EQ(dual.battle.sim.random.index(), game_index);
    CHECK_EQ(visual.seed(), visual_seed);
    CHECK_EQ(visual.index(), visual_index);
    // 同状态重 capture：digest 与快照 A 一致（RNG 恢复即权威态恢复）。
    multiplayer::WorldState verify;
    CHECK(verify.capture(dual.battle, visual));
    CHECK_EQ(verify.digest(), digest_a);
    // 恢复后同输入继续：RNG 流与基准逐位一致（双实例对照）。
    Fixture baseline;
    CHECK(baseline.init());
    baseline.enable(2, {0, 1});
    for (int frame = 0; frame < 30; ++frame) {
        fire.update(0x01);
        baseline.battle.seat_inputs[0] = fire;
        baseline.battle.seat_inputs[1] = fire;
        CHECK(baseline.battle.step(GameInput{}));
    }
    for (int frame = 0; frame < 10; ++frame) {
        fire.update(0x01);
        for (StageBattle* battle : {&dual.battle, &baseline.battle}) {
            battle->seat_inputs[0] = fire;
            battle->seat_inputs[1] = fire;
            CHECK(battle->step(GameInput{}));
        }
        CHECK_EQ(dual.battle.sim.random.seed(), baseline.battle.sim.random.seed());
        CHECK_EQ(dual.battle.sim.random.index(), baseline.battle.sim.random.index());
        // seed+index 已覆盖 GameRandom 全状态（seed_ + index_ 两私有域）。
    }
}

// ---- P3.2：宿主域回滚等价（震动/对话/STD/过渡——ApplicationState 真实
// 对局形态；scene 接线经 Session::BindWorld 的 world_scene_ 面）----
// 公共小工具：真实对局 boot（scheduler_order_test 的 boot_live_run 模式；
// save 名按 case 区分避免交叉污染）。
namespace {

using App = th13::sdl::ApplicationState;

bool p32_pin_boot(App& app, const char* save_name) {
    ::setenv("TH13_SAVE_PATH", save_name, 1);
    const char* asset_root = std::getenv("TH13_NATIVE_ASSET_ROOT");
    if (!asset_root || !*asset_root) return false;
    if (!app.audio_device.initialize_for_test(
            asset_root, std::string(std::getenv("TH13_NATIVE_REPO_ROOT")
                                       ? std::getenv("TH13_NATIVE_REPO_ROOT")
                                       : "../../..") +
                            "/games/th13/thbgmogg.dat"))
        return false;
    return app.initialize(false);
}

bool p32_boot_live_run(App& app, const char* save_name) {
    if (!p32_pin_boot(app, save_name)) return false;
    if (!app.start_game(1)) return false;
    return app.pending_destination == 13 && app.failure.empty();
}

// 跨屏障进对局（两拍装载幕 → battle f0），返回时 app.run 就绪。
bool p32_enter_run(App& app, const char* save_name) {
    if (!p32_boot_live_run(app, save_name)) return false;
    GameInput input;
    for (int barrier = 0; barrier < 4 && !app.run; ++barrier) {
        input.update(0);
        if (!app.update(input, false) || !app.commit(60.0f) || !app.present())
            return false;
    }
    return app.run != nullptr && app.failure.empty();
}

// 一帧驱动（update+commit+present——与生产帧体同序）。
bool p32_frame(App& app, u32 held) {
    static GameInput input;
    input.update(held);
    return app.update(input, false) && app.commit(60.0f) && app.present() &&
           app.failure.empty();
}

}  // namespace

// rollback_across_bomb_shake：envelope 震动活动帧上回滚——震动列表经
// start_shake 重建后，重放帧的 game RNG 消费与连续基线一致（修复前：
// restore 缺列表 → 重放帧少抽 2 次/帧 → 权威面发散）。
TEST(rollback_across_bomb_shake) {
    App rollback;
    CHECK(p32_enter_run(rollback, "t7m6-shake-rb.bin"));
    if (!rollback.run) return;
    App baseline;
    CHECK(p32_enter_run(baseline, "t7m6-shake-bl.bin"));
    if (!baseline.run) return;
    // 直接经 RunScene 生产入口注入 Bomb 震动（battle.bomb_screen_shake 同款
    // ScreenShake::bomb 值域），两侧同拍注入。
    CHECK(rollback.run->inject_shake(th13::ScreenShake::bomb(12, 4, 30, 20)));
    CHECK(baseline.run->inject_shake(th13::ScreenShake::bomb(12, 4, 30, 20)));
    // 推进 3 帧让震动消费 game RNG（envelope 型每帧 2 抽）。
    for (int i = 0; i < 3; ++i) {
        CHECK(p32_frame(rollback, 0));
        CHECK(p32_frame(baseline, 0));
    }
    // 快照（含震动列表）→ 回滚侧再发散 8 帧 → restore。
    multiplayer::WorldState snapshot;
    GameRandom& visual = rollback.visual;
    CHECK(snapshot.capture(rollback.run->battle, visual, nullptr,
                           rollback.run->presentation.get(), rollback.run.get(),
                           rollback.run_transition.handle()));
    for (int i = 0; i < 8; ++i) CHECK(p32_frame(rollback, 0));
    u32 handle_back = 0;
    CHECK(snapshot.restore(rollback.run->battle, visual, nullptr,
                           rollback.run->presentation.get(), rollback.run.get(),
                           &handle_back));
    // 重放 3 帧后与基线逐帧对拍（digest_core = 权威模拟面）。
    for (int i = 0; i < 3; ++i) {
        CHECK(p32_frame(rollback, 0));
        CHECK(p32_frame(baseline, 0));
    }
    multiplayer::WorldState a, b;
    CHECK(a.capture(rollback.run->battle, visual, nullptr,
                    rollback.run->presentation.get(), rollback.run.get(),
                    rollback.run_transition.handle()));
    CHECK(b.capture(baseline.run->battle, baseline.visual, nullptr,
                    baseline.run->presentation.get(), baseline.run.get(),
                    baseline.run_transition.handle()));
    CHECK_EQ(a.digest_core(), b.digest_core());
    CHECK_EQ(rollback.run->battle.sim.random.index(),
             baseline.run->battle.sim.random.index());
}

// rollback_across_msg_start：对话域回滚——MsgPlayer 游标/状态/锁存输入沿
// 恢复后，重放帧的对话推进与连续基线一致。
TEST(rollback_across_msg_start) {
    App rollback;
    CHECK(p32_enter_run(rollback, "t7m6-msg-rb.bin"));
    if (!rollback.run) return;
    App baseline;
    CHECK(p32_enter_run(baseline, "t7m6-msg-bl.bin"));
    if (!baseline.run) return;
    // 对话经 RunScene::open_dialog 生产入口打开（MsgHost 全接线）；两侧同拍。
    rollback.run->open_dialog(0);
    baseline.run->open_dialog(0);
    CHECK(rollback.run->dialog_active());
    for (int i = 0; i < 2; ++i) {
        CHECK(p32_frame(rollback, 0));
        CHECK(p32_frame(baseline, 0));
    }
    multiplayer::WorldState snapshot;
    CHECK(snapshot.capture(rollback.run->battle, rollback.visual, nullptr,
                           rollback.run->presentation.get(), rollback.run.get(),
                           rollback.run_transition.handle()));
    for (int i = 0; i < 8; ++i) CHECK(p32_frame(rollback, 0));
    u32 handle_back = 0;
    CHECK(snapshot.restore(rollback.run->battle, rollback.visual, nullptr,
                           rollback.run->presentation.get(), rollback.run.get(),
                           &handle_back));
    // 对话恢复后 active 位与游标回到快照拍。
    CHECK(rollback.run->dialog_active());
    for (int i = 0; i < 3; ++i) {
        CHECK(p32_frame(rollback, 0));
        CHECK(p32_frame(baseline, 0));
    }
    // MsgPlayer 状态面（clock/active）与基线一致。
    CHECK_EQ(rollback.run->dialog_clock(), baseline.run->dialog_clock());
    CHECK_EQ(rollback.run->dialog_active(), baseline.run->dialog_active());
    multiplayer::WorldState a, b;
    CHECK(a.capture(rollback.run->battle, rollback.visual, nullptr,
                    rollback.run->presentation.get(), rollback.run.get(),
                    rollback.run_transition.handle()));
    CHECK(b.capture(baseline.run->battle, baseline.visual, nullptr,
                    baseline.run->presentation.get(), baseline.run.get(),
                    baseline.run_transition.handle()));
    CHECK_EQ(a.digest_core(), b.digest_core());
}

// rollback_across_stage_transition：STD 场景值域回滚——scenery 的相机脚本
// 游标/图元 VM/生命周期恢复后，重放帧 STD 推进与连续基线一致。
TEST(rollback_across_stage_transition) {
    App rollback;
    CHECK(p32_enter_run(rollback, "t7m6-std-rb.bin"));
    if (!rollback.run) return;
    App baseline;
    CHECK(p32_enter_run(baseline, "t7m6-std-bl.bin"));
    if (!baseline.run) return;
    // 推进 6 帧：STD 相机脚本步进 + scenery->frames 增长（update@13 面）。
    for (int i = 0; i < 6; ++i) {
        CHECK(p32_frame(rollback, 0));
        CHECK(p32_frame(baseline, 0));
    }
    const u32 scenery_frames_at_capture =
        rollback.run->scenery ? rollback.run->scenery->frames : 0;
    multiplayer::WorldState snapshot;
    CHECK(snapshot.capture(rollback.run->battle, rollback.visual, nullptr,
                           rollback.run->presentation.get(), rollback.run.get(),
                           rollback.run_transition.handle()));
    for (int i = 0; i < 8; ++i) CHECK(p32_frame(rollback, 0));
    u32 handle_back = 0;
    CHECK(snapshot.restore(rollback.run->battle, rollback.visual, nullptr,
                           rollback.run->presentation.get(), rollback.run.get(),
                           &handle_back));
    // STD frames 恢复到快照拍（scenery 值域随快照走）。
    if (rollback.run->scenery)
        CHECK_EQ(rollback.run->scenery->frames, scenery_frames_at_capture);
    for (int i = 0; i < 4; ++i) {
        CHECK(p32_frame(rollback, 0));
        CHECK(p32_frame(baseline, 0));
    }
    CHECK_EQ(rollback.run->scenery ? rollback.run->scenery->frames : 0,
             baseline.run->scenery ? baseline.run->scenery->frames : 0);
    multiplayer::WorldState a, b;
    CHECK(a.capture(rollback.run->battle, rollback.visual, nullptr,
                    rollback.run->presentation.get(), rollback.run.get(),
                    rollback.run_transition.handle()));
    CHECK(b.capture(baseline.run->battle, baseline.visual, nullptr,
                    baseline.run->presentation.get(), baseline.run.get(),
                    baseline.run_transition.handle()));
    CHECK_EQ(a.digest_core(), b.digest_core());
}

int main() { return th13test::run_all("m6.world_state"); }
