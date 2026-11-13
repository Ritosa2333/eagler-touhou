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
#include "multiplayer/Session.hpp"
#include "multiplayer/StageEntry.hpp"
// P3.2 宿主域 case：ApplicationState 真实对局（boot/帧体与
// scheduler_order_test 同模式——update/commit/present 三段序）。
#include "sdl/ApplicationState.hpp"
#include <cmath>
#include <cstdlib>
#include <sstream>

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
    std::printf("# digest-baseline full=%016llx core=%016llx\n",
        static_cast<unsigned long long>(digest_at_capture),
        static_cast<unsigned long long>(snapshot.digest_core()));
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
    dual.battle.seat_huds[1].life_piece_threshold_index=3;
    multiplayer::WorldState mutated;
    CHECK(mutated.capture(dual.battle,visual)); CHECK(mutated.digest()!=before);
    CHECK(snapshot.restore(dual.battle,visual));
    CHECK_EQ(dual.battle.seat_huds[1].life_piece_threshold_index,0);
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

// R-A/R-I9: transfer progress and a gift's recipient affect later frames.
TEST(transfer_snapshot_and_digest) {
    Fixture world; CHECK(world.init()); world.enable(3, {0, 0, 0});
    auto& battle = world.battle;
    battle.seats[0].life_transfer_state = 0x43;
    battle.seats[1].power_transfer_state = 0x62;
    CHECK(battle.item_manager.spawn_targeted_item(0, 300, ItemExtraLife, 2));
    GameRandom visual(0xbeef);
    multiplayer::WorldState saved, changed;
    CHECK(saved.capture(battle, visual));
    const auto original = saved.digest(), core = saved.digest_core();
    char domains[4096]{}, mutated[4096]{};
    saved.digest_domains(domains, sizeof(domains));
    battle.seats[1].power_transfer_state ^= 1;
    CHECK(changed.capture(battle, visual));
    CHECK(changed.digest() != original); CHECK(changed.digest_core() != core);
    changed.digest_domains(mutated, sizeof(mutated));
    CHECK(std::string(domains) != mutated);
    CHECK(saved.restore(battle, visual));
    CHECK_EQ(battle.seats[1].power_transfer_state, 0x62);
    Item* gift = nullptr;
    for (auto& item : battle.item_manager.items())
        if (item.alive && item.target_seat == 2) gift = &item;
    CHECK(gift != nullptr); if (!gift) return;
    gift->target_seat = 1;
    CHECK(changed.capture(battle, visual));
    CHECK(changed.digest() != original); CHECK(changed.digest_core() != core);
    changed.digest_domains(mutated, sizeof(mutated));
    CHECK(std::string(domains) != mutated);
    CHECK(saved.restore(battle, visual));
    CHECK(changed.capture(battle, visual)); CHECK_EQ(changed.digest(), original);
    CHECK_EQ(battle.seats[0].life_transfer_state, 0x43);
    multiplayer::StageEntry entry;
    entry.Capture(battle, world.progress, world.score, 1, 0, 2, 0);
    entry.RestoreBattleCarry(battle);
    CHECK_EQ(battle.seats[0].life_transfer_state, 0);
    CHECK_EQ(battle.seats[1].power_transfer_state, 0);
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
    const auto captured_digest = snapshot.digest();
    // Record the actual next allocation, including its slot, serial and RNG.
    // Both seats now deal real damage, so capture may already contain spirits.
    dual.battle.item_manager.spawn_value_item(1, 0.0f, 0.0f, 0.0f, 1.0f);
    multiplayer::WorldState next_allocation;
    CHECK(next_allocation.capture(dual.battle, visual));
    const auto next_digest = next_allocation.digest();
    CHECK(snapshot.restore(dual.battle, visual));
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
    multiplayer::WorldState restored;
    CHECK(restored.capture(dual.battle, visual));
    CHECK_EQ(restored.digest(), captured_digest);
    // The same allocation after restore must reproduce the complete pool and
    // RNG state, including all pre-existing drops and the reused slot order.
    dual.battle.item_manager.spawn_value_item(1, 0.0f, 0.0f, 0.0f, 1.0f);
    CHECK(restored.capture(dual.battle, visual));
    CHECK_EQ(restored.digest(), next_digest);
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


// The confirmed state must include scene owners, not only StageBattle.
TEST(scene_restore_full_one_three_twelve) {
    for (int distance : {1, 3, 12}) {
        App app;
        CHECK(p32_enter_run(app, "scene-full-restore.bin"));
        if (!app.run) return;
        for (int i=0;i<6;++i) CHECK(p32_frame(app,0));
        multiplayer::WorldState snapshot;
        CHECK(snapshot.capture(app.run->battle,app.visual,nullptr,
            app.run->presentation.get(),app.run.get(),app.run_transition.handle()));
        GameInput input;const auto saved_input=input;
        const auto frame=[&] { input.update(1);return app.update(input,false)&&app.commit(60.f)&&app.present(); };
        std::vector<uint64_t> expected;
        std::vector<std::string> expected_domains;
        std::vector<std::string> expected_details;
        for(int i=0;i<distance;++i) {
            CHECK(frame());
            multiplayer::WorldState state;
            CHECK(state.capture(app.run->battle,app.visual,nullptr,
                app.run->presentation.get(),app.run.get(),app.run_transition.handle()));
            expected.push_back(state.digest());
            char domains[2048];state.digest_domains(domains,sizeof domains);expected_domains.emplace_back(domains);expected_details.push_back(state.diagnostic_detail());
        }
        u32 transition=0;
        CHECK(snapshot.restore(app.run->battle,app.visual,nullptr,
            app.run->presentation.get(),app.run.get(),&transition));
        multiplayer::WorldState restored;
        CHECK(restored.capture(app.run->battle,app.visual,nullptr,
            app.run->presentation.get(),app.run.get(),transition));
        CHECK_EQ(restored.digest(),snapshot.digest());
        input=saved_input;
        for(int i=0;i<distance;++i) {
            CHECK(frame());
            multiplayer::WorldState state;
            CHECK(state.capture(app.run->battle,app.visual,nullptr,
                app.run->presentation.get(),app.run.get(),app.run_transition.handle()));
            if(state.digest()!=expected[i]) {
                char domains[2048];state.digest_domains(domains,sizeof domains);
                std::printf("distance=%d frame=%d\nexpected %s\nactual   %s\n",distance,i,expected_domains[i].c_str(),domains);
                std::printf("expected detail %s\nactual detail %s\n",expected_details[i].c_str(),state.diagnostic_detail().c_str());
            }
            CHECK_EQ(state.digest(),expected[i]);
        }
    }
}

TEST(scene_digest_mutation_controls) {
    App app;
    CHECK(p32_enter_run(app,"scene-mutation.bin"));
    if(!app.run)return;
    const auto capture=[&] { multiplayer::WorldState state;
        CHECK(state.capture(app.run->battle,app.visual,nullptr,
            app.run->presentation.get(),app.run.get(),app.run_transition.handle()));
        return state;
    };
    auto initial=capture();
    const auto state=app.run->capture_rollback();
    std::unordered_map<const AnmVm*,AnmVm*> remapped;
    auto changed=state; ++changed.post.clear_frame_;
    app.run->restore_rollback(changed,remapped);
    CHECK(capture().digest_core()!=initial.digest_core());
    app.run->restore_rollback(state,remapped);
    changed=state;changed.player_activation_pending=!changed.player_activation_pending;
    app.run->restore_rollback(changed,remapped);
    CHECK(capture().digest_core()!=initial.digest_core());
    app.run->restore_rollback(state,remapped);
    changed=state; changed.camera_origin.x+=1;
    app.run->restore_rollback(changed,remapped);
    CHECK(capture().digest_core()!=initial.digest_core());
    app.run->restore_rollback(state,remapped);
    changed=state;++changed.hud.boss.warn_state_;
    app.run->restore_rollback(changed,remapped);
    CHECK(capture().digest()!=initial.digest());
    app.run->restore_rollback(state,remapped);
    const auto hud_change = [&](auto mutate) {
        auto updated=state; mutate(updated.hud);
        app.run->restore_rollback(updated,remapped);
        CHECK(capture().digest()!=initial.digest());
        app.run->restore_rollback(state,remapped);
        CHECK_EQ(capture().digest(),initial.digest());
    };
    hud_change([](auto& hud){++hud.mp_seat_count;});
    hud_change([](auto& hud){++hud.mp_life_icons[0][0];});
    hud_change([](auto& hud){++hud.mp_bomb_icons[1][7];});
    hud_change([](auto& hud){++hud.mp_resource_labels[0][0];});
    hud_change([](auto& hud){++hud.mp_shared_labels[2];});
    changed=state;changed.spell.anchor_[0]+=1;
    app.run->restore_rollback(changed,remapped);
    CHECK(capture().digest()!=initial.digest());
    app.run->restore_rollback(state,remapped);
    changed=state;++changed.popups.cursor;
    app.run->restore_rollback(changed,remapped);
    CHECK(capture().digest()!=initial.digest());
    app.run->restore_rollback(state,remapped);
    CHECK(app.run->inject_shake(ScreenShake::bomb(12,4,30,20)));
    CHECK(capture().digest_core()!=initial.digest_core());
    CHECK(initial.restore(app.run->battle,app.visual,nullptr,
        app.run->presentation.get(),app.run.get(),nullptr));
    CHECK_EQ(capture().digest(),initial.digest());
    if(app.run->scenery) {
        ++app.run->scenery->script.state.instruction_offset;
        CHECK(capture().digest_core()!=initial.digest_core());
    }
}

// The draw buckets are part of the captured frame: a paused renderer may
// consume them before another update. Ordering and embedded item identity
// must survive capture/restore without advancing the game or either RNG.
TEST(anm_draw_layers_restore_and_digest) {
    // Pool lookup must also serialize sparse/inactive slots, overflow nodes,
    // borrowed roots and repeated draw references in the original order.
    {
        AnmRegistry registry;
        std::vector<AnmVm*> vms;
        std::vector<u32> handles;
        bool allocated = true;
        for (u32 i = 0; i < 0x2001; ++i) {
            auto* vm = registry.allocate();
            const auto handle = vm ? registry.submit(*vm, i & 1 ? 4 : 0) : 0;
            if (!handle) { allocated = false; break; }
            vm->visual.translation.x = float(i);
            vms.push_back(vm); handles.push_back(handle);
        }
        CHECK(allocated); if (!allocated) return;
        CHECK(registry.discard(*vms[42]));
        CHECK(registry.attach(*vms[0x1fff], *vms[100]));
        AnmVm borrowed;
        CHECK(registry.attach(*vms[0x2000], borrowed));
        vms[0x1fff]->creation_parent = vms[100];
        vms[0x2000]->rotation_parent = &borrowed;
        registry.append_draw(*vms[0]);
        registry.append_draw(*vms[0x1fff]);
        registry.append_draw(borrowed);
        registry.append_draw(*vms[0]);
        AnmRegistry::SnapshotData snapshot;
        CHECK(registry.capture(snapshot));
        CHECK_EQ(snapshot.entries.size(), 0x2002u);
        CHECK(!snapshot.entries[42].active);
        CHECK(snapshot.entries[0x1fff].in_overflow);
        CHECK_EQ(snapshot.entries[0x1fff].parent, 100u);
        CHECK_EQ(snapshot.entries[0x1fff].creation_parent, 100u);
        CHECK(snapshot.entries[0x2001].borrowed);
        CHECK_EQ(snapshot.entries[0x2000].parent, 0x2001u);
        CHECK_EQ(snapshot.entries[0x2000].rotation_parent, 0x2001u);
        CHECK_EQ(snapshot.layers[0].size(), 4u);
        CHECK_EQ(snapshot.layers[0][1].entry, 0x1fffu);
        CHECK_EQ(snapshot.layers[0][2].entry, 0x2001u);
        CHECK_EQ(snapshot.layers[0][0].entry, snapshot.layers[0][3].entry);
        Fixture fixture; CHECK(fixture.init());
        fixture.battle.mp_registry_hook = &registry;
        GameRandom visual(123);
        multiplayer::WorldState saved, restored;
        CHECK(saved.capture(fixture.battle, visual));
        const auto full = saved.digest(), core = saved.digest_core();
        CHECK(registry.discard(*vms[0x1fff]));
        CHECK(saved.restore(fixture.battle, visual));
        CHECK(restored.capture(fixture.battle, visual));
        CHECK_EQ(restored.digest(), full); CHECK_EQ(restored.digest_core(), core);
        std::printf("capture-pool-overflow full=%016llx core=%016llx\n",
                    (unsigned long long)full, (unsigned long long)core);
        CHECK_EQ(registry.layer(0).size(), 4u);
        CHECK(registry.layer(0)[0] == registry.layer(0)[3]);
        CHECK_EQ(registry.find(handles[0x1fff])->creation_parent, registry.find(handles[100]));
        CHECK_EQ(registry.find(handles[0x2000])->rotation_parent, &borrowed);
    }
    // Reusing a checkpoint for a different registry must replace deep values
    // and remove old parent/child links, rather than retaining the last frame.
    {
        AnmRegistry first_registry, second_registry;
        auto* parent = first_registry.allocate();
        auto* child = first_registry.allocate();
        CHECK(parent && child); if (!parent || !child) return;
        CHECK(first_registry.submit(*parent)); CHECK(first_registry.submit(*child));
        CHECK(first_registry.attach(*child, *parent));
        parent->geometry.screen_vertices.resize(24);
        parent->geometry.screen_vertices.back().position.x = 17;
        child->geometry.world_vertices.resize(12);
        AnmRegistry::SnapshotData reused;
        CHECK(first_registry.capture(reused));
        const auto original = reused;
        auto* root0 = second_registry.allocate();
        auto* root1 = second_registry.allocate();
        CHECK(root0 && root1); if (!root0 || !root1) return;
        const auto handle0 = second_registry.submit(*root0);
        const auto handle1 = second_registry.submit(*root1);
        CHECK(handle0 && handle1);
        root0->geometry.screen_vertices.resize(1);
        root0->geometry.screen_vertices[0].position.x = -23;
        CHECK(second_registry.capture(reused));
        CHECK_EQ(reused.entries.size(), 2u);
        CHECK(reused.entries[0].children.empty());
        CHECK(reused.entries[1].parent == AnmRegistry::SnapshotData::kNoVm);
        CHECK_EQ(reused.entries[0].vm.geometry.screen_vertices.size(), 1u);
        CHECK_EQ(reused.entries[0].vm.geometry.screen_vertices[0].position.x, -23.f);
        CHECK(reused.entries[1].vm.geometry.world_vertices.empty());
        CHECK_EQ(original.entries[0].vm.geometry.screen_vertices.size(), 24u);
        CHECK_EQ(original.entries[0].vm.geometry.screen_vertices.back().position.x, 17.f);
        root0->geometry.screen_vertices[0].position.x = 101;
        CHECK(second_registry.restore(reused));
        CHECK_EQ(second_registry.find(handle0)->geometry.screen_vertices[0].position.x, -23.f);
        CHECK(second_registry.children(*second_registry.find(handle0)).empty());
        CHECK(second_registry.children(*second_registry.find(handle1)).empty());
        AnmRegistry empty_registry;
        CHECK(empty_registry.capture(reused));
        CHECK(reused.entries.empty());
    }
    App app;
    CHECK(p32_pin_boot(app, "anm-layer-restore.bin"));
    CHECK(app.configure_multiplayer(3, {0, 1, 3}, 0));
    CHECK(app.start_game(1)); app.pending_destination = -1;
    CHECK(app.begin_run()); if (!app.run) return;
    auto& battle = app.run->battle;
    battle.item_manager.spawn_item(-80, 260, 0, ItemPower, 0, 0);
    battle.item_manager.spawn_item(80, 260, 0, ItemPower, 0, 0);
    GameInput input;
    for (int frame = 0; frame < 8; ++frame) {
        input.update(0);
        CHECK(app.update(input, false) && app.commit(60.f) && app.present());
    }
    const AnmVm* first = nullptr;
    const AnmVm* second = nullptr;
    u32 first_slot = 0, second_slot = 0;
    for (u32 slot = 0; slot < ItemManager::kItemCapacity; ++slot) {
        const auto* item = app.run->presentation->item_body_vm(slot);
        if (!item) continue;
        if (!first) { first = item; first_slot = slot; }
        else if (!second) { second = item; second_slot = slot; break; }
    }
    CHECK(first && second); if (!first || !second) return;
    const auto& items = app.animations.registry.layer(15);
    CHECK(std::find(items.begin(), items.end(), first) != items.end());
    CHECK(std::find(items.begin(), items.end(), second) != items.end());
    const auto capture = [&] {
        multiplayer::WorldState state;
        CHECK(state.capture(battle, app.visual, nullptr, app.run->presentation.get(),
                            app.run.get(), app.run_transition.handle()));
        return state;
    };
    auto saved = capture();
    const u32 game_seed = app.game_random.seed(), game_index = app.game_random.index();
    const u32 visual_seed = app.visual.seed(), visual_index = app.visual.index();
    // A duplicate is an observable extra draw even when VM values match.
    app.animations.registry.append_draw(*const_cast<AnmVm*>(first));
    const auto duplicate = capture();
    CHECK(duplicate.digest() != saved.digest());
    app.animations.registry.append_draw(*const_cast<AnmVm*>(second));
    const auto first_then_second = capture();
    CHECK(saved.restore(battle, app.visual, nullptr, app.run->presentation.get(), app.run.get()));
    first = app.run->presentation->item_body_vm(first_slot);
    second = app.run->presentation->item_body_vm(second_slot);
    CHECK(first && second); if (!first || !second) return;
    app.animations.registry.append_draw(*const_cast<AnmVm*>(second));
    app.animations.registry.append_draw(*const_cast<AnmVm*>(first));
    const auto second_then_first = capture();
    CHECK(first_then_second.digest() != second_then_first.digest());
    CHECK(saved.restore(battle, app.visual, nullptr, app.run->presentation.get(), app.run.get()));
    CHECK_EQ(app.game_random.seed(), game_seed); CHECK_EQ(app.game_random.index(), game_index);
    CHECK_EQ(app.visual.seed(), visual_seed); CHECK_EQ(app.visual.index(), visual_index);
    const auto restored = capture();
    CHECK_EQ(restored.digest(), saved.digest());
    CHECK(app.present());
    CHECK_EQ(capture().digest(), saved.digest());
    // Unknown, draw-only VM has no stable identity and must fail capture.
    AnmVm unknown;
    unknown.visual.layer = 15;
    app.animations.registry.append_draw(unknown);
    CHECK(!saved.capture(battle, app.visual, nullptr, app.run->presentation.get(), app.run.get()));
    CHECK(!saved.ready());
    CHECK(!saved.restore(battle, app.visual, nullptr, app.run->presentation.get(), app.run.get()));
}

TEST(scene_present_does_not_advance_state) {
    App app;CHECK(p32_enter_run(app,"draw-repeat-state.bin"));
    for(int i=0;i<6;++i) CHECK(p32_frame(app,0));
    CHECK(app.run->scenery!=nullptr);app.run->scenery->begin_exit();
    const auto capture=[&] { multiplayer::WorldState s;
        CHECK(s.capture(app.run->battle,app.visual,nullptr,app.run->presentation.get(),app.run.get(),app.run_transition.handle()));
        return s; };
    const auto a=capture();CHECK(app.present());const auto b=capture();
    CHECK(app.present());const auto c=capture();
    if(a.digest()!=b.digest() || b.digest()!=c.digest())
        std::printf("draw A %s\ndraw B %s\ndraw C %s\n",a.diagnostic_detail().c_str(),b.diagnostic_detail().c_str(),c.diagnostic_detail().c_str());
    CHECK_EQ(a.digest(),b.digest());CHECK_EQ(b.digest(),c.digest());
}

TEST(scene_update_then_present_preserves_full_state) {
    App app;CHECK(p32_enter_run(app,"update-draw-state.bin"));
    for(int i=0;i<6;++i) CHECK(p32_frame(app,0));
    for(int frame=0;frame<12;++frame) {
        GameInput input;input.update(1);CHECK(app.update(input,false));CHECK(app.commit(60.f));
        multiplayer::WorldState before,after;
        CHECK(before.capture(app.run->battle,app.visual,nullptr,app.run->presentation.get(),app.run.get(),app.run_transition.handle()));
        CHECK(app.present());
        CHECK(after.capture(app.run->battle,app.visual,nullptr,app.run->presentation.get(),app.run.get(),app.run_transition.handle()));
        if(before.digest()!=after.digest()) {
            char a[2048],b[2048];before.digest_domains(a,sizeof a);after.digest_domains(b,sizeof b);
            std::printf("update-draw frame=%d before %s\nafter %s\n",frame,a,b);
        }
        CHECK_EQ(before.digest(),after.digest());
    }
}

TEST(registry_restore_removes_speculative_high_water_slots) {
    AnmRegistry registry;AnmRegistry::SnapshotData before,after;
    CHECK(registry.capture(before));
    auto* vm=registry.allocate();CHECK(vm!=nullptr);CHECK(registry.submit(*vm)!=0);
    CHECK(registry.restore(before));CHECK(registry.capture(after));
    CHECK_EQ(after.entries.size(),before.entries.size());
    CHECK(after.free_slots==before.free_slots);CHECK_EQ(after.generation,before.generation);
    vm=registry.allocate();CHECK(vm!=nullptr);CHECK(registry.submit(*vm)!=0);
}

TEST(new_network_world_ignores_local_title_allocation_history) {
    using namespace th13::multiplayer;
    std::uint64_t expected=0;
    for(unsigned preview_frames : {0u,37u,143u}) {
        session.Stop();App app;CHECK(p32_pin_boot(app,"world-epoch.bin"));
        for(unsigned i=0;i<preview_frames;++i) CHECK(p32_frame(app,0));
        CHECK(app.configure_multiplayer(2,{0,0,0},0));
        SessionSetup setup;setup.version=2;setup.mode=2;setup.seed=1234;setup.session_id=42;setup.build[0]=1;
        const auto words=EncodeSessionSetup(setup);CHECK(session.Configure(words.data(),words.size()));
        CHECK(session.BeginLocalSimulation());
        app.progress.difficulty=1;CHECK(app.start_game(1));app.pending_destination=-1;
        app.game_random.reseed_restore(1234);app.visual.reseed_restore(1234);
        CHECK(app.begin_run());
        WorldState state;CHECK(state.capture(app.run->battle,app.visual,nullptr,
            app.run->presentation.get(),app.run.get(),app.run_transition.handle()));
        if(preview_frames==0) expected=state.digest();
        else CHECK_EQ(state.digest(),expected);
        session.Stop();
    }
}

TEST(scene_laser_retire_restore_update_only) {
    for (int distance : {1,3,12}) {
        App app; CHECK(p32_enter_run(app,"scene-laser-restore.bin"));
        if(!app.run)return;
        auto& sim=app.run->battle.sim;
        StageEt et;et.reset();et.sprite=8;et.subtype=6;
        et.spawn[0]=et.spawn[1]=480;et.laser_width=16;et.speed1=8;et.sound=-1;
        et.laser_timing[0]=20;et.laser_timing[1]=8;
        et.laser_timing[2]=140;et.laser_timing[3]=10;
        StageEnemy enemy;enemy.position[0]=96;enemy.position[1]=144;
        sim.emit_infinite_laser(et,enemy,257);
        CHECK(sim.error().empty()); CHECK(!sim.lasers.empty());
        multiplayer::WorldState snapshot;
        CHECK(snapshot.capture(app.run->battle,app.visual,nullptr,
            app.run->presentation.get(),app.run.get(),app.run_transition.handle()));
        std::vector<uint64_t> expected;
        for(int i=0;i<distance;++i) {
            sim.advance_lasers();CHECK(sim.error().empty());
            multiplayer::WorldState state;
            CHECK(state.capture(app.run->battle,app.visual,nullptr,
                app.run->presentation.get(),app.run.get(),app.run_transition.handle()));
            expected.push_back(state.digest());
        }
        // The future owner is destroyed; a restore must recreate its values
        // without re-running animation birth or consuming RNG.
        sim.lasers.front().phase=1;sim.advance_lasers();CHECK(sim.lasers.empty());
        CHECK(snapshot.restore(app.run->battle,app.visual,nullptr,
            app.run->presentation.get(),app.run.get(),nullptr));
        for(int i=0;i<distance;++i) {
            sim.advance_lasers();CHECK(sim.error().empty());
            multiplayer::WorldState state;
            CHECK(state.capture(app.run->battle,app.visual,nullptr,
                app.run->presentation.get(),app.run.get(),app.run_transition.handle()));
            CHECK_EQ(state.digest(),expected[i]);
        }
    }
}

TEST(stage6_clear_flow_restore_179_379) {
    struct Host final : PostGamePlatform {
        int tick=0;std::vector<int> fades,ends;
        void request_destination(i32 raw) override { CHECK_EQ(raw,15);ends.push_back(tick); }
        bool request_stage6_fade() override { fades.push_back(tick);return true; }
        void bgm(i32) override {}
    } host;
    SessionState progress;progress.stage=6;
    HudPlayerState player;HudScoreState score;
    PostGameFlow flow(progress,player,score,host);
    flow.on_stage_clear();CHECK(flow.clear_sequence());
    for(int boundary:{179,379}) {
        auto state=flow.capture_rollback();
        state.clear_sequence_=true;state.clear_frame_=u32(boundary);
        state.age_=u32(boundary);flow.restore_rollback(state);
        const auto saved_progress=progress;
        std::vector<int> expected_fades,expected_ends;
        for(int pass=0;pass<2;++pass) {
            host.fades.clear();host.ends.clear();
            flow.restore_rollback(state);progress=saved_progress;
            for(host.tick=0;host.tick<3;++host.tick)flow.update();
            if(pass==0){expected_fades=host.fades;expected_ends=host.ends;}
            else {CHECK(host.fades==expected_fades);CHECK(host.ends==expected_ends);}
        }
        if(boundary==179){CHECK(expected_fades==std::vector<int>{0});CHECK(expected_ends.empty());}
        else {CHECK(expected_fades.empty());CHECK(expected_ends==std::vector<int>{0});}
    }
}

int main() { return th13test::run_all("m6.world_state"); }

TEST(host_outputs_and_handoff_rollback_one_three_twelve) {
    using namespace th13::multiplayer;
    for (unsigned distance : {1u,3u,12u}) {
        session.Stop();
        App app; CHECK(p32_enter_run(app,"host-commit-restore.bin"));
        if (!app.run) return;
        CHECK(app.run->enable_multiplayer(2,{0,0,0}));
        SessionSetup setup;setup.version=2;setup.mode=2;setup.automatic=false;
        setup.session_id=42;setup.build[0]=1;
        const auto words=EncodeSessionSetup(setup);
        CHECK(session.Configure(words.data(),words.size()));
        session.BindWorld(&app.run->battle,&app.visual,app.run->presentation.get(),app.run.get(),&app.mp_transition_handle_);
        session.capture_host=[&] { return app.capture_multiplayer_host(); };
        app.run->playback_input=[] { return session.LocalDelayedInput(); };
        CHECK(session.BeginLocalSimulation());
        const auto step=[&] { CHECK(session.CanSimulate());
            CHECK(app.update(session.MenuInput(),false));CHECK(app.commit(60.f));session.OnFrameSimulated(); };
        CHECK(session.CaptureLocal(0,0));session.AdvanceCapturePhase();
        CHECK(session.SubmitRemote(1,0,{})==Netplay::RemoteInputResult::Accepted);
        step();
        const auto base_frames=app.frames;
        const auto base_fades=app.ending_fades.size();
        const auto base_play=app.files.ordinary_records[0].total_frames;
        CHECK(app.loading_prayer!=0);
        auto* caption=app.loading_animations.registry.find(app.loading_prayer);
        CHECK(caption!=nullptr);
        const auto base_caption_time=caption->timer.current;
        app.slow_window_time_=.98;app.slow_window_frames_=49;
        const auto base_slow=app.slow_num_;
        for(unsigned i=1;i<=distance;++i) {
            CHECK(session.CaptureLocal(1,i));session.AdvanceCapturePhase();
            step();
        }
        CHECK_EQ(app.files.ordinary_records[0].total_frames,base_play);
        CHECK(!session.AllFramesCommitted());
        CHECK(!session.FinishWorldSimulation());
        CHECK(session.Simulating());
        app.pending_destination=i32(th13::sdl::Destination::NextStage);
        CHECK(app.request_stage6_fade());
        auto* old=app.run.get();
        CHECK(app.present());CHECK(app.run.get()==old);
        CHECK_EQ(app.pending_destination,i32(th13::sdl::Destination::NextStage));
        for(unsigned i=1;i<=distance;++i) {
            Netplay::FrameInput input;if(i==1) input.buttons=0x2;
            const auto result=session.SubmitRemote(1,i,input);
            CHECK(result==Netplay::RemoteInputResult::Accepted || result==Netplay::RemoteInputResult::RollbackRequired || result==Netplay::RemoteInputResult::PredictionCorrect);
        }
        CHECK(session.Pump(distance+1));
        CHECK_EQ(session.Restores(),1u);CHECK_EQ(session.SimFrame(),1u);
        CHECK_EQ(app.frames,base_frames);CHECK_EQ(app.pending_destination,-1);
        CHECK_EQ(app.ending_fades.size(),base_fades);
        CHECK_EQ(app.slow_window_time_,.98);CHECK_EQ(app.slow_window_frames_,49u);
        CHECK_EQ(app.slow_num_,base_slow);
        CHECK(app.loading_prayer!=0);
        caption=app.loading_animations.registry.find(app.loading_prayer);
        CHECK(caption!=nullptr);CHECK_EQ(caption->timer.current,base_caption_time);
        CHECK_EQ(app.files.ordinary_records[0].total_frames,base_play);
        while(session.Correcting()) step();
        CHECK(session.AllFramesCommitted());
        CHECK_EQ(app.frames,base_frames+distance);
        CHECK_EQ(app.files.ordinary_records[0].total_frames,base_play+distance);
        CHECK(session.FinishWorldSimulation());CHECK(!session.Simulating());
        // R-G: ending/result input runs locally after the confirmed battle tail.
        CHECK(app.begin_title(3));CHECK(app.run==nullptr);CHECK(app.title!=nullptr);
        CHECK(app.step(GameInput{},60.f,false));CHECK(!session.Simulating());
        session.Stop();
    }
}

// A stage activates inside an already granted network frame. The new battle
// must receive every seat's saved input before its first scheduler update.
TEST(network_stage_activation_inputs_and_resources) {
    using namespace th13::multiplayer;
    for (bool ghost : {false, true}) {
    std::uint64_t expected = 0;
    for (unsigned local : {0u, 1u}) {
        session.Stop();
        App app; CHECK(p32_pin_boot(app, "network-stage-activation.bin"));
        CHECK(app.configure_multiplayer(2, {0, 1, 0}, int(local)));
        SessionSetup setup; setup.version=2; setup.mode=2; setup.automatic=false;
        setup.session_id=42; setup.seed=1234; setup.build[0]=1; setup.local_player=local;
        setup.selections={0,1,0};
        const auto words=EncodeSessionSetup(setup);
        CHECK(session.Configure(words.data(),words.size()));
        CHECK(app.start_game(1)); CHECK(app.complete_loading());
        CHECK(app.apply_destination()); CHECK(app.run != nullptr);
        if (!app.run) return;
        CHECK(session.BeginLocalSimulation());
        auto& old=app.run->battle;
        old.seat_huds[0].extra_lives=4; old.seat_huds[0].bombs=1;
        old.seat_huds[0].power=173; old.seat_huds[0].life_pieces=2;
        old.seat_huds[1].extra_lives=0; old.seat_huds[1].bombs=0;
        old.seat_huds[1].power=249; old.seat_huds[1].bomb_pieces=3;
        old.seats[1].ghost=ghost;
        old.trance_generation=7;
        const std::array<u32,2> held{0x49u,0x81u};
        CHECK(session.CaptureLocal(held[local],0));session.AdvanceCapturePhase();
        CHECK(session.SubmitRemote(1-local,0,Netplay::FrameInput(held[1-local])) == Netplay::RemoteInputResult::Accepted);
        CHECK(session.CanSimulate());
        app.progress.stage=2; app.stage_transition_active=true; app.stage_startup_age=30;
        CHECK(app.update(session.MenuInput(),false));CHECK(app.commit(60.f));
        session.OnFrameSimulated();
        CHECK_EQ(app.progress.stage_frame,1);
        auto& next=app.run->battle;
        CHECK_EQ(next.seat_inputs[0].held,held[0]);
        CHECK_EQ(next.seat_inputs[1].held,held[1]);
        CHECK(next.seats[0].x<0);CHECK(next.seats[1].x>0);
        CHECK_EQ(next.seat_huds[0].extra_lives,4);CHECK_EQ(next.seat_huds[0].bombs,1);
        CHECK_EQ(next.seat_huds[0].power,173);CHECK_EQ(next.seat_huds[0].life_pieces,2);
        CHECK_EQ(next.seat_huds[1].extra_lives,0);CHECK_EQ(next.seat_huds[1].bombs,ghost ? 2 : 0);
        // R-G stage-clear revival restores half max Power; live seats carry
        // their current Power unchanged. This update already consumed the
        // first new-stage player tick, so the 120-frame protection is now119.
        CHECK_EQ(next.seat_huds[1].power,ghost ? next.seat_huds[1].max_power / 2 : 249);
        CHECK_EQ(next.seat_huds[1].bomb_pieces,ghost ? 0 : 3);
        if (ghost) CHECK_EQ(next.seats[1].invulnerable,119);
        CHECK(!next.seats[1].ghost);CHECK(!next.seats[1].dead);
        CHECK_EQ(next.trance_generation,7u);
        WorldState state;CHECK(state.capture(next,app.visual,nullptr,app.run->presentation.get(),app.run.get(),app.mp_transition_handle_));
        if (local==0) expected=state.digest(); else CHECK_EQ(state.digest(),expected);
        session.Stop();
    }
    }
}

TEST(audit_epoch_and_timing_bounds) {
    using namespace th13::multiplayer;
    DialogueText::RollbackState dialogue{};
    dialogue.pixels = std::make_shared<std::vector<u8>>(16, 0);
    const auto original_host = WorldState::digest_dialogue(dialogue);
    auto mutated_host = dialogue;
    mutated_host.box = 42;
    CHECK(WorldState::digest_dialogue(mutated_host) != original_host);
    mutated_host = dialogue;
    mutated_host.cached[0] = true;
    CHECK(WorldState::digest_dialogue(mutated_host) != original_host);
    mutated_host = dialogue;
    mutated_host.pixels = std::make_shared<std::vector<u8>>(16, 1);
    CHECK(WorldState::digest_dialogue(mutated_host) != original_host);
    session.Stop();
    Fixture world;CHECK(world.init());CHECK(world.battle.enable_seats(world.provider,2,{0,0,0}));
    GameRandom visual{1234};
    SessionSetup setup;setup.version=2;setup.mode=2;setup.automatic=false;
    setup.session_id=42;setup.build[0]=1;
    const auto words=EncodeSessionSetup(setup);CHECK(session.Configure(words.data(),words.size()));
    session.BindWorld(&world.battle,&visual,nullptr);CHECK(session.BeginLocalSimulation());
    session.SetAudit(true);
    CHECK(session.CaptureLocal(0,0));session.AdvanceCapturePhase();CHECK(session.CanSimulate());
    CHECK(world.battle.step(*session.LocalDelayedInput()));session.OnFrameSimulated();
    CHECK_EQ(session.RecordCount(),1u);
    session.SetAudit(false);
    CHECK(session.SubmitRemote(1,0,{})==Netplay::RemoteInputResult::PredictionCorrect);
    CHECK(session.Pump(1));CHECK_EQ(session.RecordCount(),0u);
    CHECK(session.TakeAuditJson().find("\"frames\":[]")!=std::string::npos);
    session.SetAudit(true);
    WorldState before;CHECK(before.capture(world.battle,visual));
    for(unsigned i=0;i<4097;++i)session.TraceTiming("probe",i);
    const auto timing=session.TakeTimingJson();
    CHECK(timing.find("\"overflow\":1")!=std::string::npos);
    std::size_t events=0,at=0;
    while((at=timing.find("\"kind\":",at))!=std::string::npos){++events;++at;}
    CHECK_EQ(events,4096u);
    WorldState after;CHECK(after.capture(world.battle,visual));CHECK_EQ(after.digest(),before.digest());
    CHECK(session.FinishWorldSimulation(true));
    const auto terminal=session.TakeAuditJson();
    std::ostringstream expected;
    expected << "\"full\":\"" << std::hex << after.digest() << "\"";
    CHECK(terminal.find(expected.str())!=std::string::npos);
    CHECK(terminal.find("\"frames\":1,\"completed\":true")!=std::string::npos);
    CHECK(session.TakeAuditJson().find(expected.str())!=std::string::npos);
    session.Stop();CHECK(session.TakeTimingJson().find("\"events\":[]")!=std::string::npos);
    CHECK(session.TakeAuditJson().find("\"terminal\":null")!=std::string::npos);
}

TEST(network_dialog_prediction_one_three_twelve) {
    using namespace th13::multiplayer;
    for (unsigned distance : {1u, 3u, 12u}) {
        std::vector<u64> expected;
        for (bool prediction : {false, true}) {
            session.Stop();
            App app; CHECK(p32_pin_boot(app, "network-dialog-prediction.bin"));
            CHECK(app.configure_multiplayer(2, {0,0,0}, 0));
            SessionSetup setup; setup.version=2; setup.mode=2; setup.automatic=false;
            setup.session_id=42; setup.seed=1234; setup.build[0]=1;
            const auto words=EncodeSessionSetup(setup); CHECK(session.Configure(words.data(),words.size()));
            CHECK(session.BeginLocalSimulation());
            app.progress.difficulty=1; CHECK(app.start_game(1)); app.pending_destination=-1;
            app.game_random.reseed_restore(1234); app.visual.reseed_restore(1234);
            CHECK(app.begin_run()); CHECK(app.complete_loading());
            const auto step=[&] {
                CHECK(session.CanSimulate());
                CHECK(app.update(session.MenuInput(),false)); CHECK(app.commit(60.f));
                session.OnFrameSimulated(); CHECK(app.present());
            };
            // Confirm the launch, then enter the real MSG interpreter and let
            // ApplicationState publish its prediction policy for this scene.
            for(unsigned frame=0;frame<2;++frame) {
                CHECK(session.CaptureLocal(0,frame)); session.AdvanceCapturePhase();
                CHECK(session.SubmitRemote(1,frame,{})==Netplay::RemoteInputResult::Accepted);
                step(); CHECK(session.Pump(frame));
                if(frame==0) app.run->open_dialog(0);
            }
            CHECK(app.run->dialog_active()); CHECK(!session.PredictionBarrier());
            const auto observe=[&](unsigned index) {
                WorldState state; CHECK(state.capture(app.run->battle,app.visual,nullptr,
                    app.run->presentation.get(),app.run.get(),app.mp_transition_handle_));
                if(prediction) CHECK_EQ(state.digest(),expected[index]);
                else expected.push_back(state.digest());
            };
            for(unsigned index=0;index<distance;++index) {
                const unsigned frame=index+2;
                CHECK(session.CaptureLocal(0,frame)); session.AdvanceCapturePhase();
                if(!prediction) CHECK(session.SubmitRemote(1,frame,
                    Netplay::FrameInput(InputLanes::Confirm | InputLanes::Dialogue | 1))==Netplay::RemoteInputResult::Accepted);
                step();
                if(!prediction) { CHECK(session.Pump(frame)); observe(index); }
            }
            if(prediction) {
                for(unsigned index=0;index<distance;++index) {
                    const auto result=session.SubmitRemote(1,index+2,
                        Netplay::FrameInput(InputLanes::Confirm | InputLanes::Dialogue | 1));
                    CHECK(result==Netplay::RemoteInputResult::RollbackRequired ||
                          result==Netplay::RemoteInputResult::Accepted);
                }
                CHECK(session.Pump(distance+2)); CHECK_EQ(session.SimFrame(),2u);
                for(unsigned index=0;index<distance;++index) { step(); observe(index); }
                CHECK_EQ(session.Restores(),1u); CHECK(!session.Correcting());
            }
            session.Stop();
        }
    }
}

TEST(history_remote_menu_edges) {
    using namespace th13::multiplayer;
    session.Stop();
    App app; CHECK(p32_pin_boot(app, "history-remote-menu.bin"));
    CHECK(app.configure_multiplayer(3, {0, 0, 0}, 0));
    SessionSetup setup; setup.version=2; setup.mode=2; setup.automatic=false;
    setup.session_id=42; setup.build[0]=1; setup.player_count=3;
    const auto words=EncodeSessionSetup(setup); CHECK(session.Configure(words.data(), words.size()));
    app.mp_history_playback = true;
    CHECK(app.start_game(1)); CHECK(app.apply_destination()); CHECK(app.complete_loading());
    std::array<Netplay::FrameInput, 3> inputs{};
    inputs[0].buttons=InputLanes::Dialogue;
    inputs[2].buttons=InputLanes::Pause | InputLanes::Dialogue | 1;
    auto menu=InputLanes::AdvanceHistory(app.run->battle, inputs.data(), inputs.size());
    CHECK(menu.pressed & 0x100u); CHECK(menu.pressed & 1);
    CHECK(app.run->playback_input() == &app.run->battle.seat_inputs[0]);
    CHECK(app.update(menu, false)); CHECK(app.commit(60)); CHECK(app.pause->active());
    inputs[2].buttons=0;
    for (int i=0;i<12;++i) {
        menu=InputLanes::AdvanceHistory(app.run->battle,inputs.data(),inputs.size());
        CHECK(app.update(menu,false));CHECK(app.commit(60));
    }
    CHECK_EQ(app.pause->state.phase,6);
    inputs[2].buttons=InputLanes::Pause | InputLanes::Dialogue;
    menu=InputLanes::AdvanceHistory(app.run->battle,inputs.data(),inputs.size());
    CHECK(menu.pressed & 0x100u);
    CHECK(menu.pressed & 0x200u); // P1's held Ctrl must not hide P3's press.
    CHECK(app.update(menu,false));CHECK(app.commit(60));
    inputs[2].buttons=0;
    for(int i=0;i<15;++i) {
        menu=InputLanes::AdvanceHistory(app.run->battle,inputs.data(),inputs.size());
        CHECK(app.update(menu,false));CHECK(app.commit(60));
    }
    CHECK(!app.pause->active());
    session.Stop();
}

TEST(network_pause_restart_and_terminal_save) {
    using namespace th13::multiplayer;
    session.Stop();
    App app;CHECK(p32_pin_boot(app,"network-menu-restart.bin"));
    CHECK(app.configure_multiplayer(2,{0,0,0},0));
    SessionSetup setup;setup.version=2;setup.mode=2;setup.automatic=false;
    setup.session_id=42;setup.build[0]=1;
    const auto words=EncodeSessionSetup(setup);CHECK(session.Configure(words.data(),words.size()));
    CHECK(app.start_game(1));CHECK(app.apply_destination());CHECK(session.BeginLocalSimulation());
    unsigned frame=0;
    const auto tick=[&](u32 remote) {
        CHECK(app.complete_loading());
        CHECK(session.CaptureLocal(0,frame));session.AdvanceCapturePhase();
        CHECK(session.SubmitRemote(1,frame,Netplay::FrameInput(remote))==Netplay::RemoteInputResult::Accepted);
        CHECK(session.CanSimulate());
        CHECK(session.Pump(frame));CHECK_EQ(session.RecordCount(),1u);
        CHECK(app.update(session.MenuInput(),false));CHECK(app.commit(60.f));
        session.OnFrameSimulated();CHECK(app.present());++frame;
    };
    tick(InputLanes::Pause);
    CHECK(app.pause->active());
    for(int i=0;i<12;++i)tick(0);
    CHECK_EQ(app.pause->state.phase,6);
    app.run->battle.seat_huds[0].bombs=0;
    tick(InputLanes::Restart);
    for(int i=0;i<35;++i)tick(0);
    CHECK_EQ(app.progress.stage,1);CHECK(!app.pause->active());
    CHECK_EQ(app.run->battle.seat_huds[0].bombs,2);
    CHECK_EQ(app.run->battle.seat_huds[1].bombs,2);
    Netplay::FrameInput input;
    CHECK(session.CaptureLocal(0x200000u | 0x10000u,frame));
    const auto captured=session.Runtime().Prepare(frame);
    CHECK_EQ(captured.inputs[0].buttons,InputLanes::Restart | InputLanes::Quit);
    bool available=false;unsigned saved=0;
    app.multiplayer_replay_available=[&]{return available;};
    app.multiplayer_replay_save=[&](i32 slot,const std::string& name){
        CHECK_EQ(slot,3);CHECK(name=="NETPLAY");++saved;return true;
    };
    CHECK(!app.can_save_replay());CHECK(!app.result_replay_save_available());
    available=true;CHECK(app.can_save_replay());CHECK(app.result_replay_save_available());
    CHECK(app.save_replay_slot(3,"NETPLAY"));CHECK_EQ(saved,1u);
    CHECK(session.FinishWorldSimulation(true));CHECK(session.WorldCompleted());
    session.Stop();CHECK(!session.WorldCompleted());
}
