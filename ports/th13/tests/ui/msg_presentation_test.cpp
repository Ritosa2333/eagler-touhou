// MsgPresentation 单测（r3 模块 D）：stage 表行绑定、bank→合并表全局脚本号、
// 自机/右立绘/名牌 VM 创建与句柄释放、表情中断、幂等与显式失败路径。
// 证据：0x428ae0 jump table 0x429c7c 逐 handler + stage 表 0x4bb4f0 EXE 实读。
#include "../harness/test_framework.hpp"

#include "game/MsgPresentation.hpp"
#include "ui_test_fixtures.hpp"

#include <algorithm>
#include <cstring>
#include <string>
#include <vector>

using namespace th13;
using th13::test::AnmIns;
using th13::test::label;
using th13::test::sprite;
using th13::test::stop;
using th13::test::synthetic_anm;
using th13::test::wait;

namespace {

// 立绘可中断脚本：sprite(tag) 身份标记 + label 2/3（说话方亮暗）+
// label 17（表情）+ 停止。身份断言经 vm->visual.sprite（创建即首 tick，
// anm_ui_test interrupt 用例同模式；AnmVm::script 为私有不可查）。
std::vector<AnmIns> portrait_script(i32 tag) {
    return {sprite(tag), label(2), label(3), label(17), wait(), stop()};
}

struct Fixture {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    MsgPresentation::Banks banks{6, 4};  // player / stage 槽位（测试自定）
    std::vector<u8> player_bytes, enemy_bytes, st01enm_bytes, third_bytes;
    AnmResource stage_resource;

    Fixture() {
        // 玩家 bank：35 个脚本（0x4c0aa0 表最大 script 34），sprite 标记 = 脚本号。
        std::vector<std::vector<AnmIns>> player_scripts;
        for (i32 i = 0; i < 35; i++) player_scripts.push_back(portrait_script(i));
        player_bytes = synthetic_anm(35, player_scripts);
        CHECK(manager.load(banks.player, player_bytes.data(), u32(player_bytes.size())));
        // 舞台 bank 两个合并文件：名单序 0 = "enemy.anm"（2 脚本）、
        // 名单序 1 = "st01enm.anm"（20 脚本）→ 后者全局基号 = 2；标记 = 文件内号。
        std::vector<std::vector<AnmIns>> enemy_scripts;
        for (i32 i = 0; i < 2; i++) enemy_scripts.push_back(portrait_script(i));
        enemy_bytes = synthetic_anm(2, enemy_scripts);
        std::vector<std::vector<AnmIns>> st01_scripts;
        for (i32 i = 0; i < 20; i++) st01_scripts.push_back(portrait_script(i));
        st01enm_bytes = synthetic_anm(20, st01_scripts);
        CHECK(stage_resource.append(enemy_bytes.data(), u32(enemy_bytes.size())));
        CHECK(stage_resource.append(st01enm_bytes.data(), u32(st01enm_bytes.size())));
        CHECK(manager.adopt(banks.stage, std::move(stage_resource)));
    }

    bool read(const std::string& name, std::vector<u8>& bytes) {
        if (name == "enemy.anm") { bytes = enemy_bytes; return true; }
        if (name == "st01enm.anm") { bytes = st01enm_bytes; return true; }
        if (name == "st03boss.anm") { bytes = third_bytes; return true; }
        return false;
    }
};

// 活动根 VM 恰一个时回查其句柄；多余/为零返回 0。
u32 single_active_handle(AnmManager& manager) {
    u32 found = 0;
    int hits = 0;
    for (u32 i = 0; i < manager.registry.ordered_count(false); i++) {
        const u32 handle = manager.registry.ordered_handle(false, i);
        if (handle && manager.registry.find(handle)) {
            found = handle;
            hits++;
        }
    }
    return hits == 1 ? found : 0;
}

// 脚本身份：VM 首指令 sprite(tag) 创建即执行 → visual.sprite == 目标文件内
// 脚本号（tag 与脚本号一一对应，见 Fixture）。
i32 active_sprite_tag(AnmManager& manager, u32 handle) {
    auto* vm = manager.registry.find(handle);
    return vm ? vm->visual.sprite : -1;
}

}  // namespace

TEST(msg_bind_stage_scripts_computes_global_base) {
    Fixture f;
    MsgPresentation presentation(f.manager, f.banks);
    CHECK(presentation.bind_stage_scripts(
        1, {"enemy.anm", "st01enm.anm"},
        [&](const std::string& name, std::vector<u8>& bytes) { return f.read(name, bytes); }));
}

TEST(msg_player_portrait_script_identity_and_idempotent) {
    Fixture f;
    MsgPresentation presentation(f.manager, f.banks);
    CHECK(presentation.bind_stage_scripts(
        1, {"enemy.anm", "st01enm.anm"},
        [&](const std::string& name, std::vector<u8>& bytes) { return f.read(name, bytes); }));
    CHECK(presentation.player_portrait(0));  // Reimu → 0x4c0aa0[0] = script 29
    // 脚本身份：唯一活动根 VM 的 sprite 标记 = 29（0x429298 表实读）。
    const u32 handle = single_active_handle(f.manager);
    CHECK(handle != 0);
    CHECK_EQ(active_sprite_tag(f.manager, handle), 29);
    // 幂等：已建不再重建（原版 +0x40 非零短路）——活动根仍恰一个。
    CHECK(presentation.player_portrait(0));
    CHECK(single_active_handle(f.manager) == handle);
}

TEST(msg_side_portrait_and_nameplate_global_scripts) {
    Fixture f;
    MsgPresentation presentation(f.manager, f.banks);
    CHECK(presentation.bind_stage_scripts(
        1, {"enemy.anm", "st01enm.anm"},
        [&](const std::string& name, std::vector<u8>& bytes) { return f.read(name, bytes); }));
    CHECK_EQ(f.manager.resource(f.banks.stage)->scripts.size(), 22u);  // 2 + 20 合并
    // opcode 2，entry 0（<2 或 stage1 无 Boss 对）→ st01enm script10 → 全局 2+10。
    CHECK(presentation.side_portrait(0));
    const u32 side = single_active_handle(f.manager);
    CHECK(side != 0);
    CHECK_EQ(active_sprite_tag(f.manager, side), 10);  // st01enm 文件内 script10
    // opcode 20 名牌 → st01enm script15 → 全局 2+15。
    CHECK(presentation.nameplate());
    // 两 VM 并存：sprite 标记集 {10, 15}。
    std::vector<i32> tags;
    for (u32 i = 0; i < f.manager.registry.ordered_count(false); i++) {
        const u32 handle = f.manager.registry.ordered_handle(false, i);
        if (handle) tags.push_back(active_sprite_tag(f.manager, handle));
    }
    std::sort(tags.begin(), tags.end());
    CHECK_EQ(tags.size(), 2u);
    CHECK_EQ(tags[0], 10);
    CHECK_EQ(tags[1], 15);
    // entry 2 且 stage1 Boss 对缺席（0x4bb4f0 行值 -1）→ 仍取中Boss 对（同 0x429335）。
    presentation.release_all();
    // retire 只置 0x20 待回收位（0x46fc30/0x46f170 同构）——推进一帧让
    // 管理器回收旧 VM，再重建并核对身份。
    CHECK(f.manager.update());
    CHECK(presentation.side_portrait(2));
    CHECK_EQ(active_sprite_tag(f.manager, single_active_handle(f.manager)), 10);
}

TEST(msg_stage1_boss_side_portrait_absent_row_fails_explicitly) {
    Fixture f;
    MsgPresentation presentation(f.manager, f.banks);
    CHECK(presentation.bind_stage_scripts(
        1, {"enemy.anm", "st01enm.anm"},
        [&](const std::string& name, std::vector<u8>& bytes) { return f.read(name, bytes); }));
    // opcode 31（0x42948c）在 stage1 Boss 对 = -1 行：原版表无此创建路径——
    // port 显式失败，不静默取中Boss 对。
    CHECK(!presentation.boss_side_portrait());
    CHECK(!presentation.error.empty());
}

TEST(msg_expression_interrupt_and_release) {
    Fixture f;
    MsgPresentation presentation(f.manager, f.banks);
    CHECK(presentation.bind_stage_scripts(
        1, {"enemy.anm", "st01enm.anm"},
        [&](const std::string& name, std::vector<u8>& bytes) { return f.read(name, bytes); }));
    CHECK(presentation.player_portrait(1));   // Marisa → script 34
    CHECK(presentation.side_portrait(0));
    // 13/14 表情（label = arg+17）与 7/8 亮暗（label 2/3）都走立即中断。
    CHECK(presentation.interrupt_player(17));
    CHECK(presentation.interrupt_side(20));
    CHECK(presentation.interrupt_player(2));
    CHECK(presentation.interrupt_side(3));
    // opcode 4/5/6 释放；重复释放与空槽中断均空转不失败。
    presentation.release_player();
    presentation.release_side();
    presentation.release_nameplates();
    presentation.release_all();
    CHECK(presentation.interrupt_player(2));
    CHECK(presentation.interrupt_side(2));
}

TEST(msg_missing_anim_file_fails_explicitly) {
    Fixture f;
    MsgPresentation presentation(f.manager, f.banks);
    CHECK(!presentation.bind_stage_scripts(
        1, {"enemy.anm", "missing.anm"},
        [&](const std::string& name, std::vector<u8>& bytes) { return f.read(name, bytes); }));
    CHECK(!presentation.error.empty());
    // 未绑定名单时创建立绘：显式失败（bank 越名单界）。
    Fixture f2;
    MsgPresentation unbound(f2.manager, f2.banks);
    CHECK(!unbound.side_portrait(0));
    CHECK(!unbound.error.empty());
}

int main() { return th13test::run_all("MsgPresentation 对话立绘"); }
