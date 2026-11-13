// 符卡练习 TitleScene 全链路 ESC 回归（真实 title.anm / ascii.anm）。
// 用户报告（2026-11-11）：符卡练习对局返回后 ESC 逐级退回主菜单，画面残留
// 关卡表/卡表/难度行相互重叠。成因是三处与原版的机制分歧（修复与反汇编
// 依据见 TitleSpellPracticeView.cpp）：
//   ① [H-SP2] 对局返回级联的 label3/label(7+X)/label6 三连发须全部
//      immediate（0x456204-0x456238 / 0x456774-0x4567a2）；
//   ② 难度层取消对 188 发 label1 并清句柄，重入 0x12 由 0x456650 case0
//      判句柄失效整表重建（0x457941-0x457977、0x4566c1-0x456738）；
//   ③ rank 族 7 柄不发 label，render_flags|=0x20 对 VM+子体整树硬回收
//      （0x4569b6-0x4569f6）。
// 合成脚本没有行对的淡入/锁定/退场时间轴，观测不到上述残留，故本套件按
// music_room_real_assets_test 惯例装载原始资产，在四个稳态核对上屏 VM
// 构成。行对脚本（title.anm 实读）：关卡行 b 层 189..195 / a 层 196..202，
// 卡行 b 层 203..215 / a 层 216..228——选中行经 b 层显示、未选中行经 a 层
// 显示；难度行 182..186、rank 229..235、徽记 109（关卡层）/103（卡表层起）。
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"
#include "game/TitleScene.hpp"

#include <algorithm>
#include <cstdio>
#include <memory>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

bool read_file(const std::string& path, std::vector<u8>& out) {
    std::FILE* file = std::fopen(path.c_str(), "rb");
    if (!file) return false;
    std::fseek(file, 0, SEEK_END);
    const long size = std::ftell(file);
    std::fseek(file, 0, SEEK_SET);
    if (size < 0) { std::fclose(file); return false; }
    out.resize(size_t(size));
    const bool ok = std::fread(out.data(), 1, out.size(), file) == out.size();
    std::fclose(file);
    return ok;
}

struct SpellFlowHost final : TitleScenePlatform {
    SessionState& progress;
    AnmManager& animations;
    explicit SpellFlowHost(SessionState& p, AnmManager& m) : progress(p), animations(m) {}
    bool clear_title_overlay() override { return true; }
    bool queue_title_music(i32, i32, const std::string&) override { return true; }
    bool clear_current_wave() override { return true; }
    bool title_exit(i32) override { return true; }
    bool fade_out_title_music() override { return true; }
    bool sound(i32) override { return true; }
    bool begin_transition(u32& animation) override {
        animation = animations.create(8, 0, -1, 0, {});
        return animation != 0;
    }
    bool start_game(i32) override { return true; }
    bool record_menu_views() const override { return true; }
    bool rasterize_title_text(AnmVm&, u32, const std::string&,
                              AnmTextAlignment) override { return true; }
    bool hud_text(const HudTextDraw&) override { return true; }
    // 符卡练习目录：stage1 slot0 解锁（真实门数据语义由 menu/view 套件覆盖）。
    i32 stage_count() const override { return kSpellPracticeStages; }
    bool stage_available(i32 stage) const override { return stage >= 1 && stage <= 7; }
    std::string stage_name(i32) const override { return "stage"; }
    i32 spell_count(i32 stage) const override { return kSpellPracticeSlots[stage - 1]; }
    bool spell_unlocked(i32 stage, i32 slot) const override {
        return stage == 1 && slot == 0;
    }
    std::string spell_name(i32, i32) const override { return "spell"; }
    i32 spell_id(i32 stage, i32 slot, i32 difficulty) const override {
        return stage * 100 + slot * 10 + difficulty;
    }
    i32 spell_rank(i32, i32, i32 difficulty) const override { return difficulty; }
    bool spell_variant_available(i32, i32, i32) const override { return true; }
    std::string spell_variant_name(i32, i32, i32) const override { return "variant"; }
    bool spell_record(i32, i32, SpellPracticeRecord& record) const override {
        record = SpellPracticeRecord{};
        record.encountered = true;
        return true;
    }
    i32 character_count() const override { return kSpellPracticeCharacters; }
    std::string character_name(i32) const override { return "char"; }
};

struct SpellFlowHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SessionState progress;
    TitleSelectionSettings settings;
    FrameScheduler scheduler;
    SpellFlowHost host{progress, manager};
    std::array<FrameCallback, 2> updates{};
    std::unique_ptr<TitleScene> scene;

    SpellFlowHarness() {
        for (i32 i = 0; i < 2; ++i) {
            updates[u32(i)].owner = &manager;
            updates[u32(i)].enabled = true;
            updates[u32(i)].run = i == 0 ? +[](void* p) {
                return static_cast<AnmManager*>(p)->update(true) ? 1 : 5;
            } : +[](void* p) {
                return static_cast<AnmManager*>(p)->update(false) ? 1 : 5;
            };
            CHECK(scheduler.add(updates[u32(i)], FramePass::Update, i == 0 ? 9 : 34) >= 0);
        }
        std::vector<u8> title_bytes, ascii_bytes;
        CHECK(read_file("artifacts/reference/raw/title.anm", title_bytes));
        CHECK(read_file("artifacts/reference/raw/ascii.anm", ascii_bytes));
        CHECK(load_bank(manager, 16, title_bytes));
        CHECK(load_bank(manager, 2, ascii_bytes));
        CHECK(load_bank(manager, 17, synthetic_anm(1, trivial_scripts(1))));
        CHECK(load_bank(manager, 8, synthetic_anm(4, trivial_scripts(4))));
        scene = std::make_unique<TitleScene>(progress, settings, manager, host, scheduler);
        scene->state.return_reason = 0;
    }
    void frame(u32 pressed = 0) {
        scene->controls({0, pressed, 0, 0});
        CHECK(scheduler.update() >= 0);
        CHECK(scene->error.empty());
    }
    void advance(i32 count) { for (i32 i = 0; i < count; ++i) frame(); }
    void ready_main() {
        advance(kTitleBootIntroFrames + kTitleMenuReadyDelay + 3);
        CHECK_EQ(i32(scene->state.screen), i32(TitleScreen::Main));
        CHECK_EQ(scene->state.substate, 2);
    }
    const TitleSpellPracticeMenu* menu() const { return scene->spell_practice_menu(); }
    i32 level() const { return menu() ? i32(menu()->level()) : -1; }
};

// 上屏判定与 AnmRenderer 绘制门一致：flags&3==3（+0x18 bit0 脚本可见位 +
// bit1 外部隐藏门）、render_flags 无 0x20（硬回收）、color/secondary_color
// 至少一个 alpha>0。
bool on_screen(const AnmVm* vm) {
    return vm && (vm->visual.flags & 3u) == 3u &&
           !(vm->visual.render_flags & 0x20u) &&
           ((vm->visual.color >> 24) != 0u || (vm->visual.secondary_color >> 24) != 0u);
}

constexpr u32 kLayerBuckets = 33;  // AnmRegistry 渲染层桶数

// 渲染层桶全走查：[first,last] 内有上屏实例的脚本号（升序去重）。
std::vector<i32> drawn_scripts(const AnmManager& manager, i32 first, i32 last) {
    std::vector<i32> scripts;
    for (u32 bucket = 0; bucket < kLayerBuckets; ++bucket)
        for (const AnmVm* vm : manager.registry.layer(bucket))
            if (on_screen(vm) && vm->source_script >= first && vm->source_script <= last &&
                std::find(scripts.begin(), scripts.end(), vm->source_script) == scripts.end())
                scripts.push_back(vm->source_script);
    std::sort(scripts.begin(), scripts.end());
    return scripts;
}

// 指定脚本号上屏实例的最大 alpha（0 = 无上屏实例）。
u32 drawn_alpha(const AnmManager& manager, i32 script) {
    u32 alpha = 0;
    for (u32 bucket = 0; bucket < kLayerBuckets; ++bucket)
        for (const AnmVm* vm : manager.registry.layer(bucket))
            if (on_screen(vm) && vm->source_script == script)
                alpha = std::max(alpha, vm->visual.color >> 24);
    return alpha;
}

// 集合不等时打印两侧，定位残留/缺失的具体行。
bool same_scripts(const std::vector<i32>& actual, const std::vector<i32>& expected) {
    if (actual == expected) return true;
    std::printf("    上屏 {");
    for (i32 script : actual) std::printf(" %d", script);
    std::printf(" } 期望 {");
    for (i32 script : expected) std::printf(" %d", script);
    std::printf(" }\n");
    return false;
}

}  // namespace

TEST(spell_practice_esc_recedes_exactly_one_level_per_cancel) {
    SpellFlowHarness h;
    h.ready_main();
    // 缺省存档 Extra 行禁用：一次 ↓ 自 0 跳 2（practice flow 同款），再 ↓ 到 3。
    h.frame(0x20); h.frame();
    h.frame(0x20); h.frame();
    CHECK_EQ(h.scene->state.menu.cursor, 3);
    h.frame(1);                       // 主菜单 Z → SpellPractice 屏
    h.advance(45);
    CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::SpellPractice));
    CHECK(h.menu() != nullptr);
    CHECK_EQ(h.level(), i32(SpellPracticeLevel::Stage));

    h.frame(1);                       // stage → spell（20+11 帧延迟链）
    h.advance(45);
    CHECK_EQ(h.level(), i32(SpellPracticeLevel::Spell));
    h.frame(1);                       // spell → difficulty（14+11 帧延迟链）
    h.advance(45);
    CHECK_EQ(h.level(), i32(SpellPracticeLevel::Difficulty));

    // 一次 ESC：只退一级（难度→卡）。逐帧采样，不允许跳级。
    h.frame(0x100);                   // Esc
    for (i32 i = 0; i < 12; ++i) {
        h.frame();
        CHECK(h.level() == i32(SpellPracticeLevel::Difficulty) ||
              h.level() == i32(SpellPracticeLevel::Spell));
    }
    h.advance(40);
    CHECK_EQ(h.level(), i32(SpellPracticeLevel::Spell));

    h.frame(0x100);
    h.advance(52);
    CHECK_EQ(h.level(), i32(SpellPracticeLevel::Stage));
    CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::SpellPractice));

    h.frame(0x100);
    h.advance(52);
    CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::Main));
    CHECK(h.menu() == nullptr);
}

// [H-SP2] 对局返回级联（0x44c570 case0 dc670==5 → 0x4560a0 → 0x456650 →
// 0x4576c0 直入难度层）后 ESC×3 退回主菜单，走生产 restore 路径；每级留
// 80 帧让行对的淡入/退场时间轴落定后再核对。
TEST(spell_run_return_cascade_and_esc_chain_leave_no_stale_rows) {
    SpellFlowHarness h;
    // 对局返回会话标记（0x4576c0 启动拍写点）：stage=1、spell id=100、
    // run_mode_flags 0x20（符卡练习）、difficulty=rank。
    h.progress.run_mode_flags = (h.progress.run_mode_flags & ~0x70u) | 0x20u;
    h.progress.starting_stage = 1;
    h.progress.selected_spell_id = 100;
    h.progress.character = 0;
    h.progress.difficulty = 0;
    h.scene->state.return_reason = 5;
    h.frame();  // TitleFrame::initialize → restore_spell_practice_selection
    CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::SpellPractice));
    CHECK_EQ(h.level(), i32(SpellPracticeLevel::Difficulty));

    // 稳态 A：级联落定的难度层。选中的关卡行/卡行先经 label(7+X) 选中分支
    // 启动淡入、再被 label6 锁定为表头（①：旧实现的 7+X 挂 pending 后被
    // label6 覆盖，淡入从未启动，两个表头 alpha 恒 0）；未选中行全部退场，
    // 难度 5 行与对应 rank 在场。
    h.advance(50);
    CHECK_EQ(h.level(), i32(SpellPracticeLevel::Difficulty));
    CHECK(same_scripts(drawn_scripts(h.manager, 189, 202), {189}));
    CHECK(same_scripts(drawn_scripts(h.manager, 203, 228), {203}));
    CHECK_EQ(drawn_alpha(h.manager, 189), 255u);
    CHECK_EQ(drawn_alpha(h.manager, 203), 255u);
    CHECK(same_scripts(drawn_scripts(h.manager, 182, 186), {182, 183, 184, 185, 186}));
    CHECK(same_scripts(drawn_scripts(h.manager, 229, 235), {229, 230, 231, 232, 233}));
    CHECK_EQ(drawn_alpha(h.manager, 103), 255u);
    CHECK_EQ(drawn_alpha(h.manager, 109), 0u);

    // 稳态 B：ESC 回卡表层。188 整表重建（②：旧实现不重建，级联 label6 已
    // 让 a 层行退场，卡表只剩 203 一行）——stage1 的 4 张卡全部在场，缺席槽
    // 5..13 由 label39-i 收起；关卡表头、难度行与 rank 保留。
    h.frame(0x100);
    h.advance(79);
    CHECK_EQ(h.level(), i32(SpellPracticeLevel::Spell));
    CHECK(same_scripts(drawn_scripts(h.manager, 203, 228), {203, 217, 218, 219}));
    CHECK(same_scripts(drawn_scripts(h.manager, 189, 202), {189}));
    CHECK(same_scripts(drawn_scripts(h.manager, 182, 186), {182, 183, 184, 185, 186}));
    CHECK(same_scripts(drawn_scripts(h.manager, 229, 235), {229, 230, 231, 232, 233}));
    CHECK_EQ(drawn_alpha(h.manager, 103), 255u);

    // ESC 回关卡层的当帧：rank 整树硬回收，图标立即离屏（③：旧实现改发
    // label1，rank 随脚本的 label1 淡出约 25 帧，与入场的关卡表重叠）。
    h.frame(0x100);
    i32 settle = 79;
    for (; settle > 0 && h.level() != i32(SpellPracticeLevel::Stage); --settle) h.frame();
    CHECK_EQ(h.level(), i32(SpellPracticeLevel::Stage));
    CHECK(same_scripts(drawn_scripts(h.manager, 229, 235), {}));

    // 稳态 C：关卡层。187 重建为完整七行（选中行 b 层 + 其余 a 层）；卡表、
    // 难度行、103 徽记经 label1 退场完毕。
    h.advance(settle);
    CHECK(same_scripts(drawn_scripts(h.manager, 189, 202),
                       {189, 197, 198, 199, 200, 201, 202}));
    CHECK(same_scripts(drawn_scripts(h.manager, 203, 228), {}));
    CHECK(same_scripts(drawn_scripts(h.manager, 182, 186), {}));
    CHECK(same_scripts(drawn_scripts(h.manager, 229, 235), {}));
    CHECK_EQ(drawn_alpha(h.manager, 109), 255u);
    CHECK_EQ(drawn_alpha(h.manager, 103), 0u);

    // 稳态 D：ESC 回主菜单。符卡练习全族（徽记 103/109、182..242）零上屏，
    // 主菜单行在场。
    h.frame(0x100);
    h.advance(129);
    CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::Main));
    CHECK(h.menu() == nullptr);
    CHECK(same_scripts(drawn_scripts(h.manager, 182, 242), {}));
    CHECK_EQ(drawn_alpha(h.manager, 103), 0u);
    CHECK_EQ(drawn_alpha(h.manager, 109), 0u);
    CHECK(!drawn_scripts(h.manager, 89, 100).empty());
}

int main() { return th13test::run_all("Spell practice ESC flow"); }
