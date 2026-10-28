#include "TitleSelectionMenu.hpp"

namespace th13 {

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
    auto* tree_vm = animations.registry.find(state.handles[tree]);
    if (!tree_vm) return check(false);
    if (!check(visuals.interrupt(tree, 3, true))) return false;
    return check(visuals.interrupt(tree, 17 + state.menu.cursor, false));
}

// [解剖轮2] 主菜单 Z+21 隐藏时间线（解剖 §③/⑥ + 顶点色）：
//   非选中灰条 9 枚隐藏（visible=false，VM 留池）；选中项白化（rgb→255、
//   alpha=204）续 -13/帧 衰减（advance_choreography 接力）至 Z+37 隐藏；
//   白条 9 枚（items 1-9 的 13+k = script14-22）闪现 8 帧（alpha 64→0，
//   -7/帧）后销毁——白条建后中和其 spawn 淡入插值（interpolators.alpha
//   .duration=0，poke 才不被逐帧覆写；引擎语义差异补偿见蓝图 §8.2）。
void TitleDifficultyMenu::finish_menu_hides() {
    auto* menu_root = animations.registry.find(state.handles[0]);
    if (!menu_root) return;
    for (i32 item = 0; item < kTitleMainEntries; item++) {
        auto* child = animations.registry.find_child_script(*menu_root, item + 13, 0);
        if (!child) continue;
        if (item == state.confirmed_entry) {
            child->visual.color = 0xccffffffu;  // 白化 @204（oracle f490 样本）
            continue;
        }
        child->visible = false;
        child->visual.flags &= ~1u;
    }
    for (i32 item = 1; item < kTitleMainEntries; item++) {
        const i32 script = 13 + item;  // 白条 14-22（items 1-9；0 号白条=选中灰条本体）
        if (!visuals.create(script)) continue;
        auto* white = animations.registry.find(state.handles[script]);
        if (!white) continue;
        white->interpolators.alpha.duration = 0;
        white->visual.color = 0x40ffffffu;  // alpha 64（oracle f490 L25 批本）
    }
}

// [解剖轮2] 神霊粒子：ascii bank2 script18 克隆（解剖「打字机不存在」纠偏）。
// 脚本体经 RNG 槽 10010-10012（AnmVariables 映射游戏 RNG 流）自算全屏随机
// 出生位与 300f 漂移目标（窗内不死灭 ✓ +300 自灭），逐帧几何在
// title-draws.jsonl——节奏（f495 起 17@f500→55@f599，≈1/3 帧）先按
// 「age5 突发 17 + 每 3 帧 1 枚」落码，出生次序/节奏待对拍校准。
void TitleDifficultyMenu::spawn_particles(i32 count) {
    for (i32 i = 0; i < count; i++) {
        if (!animations.create(resources.ascii, resources.difficulty_particle, -1, 0)) {
            check(false);
            return;
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
    if (f <= 8) {
        i32 alpha = 64 - 7 * f;
        if (alpha < 0) alpha = 0;
        for (i32 item = 1; item < kTitleMainEntries; item++) {
            const i32 script = 13 + item;
            auto* white = animations.registry.find(state.handles[script]);
            if (!white) continue;
            if (f == 8) {
                animations.retire(state.handles[script]);
                continue;
            }
            white->visual.color = u32(alpha) << 24 | 0xffffffu;
        }
    }
    if (auto* menu_root = animations.registry.find(state.handles[0])) {
        if (auto* selected =
                animations.registry.find_child_script(*menu_root, state.confirmed_entry + 13, 0)) {
            if (f < 16) {
                i32 alpha = 204 - 13 * f;
                if (alpha < 0) alpha = 0;
                selected->visual.color = u32(alpha) << 24 | 0xffffffu;
            } else if (f == 16) {
                selected->visible = false;
                selected->visual.flags &= ~1u;  // oracle f506：title01 全消
            }
        }
    }
    // 粒子节奏（age5 突发 17 + 每 3 帧 1 枚；总数 17+34=51 vs oracle 55，待校）。
    if (f == 5) spawn_particles(17);
    else if (f > 5 && f % 3 == 0) spawn_particles(1);
    // oracle f517（Z+48）：select00 右半自 L21 搬 L22（左半留 L21 = 层分裂）。
    // 真机由 script118 承接右半；移植侧直接搬既有 93 VM 层（几何等价，VM
    // 构成差异登记蓝图待裁）。
    if (f == 27) {
        if (auto* bg = animations.registry.find(state.handles[89]))
            if (auto* right = animations.registry.find_child_script(*bg, 93, 0))
                right->visual.layer = 22;
    }
}

void TitleDifficultyMenu::retire_difficulty() {
    // root123 的 label1 已在 Esc 按键帧 pending 并清根槽（0x451255-0x451263）；
    // 此处不重发 root 退场，只收 Rank Select 徽记并做背景右半层复位
    //（难度期 f517 起 L22；回主菜单推定复 L21，待 oracle）。
    visuals.retire(resources.difficulty_badge);
    if (auto* bg = animations.registry.find(state.handles[89]))
        if (auto* right = animations.registry.find_child_script(*bg, 93, 0))
            right->visual.layer = 21;
}

void TitleDifficultyMenu::choose_character() noexcept {
    state.change_screen(TitleScreen::Character);
    // LAB_00451363（0x450fb0 case3 收场）：be7c4<4 时才取光标为难度；
    // Extra(4) 保持 4（bb4d0 已被 0x44cd10 case1 置 4）。
    if (progress.difficulty < 4) {
        settings.configured_difficulty = state.menu.cursor;
        progress.difficulty = state.menu.cursor;
    }
    state.menu.push();
    state.menu.wrapping = true;
    state.menu.count = kTitleCharacterRows;
    // 角色行 1-4 = 霊夢/魔理沙/早苗/妖夢（推定行位，蓝图 §1.4）。
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
// 未通关选择框靠 vm+0x18 bit1 游戏抑制位（0x470030）保持不绘——跳转只置
// bit0（0x46265d）不清 bit1。port 引擎可见位与跳转共用 flags bit0（跳转即
// visible=true），根广播会解除 r4-M clear-tag 隐藏；不改共享 core，按原版
// 可见语义定向发送：仅当前角色三件套收标签（未通关选择框在原版被抑制，
// 等价于不接收），root 自身无该标签本为 no-op。
bool TitleCharacterMenu::character_interrupt(i32 character, i32 label, bool immediate) {
    // 0x451450 case0：Extra 屏根 130，武器 139+（weapon_base_ex）；立绘/
    // 选择框标签族同号共用。
    const bool extra = progress.difficulty == 4;
    const i32 root = extra ? resources.char_menu_root_ex : resources.char_menu_root;
    const i32 weapon_base = extra ? resources.weapon_base_ex : resources.weapon_base;
    const i32 full = label + character;  // 立绘/武器/选择框同号标签族
    if (!visual(visuals.child_interrupt(root, resources.portrait_base + character, full, immediate)))
        return false;
    if (!visual(visuals.child_interrupt(root, weapon_base + character, full, immediate)))
        return false;
    if (host.character_cleared(character, progress.difficulty))
        return visual(visuals.child_interrupt(root, resources.clear_tag_base + character,
                                              full, immediate));
    return true;
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
    // 根 129/130 按难度一并回收（Extra 屏回退同样只回难度屏）。
    visuals.retire(resources.difficulty_badge + 1);
    visuals.retire(resources.char_menu_root_ex);
    visuals.retire(resources.character_root);
    state.change_screen(TitleScreen::Difficulty);
    if (state.menu.cursor >= kTitleCharacterFirst && state.menu.cursor <= kTitleCharacterLast)
        settings.preferred_character = title_character_at(state.menu.cursor);
    state.menu.pop();
}

bool TitleCharacterMenu::update(u32 pressed, u32 repeated) {
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
        // 与 port draws L25×28/L26×12 双证。行为层光标仍按 7 行建模（menu
        // count 不变），仅撤 VM 树。
        if (state.handles[root]) visuals.retire(root);
        // 0x451450 case0：自机菜单根 = (be7c4==4)+0x81 → 129（普通）/130
        //（EX 武器 139-142）；root（character_root=2 名行树）只做上方清理。
        const i32 menu_root = progress.difficulty == 4 ? resources.char_menu_root_ex
                                                       : resources.char_menu_root;
        if (!visual(visuals.retire(menu_root)) || !visual(visuals.create(menu_root)))
            return false;
        // Player Select 标题条（select01 sprite80，dump L24293 与 101 同体：
        // (-256,8)→(32,8) 20f 滑入）：自机屏独占（oracle 窗B f622 起实证，
        // 难度屏期间不建）。
        if (!visual(visuals.create(resources.difficulty_badge + 1))) return false;
        // [r4-M] clear-tag 可见门（0x45145f-0x451734 四路展开判定：score_data
        // +0x56dc*c+0x7e4+DAT_004be7c4(当前难度)*4 == 0 → 00470030 递归清
        // script 0x83+c 子树可见位）。缺关（子不存在）原版为空指针 no-op。
        // 原版清 vm+0x18 bit1；port 的可见位 = visual.flags bit0（tick 每帧
        // 以 visible() 重算），故此处同清 flags bit0 + visible 才能跨帧持久。
        if (AnmVm* menu = animations.registry.find(state.handles[menu_root])) {
            const std::function<void(AnmVm&)> hide_tree = [&](AnmVm& vm) {
                vm.visual.flags &= ~1u;
                vm.visible = false;
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
        const bool was_character = state.menu.cursor >= kTitleCharacterFirst &&
                                   state.menu.cursor <= kTitleCharacterLast;
        if ((pressed | repeated) & 0x40) {  // ←：上一行
            // 0x4519c1：退场标签读活动选角（同帧 ←→ 时为中间值），非帧初快照。
            if (was_character)
                if (!exit_portrait(title_character_at(state.menu.cursor), 22)) return false;
            state.menu.move(-1);
            if (!state.menu.error.empty()) return check(false, state.menu.error.c_str());
            const bool now_character = state.menu.cursor >= kTitleCharacterFirst &&
                                       state.menu.cursor <= kTitleCharacterLast;
            if (now_character)
                if (!enter_portrait(title_character_at(state.menu.cursor), 12)) return false;
        }
        if ((pressed | repeated) & 0x80) {  // →：下一行
            if (was_character)
                if (!exit_portrait(title_character_at(state.menu.cursor), 17)) return false;
            state.menu.move(1);
            if (!state.menu.error.empty()) return check(false, state.menu.error.c_str());
            const bool now_character = state.menu.cursor >= kTitleCharacterFirst &&
                                       state.menu.cursor <= kTitleCharacterLast;
            if (now_character)
                if (!enter_portrait(title_character_at(state.menu.cursor), 7)) return false;
        }
        if (pressed & 0x102) {  // Esc/X：回难度菜单
            state.change_substate(4);
            return check(host.sound(9), "Character cancel sound failed");
        }
        if (pressed & 0x80001) {  // Z/Enter
            if (state.menu.cursor == kTitleCharacterRows - 1) {  // やめる行
                state.change_substate(4);
                return check(host.sound(9), "Character cancel sound failed");
            }
            if (state.menu.cursor < kTitleCharacterFirst ||
                state.menu.cursor > kTitleCharacterLast) {
                // 0/5 行（EXTRA/LUNATIC 字条位）：语义待锚定，本轮确认音后不动作。
                return check(host.sound(7), "Character confirm sound failed");
            }
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
            if (!check(host.begin_transition(state.start_effect),
                       "Game start transition unavailable") ||
                !check(animations.interrupt(state.start_effect, 7),
                       "Game start transition interrupt failed"))
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
            if (!check(host.begin_transition(state.start_effect),
                       "Practice start transition unavailable") ||
                !check(animations.interrupt(state.start_effect, 7),
                       "Practice start transition interrupt failed")) return false;
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
