#include "../harness/test_framework.hpp"
#include "game/AnmManager.hpp"
#include "game/AnmRenderer.hpp"
#include "game/EndingScene.hpp"
#include "ui_test_fixtures.hpp"

#include <cmath>
#include <fstream>
#include <map>

using namespace th13;
using namespace touhou::graphics;

namespace {

struct ScenePlatform final : EndingPlatform {
    std::map<std::string, std::vector<u8>> assets;
    std::vector<std::pair<i32, i32>> cues;
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
        if (stride == 20) {
            ++world_uv_primitives;
            world_uv_frames = std::max(world_uv_frames, 1u);
        }
    }
    bool select_target(const AnmResource*, u32) override { return true; }
    bool clear_target(u32, const GraphicsViewport*) override { return true; }
    bool clear_depth(const GraphicsViewport*) override { return true; }
    void set_viewport(const GraphicsViewport&) override {}
};

}

TEST(ending_staff_script5_spins_credit_strip_and_draws) {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager(random, environment);
    const auto bytes = read_asset("staff.anm");
    CHECK(!bytes.empty());
    CHECK(manager.load(20, bytes.data(), u32(bytes.size())));
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
        CHECK(renderer.draw_layer(manager.registry.layer(25)));
        CHECK(renderer.draw_layer(manager.registry.layer(26)));
    }
    CHECK(saw_child_15);
    CHECK(saw_child_19);
    CHECK_NEAR(rotation_y_at_30, -1.5707964f / 2, 0.35f);
    CHECK_NEAR(rotation_y_at_90, 0.f, 0.01f);
    CHECK(graphics.world_uv_primitives > 0);
}

TEST(ending_staff_real_msg_cues_all_credit_scripts_by_line) {
    ScenePlatform host;
    host.assets["staff.msg"] = read_asset("staff.msg");
    CHECK(!host.assets["staff.msg"].empty());
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
    CHECK(!scene.finished());
    std::string got;
    for (auto& cue : host.cues)
        got += std::to_string(cue.second) + ",";
    for (i32 script : {0, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14})
        CHECK(got.find(std::to_string(script) + ",") != std::string::npos);
}

int main() { return th13test::run_all("Ending staff roll 名单 VM"); }
