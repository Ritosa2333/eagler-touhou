// op334 scene_effect kind1 定向测试：显式位置传递 → AnmManager::spawn_effect
// 8 段小粒子轨迹（0x4bba80[1]：init 401000 / tick 401190 / draw 4012c0）。
// 原版锚点（容器内 capstone 逐条核对，证据 artifacts/verification/round-20261028/effect/）：
//   41b7ac 解析 1 个 int(kind) 后 415650(null,&out,kind,敌机VM+0x44)——位置=
//   敌机 ECL VM 主位置（port: StageEnemy.position），非玩家坐标、非常量；
//   401000：seg0=(x+320,y+16)、angle=rand11*PI、Timer{last=0,idx=1,acc=1.0}、
//   8 色 R=255-32i/G=0x80/B=0xFF/A=255（i≥4: A=255-16*(i-4)）；
//   401190：idx>=8 → ret 1（退休）；seg[idx]=seg[idx-1]+(sin a·s, cos a·s)
//   （401490 fsincos），s=unit()*5+4（472de0 先取），a+=rand11*PI/5（472e90
//   回卷，472e10 后取）——RNG 每帧消耗顺序 unit→signed_unit。
#include "ui_test_fixtures.hpp"
#include "../ecl/ecl_test_fixtures.hpp"
#include "../harness/test_framework.hpp"

#include "game/AnmEnvironment.hpp"
#include "game/AnmManager.hpp"
#include "game/AnmVisualState.hpp"
#include "game/EclContext.hpp"
#include "game/StageSim.hpp"

#include <cmath>
#include <cstring>
#include <vector>

using namespace th13;
using th13test::TestFileReader;

namespace {
u32 bits(float f) { u32 x; std::memcpy(&x, &f, 4); return x; }
}

// case334 把执行敌机的 +0x44 主位置显式交给回调（41b7ac edi+0x44）。
TEST(scene_effect_dispatch_passes_enemy_position) {
    StageSim sim;
    sim.commands.sim = &sim;
    StageEnemy enemy;
    enemy.position[0] = -64.f;
    enemy.position[1] = 32.f;
    enemy.position[2] = 0.f;
    bool called = false;
    i32 seen_kind = -1;
    Vec3 seen_position{};
    sim.scene_effect = [&](i32 kind, const Vec3& position) {
        called = true;
        seen_kind = kind;
        seen_position = position;
        return true;
    };
    sim.current = &enemy;
    EclContext context;
    std::vector<u8> code;
    th13test::emit(code, 0, 334, 0, 255, 0, {1});  // mask=0：字面 kind=1
    context.instruction = reinterpret_cast<const EclInstruction*>(code.data());
    CHECK_EQ(sim.commands.execute(context, 334), 0);
    CHECK(called);
    CHECK_EQ(seen_kind, 1);
    CHECK_EQ(bits(seen_position.x), bits(-64.f));
    CHECK_EQ(bits(seen_position.y), bits(32.f));
    CHECK_EQ(bits(seen_position.z), bits(0.f));
}

// spawn_effect(vm,1) 按 401000 初始化 8 段轨迹；401190 逐帧语义与 8 段退休。
TEST(scene_effect_kind1_trail_init_tick_retire) {
    GameRandom random{0};
    AnmEnvironment env;
    AnmManager manager{random, env};
    TestFileReader reader;
    std::vector<u8> bytes;
    CHECK(reader.read("artifacts/reference/raw/ascii.anm", bytes));
    CHECK(manager.load(2, bytes.data(), u32(bytes.size())));
    CHECK(manager.sprite_fallback(2));
    CHECK(reader.read("artifacts/reference/raw/effect.anm", bytes));
    const bool loaded = manager.load(8, bytes.data(), u32(bytes.size()));
    if (!loaded) std::printf("load8 error=%s\n", manager.error.c_str());
    CHECK(loaded);
    const Vec3 spawn{-64.f, 32.f, 0.f};
    // 位置经 create 的 translation 通道携带（401000 init 参数）；
    // spawn_effect 按 415650 语义（fldz）读取后清零绘制位。
    const u32 handle = manager.create(8, 0, -1, 0, spawn);
    auto* vm = manager.registry.find(handle);
    CHECK(vm);
    if (!vm) return;
    const u32 draws_before_spawn = random.index();
    const u16 seed_before = random.seed();
    CHECK(manager.spawn_effect(*vm, 1));
    CHECK(vm->geometry.trail);
    CHECK_EQ(vm->visual.layer, 13);
    CHECK_EQ(bits(vm->visual.translation.x), bits(0.f));
    CHECK_EQ(bits(vm->visual.translation.y), bits(0.f));
    CHECK(vm->visual.flags & 0x20u);
    auto& trail = *vm->geometry.trail;
    CHECK_EQ(trail.segment_count, 8);
    CHECK_EQ(vm->geometry.allocation_bytes, 0x78u);
    // 401000：Timer 尾 last=0/idx=1/acc=1.0（age.set(1)）
    CHECK_EQ(trail.age.current, 1);
    CHECK_EQ(trail.age.previous, 0);
    // seg0 = (x+320, y+16)
    CHECK_EQ(bits(trail.points[0].x), bits(256.f));
    CHECK_EQ(bits(trail.points[0].y), bits(48.f));
    // 8 色（0x40..0x5f；port u32=AARRGGBB，原字节序 [B,G,R,A]）
    CHECK_EQ(trail.colors[0], 0xffff80ffu);
    CHECK_EQ(trail.colors[1], 0xffdf80ffu);
    CHECK_EQ(trail.colors[4], 0xff7f80ffu);
    CHECK_EQ(trail.colors[5], 0xef5f80ffu);
    CHECK_EQ(trail.colors[7], 0xcf1f80ffu);
    // angle = rand11*PI：spawn_effect 只消耗一次 signed_unit（无额外双 RNG）
    GameRandom probe{seed_before};
    CHECK_EQ(bits(trail.angle), bits(float(probe.signed_unit() * 3.1415927410125732f)));
    CHECK_EQ(random.index(), draws_before_spawn + 2);  // 单次消耗（0x472c90 每步计数 +2）
    // 首帧 401190：idx=1≠last=0 → seg1=seg0+(sin a·s, cos a·s)，a+=rand11*PI/5
    const float angle0 = trail.angle;
    const u16 seed0 = random.seed();
    const u32 draws0 = random.index();
    CHECK_EQ(trail.update(random, 1.f), 0);
    CHECK_EQ(random.index(), draws0 + 4);  // 每帧 unit+signed_unit 各 next() 一次（index 各 +2）
    CHECK_EQ(trail.age.current, 2);
    CHECK_EQ(trail.age.previous, 1);
    GameRandom probe2{seed0};
    const float speed = float(probe2.unit() * 5.f + 4.f);  // 472de0 在前
    CHECK_EQ(bits(trail.points[1].x), bits(float(256.f + std::sin(double(angle0)) * double(speed))));
    CHECK_EQ(bits(trail.points[1].y), bits(float(48.f + std::cos(double(angle0)) * double(speed))));
    const float delta = float(probe2.signed_unit() * 3.1415927410125732f / 5.f);
    CHECK_EQ(bits(trail.angle), bits(normalize_angle(float(angle0 + delta))));
    CHECK_EQ(trail.colors[0] >> 24, 239u);  // 已生成段 alpha −16
    // 第 2..7 次更新生成 seg2..seg7（idx→8），第 8 次更新 idx>=8 → 返回 1 退休
    for (i32 frame = 0; frame < 6; ++frame) CHECK_EQ(trail.update(random, 1.f), 0);
    CHECK_EQ(trail.age.current, 8);
    CHECK_EQ(trail.colors[6] >> 24, 207u);  // seg6 仅在 count=7 生成 seg7 时淡出一次（223−16）
    CHECK_EQ(trail.update(random, 1.f), 1);
}

// 新签名下 kind6 路径契约保持（回调形状、kind 直通）。
TEST(scene_effect_callback_contract_kind6_unchanged) {
    StageSim sim;
    sim.commands.sim = &sim;
    StageEnemy enemy;
    sim.current = &enemy;
    bool called = false;
    sim.scene_effect = [&](i32 kind, const Vec3&) { called = kind == 6; return called; };
    EclContext context;
    std::vector<u8> code;
    th13test::emit(code, 0, 334, 0, 255, 0, {6});
    context.instruction = reinterpret_cast<const EclInstruction*>(code.data());
    CHECK_EQ(sim.commands.execute(context, 334), 0);
    CHECK(called);
}

int main() { return th13test::run_all("scene-effect-dispatch"); }
