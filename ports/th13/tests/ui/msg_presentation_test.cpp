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

std::vector<AnmIns> portrait_script(i32 tag) { return {sprite(tag), label(2), label(3), label(17), wait(), stop()}; }

struct Fixture {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    MsgPresentation::Banks banks{6, 4};
    std::vector<u8> player_bytes, enemy_bytes, st01enm_bytes, third_bytes;
    AnmResource stage_resource;

    Fixture() {
        std::vector<std::vector<AnmIns>> player_scripts;
        for (i32 i = 0; i < 35; i++)
            player_scripts.push_back(portrait_script(i));
        player_bytes = synthetic_anm(35, player_scripts);
        CHECK(manager.load(banks.player, player_bytes.data(), u32(player_bytes.size())));
        std::vector<std::vector<AnmIns>> enemy_scripts;
        for (i32 i = 0; i < 2; i++)
            enemy_scripts.push_back(portrait_script(i));
        enemy_bytes = synthetic_anm(2, enemy_scripts);
        std::vector<std::vector<AnmIns>> st01_scripts;
        for (i32 i = 0; i < 20; i++)
            st01_scripts.push_back(portrait_script(i));
        st01enm_bytes = synthetic_anm(20, st01_scripts);
        CHECK(stage_resource.append(enemy_bytes.data(), u32(enemy_bytes.size())));
        CHECK(stage_resource.append(st01enm_bytes.data(), u32(st01enm_bytes.size())));
        CHECK(manager.adopt(banks.stage, std::move(stage_resource)));
    }

    bool read(const std::string& name, std::vector<u8>& bytes) {
        if (name == "enemy.anm") {
            bytes = enemy_bytes;
            return true;
        }
        if (name == "st01enm.anm") {
            bytes = st01enm_bytes;
            return true;
        }
        if (name == "st03boss.anm") {
            bytes = third_bytes;
            return true;
        }
        return false;
    }
};

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

i32 active_sprite_tag(AnmManager& manager, u32 handle) {
    auto* vm = manager.registry.find(handle);
    return vm ? vm->visual.sprite : -1;
}

}

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
    CHECK(presentation.player_portrait(0));
    const u32 handle = single_active_handle(f.manager);
    CHECK(handle != 0);
    CHECK_EQ(active_sprite_tag(f.manager, handle), 29);
    CHECK(presentation.player_portrait(0));
    CHECK(single_active_handle(f.manager) == handle);
}

TEST(msg_side_portrait_and_nameplate_global_scripts) {
    Fixture f;
    MsgPresentation presentation(f.manager, f.banks);
    CHECK(presentation.bind_stage_scripts(
        1, {"enemy.anm", "st01enm.anm"},
        [&](const std::string& name, std::vector<u8>& bytes) { return f.read(name, bytes); }));
    CHECK_EQ(f.manager.resource(f.banks.stage)->scripts.size(), 22u);
    CHECK(presentation.side_portrait(0));
    const u32 side = single_active_handle(f.manager);
    CHECK(side != 0);
    CHECK_EQ(active_sprite_tag(f.manager, side), 10);
    CHECK(presentation.nameplate(0));
    std::vector<i32> tags;
    for (u32 i = 0; i < f.manager.registry.ordered_count(false); i++) {
        const u32 handle = f.manager.registry.ordered_handle(false, i);
        if (handle) tags.push_back(active_sprite_tag(f.manager, handle));
    }
    std::sort(tags.begin(), tags.end());
    CHECK_EQ(tags.size(), 2u);
    CHECK_EQ(tags[0], 10);
    CHECK_EQ(tags[1], 15);
    presentation.release_all();
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
    CHECK(!presentation.boss_side_portrait());
    CHECK(!presentation.error.empty());
}

TEST(msg_expression_interrupt_and_release) {
    Fixture f;
    MsgPresentation presentation(f.manager, f.banks);
    CHECK(presentation.bind_stage_scripts(
        1, {"enemy.anm", "st01enm.anm"},
        [&](const std::string& name, std::vector<u8>& bytes) { return f.read(name, bytes); }));
    CHECK(presentation.player_portrait(1));
    CHECK(presentation.side_portrait(0));
    CHECK(presentation.interrupt_player(17));
    CHECK(presentation.interrupt_side(20));
    CHECK(presentation.interrupt_player(2));
    CHECK(presentation.interrupt_side(3));
    presentation.release_player();
    presentation.release_side();
    presentation.release_nameplates();
    presentation.release_all();
    CHECK(presentation.interrupt_player(2));
    CHECK(presentation.interrupt_side(2));
}

TEST(msg_opcode_23_24_deferred_label7_keeps_handles) {
    Fixture f;
    MsgPresentation presentation(f.manager, f.banks);
    CHECK(presentation.bind_stage_scripts(
        1, {"enemy.anm", "st01enm.anm"},
        [&](const std::string& name, std::vector<u8>& bytes) { return f.read(name, bytes); }));
    CHECK(presentation.player_portrait(0));
    CHECK(presentation.side_portrait(0));
    std::vector<u32> handles;
    for (u32 i = 0; i < f.manager.registry.ordered_count(false); ++i) {
        const u32 handle = f.manager.registry.ordered_handle(false, i);
        if (handle) handles.push_back(handle);
    }
    CHECK_EQ(handles.size(), 2u);
    CHECK(presentation.fade_side(7));
    CHECK(presentation.fade_player(7));
    for (u32 handle : handles)
        CHECK(f.manager.registry.find(handle) != nullptr);
    CHECK(f.manager.update());
    for (u32 handle : handles)
        CHECK(f.manager.registry.find(handle) != nullptr);
    CHECK(presentation.interrupt_side(20));
    CHECK(presentation.interrupt_player(20));
    CHECK(presentation.error.empty());
    presentation.release_side();
    CHECK(presentation.interrupt_side(2));
}

TEST(msg_missing_anim_file_fails_explicitly) {
    Fixture f;
    MsgPresentation presentation(f.manager, f.banks);
    CHECK(!presentation.bind_stage_scripts(
        1, {"enemy.anm", "missing.anm"},
        [&](const std::string& name, std::vector<u8>& bytes) { return f.read(name, bytes); }));
    CHECK(!presentation.error.empty());
    Fixture f2;
    MsgPresentation unbound(f2.manager, f2.banks);
    CHECK(!unbound.side_portrait(0));
    CHECK(!unbound.error.empty());
}

int main() { return th13test::run_all("MsgPresentation 对话立绘"); }
