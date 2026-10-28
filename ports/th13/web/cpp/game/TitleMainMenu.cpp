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

// [f371 复验] 灰条入场重挂：oracle 批序灰条在**批尾**（f371 v0=w13@8e、
// g3 在第 11 枚）——root spawn 序 3..22 的灰 VM 在链首，oracle 侧实际是
// 入场时旧 VM 隐藏 + 新 VM 尾插（0x46f3f0）的重挂语义。入场曲线由
// advance_selection 的灰条驱动接管（SCAN 5f 序列）。
void TitleMainMenu::rearm_grey(i32 item) {
    auto* root = animations.registry.find(state.handles[resources.main_root]);
    if (!root) return;
    auto* grey = animations.registry.find_child_script(*root, item + 3, 0);
    if (!grey) return;
    const Vec3 translation = grey->visual.translation;
    const i32 layer = grey->visual.layer;
    // 旧灰 VM 隐藏（留池），新 VM 同脚本尾插承接入场。
    grey->visual.flags &= ~1u;
    grey->visible = false;
    auto* vm = animations.create_instance(*grey->resource, item + 3);
    if (!vm) return;
    vm->visual.render_flags |= 0x200;
    vm->visual.translation = translation;
    vm->visual.layer = layer;
    vm->creation_parent = grey->creation_parent;
    vm->rotation_parent = grey->rotation_parent;
    if (!animations.prime_instance(*vm)) return;
    vm->ordering = 0;
    // [f379 校准] oracle 输入当帧新灰已入批（f379 count 24 实证）——首帧
    // alpha 直接落 in 曲线首值 91（0x5b），advance_selection move 分支
    // step0 的灰条 poke 同帧接管后续帧。
    vm->visual.color = 0x5b000000u | (vm->visual.color & 0xffffffu);
    if (!animations.registry.submit(*vm)) return;
    animations.registry.attach(*vm, *root);
    // [f389 校准] ver 重挂链尾：rearm 后链序须为 [白条…, g5, ver]——
    // oracle f389 批形态 = title01 单批（含 g5）+ ver 尾批。
    if (auto* bar = animations.registry.find(state.version_bar))
        animations.registry.requeue_to_back(*bar);
}

bool TitleMainMenu::sound(i32 id) {
    return play_sound ? play_sound(id) || fail("Title sound request failed")
                      : fail("Title sound service unavailable");
}

// 全项入场：真机 boot 菜单全员可见（oracle 标题窗 L26 批次 10 字条 alpha 渐升、
// 色恒 192,192,192 实证），灰态逐项 interrupt(0,7+k)；禁用项 1 不入场、黑化。
// （th15 的 interrupt(root,3)+paint(30/31) 形状不搬：th13 灰态 label 3=淡出、
// 30/31=空操作，oracle 实测移动光标时旧项保持可见——不广播 label 3。）
// [wf360 实测] boot 入场 = 全员 15 帧线性 alpha 渐升（oracle f360-373 逐帧
// 17/帧，rgb 恒 c0c0c0）+ 禁用项恒 0xa0000000；**无 label-7 脉动**（稳态
// f383-469 恒 ff 平直——旧 interrupt(7+k) 入场的 192↔255 色脉动不成立，
// oracle f410 恒 c0c0c0 vs port 旧值 dfdfdf 实证）。入场经逐帧 poke（见
// update substate 1 的 boot_fade）。
bool TitleMainMenu::show_menu() {
    dim_disabled();
    return true;
}

// 1 号项禁用观感：黑 + alpha160（oracle 顶点色 0xA0000000，自建帧起恒定）。
// 直接落子 VM 色（真机疑为 supervisor 逐 VM 置色，蓝图 §oracle 章「机制待锚」）。
void TitleMainMenu::dim_disabled() {
    auto* root = animations.registry.find(state.handles[resources.main_root]);
    if (!root) return;
    auto* child = animations.registry.find_child_script(
        *root, i32(TitleMainEntry::ExtraStart) + 13, 0);
    if (child) child->visual.color = 0xA0000000u;
}

// Extra 行解锁门（0x44c570 case0：+0x5c40 bit1 非 0 = 解锁；写点 0x416130
// 通关落档 0x1b26b+char*2 bit0x10，任一角色普通难度通关置位）。宿主未接 =
// 禁用（原版无存档行为）。0x416150 无全角色判定——任一角色位即开。
bool TitleMainMenu::extra_gate() {
    for (i32 character = 0; character < 4; ++character)
        if (extra_unlocked && extra_unlocked(character)) return true;
    return false;
}

// [窗A 修正] 背景半张层值按 oracle 实测落码（蓝图 §8.3 层偏移现象；机制 =
// supervisor 逐 VM 置层推定，反汇编待锚）。oracle 证据（title-draws.jsonl）：
//   f349/g350 起 title00a 左半(entry1)@L23、右半(entry2)@L22（dump ins_304 为
//   22/21，各 +1）；select00 右半(entry4) boot 期 @L23（dump 21，+2），
//   g350 搬至 L21（与森林淡入同帧，substate 1 age==169 复刻）；select00 左半
//   (entry3)@L21 与 dump 一致不动。
void TitleMainMenu::adopt_background_layers() {
    auto* root = animations.registry.find(state.handles[resources.background_root]);
    if (!root) return;
    if (auto* forest_left = animations.registry.find_child_script(*root, 90, 0))
        forest_left->visual.layer = 23;
    if (auto* forest_right = animations.registry.find_child_script(*root, 91, 0))
        forest_right->visual.layer = 22;
    // [窗A 复验修正] select00 右半 boot 期（截窗 f0-348）oracle 实测 @L23——
    // 早前按「f349 起 L21」在创建时置 21 是错读（f349 才搬）。f349 搬移由
    // update substate1 age>=169 分支执行（保持）；此处创建期置 23。
    if (auto* overlay_right = animations.registry.find_child_script(*root, 93, 0))
        overlay_right->visual.layer = 23;
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
        // 未列出的入口保持确认音后不切屏（当前枚举已全部具名；兜底不伪造画面）。
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
        if (!present(state.handles[resources.background_root])) {
            if (!create(resources.background_root)) return false;
            // 仅新建时落层值（返回路径背景常驻，保留 g350 搬移后的状态）。
            adopt_background_layers();
        }
        const bool returning = (state.flags & 2u) != 0;
        // LOGO 复位根 95（dump L24155：spawn 99+96+100，无 94 的 110f 延迟）建后
        // 存进 94 槽——present/handles[94] 语义统一为「LOGO 树当前根」。
        if (!present(state.handles[resources.logo_root])) {
            const i32 logo = returning ? resources.logo_restore_root : resources.logo_root;
            if (!create(logo)) return false;
            if (logo != resources.logo_root) {
                state.handles[resources.logo_root] = state.handles[logo];
                state.handles[logo] = 0;
            }
        }
        state.change_substate(1);
        if (returning && !open_menu()) return false;
        [[fallthrough]];
    }
    case 1: {
        // 菜单 intro：boot 180 帧后建菜单（oracle g361：root0 + title01 十灰条
        // 入 L26 批次、title_v 版本条 30f 延迟淡入 → g390 可见）；返回路径立即建。
        const bool returning = (state.flags & 2u) != 0;
        // [wf360 白条定案] oracle 菜单白条 f360=淡入第 3 帧（0x33）→ spawn 于
        // f357=age177（w13 滞后 1 帧=rebind）；f357-359 无 op（低 alpha 帧不可
        // 见）→ 绘制侧压 0 复刻。可交互仍锚 age191（f372，oracle 实测）。
        const i32 menu_at = returning ? 0 : 179;
        if (!returning && state.age.current == menu_at && !open_menu()) return false;
        // [f389 校准] ver 延 1 帧创建（open_menu 挂起，此处消费——
        // oracle f389 首绘 vs port 原 f388）。
        if (version_bar_pending && state.age.current == menu_at + 1 &&
            !open_menu_version_bar())
            return false;

        if (!returning && state.age.current >= 169) {
            // [窗A 修正] oracle f349/g350：select00 右半自 L23 搬至 L21（与森林
            // title00a 淡入同帧；最后 23 帧 f348/g349、首个 21 帧 f349/g350，
            // 蓝图 §8.3 层搬移现象）。搬移机制（supervisor 逐 VM 置层）未反汇编。
            if (auto* root = animations.registry.find(state.handles[resources.background_root]))
                if (auto* overlay = animations.registry.find_child_script(*root, 93, 0))
                    overlay->visual.layer = 21;
        }
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
            // [轮7] 双向交叉（全 poke 序列，advance_selection 驱动）。
            selection_previous = state.menu.previous;
            selection_item = state.menu.cursor;
            selection_step = 0;
            selection_move = true;
            selection_active = true;
            // [f379-380] 移动交叉：新项灰条重挂入场（oracle f379 count 24 双灰
            // 在场实证——旧灰（前一选中项）留批 + 新灰尾插）。
            rearm_grey(state.menu.cursor);
        }
        if (pressed & 0x102) {  // Esc/X：跳到末项或直接退出分支
            if (state.menu.cursor == kTitleMainEntries - 1) {
                if (!sound(9)) return false;
                state.change_substate(4);
                return true;
            }
            if (!sound(9)) return false;
            state.menu.select(kTitleMainEntries - 1);
            if (!interrupt(resources.main_root, i32(i16(state.menu.cursor)) + 7, false))
                return false;
        }
        if (pressed & 0x80001) {  // Z/Enter：确认
            const i32 selected = state.menu.cursor;
            if (selected < 0 || selected >= kTitleMainEntries) break;
            if (selected == i32(TitleMainEntry::Quit)) {
                if (!sound(9)) return false;
                state.change_substate(4);
                break;
            }
            if (!sound(7)) return false;
            // 0044d924: confirmation broadcasts label6. The two record menus
            // consume the original ANM fade/retirement, including rearmed rows.
            if ((selected == i32(TitleMainEntry::SpellPractice) ||
                 selected == i32(TitleMainEntry::PlayerData)) &&
                !interrupt(resources.main_root, 6, false)) return false;
            // [解剖轮2] Z 确认序重做（difficulty-menu-anatomy.md §③/⑥ + 顶点色）：
            // 主菜单 VM **不销毁**（解剖实证 3..12/13..22 转屏后仍在池、仅逐步
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
            // 其余入口（真机有画面，本轮不实现）：确认序同上，enter() 不切屏
            // ——解剖实证字条隐藏与转屏解耦，难度侧时间线照走。
        }
        break;
    }
    case 4: {
        if (state.age.current == 9 && state.version_bar &&
            state.menu.cursor != kTitleMainEntries - 1) {
            // [f477 复验] oracle ver 末日 f478（Z+9、alpha 末值 0x19）——
            // 早前「Z+8」系末日帧 off-by-one，retire 于 age==9。
            animations.retire(state.version_bar);
        }
        if (state.age.current == 15 && state.handles[resources.logo_root]) {
            // [f483 复验] oracle title_logo 末日 f483（Z+14，本帧仍有 op）、
            // f484 起无——retire 于 age==15（末日+1，与 ver 同型 off-by-one）。
            animations.retire(state.handles[resources.logo_root]);
            // retire 后必须清槽：返回主菜单时 present(hs[94]) 才不会把已死
            // handle 误判为存活而跳过 create(95) 复位根（dump L24155：95 =
            // 99+96+100 静止 LOGO，无 110f 延迟）——旧残留 logo 的根因。
            state.handles[resources.logo_root] = 0;
        }
        if (state.age.current == 20 && state.handles[resources.background_root]) {
            // [f489 复验修正] oracle select00 右半亮化：f489 首绘步值 rgb 68
            // （f486-488 恒 64，Z=f469 → Z+20；408(60,0,192) 步值 68 起 +2/帧）。
            // port 的 interrupt(immediate=false) pending 当帧消费落第 1 步 66
            // ——true 值是首绘 68：改 immediate=true（中断即时 tick 至标签体
            // 首步，408 计步同帧起算），与 ver label1 的 off-by-one 同族根因
            // （插值首帧 tick 相位），不挪 age。
            animations.interrupt(state.handles[resources.background_root], 3, true);
        }
        if (state.age.current >= 9 && state.age.current <= 30) fade_menu_alphas(state.age.current - 9);
        if (state.age.current > 19) enter(state.menu.cursor);
        break;
    }
    }
    return true;
}

// 逐项 alpha poke（灰条入场插值早已结束/未开始，poke 不被覆写）。
// rgb 恒 (192,192,192)；禁用项恒黑 rgb（oracle 全程 0xa0000000）。
void TitleMainMenu::set_item_alpha(i32 item, i32 alpha, bool white) {
    auto* root = animations.registry.find(state.handles[resources.main_root]);
    if (!root) return;
    auto* child = animations.registry.find_child_script(*root, item + (white ? 13 : 3), 0);
    if (!child) return;
    if (alpha < 0) alpha = 0;
    if (alpha > 255) alpha = 255;
    const u32 rgb = item == i32(TitleMainEntry::ExtraStart) ? 0u : 0xc0c0c0u;
    child->visual.color = u32(alpha) << 24 | rgb;
}

// boot 15 帧线性淡入：oracle f360(alpha 0x22/0x33=帧 2/3)→f373 ff，17/帧。
// 菜单展开帧 = 帧 0（age==menu_at），次帧可见。
// boot 渐入（目标白条 13-22）：oracle f360-373 逐帧 = 线性 17/帧（帧 0-10
// →204）、尾段帧 11→249(0xf9)、12+→255。真机语义 = 白条自带 ins_409(15,
// 0,255) 停车推进；port 停车插值冻结（§8.2 反向）→ 监督者逐帧 poke 复刻。
// [废除] boot 渐入 = 白条自带 ins_409(15,0,255) 停车插值推进（port 引擎
// 同语义实证：f360 op10 alpha=0x22=第2帧值，监督者 poke 被插值逐帧覆写）。
void TitleMainMenu::boot_fade(i32) {}

// Z 序衰减（[f478 复验] oracle 实测 f478 起：白条 -13/帧、禁用黑条独立
// -8/帧——「Z+10 起」与黑条同率两处皆系旧读，帧位与曲线一并修正）。
void TitleMainMenu::fade_menu_alphas(i32 step) {
    auto* root = animations.registry.find(state.handles[resources.main_root]);
    if (!root) return;
    // [f482 精率] 白条 = 20 帧线性 12.75/帧（oracle f479-498 序列 242/229/216/204
    // /191… round(255-12.75k) 实证，旧 -13 系近似）；黑条独立 -8/帧。
    const i32 alpha = i32(255.0f - 12.75f * float(step)), dimmed = 160 - 8 * step;
    for (i32 item = 0; item < kTitleMainEntries; item++) {
        auto* child = animations.registry.find_child_script(*root, item + 13, 0);
        if (!child) continue;
        // [f478] 跳过已隐藏项（Z 序把 w13 隐 0 后 fade 不复活——oracle
        // f477 双侧同缺 279.5、f478 oracle 仍 20 枚的实证）。
        if (!(child->visual.color >> 24)) continue;
        const u32 rgb = child->visual.color & 0xffffffu;
        i32 a = item == i32(TitleMainEntry::ExtraStart) ? dimmed : alpha;
        if (a < 0) a = 0;
        if (a > 255) a = 255;
        child->visual.color = u32(a) << 24 | rgb;
    }
}

// g361 菜单展开：root0（spawn 3-22）+ 版本条 + 全项入场 + 禁用项黑化。
// 白态 13-22 子 VM 建后即毁——真机 boot 无白条绘制（oracle 全窗无 title01@L25
// ops；真机 ins_500 建后待中断的插值冻结语义 vs 移植侧自动淡入，引擎差异，
// 蓝图 §oracle 章登记，待 AnmUpdate 语义裁定后改为保留）。
// [解剖轮2] Z 序不再销毁字条 VM（解剖 §⑥：转屏后仍在池）——返回路径无需
// 整树重建，present 复用 + 全项重入场（label 7+k 恢复透明度/可见性）。
// [f389 校准] ver 的延帧创建体（open_menu 挂起 → update age==menu_at+1 消费）。
bool TitleMainMenu::open_menu_version_bar() {
    version_bar_pending = false;
    state.version_bar = animations.create(resources.version, resources.version_bar, -1, 0);
    return state.version_bar || fail(animations.error);
}

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
        // [wf360 定案] boot 菜单条 = 白条 13-22（右列 sprite 奇数、UV x∈
        // [0.5,1]）；灰条 3-12 无中断即驻池隐匿（alpha0 parked）。白条 dump
        // 层 25、oracle 绘于 26（+1 绘制侧，§8.3 族）——建后置 26；渐入由
        // boot_fade poke（真机白条 ins_409(15,0,255) 停车推进，port 冻结）。
        for (i32 white = 13; white <= 22; white++)
            if (auto* child = animations.registry.find_child_script(*root, white, 0))
                child->visual.layer = 26;
    }
    if (!present(state.version_bar) && !version_bar_pending) {
        // [f389 校准] ver 延 1 帧创建：open_menu 内挂起（title01 与 ver 的
        // oracle 首绘帧差 1——f388 vs f389），由 update substate1 的
        // open_menu_version_bar 消费。
        version_bar_pending = true;
    }
    // 选中项曾白化续衰减（Z 序）：恢复灰 192（label 28 = rgb→192 4f）。
    if (state.confirmed_entry >= 0 && state.confirmed_entry < kTitleMainEntries &&
        state.confirmed_entry != i32(TitleMainEntry::ExtraStart))
        child_interrupt(state.confirmed_entry + 13, 28, false);
    // 禁用项（1 号白条 14）走真中断 label 29（ins_404(0,0,0)+ins_403(255)+
    // ins_409(1,0,160) 即时黑@160——oracle 全程 0xa0000000 的真机制）。
    child_interrupt(i32(TitleMainEntry::ExtraStart) + 13, 29, false);
    return show_menu();
}

// [轮7→f371 复验重写] 选择交叉 = 白条 out/in 与灰条 in/out 的**同帧配对交换**
// （oracle f371 count=22 → 11 枚 = 10 白 + g3@91 首现；f375 20 = 9 白 + g3@255；
// f379-380 24 = 交叉期双灰在场）。[轮8「灰条不绘」定案系 up 前缀取样偏差
// （前缀只覆盖首 3.67 枚，灰条 u=0 列在批尾被截断）——废除。]
// 曲线全部来自 oracle SCAN 实测：灰 in {91,163,214,244,255}(5f)/out {40,10,0}；
// 白 out {142,79,35,8,0}；移动旧白升 {214,244,255}、新白降 {163,91,40,0}。
void TitleMainMenu::advance_selection() {
    if (!selection_active) return;
    static const i32 kGreyIn[5] = {91, 163, 214, 244, 255};
    static const i32 kGreyOut[3] = {40, 10, 0};
    static const i32 kWhiteOut[5] = {142, 79, 35, 8, 0};
    static const i32 kOldWhiteRise[4] = {214, 244, 255, 255};
    static const i32 kNewWhiteFall[4] = {163, 91, 40, 0};
    const i32 step = selection_step++;
    if (!selection_move) {
        // [轮9] oracle f371-381 白条全序：降 {142,79,35,8,0}（5 帧）→隐（3 帧）
        // →回升 {214,244,255}（f379-381）→稳态 255（w13 复入批）。
        static const i32 kRise[3] = {214, 244, 255};
        static const i32 kOut[5] = {142, 79, 35, 8, 0};
        // 灰条同帧配对入场（f371 count 22 = 10 白 + g3@91 实证）。
        if (step < 5) {
            set_item_alpha(selection_item, kOut[step], true);
            set_item_alpha(selection_item, kGreyIn[step], false);
        } else if (step < 8) {
            set_item_alpha(selection_item, 0, true);
            set_item_alpha(selection_item, 255, false);
        } else if (step < 11) {
            set_item_alpha(selection_item, kRise[step - 8], true);
            // 回升期灰条同步淡出（SCAN out {40,10,0} 帧位对齐回升 3 帧）。
            const i32 grey = step - 8 < 2 ? kGreyOut[step - 8] : 0;
            set_item_alpha(selection_item, grey, false);
        } else {
            set_item_alpha(selection_item, 255, true);
            set_item_alpha(selection_item, 0, false);
            selection_active = false;
        }
    } else {
        const i32 old_item = selection_previous, new_item = selection_item;
        // [f379-383 oracle 重构] w15 降 5 帧 {163,91,40,10,0}（f382=0a 在场
        // 锚点）；旧灰 s2 出批（f381 count 22）；新灰入场后**恒驻**
        // （f383 oracle 20 = 9 白 + g5 实证——选中新项以灰条驻场）。
        static const i32 kNewWhiteFall5[5] = {163, 91, 40, 10, 0};
        static const i32 kMoveGreyNew[5] = {91, 163, 214, 244, 255};
        if (step < 5) {
            set_item_alpha(old_item, step < 4 ? kOldWhiteRise[step] : 255, true);
            set_item_alpha(new_item, kNewWhiteFall5[step], true);
            set_item_alpha(new_item, kMoveGreyNew[step], false);
            if (step >= 2) set_item_alpha(old_item, 0, false);  // 旧灰 s2 出批
        } else {
            set_item_alpha(old_item, 255, true);
            set_item_alpha(new_item, 0, true);
            set_item_alpha(new_item, 255, false);  // 新灰满驻（稳态选中项灰条）
            set_item_alpha(old_item, 0, false);
            selection_active = false;
        }
    }
}

// [wf371 实测] 菜单就绪（f371=age191）选中项闪场：alpha 255→0 约 4 帧线性
// （f371-374: 0x8e/0x4f/0x23/0x08）、隐 4 帧（f375-378 批内消失）、
// 3 帧回升至 ff（f379-381: 0xd6/0xf4/0xff）后稳态平直。移动光标的入场
// 同为短淡入至 ff 平直（无脉动）。经 alpha poke 序列驱动（update 内）。
bool TitleMainMenu::select_entry() {
    selection_step = 0;
    selection_active = true;
    selection_move = false;
    selection_item = state.menu.cursor;
    // [f371] 选中项灰条入场重挂（oracle 批序尾插实证；首帧 alpha 0，曲线
    // 由 advance_selection 的灰条驱动接管——5f 序列首值 91 于 step0 落）。
    rearm_grey(selection_item);
    return true;  // 首帧由绘制回调 step0 落 142（oracle f371=0x8e）
}

}  // namespace th13
