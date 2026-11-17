// ED staff roll 名单 VM 实证（用户 BUG：1CC 通关名单背景显示但无滚动制作者
// 字样）。真实 staff.anm + cue_ending_anm 同式创建（Application.cpp
// animations.create(slot, script, -1, 0)），逐帧 tick 后断言：
//   (a) script5 根 spawn 子 script15（层25 装饰）与 script19（层26 名单条带）
//   (b) ins_410 旋入推进（rotation.y 从 -π/2 走向 0）
//   (c) draw_layer(25/26) 提交 WorldUv 图元（mode8 世界四边形）
// 资产：artifacts/reference/raw/staff.anm（ending/staff.png 名单位图条带）。
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"
#include "game/AnmManager.hpp"
#include "game/AnmRenderer.hpp"
#include "game/EndingScene.hpp"

#include <cmath>
#include <fstream>
#include <map>

using namespace th13;
using namespace touhou::graphics;

namespace {

// —— 场景层：真实 staff.msg 逐行驱动（op8@200..3540 是否按时 cue）——
struct ScenePlatform final : EndingPlatform {
    std::map<std::string, std::vector<u8>> assets;
    std::vector<std::pair<i32, i32>> cues;  // (bank_slot, script)
    bool read_asset(const std::string& name, std::vector<u8>& bytes, bool = true) override {
        const auto it = assets.find(name);
        if (it == assets.end()) return false;
        bytes = it->second;
        return true;
    }
    struct Bytes final : EndingSaveBytes {
        u8 memory[64]{};
        u8 read(i32 i) const override { return memory[u32(i)]; }
        void write(i32 i, u8 v) override { memory[u32(i)] = v; }
    } bytes;
    EndingSaveBytes* save_bytes() override { return &bytes; }
    bool request_destination(i32) override { return true; }
    bool queue_music(i32, i32, const std::string&) override { return true; }
    bool fade_out_music(i32) override { return true; }
    bool advance_sound() override { return true; }
    bool load_ending_anm(i32, const std::string&) override { return true; }
    bool cue_ending_anm(i32, i32 bank_slot, i32 script) override {
        cues.emplace_back(bank_slot, script);
        return true;
    }
    void release_ending_anm(i32) override {}
    bool ending_page_line(i32, u32, const std::string&) override { return true; }
};

std::vector<u8> read_asset(const char* name) {
    std::ifstream file(std::string("artifacts/reference/raw/") + name, std::ios::binary);
    return {std::istreambuf_iterator<char>(file), {}};
}

struct Graphics final : ZunGraphics {
    // 与 anm_shape_geometry_test 同款无头桩；额外记录 WorldUv 图元次数。
    PipelineState state;
    u32 world_uv_primitives = 0;
    u32 world_uv_frames = 0;
    std::vector<Vec3> world_positions;
    PipelineState& pipeline() override { return state; }
    u32 texture(const AnmResource&, u32) override { return 1; }
    void bind_texture(u32) override {}
    void set_layout(VertexLayout) override {}
    void set_matrix(MatrixKind, const Matrix4&) override {}
    void triangles(u32, const void*, u32) override {}
    void primitives(Topology, u32, const void*, u32 stride) override {
        if (stride == 20) {  // VertexLayout::WorldUv（world_quad 顶点）
            ++world_uv_primitives;
            world_uv_frames = std::max(world_uv_frames, 1u);
        }
    }
    bool select_target(const AnmResource*, u32) override { return true; }
    bool clear_target(u32, const GraphicsViewport*) override { return true; }
    bool clear_depth(const GraphicsViewport*) override { return true; }
    void set_viewport(const GraphicsViewport&) override {}
};

}  // namespace

TEST(ending_staff_script5_spins_credit_strip_and_draws) {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager(random, environment);
    const auto bytes = read_asset("staff.anm");
    CHECK(!bytes.empty());
    CHECK(manager.load(20, bytes.data(), u32(bytes.size())));
    // cue_ending_anm 同式：bank 20（0x14），script5（staff.msg@0x50 vm1）。
    const u32 handle = manager.create(20, 5, -1, 0);
    CHECK(handle != 0);
    float rotation_y_at_30 = 0.f, rotation_y_at_90 = 0.f;
    bool saw_child_15 = false, saw_child_19 = false;
    Graphics graphics;
    AnmRenderer renderer{graphics};
    for (i32 frame = 0; frame < 120; frame++) {
        CHECK(manager.update());
        if (frame == 30 || frame == 90) {
            for (u32 i = 0; i < manager.registry.ordered_count(false); ++i) {
                const auto* vm = manager.registry.find(manager.registry.ordered_handle(false, i));
                if (vm && vm->source_script == 19)
                    rotation_y_at_30 = frame == 30 ? vm->variables.rotation.y : rotation_y_at_30,
                    rotation_y_at_90 = frame == 90 ? vm->variables.rotation.y : rotation_y_at_90;
            }
        }
        for (const auto* vm : manager.registry.layer(25))
            if (vm && vm->source_script == 15) saw_child_15 = true;
        for (const auto* vm : manager.registry.layer(26))
            if (vm && vm->source_script == 19) saw_child_19 = true;
        // 层 25/26 绘制（与 AnmDrawSchedule {25,68}/{26,69} 同式直调）。
        CHECK(renderer.draw_layer(manager.registry.layer(25)));
        CHECK(renderer.draw_layer(manager.registry.layer(26)));
    }
    CHECK(saw_child_15);  // 装饰位图（st01.png sprite4）
    CHECK(saw_child_19);  // 名单条带（staff.png sprite5）
    // ins_401(0,-π/2,0) + ins_410(60,21,0,0,0)：60 帧旋入，30 帧时应在中途。
    CHECK_NEAR(rotation_y_at_30, -1.5707964f / 2, 0.35f);
    CHECK_NEAR(rotation_y_at_90, 0.f, 0.01f);
    CHECK(graphics.world_uv_primitives > 0);  // mode8 世界四边形有提交
}

TEST(ending_staff_real_msg_cues_all_credit_scripts_by_line) {
    // 真实 staff.msg（raw）：op8 vm0@0(背景) + vm1..6@200..3540(名单 script5..14)
    // + op5(-1)@3600 按键门 + op14/op11 + END@3660。无输入驱动 3660+ 帧，
    // 断言全部名单 script 都按行号 cue（用户 BUG 复现路径的场景层）。
    ScenePlatform host;
    host.assets["staff.msg"] = read_asset("staff.msg");
    CHECK(!host.assets["staff.msg"].empty());
    // 合成 e01.msg：仅一条 op12 → staff.msg（msg 头同式 ending_scene_test）。
    const std::string jump = "staff.msg";
    std::vector<u8> e01{1, 0, 0, 0, 0x0c, 0, 0, 0, 0x00, 0x01, 0, 0};
    e01.insert(e01.end(), {0, 0, 12, u8(jump.size() + 1)});
    e01.insert(e01.end(), jump.begin(), jump.end());
    e01.push_back(0);
    e01.insert(e01.end(), {0, 0, 0, 0});
    host.assets["e01.msg"] = e01;
    SessionState progress;
    EndingScene scene(progress, host);
    CHECK(scene.initialize());
    GameInput input;
    for (i32 frame = 0; frame < 3660 + 30 && !scene.finished(); frame++)
        CHECK(scene.update(input));
    // 不按键：op5(-1)@3600 应拦住 END——未完。
    CHECK(!scene.finished());
    std::string got;
    for (auto& cue : host.cues) got += std::to_string(cue.second) + ",";
    // 0=背景,5..14=名单条带（除 14 在 3540 也应已 cue——3660 帧内）。
    for (i32 script : {0, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14})
        CHECK(got.find(std::to_string(script) + ",") != std::string::npos);
}

int main() { return th13test::run_all("Ending staff roll 名单 VM"); }
