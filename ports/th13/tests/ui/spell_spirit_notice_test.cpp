// 0x43d530 吸灵满槽提示句柄生命周期回归（独立于生产实现的断言源）。
//
// 原版 0043d530.c:24-34 + 0x43d530 反汇编：
//   local = 自机 (x, y-32, z)；script = be808/200 + 0x6b（level1/2/3 =
//   front108/109/110）；只调用 FUN_0046f820 创建并返回指针，**不保存、不回收、
//   不替换上一级**。三个脚本各自 ins_6(96) 自然结束。因此快速跨 200/400/600
//   槽时上一级提示仍在播；端口必须保留所有 live 句柄（getter 只给最近一次），
//   release() 全部回收且对已自然结束句柄幂等。
//
// 负控 1：show_spirit_notice(2) 后 level1 句柄仍可解析（未被 retire）。
// 负控 2：show_notice(61→63) 是 ctx+0xd0 替换语义，旧句柄必须被回收。
#include "../harness/test_framework.hpp"
#include "game/SpellPresentation.hpp"

#include <fstream>
#include <iterator>
#include <vector>

using namespace th13;
namespace {

std::vector<u8> asset(const std::string& name) {
    std::ifstream file("artifacts/reference/raw/" + name, std::ios::binary);
    return {std::istreambuf_iterator<char>(file), {}};
}

struct Fixture {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    bool build() {
        const auto bytes = asset("front.anm");
        return !bytes.empty() && manager.load(3, bytes.data(), u32(bytes.size()));
    }
};

i32 source_script_of(AnmManager& manager, u32 handle) {
    auto* vm = manager.registry.find(handle);
    return vm ? i32(vm->source_script) : -1;
}

}  // namespace

TEST(spirit_notice_keeps_previous_levels_alive) {
    Fixture fixture;
    CHECK(fixture.build());
    SpellPresentation presentation(fixture.manager, SpellPresentation::Banks{4, 1});

    CHECK(presentation.show_spirit_notice(1, 200.0f, 400.0f, 0.0f));
    const u32 first = presentation.spirit_notice_handle();
    CHECK(first != 0);
    CHECK_EQ(source_script_of(fixture.manager, first), 108);

    CHECK(presentation.show_spirit_notice(2, 200.0f, 400.0f, 0.0f));
    const u32 second = presentation.spirit_notice_handle();
    CHECK(second != 0 && second != first);
    CHECK_EQ(source_script_of(fixture.manager, second), 109);
    // 原版不清上一级：level1 VM 必须仍存活。
    CHECK_EQ(source_script_of(fixture.manager, first), 108);

    CHECK(presentation.show_spirit_notice(3, 200.0f, 400.0f, 0.0f));
    const u32 third = presentation.spirit_notice_handle();
    CHECK(third != 0 && third != second);
    CHECK_EQ(source_script_of(fixture.manager, third), 110);
    CHECK_EQ(source_script_of(fixture.manager, first), 108);
    CHECK_EQ(source_script_of(fixture.manager, second), 109);
    CHECK_EQ(presentation.spirit_notice_handles().size(), size_t(3));

    // 位置来自 FUN_0046f820 param_3：y 减 32（0043d530.c:25）。
    auto* vm = fixture.manager.registry.find(first);
    CHECK(vm != nullptr);
    if (vm) {
        CHECK_EQ(vm->visual.translation.x, 200.0f);
        CHECK_EQ(vm->visual.translation.y, 368.0f);
        CHECK_EQ(vm->visual.translation.z, 0.0f);
    }

    // 非法 level 不改状态、不新增句柄。
    CHECK(presentation.show_spirit_notice(0, 0.0f, 0.0f, 0.0f));
    CHECK(presentation.show_spirit_notice(4, 0.0f, 0.0f, 0.0f));
    CHECK_EQ(presentation.spirit_notice_handles().size(), size_t(3));
    CHECK_EQ(presentation.spirit_notice_handle(), third);
}

TEST(spirit_notice_release_retires_every_live_handle) {
    Fixture fixture;
    CHECK(fixture.build());
    SpellPresentation presentation(fixture.manager, SpellPresentation::Banks{4, 1});
    CHECK(presentation.show_spirit_notice(1, 100.0f, 100.0f, 0.0f));
    CHECK(presentation.show_spirit_notice(2, 100.0f, 100.0f, 0.0f));
    CHECK(presentation.show_spirit_notice(3, 100.0f, 100.0f, 0.0f));
    const std::vector<u32> handles = presentation.spirit_notice_handles();
    CHECK_EQ(handles.size(), size_t(3));
    presentation.release();
    // AnmManager::retire schedules bit0x20; the following chain update
    // reclaims the VM. The production manager must consume that retirement.
    CHECK(fixture.manager.update());
    CHECK_EQ(presentation.spirit_notice_handle(), 0u);
    CHECK(presentation.spirit_notice_handles().empty());
    for (u32 handle : handles) CHECK(fixture.manager.registry.find(handle) == nullptr);
}

TEST(spirit_notice_after_natural_end_is_pruned_without_collateral_retire) {
    Fixture fixture;
    CHECK(fixture.build());
    SpellPresentation presentation(fixture.manager, SpellPresentation::Banks{4, 1});
    CHECK(presentation.show_spirit_notice(1, 0.0f, 0.0f, 0.0f));
    const u32 first = presentation.spirit_notice_handle();
    // 脚本 108 帧数结束 → 手动走完更新让 VM 自然结束（沿用引擎 tick）。
    for (i32 i = 0; i < 200; ++i) fixture.manager.update();
    CHECK(fixture.manager.registry.find(first) == nullptr);
    // 下一次创建只摘除死句柄，不影响其它 live 提示。
    CHECK(presentation.show_spirit_notice(2, 0.0f, 0.0f, 0.0f));
    const u32 second = presentation.spirit_notice_handle();
    CHECK(second != 0);
    CHECK_EQ(source_script_of(fixture.manager, second), 109);
    CHECK_EQ(presentation.spirit_notice_handles().size(), size_t(1));
}

TEST(show_notice_61_to_63_still_replaces_ctx_d0_slot) {
    Fixture fixture;
    CHECK(fixture.build());
    SpellPresentation presentation(fixture.manager, SpellPresentation::Banks{4, 1});
    CHECK(presentation.show_notice(61));
    const u32 first = presentation.notice_handle();
    CHECK(first != 0);
    CHECK_EQ(source_script_of(fixture.manager, first), 61);
    CHECK(presentation.show_notice(63));
    const u32 second = presentation.notice_handle();
    CHECK(second != 0 && second != first);
    CHECK_EQ(source_script_of(fixture.manager, second), 63);
    // 原版 +0xd0 替换：旧句柄经 0x470110 标记，下一次 ANM walk 回收。
    const auto* retiring = fixture.manager.registry.find(first);
    CHECK(retiring && (retiring->visual.render_flags & 0x20u));
    CHECK(fixture.manager.update());
    CHECK(fixture.manager.registry.find(first) == nullptr);
    // 不可达 case3(62) 与其它值不改变槽位。
    CHECK(presentation.show_notice(62));
    CHECK_EQ(presentation.notice_handle(), second);
}

int main() { return th13test::run_all("spirit notices"); }
