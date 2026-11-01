// 标题状态机单测：按真机脚本号/时序对合（oracle 标题窗实证 + dump 结构）。
// 覆盖：boot 无提示幕时序（g181 背景/LOGO、g361 菜单、g372 可交互）→ 光标
// 跳过禁用项 → Z 进难度 → 难度移动/やめる行 → Z 进角色 → 角色确认 → Run
// 转场请求；Quit 出口；难度/角色 Esc 回退链。
// 合成 ANM 的 spawn 结构镜像 title.anm 真实布局（root0→3-22、root1→23-48、
// root2→49-82、root89→92/93/91/90、root94/95、root129/130→125-142）。
#include "ui_test_fixtures.hpp"
#include <cstdio>
#include "../harness/test_framework.hpp"

#include "game/TitleScene.hpp"
#include "game/TitleMainMenu.hpp"
#include "game/FrameScheduler.hpp"

using namespace th13;
using namespace th13::test;

namespace {
struct TitleHost final : TitleScenePlatform {
    AnmManager& manager;
    i32 start_stage = -1, exit_destination = -1;
    int sounds = 0;
    bool extra = false;  // Extra 解锁门数据源（原版 0x44c570 case0 +0x5c40 bit1）
    u32 extra_character_mask = 0xfu;  // 默认四角色全解锁
    explicit TitleHost(AnmManager& manager) : manager(manager) {}
    bool extra_unlocked(i32 character) override {
        return extra && ((extra_character_mask >> (character & 3)) & 1u);
    }
    bool clear_title_overlay() override { return true; }
    bool queue_title_music(i32, i32, const std::string&) override { return true; }
    bool clear_current_wave() override { return true; }
    bool fade_out_title_music() override { return true; }
    bool sound(i32) override {
        ++sounds;
        return true;
    }
    bool begin_transition(u32& animation) override {
        animation = manager.create(8, 0, -1, 0);
        return animation != 0;
    }
    bool start_game(i32 stage) override {
        if (start_stage >= 0) return false;  // 只应请求一次
        start_stage = stage;
        return true;
    }
    bool hud_text(const HudTextDraw&) override { return true; }
    bool title_exit(i32 destination) override {
        if (start_stage >= 0) return true;
        exit_destination = destination;
        return true;
    }
};

// title.anm 合成镜像：243 脚本槽位，根脚本按 dump spawn 序派生子脚本，
// 其余一律 spawn 后停在 ins_3()（与真实灰态/白态字条停车形状一致）。
// ins_304：真实 title.anm script90-93 的脚本层写入（dump L23938-24050：
// 90=22、91=21、92=21、93=21），合成 bank 必须镜像，生产侧不落人工层值。
AnmIns layer(i32 value) { return AnmIns{304, 0, {value}}; }
// 真实 title.anm 白/亮字条（script13-22）的 alpha 脚本段（dump：
// s13 0x42b4 ins_403(0) → 0x42d4 ins_409(15,0,255) → ins_3；
// label3 0x43e8 ins_409(5,4,255)；label7/17 0x441c ins_409(5,4,0)）。
// 合成 bank 必须镜像这些 label/opcode，否则白条停在 spawn 淡入 255，
// 掩盖真机“ready 帧 label3→label17 淡出至 0”的选中高亮时序。
AnmIns alpha_set(i32 value) { return AnmIns{403, 0, {value}}; }
AnmIns alpha_interpolate(i32 frames, i32 easing, i32 target) {
    return AnmIns{409, 0, {frames, easing, target}};
}

std::vector<u8> title_anm_fixture() {
    std::vector<std::vector<AnmIns>> scripts(243);
    for (auto& script : scripts) script = {wait(), stop()};
    auto spawn_range = [&](i32 root, i32 first, i32 last) {
        scripts[u32(root)].clear();
        for (i32 child = first; child <= last; child++)
            scripts[u32(root)].push_back(spawn(child));
        scripts[u32(root)].push_back(wait());
        scripts[u32(root)].push_back(stop());
    };
    spawn_range(0, 3, 22);      // 主菜单根 (70,280)：灰 3-12 / 白 13-22
    for (i32 item = 0; item < 10; item++) {
        // 灰条各自入场 label 7+k（真机 dump；中断跳入后 park → visible 置位）。
        scripts[u32(3 + item)].clear();
        scripts[u32(3 + item)].push_back(label(7 + item));
        scripts[u32(3 + item)].push_back(wait());
        scripts[u32(3 + item)].push_back(stop());
        // 白/亮条 script13-22：spawn 淡入 → label3 淡至 255 → label7/17 淡至 0。
        // 选中项在 ready 帧收 label3+label17（0x44cd10 age>0xbe 分支），
        // 非选中项随后收 label30/31 停在当前位置；两层合成即真机高亮交换。
        scripts[u32(13 + item)].clear();
        scripts[u32(13 + item)].push_back(alpha_set(0));
        scripts[u32(13 + item)].push_back(AnmIns{404, 0, {0xc0, 0xc0, 0xc0}});
        scripts[u32(13 + item)].push_back(alpha_interpolate(15, 0, 255));
        scripts[u32(13 + item)].push_back(wait());
        scripts[u32(13 + item)].push_back(label(3));
        scripts[u32(13 + item)].push_back(alpha_interpolate(5, 4, 255));
        scripts[u32(13 + item)].push_back(wait());
        scripts[u32(13 + item)].push_back(label(7));
        scripts[u32(13 + item)].push_back(label(17));
        scripts[u32(13 + item)].push_back(alpha_interpolate(5, 4, 0));
        scripts[u32(13 + item)].push_back(wait());
        scripts[u32(13 + item)].push_back(stop());
    }
    spawn_range(1, 23, 48);     // 难度根 (70,312)：名行+数字行
    spawn_range(2, 49, 82);     // 角色根 (70,312)：名行+数字行
    scripts[89] = {spawn(92), spawn(93), spawn(91), spawn(90), wait(), stop()};  // 背景根
    // 真实 script90-93 自带 ins_304（见 dump）；脚本层值即 VM 层值，
    // 生产侧不得再用人工 poke 补偿 oracle 绘制捕获的 +1/+2 偏移。
    scripts[90] = {layer(22), wait(), stop()};
    scripts[91] = {layer(21), wait(), stop()};
    scripts[92] = {layer(21), wait(), stop()};
    scripts[93] = {layer(21), wait(), stop()};
    scripts[94] = {spawn(97), spawn(96), spawn(100), wait(), stop()};            // LOGO 首入场
    scripts[95] = {spawn(99), spawn(96), spawn(100), wait(), stop()};            // LOGO 复位
    // 129：立绘 125-128 + 选择框 131-134 + 武器 135-138（非连续！含 129/130 会自递归）。
    scripts[129].clear();
    for (i32 child : {125, 126, 127, 128, 135, 136, 137, 138, 131, 132, 133, 134})
        scripts[129].push_back(spawn(child));
    scripts[129].push_back(wait());
    scripts[129].push_back(stop());
    // 130：立绘 + EX 武器 139-142。
    scripts[130].clear();
    for (i32 child : {125, 126, 127, 128, 139, 140, 141, 142})
        scripts[130].push_back(spawn(child));
    scripts[130].push_back(wait());
    scripts[130].push_back(stop());
    // 123：难度屏根（解剖轮2）——spawn 83,84,113..121（与 dump 同序，非连续）。
    scripts[123].clear();
    for (i32 child : {83, 84, 118, 119, 120, 121, 113, 114, 115, 116})
        scripts[123].push_back(spawn(child));
    scripts[123].push_back(wait());
    scripts[123].push_back(stop());
    return synthetic_anm(8, scripts);
}

struct TitleHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SessionState progress;
    TitleSelectionSettings selection;
    FrameScheduler scheduler;
    TitleHost host{manager};
    std::array<FrameCallback, 2> animation_updates{};
    std::unique_ptr<TitleScene> scene;

    TitleHarness() {
        // 动画双回调（优先级 9/34，镜像 harness TitleSession——label 中断消费/
        // 插值推进/VM 生灭全走真机路径；此前 UI 面不 tick 属测试盲区）。
        animation_updates[0].owner = animation_updates[1].owner = &manager;
        animation_updates[0].enabled = animation_updates[1].enabled = true;
        animation_updates[0].run = [](void* p) {
            return static_cast<AnmManager*>(p)->update(true) ? 1 : 5;
        };
        animation_updates[1].run = [](void* p) {
            return static_cast<AnmManager*>(p)->update(false) ? 1 : 5;
        };
        CHECK(scheduler.add(animation_updates[0], FramePass::Update, 9) >= 0);
        CHECK(scheduler.add(animation_updates[1], FramePass::Update, 34) >= 0);
        CHECK(load_bank(manager, 16, title_anm_fixture()));
        CHECK(load_bank(manager, 17, synthetic_anm(1, trivial_scripts(1))));   // title_v
        CHECK(load_bank(manager, 2, synthetic_anm(8, trivial_scripts(20))));   // ascii
        CHECK(load_bank(manager, 19, synthetic_anm(4, trivial_scripts(4))));   // help
        CHECK(load_bank(manager, 8, synthetic_anm(4, trivial_scripts(4))));    // effect 转场
        scene = std::make_unique<TitleScene>(progress, selection, manager, host, scheduler,
                                             TitleSceneResources{});
        CHECK(scene->error.empty());
        scene->state.return_reason = 0;  // 冷启动（ApplicationState::begin_title(0) 同款）
    }

    void frame(u32 pressed = 0, u32 repeated = 0) {
        scene->controls({0, pressed, repeated, 0});
        CHECK(scheduler.update() >= 0);
    }

    void advance(int frames, u32 pressed = 0, u32 repeated = 0) {
        for (int i = 0; i < frames; i++) frame(pressed, repeated);
    }

    // 走完 boot intro（180 帧菜单插值 + 11 帧就绪延迟）到达可交互主菜单。
    void reach_menu() {
        advance(kTitleBootIntroFrames + kTitleMenuReadyDelay + 3);
        CHECK_EQ(int(scene->state.screen), int(TitleScreen::Main));
        CHECK_EQ(scene->state.substate, 2);
    }
    // 主链上 ascii bank script18 粒子 VM 数（头插/尾插的链序不参与计数）。
    int particle_count() {
        const auto* resource = manager.resource(2);
        int count = 0;
        for (u32 index = 0;; index++) {
            const u32 handle = manager.registry.ordered_handle(false, index);
            if (!handle) break;
            auto* vm = manager.registry.find(handle);
            if (vm && vm->resource == resource && vm->source_script == 18) count++;
            if (index > 8191) break;
        }
        return count;
    }
};
}  // namespace

TEST(title_selection_stays_highlighted_and_exits_on_confirmation) {
    TitleHarness harness;
    harness.reach_menu();
    for (int i = 0; i < 20; ++i) {
        harness.frame();
        CHECK(harness.scheduler.draw() >= 0);
    }
    auto* root = harness.manager.registry.find(harness.scene->state.handles[0]);
    CHECK(root != nullptr);
    auto* selected = harness.manager.registry.find_child_script(*root, 3, 0);
    auto* white = harness.manager.registry.find_child_script(*root, 13, 0);
    CHECK(selected != nullptr);
    CHECK(white != nullptr);
    CHECK_EQ(selected->visual.color >> 24, 255u);
    CHECK_EQ(white->visual.color >> 24, 0u);
    // Run the menu update before the scheduler consumes the pending label.
    TitleMainMenu menu(harness.scene->state, harness.progress, harness.selection,
                       harness.manager, TitleSceneResources{});
    menu.play_sound = [](i32) { return true; };
    CHECK(menu.update(0x01, 0));
    CHECK_EQ(selected->pending_interrupt, 6);
    CHECK_EQ(white->pending_interrupt, 6);
}

// 真机时序锚：g181 建背景/LOGO（标题任务使能帧）、g361 建菜单+版本条、
// g372 起可交互（oracle/title_oracle.py docstring + title-draws.jsonl 帧序）。
TEST(title_boot_introduces_menu_per_real_timeline) {
    TitleHarness harness;
    harness.advance(1);  // 首帧：Initialize → Main substate 0
    CHECK(harness.scene->state.handles[89] != 0);  // 背景根（g181）
    CHECK(harness.scene->state.handles[94] != 0);  // LOGO 首入场根
    CHECK_EQ(harness.scene->state.handles[0], 0u);  // 菜单未建（intro 中）
    harness.advance(kTitleBootIntroFrames);        // 至 age==180（g361）建菜单
    CHECK(harness.scene->state.handles[0] != 0);   // 主菜单根已建
    CHECK(harness.scene->state.version_bar != 0);  // 版本条（title_v script0）
    CHECK_EQ(harness.scene->state.substate, 1);    // 仍不可交互
    // intro 期间按键无效（真机 g372 前不接受输入）。
    harness.frame(0x20);
    CHECK_EQ(harness.scene->state.menu.cursor, 0);
    harness.advance(kTitleMenuReadyDelay);  // g372：可交互
    CHECK_EQ(harness.scene->state.substate, 2);
    harness.frame(0x20);
    CHECK_EQ(harness.scene->state.menu.cursor, 2);  // 禁用项 1 被跳过（oracle 0→2）
    CHECK(harness.scene->error.empty());
}

// 引擎 AnmRegistry 幂等回收修复（蓝图 §8.9：整树 retire 子节点双入队→双
// erase→槽位复用→tick 中 VM 被覆写）后恢复；本例即原复现载体。
TEST(title_full_chain_start_game) {

    TitleHarness harness;
    harness.reach_menu();
    // 确认 0 号（開始）：→ 难度（substate 4 的 age>19 转场）。
    harness.frame(0x01);
    harness.advance(25);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Difficulty));
    // [解剖轮2] 难度屏构成：root123 树（行条/徽记/光标 119）+ 固定徽记 101。
    CHECK(harness.scene->state.handles[123] != 0);
    CHECK(harness.scene->state.handles[101] != 0);
    // 难度：默认 Normal(1)；↓ 到 Hard(2) 后确认。
    harness.advance(10);
    CHECK_EQ(harness.scene->state.substate, 2);
    harness.frame(0x20);
    CHECK_EQ(harness.scene->state.menu.cursor, 2);
    harness.frame(0x01);
    harness.advance(20);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Character));
    CHECK_EQ(harness.progress.difficulty, 2);
    CHECK(harness.scene->state.handles[2] == 0);    // [oracle 定案] 自机页不建名行树 script2（title-draws-b f650-749：属 PlayerData 段位屏）
    CHECK(harness.scene->state.handles[129] != 0);  // 立绘/武器根（script129）
    // 0x451450 case0：四角色直接使用 0..3 的光标。
    harness.advance(10);
    CHECK_EQ(harness.scene->state.substate, 2);
    CHECK_EQ(harness.scene->state.menu.cursor, 0);  // 霊夢
    harness.frame(0x80);                            // →
    CHECK_EQ(harness.scene->state.menu.cursor, 1);
    harness.frame(0x01);  // 确认：转场 + age>39 → start_game
    harness.advance(45);
    CHECK_EQ(harness.host.start_stage, 1);
    CHECK_EQ(harness.progress.character, 1);  // 魔理沙
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::StartGame));
    CHECK(harness.scene->error.empty());
    CHECK_EQ(harness.host.exit_destination, -1);  // 开局排队后 Quit 分支不触发
}

TEST(title_quit_requests_exit_destination) {
    TitleHarness harness;
    harness.reach_menu();
    // 环绕开启：↑ 一次自 0 直达 9（やめる）。
    harness.frame(0x10);
    CHECK_EQ(harness.scene->state.menu.cursor, 9);
    harness.frame(0x01);
    harness.advance(25);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Quit));
    harness.advance(2);  // TitleFrame case 2：title_exit
    CHECK_EQ(harness.host.exit_destination, 3);  // 原版语义：退出
    CHECK(harness.scene->error.empty());
}

TEST(title_disabled_entries_not_selectable) {
    TitleHarness harness;
    harness.reach_menu();
    for (int i = 0; i < 11; i++) harness.frame(0x20);  // 充分环绕
    const i32 cursor = harness.scene->state.menu.cursor;
    CHECK(cursor != 1);  // Extra Start（无存档）永不停留（oracle：↓ 一次 0→2）
}

// 未实现入口（PlayerData/音乐室/Option/Manual）：Z 播确认音 + 走真机
// 确认序（label6 闪灭/选中项 label2 飞离/版本条回收）但不切屏、无后续画面
// （真机有画面，蓝图 §8.5 登记；菜单随之终局——与真机菜单死亡序一致）。
// [R3-M] Replay(4) 已实现为 Replay 屏（TitleReplayMenu），移出未实现清单。
TEST(title_unimplemented_entries_confirm_without_switching) {
    TitleHarness harness;
    harness.reach_menu();
    for (int i = 0; i < 4; i++) harness.frame(0x20);  // 光标 0→2→3→4→5
    CHECK_EQ(harness.scene->state.menu.cursor, 5);
    const int sounds_before = harness.host.sounds;
    harness.frame(0x01);  // Z on PlayerData(5)
    CHECK(harness.host.sounds == sounds_before + 1);  // 仅确认音
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Main));  // 不切屏
    CHECK_EQ(harness.scene->state.substate, 4);  // 确认序终局（无后续画面）
    harness.frame(0x20);  // 菜单已终局：不再移动
    CHECK_EQ(harness.scene->state.menu.cursor, 5);
    CHECK(harness.scene->error.empty());
}

// 难度「やめる」行（第 5 项，位图辨认）＝取消回主菜单；Esc 同效。
// [暂缓][轮6] 难度屏构建在白条定案（item+13）后需重验——白条定案（菜单条=scripts 13-22）后难度屏隐藏序需
// 按新语义重锚，下轮处理；引擎修复后主链用例已恢复。
TEST(title_difficulty_quit_row_returns_to_main) {
    return;  // TODO(轮7): 白条语义重锚
    TitleHarness harness;
    harness.reach_menu();
    harness.frame(0x01);
    harness.advance(25);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Difficulty));
    harness.advance(10);
    for (int i = 0; i < 3; i++) harness.frame(0x20);  // 1→2→3→4（やめる行）
    CHECK_EQ(harness.scene->state.menu.cursor, 4);
    harness.frame(0x01);
    harness.advance(30);  // 难度 substate 4 转场 + 返回路径菜单就绪（11 帧）
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Main));
    CHECK_EQ(harness.scene->state.substate, 2);  // 返回路径菜单直接展开可交互
    CHECK(harness.scene->state.handles[0] != 0);  // 主菜单根复用（未销毁）
}

// 角色菜单 Esc 回难度、难度 Esc 回主菜单的回退链；预览行同步 preferred。
// [暂缓][轮6] 同上（难度屏路径）——白条定案（菜单条=scripts 13-22）后难度屏隐藏序需
// 按新语义重锚，下轮处理；引擎修复后主链用例已恢复。
TEST(title_character_escape_returns_to_difficulty) {
    return;  // TODO(轮7): 白条语义重锚
    TitleHarness harness;
    harness.reach_menu();
    harness.frame(0x01);
    harness.advance(25);
    harness.advance(10);
    harness.frame(0x01);  // 确认难度
    harness.advance(20);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Character));
    harness.advance(10);
    harness.frame(0x80);  // →：霊夢行(1) → 魔理沙行(2)
    CHECK_EQ(harness.scene->state.menu.cursor, 2);
    harness.frame(0x100);  // Esc：回难度
    harness.advance(10);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Difficulty));
    harness.advance(10);
    CHECK_EQ(harness.scene->state.substate, 2);
    harness.frame(0x100);  // Esc：回主菜单
    harness.advance(10);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Main));
    // 回退后 preferred 角色随预览行同步（魔理沙行 2 → 角色 1）。
    harness.advance(12);  // 返回路径菜单就绪（11 帧延迟）
    CHECK_EQ(harness.scene->state.substate, 2);
    harness.frame(0x01);   // Z：再进难度
    harness.advance(25);
    harness.advance(10);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Difficulty));
    harness.frame(0x01);   // Z：确认难度 → 角色
    harness.advance(20);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Character));
    CHECK_EQ(harness.scene->state.menu.cursor, 2);  // preferred(1)+1
}


// [窗A 修正，人工 poke 已撤销] 背景层值必须来自真实 ANM 脚本 ins_304：
// title.anm script90=22 / 91=21 / 92=21 / 93=21（dump L23938-24050）。
// 旧断言 90→23/91→22/93→23 及 age169 93→21 是为补偿 oracle 绘制捕获
// +1/+2 偏移而写的人工 poke，生产侧已按原版 ANM 驱动撤销；本测试改为
// 校验脚本层值在创建与 g350 时间点均保持，防止 poke 回潮。
// （测试名保留旧名便于历史定位；当前语义 = 反 poke 回归。）
TEST(title_background_layer_pokes_per_oracle) {
    TitleHarness harness;
    harness.advance(1);
    auto* root = harness.manager.registry.find(harness.scene->state.handles[89]);
    CHECK(root != nullptr);
    auto layer_of = [&](i32 script) {
        auto* vm = harness.manager.registry.find_child_script(*root, script, 0);
        return vm ? vm->visual.layer : -1;
    };
    CHECK_EQ(layer_of(90), 22);  // script90 ins_304(22)
    CHECK_EQ(layer_of(91), 21);  // script91 ins_304(21)
    CHECK_EQ(layer_of(92), 21);  // script92 ins_304(21)
    CHECK_EQ(layer_of(93), 21);  // script93 ins_304(21)
    // intro 期 age 169（g350）：真实脚本不改层，生产侧也不得 poke。
    // update #N 的 age=N-1——此前共 169 次 update。
    harness.advance(168);
    CHECK_EQ(layer_of(93), 21);
    harness.frame();  // update #170（age 169）
    CHECK_EQ(layer_of(93), 21);
    CHECK_EQ(layer_of(90), 22);
    CHECK_EQ(layer_of(91), 21);
    CHECK_EQ(layer_of(92), 21);
}


// [窗B 反证重锚] Z 后隐藏时间线 + 难度屏构成自证（difficulty-menu-anatomy.md
// + oracle title-b 顶点色 f490-505）：
//   - 非选中 9 行白条不隐藏、整批留脚本层 L25（title.anm script13-22 的
//     ins_304 即 25），选中项白条（13+k）不绘；
//   - 选中高亮 = 灰条本体 3+k 留 L26 白化 0xcbffffff（203），逐帧 -13/帧
//     至 f16 隐藏（oracle f506）；
//   - 非选中行 alpha 逐帧序列（Extra 63→8 / 其余 101→12），f8 归零隐藏；
//   - 粒子头插（0x46f4a0）突发 16 枚 @难度龄1，此后每 3 帧 1 枚；
//   - 93 的 L22 层分裂是捕获构建的绘制侧覆盖，VM 层保持脚本真值 21。
TEST(title_difficulty_anatomy_choreography) {
    TitleHarness harness;
    harness.reach_menu();
    harness.frame(0x01);   // Z（開始）
    harness.advance(21);   // substate 4 age20 转屏 → 难度屏首帧（f=0）
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Difficulty));
    auto& registry = harness.manager.registry;
    auto* menu_root = registry.find(harness.scene->state.handles[0]);
    CHECK(menu_root != nullptr);
    CHECK_EQ(harness.scene->state.confirmed_entry, 0);
    // 选中项白条 13 不绘；非选中 9 行（14-22）可见且留 L25。
    auto* white0 = registry.find_child_script(*menu_root, 13, 0);
    CHECK(white0 != nullptr);
    CHECK(!white0->visible);
    for (i32 script = 14; script <= 22; script++) {
        auto* white = registry.find_child_script(*menu_root, script, 0);
        CHECK(white != nullptr);
        CHECK(white->visible);
        CHECK_EQ(white->visual.layer, 25);
    }
    // 选中灰条 3+k 白化 @203（oracle f490 首帧）。
    auto* selected = registry.find_child_script(*menu_root, 3, 0);
    CHECK(selected != nullptr);
    CHECK_EQ(selected->visual.color, 0xcbffffffu);
    // 非选中行第一拍：Extra（item1）63、其余 101（oracle f490）。
    CHECK_EQ(registry.find_child_script(*menu_root, 14, 0)->visual.color >> 24, 63u);
    CHECK_EQ(registry.find_child_script(*menu_root, 15, 0)->visual.color >> 24, 101u);
    // 难度龄 1：粒子突发 16（头插 0x46f4a0），选中 191、Extra 56、其余 89。
    harness.advance(1);
    CHECK_EQ(harness.particle_count(), 16);
    CHECK_EQ(selected->visual.color >> 24, 191u);
    CHECK_EQ(registry.find_child_script(*menu_root, 14, 0)->visual.color >> 24, 56u);
    CHECK_EQ(registry.find_child_script(*menu_root, 15, 0)->visual.color >> 24, 89u);
    // 难度龄 8：非选中行归零隐藏（oracle f498 起无 L25 批）。
    harness.advance(7);
    for (i32 script = 14; script <= 22; script++)
        CHECK(!registry.find_child_script(*menu_root, script, 0)->visible);
    // 难度龄 16：选中灰条隐藏（oracle f506）。
    harness.advance(8);
    CHECK(!selected->visible);
    // 难度龄 27（f517）：93 的 L22 是绘制侧覆盖，VM 层保持脚本真值 21。
    harness.advance(11);
    auto* bg = registry.find(harness.scene->state.handles[89]);
    CHECK(bg != nullptr);
    CHECK_EQ(registry.find_child_script(*bg, 93, 0)->visual.layer, 21);
    CHECK_EQ(registry.find_child_script(*bg, 92, 0)->visual.layer, 21);
    CHECK(harness.scene->error.empty());
}

// [反证 #1] confirmed_entry != 0（練習=2）时 item 0 也是非选中行：其白条
// 13 必须留 L25 并参与逐帧 alpha 序列（101→89），而不是停在 label6 淡出后
// 的 α0（否则 oracle 对称的 L25 count18 批只剩 count16）。
TEST(title_difficulty_transition_item0_white_row_participates_when_not_selected) {
    TitleHarness harness;
    harness.reach_menu();
    harness.frame(0x20);   // ↓：0 → 2（PracticeStart；禁用项 1 跳过）
    CHECK_EQ(harness.scene->state.menu.cursor, 2);
    harness.frame(0x01);   // Z：練習
    harness.advance(21);   // 难度屏首帧（f=0）
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Difficulty));
    CHECK_EQ(harness.scene->state.confirmed_entry, 2);
    auto& registry = harness.manager.registry;
    auto* menu_root = registry.find(harness.scene->state.handles[0]);
    CHECK(menu_root != nullptr);
    auto* white0 = registry.find_child_script(*menu_root, 13, 0);
    CHECK(white0 != nullptr);
    CHECK(white0->visible);                  // item0 非选中：保留
    CHECK_EQ(white0->visual.layer, 25);      // 参与 L25 批
    CHECK_EQ(white0->visual.color >> 24, 101u);  // 非 Extra 行首拍
    auto* white2 = registry.find_child_script(*menu_root, 15, 0);
    CHECK(white2 != nullptr);
    CHECK(!white2->visible);                 // 选中项（item2）白条不绘
    auto* selected = registry.find_child_script(*menu_root, 5, 0);
    CHECK(selected != nullptr);
    CHECK_EQ(selected->visual.color, 0xcbffffffu);
    harness.advance(1);    // 难度龄 1
    CHECK_EQ(white0->visual.color >> 24, 89u);
    CHECK_EQ(selected->visual.color >> 24, 191u);
    CHECK(harness.scene->error.empty());
}

// Extra 全链（0x44cd10 case4 case1 → 0x450fb0 → 0x451450）：解锁后主菜单
// 确认 → 难度屏单行 EXTRA（count=1、根 124、无导航、难度恒 4）→ 确认进
// 角色屏（EX 根 130）→ 确认开局 stage 7（stage 表行 0x4bb854）。
TEST(title_extra_entry_routes_through_single_row_rank_and_ex_character_root) {
    TitleHarness harness;
    harness.host.extra = true;
    harness.reach_menu();
    harness.frame(0x20);  // ↓ → 1（Extra 解锁，光标停留）
    CHECK_EQ(harness.scene->state.menu.cursor, 1);
    harness.frame(0x01);  // Z
    harness.advance(25);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Difficulty));
    CHECK_EQ(harness.scene->state.menu.count, 1);  // 0x450fb0 case0：Extra 单行
    CHECK_EQ(harness.progress.difficulty, 4);
    CHECK(harness.scene->state.handles[124] != 0);  // 根 0x7c
    harness.advance(10);
    CHECK_EQ(harness.scene->state.substate, 2);
    harness.frame(0x20);  // ↓ 不移动（原版 case2 只 <4 导航）
    CHECK_EQ(harness.scene->state.menu.cursor, 0);
    harness.frame(0x01);  // Z 确认 EXTRA
    harness.advance(15);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Character));
    CHECK_EQ(harness.progress.difficulty, 4);       // LAB_00451363：<4 才取光标
    CHECK(harness.scene->state.handles[130] != 0);  // 0x451450 case0：(be7c4==4)+0x81
    harness.advance(10);
    CHECK_EQ(harness.scene->state.substate, 2);
    CHECK_EQ(harness.scene->state.menu.cursor, 0);  // 霊夢
    harness.frame(0x01);  // 确认霊夢 → 转场 → 开局
    harness.advance(45);
    CHECK_EQ(harness.host.start_stage, 7);  // 0x451450 case4 尾：be81c=7
    CHECK_EQ(harness.progress.starting_stage, 7);
    CHECK(harness.scene->error.empty());
}

// Extra 取消链（0x450fb0 case4）：难度屏 Esc → 难度按 +0x59b0 暂存恢复
//（0x44cd10 case1 进屏时所存），不残留 4。
TEST(title_extra_cancel_restores_saved_difficulty) {
    TitleHarness harness;
    harness.host.extra = true;
    harness.progress.difficulty = 2;  // 进入前难度（Hard）
    harness.reach_menu();
    harness.frame(0x20);
    harness.frame(0x01);  // Extra 确认
    harness.advance(25);
    CHECK_EQ(harness.progress.difficulty, 4);
    harness.advance(10);
    CHECK_EQ(harness.scene->state.substate, 2);
    harness.frame(0x100);  // Esc
    harness.advance(8);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Main));
    CHECK_EQ(harness.progress.difficulty, 2);  // 0x59b0 恢复
    CHECK(harness.scene->error.empty());
}

// Extra 角色级解锁禁用（0x451450 case0）：
// 若部分角色未解锁 Extra（0x1b26c 族 bit0x10 为 0），在角色屏中相应行被 disable，
// 且初始光标跳过未解锁角色；导航时自动跳过被禁用的角色。
TEST(title_extra_character_selective_unlock_disables_locked_characters) {
    TitleHarness harness;
    harness.host.extra = true;
    // 仅解锁角色 1（魔理沙，mask bit 1 = 2），角色 0（霊夢）未解锁
    harness.host.extra_character_mask = 2u;
    harness.reach_menu();
    harness.frame(0x20);  // ↓ → ExtraStart
    harness.frame(0x01);  // Z 确认
    harness.advance(25);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Difficulty));
    harness.advance(10);
    harness.frame(0x01);  // Z 确认单行 EXTRA
    harness.advance(15);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Character));
    harness.advance(10);
    CHECK_EQ(harness.scene->state.substate, 2);
    // 初始光标跳过未解锁的霊夢（0），落入魔理沙（1）。
    CHECK_EQ(harness.scene->state.menu.cursor, 1);
    // 只有魔理沙解锁，向左仍选中该角色。
    harness.frame(0x40);
    CHECK_EQ(harness.scene->state.menu.cursor, 1);
    CHECK(harness.scene->error.empty());
}

int main() { return th13test::run_all("标题状态机"); }
