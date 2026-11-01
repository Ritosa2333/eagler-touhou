#include "AnmRenderer.hpp"
#include "TitleMainMenu.hpp"

namespace th13 {

bool TitleMainMenu::fail(const std::string& reason) {
    if (error.empty()) error = reason;
    return false;
}

bool TitleMainMenu::present(u32& handle) {
    if (animations.registry.find(handle)) return true;
    handle = 0;
    return false;
}

bool TitleMainMenu::create(i32 script) {
    state.handles[script] = animations.create(resources.title, script, -1, 0);
    return state.handles[script] || fail(animations.error);
}

bool TitleMainMenu::create_at(i32 script, const Vec3& translation) {
    // 逐实例平移覆盖（行条 y 步距 48px 由 golden 难度帧行位拟合，蓝图 §1.7）。
    state.handles[script] = animations.create(resources.title, script, -1, 2, translation);
    return state.handles[script] || fail(animations.error);
}

bool TitleMainMenu::interrupt(i32 script, i32 label, bool immediate) {
    return animations.interrupt(state.handles[script], label, immediate) || fail(animations.error);
}

bool TitleMainMenu::child_interrupt(i32 script, i32 label, bool immediate) {
    auto* root = animations.registry.find(state.handles[resources.main_root]);
    if (!root) {
        state.handles[resources.main_root] = 0;
        return fail("Title menu root unavailable");
    }
    auto* child = animations.registry.find_child_script(*root, script, 0);
    if (!child) return fail("Title menu child unavailable");
    child->pending_interrupt = label;
    return !immediate || animations.tick_instance(*child) >= 0 || fail(animations.error);
}

bool TitleMainMenu::sound(i32 id) {
    return play_sound ? play_sound(id) || fail("Title sound request failed")
                      : fail("Title sound service unavailable");
}

// 0044cd10 sends labels 30/31 to rows on either side of the cursor,
// executing each immediately. Locked Extra uses label29 on both rows.
bool TitleMainMenu::show_menu() {
    for (i32 item = 0; item < kTitleMainEntries; ++item) {
        if (item == state.menu.cursor) continue;
        const i32 label = item < state.menu.cursor ? 30 : 31;
        if (!child_interrupt(item + 3, label, true) ||
            !child_interrupt(item + 13, label, true)) return false;
    }
    if (!extra_gate()) {
        if (!child_interrupt(4, 29, false) || !child_interrupt(14, 29, false)) return false;
    }
    return true;
}

// Extra 行解锁门（0x44c570 case0：+0x5c40 bit1 非 0 = 解锁；写点 0x416130
// 通关落档 0x1b26b+char*2 bit0x10，任一角色普通难度通关置位）。宿主未接 =
// 禁用（原版无存档行为）。0x416150 无全角色判定——任一角色位即开。
bool TitleMainMenu::extra_gate() {
    for (i32 character = 0; character < 4; ++character)
        if (extra_unlocked && extra_unlocked(character)) return true;
    return false;
}

void TitleMainMenu::enter(i32 entry) {
    switch (TitleMainEntry(entry)) {
    case TitleMainEntry::StartGame:
    case TitleMainEntry::PracticeStart:
        // 0x44da63 / 0x44dac8 -> 0x449360: Start=0, Practice=1 << 4.
        // Extra also uses mode 0 (difficulty 4); it is not Practice mode 1.
        progress.run_mode_flags = (progress.run_mode_flags & ~0x30u) |
            (entry == i32(TitleMainEntry::PracticeStart) ? 0x10u : 0u);
        progress.practice_mode = 0;  // GameInf is configured only on launch.
        // 0x44cd10 case4 case0/2：上次配置为 Extra（bb4d0>3）时普通开局重置
        // Normal（bb4d0=1, be7c4=1），光标不落在不存在的第 5 行。
        if (settings.configured_difficulty > 3) settings.configured_difficulty = 1;
        state.flags |= 2u;  // 离开主菜单：返回时不再走 180 帧 intro
        state.change_screen(TitleScreen::Difficulty);
        state.menu.push();
        settings.saved_difficulty = progress.difficulty;
        // 0x450fb0：Rank Select 光标项数 = 4（EASY..LUNATIC；extra 分支另计），
        // 原版无“やめる”行——旧 5 项/やめる行为废除。
        state.menu.count = 4;
        state.menu.select(settings.configured_difficulty);
        break;
    case TitleMainEntry::ExtraStart:
        // 0x44cd10 case4 case1（Extra 開始，20 帧转场后）：FUN_00449360
        //（mode=0，同 Start）+ 屏5（难度屏）+ 0x473190（光标 push）+
        // +0x59b0=be7c4（暂存当前难度）+ DAT_004bb4d0=4 + DAT_004be7c4=4
        //（难度强制 Extra）+ 0x415030(0)（光标归 0）。难度屏在 Extra 态为
        // 单行 EXTRA（见 TitleDifficultyMenu，0x450fb0 case0 count 公式）；
        // Esc 取消按 0x59b0 恢复原难度。解锁门在 update 侧
        //（0x44c570 case0 +0x5c40 bit1），不可达时本 case 不会被确认。
        progress.run_mode_flags &= ~0x30u;
        progress.practice_mode = 0;
        settings.saved_difficulty = progress.difficulty;  // +0x59b0 暂存
        progress.difficulty = 4;
        state.flags |= 2u;
        state.change_screen(TitleScreen::Difficulty);
        state.menu.push();
        state.menu.count = 4;
        state.menu.select(0);
        break;
    case TitleMainEntry::Replay:
        // 0x44c570 case0 iVar7==2 进 Replay 屏（0x44e2xx）：+0x30=10、cursor=4
        // （返回定位）、光标 push、screen=0xb。flags|=2 同开局路径（对局/回放
        // 返回不再走 180 帧 intro）。
        state.flags |= 2u;
        state.menu.push();
        state.menu.count = kTitleMainEntries;
        state.menu.select(i32(TitleMainEntry::Replay));
        state.change_screen(TitleScreen::Replay);
        break;
    case TitleMainEntry::PlayerData:
        // 0x44c570 case a → 0x453170（屏值 10）：记录浏览对合。光标 push 供
        // 收面 pop 恢复主菜单光标；数据表/解锁全部由宿主
        // TitlePlayerDataPlatform 提供（默认失败，不造记录）。
        state.flags |= 2u;
        state.menu.push();
        state.menu.count = kTitleMainEntries;
        state.menu.select(i32(TitleMainEntry::PlayerData));
        state.change_screen(TitleScreen::PlayerData);
        break;
    case TitleMainEntry::SpellPractice:
        // 0x44c570 case 0x11/0x12/0x13 → 0x4560a0/0x456650/0x4576c0
        //（屏值 17）：三级符卡练习菜单。光标 push 供 Cancel 回主菜单时 pop；
        // 解锁/符卡表/启动路由归宿主 TitleSpellPracticePlatform。
        state.flags |= 2u;
        state.menu.push();
        state.menu.count = kTitleMainEntries;
        state.menu.select(i32(TitleMainEntry::SpellPractice));
        state.change_screen(TitleScreen::SpellPractice);
        break;
    case TitleMainEntry::Quit:
        state.change_screen(TitleScreen::Quit);
        break;
    case TitleMainEntry::Option:
        // 0x44c570 case 3 → 0x44dd00（Option 屏，屏值 3）。光标栈 push/pop 与
        // 行光标归 0 由 TitleOptionsMenu::open/leave 自管对合（0x73190/0x731d0）；
        // flags|=2 同 Replay（离主菜单返回不再走 180 帧 intro）。
        state.flags |= 2u;
        state.change_screen(TitleScreen::Option);
        break;
    case TitleMainEntry::MusicRoom:
        // 0x44c570 case d → 0x4554f0（音楽室，屏值 13）。光标栈 push/曲数入
        // count/光标归 0 全由 TitleMusicRoomMenu case0 自管（depth==0 兜底即
        // 主菜单进入路径）；case3 leave() pop 还原 + 恢复标题曲 th13_00。
        state.flags |= 2u;
        state.change_screen(TitleScreen::MusicRoom);
        break;
    case TitleMainEntry::Manual:
        // 0x44c570 case 10 → 0x455330（マニュアル，屏值 16）。supervisor 不
        // 消费输入：查看器开面/轮询/收面全经 TitleManualPlatform（真实 help
        // viewer 宿主未接 = open 如实失败）；case1 结束位 → 回 Main。
        state.flags |= 2u;
        state.change_screen(TitleScreen::Manual);
        break;
    default:
        // Unmapped entries keep the native confirmation path without inventing a screen.
        break;
    }
}

bool TitleMainMenu::update(u32 pressed, u32 repeated) {
    ++menu_age;  // 单调屏龄（substate 切换会重置 state.age，渐入/闪场用此）
    switch (state.substate) {
    case 0: {
        state.menu.count = kTitleMainEntries;
        state.menu.wrapping = true;
        state.menu.disabled_count = 0;  // Rebuild per-screen gates, not duplicate on each cancel.
        // Extra Start 门（0x44c570 case0 +0x5c40 bit1）：无解锁存档禁用
        //（真机缺省行为，oracle ↓ 0→2 跳过）。
        if (!extra_gate() && !state.menu.disable(i32(TitleMainEntry::ExtraStart)))
            return fail(state.menu.error);
        // 真机 boot：无提示幕。标题任务使能帧（=g181）建背景+LOGO 根（oracle：
        // g181 起 select00 两半、g291 LOGO 翻转 = script94 内建 110f 延迟、
        // g350 森林 = script90/91 内建 170f）。
        // 0x44cdb9-0x44cdd6：返回路径重新创建 89（背景）与 94（LOGO）根。
        // 原版 0x44bb80 只创建新 VM 并覆盖句柄，不回收旧树——旧树保持最后
        // 可见状态（灵梦/背景立即在场），新树按脚本重新入场。旧树随 title
        // bank 生命周期回收（TitleScene 析构 retire_resource）。
        const bool returning = (state.flags & 2u) != 0;
        if (returning) {
            if (!create(resources.background_root)) return false;
            // 新建 89 的 90/91 仍带脚本内建 ins_6(170) 初延迟（g350 森林）。
            // 返回路径立即发 label2：90/91 label2 = ins_409(60,0,255)，新树
            // 也在 60f 内回到 alpha255（真实 title.anm 窄测：65f 后 90/91
            // alpha=255；无 label2 时仍为 0）。旧树同时保留，65f 窗口内
            // 灵梦不会因重建而消失。
            if (!interrupt(resources.background_root, 2, true)) return false;
            if (!create(resources.logo_root)) return false;
        } else {
            if (!present(state.handles[resources.background_root]) &&
                !create(resources.background_root)) return false;
            if (!present(state.handles[resources.logo_root]) &&
                !create(resources.logo_root)) return false;
        }
        state.change_substate(1);
        if (returning && !open_menu()) return false;
        [[fallthrough]];
    }
    case 1: {
        // 菜单 intro：boot 180 帧后建菜单（oracle g361：root0 + title01 十灰条
        // 入 L26 批次、title_v 版本条 30f 延迟淡入 → g390 可见）；返回路径立即建。
        const bool returning = (state.flags & 2u) != 0;
        // [蓝图 §8.3 / 窗A 实测] g350（age169）：select00 右半 script93 绘制层
        // L23→L21（f348 最后 L23、f349 首 L21）。VM 层保持脚本 ins_304(21)，
        // 偏移在 apply_draw_layers 的绘制侧覆盖里按 age 落码。
        // [wf360 白条定案] oracle 菜单白条 f360=淡入第 3 帧（0x33）→ spawn 于
        // f357=age177（w13 滞后 1 帧=rebind）；f357-359 无 op（低 alpha 帧不可
        // 见）→ 绘制侧压 0 复刻。可交互仍锚 age191（f372，oracle 实测）。
        const i32 menu_at = returning ? 0 : 180;
        if (!returning && state.age.current == menu_at && !open_menu()) return false;
        const i32 ready_at = returning ? kTitleMenuReadyDelay :
            kTitleBootIntroFrames + kTitleMenuReadyDelay;
        if (state.age.current == ready_at) {
            // [轮7] 交叉武装提前一帧（oracle grey3 于 f371 首绘 alpha=0x5b=
            // 中断后第 1 tick——port 中断 tick 才执行标签体、alpha 写在次 tick，
            // 故武装于 190）。白条淡出同帧起（f371=0x8e=142 实测）。
            if (!select_entry()) return false;
        }
        if (state.age.current >= ready_at)
            state.change_substate(2);
        break;
    }
    case 2: {
        state.menu.previous = state.menu.cursor;
        if ((pressed | repeated) & 0x10) state.menu.move(-1);  // ↑=0x10（oracle 位布局）
        if ((pressed | repeated) & 0x20) state.menu.move(1);   // ↓=0x20
        if (!state.menu.error.empty()) return fail(state.menu.error);
        if (state.menu.previous != state.menu.cursor) {
            if (!sound(10)) return false;
            // 0044d5xx: immediate label3 restores all rows; deferred label
            // 7+cursor selects the new row on the next manager update.
            if (!interrupt(resources.main_root, 3, true) ||
                !interrupt(resources.main_root, 7 + i32(i16(state.menu.cursor)), false) ||
                !show_menu()) return false;
        }
        if (pressed & 0x102) {  // Esc/X：跳到末项或直接退出分支
            if (state.menu.cursor == kTitleMainEntries - 1) {
                if (!sound(9)) return false;
                state.change_substate(4);
                return true;
            }
            if (!sound(9)) return false;
            state.menu.select(kTitleMainEntries - 1);
            if (!interrupt(resources.main_root, 3, true) ||
                !interrupt(resources.main_root, i32(i16(state.menu.cursor)) + 7, false) ||
                !show_menu())
                return false;
        }
        if (pressed & 0x80001) {  // Z/Enter：确认
            const i32 selected = state.menu.cursor;
            if (selected < 0 || selected >= kTitleMainEntries) break;
            if (!interrupt(resources.main_root, 6, false)) return false;
            if (selected == i32(TitleMainEntry::Quit)) {
                if (!sound(9)) return false;
                state.change_substate(4);
                break;
            }
            if (!sound(7)) return false;
            // [解剖轮2] Z 确认序（difficulty-menu-anatomy.md §③/⑥ + 顶点色）：
            // 主菜单 VM 经 label6 退场，字条逐步
            // 隐出）——废除轮1 的 interrupt(0,6)/label2。帧级时间线（Z=age0）：
            //   +8  title_ver 末日（age==8 回收，次帧起无 op，oracle f477/478）；
            //   +10 灰条透明度线性衰减 -13/帧（oracle f479 起 0xff→0xf2→0xe5…，
            //       rgb 恒 192、禁用项黑 rgb 不变自 0xa0 起）——无对应标签序列，
            //       supervisor 置色推定，逐帧 poke（入场插值早已结束不覆写）；
            //   +14 LOGO 树回收（title_logo 末日 f483、f484 起无 op）；
            //   +20 enter() 转难度屏（难度内容 f490=Z+21 起在场）；
            //   +21 起的隐藏时间线（非选中灰条隐藏/选中白化续衰减/白条 9 枚闪
            //       8 帧）由难度屏接手（TitleDifficultyMenu，其 age0 = Z+21）。
            state.confirmed_entry = selected;
            state.change_substate(4);
            // [f469 复验] oracle Z 当帧 ver 走 label 1 淡出（f469 alpha 229 =
            // 淡出第 1 帧值）——Z 分支内当帧发 interrupt（immediate=false：
            // pending 由本帧 AnmManager.update 的 tick 消费恰 1 步）。
            if (state.version_bar && selected != kTitleMainEntries - 1)
                animations.interrupt(state.version_bar, 1, false);
            // 44d954 also queues label1 to both logo-root slots; their scripts
            // own their exit timing and lifetime.
            if (!interrupt(resources.logo_root, 1, false)) return false;
            // 其余入口（真机有画面，本轮不实现）：确认序同上，enter() 不切屏
            // ——解剖实证字条隐藏与转屏解耦，难度侧时间线照走。
        }
        break;
    }
    case 4: {
        if (state.age.current == 20 && state.handles[resources.background_root]) {
            // [f489 复验修正] oracle select00 右半亮化：f489 首绘步值 rgb 68
            // （f486-488 恒 64，Z=f469 → Z+20；408(60,0,192) 步值 68 起 +2/帧）。
            // port 的 interrupt(immediate=false) pending 当帧消费落第 1 步 66
            // ——true 值是首绘 68：改 immediate=true（中断即时 tick 至标签体
            // 首步，408 计步同帧起算），与 ver label1 的 off-by-one 同族根因
            // （插值首帧 tick 相位），不挪 age。
            // 仅开局族（開始/練習/Extra）淡出当前背景；子菜单入口原版
            // 0x44c8d4 是「新建 89 + 对新树 label3」，旧树保持可见——
            // 端口无新树，等价做法是不淡出当前（否则返回时灵梦已 alpha0）。
            const i32 selected = state.menu.cursor;
            const bool starts_game = selected == i32(TitleMainEntry::StartGame) ||
                selected == i32(TitleMainEntry::PracticeStart) ||
                selected == i32(TitleMainEntry::ExtraStart);
            if (starts_game)
                animations.interrupt(state.handles[resources.background_root], 3, true);
        }
        if (state.age.current > 19) enter(state.menu.cursor);
        break;
    }
    }
    return true;
}

// [蓝图 §8.3/§8.9] 绘制侧层偏移（原版 supervisor 逐 VM 置层，机制未反汇编；
// VM dump 层恒为脚本 ins_304 真值 22/21/21/21 与 LOGO 25）。本函数不改 VM
// 层（title_background_layer_pokes_per_oracle 回归守护），只在 Draw 优先级 2
// 写 op 层覆盖：
//   背景 90/91/93：主菜单 90=L23、91=L22、93=L23（g350 age169 后 93→L21）；
//     难度/角色屏 90=91=93=L22（难度龄 27 后 93 自 L21 搬 L22）。
//   LOGO 97 翻转窗（age<=60）L26、98 爆闪全命 L26。
// 桶序仍按脚本层（与 oracle 任务层相对序一致），故绘制顺序不变。
void TitleMainMenu::apply_draw_layers() {
    clear_draw_layer_overrides();
    if (auto* bg = animations.registry.find(state.handles[resources.background_root])) {
        // 难度屏自难度根建面帧起算（f489 转屏帧尚未建树 = 主菜单语义）。
        const bool selecting =
            state.handles[resources.difficulty_tree] != 0 ||
            state.handles[resources.difficulty_tree_extra] != 0;
        if (auto* left = animations.registry.find_child_script(*bg, 90, 0))
            set_draw_layer_override(*left, selecting ? 22 : 23);
        if (auto* right = animations.registry.find_child_script(*bg, 91, 0))
            set_draw_layer_override(*right, 22);
        if (auto* half = animations.registry.find_child_script(*bg, 93, 0)) {
            // 主菜单 boot：g181 起 L23，g350（script93 VM age 170→171 帧）
            // →L21（窗A f348 最后 L23、f349 首 L21，探针实证）；返回路径
            // 重建树 age 从头计，沿用同序（oracle 无返回窗，§8.3 推定）。
            const i32 layer = selecting ? (state.particle_clock >= 28 ? 22 : 21)
                                        : (half->age.current <= 170 ? 23 : 21);
            set_draw_layer_override(*half, layer);
        }
    }
    if (auto* logo = animations.registry.find(state.handles[resources.logo_root])) {
        if (auto* flip = animations.registry.find_child_script(*logo, 97, 0))
            set_draw_layer_override(*flip, flip->age.current <= 60 ? 26 : 25);
        if (auto* burst = animations.registry.find_child_script(*logo, 98, 0))
            set_draw_layer_override(*burst, 26);
    }
}

// g361 菜单展开：root0（spawn 3-22）+ 版本条 + 全项入场 + 禁用项黑化。
// 白态 13-22 子 VM 建后即毁——真机 boot 无白条绘制（oracle 全窗无 title01@L25
// ops；真机 ins_500 建后待中断的插值冻结语义 vs 移植侧自动淡入，引擎差异，
// 蓝图 §oracle 章登记，待 AnmUpdate 语义裁定后改为保留）。
// [解剖轮2] Z 序不再销毁字条 VM（解剖 §⑥：转屏后仍在池）——返回路径无需
// 整树重建，present 复用 + 全项重入场（label 7+k 恢复透明度/可见性）。
bool TitleMainMenu::open_menu() {
    menu_age = 0;
    // Submenus have hidden the old rows. Recreate their script-driven entry
    // state on return rather than leaving a live-but-invisible menu tree.
    if ((state.flags & 2u) && present(state.handles[resources.main_root])) {
        animations.retire(state.handles[resources.main_root]);
        state.handles[resources.main_root] = 0;
    }
    if (!present(state.handles[resources.main_root])) {
        if (!create(resources.main_root)) return false;
        auto* root = animations.registry.find(state.handles[resources.main_root]);
        if (!root) return fail("Title menu root unavailable");
        // White rows retain their script layer 25. Their shared batch is
        // flushed at the subsequent layer26 draw (not a layer reassignment).
    }
    if (!present(state.version_bar)) {
        state.version_bar = animations.create(resources.version, resources.version_bar, -1, 0);
        if (!state.version_bar) return fail(animations.error);
    }
    return show_menu();
}

// 0044d138/157: restore the menu tree immediately, then queue the
// selected row's label17+cursor. The existing child VMs own interpolation.
bool TitleMainMenu::select_entry() {
    return interrupt(resources.main_root, 3, true) &&
           interrupt(resources.main_root, 17 + i32(i16(state.menu.cursor)), false) &&
           show_menu();
}

}  // namespace th13
