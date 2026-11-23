#include "GameMenu.hpp"

#include <cstring>

#include "AnmManager.hpp"
#include "Ascii.hpp"
#include "GameGlobals.hpp"
#include "GameManager.hpp"
#include "Gui.hpp"
#include "Input.hpp"
#include "Player.hpp"
#include "ScoreFile.hpp"
#include "ScreenCompositor.hpp"

namespace th16 {

namespace {

// 名字输入字符表（0x43f980 case 0xc）：下标 0x58 / 0x59 / 0x5a 为空格 / 退格 / 结束
constexpr const char* kNameChars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*   ";
constexpr int kNameColumns = 13;
constexpr i32 kContinueLives = 2;
constexpr i32 kContinueBombs = 2;
constexpr i32 kMaxContinues = 9;

// 名字输入字符表末尾三个功能键在 ascii.anm 里的专用字形（0x43e730：空格 0x81、退格 0x7f、结束 0x80）
constexpr u8 kNameSpaceGlyph = 0x81, kNameBackspaceGlyph = 0x7f, kNameEndGlyph = 0x80;
// 菜单列表文字的两种颜色：当前行黄、其余灰
constexpr u32 kListHighlight = 0xffffff00, kListNormal = 0xff808080;

// 0x43e9b0 / 0x43ec10 列表里的名称表（.rdata 0x491970 / 0x4918ac / 0x4918d4 / 0x4918f0 / 0x491920）
constexpr const char* kShotNames[4] = {"Reimu  ", "Cirno  ", "Aya    ", "Marisa "};
constexpr const char* kSeasonShortNames[5] = {"Sp", "Sm", "At", "Wt", "Fl"};
constexpr const char* kDifficultyShortNames[6] = {"E ", "N ", "H ", "L ", "EX", "OD"};
constexpr const char* kStageShortNames[8] = {"tst", "St1", "St2", "St3", "St4", "St5", "St6", "Ex "};
constexpr const char* kStageNames[8] = {"test   ", "Stage 1", "Stage 2", "Stage 3", "Stage 4", "Stage 5", "Stage 6", "Extra  "};

// gmtime 的日期部分（UTC，不处理负数）：原版用 CRT gmtime 拆回放 / 排名时刻，列表只显示 年%100 / 月 / 日
struct CivilDate { i32 year; i32 month; i32 day; };

CivilDate civil_date(i64 seconds) {
    i64 days = seconds / 86400;
    // Howard Hinnant 的 days → civil 公式（纪元 1970-01-01 平移到 0000-03-01）
    days += 719468;
    const i64 era = (days >= 0 ? days : days - 146096) / 146097;
    const i64 doe = days - era * 146097;
    const i64 yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    const i64 doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    const i64 mp = (5 * doy + 2) / 153;
    const i32 day = i32(doy - (153 * mp + 2) / 5 + 1);
    const i32 month = i32(mp < 10 ? mp + 3 : mp - 9);
    const i32 year = i32(yoe + era * 400 + (month <= 2 ? 1 : 0));
    return {year, month, day};
}

template <size_t N>
const char* lookup(const char* const (&table)[N], i32 index) {
    return index >= 0 && size_t(index) < N ? table[index] : "";
}

// 0x43f980：主菜单各项对应的子脚本组（选中后中断 6 退场）
constexpr i32 kItemScripts[5][5] = {
    {},
    {0x81, 0x8a, 0x92, 0x94, 0x96},
    {0x82, 0x8b, 0x97, 0, 0},
    {0x83, 0x8c, 0, 0, 0},
    {0x84, 0x8d, 0x93, 0x95, 0x98},
};

}  // namespace

// ---- 操作说明 ----

// 0x42efb0：help.anm 脚本 index 的 UI VM
void HelpScreen::spawn_items(AnmManager& anm_manager) {
    for (int i = 0; i < 9; ++i) {
        item_vms[i] = anm != nullptr ? anm_manager.spawn_ui(*anm, i) : 0;
        anm_manager.interrupt_now(item_vms[i], cursor.index == i ? 2 : 3);
    }
}

void HelpScreen::highlight(AnmManager& anm_manager) {
    for (int i = 0; i < 9; ++i) anm_manager.interrupt_now(item_vms[i], cursor.index == i ? 2 : 3);
}

// LAB_0042eddb：目录项退场，请求装载 help_NN.png（纹理替换属渲染侧；逻辑上直接视为装好）
void HelpScreen::start_page_load(AnmManager& anm_manager) {
    step = LoadPage;
    timer.set(0);
    for (u32 id : item_vms) anm_manager.interrupt(id, 1);
    step = PageReady;
}

// 0x42eab0
void HelpScreen::update(AnmManager& anm_manager, const InputState& input, float game_speed) {
    auto nav = [&](u32 bit) { return ((input.pressed | input.repeat) & bit) != 0; };
    switch (phase) {
    case Loading:
        phase = Running;
        break;
    case Closing:
        if (timer.current > 0x1d) closed = true;
        break;
    case Running:
        switch (step) {
        case BuildMenu:
            cursor.count = 9;
            cursor.set(0);
            spawn_items(anm_manager);
            step = Menu;
            [[fallthrough]];
        case Menu:
            if (timer.current <= 0x13) break;
            {
                const i32 before = cursor.index;
                if (nav(InputState::kUp)) cursor.move(-1);
                if (nav(InputState::kDown)) cursor.move(1);
                if (before != cursor.index) {
                    sfx_queue.push_back(10);
                    highlight(anm_manager);
                }
            }
            if ((input.pressed & InputState::kConfirmMask) != 0) {
                sfx_queue.push_back(7);
                start_page_load(anm_manager);
            } else if ((input.pressed & InputState::kCancelMask) != 0) {
                sfx_queue.push_back(9);
                for (u32 id : item_vms) anm_manager.interrupt(id, 1);
                phase = Closing;
                step = BuildMenu;
                timer.reset();
            }
            break;
        case LoadPage:
            break;
        case PageReady:
            page_vm = anm != nullptr ? anm_manager.spawn_ui(*anm, 9) : 0;
            step = Page;
            timer.set(0);
            [[fallthrough]];
        case Page:
            if (timer.current < 0x14) break;
            if ((input.pressed & InputState::kDown) != 0 && cursor.index <= 7) {
                step = TurnPage;
                timer.set(0);
                sfx_queue.push_back(7);
                cursor.move(1);
                anm_manager.interrupt_now(page_vm, 7);
            } else if ((input.pressed & InputState::kUp) != 0 && cursor.index >= 1) {
                step = TurnPage;
                timer.set(0);
                sfx_queue.push_back(7);
                cursor.move(-1);
                anm_manager.interrupt_now(page_vm, 8);
            } else if ((input.pressed & (InputState::kConfirmMask | InputState::kCancelMask)) != 0) {
                sfx_queue.push_back(9);
                step = Menu;
                timer.set(0);
                anm_manager.interrupt(page_vm, 1);
                spawn_items(anm_manager);
                break;
            } else {
                break;
            }
            [[fallthrough]];
        case TurnPage:
            if (timer.current >= 0x14) start_page_load(anm_manager);
            break;
        }
        break;
    }
    timer.speed_slot = 0;
    timer.tick(game_speed);
}

// ---- GameMenu ----

bool GameMenu::nav(u32 bit) const {
    return ((ctx.input->pressed | ctx.input->repeat) & bit) != 0;
}

// 0x43e150
void GameMenu::set_mode(Mode m) {
    previous_mode = mode;
    mode = m;
    state = PauseIntro;
    timer.reset();
    timer2.reset();
    cursor.disabled_count = 0;   // +0x108
}

// 0x43e200
void GameMenu::set_state(State s) {
    state = s;
    timer.reset();
}

void GameMenu::cursor_interrupt(i32 base) {
    ctx.animations->interrupt(menu_vm, i32(i16(cursor.index + base)));
}

// 0x46f5a0：菜单 VM 不存在时顺带清掉 id；子 VM 不存在时中断 0 号 id（空操作）
void GameMenu::child_interrupt(i32 script, i32 value) {
    AnmVm* root = ctx.animations->find(menu_vm);
    if (root == nullptr) {
        menu_vm = 0;
        return;
    }
    AnmVm* child = AnmManager::find_child(*root, script, 0);
    ctx.animations->interrupt(child != nullptr ? child->id : 0, value);
}

void GameMenu::hide_menu() {
    if (AnmVm* vm = ctx.animations->find(menu_vm)) AnmManager::hide_tree(*vm);
}

void GameMenu::show_menu() {
    if (AnmVm* vm = ctx.animations->find(menu_vm)) AnmManager::show_tree(*vm);
}

// 0x43ef20：text.anm 0x34 的 UI VM 作截屏框；把后备缓冲的场地区域拷进其纹理属渲染侧（0x440c60 登记拷贝请求）
void GameMenu::capture_screenshot() {
    ctx.animations->kill(screenshot_vm);
    screenshot_vm = 0;
    if (ctx.text_anm == nullptr) return;
    screenshot_vm = ctx.animations->spawn_ui(*ctx.text_anm, 0x34);
    if (ctx.animations->find(screenshot_vm) == nullptr) screenshot_vm = 0;
}

// 0x42dbc0：把本段游戏时长累加进成绩文件（总计与本机体项）；计时源为 QueryPerformanceCounter，不影响游戏状态
void GameMenu::accumulate_play_time() {
    if (ctx.accumulate_play_time) ctx.accumulate_play_time();
}

// 0x43e5f0
void GameMenu::update() {
    if (mode == Closed) {
        const GameGlobals& g = *ctx.globals;
        const GameManager& gm = *ctx.game;
        const bool esc = (ctx.input->pressed & InputState::kEscape) != 0 ||
                         (ctx.config_flags != nullptr && (*ctx.config_flags & 0x10u) != 0);
        // 原版还要求 GameManager 更新链已登记且启用；该链在 0x42cb60 装载完成后始终启用，与 kLoading 清零同步
        if ((g.flags & 0x40u) == 0 && (gm.flags & GameManager::kStageTransition) == 0 && esc &&
            (gm.flags & GameManager::kLoading) == 0 && gm.timer.current > 0x1d) {
            open_pause();
        }
    } else if (mode >= Pause && mode <= PracticeResult) {
        run_state_machine();
    }
    timer.speed_slot = 0;
    timer.tick(*ctx.game_speed);
    timer2.speed_slot = 0;
    timer2.tick(*ctx.game_speed);
}

// 0x43f0f0
void GameMenu::open_pause() {
    AnmManager& anm = *ctx.animations;
    accumulate_play_time();
    set_mode(Pause);
    ctx.game->flags |= GameManager::kMenuOpen;
    front = ctx.gui->front;
    anm.kill(menu_vm);
    menu_vm = 0;
    if (front != nullptr) menu_vm = anm.spawn_ui(*front, ctx.game->mode == 0 ? 0x9c : 0x9e);
    anm.interrupt(menu_vm, 3);
    suspend_sound();
    sfx(0xe);
    if ((ctx.globals->flags & GameGlobals::kModeMask) != 0x20) bgm(6, 0, "Pause");
    flush_sound();
    capture_screenshot();
    saved_game_speed = *ctx.game_speed;
    *ctx.game_speed = 1.0f;
    saved_sound_state = *ctx.sound_thread_state;
    *ctx.sound_thread_state = 0;
    ctx.gui->hide_dialogue();
    ctx.gui->hide_hud();
    flags &= ~kFlagSpellPractice;
}

// 0x43f350
void GameMenu::open_continue() {
    accumulate_play_time();
    if (ctx.game->mode == 1) {
        // 回放中：直接回标题（DAT_004c1800 & 0x2000 时退出）
        ctx.globals->scene_request = (ctx.config_flags != nullptr && (*ctx.config_flags & 0x2000u) != 0) ? 2 : 4;
        return;
    }
    set_mode(Continue);
    set_state(ContinueIntro);
    ctx.game->flags |= GameManager::kMenuOpen;
    suspend_sound();
    sfx(0xe);
    const bool spell_practice = (ctx.globals->flags & GameGlobals::kModeMask) == 0x20;
    if (!spell_practice) bgm(6, 0, "Pause");
    flush_sound();
    capture_screenshot();
    front = ctx.gui->front;
    if (!spell_practice) {
        // 保存当前曲名与播放位置（0x471bd0），切到 Game Over 曲 th128_08；原版置的是音乐室 0 号位 +0x19fa6
        if (ctx.music != nullptr) {
            saved_bgm = ctx.music->current;
            saved_bgm_position = ctx.music->output != nullptr ? ctx.music->output->position_seconds() : 0.0;
            ctx.music->register_track(0, "th128_08");   // 0x43c370
            ctx.music->play_slot(0);                     // 0x43c3f0 的命令部分
        }
        if (ctx.scores != nullptr) ctx.scores->mark_bgm_heard(0);
    }
    practice = 0;
    saved_game_speed = *ctx.game_speed;
    *ctx.game_speed = 1.0f;
    saved_sound_state = *ctx.sound_thread_state;
    *ctx.sound_thread_state = 1;
    flags &= ~kFlagSpellPractice;
}

// 0x43f500
void GameMenu::open_practice_result() {
    accumulate_play_time();
    if (ctx.game->mode == 1) {
        ctx.globals->scene_request = (ctx.config_flags != nullptr && (*ctx.config_flags & 0x2000u) != 0) ? 2 : 4;
        return;
    }
    ctx.game->flags |= GameManager::kMenuOpen;
    set_mode(PracticeResult);
    set_state(PracticeResultIntro);
    const u32 game_mode = ctx.globals->flags & GameGlobals::kModeMask;
    if (game_mode == 0x20) set_state(SpellPracticeIntro);
    if (game_mode == 0) {
        // 普通模式走到这里只有回放结束：截屏框（0x440cd0 登记场地区域拷贝）
        ctx.animations->kill(screenshot_vm);
        screenshot_vm = ctx.text_anm != nullptr ? ctx.animations->spawn_ui(*ctx.text_anm, 0x34) : 0;
    }
    front = ctx.gui->front;
    practice = 1;
    saved_game_speed = *ctx.game_speed;
    *ctx.game_speed = 1.0f;
    saved_sound_state = *ctx.sound_thread_state;
    *ctx.sound_thread_state = 1;
    ctx.gui->hide_dialogue();
    ctx.gui->hide_hud();
    flags |= kFlagSpellPractice;
}

// 0x43f240：结局 / 回放结束后回标题前的菜单（front.anm 0x9f）
void GameMenu::open_return_to_title() {
    set_mode(Pause);
    set_state(PracticeIntro);
    ctx.game->flags |= GameManager::kMenuOpen;
    capture_screenshot();
    front = ctx.gui->front;
    ctx.animations->kill(menu_vm);
    menu_vm = front != nullptr ? ctx.animations->spawn_ui(*front, 0x9f) : 0;
    ctx.animations->interrupt(menu_vm, 3);
    suspend_sound();
    bgm(6, 0, "Pause");
    saved_game_speed = *ctx.game_speed;
    *ctx.game_speed = 1.0f;
    saved_sound_state = *ctx.sound_thread_state;
    *ctx.sound_thread_state = 0;
    flags &= ~kFlagSpellPractice;
}

// 0x43f6a0
void GameMenu::resume_game() {
    if (ctx.game->mode == 0 && ctx.restart_play_clock) ctx.restart_play_clock();
    ctx.game->flags &= ~GameManager::kMenuOpen;
    *ctx.game_speed = saved_game_speed;
    ctx.gui->show_dialogue();
    ctx.gui->show_hud();
    *ctx.sound_thread_state = saved_sound_state;
}

// 0x43f740 / 0x43f790
void GameMenu::close_overlay() {
    if (ctx.game->mode == 0 && ctx.restart_play_clock) ctx.restart_play_clock();
    ctx.animations->interrupt(screenshot_vm, 1);
    ctx.animations->interrupt(menu_vm, 1);
    *ctx.game_speed = saved_game_speed;
}

// 0x43f980 case 0x10 模式 2 光标 0：续关
void GameMenu::continue_game() {
    GameGlobals& g = *ctx.globals;
    if (g.stage == 7) {
        ctx.globals->scene_request = 0xe;   // Extra：重新开始
        return;
    }
    g.lives = kContinueLives;
    g.life_fragments = 0;
    g.bombs = kContinueBombs;
    ctx.gui->refresh_bombs_display(kContinueBombs, g.bomb_fragments);
    g.power = 0;
    g.bomb_fragments = 0;
    if (g.power_max < 0) g.power = g.power_max;
    else if (g.power_per_level > 0) g.power = g.power_per_level;
    {
        // 0x43ddf0(power_per_level × 4)
        const i32 add = g.power_per_level * 4;
        if (g.power_max > g.power) {
            g.power += add;
            if (g.power > g.power_max) {
                g.power = g.power_max;
                ctx.gui->show_notice(Gui::Notice::FullPower, 0);   // 0x42bcf0(0, 2)
            }
        }
    }
    ctx.player->setup_options(*ctx.animations, g);
    ctx.gui->refresh_lives_display(g.lives, g.life_fragments);
    ctx.gui->refresh_bombs_display(g.bombs, g.bomb_fragments);
    g.continues = g.continues + 1 > kMaxContinues ? kMaxContinues : g.continues + 1;
    g.score = 0;
    --g.continues_left;
    ctx.game->flags &= ~GameManager::kMenuOpen;
    resume_sound();
    // 0x45ed00(2, -1, 保存曲名) 续播 → 0x45e330 排空 → 0x471c90 从保存时刻重填缓冲
    bgm(2, -1, saved_bgm.c_str());
    flush_sound();
    if (ctx.music != nullptr && ctx.music->output != nullptr) ctx.music->output->restart_at(saved_bgm_position);
    *ctx.game_speed = saved_game_speed;
    ctx.gui->show_dialogue();
    ctx.gui->show_hud();
    *ctx.sound_thread_state = saved_sound_state;
}

// 0x43f980 case 0x10
void GameMenu::execute_selection() {
    if (timer.current <= 0xb) return;
    if (mode == Pause) resume_game();
    else if (mode == Continue || mode == PracticeResult) close_overlay();
    const bool quit_app = ctx.config_flags != nullptr && (*ctx.config_flags & 0x2000u) != 0;
    switch (cursor.index) {
    case Resume:
        if (mode == Pause) {
            resume_sound();
            bgm(7, 0, "UnPause");
        } else if (mode == Continue) {
            if (practice == 0) continue_game();
            else ctx.globals->scene_request = 10;
        }
        break;
    case ToTitle:
        ctx.animations->interrupt(screenshot_vm, 1);
        ctx.animations->interrupt(menu_vm, 1);
        ctx.globals->scene_request = quit_app ? 2 : 4;
        break;
    case Restart:
        ctx.animations->kill(screenshot_vm);
        screenshot_vm = 0;
        ctx.animations->kill(menu_vm);
        menu_vm = 0;
        ctx.globals->scene_request = ctx.game->mode != 0 ? 11 : 10;
        break;
    default:
        break;
    }
    set_mode(Closed);
}

// 成绩文件机体块下标：(shot + shot_aux) × 0x5318
int GameMenu::score_block() const {
    const i32 block = ctx.globals->shot + ctx.globals->shot_aux;
    return block >= 0 && block < ScoreFile::kShots ? block : 0;
}

// 0x43f7e0：续关首帧的排名登记；返回 true 时进入名字输入（光标槽数 0x19、光标停在名次上）
bool GameMenu::register_rank() {
    GameGlobals& g = *ctx.globals;
    const u32 game_mode = (g.flags >> 4) & 3u;
    if (game_mode != 2) {
        if (game_mode == 0) {
            // Extra 练习结算以关卡 9 记录
            if (g.stage == 7 && practice != 0) g.stage = 9;
            const i32 rank = ctx.scores->insert_rank(score_block(), g.difficulty, u32(g.score), u8(g.stage),
                                                     u8(g.continues), g.season, 0, 0);
            if (g.stage == 9 && practice != 0) g.stage = 7;
            if (rank >= 0) {
                cursor.count = 0x19;
                cursor.wrap = true;
                cursor.set(rank);
                begin_name_entry();
                no_rank = 0;
                return true;
            }
        } else {
            ctx.scores->update_practice(score_block(), g.difficulty, g.stage, u32(g.score));
        }
    }
    no_rank = 1;
    return false;
}

// 名字输入初值：光标 0、91 项、名字取成绩文件最近一次输入；与默认 8 空格不同则光标回退一格；长度为末尾非空格位
void GameMenu::begin_name_entry() {
    name_cursor.set(0);
    name_cursor.count = kNameCharCount;
    name_entry_active = 1;
    std::memcpy(name, ctx.scores->stats.last_name, 9);
    name_length = 0;
    if (std::strcmp(name, ScoreFile::kDefaultName) != 0) name_cursor.move(-1);
    i32 len = 8;
    while (len > 0 && name[len - 1] == ' ') --len;
    name_length = len;
}

// LAB_004404ae：续关 / 结算主菜单（front.anm 0xa0 续关、0xa1 结算）
void GameMenu::enter_main_menu_after_rank() {
    set_state(MainMenu);
    cursor.count = 5;
    cursor.wrap = true;
    AnmManager& anm = *ctx.animations;
    const bool spell_practice_result = (flags & kFlagSpellPractice) != 0;
    if (!spell_practice_result && (ctx.globals->flags & GameGlobals::kModeMask) == 0) {
        menu_vm = front != nullptr ? anm.spawn_ui(*front, 0xa0) : 0;
        if (ctx.globals->continues > 0) cursor.disable(SaveReplay);
        i32 start = Resume;
        if (ctx.globals->continues_left < 1) {
            cursor.disable(Resume);
            start = ToTitle;
        }
        cursor.set(start);
    } else {
        menu_vm = front != nullptr ? anm.spawn_ui(*front, 0xa1) : 0;
        cursor.disable(Resume);
        cursor.disable(Manual);
        cursor.set(0);
        if (!spell_practice_result) cursor.set(Restart);
    }
    anm.interrupt_now(menu_vm, 3);
    cursor_interrupt(7);
}

// case 10：进入回放槽列表
void GameMenu::enter_replay_slots() {
    flags = (flags & ~kFlagNameEntry) | kFlagReplaySave;
    set_state(ReplaySlot);
    hide_menu();
    cursor.push();
    cursor.count = kReplaySlots;
    cursor.wrap = true;
    cursor.set(0);
    for (int i = 0; i < kReplaySlots; ++i) replay_slot_used[i] = ctx.replays != nullptr && ctx.replays->slot_exists(i + 1);
}

// 0x43f980
void GameMenu::run_state_machine() {
    AnmManager& anm = *ctx.animations;
    GameGlobals& g = *ctx.globals;
    const u32 pressed = ctx.input->pressed;
    const u32 score_at_entry = u32(g.score);

    switch (state) {
    case PauseIntro:
        if (timer.current <= 9) return;
        set_state(MainMenu);
        cursor.count = 5;
        if (ctx.game->mode != 0) {
            cursor.disable(SaveReplay);
            cursor.disable(Manual);
        }
        if (g.continues > 0) {
            // 已续关：不能存回放，相关子脚本直接退场
            cursor.disable(SaveReplay);
            cursor_interrupt(7);
            child_interrupt(0x82, 5);
            child_interrupt(0x8b, 5);
            child_interrupt(0x97, 5);
        }
        cursor.wrap = true;
        cursor.set(Resume);
        cursor_interrupt(7);
        kind = 0;
        return;

    case PracticeIntro:
        if (timer.current <= 9) return;
        set_state(MainMenu);
        cursor.count = 5;
        cursor.disable(Manual);
        cursor.disable(SaveReplay);
        cursor.disable(Resume);
        cursor.wrap = true;
        cursor.set(ToTitle);
        cursor_interrupt(7);
        kind = 1;
        return;

    case ContinueIntro:
    case PracticeResultIntro:
    case ResultIntro4:
    case SpellPracticeIntro:
        if (timer.current < 10) return;
        ctx.gui->displayed_score = g.score;
        if (g.hiscore < i32(score_at_entry)) g.hiscore = i32(score_at_entry);
        if (register_rank()) {
            set_state(RankName);
            hide_menu();
            return;
        }
        enter_main_menu_after_rank();
        return;

    case MainMenu: {
        cursor_previous = cursor.index;
        if (nav(InputState::kUp)) cursor.move(-1);
        if (nav(InputState::kDown)) cursor.move(1);
        if (cursor_previous != cursor.index) {
            cursor_interrupt(7);
            sfx(10);
        }
        if ((pressed & InputState::kConfirmMask) != 0) {
            sfx(7);
            const i32 item = cursor.index;
            if (item == Resume) {
                anm.interrupt(screenshot_vm, 1);
                anm.interrupt(menu_vm, 1);
                set_state(Execute);
            } else if (item >= ToTitle && item <= Restart) {
                for (i32 script : kItemScripts[item]) if (script != 0) child_interrupt(script, 6);
                State next;
                if (item == SaveReplay) next = mode == Pause ? ConfirmQuit : ReplayPrompt;
                else if (item == Manual) next = HelpPages;
                else next = (ctx.game->mode == 0 && mode == Pause) ? ConfirmRestart : Execute;
                set_state(next);
            }
            timer.set(0);
        }
        if (kind == 0) {
            if ((pressed & InputState::kR) != 0) {
                // R：直接重新开始
                sfx(7);
                child_interrupt(0x84, 6);
                anm.interrupt(screenshot_vm, 1);
                cursor.set(Restart);
                set_state(Execute);
            }
            if ((pressed & InputState::kEscape) != 0) {
                anm.interrupt(screenshot_vm, 1);
                anm.interrupt(menu_vm, 1);
                cursor.set(Resume);
                set_state(Execute);
                return;
            }
        }
        if ((pressed & InputState::kQ) != 0) {
            // Q：直接回标题
            sfx(7);
            child_interrupt(0x81, 6);
            cursor.set(ToTitle);
            set_state(Execute);
        }
        return;
    }

    case ConfirmRestart:
    case ConfirmQuit: {
        if (timer.current < 0x14) return;
        if (timer.current == 0x14) {
            cursor.push();
            cursor.count = 2;
            cursor.wrap = true;
            cursor.set(1);
            anm.interrupt(menu_vm, 0xe);
        }
        if (timer.current < 0x1e) return;
        if (timer.current == 0x1e) cursor_interrupt(0xf);
        cursor_previous = cursor.index;
        if (nav(InputState::kUp)) cursor.move(-1);
        if (nav(InputState::kDown)) cursor.move(1);
        if (cursor_previous != cursor.index) {
            cursor_interrupt(0xf);
            sfx(10);
        }
        if ((pressed & InputState::kConfirmMask) != 0) {
            if (cursor.index == 0) {
                child_interrupt(0x9a, 6);
                set_state(state == ConfirmQuit ? ReplayPrompt : ConfirmDone);
                sfx(7);
            } else if (cursor.index == 1) {
                child_interrupt(0x9b, 6);
                set_state(ConfirmDone);
                sfx(9);
            }
        }
        if ((pressed & InputState::kCancelMask) != 0) {
            sfx(9);
            if (cursor.index == 0) {
                cursor.set(1);
                cursor_interrupt(0xf);
            } else if (cursor.index == 1) {
                child_interrupt(0x9b, 6);
                set_state(ConfirmDone);
            }
        }
        if ((pressed & InputState::kEscape) != 0) {
            anm.interrupt(screenshot_vm, 1);
            anm.interrupt(menu_vm, 1);
            cursor.set(Resume);
            set_state(Execute);
        }
        return;
    }

    case ConfirmDone:
        if (timer.current < 0x14) return;
        if (cursor.index == 0) {
            anm.interrupt(menu_vm, 1);
            cursor.pop();
            set_state(Execute);
        } else if (cursor.index == 1) {
            cursor.pop();
            if (g.continues > 0) cursor.disable(SaveReplay);
            cursor_interrupt(7);
            set_state(MainMenu);
        }
        return;

    case ReplayPrompt:
        if (timer.current > 0x13) enter_replay_slots();
        return;

    case ReplaySlot: {
        if (timer.current < 10) return;
        cursor_previous = cursor.index;
        if (nav(InputState::kUp)) cursor.move(-1);
        if (nav(InputState::kDown)) cursor.move(1);
        if (cursor_previous != cursor.index) sfx(10);
        if ((pressed & InputState::kConfirmMask) == 0) {
            if ((pressed & InputState::kCancelMask) == 0) return;
            flags &= ~(kFlagReplaySave | kFlagNameEntry);
            cursor.pop();
            cursor.count = 5;
            cursor.wrap = true;
            cursor_interrupt(7);
            for (bool& used : replay_slot_used) used = false;
            if (mode == Pause) {
                set_state(Execute);
                cursor.set(ToTitle);
                sfx(9);
                return;
            }
            set_state(MainMenu);
            show_menu();
            if ((g.flags & GameGlobals::kModeMask) != 0) {
                cursor.disable(Resume);
                cursor.disable(Manual);
            }
            sfx(9);
            return;
        }
        flags = (flags & ~kFlagReplaySave) | kFlagNameEntry;
        set_state(ReplayName);
        // 回放头：时间戳与关卡号（0x4483b0(1) 练习段）由 ReplayStore::save 时写入
        begin_name_entry();
        sfx(7);
        return;
    }

    case ReplayName:
    case RankName: {
        if (timer.current < 10) return;
        name_cursor_previous = name_cursor.index;
        if (nav(InputState::kUp)) name_cursor.move(-kNameColumns);
        if (nav(InputState::kDown)) name_cursor.move(kNameColumns);
        if (nav(InputState::kLeft)) name_cursor.move(name_cursor.index % kNameColumns == 0 ? kNameColumns - 1 : -1);
        if (nav(InputState::kRight)) name_cursor.move(name_cursor.index % kNameColumns == kNameColumns - 1 ? -(kNameColumns - 1) : 1);
        if (name_cursor_previous != name_cursor.index) sfx(10);
        if ((pressed & InputState::kConfirmMask) == 0) {
            if ((pressed & InputState::kCancelMask) != 0) {
                sfx(9);
                if (name_length != 0) {
                    --name_length;
                    name[name_length] = ' ';
                    return;
                }
                if (state == ReplayName) {
                    flags = (flags & ~kFlagNameEntry) | kFlagReplaySave;
                    set_state(ReplaySlot);
                    return;
                }
            }
            return;
        }
        const i32 pick = name_cursor.index;
        if (pick < kNameSpace) {
            if (name_length > 7) {
                name[7] = kNameChars[pick];
                sfx(7);
                return;
            }
            name[name_length] = kNameChars[pick];
        } else if (pick == kNameSpace) {
            if (name_length > 7) {
                name[7] = ' ';
                sfx(7);
                return;
            }
            name[name_length] = ' ';
        } else if (pick == kNameBackspace) {
            if (name_length != 0) {
                --name_length;
                name[name_length] = ' ';
                sfx(9);
            }
            return;
        } else if (pick == kNameEnd) {
            if (state == ReplayName) {
                flags = (flags & ~kFlagNameEntry) | kFlagReplaySave;
                sfx(0x11);
                if (ctx.replays != nullptr) ctx.replays->save(cursor.index + 1, name);
                replay_slot_used[cursor.index] = ctx.replays != nullptr && ctx.replays->slot_exists(cursor.index + 1);
                set_state(ReplaySlot);
                std::memcpy(name, ctx.scores->stats.last_name, 9);
                sfx(7);
                return;
            }
            sfx(7);
            std::memcpy(ctx.scores->blocks[score_block()].ranks[g.difficulty][cursor.index].name, name, 9);
            std::memcpy(ctx.scores->stats.last_name, name, 9);
            enter_main_menu_after_rank();
            return;
        } else {
            sfx(7);
            return;
        }
        ++name_length;
        if (name_length > 7) name_cursor.set(kNameEnd);
        sfx(7);
        return;
    }

    case HelpPages:
        if (timer.current == 0x14) {
            hide_menu();
            delete help;
            help = new HelpScreen();
            help->anm = ctx.load_help_anm ? ctx.load_help_anm() : nullptr;
            help->scroll = 32.0f;
        }
        if (help != nullptr) {
            if (help->anm == nullptr && ctx.load_help_anm) help->anm = ctx.load_help_anm();
            if (help->anm != nullptr) {
                help->update(anm, *ctx.input, *ctx.game_speed);
                sfx_queue.insert(sfx_queue.end(), help->sfx_queue.begin(), help->sfx_queue.end());
                help->sfx_queue.clear();
            }
            if (help->closed) {
                delete help;
                help = nullptr;
                set_state(MainMenu);
                show_menu();
            }
        }
        return;

    case Execute:
        execute_selection();
        return;

    default:
        return;
    }
}

// 0x43e4c0
void GameMenu::destroy() {
    for (bool& used : replay_slot_used) used = false;
    delete help;
    help = nullptr;
}

// ---- 绘制链 0x4a ----

// 0x43edc0
void GameMenu::draw(AsciiManager& text, ScreenCompositor& compositor) {
    text.style.shadow = true;
    // 原版连查三次同一 id（每次失败都清零 id），效果等于查一次
    if (AnmVm* frame = ctx.animations->find(screenshot_vm)) {
        if (const AnmVm* tint = AnmManager::find_child(*frame, 0x39, 0)) {
            u32 argb;
            std::memcpy(&argb, tint->color, 4);   // VM 颜色字（b,g,r,a 字节序即小端 ARGB 字）
            compositor.set_pause_tint(argb);
        }
    } else {
        screenshot_vm = 0;
    }

    const u32 overlay = flags & (kFlagReplaySave | kFlagNameEntry);
    switch (mode) {
    case Pause:
    case PracticeResult:
        if (overlay == kFlagNameEntry) draw_replay_name_entry(text);
        else if (overlay == kFlagReplaySave) draw_replay_slots(text);
        break;
    case Continue:
        if (state == RankName) draw_score_ranking(text);
        if (overlay == kFlagReplaySave) draw_replay_slots(text);
        else if (overlay == kFlagNameEntry) draw_replay_name_entry(text);
        else if (state != HelpPages) text.printf({184.0f, 448.0f, 0.0f}, "Credit %d", ctx.globals->continues_left);
        break;
    default:
        break;
    }
    text.style.shadow = false;
}

// 0x43e730：名字 + 黄色下划线光标，下方 13 列 × 7 行的字符表（当前项黄、其余灰）
void GameMenu::draw_name_entry(AsciiManager& text, Vec3 pos) const {
    text.printf(pos, "%s", name);
    pos.x += float(name_length * 9);
    if (name_length == 8) pos.x -= 9.0f;   // 第 8 字已满：光标压在末字上
    text.style.color = kListHighlight;
    text.print(pos, "_");
    text.style.color = 0xffffffff;

    Vec3 cell{112.0f, 320.0f, 0.0f};
    for (i32 i = 0; i < kNameCharCount; ++i) {
        text.style.color = name_cursor.index == i ? kListHighlight : kListNormal;
        u8 glyph;
        if (i < kNameSpace) glyph = u8(kNameChars[i]);
        else if (i == kNameSpace) glyph = kNameSpaceGlyph;
        else if (i == kNameBackspace) glyph = kNameBackspaceGlyph;
        else glyph = kNameEndGlyph;
        text.printf(cell, "%c", glyph);
        if (i % kNameColumns == kNameColumns - 1) {
            cell.x = 112.0f;
            cell.y += 16.0f;
        } else {
            cell.x += 18.0f;
        }
    }
    text.style.color = 0xffffffff;
}

// 0x43e8c0：回放槽一行——"No.NN 名字 年/月/日 机体 季节 难度 关卡"，符卡练习改为 "Sp 符卡号"
void GameMenu::draw_replay_row(AsciiManager& text, Vec3 pos, int slot, const ReplaySummary& summary) const {
    const CivilDate date = civil_date(summary.timestamp);
    const char* shot = lookup(kShotNames, summary.shot + summary.shot_aux);
    const char* season = lookup(kSeasonShortNames, summary.season);
    if (summary.spell_practice) {
        text.printf(pos, "No.%.2d %s %.2d/%.2d/%.2d %s%s Sp %3d", slot, summary.name,
                    date.year % 100, date.month, date.day, shot, season, summary.spell_id + 1);
        return;
    }
    text.printf(pos, "No.%.2d %s %.2d/%.2d/%.2d %s%s %s %s", slot, summary.name,
                date.year % 100, date.month, date.day, shot, season,
                lookup(kDifficultyShortNames, summary.difficulty), lookup(kStageShortNames, summary.stage));
}

// 0x43e9b0：25 个回放槽，行距 15，空槽画占位横线
void GameMenu::draw_replay_slots(AsciiManager& text) const {
    Vec3 pos{36.0f, 64.0f, 0.0f};
    for (int slot = 1; slot <= kReplaySlots; ++slot) {
        text.style.color = cursor.index == slot - 1 ? kListHighlight : kListNormal;
        ReplaySummary summary;
        if (replay_slot_used[slot - 1] && ctx.replays != nullptr && ctx.replays->slot_summary(slot, summary)) {
            draw_replay_row(text, pos, slot, summary);
        } else {
            text.printf(pos, "No.%.2d -------- --/--/-- ------ -- -- St-", slot);
        }
        pos.y += 15.0f;
    }
    text.style.color = 0xffffffff;
}

// 0x43eb50：回放名字输入——选中行在前 10 帧从列表位置滑到 y = 224，名字输入画在 x = 90，
// 其右侧（x = 36 起）重画本局回放头那一行（名字栏先清成 8 个空格，所以显示为空）
void GameMenu::draw_replay_name_entry(AsciiManager& text) const {
    float y = 224.0f;
    if (timer.current < 10) {
        const float from = float(cursor.index) * 15.0f + 64.0f;
        y = (224.0f - from) * float(timer.current) / 10.0f + from;
    }
    draw_name_entry(text, {90.0f, y, 0.0f});

    const GameGlobals& g = *ctx.globals;
    ReplaySummary current;
    current.timestamp = ctx.replay_timestamp;
    current.spell_practice = (g.flags & GameGlobals::kModeMask) == 0x20;
    current.shot = g.shot;
    current.shot_aux = g.shot_aux;
    current.difficulty = g.difficulty;
    current.stage = g.stage;
    current.spell_id = g.spell_id;
    current.season = g.season;
    draw_replay_row(text, {36.0f, y, 0.0f}, cursor.index + 1, current);
}

// 0x43ec10：Game Over 排名——标题、入榜行的名字输入（x = 75）、本机体 × 难度的 10 条记录（行距 18）
void GameMenu::draw_score_ranking(AsciiManager& text) const {
    text.printf({48.0f, 64.0f, 0.0f}, "            Score Ranking!!");
    const i32 highlight = no_rank == 0 ? cursor.index : -1;
    if (no_rank == 0) draw_name_entry(text, {75.0f, float(cursor.index) * 18.0f + 96.0f, 0.0f});

    const GameGlobals& g = *ctx.globals;
    Vec3 pos{48.0f, 96.0f, 0.0f};
    for (i32 rank = 0; rank < ScoreFile::kRanks; ++rank) {
        text.style.color = rank == highlight ? kListHighlight : kListNormal;
        const ScoreEntry& entry = ctx.scores->blocks[score_block()].ranks[g.difficulty][rank];
        if (entry.time == 0) {
            text.printf(pos, "%2d %s %9ld%d --/--/-- Stage -", rank + 1, entry.name, long(entry.score), i32(entry.continues));
        } else {
            const CivilDate date = civil_date(entry.time);
            text.printf(pos, "%2d %s %9ld%d %.2d/%.2d/%.2d %s", rank + 1, entry.name, long(entry.score), i32(entry.continues),
                        date.year % 100, date.month, date.day, lookup(kStageNames, i32(entry.stage)));
        }
        pos.y += 18.0f;
    }
    text.style.color = 0xffffffff;
}

}  // namespace th16
