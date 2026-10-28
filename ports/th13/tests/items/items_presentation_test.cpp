// 道具/神霊呈现定向单测（真实资源）：StagePresentation 神霊池 → registry
// VM 的出生/层提交/位置同步/收集 retire 全链路（0x438ec0/0x438730/0x439088）。
// 资源 = 真实 enemy.anm+st01enm.anm 合并 stage bank + bullet/pl00；不模拟池。
#include "../harness/test_framework.hpp"

#include "game/AnmManager.hpp"
#include "game/AnmRenderer.hpp"
#include "game/AnmResource.hpp"
#include "game/ItemManager.hpp"
#include "game/SessionState.hpp"
#include "game/StageBattle.hpp"
#include "game/StagePresentation.hpp"

#include <cstdio>
#include <vector>

namespace {

using namespace th13;

bool read_file(const char* relative, std::vector<u8>& out) {
    for (const char* base : {".", "..", "../../..", "../../../.."}) {
        std::string path = std::string(base) + "/" + relative;
        FILE* f = std::fopen(path.c_str(), "rb");
        if (!f) continue;
        std::fseek(f, 0, SEEK_END);
        long n = std::ftell(f);
        std::fseek(f, 0, SEEK_SET);
        out.resize(n);
        std::fread(out.data(), 1, n, f);
        std::fclose(f);
        return true;
    }
    return false;
}

// ZunGraphics 纯虚桩（run_scene_test 同式：不落 GPU，只验提交层与句柄）。
struct NullGraphics final : ZunGraphics {
    touhou::graphics::PipelineState& pipeline() override { return pipeline_state; }
    u32 texture(const AnmResource&, u32) override { return 0; }
    void bind_texture(u32) override {}
    void set_layout(touhou::graphics::VertexLayout) override {}
    void set_matrix(touhou::graphics::MatrixKind, const Matrix4&) override {}
    void triangles(u32, const void*, u32) override {}
    void primitives(touhou::graphics::Topology, u32, const void*, u32) override {}
    bool select_target(const AnmResource*, u32) override { return true; }
    bool clear_target(u32, const GraphicsViewport*) override { return true; }
    bool clear_depth(const GraphicsViewport*) override { return true; }
    void set_viewport(const GraphicsViewport&) override {}

private:
    touhou::graphics::PipelineState pipeline_state;
};

struct PresentationFixture {
    GameRandom random{0x50};
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    NullGraphics graphics;
    AnmRenderer renderer{graphics};
    StagePresentation presentation;

    PresentationFixture(const char* player_anm = "artifacts/reference/raw/pl00.anm")
        : presentation(manager, renderer, StagePresentation::Banks{4, 5, 6}, 0) {
        std::vector<u8> bytes;
        AnmResource stage;
        CHECK(read_file("artifacts/reference/raw/enemy.anm", bytes));
        CHECK(stage.open(bytes.data(), u32(bytes.size())));
        CHECK(read_file("artifacts/reference/raw/st01enm.anm", bytes));
        CHECK(stage.append(bytes.data(), u32(bytes.size())));
        CHECK(manager.adopt(4, std::move(stage)));
        CHECK(read_file("artifacts/reference/raw/bullet.anm", bytes));
        CHECK(manager.load(5, bytes.data(), u32(bytes.size())));
        CHECK(read_file(player_anm, bytes));
        CHECK(manager.load(6, bytes.data(), u32(bytes.size())));
        // 00405530: astral.anm 装在原版槽 25，神霊 VM（0x439088 kind*6+1）
        // 由 StagePresentation 的 spirit bank 25 创建。
        CHECK(read_file("artifacts/reference/raw/astral.anm", bytes));
        CHECK(manager.load(25, bytes.data(), u32(bytes.size())));
    }
};

// 收集器装配（StageBattle::update_item_side 同式）。
ItemCollector collector_for(SessionState& progress, HudPlayerState& player,
                            HudScoreState& score, float x, float y) {
    ItemCollector collector;
    collector.x = x;
    collector.y = y;
    collector.state = 1;
    collector.homing_speed = 5.0f;
    collector.box = {x - 15.0f, y - 15.0f, x + 15.0f, y + 15.0f};
    collector.progress = &progress;
    collector.player = &player;
    collector.score = &score;
    progress.difficulty = 1;
    progress.character = 0;
    return collector;
}

}  // namespace

TEST(spirit_vm_submit_layer8_and_follow) {
    PresentationFixture fx;
    StageBattle battle;
    battle.item_manager.bind_random(battle.sim.random);
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    battle.item_manager.spawn_value_item(0, 10.0f, 300.0f, 0.0f, 0.5f);
    Spirit& s = battle.item_manager.spirits()[0];
    s.x = 10.0f; s.y = 300.0f;
    // 首帧同步：出生提交（kind0 → script 1；层由脚本 ins_304 置 8）。
    CHECK(fx.presentation.sync(battle, 0));
    CHECK(fx.presentation.error().empty());
    // 神霊 VM 在 registry：kind0 常规产件 script = 1（0x439088 lea [kind*6+1]）。
    AnmVm* vm = nullptr;
    for (u32 h = 1; h < 0x10000u; ++h)
        if (auto* candidate = fx.manager.registry.find(h))
            if (candidate->source_script == 1) { vm = candidate; break; }
    CHECK(vm != nullptr);
    if (!vm) return;
    // astral.anm script1：ins_3(-1 t=200) 先挂起（visible=0），首帧后由
    // ins_304(6)/ins_500 子链推进；slot 25 的精灵由子脚本 2/3（30x30）绘制。
    CHECK(!vm->visible);
    CHECK_EQ(vm->visual.layer, 6);  // astral.anm script1 ins_304(6)
    CHECK_EQ(vm->visual.translation.y, 300.0f);
    // 位移跟随（0x438730 尾 +0x574 回写）。
    s.y = 280.0f;
    CHECK(fx.presentation.sync(battle, 0));
    CHECK_NEAR(vm->visual.translation.y, 280.0f, 1e-5f);
}

TEST(spirit_vm_kind1_and_trance_variant_script) {
    PresentationFixture fx;
    StageBattle battle;
    battle.item_manager.bind_random(battle.sim.random);
    battle.item_manager.spawn_value_item(1, 0.0f, 300.0f, 0.0f, 0.5f);
    CHECK(fx.presentation.sync(battle, 0));
    bool saw7 = false;
    for (u32 h = 1; h < 0x10000u; ++h)
        if (auto* candidate = fx.manager.registry.find(h))
            if (candidate->source_script == 7) saw7 = true;
    CHECK(saw7);  // kind1 → script 7

    // trance 期产件（bit0）→ kind*6+4（0x4390aa 分支）。
    SessionState session;
    trance_begin(session);
    battle.item_manager.bind_session(session);
    battle.item_manager.spawn_value_item(2, 40.0f, 300.0f, 0.0f, 0.5f);
    CHECK(fx.presentation.sync(battle, 0));
    bool saw16 = false;
    for (u32 h = 1; h < 0x10000u; ++h)
        if (auto* candidate = fx.manager.registry.find(h))
            if (candidate->source_script == 16) saw16 = true;  // kind2 → 2*6+4
    CHECK(saw16);
}

TEST(spirit_vm_retire_on_collect) {
    PresentationFixture fx;
    StageBattle battle;
    battle.item_manager.bind_random(battle.sim.random);
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    battle.item_manager.spawn_value_item(0, 5.0f, 205.0f, 0.0f, 0.5f);
    Spirit& s = battle.item_manager.spirits()[0];
    s.x = 5.0f; s.y = 205.0f;  // 玩家 (0,200) 20px 内
    CHECK(fx.presentation.sync(battle, 0));
    AnmVm* vm = nullptr;
    for (u32 h = 1; h < 0x10000u; ++h)
        if (auto* candidate = fx.manager.registry.find(h))
            if (candidate->source_script == 1) { vm = candidate; break; }
    CHECK(vm != nullptr);
    // 收集帧：0x438730 收集路径（trance +2 / kind0 残机碎片）。
    auto collector = collector_for(progress, player, score, 0.0f, 200.0f);
    battle.item_manager.update_spirits(collector);
    CHECK(!s.alive);
    CHECK_EQ(progress.ending_points, 2);
    CHECK_EQ(player.life_pieces, 1);
    // 同步后 VM 标 0x20 待回收（0x470110 语义），registry.update 链扫描清出。
    CHECK(fx.presentation.sync(battle, 0));
    CHECK(fx.manager.update());
    bool still = false;
    for (u32 h = 1; h < 0x10000u; ++h)
        if (auto* candidate = fx.manager.registry.find(h))
            if (candidate->source_script == 1) still = true;
    CHECK(!still);
}

TEST(spirit_vm_retire_on_cull) {
    PresentationFixture fx;
    StageBattle battle;
    battle.item_manager.bind_random(battle.sim.random);
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    battle.item_manager.spawn_value_item(3, 230.0f, 200.0f, 0.0f, 0.5f);
    Spirit& s = battle.item_manager.spirits()[0];
    s.x = 230.0f; s.y = 200.0f;  // 越界盒外
    CHECK(fx.presentation.sync(battle, 0));
    auto collector = collector_for(progress, player, score, 0.0f, 200.0f);
    battle.item_manager.update_spirits(collector);
    CHECK(!s.alive);
    CHECK(fx.presentation.sync(battle, 0));
    CHECK(fx.manager.update());
    bool still = false;
    for (u32 h = 1; h < 0x10000u; ++h)
        if (auto* candidate = fx.manager.registry.find(h))
            if (candidate->source_script == 19) still = true;  // kind3 → 19
    CHECK(!still);
}

// ---- file≥3 合并 bank 局部脚本号偏移（r4/effects/integration §1）----

TEST(stage_file3_script_offset_resolves_local_to_global) {
    PresentationFixture fx;
    StageBattle battle;
    AnmEffectRequest req;
    req.kind = 0;
    req.x = 10.0f;
    req.y = 20.0f;
    // file=3（st01enm.anm，合并第二文件）：局部 0 → 全局 139（enemy 139 脚本）。
    req.file = 3;
    req.script = 0;
    battle.sim.anm_effects.push_back(req);
    CHECK(fx.presentation.sync(battle, 0));
    bool saw139 = false;
    for (u32 h = 1; h < 0x10000u; ++h)
        if (auto* candidate = fx.manager.registry.find(h))
            if (candidate->source_script == 139) saw139 = true;
    CHECK(saw139);
    // file=2（enemy.anm，合并首位）：局部=全局，不偏移。
    battle.sim.anm_effects.clear();
    req.file = 2;
    req.script = 0;
    battle.sim.anm_effects.push_back(req);
    CHECK(fx.presentation.sync(battle, 0));
    bool saw0 = false;
    for (u32 h = 1; h < 0x10000u; ++h)
        if (auto* candidate = fx.manager.registry.find(h))
            if (candidate->source_script == 0) saw0 = true;
    CHECK(saw0);
    // file 超出合并文件数：阻塞报错（不猜偏移）。
    battle.sim.anm_effects.clear();
    req.file = 4;
    battle.sim.anm_effects.push_back(req);
    CHECK(!fx.presentation.sync(battle, 0));
    CHECK(!fx.presentation.error().empty());
}

TEST(item_vm_two_scripts_layer0_and_collect_retire) {
    PresentationFixture fx;
    StageBattle battle;
    battle.item_manager.bind_random(battle.sim.random);
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    // type1 小P（0x42f020 双模板：bullet.anm script 90 + 99，层 0）。
    battle.item_manager.spawn_item(10.0f, 205.0f, 0.0f, 1, -1.5707964f, 2.2f);
    CHECK(fx.presentation.sync(battle, 0));
    CHECK(fx.presentation.error().empty());
    AnmVm* body = nullptr;
    AnmVm* side = nullptr;
    for (u32 h = 1; h < 0x10000u; ++h)
        if (auto* candidate = fx.manager.registry.find(h)) {
            if (!body && candidate->source_script == 90) body = candidate;
            if (!side && candidate->source_script == 99) side = candidate;
        }
    CHECK(body != nullptr);
    CHECK(side != nullptr);
    if (body && side) {
        CHECK_EQ(body->visual.layer, 0);  // 模板层（运行时 +0x24 实证）
        CHECK_EQ(side->visual.layer, 0);
        CHECK_NEAR(body->visual.translation.x, 10.0f, 1e-4f);
        CHECK_NEAR(body->visual.translation.y, 205.0f, 1e-4f);  // 出生位（未步进）
        CHECK(body->visible && side->visible);
    }
    // 收集：玩家 (0,200) 盒内 → type1 小P power+1；两 VM 同帧 retire。
    auto collector = collector_for(progress, player, score, 10.0f, 205.0f);
    battle.item_manager.update(collector);
    CHECK(!battle.item_manager.items()[599].alive);
    CHECK_EQ(player.power, 101);
    CHECK(fx.presentation.sync(battle, 0));
    CHECK(fx.manager.update());
    bool still90 = false, still99 = false;
    for (u32 h = 1; h < 0x10000u; ++h)
        if (auto* candidate = fx.manager.registry.find(h)) {
            if (candidate->source_script == 90) still90 = true;
            if (candidate->source_script == 99) still99 = true;
        }
    CHECK(!still90);
    CHECK(!still99);
}

// ---- 妖梦斩击池（YoumuBomb ↔ pl03 script 19/20 VM，r4/bomb 接线）----

TEST(youmu_slash_vm_script19_layer11_and_lifecycle) {
    PresentationFixture fx("artifacts/reference/raw/pl03.anm");
    StageBattle battle;
    // 生成支路（0x40cb50：age0 → cursor0 偶 → script 19、slot 0、lane+jitter）。
    battle.youmu_bomb_.reset();
    auto spawn = battle.youmu_bomb_.step(0, battle.sim.random);
    CHECK(spawn.has_value());
    if (!spawn) return;
    CHECK_EQ(spawn->script, 19);
    CHECK_EQ(spawn->slot, 0);
    battle.youmu_slash_spawns_.push_back(*spawn);
    CHECK(fx.presentation.sync(battle, 0));
    battle.youmu_slash_spawns_.clear();  // battle.step 每帧开头清空（Combat 侧）
    CHECK(fx.presentation.error().empty());
    AnmVm* vm = nullptr;
    for (u32 h = 1; h < 0x10000u; ++h)
        if (auto* candidate = fx.manager.registry.find(h))
            if (candidate->source_script == 19) { vm = candidate; break; }
    CHECK(vm != nullptr);
    if (!vm) return;
    CHECK_EQ(vm->visual.layer, 11);  // pl03 script19 ins_304(11)
    CHECK_NEAR(vm->visual.translation.x, 0.0f, 1e-5f);
    CHECK_NEAR(vm->visual.translation.y, spawn->y, 1e-4f);
    // ins_119 角随机已由 create 首拍执行（rotation.z = fmod(su*π,8)−π/2）。
    CHECK(vm->variables.rotation.z != 0.0f);
    // set_vm 回填：槽 0 已登记 anm 且 alive。
    CHECK(battle.youmu_bomb_.has_anm(0));
    CHECK(battle.youmu_bomb_.active(0));
    CHECK_NEAR(battle.youmu_bomb_.angle(0), vm->variables.rotation.z, 1e-6f);
    // 脚本 op1 自灭（约 16 帧）→ set_vm(false) → 槽 anm 清。
    for (int i = 0; i < 24 && battle.youmu_bomb_.has_anm(0); ++i) {
        CHECK(fx.manager.update());
        CHECK(fx.presentation.sync(battle, 0));
    }
    CHECK(!battle.youmu_bomb_.has_anm(0));
    CHECK(!battle.youmu_bomb_.active(0));
}

int main() {
    return th13test::run_all("items-presentation") ? 1 : 0;
}
