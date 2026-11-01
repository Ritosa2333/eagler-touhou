// 立绘/名牌角色身份回归（独立于生产映射的 expected）。
//
// 预期身份来源（不是 MsgPresentation::kStageRows / side_portrait 的复制）：
//   - 原版 ECL ANIM 名单（st03.ecl 头，原版数据实读）：
//       [enemy.anm, st03enm.anm, st03menm.anm]
//     原版 0x418990 把名单第 i 项注册到敌机 ANM 管理器 bank 2+i
//     （0x4c2188+0x88+i*4，管理器槽号 i+10）；因此 stage 表 bank 3 =
//     st03enm.anm、bank 4 = st03menm.anm。
//   - 原版 st03*.msg 文本与 0x428ae0 opcode 2/20 分支：
//       entry 0/1（芳香 宮古芳香，boss）→ entry<2 → stage 表 +0x6c/+0x70
//         = bank3/st03enm；
//       entry 2（小傘 多々良小傘，中boss）→ entry>=2 → +0x64/+0x68
//         = bank4/st03menm。
//   - 原版 ANM 内嵌纹理（st03enm.anm face/enemy3/face03bs.png、
//     face/enemy3/ename01.png=宮古芳香；st03menm.anm
//     face/enemy3m/face03mbs.png、face/enemy3m/ename01.png=多々良小傘）。
//   合并 bank 内全局脚本号 = 名单前序文件脚本数之和 + 文件内脚本号；
//   该计数在本测试内由原版 ANM 字节独立解析，不读取生产表。
#include "../anm/anm_test_fixtures.hpp"
#include "game/RunScene.hpp"

#include <set>
#include <string>
#include <vector>

using namespace th13;

namespace {

struct Graphics final : ZunGraphics {
    touhou::graphics::PipelineState state;
    std::set<std::string> textures;
    touhou::graphics::PipelineState& pipeline() override { return state; }
    u32 texture(const AnmResource& file, u32 index) override {
        textures.insert(file.textures[index].name);
        return index + 1;
    }
    void bind_texture(u32) override {}
    void set_layout(touhou::graphics::VertexLayout) override {}
    void set_matrix(touhou::graphics::MatrixKind, const Matrix4&) override {}
    void triangles(u32, const void*, u32) override {}
    void primitives(touhou::graphics::Topology, u32, const void*, u32) override {}
    bool select_target(const AnmResource*, u32) override { return true; }
    bool clear_target(u32, const GraphicsViewport*) override { return true; }
    bool clear_depth(const GraphicsViewport*) override { return true; }
    void set_viewport(const GraphicsViewport&) override {}
};

struct Host final : RunSceneServices {
    AnmManager& animations;
    Graphics& graphics;
    th13test::TestFileReader reader;
    int lines = 0;
    Host(AnmManager& a, Graphics& g) : animations(a), graphics(g) {}
    bool read_asset(const std::string& name, std::vector<u8>& bytes, bool = true) override {
        return reader.read("artifacts/reference/raw/" + name, bytes);
    }
    bool load(i32 bank, const std::string& name) {
        std::vector<u8> bytes;
        const bool ok = read_asset(name, bytes) &&
                        animations.load(bank, bytes.data(), u32(bytes.size()));
        if (!ok) std::printf("load %s: %s\n", name.c_str(), animations.error.c_str());
        return ok;
    }
    bool preload_scenery_animation(i32 bank, const std::string& name) override {
        return load(bank, name);
    }
    ZunGraphics& stage_graphics() override { return graphics; }
    bool request_pause() override { return true; }
    bool request_game_over() override { return true; }
    bool request_destination(i32) override { return true; }
    bool record_stage_clear() override { return true; }
    bool hud_sound(i32) override { return true; }
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool camera(DrawCamera, bool) override { return true; }
    bool hud_text(const HudTextDraw&) override { return true; }
    bool queue_music(i32, i32, const std::string&) override { return true; }
    bool dialogue_line(i32, const MsgLine&, float, float) override { ++lines; return true; }
};

// 原版 ANM 文件的脚本总数（chunk 链：version@0、脚本数@6、stride@36）。
bool anm_script_count(const std::vector<u8>& bytes, u32& count) {
    u32 base = 0, total = 0;
    for (;;) {
        if (base + 64 > bytes.size()) return false;
        u32 version = 0, stride = 0;
        u16 scripts = 0;
        std::memcpy(&version, bytes.data() + base, 4);
        std::memcpy(&scripts, bytes.data() + base + 6, 2);
        std::memcpy(&stride, bytes.data() + base + 36, 4);
        if (version != 8) return false;
        total += scripts;
        if (!stride) break;
        base += stride;
    }
    count = total;
    return true;
}

struct StageAssets {
    std::vector<std::string> anim_list;
    std::vector<u32> script_base;  // 名单序 i 的全局脚本基号
    std::vector<u8> msg;
    std::vector<u8> ecl;
    std::string std_name;
};

bool load_stage_assets(th13test::TestFileReader& reader, i32 stage, StageAssets& out) {
    char name[32];
    std::snprintf(name, sizeof name, "st%02d.ecl", int(stage));
    if (!reader.read("artifacts/reference/raw/" + std::string(name), out.ecl)) return false;
    EclResource resource;
    if (!resource.open(out.ecl.data(), u32(out.ecl.size()))) return false;
    out.anim_list = resource.animations;
    u32 base = 0;
    for (const auto& anim : out.anim_list) {
        std::vector<u8> bytes;
        if (!reader.read("artifacts/reference/raw/" + anim, bytes)) return false;
        u32 scripts = 0;
        if (!anm_script_count(bytes, scripts)) return false;
        out.script_base.push_back(base);
        base += scripts;
    }
    std::snprintf(name, sizeof name, "st%02da.msg", int(stage));
    if (!reader.read("artifacts/reference/raw/" + std::string(name), out.msg)) return false;
    std::snprintf(name, sizeof name, "st%02d.std", int(stage));
    out.std_name = name;
    return true;
}

// 独立身份判定：entry 0/1 = 芳香（st03enm face/enemy3），entry 2 = 小傘
// （st03menm face/enemy3m）。纹理路径来自原版 ANM 内嵌 THTX 名。
bool expected_family(i32 entry, const std::string& texture) {
    if (entry == 2) return texture.rfind("face/enemy3m/", 0) == 0;
    return texture.rfind("face/enemy3/", 0) == 0;
}

struct Observation {
    std::string texture;
    i32 source_script = -1;
    i32 bank = -1;
    bool visible = false;
    u32 alpha = 0;
    u32 handle = 0;
};

std::vector<Observation> observe(AnmManager& animations) {
    std::vector<Observation> out;
    for (u32 layer = 0; layer < 33; ++layer) {
        for (AnmVm* vm : animations.registry.layer(layer)) {
            if (!vm) continue;
            const auto* source = vm->sprite_resource ? vm->sprite_resource : vm->resource;
            Observation o;
            if (source && vm->texture_sprite()) {
                const u32 index = vm->texture_sprite()->texture;
                if (index < source->textures.size()) o.texture = source->textures[index].name;
            }
            o.source_script = vm->source_script;
            o.bank = vm->resource ? animations.resource_id(vm->resource) : -1;
            o.visible = vm->visible;
            o.alpha = vm->visual.color >> 24;
            o.handle = animations.registry.handle(*vm);
            out.push_back(std::move(o));
        }
    }
    return out;
}

bool has_texture(const std::vector<Observation>& vms, const std::string& texture,
                 i32 script = -1, bool visible = true) {
    for (const auto& vm : vms) {
        if (vm.texture != texture) continue;
        if (visible && (!vm.visible || !vm.alpha)) continue;
        if (script >= 0 && vm.source_script != script) continue;
        return true;
    }
    return false;
}

bool has_prefix(const std::vector<Observation>& vms, const std::string& prefix,
                bool visible = true) {
    for (const auto& vm : vms) {
        if (vm.texture.rfind(prefix, 0) != 0) continue;
        if (visible && (!vm.visible || !vm.alpha)) continue;
        return true;
    }
    return false;
}

struct Scene {
    GameRandom rng{0};
    AnmEnvironment env;
    AnmManager animations{rng, env};
    Graphics graphics;
    AnmRenderer renderer{graphics};
    Host host{animations, graphics};
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    FrameScheduler scheduler;
    GameInput input;
    std::unique_ptr<RunScene> scene;
    StageAssets assets;

    bool build(i32 stage, i32 character) {
        if (!animations.resource(0) && !host.load(0, "text.anm")) return false;
        if (!animations.resource(2) && !host.load(2, "ascii.anm")) return false;
        if (!animations.sprite_fallback(2)) return false;
        for (const auto& bank : std::vector<std::pair<i32, std::string>>{
                 {3, "front.anm"}, {5, "bullet.anm"}, {8, "effect.anm"}, {9, "astral.anm"}}) {
            if (animations.resource(bank.first)) continue;
            if (!host.load(bank.first, bank.second)) return false;
        }
        if (!animations.resource(6) &&
            !host.load(6, "pl0" + std::to_string(character) + ".anm"))
            return false;
        if (!load_stage_assets(host.reader, stage, assets)) return false;
        AnmResource merged;
        for (const auto& name : assets.anim_list) {
            std::vector<u8> bytes;
            if (!host.read_asset(name, bytes)) return false;
            if (!merged.append(bytes.data(), u32(bytes.size()))) return false;
        }
        if (!animations.adopt(4, std::move(merged))) return false;
        char logo[32], prefix[32];
        std::snprintf(logo, sizeof logo, "st%02dlogo.anm", int(stage));
        std::snprintf(prefix, sizeof prefix, "st%02d", int(stage));
        if (!host.load(26, logo)) return false;
        progress.character = character;
        progress.stage = stage;
        scene = std::make_unique<RunScene>(
            progress, player, score, animations, renderer, env, host, scheduler,
            StagePresentation::Banks{4, 5, 6, 8, 9}, 3, 2, 7, assets.std_name);
        scene->battle_sht = "pl0" + std::to_string(character) + "a.sht";
        scene->hud.stage_logo_bank = 26;
        if (!scene->initialize()) {
            std::printf("initialize stage %d: %s\n", int(stage), scene->error.c_str());
            return false;
        }
        return true;
    }

    void drop() {
        scene.reset();
        animations.unload(4);
        animations.unload(5);
        animations.unload(7);   // scenery bank (stXXwl.anm) must be reloaded per stage
        animations.unload(9);
        animations.unload(26);
    }

    // 推进对话到结束或 max_frames；返回本 entry 内观察到的全部 VM。
    bool play(i32 entry, int max_frames, std::vector<Observation>& seen, bool& closed) {
        scene->open_dialog(entry);
        if (!scene->dialog_active()) {
            std::printf("stage %d entry %d open: %s\n", int(progress.stage), int(entry),
                        scene->error.c_str());
            return false;
        }
        GameInput& input = this->input;
        scene->controls(input);
        // Isolated entry probes start after ECL 524's original chapter write.
        progress.chapter = entry == 2 ? 4 : 6;
        closed = false;
        for (int frame = 0; frame < max_frames; ++frame) {
            input.held = input.pressed = (frame % 30 == 0) ? 1u : 0u;
            input.duration[0] = input.held ? 1 : 0;
            if (!scene->update()) {
                if (!scene->error.empty()) {
                    std::printf("stage %d entry %d update: %s\n", int(progress.stage),
                                int(entry), scene->error.c_str());
                    return false;
                }
                closed = true;
                break;
            }
            if (!animations.update()) return false;
            for (const auto& vm : observe(animations)) seen.push_back(vm);
            if (!scene->dialog_active()) {
                closed = true;
                break;
            }
        }
        return true;
    }
};

// 合并 bank 全局脚本号：名单序 index 的基号 + 文件内脚本号。
i32 script_of(const StageAssets& assets, const char* file, i32 local) {
    for (u32 i = 0; i < assets.anim_list.size(); ++i)
        if (assets.anim_list[i] == file) return i32(assets.script_base[i]) + local;
    return -1;
}

}  // namespace

TEST(portrait_identity_expected_mapping_rejects_other_family) {
    // 独立负控：expected 判定对另一角色族必须为 false。
    CHECK(!expected_family(0, "face/enemy3m/face03mbs.png"));
    CHECK(!expected_family(1, "face/enemy3m/ename01.png"));
    CHECK(!expected_family(2, "face/enemy3/face03bs.png"));
    CHECK(!expected_family(2, "face/enemy3/ename01.png"));
    CHECK(expected_family(0, "face/enemy3/face03bs.png"));
    CHECK(expected_family(1, "face/enemy3/ename01.png"));
    CHECK(expected_family(2, "face/enemy3m/face03mbs.png"));
    CHECK(expected_family(2, "face/enemy3m/ename01.png"));
}

TEST(stage3_boss_entry0_yoshika_portrait_and_nameplate) {
    Scene s;
    CHECK(s.build(3, 1));  // Marisa st03b.msg
    if (!s.scene) return;
    const i32 boss_base = script_of(s.assets, "st03enm.anm", 0);
    CHECK(boss_base >= 0);
    std::vector<Observation> seen;
    bool closed = false;
    CHECK(s.play(0, 2400, seen, closed));
    CHECK(closed);
    // 芳香立绘：st03enm 的 face/enemy3/face03bs.png（原版 ANM 内嵌纹理）。
    CHECK(has_texture(seen, "face/enemy3/face03bs.png"));
    // 根脚本 = bank3（st03enm）文件基号 + 11；子脚本 9/10 承载可见面片。
    bool root = false;
    for (const auto& vm : seen) {
        if (vm.source_script == boss_base + 11 && vm.bank == 4) root = true;
        if (vm.texture.rfind("face/enemy3/", 0) == 0 &&
            (vm.source_script == boss_base + 9 || vm.source_script == boss_base + 10))
            CHECK(expected_family(0, vm.texture));
    }
    CHECK(root);
    // 名牌 = st03enm ename01（宮古芳香），根脚本基号+16。
    CHECK(has_texture(seen, "face/enemy3/ename01.png"));
    bool name_root = false;
    for (const auto& vm : seen)
        if (vm.source_script == boss_base + 16 && vm.bank == 4) name_root = true;
    CHECK(name_root);
    // 负控：entry 0 不允许出现小傘族。
    CHECK(!has_prefix(seen, "face/enemy3m/"));
}

TEST(stage3_midboss_entry2_kogasa_portrait_and_nameplate) {
    Scene s;
    CHECK(s.build(3, 1));
    if (!s.scene) return;
    const i32 mid_base = script_of(s.assets, "st03menm.anm", 0);
    CHECK(mid_base >= 0);
    std::vector<Observation> seen;
    bool closed = false;
    CHECK(s.play(2, 2400, seen, closed));
    CHECK(closed);
    CHECK(has_texture(seen, "face/enemy3m/face03mbs.png"));
    bool root = false;
    for (const auto& vm : seen) {
        if (vm.source_script == mid_base + 11 && vm.bank == 4) root = true;
        if (vm.texture.rfind("face/enemy3m/", 0) == 0 &&
            (vm.source_script == mid_base + 9 || vm.source_script == mid_base + 10))
            CHECK(expected_family(2, vm.texture));
    }
    CHECK(root);
    CHECK(has_texture(seen, "face/enemy3m/ename01.png"));
    bool name_root = false;
    for (const auto& vm : seen)
        if (vm.source_script == mid_base + 16 && vm.bank == 4) name_root = true;
    CHECK(name_root);
    // 负控：entry 2 不允许出现芳香族。
    CHECK(!has_prefix(seen, "face/enemy3/"));
}

TEST(stage2_to_stage3_lifecycle_drops_old_portrait_identity) {
    Scene s;
    CHECK(s.build(2, 1));  // st02b.msg, 小傘? no: stage2 boss 響子 face/enemy2
    if (!s.scene) return;
    std::vector<Observation> seen2;
    bool closed = false;
    CHECK(s.play(0, 2400, seen2, closed));
    CHECK(closed);
    CHECK(has_prefix(seen2, "face/enemy2/"));
    // 关闭后旧立绘/名牌必须退出。
    for (const auto& vm : observe(s.animations)) {
        if (!vm.visible || !vm.alpha) continue;
        CHECK(vm.texture.rfind("face/enemy2/", 0) != 0);
    }
    // 模拟换关：Application 在换关时卸载 stage/bullet/spirit/logo bank，
    // 再把新关 ANM 名单合并进同一 kStageBank(4)。
    s.drop();
    CHECK(s.build(3, 1));
    if (!s.scene) return;
    std::vector<Observation> seen3;
    CHECK(s.play(0, 2400, seen3, closed));
    CHECK(closed);
    CHECK(has_texture(seen3, "face/enemy3/face03bs.png"));
    CHECK(has_texture(seen3, "face/enemy3/ename01.png"));
    // 旧关资源不得残留在可见 VM 上。
    CHECK(!has_prefix(seen3, "face/enemy2/"));
}

int main() { return th13test::run_all("portrait-identity"); }
