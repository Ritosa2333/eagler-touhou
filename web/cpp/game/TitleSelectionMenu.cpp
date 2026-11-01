#include "TitleSelectionMenu.hpp"

#include <vector>

namespace th13 {
namespace {
// [窗B 链序] 原版粒子经 0x46f4a0 头插（后建者先绘）；同层桶序由主链次序
// 决定。头插后新粒子位于链首，而 oracle 的 L22 桶序是「背景(91/93) →
// 粒子 → 左半 90 → 行条 83/84」，故把粒子整块移到背景之后（保持头插的
// 相对次序 = 新→旧）：逐个头插粒子按链序 requeue_to_back 即得原相对序。
// 只动绘制/步进链序，不改任何 VM 运行态；粒子在 create 的 initial_tick
// 已抽完 RNG，链序不影响随机流。
void requeue_particles_to_back(AnmManager& animations, i32 bank, i32 script) {
    auto* resource = animations.resource(bank);
    if (!resource) return;
    std::vector<u32> handles;
    for (u32 index = 0;; index++) {
        const u32 handle = animations.registry.ordered_handle(false, index);
        if (!handle) break;
        AnmVm* vm = animations.registry.find(handle);
        if (vm && vm->resource == resource && vm->source_script == script)
            handles.push_back(handle);
        if (index > 8191) break;  // 防御：池上限
    }
    for (const u32 handle : handles)
        if (AnmVm* vm = animations.registry.find(handle))
            animations.registry.requeue_to_back(*vm);
}
}  // namespace

bool TitleDifficultyMenu::check(bool ok) {
    if (!ok && error.empty()) error = state.menu.error.empty() ? visuals.error : state.menu.error;
    return ok;
}

bool TitleDifficultyMenu::sound(i32 id) {
    if (play_sound && play_sound(id)) return true;
    error = "Title difficulty sound service unavailable";
    return false;
}

// [难度行根因修正] 原版 Rank Select 状态机 0x450fb0（th13.exe 反汇编）给出
// 根级驱动语义，不是逐子 poke：
//   入口（0x4510a6-0x4510f6）：create root123（难度<4；extra 分支另有 124）后
//     interrupt(root, 3, immediate=true) → interrupt(root, 17+cursor, pending)；
//   光标移动（0x4511fb-0x451223）：sound10 → interrupt(root, 3, true) →
//     interrupt(root, 7+new_cursor, pending)。
// 0x46fc30 把 pending 标签写入根与全部直接子 VM（+0x4a0）；root123 spawn
// 83,84,118..121,113..116（暗行 95..98 / 亮行 90..93），脚本 label 7..10 与
// 17..20 等价，按 (row-cursor)*128 落位并给层/α（选中 L26/255，邻行 25/192、
// 23/128、22/0..192）。旧实现只给 118 发 label9、给 119 写层，其余 VM 停在
// label0：暗行在中心淡入到 α255 全叠（run-after-2 中心堆字根因）；选中亮行
// 永不出现。这里按原版根级广播驱动。
bool TitleDifficultyMenu::build_difficulty() {
    // 0x451092-0x4510c3：重入时旧根先 pending label1 并清槽（0x46fc30），
    // 再创建新根——Z 确认路径不清根槽（root 空壳跨自机屏存活）。
    // [0x450fb0 case0] Extra 态（be7c4>=4）根 = 0x7c=124（(be7c4>3)+0x7b）。
    const i32 tree = progress.difficulty >= 4 ? resources.difficulty_tree_extra
                                              : resources.difficulty_tree;
    if (state.handles[tree]) {
        if (!check(visuals.interrupt(tree, 1, false))) return false;
        state.handles[tree] = 0;
    }
    if (!check(visuals.create(tree))) return false;
    if (!check(visuals.create(resources.difficulty_badge))) return false;
    // [窗A f490 实证] 难度屏起背景左半 title00a 90 绘制层 L23→L22（oracle
    // f489 [22,23] → f490 起 [22,22]）。VM 层保持脚本真值，偏移由
    // TitleMainMenu::apply_draw_layers 按 screen 落绘制侧覆盖。
    auto* tree_vm = animations.registry.find(state.handles[tree]);
    if (!tree_vm) return check(false);
    if (!check(visuals.interrupt(tree, 3, true))) return false;
    return check(visuals.interrupt(tree, 17 + state.menu.cursor, false));
}

// [窗B 稳定字段实证 f490-497] 主菜单 Z+21 时间线（oracle title-b 顶点色）：
//   非选中 9 行**不隐藏**——白条 13..22（除选中项）整批 L25（oracle f490 起
//   L25 count18：Extra 行黑 0x3f000000、其余 8 行灰 0x65c0c0c0，alpha 续脚本
//   label6 衰减，f498 归 0）。title.anm script13-22 的 ins_304 层值即 25
//   （anm-list.txt title.anm 段脚本 13-22 首段；不是 26），故层无需改，仅
//   选中项白条不绘；高亮由**灰条本体 3+k 留 L26 白化 0xcbffffff（203）
//   承接**，续 -13/帧至 f505=0x0c、f506 隐藏（oracle f490 op15：L26 count2
//   UV(0,0) 左半 0xcbffffff = 灰条 sprite0，不是白条 sprite1）。
//   旧实现隐藏非选中行 + 建白条 14-22（0x40ffffff）是误读：oracle 窗口无白条
//   op，L25 批本即非选中白条本体（count18=9 行×2 图元）。
// [反证修正] 选中项由 state.confirmed_entry 决定（0=開始/2=練習/1=Extra）。
// 非默认入口（confirmed_entry != 0）时 item 0 也是非选中行，必须与其余行
// 同享 L25 批次与逐帧 alpha 序列——advance_choreography 的 poke 循环不得
// 从 item=1 起跳（否则 item 0 白条停在 label6 淡出后的 α0，L25 批少 2 图元）。
void TitleDifficultyMenu::finish_menu_hides() {
    auto* menu_root = animations.registry.find(state.handles[0]);
    if (!menu_root) return;
    for (i32 item = 0; item < kTitleMainEntries; item++) {
        auto* white = animations.registry.find_child_script(*menu_root, item + 13, 0);
        if (white) {
            // alpha 由 advance_choreography 的逐帧序列落色；先中和脚本
            // ins_409 插值（否则管理器 tick 在菜单 update 之后覆写 poke）。
            white->interpolators.alpha.duration = 0;
            if (item == state.confirmed_entry) {
                // 选中项白条不绘：oracle L25 批只有 9 个非选中行。
                white->visible = false;
                white->visual.flags &= ~1u;
            } else {
                // 非选中行：脚本层即 25（ins_304(25)），此处显式落层只为
                // 防御脚本改动；绘制侧 L25 批由脚本层直接成立。
                white->visual.layer = 25;
            }
        }
        if (item == state.confirmed_entry) {
            // 选中灰条 3+k：留 L26（脚本层），白化由本函数 + 逐帧序列落色。
            auto* row = animations.registry.find_child_script(*menu_root, item + 3, 0);
            if (!row) continue;
            row->interpolators.alpha.duration = 0;
            row->interpolators.color.duration = 0;
            row->visual.color = 0xcbffffffu;  // 白化 @203（oracle f490 样本）
            row->visual.secondary_color = 0xcbffffffu;
        }
    }
}

// [解剖轮2] 神霊粒子：ascii bank2 script18 克隆（解剖「打字机不存在」纠偏）。
// 脚本体经 RNG 槽 10010-10012（AnmVariables 的 ANM 视觉流 0x4dc31c，与
// 0x45ac90 的 256 步字体噪声共用；不是游戏流 0x4dc324）自算全屏随机出生位
// 与 300f 漂移目标（窗内不死灭 ✓ +300 自灭）。
// [窗B 插入实证] 粒子走 0x46f4a0 **头插**（ordering=2；oracle 插入 dump
// f491 起 16 枚 'main' 18@L22，此后每 3 帧 1 枚至 f749 = 窗B 共 102 枚）。
// 头插决定同层链序（后建者先绘）与 tick 次序；此前的尾插（0x46f3f0）与
// 原版相反。RNG 抽签在 create 的 initial_tick 完成，与链序无关；生产侧
// ApplicationState 先做 128 步视觉 RNG 字体噪声再装载银行，故 f491 的 RNG
// 状态与 oracle 一致（tools/title_rng_probe.py 实证：f491 首抽前 seed=30195，
// 与 oracle 0x4dc31c 相同）。
void TitleDifficultyMenu::spawn_particles(i32 count) {
    for (i32 i = 0; i < count; i++) {
        if (!animations.create(resources.ascii, resources.difficulty_particle, -1, 2)) {
            check(false);
            return;
        }
    }
}

// [窗B f490-618 绘制序] 背景左半 90 与难度行条 83/84 重挂主链尾（原版
// supervisor 在难度构建时把 90 的层 23→22 并重挂；移植侧用链尾重挂同构）。
// 粒子为 0x46f4a0 头插，随后由 requeue_particles_to_back 整块移到背景之后，
// 形成 oracle 的 L22 桶序：91/93 → 粒子（新→旧）→ 90 → 83/84。
void TitleDifficultyMenu::reorder_difficulty_draws() {
    if (auto* bg = animations.registry.find(state.handles[89])) {
        if (auto* left = animations.registry.find_child_script(*bg, 90, 0))
            animations.registry.requeue_to_back(*left);
    }
    const i32 tree = progress.difficulty >= 4 ? resources.difficulty_tree_extra
                                              : resources.difficulty_tree;
    if (auto* root = animations.registry.find(state.handles[tree])) {
        for (const i32 script : {83, 84}) {
            if (auto* shape = animations.registry.find_child_script(*root, script, 0))
                animations.registry.requeue_to_back(*shape);
        }
    }
}

void TitleDifficultyMenu::advance_choreography() {
    const i32 f = frames;
    if (f < 0) return;
    // [oracle 修正] 光标 119：x 恒根中（oracle 全窗中心 x=319.5=根锚 320 半像素
    // 舍入），y 自驱下滑 185.5→277.5（f490-f505，减速）——即 119 的脚本
    // label 序列动画，**无 x 平移、无 supervisor 缓动**。旧 kCursorEase
    // 十九帧 x 序列 + translation 每帧写是错读（把行条 83/84 的滑入当成了
    // 光标轨迹），产生「徽记错位条」（实测 port L26 条中心随 ease 漂移而
    // oracle 光标 x 全窗恒定）。光标 VM 现按 dump s119 自驱（ins_5(3) 入场
    // 序即 y 滚动），移植侧不再写 translation/position。
    // 徽记下滚 114 同废 poke：dump s114 自带 label 9 的 30f 插值（y→-256
    // 相对条），oracle 下滚条 y 254.5→292 由脚本驱动；旧两点折线 poke 与
    // ins_407 插值互相覆写（探针实证 interp_pos=15 活跃时 poke 无效）。
    // 白条衰减（64-7f/帧，f8 归零销毁；oracle f498=L25 批本消失）与选中白化
    // 衰减（204-13f，f16 归零隐藏；oracle f506=title01 全消）保留。
    // [窗B 顶点色逐帧序列] 非选中行 alpha（oracle f490-f497）：
    //   Extra 行（item1）63,56,48,40,31,23,16,8；其余 8 行 101,89,76,63,50,
    //   38,25,12。选中行（f490-f505）203,191,178,165,152,140,127,114,101,89,
    //   76,63,50,38,25,12。序列在 finish_menu_hides 已中和脚本插值后由本函数
    //   逐帧落色（菜单 update 早于管理器 tick，必须关插值才不被覆写）。
    // [反证修正] 循环覆盖全部 10 项并跳过选中项：confirmed_entry != 0
    // （練習=2 / Extra=1）时 item 0 的白条也是 L25 批次成员，必须同享序列。
    static constexpr u8 kExtraAlpha[8] = {63, 56, 48, 40, 31, 23, 16, 8};
    static constexpr u8 kGreyAlpha[8] = {101, 89, 76, 63, 50, 38, 25, 12};
    static constexpr u8 kSelectedAlpha[16] = {203, 191, 178, 165, 152, 140, 127,
                                              114, 101, 89,  76,  63,  50,  38,
                                              25,  12};
    if (auto* menu_root = animations.registry.find(state.handles[0])) {
        for (i32 item = 0; item < kTitleMainEntries && f <= 8; item++) {
            if (item == state.confirmed_entry) continue;
            auto* child = animations.registry.find_child_script(*menu_root, item + 13, 0);
            if (!child) continue;
            if (f == 8) {
                // [窗B f498 汇合] 非选中行序列 f0-7 衰减到 8/12，f8 归零隐藏
                // （oracle f498 起无 L25 批，仅选中 L26 0x65ffffff）。
                child->visible = false;
                child->visual.flags &= ~1u;
                continue;
            }
            const u32 alpha = item == 1 ? kExtraAlpha[f] : kGreyAlpha[f];
            child->visual.color = alpha << 24 | (child->visual.color & 0xffffffu);
        }
        // 选中高亮 = 灰条本体 3+k（L26，sprite0 左半）；白条 13+k 已在
        // finish_menu_hides 隐藏。
        if (auto* selected =
                animations.registry.find_child_script(*menu_root, state.confirmed_entry + 3, 0)) {
            if (f < 16) {
                selected->visual.color =
                    u32(kSelectedAlpha[f]) << 24 | (selected->visual.color & 0xffffffu);
            } else if (f == 16) {
                selected->visible = false;
                selected->visual.flags &= ~1u;  // oracle f506：title01 全消
            }
        }
    }
    // [窗B 插入帧实证 f490-f491] oracle 建面当帧（f0=Z+21）插入 16 枚、f491
    // 首绘 16；此后每 3 帧 1 枚（f491,494,... = 难度帧 f1,4,...）。移植侧
    // update 早于 manager tick，故 f1 落 burst 16（f0 不绘）、相位 f≡1 (mod 3)。
    // 相位交给 TitleState::particle_clock 供角色屏接力（oracle 全窗不断，
    // f749 仍插）。
    if (f == 1) spawn_particles(16);
    else if (f > 1 && (f - 1) % 3 == 0) spawn_particles(1);
    state.particle_clock = u32(f + 1);
    // 头插粒子先落到链首；再整块挪到背景 91/93 之后（保持新→旧相对序），
    // 最后由 reorder_difficulty_draws 把 90/83/84 重挂链尾。
    requeue_particles_to_back(animations, resources.ascii, resources.difficulty_particle);
    reorder_difficulty_draws();
    // oracle f517（Z+48）：select00 右半自 L21 搬 L22（左半留 L21 = 层分裂）。
    // VM 层保持脚本真值；绘制侧覆盖按 particle_clock（f=27 → clock 28）落
    // 93 的 L22（TitleMainMenu::apply_draw_layers）。
}

void TitleDifficultyMenu::retire_difficulty() {
    // root123 的 label1 已在 Esc 按键帧 pending 并清根槽（0x451255-0x451263）；
    // 此处不重发 root 退场，只收 Rank Select 徽记。背景半张层偏移由
    // apply_draw_layers 按 screen 复位（回主菜单 90=L23/93=L21）。
    visuals.retire(resources.difficulty_badge);
}

void TitleDifficultyMenu::choose_character() noexcept {
    state.change_screen(TitleScreen::Character);
    // [窗B 粒子相位] 转屏帧难度 update 已推进 particle_clock，但角色屏本帧
    // 不 update——补一拍使 clock 与全局帧同步（否则角色屏粒子相位晚 1 帧）。
    ++state.particle_clock;
    // LAB_00451363（0x450fb0 case3 收场）：be7c4<4 时才取光标为难度；
    // Extra(4) 保持 4（bb4d0 已被 0x44cd10 case1 置 4）。
    if (progress.difficulty < 4) {
        settings.configured_difficulty = state.menu.cursor;
        progress.difficulty = state.menu.cursor;
    }
    state.menu.push();
    state.menu.wrapping = true;
    state.menu.count = kTitleCharacterRows;
    // 0x451450 uses the cursor directly as the character index.
    state.menu.select(settings.preferred_character + kTitleCharacterFirst);
    progress.character = settings.preferred_character;
}

void TitleDifficultyMenu::return_main() {
    // No abandoned Practice selection may leak into a subsequent ordinary run.
    progress.run_mode_flags &= ~0x30u;
    progress.practice_mode = 0;
    state.change_screen(TitleScreen::Main);
    state.menu.pop();
}

bool TitleDifficultyMenu::update(u32 pressed, u32 repeated) {
    switch (state.substate) {
    case 0:
        state.menu.wrapping = false;
        // 0x450fb0 case0：count = ((bb4d0>=4)-1 & 3)+1 → 难度 0..3 为 4 行
        //（EASY..LUNATIC；原版无“やめる”行，取消走 X/Esc → 0x44bb20(4)）；
        // Extra(4) 为 1（单行 EXTRA，根 124）。
        state.menu.count = ((i32(progress.difficulty >= 4) - 1) & 3) + 1;
        frames = 0;
        if (!build_difficulty()) return false;
        finish_menu_hides();
        state.change_substate(1);
        [[fallthrough]];
    case 1:
    case 2:
    case 3:
    case 4:
        advance_choreography();
        frames++;
        break;
    }
    switch (state.substate) {
    case 1:
        if (state.age.current > 6) state.change_substate(2);
        break;
    case 2:
        state.menu.previous = state.menu.cursor;
        // 0x450fb0 case2：↑↓ 导航只在普通难度（be7c4<4）；Extra 单行态无
        // 导航（count=1，光标恒 0）。
        if (progress.difficulty < 4) {
            if ((pressed | repeated) & 0x10) state.menu.move(-1);  // ↑=0x10
            if ((pressed | repeated) & 0x20) state.menu.move(1);   // ↓=0x20
            if (!state.menu.error.empty()) return check(false);
            if (state.menu.previous != state.menu.cursor) {
                // 0x450fb0 case2：sound10 → root label3(immediate) → root label7+new
                // (pending)。label3 只存在于行脚本（5f α→0），83/84 无 3 保持旋转。
                if (!sound(10)) return false;
                if (!check(visuals.interrupt(resources.difficulty_tree, 3, true))) return false;
                if (!check(visuals.interrupt(resources.difficulty_tree, 7 + state.menu.cursor, false)))
                    return false;
            }
        }
        if (pressed & 0x102) {  // Esc/X：回主菜单
            // 0x451237-0x451263：当帧 root pending label1 并清根槽（不是 6 帧后
            // 才 retire）；其余退场由脚本 label1 自行播完。Extra 态根为 124。
            const i32 tree = progress.difficulty >= 4 ? resources.difficulty_tree_extra
                                                      : resources.difficulty_tree;
            if (!check(visuals.interrupt(tree, 1, false))) return false;
            state.handles[tree] = 0;
            state.change_substate(4);
            return sound(9);
        }
        if (pressed & 0x80001) {  // Z/Enter
            // 0x451283-0x4512c2：root pending6（行/黑片退场）；难度<4 时取
            // root123 子 113+cursor（亮行）再 pending2（确认缩放），子标签覆盖6。
            // Extra 态（be7c4>=4）原版走 else 分支：root124 子 0x75(117)
            // interrupt（EXTRA 行确认高亮）；root124 子构成未 dump，高亮暂缺，
            // 不影响路由/难度语义（登记蓝图 TODO）。
            const i32 tree = progress.difficulty >= 4 ? resources.difficulty_tree_extra
                                                      : resources.difficulty_tree;
            if (!check(visuals.interrupt(tree, 6, false))) return false;
            if (progress.difficulty < 4 && state.menu.cursor < 4) {
                auto* tree_vm = animations.registry.find(state.handles[tree]);
                if (!tree_vm) return check(false);
                auto* row = animations.registry.find_child_script(
                    *tree_vm, 113 + state.menu.cursor, 0);  // 0x4512ae: edi += 0x71
                if (!row) return check(false);
                if (!check(visuals.handle_interrupt(animations.registry.handle(*row), 2, false)))
                    return false;
            }
            // Rank Select 101 同帧退场（oracle 窗B f603-621 滑出，102 f622 起入场）。
            if (!check(visuals.retire(resources.difficulty_badge))) return false;
            state.change_substate(3);
            return sound(7);
        }
        break;
    case 3:
        if (state.age.current > 13) choose_character();
        break;
    case 4:
        if (state.age.current > 5) {
            // 0x451403-0x45141c / 0x450fb0 case4：Esc 把难度写回——普通难度取
            // 光标（bb4d0=+0x28）；Extra 取 +0x59b0 暂存（取消 Extra 恢复原
            // 难度，be7c4=bb4d0）。转场回主菜单。
            if (progress.difficulty < 4) {
                settings.configured_difficulty = state.menu.cursor;
                progress.difficulty = state.menu.cursor;
            } else {
                settings.configured_difficulty = settings.saved_difficulty;
                progress.difficulty = settings.saved_difficulty;
            }
            retire_difficulty();
            return_main();
        }
        break;
    }
    return true;
}

bool TitleCharacterMenu::check(bool ok, const char* reason) {
    if (!ok && error.empty()) error = reason;
    return ok;
}

bool TitleCharacterMenu::visual(bool ok) {
    if (!ok && error.empty()) error = visuals.error;
    return ok;
}

// [残留修复] 0x451450 case2 逐指令（0x4519b7-0x451a57）：方向键 → sound(10) →
// 0x46fca0(root, 退场标签)【immediate：写 pending 后当场 0x462450 重绕起播】→
// 0x473210(±1) 推进选角 → 0x46fc30(root, 入场标签)【仅 pending，子脚本在下一个
// 等待点消费】。左=退 0x16+old/入 0x0c+new，右=退 0x11+old/入 0x07+new
// （dump 实证 s125：7=自 x272 右入、12=自 x16 左入、17=alpha→0 出向 x16、
// 22=alpha→0 出向 x272）。旧实现退/入都用 pending 且同帧发出：入场广播把
// 旧立绘的退场 pending 覆盖，退场标签永不消费 → 上一角色立绘残留叠屏。
// 传播面：原版 0x46fca0/fc30 把标签写到 root 全部直接子（125-128 立绘/
// 135-138 武器/131-134 选择框，dump 实证三者标签集相同 7+c/12+c/17+c/22+c/3/6/1），
// 未通关选择框靠 vm+0x18 bit1 游戏抑制位（0x470030）保持不绘；
// 中断仍传给所有子 VM，脚本状态照常推进。绘制器要求 flags 两低位均置位。
bool TitleCharacterMenu::character_interrupt(i32 character, i32 label, bool immediate) {
    // 0x451450 case0：Extra 屏根 130，武器 139+（weapon_base_ex）；立绘/
    // 选择框标签族同号共用。
    const bool extra = progress.difficulty == 4;
    const i32 root = extra ? resources.char_menu_root_ex : resources.char_menu_root;
    const i32 full = label + character;  // 立绘/武器/选择框同号标签族
    return visual(visuals.interrupt(root, full, immediate));
}

bool TitleCharacterMenu::exit_portrait(i32 character, i32 label) {
    // 退场必须 immediate（0x46fca0）：退场动画当帧起播，不被随后同帧的入场
    // pending 覆盖（入场只写 pending，落点在子脚本下一个等待点）。
    return character_interrupt(character, label, true);
}

bool TitleCharacterMenu::enter_portrait(i32 character, i32 label) {
    return character_interrupt(character, label, false);
}

void TitleCharacterMenu::return_difficulty() {
    // Player Select 标题条 102 退场（oracle：难度屏 101 在 f603-621 同款 20f
    // 滑出后自机屏建 102；本屏回退走 retire=label1 淡出，帧级时序待窗对拍）。
    // 0x451450 case4 sends label 1 to the active 129/130 root.
    visuals.retire(resources.difficulty_badge + 1);
    visuals.retire(progress.difficulty == 4 ? resources.char_menu_root_ex
                                            : resources.char_menu_root);
    visuals.retire(resources.character_root);
    state.change_screen(TitleScreen::Difficulty);
    if (state.menu.cursor >= kTitleCharacterFirst && state.menu.cursor <= kTitleCharacterLast)
        settings.preferred_character = title_character_at(state.menu.cursor);
    state.menu.pop();
}

bool TitleCharacterMenu::update(u32 pressed, u32 repeated) {
    // [窗B 粒子接力] 神霊粒子在角色屏继续按 3 帧相位出生（oracle 主池
    // 0x46f4a0 头插 #18@L22 从 f491 连续到 f749，窗B 共 102 枚；难度屏
    // 停更后不得断流）。相位沿用难度屏 clock=f+1（难度帧 f≡1 mod 3 出生
    // ⇔ clock≡2 mod 3）。ascii bank 未装载的隔离宿主（单模块 UI 测试）
    // 跳过出生，不伪造资源。ordering=2 = 原版头插。
    if (state.particle_clock % 3 == 2 && animations.resource(resources.ascii)) {
        if (!animations.create(resources.ascii, resources.difficulty_particle, -1, 2))
            return check(false, "Character particle spawn failed");
        // 头插后把粒子整块移到角色屏既有 VM 之后（L22 桶序：背景 → 粒子）。
        requeue_particles_to_back(animations, resources.ascii,
                                  resources.difficulty_particle);
    }
    ++state.particle_clock;
    // 0x451450 case0：根动画 = (be7c4==4)+0x81 → 普通 129 / Extra 130
    //（EX 武器 139-142；TitleSceneResources.char_menu_root_ex）。
    const i32 root = progress.difficulty == 4 ? resources.char_menu_root_ex
                                              : resources.character_root;
    switch (state.substate) {
    case 0: {
        state.menu.count = kTitleCharacterRows;
        // 0x451450 case0：Extra 态（be7c4==4）角色级解锁门（0x44bec0 读 0x1b26c 族 bit0x10）。
        // 未解锁角色调用 FUN_004417c0（MenuCursor::disable 对应物）禁用对应行（kTitleCharacterFirst + c）；
        // 若当前选角未解锁，光标重定向到第一个解锁角色。
        if (progress.difficulty == 4) {
            state.menu.disabled_count = 0;
            i32 first_unlocked = -1;
            for (i32 c = 0; c < 4; c++) {
                if (host.extra_unlocked(c)) {
                    if (first_unlocked < 0) first_unlocked = c;
                } else {
                    state.menu.disable(kTitleCharacterFirst + c);
                }
            }
            const i32 cur_char = title_character_at(state.menu.cursor);
            if (cur_char >= 0 && cur_char < 4 && !host.extra_unlocked(cur_char)) {
                if (first_unlocked >= 0) {
                    state.menu.select(kTitleCharacterFirst + first_unlocked);
                }
            }
        }
        // [oracle 自机页构成定案] 真机自机页零 title01 op（title-draws-b
        // f650-749 全段实证）——root script2 名行树（spawn 49-82：霊夢..やめる
        // 行条 + 段位扇形量表 59-82）属 PlayerData 段位屏，自机页**不建**。
        // 旧 create(2)+interrupt(2,7+k) 是错建：名行条入场后以 title01
        // 行位图（家内安全..早苗行区）绘在自机页左下——stable 截图像素匹配
        // 与 port draws L25×28/L26×12 双证。光标按原版四个角色建模。
        if (state.handles[root]) visuals.retire(root);
        // 0x451450 case0：自机菜单根 = (be7c4==4)+0x81 → 129（普通）/130
        //（EX 武器 139-142）；root（character_root=2 名行树）只做上方清理。
        const i32 menu_root = progress.difficulty == 4 ? resources.char_menu_root_ex
                                                       : resources.char_menu_root;
        if (!visual(visuals.retire(menu_root)) || !visual(visuals.create(menu_root)))
            return false;
        // 45154a broadcasts label3 immediately to the newly created tree,
        // before 45155d queues the selected character's label7+cursor.
        if (!visual(visuals.interrupt(menu_root, 3, true))) return false;
        // Player Select 标题条（select01 sprite80，dump L24293 与 101 同体：
        // (-256,8)→(32,8) 20f 滑入）：自机屏独占（oracle 窗B f622 起实证，
        // 难度屏期间不建）。
        if (!visual(visuals.create(resources.difficulty_badge + 1))) return false;
        // [r4-M] clear-tag 可见门（0x45145f-0x451734 四路展开判定：score_data
        // +0x56dc*c+0x7e4+DAT_004be7c4(当前难度)*4 == 0 → 00470030 递归清
        // script 0x83+c 子树可见位）。缺关（子不存在）原版为空指针 no-op。
        // 00470030 clears bit1 while retaining script execution and bit0.
        if (AnmVm* menu = animations.registry.find(state.handles[menu_root])) {
            const std::function<void(AnmVm&)> hide_tree = [&](AnmVm& vm) {
                vm.visual.flags &= ~2u;
                for (AnmVm* child : animations.registry.children(vm)) hide_tree(*child);
            };
            for (i32 c = 0; c < 4; c++) {
                if (host.character_cleared(c, progress.difficulty)) continue;
                if (AnmVm* tag = animations.registry.find_child_script(
                        *menu, resources.clear_tag_base + c, 0))
                    hide_tree(*tag);
            }
        }
        if (!enter_portrait(title_character_at(state.menu.cursor), 7)) return false;
        state.change_substate(1);
        [[fallthrough]];
    }
    case 1:
        if (state.age.current > 6) state.change_substate(2);
        break;
    case 2: {
        state.menu.previous = state.menu.cursor;
        if ((pressed | repeated) & 0x40) {  // ←：上一行
            if (!check(host.sound(10), "Character navigation sound failed")) return false;
            // 0x4519c1：退场标签读活动选角（同帧 ←→ 时为中间值），非帧初快照。
            if (!exit_portrait(title_character_at(state.menu.cursor), 22)) return false;
            state.menu.move(-1);
            if (!state.menu.error.empty()) return check(false, state.menu.error.c_str());
            if (!enter_portrait(title_character_at(state.menu.cursor), 12)) return false;
        }
        if ((pressed | repeated) & 0x80) {  // →：下一行
            if (!check(host.sound(10), "Character navigation sound failed")) return false;
            if (!exit_portrait(title_character_at(state.menu.cursor), 17)) return false;
            state.menu.move(1);
            if (!state.menu.error.empty()) return check(false, state.menu.error.c_str());
            if (!enter_portrait(title_character_at(state.menu.cursor), 7)) return false;
        }
        if (pressed & 0x102) {  // Esc/X：回难度菜单
            state.change_substate(4);
            return check(host.sound(9), "Character cancel sound failed");
        }
        if (pressed & 0x80001) {  // Z/Enter
            // 确认：立绘/武器/选择框中央淡出（label 3）。名行树不建（构成定案，
            // 旧 interrupt(2,6) 名行闪灭随之废除）。
            if (!enter_portrait(title_character_at(state.menu.cursor), 3)) return false;
            if (!check(host.sound(7), "Character confirm sound failed")) return false;
            state.change_substate(3);
        }
        break;
    }
    case 3:
        if (state.age.current == 10) {
            // 0x451450: age10, Practice (be830 & 0x30 != 0) goes to screen8;
            // only the ordinary path starts the game transition here.
            if ((progress.run_mode_flags & 0x30u) == 0x10u) {
                progress.character = settings.preferred_character =
                    title_character_at(state.menu.cursor);
                state.menu.push();
                state.menu.count = 6;
                state.menu.wrapping = true;
                state.menu.select(settings.practice_stage);
                if (!visual(visuals.retire(resources.difficulty_badge + 1)) ||
                    !visual(visuals.retire(resources.char_menu_root))) return false;
                state.change_screen(TitleScreen::Stage);
                return true;
            }
            // 00451450 case3 age10：FUN_00417370 只创建转场 VM（无 label 7
            // 中断；effect.anm script1 无该标签）。
            if (!check(host.begin_transition(state.start_effect),
                       "Game start transition unavailable"))
                return false;
        }
        if (state.age.current > 39) {
            progress.character = settings.preferred_character =
                title_character_at(state.menu.cursor);
            state.menu.push();
            state.change_screen(TitleScreen::StartGame);
            // Player Select 条 102 退场（对局转场；oracle 窗B 开局序列同）。
            visuals.retire(resources.difficulty_badge + 1);
            // 0x451450 case4 尾：be7c4==4 → DAT_004be81c=7（stage 表行 7 =
            // Extra，表基 0x4bb854）；普通路径 stage 表行 1（DAT_004bb4f0）。
            // 对局侧 st07 装载属 Stage 批次，此处只保证菜单路由忠实。
            progress.stage = progress.difficulty == 4 ? 7 : 1;
            progress.starting_stage = progress.stage;
            progress.practice_mode = 0;
            if (!check(host.start_game(progress.stage), "Game scene destination failed")) return false;
        }
        break;
    case 4:
        if (state.age.current > 5) return_difficulty();
        break;
    }
    return true;
}

bool TitleStageMenu::check(bool ok, const char* reason) {
    if (!ok && error.empty()) error = reason;
    return ok;
}

bool TitleStageMenu::available(i32 stage) {
    return host.practice_stage_played(progress.character, progress.difficulty, stage);
}

bool TitleStageMenu::update(u32 pressed, u32 repeated) {
    switch (state.substate) {
    case 0:
        state.menu.count = 6;
        state.menu.wrapping = true;
        state.menu.select(settings.practice_stage);
        // On a return from gameplay the title scene has no persistent backdrop.
        if (!animations.registry.find(state.handles[89])) {
            if (!check(visuals.create(89) && visuals.interrupt(89, 3, true),
                       "Practice return background unavailable")) return false;
        }
        // Original handles +0x47c / +0x488 are title scripts 109 / 112.
        if (!check(visuals.create(109) && visuals.create(112),
                   "Practice stage animations unavailable")) return false;
        state.change_substate(1);
        [[fallthrough]];
    case 1:
        if (state.age.current > 10) state.change_substate(2);
        break;
    case 2:
        state.menu.previous = state.menu.cursor;
        if ((pressed | repeated) & 0x10) state.menu.move(-1);
        if ((pressed | repeated) & 0x20) state.menu.move(1);
        if (!check(state.menu.error.empty(), "Practice stage cursor failed")) return false;
        if (state.menu.previous != state.menu.cursor &&
            !check(host.sound(10), "Practice stage cursor sound failed")) return false;
        if (pressed & 0x102) {
            settings.practice_stage = state.menu.cursor;
            state.change_substate(4);
            return check(host.sound(9), "Practice stage cancel sound failed");
        }
        if (pressed & 0x80001) {
            // 0x451cb0 rejects an unplayed stage with sound16, without leaving.
            if (!available(state.menu.cursor + 1))
                return check(host.sound(16), "Locked practice stage sound failed");
            settings.practice_stage = state.menu.cursor;
            state.change_substate(3);
            return check(host.sound(7) && host.sound(50), "Practice stage confirm sound failed");
        }
        break;
    case 3:
        if (state.age.current == 10) {
            // 00451cb0 case3 age10：同 00417370，仅创建（无 label 7）。
            if (!check(host.begin_transition(state.start_effect),
                       "Practice start transition unavailable")) return false;
        }
        if (state.age.current > 39) {
            progress.stage = progress.starting_stage = state.menu.cursor + 1;
            progress.practice_mode = 1;
            progress.run_mode_flags = (progress.run_mode_flags & ~0x30u) | 0x10u;
            state.menu.push();
            state.change_screen(TitleScreen::StartGame);
            return check(host.start_game(progress.stage), "Practice game destination failed");
        }
        break;
    case 4:
        if (state.age.current > 5) {
            if (!check(visuals.retire(109) && visuals.retire(112),
                       "Practice stage animation exit failed")) return false;
            state.menu.pop();
            state.change_screen(TitleScreen::Character);
        }
        break;
    }
    return true;
}

bool TitleStageMenu::draw_texts(HudDrawServices& host) {
    if (state.screen != TitleScreen::Stage ||
        (state.substate != 2 && state.substate != 3)) return true;
    // 0x452170: (296,216), row step18; locked selected=df, available=yellow.
    // Stage best scores are not stored by this port yet: omit, don't invent them.
    for (i32 row = 0; row < 6; ++row) {
        const bool played = available(row + 1);
        HudTextStyle style;
        style.color = row != state.menu.cursor ? 0xff808080u :
                      !played ? 0xffdfdfdfu :
                      state.substate == 3 && state.age.current % 4 >= 2 ?
                          0xff000000u : 0xffffff00u;
        const std::string text = "Stage " + std::to_string(row + 1) +
            (played ? "" : "  ---------");
        if (!check(host.hud_text({{296, float(216 + row * 18), 0}, style, text}),
                   "Practice stage text failed")) return false;
    }
    return true;
}

}  // namespace th13
