// 妖梦 S1->S2 装载门的 option 卫星生命周期（0x42b6d0 / 0x442960 / 0x441b00 /
// 0x444df0）。
//
// 原版证据（capstone 直读 th13.exe；decomp 见 /work/artifacts/decomp/jp/）：
//   0x42b6d0（0x42cd40 关终调用）：0x42b6e0 or [eax+0x14698],8 置隐藏位；
//     0x42b72f/0x42b753/0x42b78e/0x42b7b2 mov [vm+0x4a0],3 对 8 个槽的两个
//     根 VM（0xa368 普通树 / 0xa36c 满P树）及直接子体写 pending=3；
//     0x42b7d4 mov [edx+0x1469c],0 清隐藏计数；全程无 0x462450 调用。
//   0x442960：0x442f2b test bit3；0x442f34 inc [edi+0x1469c]；0x443017
//     cmp [edi+0x1469c],0x1e；0x44301e jl；0x44302e 清槽 active 标志
//     player+0xa2b8；0x443059/0x443089/0x4430c9/0x4430f9 对两个根置
//     pending=1；0x443114 继续下一槽；无立即 tick。
//   0x441b00：清隐藏位 + 对两个句柄及直接子体 deferred pending=2。
//   0x444df0：power<max（妖梦 S2 293<400）→ 0x46fc30(handle,1) 满P树 +
//     公共循环 retire/create 普通根；power>=max 才走 0x11/0x14/0x15 分支。
//
// pl03.anm 实证（/work/artifacts/reference/raw/pl03.anm，脚本按全局序 15/16）：
//   script15: op403 t0 / op500 spawn16 t0 / op200 [12,0] t4 / label1 t4 /
//             op1 t8；每 4 帧循环 spawn。
//   script16: label2 t0 / op119 mask3（10010 抽）/ op117 mask3（10011 抽）/
//             op1 t16；每实例每次出生 2 次 next() = 4 index 单位。
//
// 原版采样（A8/original-continuous-full/battle-replay-core-v2.jsonl）：
//   12184=268280，12185=268304，12186=268312，12210=268504，12214=268528，
//   12334=269248。卫星绘制在 12186,12190,...,12210（7 周期 × 8 单位 = 56），
//   12214 的出生被 0x442960 拦下，根 12218 回收；基线多 248（38 周期 × 8）。
//
// 本测试用真实 pl03.anm 作 player bank、合成 StageBattle（character=3、
// power=293<max=400、2 个 active option），按生产帧序 sync（优先级 20）→
// 主链 update(false)（Application Update 34）推进。所有断言只覆盖本层可
// 观测的绘制/RNG 相位；生产 wasm/浏览器对拍由主会话统一执行（本轨禁止）。
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"

#include "game/EclResource.hpp"
#include "game/StageBattle.hpp"
#include "game/StagePresentation.hpp"

#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

struct NullGraphics final : ZunGraphics {
    touhou::graphics::PipelineState& pipeline() override { return pipeline_state; }
    u32 texture(const AnmResource&, u32) override { return 1; }
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

std::vector<u8> read_asset(const char* name) {
    std::ifstream f(std::string("artifacts/reference/raw/") + name, std::ios::binary);
    return {std::istreambuf_iterator<char>(f), {}};
}

// prepare_player 在 sht_name==nullptr && cold_entry==false 时不读 provider；
// 仍保留真实接口，避免测试静默依赖空实现。
struct UnusedProvider final : EclResourceProvider {
    bool read(const std::string&, std::vector<u8>&) override { return false; }
    bool animation(u32, const std::string&) override { return false; }
};

u32 count_script(AnmManager& manager, i32 script) {
    u32 count = 0;
    for (u32 i = 0; i < manager.registry.ordered_count(false); ++i) {
        const AnmVm* vm = manager.registry.find(manager.registry.ordered_handle(false, i));
        if (vm && vm->source_script == script) ++count;
    }
    return count;
}

// 旧 option 树（script15 根）当前挂着的直接子体（script16 卫星）总数。
u32 satellite_count(AnmManager& manager) {
    u32 count = 0;
    for (u32 i = 0; i < manager.registry.ordered_count(false); ++i) {
        AnmVm* vm = manager.registry.find(manager.registry.ordered_handle(false, i));
        if (vm && vm->source_script == 15)
            count += u32(manager.registry.children(*vm).size());
    }
    return count;
}

// 当前所有 option 根（script15）的 translation。
std::vector<Vec3> option_root_positions(AnmManager& manager) {
    std::vector<Vec3> positions;
    for (u32 i = 0; i < manager.registry.ordered_count(false); ++i) {
        AnmVm* vm = manager.registry.find(manager.registry.ordered_handle(false, i));
        if (vm && vm->source_script == 15)
            positions.push_back(vm->visual.translation);
    }
    return positions;
}

struct OptionHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    NullGraphics graphics;
    AnmRenderer renderer{graphics};
    StageBattle battle;
    SessionState progress;
    HudPlayerState player;
    UnusedProvider provider;
    std::unique_ptr<StagePresentation> presentation;
    bool ready = false;

    OptionHarness() {
        const auto pl03 = read_asset("pl03.anm");
        if (pl03.empty()) {
            CHECK(!"artifacts/reference/raw/pl03.anm missing");
            return;
        }
        if (!manager.load(6, pl03.data(), u32(pl03.size()))) {  // banks.player
            std::printf("pl03 load error: %s\n", manager.error.c_str());
            CHECK(false);
            return;
        }
        progress.character = 3;  // 妖梦 = pl03
        // 原版 S2 激活时 004be7e8=293 < 004be7ec=400（0x444df0 power<max 分支）。
        player.power = 293;
        player.max_power = 400;
        player.power_step = 100;
        if (!battle.prepare_player(provider, progress, player, nullptr, false)) {
            std::printf("player prepare error: %s\n", battle.error().c_str());
            CHECK(false);
            return;
        }
        battle.option_count = 2;
        for (size_t i = 0; i < 2; ++i) {
            battle.options[i].active = true;
            battle.options[i].x = i == 0 ? -20.f : 20.f;
            battle.options[i].y = 300.f;
        }
        presentation = std::make_unique<StagePresentation>(
            manager, renderer, StagePresentation::Banks{4, 5, 6, 8, 25}, 0);
        ready = true;
    }

    // 生产帧序：RunScene::update（优先级 20）sync_player_options 先于
    // Application Update 34 的主链 update(false)。返回本帧视觉 RNG index 增量。
    u32 frame() {
        const u32 before = random.index();
        CHECK(presentation->sync_player_options(battle));
        CHECK(manager.update(false));
        if (!presentation->error().empty())
            std::printf("option harness error: %s\n", presentation->error().c_str());
        CHECK(presentation->error().empty());
        return random.index() - before;
    }

    // 建树帧（S1 对局中）：sync 创建两个根（各自首 tick 里 op500 生成
    // script16 并抽 2 次 next()），同帧主链 tick 再把根/子各推进一帧。
    u32 create_frame() { return frame(); }
};

// 固定生命周期：门首 hide → 门内第 30 帧（计数 29）起每帧幂等 interrupt。
u32 run_gate(bool hide, bool interrupt, std::vector<u32>* per_frame = nullptr,
             u32 frames = 150) {
    OptionHarness h;
    CHECK(h.ready);
    if (!h.ready) return 0;
    CHECK_EQ(h.create_frame(), 8u);  // 2 根 × 4 index 单位
    CHECK_EQ(h.frame(), 0u);         // 相位推进：根 timer 2→3
    CHECK_EQ(h.frame(), 0u);         // 根 timer 3→4（门首应处 spawn 相位）
    if (per_frame) per_frame->assign(frames, 0u);
    u32 total = 0;
    for (u32 f = 0; f < frames; ++f) {
        if (f == 0 && hide) {
            CHECK(h.presentation->hide_player_options());
            CHECK(h.presentation->error().empty());
        }
        if (f >= 29 && interrupt) {
            // 0x442960 每帧 counter>0x1d 重入：重复调用必须幂等。
            CHECK(h.presentation->interrupt_player_options());
            CHECK(h.presentation->error().empty());
        }
        const u32 delta = h.frame();
        if (per_frame) (*per_frame)[f] = delta;
        total += delta;
    }
    return total;
}

}  // namespace

TEST(hidden_option_tree_delays_satellite_draws_and_stops_after_frame_25) {
    std::vector<u32> per_frame;
    const u32 total = run_gate(true, true, &per_frame);
    // 门首 hide 使本应 frame0 的出生顺延一帧；此后每 4 帧一次，
    // 最后一次在 frame25；门内第 30 帧（frame29）的 label1 拦下 frame29
    // 出生，根按 label1(t=4)→op1(t=8) 于 frame33 回收。
    CHECK_EQ(per_frame.size(), 150u);
    if (per_frame.size() != 150u) return;
    CHECK_EQ(per_frame[0], 0u);
    for (u32 f = 0; f < per_frame.size(); ++f) {
        const bool satellite = f >= 1 && f <= 25 && (f - 1) % 4 == 0;
        CHECK_EQ(per_frame[f], satellite ? 8u : 0u);  // 2 根 × 2 next × 2 index
    }
    CHECK_EQ(total, 56u);  // 7 周期 × 8 单位，原版 12186..12210 同额
}

TEST(without_interrupt_hidden_tree_keeps_spawning_38_cycles_plus_248) {
    // 负控：只 hide 不 interrupt → 出生相位 1,5,...,149，38 周期 × 8 = 304；
    // 相对固定 56 恰为基线 +248。304/+248 是 PORT BASELINE（baseline web
    // capture, wasm f54ce562）的算术，不是 native core 值。
    const u32 total = run_gate(true, false);
    CHECK_EQ(total, 304u);
    CHECK_EQ(total - 56u, 248u);
}

TEST(without_hide_first_companion_draws_on_frame_zero) {
    // 负控：不 hide → 首个卫星在 frame0 绘制（基线的 +8 早一帧相位）。
    OptionHarness h;
    CHECK(h.ready);
    if (!h.ready) return;
    CHECK_EQ(h.create_frame(), 8u);
    CHECK_EQ(h.frame(), 0u);
    CHECK_EQ(h.frame(), 0u);
    std::vector<u32> per_frame(150, 0u);
    u32 total = 0;
    for (u32 f = 0; f < 150; ++f) {
        per_frame[f] = h.frame();
        total += per_frame[f];
    }
    CHECK_EQ(per_frame[0], 8u);
    for (u32 f = 0; f < 150; ++f)
        CHECK_EQ(per_frame[f], (f % 4 == 0) ? 8u : 0u);
    CHECK_EQ(total, 304u);  // 38 周期 × 8
}

TEST(hidden_gate_pins_option_roots_to_player_position_not_stale_options) {
    // 0x442960 隐藏支路（decomp 00442960.c:345-348；capstone 0x442f5d
    // test bit3 / 0x443005 mov edx,[edi+0x5c4] / 0x44300b mov [esi-0xc],edx）
    // 把 option 槽位置直接写成 player+0x5c4/+0x5c8。门内
    // StageBattle::update_options 因 option_movement_tick_=false 不刷新
    // battle.options[i].x/y（StageBattle.cpp:2070/2073），这些是过时值；
    // 移植侧隐藏期必须钉 player 位置而不是读 battle.options。
    OptionHarness h;
    CHECK(h.ready);
    if (!h.ready) return;
    CHECK_EQ(h.create_frame(), 8u);
    CHECK_EQ(h.frame(), 0u);
    CHECK_EQ(h.frame(), 0u);
    // 建树时的 battle.options 旧值（模拟换关前位置），与下面玩家位置不同。
    CHECK_EQ(h.battle.options[0].x, -20.f);
    CHECK_EQ(h.battle.options[0].y, 300.f);
    CHECK_EQ(h.battle.options[1].x, 20.f);
    h.battle.set_player_pos(64.f, 400.f);  // port 的 player+0x5c4/+0x5c8
    CHECK(h.presentation->hide_player_options());
    CHECK(h.presentation->error().empty());
    CHECK(h.presentation->sync_player_options(h.battle));
    CHECK(h.presentation->error().empty());
    const auto roots = option_root_positions(h.manager);
    CHECK_EQ(roots.size(), 2u);
    for (const auto& position : roots) {
        CHECK_EQ(position.x, 64.f);
        CHECK_EQ(position.y, 400.f);
    }
    // 隐藏支路每帧重钉（0x442960 每帧执行），主链 tick 后仍是玩家位置。
    CHECK_EQ(h.frame(), 0u);
    for (const auto& position : option_root_positions(h.manager)) {
        CHECK_EQ(position.x, 64.f);
        CHECK_EQ(position.y, 400.f);
    }
}

TEST(stage_activation_does_not_rebroadcast_label2_to_old_option_tree) {
    // 固定路径：门内 frame29 interrupt 后旧树于 frame33 整体回收；
    // frame40（S2 激活前）registry 里已无 script15/16，0x441b00 的 label2
    // 广播没有对象，激活帧只剩 0x444df0 重建两棵树（= 建树成本 8 单位）。
    {
        OptionHarness h;
        CHECK(h.ready);
        if (!h.ready) return;
        CHECK_EQ(h.create_frame(), 8u);
        CHECK_EQ(h.frame(), 0u);
        CHECK_EQ(h.frame(), 0u);
        for (u32 f = 0; f < 40; ++f) {
            if (f == 0) CHECK(h.presentation->hide_player_options());
            if (f >= 29) CHECK(h.presentation->interrupt_player_options());
            const bool satellite = f >= 1 && f <= 25 && (f - 1) % 4 == 0;
            CHECK_EQ(h.frame(), satellite ? 8u : 0u);
        }
        // frame33 根按 label1(t=4)→op1(t=8) 回收并连带子体；此后
        // sync_player_options 因 option_rebuild_pending_ 早退，门内不再重建。
        CHECK_EQ(count_script(h.manager, 15), 0u);
        CHECK_EQ(count_script(h.manager, 16), 0u);
        CHECK_EQ(satellite_count(h.manager), 0u);
        const u32 before = h.random.index();
        CHECK(h.presentation->activate_player_effects(h.battle));
        CHECK(h.manager.update(false));
        const u32 activation = h.random.index() - before;
        CHECK_EQ(activation, 8u);  // 只有 0x444df0 重建，无旧树 label2 重放
        CHECK_EQ(count_script(h.manager, 15), 2u);
        CHECK_EQ(count_script(h.manager, 16), 2u);
    }
    // 负控：不 hide/不 interrupt，旧树活到 frame40；0x441b00 label2 会
    // 重放 6 个存活卫星（每根 3 个 × 2 根）的抽签块，每颗 2 next = 4 单位，
    // 合计 +24——即基线 S2 首帧 +24 的构成（原报告仅推断，这里给出直接
    // 合成证据；原生 6 颗的绝对数仍以主会话 wasm 对拍为准）。
    {
        OptionHarness h;
        CHECK(h.ready);
        if (!h.ready) return;
        CHECK_EQ(h.create_frame(), 8u);
        CHECK_EQ(h.frame(), 0u);
        CHECK_EQ(h.frame(), 0u);
        for (u32 f = 0; f < 40; ++f) CHECK_EQ(h.frame(), (f % 4 == 0) ? 8u : 0u);
        const u32 satellites = satellite_count(h.manager);
        CHECK_EQ(satellites, 6u);
        const u32 before = h.random.index();
        CHECK(h.presentation->activate_player_effects(h.battle));
        CHECK(h.manager.update(false));
        const u32 label2 = h.random.index() - before;
        CHECK_EQ(label2, satellites * 4u);  // 每颗 script16 label2 块 2 次 next()
        CHECK_EQ(label2, 24u);
        CHECK_EQ(count_script(h.manager, 15), 2u);  // 旧树仍在（未 interrupt）
    }
}

int main() { return th13test::run_all("option hidden lifecycle"); }
