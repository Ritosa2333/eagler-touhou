// 标题 Option 屏状态机单测（FUN_0044dd00/0044ed70 移植对合）：
//   开屏 substate0→1（root1 自建 + 音量应用）→ >6 帧进 2 并广播 17+行 →
//   ↑↓ 环绕（音10）→ X 非4 行跳行4（音9）→ 行0/1 ←→ ±5 钳 0..100（无音）
//   + 数字 sprite/前导零可见位（小 50+d / 大 61+d）→ 行3 Z 复位 100/80（音7）
//   → 行4 Z >9 帧离屏（持久化 + 回 Main + root1 label1 清槽）。
// 音量真值在宿主共享 TitleOptionVolumes；持久化失败如实记录 error。
#include "ui_test_fixtures.hpp"
#include <cstdio>
#include "../harness/test_framework.hpp"

#include "game/TitleMainMenu.hpp"
#include "game/TitleOptionsMenu.hpp"

#include <memory>
#include <string>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

std::vector<u8> option_title_fixture() {
    auto scripts = trivial_scripts(130, 72);
    // root1 spawn 23-48（原版 spawn 列表，dump 实读；先清默认 wait/stop）。
    scripts[1].clear();
    for (i32 child = 23; child <= 48; child++) scripts[1].push_back(spawn(child));
    scripts[1].push_back(wait());
    scripts[1].push_back(stop());
    // 行条/数字子脚本按真机 label 空间建模（title.anm 实读）：首指令 op300
    // （bit0 置位），随后 label3（熄灭）与所属行的 label 7+行 / 17+行（选中）；
    // 行条 23-27/28-32 对应行 0..4，BGM 数字 33-40 属行 0，SE 数字 41-48 属行 1。
    // 行切换广播的 label3 命中即跳转并置 bit0（0x46265d）——fixture 不带
    // label 就复现不了「bit0 被广播复活」这一机制。
    for (i32 child = 23; child <= 48; child++) {
        const i32 row = child <= 27 ? child - 23 : child <= 32 ? child - 28 : child <= 40 ? 0 : 1;
        auto& script = scripts[u32(child)];
        script.clear();
        script.push_back(sprite(0));
        script.push_back(wait());
        for (const i32 id : {3, 7 + row, 17 + row}) {
            script.push_back(label(id));
            script.push_back(wait());
        }
        script.push_back(stop());
    }
    return synthetic_anm(72, scripts);
}

struct RecordingHost final : TitleOptionPlatform {
    TitleOptionVolumes persisted;
    i32 applies = 0, last_bgm = -1, last_effect = -1;
    bool persist_ok = false, key_config_ok = false;
    i32 key_config_opens = 0;
    void apply_volumes(i32 bgm, i32 effect) override {
        ++applies;
        last_bgm = bgm;
        last_effect = effect;
    }
    bool persist_options(const TitleOptionVolumes& volumes) override {
        if (!persist_ok) return false;
        persisted = volumes;
        return true;
    }
    bool open_key_config() override {
        ++key_config_opens;
        return key_config_ok;
    }
};

struct OptionHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    TitleState state;
    TitleOptionVolumes volumes;
    RecordingHost host;
    TitleSceneResources resources;
    std::unique_ptr<TitleOptionsMenu> menu;
    std::vector<i32> sounds;

    OptionHarness() {
        CHECK(load_bank(manager, 16, option_title_fixture()));
        resources.title = 16;
        resources.ascii = 2;
        state.screen = TitleScreen(kTitleScreenOption);  // 接线方（TitleFrame）职责，此处模拟
        menu = std::make_unique<TitleOptionsMenu>(state, manager, host, volumes, resources);
        menu->play_sound = [this](i32 id) {
            sounds.push_back(id);
            return true;
        };
    }

    bool frame(u32 pressed = 0, u32 repeated = 0) {
        const bool ok = menu->update(pressed, repeated);
        manager.update(true);
        manager.update(false);
        return ok;
    }
    void advance(i32 frames, u32 pressed = 0, u32 repeated = 0) {
        for (i32 i = 0; i < frames; i++) CHECK(frame(pressed, repeated));
    }
    AnmVm* child(i32 script) {
        auto* root = manager.registry.find(state.handles[1]);
        return root ? manager.registry.find_child_script(*root, script, 0) : nullptr;
    }
};

}  // namespace

TEST(option_open_builds_root_and_applies_volumes) {
    OptionHarness h;
    h.volumes.bgm = 42;
    h.volumes.effect = 7;
    CHECK(h.frame());
    CHECK_EQ(h.state.substate, 1);
    CHECK(h.state.handles[1] != 0);
    CHECK_EQ(h.host.applies, 1);
    CHECK_EQ(h.host.last_bgm, 42);
    CHECK_EQ(h.host.last_effect, 7);
}

TEST(option_digits_sprite_and_leading_zero_visibility) {
    OptionHarness h;
    h.volumes.bgm = 42;
    h.volumes.effect = 100;
    CHECK(h.frame());
    auto* root = h.manager.registry.find(h.state.handles[1]);
    CHECK(root != nullptr);
    // bgm=42：百位隐（小33/大37）；十/个位显。se=100：全显。
    const struct { i32 script; bool visible; } expects[] = {
        {33, false}, {34, true}, {35, true},
        {37, false}, {38, true}, {39, true},
        {41, true}, {42, true}, {43, true},
        {45, true}, {46, true}, {47, true},
    };
    // 补一帧使 root1 spawn 完成，全量核对可见位（sprite 选择失败会进 error）。
    // 0044ed70 尾段经 +0x18 bit1（port flags bit1，0x44f3e0 and ~2 / 0x44f640
    // or 2）隐藏前导零；bit0 归脚本 op300/label 跳转（0x46356a/0x46265d），
    // 0044ed70 不触碰 —— 绘制门 = flags&3==3，故以 (flags&3)==3 断言。
    CHECK(h.frame());
    for (const auto& e : expects) {
        auto* vm = h.child(e.script);
        CHECK(vm != nullptr);
        if (!vm) continue;
        CHECK_EQ((vm->visual.flags & 3u) == 3u, e.visible);
    }
    CHECK(h.menu->error.empty());
}

TEST(option_cursor_move_never_reveals_leading_zero_digits) {
    // 用户报告回归（2026-11-11）：行切换广播 0x46fca0(label3,immediate) +
    // 0x46fc30(7+行) 会把子体 VM+0x18 bit0 置位（0x46265d）；前导零隐藏在
    // bit1 上，bit0 置位不得复活百位 0（此前误用 bit0 隐藏曾闪「060」）。
    OptionHarness h;
    h.volumes.bgm = 42;   // 百位隐
    h.volumes.effect = 7; // 百位隐、十位隐
    CHECK(h.frame());
    h.advance(8);          // 进交互态（>6）
    for (i32 frame_index = 0; frame_index < 12; ++frame_index) {
        if (frame_index == 2) h.frame(0x20);  // ↓ 行切换（BGM → SE）
        else h.frame();
        for (const i32 script : {33, 37, 41, 45}) {   // 四个百位
            auto* vm = h.child(script);
            CHECK(vm != nullptr);
            if (!vm) continue;
            CHECK((vm->visual.flags & 3u) != 3u);
        }
        auto* se_tens = h.child(42);  // se=7：十位也隐藏
        CHECK(se_tens != nullptr);
        if (se_tens) CHECK((se_tens->visual.flags & 3u) != 3u);
    }
    CHECK(h.menu->error.empty());
}

TEST(option_entry_timing_and_cursor_move) {
    OptionHarness h;
    CHECK(h.frame());              // substate0→1
    h.advance(6);
    CHECK_EQ(h.state.substate, 1); // 原版 6 < age：第 7 帧才进 2
    CHECK(h.frame());
    CHECK_EQ(h.state.substate, 2);
    CHECK(h.frame(0x20));          // ↓：行0→1，音10
    CHECK_EQ(h.menu->row(), 1);
    CHECK(!h.sounds.empty() && h.sounds.back() == 10);
    h.advance(1, 0, 0x10);         // ↑：回行0
    CHECK_EQ(h.menu->row(), 0);
    h.advance(1, 0x10);            // ↑ 环绕：行0→4
    CHECK_EQ(h.menu->row(), 4);
}

TEST(option_cancel_jumps_to_quit_row) {
    OptionHarness h;
    CHECK(h.frame());
    h.advance(8);                  // 进 substate2
    CHECK_EQ(h.state.substate, 2);
    CHECK(h.frame(0x102));         // X：行0 → 跳行4 + 音9
    CHECK_EQ(h.menu->row(), 4);
    CHECK(!h.sounds.empty() && h.sounds.back() == 9);
    CHECK_EQ(h.state.substate, 2); // 行≠4 时不离屏
}

TEST(option_volume_adjust_and_clamp) {
    OptionHarness h;
    h.volumes.bgm = 100;
    CHECK(h.frame());
    h.advance(8);
    CHECK(h.frame(0x40));          // 行0 ←：100→95
    CHECK_EQ(h.volumes.bgm, 95);
    CHECK(h.sounds.empty());       // 原版音量步进无音效
    CHECK_EQ(h.host.last_bgm, 95);
    h.volumes.bgm = 3;
    CHECK(h.frame(0x40));          // <5 → 0（原版下溢分支）
    CHECK_EQ(h.volumes.bgm, 0);
    CHECK(h.frame(0x80));          // 0→5
    CHECK_EQ(h.volumes.bgm, 5);
    h.volumes.bgm = 98;
    CHECK(h.frame(0x80));          // >95 → 100
    CHECK_EQ(h.volumes.bgm, 100);
    h.frame(0x20);                 // ↓ 行1
    h.volumes.effect = 80;
    CHECK(h.frame(0x40));          // 行1 ←：80→75
    CHECK_EQ(h.volumes.effect, 75);
    CHECK_EQ(h.host.last_effect, 75);
}

TEST(option_reset_defaults_and_quit_exit) {
    OptionHarness h;
    h.volumes.bgm = 17;
    h.volumes.effect = 33;
    CHECK(h.frame());
    h.advance(8);
    h.frame(0x20); h.frame(0x20);  // 行0→2
    h.frame(0x20);                 // →3
    CHECK(h.frame(0x80001));       // Z 行3：复位 100/80 + 音7
    CHECK_EQ(h.volumes.bgm, 100);
    CHECK_EQ(h.volumes.effect, 80);
    CHECK(!h.sounds.empty() && h.sounds.back() == 7);
    h.frame(0x20);                 // →行4
    CHECK(h.frame(0x80001));       // Z 行4：音9 + substate4
    CHECK_EQ(h.state.substate, 4);
    CHECK_EQ(int(h.state.screen), kTitleScreenOption);  // >9 帧前不切屏
    h.advance(9);
    CHECK_EQ(h.state.substate, 4);
    CHECK(h.frame());              // 第 10 帧离屏
    CHECK_EQ(int(h.state.screen), int(TitleScreen::Main));
    CHECK_EQ(h.state.handles[1], 0);
    CHECK(!h.host.persist_ok);     // 默认宿主持久化失败如实记录
    CHECK(!h.menu->error.empty());
}

TEST(option_persist_success_path_and_key_config_gate) {
    OptionHarness h;
    h.host.persist_ok = true;
    CHECK(h.frame());
    h.advance(8);
    h.frame(0x102);                // X → 行4
    CHECK(h.frame(0x80001));       // Z 行4 → substate4
    h.advance(10);
    CHECK_EQ(h.host.persisted.bgm, 100);
    CHECK_EQ(h.host.persisted.effect, 80);
    CHECK(h.menu->error.empty());
    // 重进 → 行2 键配置：宿主未接线时真实失败，不假成功。
    h.state.screen = TitleScreen(kTitleScreenOption);
    h.state.substate = 0;
    CHECK(h.frame());
    h.advance(8);
    h.frame(0x20); h.frame(0x20);  // →行2
    CHECK_EQ(h.menu->row(), 2);
    CHECK(h.frame(0x80001));       // 音7 → substate4
    CHECK_EQ(h.state.substate, 4);
    h.advance(9);
    CHECK(!h.frame());             // >9 帧：open_key_config 默认 false → fail
    CHECK(!h.menu->error.empty());
    h.host.key_config_ok = true;
}

int main() {
    std::printf("# title_option_menu_test\n");
    return th13test::run_all("title_option_menu_test");
}
