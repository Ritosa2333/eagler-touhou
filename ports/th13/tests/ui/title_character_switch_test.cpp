// 角色左右切换残留回归（P0，原版 0x451450 逐指令）：
//   左（0x4519b7）：sound10 → 0x46fca0(root, 0x16+old)【immediate 退场】→
//                  0x473210(-1) → 0x46fc30(root, 0x0c+new)【pending 入场】
//   右（0x451a10）：0x46fca0(root, 0x11+old)【immediate】→ 0x473210(+1) →
//                  0x46fc30(root, 0x07+new)【pending】
// 真实 title.anm 证据（artifacts/reference/dumps/anm-list.txt title 段）：
//   s129 spawn 125-128（立绘）/135-138（武器）/131-134（选择框）；
//   s125/135/131 标签族 {7+c,12+c,17+c,22+c,3,6,1}：7=自 x272 入、12=自 x16 入、
//   17=alpha→0 出向 x16（层22）、22=alpha→0 出向 x272（层22）。
// 旧实现退/入均 pending 且同帧发出：入场覆盖旧立绘的退场 pending → 残留。
// 断言口径：不查 cursor 数字，逐帧记录活立绘 VM 的 source_script/句柄/
// visible/alpha/层桶提交；过渡帧允许交叠（原版时长），稳定帧旧树必须退场。
// 负控：直接按旧机制（根广播双 pending）复刻切换，同一检测器必须检出残留。
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"

#include "game/AnmManager.hpp"
#include "game/AnmEnvironment.hpp"
#include "game/FrameScheduler.hpp"
#include "game/Rng.hpp"
#include "game/SessionState.hpp"
#include "game/TitleSelectionMenu.hpp"

#include <cstdio>
#include <memory>
#include <string>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

struct SwitchHost final : TitleMenuServices {
    bool sound(i32) override { return true; }
    bool begin_transition(u32&) override { return true; }
    bool start_game(i32) override { return true; }
    // 零进度世界：clear-tag 全隐藏（原版无存档行为）。
    bool character_cleared(i32, i32) override { return false; }
};

// 仓库根定位（tests/ui/run.sh 已 cd 仓库根；再向上兜底容器/手跑场景）。
bool read_repo_file(const std::string& relative, std::vector<u8>& out) {
    for (const std::string& base : {".", "../../../..", "../../../../.."}) {
        FILE* file = std::fopen((base + "/" + relative).c_str(), "rb");
        if (!file) continue;
        std::fseek(file, 0, SEEK_END);
        const long size = std::ftell(file);
        std::fseek(file, 0, SEEK_SET);
        out.resize(size_t(size < 0 ? 0 : size));
        const size_t got = out.empty() ? 1 : std::fread(out.data(), 1, out.size(), file);
        std::fclose(file);
        if (got == out.size()) return true;
        return false;
    }
    return false;
}

// 单帧立绘树观测（实际层桶 = registry.layer，绘制消费同一桶）。
struct PortraitFrame {
    i32 script = -1;
    u32 handle = 0;
    bool visible = false;
    u32 alpha = 0;
    i32 layer = -1;
    bool in_bucket = false;
    i32 pending = 0;
    float x = 0.0f;
};

struct SwitchHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SessionState progress;
    TitleSelectionSettings choice;
    FrameScheduler scheduler;
    SwitchHost host;
    std::unique_ptr<TitleCharacterMenu> menu;
    TitleState state;
    std::array<FrameCallback, 2> updates{};  // 成员存活：scheduler 持侵入式指针

    SwitchHarness() {
        updates[0].owner = updates[1].owner = &manager;
        updates[0].enabled = updates[1].enabled = true;
        updates[0].run = [](void* p) {
            return static_cast<AnmManager*>(p)->update(true) ? 1 : 5;
        };
        updates[1].run = [](void* p) {
            return static_cast<AnmManager*>(p)->update(false) ? 1 : 5;
        };
        CHECK(scheduler.add(updates[0], FramePass::Update, 9) >= 0);
        CHECK(scheduler.add(updates[1], FramePass::Update, 34) >= 0);
        std::vector<u8> title;
        CHECK(read_repo_file("artifacts/reference/raw/title.anm", title));
        CHECK(manager.load(16, title.data(), u32(title.size())));
        menu = std::make_unique<TitleCharacterMenu>(state, progress, choice, manager, host,
                                                    TitleSceneResources{});
    }

    // 直接入角色屏（生产 Difficulty→Character 由 title_state/clear 套件覆盖；
    // 本套件隔离角色菜单模块本身，避开难度屏 83/84 真脚本独立问题）。
    void enter_character_screen(i32 character) {
        state.return_reason = 0;
        state.change_screen(TitleScreen::Character);
        state.menu.push();
        state.menu.wrapping = true;
        state.menu.count = kTitleCharacterRows;
        state.menu.select(kTitleCharacterFirst + character);
        progress.difficulty = 2;
        progress.character = character;
    }

    void frame(u32 pressed = 0, u32 repeated = 0) {
        CHECK(menu->update(pressed, repeated));
        state.age.tick(&progress.rate);
        CHECK(scheduler.update() >= 0);
        CHECK(scheduler.draw() >= 0);
    }

    void advance(int frames, u32 pressed = 0, u32 repeated = 0) {
        for (int i = 0; i < frames; i++) frame(pressed, repeated);
    }

    AnmVm* portrait(i32 character) {
        AnmVm* root = manager.registry.find(state.handles[129]);
        return root ? manager.registry.find_child_script(*root, 125 + character, 0) : nullptr;
    }

    AnmVm* clear_tag(i32 character) {
        AnmVm* root = manager.registry.find(state.handles[129]);
        return root ? manager.registry.find_child_script(*root, 131 + character, 0) : nullptr;
    }

    static bool drawn(const AnmVm* vm) {
        return vm && vm->visible && (vm->visual.color >> 24) != 0;
    }

    PortraitFrame observe(i32 character) {
        PortraitFrame row;
        AnmVm* vm = portrait(character);
        if (!vm) return row;
        row.script = vm->source_script;
        row.handle = manager.registry.handle(*vm);
        row.visible = vm->visible;
        row.alpha = vm->visual.color >> 24;
        row.layer = vm->visual.layer;
        row.pending = vm->pending_interrupt;
        row.x = vm->visual.translation.x;
        const auto& bucket = manager.registry.layer(u32(vm->visual.layer));
        for (const AnmVm* item : bucket)
            if (item == vm) row.in_bucket = true;
        return row;
    }

    i32 drawn_portraits() {
        i32 count = 0;
        for (i32 c = 0; c < 4; c++)
            if (drawn(portrait(c))) count++;
        return count;
    }
};

constexpr int kExitFrames = 12;  // 退场 ins_409(10,4,0)：10 帧淡出 + 余量

}  // namespace

// 单次右切：退场当帧起播（层 22、alpha 开始下降），稳定帧旧立绘归零、
// 新立绘入场完成，句柄不变（原版不重建 VM 树）。
TEST(character_switch_exits_old_portrait_immediately) {
    SwitchHarness h;
    h.enter_character_screen(0);
    h.advance(25);
    CHECK_EQ(h.drawn_portraits(), 1);
    const u32 old_handle = h.observe(0).handle;

    std::vector<PortraitFrame> old_log, new_log;
    h.frame(0x80);  // →：0x451a10 路径
    old_log.push_back(h.observe(0));
    new_log.push_back(h.observe(1));
    for (int i = 0; i < 24; i++) {
        h.frame();
            old_log.push_back(h.observe(0));
        new_log.push_back(h.observe(1));
    }
    // 退场 immediate：按键帧已跳 label17（层 22）。
    CHECK_EQ(old_log[0].layer, 22);
    CHECK_EQ(old_log[0].script, 125);
    CHECK(old_log[0].in_bucket);  // 层桶提交与 VM 层一致
    // 淡出进行（允许过渡帧与新立绘交叠——原版时长照旧）。
    CHECK(old_log[2].alpha < 255);
    CHECK(new_log[2].alpha > 0);
    // 稳定帧：旧树退场完成，仅新立绘在绘。
    CHECK_EQ(old_log[kExitFrames].alpha, 0u);
    CHECK_EQ(old_log[24].alpha, 0u);
    CHECK_EQ(new_log[24].alpha, 255u);
    CHECK_EQ(new_log[24].layer, 23);
    CHECK(!SwitchHarness::drawn(h.portrait(0)));
    CHECK(SwitchHarness::drawn(h.portrait(1)));
    CHECK_EQ(h.drawn_portraits(), 1);
    // VM 生命周期：句柄不变（不 retire/不重建），pending 已清。
    CHECK_EQ(old_log[24].handle, old_handle);
    CHECK_EQ(old_log[24].pending, 0);
    CHECK_EQ(new_log[24].pending, 0);
}

// 单次左切对称：退 22+old（层 22、出向 x272），入 12+new。
TEST(character_switch_left_exits_to_right) {
    SwitchHarness h;
    h.enter_character_screen(1);
    h.advance(25);
    CHECK_EQ(h.drawn_portraits(), 1);
    std::vector<PortraitFrame> old_log;
    h.frame(0x40);  // ←：0x4519b7 路径
    old_log.push_back(h.observe(1));
    for (int i = 0; i < 24; i++) {
        h.frame();
        old_log.push_back(h.observe(1));
    }
    CHECK_EQ(old_log[0].layer, 22);
    CHECK(old_log[2].alpha < 255);
    CHECK_EQ(old_log[kExitFrames].alpha, 0u);
    CHECK_EQ(old_log[24].alpha, 0u);
    CHECK(SwitchHarness::drawn(h.portrait(0)));  // 行 2 ← 行 1：霊夢（12+0 入场）
    CHECK_EQ(h.drawn_portraits(), 1);
}

// 序列 0→1→2→3→0（→,→,→ 后 ←,←,← 回）+ 快速左右反复：每站稳定帧恰好
// 一个立绘在绘；快速段允许交叠但收尾唯一。
TEST(character_switch_sequence_and_fast_alternation) {
    SwitchHarness h;
    h.enter_character_screen(0);
    h.advance(25);
    const i32 expected_after_press_right[3] = {1, 2, 3};
    for (i32 step = 0; step < 3; step++) {
        h.frame(0x80);
        h.advance(25);
        CHECK_EQ(h.state.menu.cursor, kTitleCharacterFirst + expected_after_press_right[step]);
        CHECK_EQ(h.drawn_portraits(), 1);
        CHECK(SwitchHarness::drawn(h.portrait(expected_after_press_right[step])));
    }
    for (i32 back = 2; back >= 0; back--) {
        h.frame(0x40);
        h.advance(25);
        CHECK_EQ(h.drawn_portraits(), 1);
        CHECK(SwitchHarness::drawn(h.portrait(back)));
    }
    CHECK_EQ(h.state.menu.cursor, kTitleCharacterFirst);
    // 快速左右反复（逐帧间隔，动画未完成即再切）。
    const u32 fast[4] = {0x80, 0x40, 0x80, 0x40};
    for (u32 press : fast) {
        h.frame(press);
        h.frame();  // 1 帧间隔即反向
    }
    for (i32 c = 0; c < 4; c++) {
        PortraitFrame row = h.observe(c);
        CHECK_EQ(row.pending, 0);  // 全部标签已消费，无悬留退场指令
    }
    h.advance(25);
    CHECK_EQ(h.drawn_portraits(), 1);
    CHECK(SwitchHarness::drawn(h.portrait(0)));
    CHECK(h.menu->error.empty());
}

// 行边界：霊夢行 ← 到 0 行（EXTRA 字条位）只有退场无入场；→ 回霊夢行
// 重新入场（7+0）。
TEST(character_switch_row_boundary_exit_without_enter) {
    SwitchHarness h;
    h.enter_character_screen(0);
    h.advance(25);
    CHECK_EQ(h.drawn_portraits(), 1);
    h.frame(0x40);
    h.advance(25);
    CHECK_EQ(h.state.menu.cursor, 0);
    CHECK_EQ(h.drawn_portraits(), 0);  // 退场完成且无新立绘
    CHECK_EQ(h.observe(0).alpha, 0u);
    h.frame(0x80);
    h.advance(25);
    CHECK_EQ(h.state.menu.cursor, kTitleCharacterFirst);
    CHECK_EQ(h.drawn_portraits(), 1);
    CHECK(SwitchHarness::drawn(h.portrait(0)));
}

// 未通关 clear-tag（131-134）跨全部切换保持隐藏：定向发送不触碰被抑制子
// （原版由 vm+0x18 bit1 保护；port 无该位，靠不广播实现同可见语义）。
TEST(character_switch_keeps_uncleared_tags_hidden) {
    SwitchHarness h;
    h.enter_character_screen(0);
    h.advance(25);
    for (i32 step = 0; step < 3; step++) {
        h.frame(0x80);
        h.advance(25);
        for (i32 c = 0; c < 4; c++) {
            AnmVm* tag = h.clear_tag(c);
            CHECK(tag != nullptr);
            CHECK(!SwitchHarness::drawn(tag));
            CHECK_EQ(tag->pending_interrupt, 0);
        }
    }
    CHECK_EQ(h.drawn_portraits(), 1);
}

// 负控：复刻旧机制（根广播、退/入均 pending、同帧连发）——同一「稳定帧
// 多立绘在绘」检测器必须检出残留。若产品侧回退为 pending 广播，上方全部
// 正控将以同一断言失败。
TEST(character_switch_negative_pending_broadcast_leaves_residue) {
    SwitchHarness h;
    h.enter_character_screen(0);
    h.advance(25);
    CHECK_EQ(h.drawn_portraits(), 1);
    // 旧 TitleCharacterMenu 行为逐调用复刻（0x46fc30 ×2，入场覆盖退场）。
    CHECK(h.manager.interrupt(h.state.handles[129], 17 + 0, false));
    CHECK(h.manager.interrupt(h.state.handles[129], 7 + 1, false));
    h.advance(25);
    CHECK(SwitchHarness::drawn(h.portrait(0)));  // 旧立绘残留（检出即负控成立）
    CHECK(SwitchHarness::drawn(h.portrait(1)));
    CHECK_EQ(h.drawn_portraits(), 2);  // 生产路径必须为 1
}

int main() { return th13test::run_all("角色切换残留"); }
