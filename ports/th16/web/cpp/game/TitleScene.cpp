#include "TitleScene.hpp"
#include "Ascii.hpp"
#include "GameMenu.hpp"
#include "ReplayFile.hpp"
#include "ReplayRecording.hpp"
#include "ScoreFile.hpp"
#include "SpellPracticeTable.hpp"
#include "TextPaint.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <strings.h>

namespace th16 {

namespace {
constexpr i32 kDecor61 = 0x61;
constexpr i32 kDecor65 = 0x65;
constexpr i32 kDifficultyBack = 0x68;
constexpr i32 kShotBack = 0x69;
constexpr i32 kSeasonBack = 0x6a;
constexpr i32 kStageBack = 0x71;
constexpr i32 kShotPortrait = 0x5c;  // +0x430：机体画面左右切换时淡出的 VM
constexpr i32 kExtraDifficulty = 4;

i32 to_short(i32 v) { return i32(i16(v)); }
}  // namespace

u32 TitleScene::child_id(AnmManager& anm, i32 slot, i32 script) {
    AnmVm* parent = anm.find(vm[slot]);
    if (parent == nullptr) { vm[slot] = 0; return 0; }
    AnmVm* child = AnmManager::find_child(*parent, script, 0);
    return child ? child->id : 0;
}

void TitleScene::item_interrupt_now(AnmManager& anm, i32 script, i32 value) {
    const u32 id = child_id(anm, kLogo, script);
    AnmVm* item = anm.find(id);
    if (item == nullptr) return;
    item->pending_interrupt = value;
    anm.run(*item);
}

void TitleScene::item_interrupt(AnmManager& anm, i32 script, i32 value) {
    AnmVm* item = anm.find(child_id(anm, kLogo, script));
    if (item) item->pending_interrupt = value;
}

void TitleScene::dismiss(AnmManager& anm, i32 slot) {
    anm.interrupt(vm[slot], 1);
    vm[slot] = 0;
}

// 0x46f340：根与整棵子树清可见位
void TitleScene::hide_medal(AnmManager& anm, u32 id) {
    if (AnmVm* root = anm.find(id)) AnmManager::hide_tree(*root);
}

void TitleScene::spawn_into(AnmManager& anm, i32 slot, AnmFile& file, i32 script) {
    vm[slot] = anm.spawn(file, script, -1);
}

void TitleScene::request_game(i32 stage, i32 table) {
    globals->stage_table = table;
    globals->stage = stage;
    globals->stage_copy = stage;
    globals->scene_request = kSceneGame;
    events.push_back({"scene", kSceneGame});
}

void TitleScene::transition_effect() {
    // 0x41a390：ascii.anm 0x11（Loading 文字）；0x418ba0：EffectManager 种类 0 过场；0x46f0b0(..., 7) 中断 7
    events.push_back({"transition", 7});
    if (on_transition_effect) on_transition_effect();
}

void TitleScene::tick_idle_demo(const InputState& input) {
    // 0x44af80 开头：画面==1 时 DAT_004a5bf0++；held==0 且 ≥0x708 → bit6 demo + 场景 0xd
    if (screen != kMainMenu || globals == nullptr) return;
    ++idle_frames;
    if (input.held == 0) {
        if (idle_frames < 0x708) return;
        GameGlobals& g = *globals;
        g.flags = (g.flags & ~0x80u) | 0x40u;
        g.demo_saved_difficulty = g.difficulty;  // DAT_004a59c0
        g.title_origin = 1;
        pending_demo = demo_index;
        events.push_back({"demo", demo_index});
        const i32 n = demo_index + 1;
        const i64 prod = i64(n) * 0x55555555ll;
        i32 edx = i32(u32(u64(prod) >> 32));
        edx = edx - n;
        edx >>= 1;
        i32 ecx = edx;
        ecx = i32(u32(ecx) >> 31);
        ecx = ecx + edx;
        demo_index = ecx * 3 + n;
        g.scene_request = 0xd;
        events.push_back({"scene", g.scene_request});
        idle_frames = 0;
    } else {
        idle_frames = 0;
    }
}

// 0x44af80
void TitleScene::update(AnmManager& anm, const InputState& input) {
    if (title_file == nullptr) return;
    tick_idle_demo(input);
    tick_title_bgm();
    switch (screen) {
    case kNone: {
        // 0x44af80 case 0 开头：0x46f270 标删 front / bullet / ascii / text 槽的链表 VM，
        // 再 0x41a360 给启动幕 Loading VM 挂中断 1 并清句柄。下一帧 UI tick 回收落叶。
        anm.kill_file(5);
        anm.kill_file(7);
        anm.kill_file(AnmManager::kAsciiSlot);
        anm.kill_file(0);
        if (loading_vm != nullptr && *loading_vm != 0) {
            anm.interrupt(*loading_vm, 1);
            *loading_vm = 0;
        }
        // 0x44af80 case 0：按 DAT_004a6f1c（title_origin）分流
        GameGlobals& g = *globals;
        const i32 origin = g.title_origin;
        if (origin == 3) {
            // 通关回标题：压栈主光标后落入 case 0xe 名字输入 / 排名登记（0x4532f0）
            cursor.count = 10;
            cursor.set(0);
            cursor.push();
            set_screen(kNameEntry);
            g.title_origin = 1;
            update_name_entry(anm, input);
            break;
        }
        // +0x5ce8 bit0：非 demo 返回 10 帧后重播 th16_01；demo 返回则还原 DAT_004a59c0 难度
        if ((g.flags & 0x40u) == 0) {
            title_bgm_pending = true;
            title_bgm_delay = 0;
        } else {
            title_bgm_pending = false;
            g.difficulty = g.demo_saved_difficulty;
        }
        g.flags &= ~0x40u;
        first_entry = origin == 0;
        if (origin == 0) {
            set_screen(kMainMenu);
            g.title_origin = 1;
        } else if (origin == 1) {
            if (g.difficulty == kExtraDifficulty) cursor.set(1);
            set_screen(kMainMenu);
        } else if (origin == 2) {
            // 回放结束：0x43c370 th16_01 + 0x43c3f0(0, 0)，模式清 0，主光标压栈停在 Replay
            bgm_default();
            if ((g.input_config & 0x10u) != 0) play_bgm_op(4);
            play_bgm_op(2);
            if (scores != nullptr) scores->mark_bgm_heard(0);
            g.set_mode(0);
            cursor.count = 10;
            cursor.set(4);
            cursor.push();
            set_screen(kReplay);
            g.title_origin = 1;
            update_replay(anm, input);
            break;
        } else if (origin == 4 || origin == 5) {
            // 练习 / 符卡练习返回：kind 9 淡入 + 装饰 0x61 interrupt 3，同帧进难度选择 / 符卡练习
            if (on_screen_fade) on_screen_fade(9, 0x1e, 0);
            spawn_into(anm, kDecor61, *title_file, kDecor61);
            anm.interrupt_now(vm[kDecor61], 3);
            if (origin == 5) {
                set_screen(kSpellPractice);
                g.title_origin = 1;
                update_spell_practice(anm, input);
            } else {
                set_screen(kDifficulty);
                update_difficulty(anm, input);
            }
            break;
        }
        update_main_menu(anm, input);
        break;
    }
    case kMainMenu:
        update_main_menu(anm, input);
        break;
    case kFinished: {
        // 0x44af80 case 2：DAT_004c17c4 = bit13 ? 2 : 3，再经 0x43c440 停 BGM（DAT_004c12cc bit4 → 4，否则 3）。
        // 游戏开始路径在上一帧已置 7（Supervisor 先于本链消费并销毁标题），不会走到这里
        if (globals->scene_request < 0) {
            const bool halt = config_flags != nullptr && (*config_flags & 0x2000u) != 0;
            globals->scene_request = halt ? 2 : 3;
            events.push_back({"scene", globals->scene_request});
            play_bgm_op((globals->input_config & 0x10u) != 0 ? 4 : 3);
        }
        break;
    }
    case kDifficulty: update_difficulty(anm, input); break;
    case kShot: update_shot(anm, input); break;
    case kSeason: update_season(anm, input); break;
    case kStage: update_stage(anm, input); break;
    case kOption: update_option(anm, input); break;
    case kReplay: update_replay(anm, input); break;
    case kManual: update_manual(anm, input); break;
    case kMusicRoom: update_music(anm, input); break;
    case kSpellPractice: update_spell_practice(anm, input); break;
    case kSpellList: update_spell_list(anm, input); break;
    case kSpellShot: update_spell_shot(anm, input); break;
    case kSpellStart: update_spell_start(anm, input); break;
    case kScore: update_score(anm, input); break;
    case kNameEntry: update_name_entry(anm, input); break;
    case kNameConfirm: update_name_confirm(anm, input); break;
    case kKeyConfig: update_key_config(anm, input); break;
    default: update_stub(anm, input); break;
    }
    ++timer;  // 0x44af80 末尾
}

void TitleScene::refresh_spell_slot_names(AnmManager& anm, i32 list_index) {
    // 0x4560b0(group, list_index, +0x5dd0)：名 VM 0xd2+i、按难度填 slot id、0x46d990 刷名行
    if (title_file == nullptr) return;

    // SJIS（th16.exe 栈上常量）：挑戦可能！ / オーバードライブモード挑戦可能！；DAT_0049364c 未解锁行
    static constexpr char kChallenge[] = "\x92\xa7\x90\xed\x89\xc2\x94\x5c\x81\x49";
    static constexpr char kOverdriveChallenge[] =
        "\x83\x49\x81\x5b\x83\x6f\x81\x5b\x83\x68\x83\x89\x83\x43\x83\x75\x83\x82\x81\x5b\x83\x68"
        "\x92\xa7\x90\xed\x89\xc2\x94\x5c\x81\x49";
    static constexpr char kLockedFmt[] =
        " No.%3d  \x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48";
    static constexpr i32 kDiffMarker = 0x10d;  // +0x6f4：难度角标 0x10d + 难度（7 槽）
    static constexpr size_t kBodyWidth = 0x2a;

    for (i32& id : spell_slot_ids) id = -2;
    for (int i = 0; i < 7; ++i) anm.kill(vm[kDiffMarker + i]);

    for (int i = 0; i < 5; ++i) {
        if (anm.find(row_text_vms[i]) == nullptr) {
            row_text_vms[i] = anm.spawn(*title_file, 0xd2 + i, -1);
        }
        AnmVm* name_vm = anm.find(row_text_vms[i]);
        if (name_vm == nullptr) {
            row_text_vms[i] = 0;
            continue;
        }
        anm.interrupt_now(row_text_vms[i], 2);
        text_paint_vm(anm, *name_vm, 0x808080u, 0, 0, 0, 0, " Nothing ... ", text_paints);
    }
    if (spell_group != 6) {
        for (int i = 0; i < 4; ++i) {
            spawn_into(anm, kDiffMarker + i, *title_file, kDiffMarker + i);
            anm.interrupt_now(vm[kDiffMarker + i], 3);
        }
    }

    i32 raw[5];
    SpellPracticeTable::raw_slot_ids(spell_group, list_index, raw);
    i32 max_used = 0;
    for (int n = 0; n < 5; ++n) {
        const i32 id = raw[n];
        if (id < 0) break;
        const i32 diff = SpellPracticeTable::spell_difficulty(id);
        i32 dest;
        if (spell_group == 6) {
            if (diff == 4) spawn_into(anm, 0x111, *title_file, 0x111);       // +0x704
            else if (diff == 5) spawn_into(anm, 0x113, *title_file, 0x113);  // +0x70c
            dest = diff != 4 ? 1 : 0;
        } else {
            if (diff > 4) {
                spawn_into(anm, 0x112, *title_file, 0x112);  // +0x708
                anm.interrupt_now(vm[0x112], 3);
            }
            dest = diff < 5 ? diff : 4;
        }

        const SpellRecord* all = scores != nullptr && id < ScoreFile::kSpellRecords
            ? &scores->blocks[ScoreFile::kAllShotsBlock].spells[id] : nullptr;
        // 难度 > 4（Overdrive）：本槽之前的符卡在全机体块都已收取过才开放
        bool chain_open = false;
        if (diff > 4) {
            chain_open = true;
            for (int p = 0; p < n; ++p) {
                const SpellRecord* prev = scores != nullptr && raw[p] < ScoreFile::kSpellRecords
                    ? &scores->blocks[ScoreFile::kAllShotsBlock].spells[raw[p]] : nullptr;
                if (prev == nullptr || (prev->captures[0] == 0 && prev->captures[1] == 0)) {
                    chain_open = false;
                    break;
                }
            }
        }

        AnmVm* name_vm = anm.find(row_text_vms[dest]);
        if (name_vm == nullptr) row_text_vms[dest] = 0;
        char line[0xc1 + 0x10] = {};
        if ((all != nullptr && all->attempts[0] != 0) || chain_open) {
            spell_slot_ids[dest] = id;
            char body[0xc1] = {};
            if (all == nullptr || (all->attempts[0] == 0 && all->attempts[1] == 0)) {
                std::snprintf(body, sizeof(body), "%s", diff < 5 ? kChallenge : kOverdriveChallenge);
            } else {
                std::snprintf(body, sizeof(body), "%s", all->name);
            }
            const size_t len = std::strlen(body);
            if (len < kBodyWidth) std::memset(body + len, ' ', kBodyWidth - len);
            std::snprintf(line, sizeof(line), " No.%3d  %s", id + 1, body);
            const SpellRecord* rec = scores != nullptr ? scores->spell_record(spell_shot_cursor.index, id) : nullptr;
            const u32 color = rec != nullptr && rec->captures[1] != 0 ? 0xffff80u : 0xefefefu;
            if (name_vm != nullptr) text_paint_vm(anm, *name_vm, color, 0, 0, 0, 0, line, text_paints);
        } else {
            spell_slot_ids[dest] = -1;
            std::snprintf(line, sizeof(line), kLockedFmt, id + 1);
            if (name_vm != nullptr) text_paint_vm(anm, *name_vm, 0xb0b0b0u, 0, 0, 0, 0, line, text_paints);
        }
        anm.interrupt_now(row_text_vms[dest], 2);
        if (dest > max_used) max_used = dest;
    }

    const i32 shown = spell_group == 6 ? 2 : 5;
    for (int i = 0; i < shown; ++i) anm.interrupt_now(row_text_vms[i], 3);
    for (int i = max_used + 1; i < 5; ++i) {
        anm.kill(row_text_vms[i]);
        row_text_vms[i] = 0;
    }
}

void TitleScene::draw(AsciiManager& ascii) {
    // 0x44b530
    switch (screen) {
    case kStage: draw_stage_practice_scores(ascii); break;  // 0x4513c0
    case kScore: draw_player_data(ascii); break;            // 0x453030
    case kNameEntry: draw_name_entry_ranks(ascii); break;   // 0x4538b0
    case kNameConfirm: draw_name_confirm(ascii); break;     // 0x4541b0
    case kReplay: draw_replay_list(ascii); break;           // 0x451d50
    case kSpellList:
    case kSpellShot:
    case kSpellStart: draw_spell_hud(ascii); break;         // 0x456d50（0x14 亦走 SCORE）
    default: break;
    }
}

void TitleScene::draw_stage_practice_scores(AsciiManager& ascii) {
    // 0x4513c0：sub∈[2,3] 且（timer>10 或 sub==3）时打 Stage 1..6 练习分
    if (sub != 2 && sub != 3) return;
    if (timer <= 10 && sub != 3) return;
    if (scores == nullptr || globals == nullptr) return;

    static constexpr const char* kStageNames[7] = {
        "test   ", "Stage 1", "Stage 2", "Stage 3", "Stage 4", "Stage 5", "Stage 6",
    };
    const i32 shot = globals->shot + globals->shot_aux;
    const i32 diff = globals->difficulty;
    float y = 192.0f;
    ascii.style.shadow = true;
    ascii.style.group = AsciiManager::Group::Ui;
    for (int stage = 1; stage <= 6; ++stage) {
        const PracticeRecord& rec = scores->blocks[shot].practice[diff * 8 + stage];
        const bool selected = (cursor.index == stage - 1);
        if (selected) {
            if (rec.reached == 0) ascii.style.color = 0xffdfdfdfu;
            else if (sub == 3) {
                // 闪烁：timer & 3 在 {2,3} 时黑
                const u32 phase = u32(timer) & 3u;
                ascii.style.color = (phase >= 2) ? 0xff000000u : 0xffffff00u;
            } else {
                ascii.style.color = 0xffffff00u;
            }
        } else {
            ascii.style.color = 0xff808080u;
        }
        if (rec.reached == 0) {
            ascii.printf(Vec3{240.0f, y, 0.0f}, "%s  ---------", kStageNames[stage]);
        } else {
            ascii.printf(Vec3{240.0f, y, 0.0f}, "%s  %.8d0", kStageNames[stage], int(rec.score));
        }
        y += 18.0f;
    }
    ascii.style.color = 0xffffffffu;
    ascii.style.shadow = false;
}

namespace {
// gmtime 拆分（UTC）：原版排名 / 回放列表的 年 / 月 / 日 / 时 / 分
struct ClockFields { i32 year, month, day, hour, minute; };
ClockFields clock_fields(i64 sec) {
    const i64 days = sec / 86400;
    const i32 tod = int(sec % 86400);
    i64 z = days + 719468;
    const i64 era = (z >= 0 ? z : z - 146096) / 146097;
    const i64 doe = z - era * 146097;
    const i64 yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    const i64 doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    const i64 mp = (5 * doy + 2) / 153;
    const i32 day = i32(doy - (153 * mp + 2) / 5 + 1);
    const i32 month = i32(mp < 10 ? mp + 3 : mp - 9);
    const i32 year = i32(yoe + era * 400 + (month <= 2 ? 1 : 0));
    return {year, month, day, tod / 3600, (tod / 60) % 60};
}

// 名字输入字符表（0x492840）；下标 0x58 / 0x59 / 0x5a 为空格 / 退格 / 结束，绘制时换成 ascii 字形 0x81 / 0x7f / 0x80
constexpr const char* kNameChars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*   ";
constexpr i32 kNameCols = 13;
constexpr i32 kNameCharCount = 0x5b;
constexpr i32 kNameSpace = 0x58, kNameBack = 0x59, kNameEnd = 0x5a;
constexpr u32 kRowHighlight = 0xffffff00u, kRowNormal = 0xff808080u;

// 回放行名称表（.rdata 0x491970 机体 / 0x4918c0 季节 / 0x491950 难度 / 0x4918f0 关卡，下标 8 = "All"）
constexpr const char* kReplayShotNames[4] = {"Reimu  ", "Cirno  ", "Aya    ", "Marisa "};
constexpr const char* kReplaySeasonNames[5] = {"Spring", "Summer", "Autumn", "Winter", "Full  "};
constexpr const char* kReplayDifficultyNames[6] = {"Easy   ", "Normal ", "Hard   ", "Lunatic", "Extra  ", "O.D.   "};
constexpr const char* kReplayStageNames[9] = {"tst", "St1", "St2", "St3", "St4", "St5", "St6", "Ex ", "All"};

template <size_t N>
const char* table_name(const char* const (&table)[N], i32 index) {
    return index >= 0 && size_t(index) < N ? table[index] : "";
}
}  // namespace

void TitleScene::print_rank_row(AsciiManager& ascii, float y, i32 rank, i32 shot, i32 diff) {
    static constexpr const char* kSeasonShort[5] = {"Sp", "Sm", "At", "Wt", "Fl"};
    static constexpr const char* kStageShort[8] = {"tst", "St1", "St2", "St3", "St4", "St5", "St6", "Ex "};
    const ScoreEntry& e = scores->blocks[shot].ranks[diff][rank];
    if (e.time == 0) {
        ascii.printf(Vec3{22.0f, y, 0.0f}, "%2d  %s  %9ld%d  ----/--/-- --:-- Season  Stage -  ---%%",
                     rank + 1, e.name, long(e.score), int(e.continues));
        return;
    }
    const ClockFields t = clock_fields(e.time);
    const char* season = (e.season >= 0 && e.season < 5) ? kSeasonShort[e.season] : "--";
    const char* stage = (e.stage >= 0 && e.stage < 8) ? kStageShort[e.stage] : "-- ";
    ascii.printf(Vec3{22.0f, y, 0.0f}, "%2d  %s  %9ld%d  %.4d/%.2d/%.2d %.2d:%.2d %s  %s  %2.1f%%",
                 rank + 1, e.name, long(e.score), int(e.continues),
                 t.year, t.month, t.day, t.hour, t.minute, season, stage, double(e.slowdown));
}

void TitleScene::draw_player_data(AsciiManager& ascii) {
    // 0x453030：sub==2 时排名页打 10 条排名（机体 +0x24、难度 +0xfc），底部三行各页都打
    if (sub != 2) return;
    if (scores == nullptr) return;

    const i32 shot = cursor.index;
    const i32 diff = score_difficulty_cursor.index;
    if (shot < 0 || shot >= ScoreFile::kAllShotsBlock) return;
    if (diff < 0 || diff >= ScoreFile::kDifficulties) return;

    ascii.style.shadow = true;
    ascii.style.group = AsciiManager::Group::Ui;
    if (score_page_cursor.index == 0) {
        float y = 160.0f;
        for (int rank = 0; rank < ScoreFile::kRanks; ++rank) {
            ascii.style.color = (~u32(rank * 0x10)) | 0xffffff00u;
            print_rank_row(ascii, y, rank, shot, diff);
            y += 18.0f;
        }
    }

    ascii.style.color = 0xffffffffu;
    // 底部：游玩次数 +0x515c / 游戏时长 +0x5160（1/100 秒 → 时:分:秒）/ 该难度通关次数 +0x5168
    const ScoreBlock& block = scores->blocks[shot];
    ascii.printf(Vec3{328.0f, 378.0f, 0.0f}, "    %5d", int(block.play_count));
    const u64 seconds = block.play_time / 100u;
    const u64 minutes = seconds / 60u;
    const u64 hours = minutes / 60u;
    ascii.printf(Vec3{328.0f, 396.0f, 0.0f}, "%3lld:%.2lld:%.2lld", (long long)hours, (long long)(minutes - hours * 60u),
                 (long long)(seconds - minutes * 60u));
    ascii.printf(Vec3{328.0f, 414.0f, 0.0f}, "    %5d", int(block.clears[diff]));
    ascii.style.shadow = false;
}

void TitleScene::draw_name_input(AsciiManager& ascii, float x, float y) {
    ascii.style.color = 0xffffffffu;
    ascii.printf(Vec3{x, y, 0.0f}, "%s", name_entry);
    float ux = float(name_cursor * 9) + x;
    if (name_cursor == 8) ux -= 9.0f;
    ascii.style.color = kRowHighlight;
    ascii.printf(Vec3{ux, y, 0.0f}, "_");
    ascii.style.color = 0xffffffffu;
    Vec3 cell{212.0f, 360.0f, 0.0f};
    for (i32 i = 0; i < kNameCharCount; ++i) {
        ascii.style.color = name_pick.index == i ? kRowHighlight : kRowNormal;
        const int glyph = i < kNameSpace ? int(kNameChars[i]) : i == kNameSpace ? 0x81 : i == kNameBack ? 0x7f : 0x80;
        ascii.printf(cell, "%c", glyph);
        if (i % kNameCols == kNameCols - 1) {
            cell.x = 212.0f;
            cell.y += 16.0f;
        } else {
            cell.x += 18.0f;
        }
    }
    ascii.style.color = 0xffffffffu;
}

void TitleScene::draw_name_entry_ranks(AsciiManager& ascii) {
    // 0x4538b0：入榜时当前名次白、其余 0xff808040；未入榜按名次渐变（同 0x453030）。原版不复位影子标志
    if (sub != 2 || scores == nullptr || globals == nullptr) return;
    const i32 shot = globals->shot + globals->shot_aux;
    const i32 diff = globals->difficulty;
    if (shot < 0 || shot >= ScoreFile::kAllShotsBlock) return;
    if (diff < 0 || diff >= ScoreFile::kDifficulties) return;

    ascii.style.shadow = true;
    float y = 160.0f;
    for (i32 rank = 0; rank < ScoreFile::kRanks; ++rank) {
        if (name_no_rank == 0) ascii.style.color = cursor.index == rank ? 0xffffffffu : 0xff808040u;
        else ascii.style.color = (~u32(rank * 0x10)) | 0xffffff00u;
        print_rank_row(ascii, y, rank, shot, diff);
        y += 18.0f;
    }
    if (name_no_rank == 0) draw_name_input(ascii, 58.0f, float(cursor.index) * 18.0f + 160.0f);
}

void TitleScene::print_replay_row(AsciiManager& ascii, float y, i32 slot, const ReplaySummary& s, const char* stage) {
    // 0x4541b0 行：机体 / 难度全名 / 关卡（无季节列）
    if (stage == nullptr) stage = table_name(kReplayStageNames, s.stage);
    const ClockFields t = clock_fields(s.timestamp);
    ascii.printf(Vec3{58.0f, y, 0.0f}, "No.%.2d %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %2.1f%%", slot, s.name,
                 t.year % 100, t.month, t.day, t.hour, t.minute, table_name(kReplayShotNames, s.shot + s.shot_aux),
                 table_name(kReplayDifficultyNames, s.difficulty), stage, double(s.slow_rate));
}

void TitleScene::draw_name_confirm(AsciiManager& ascii) {
    // 0x4541b0
    if (sub == 2) {
        ascii.style.shadow = true;
        float y = 80.0f;
        for (i32 i = 0; i < 0x19; ++i) {
            ascii.style.color = cursor.index == i ? kRowHighlight : kRowNormal;
            if (save_slot_live[i]) {
                print_replay_row(ascii, y, i + 1, save_slots[i], nullptr);
            } else {
                ascii.printf(Vec3{58.0f, y, 0.0f}, "No.%.2d -------- --/--/-- --:-- ------- ------- --- ---%%", i + 1);
            }
            y += 15.0f;
        }
    } else if (sub == 3) {
        float y = 240.0f;
        if (timer < 10) {
            const float from = float(save_slot_choice * 15 + 0x50);
            y = ((10.0f - float(timer)) * (from - 240.0f)) / 10.0f + 240.0f;
        }
        ReplaySummary current = current_replay_summary ? current_replay_summary() : ReplaySummary{};
        std::memcpy(current.name, ScoreFile::kDefaultName, 9);
        print_replay_row(ascii, y, save_slot_choice + 1, current, "All");
        if (timer >= 10) draw_name_input(ascii, 112.0f, 240.0f);
    } else {
        return;
    }
    ascii.style.color = 0xffffffffu;
    ascii.style.shadow = false;
}

void TitleScene::print_replay_list_row(AsciiManager& ascii, float y, i32 abs) {
    const ReplayListEntry& e = replay_entries[abs];
    const ReplaySummary& r = e.summary;
    const ClockFields t = clock_fields(r.timestamp);
    const char* shot = table_name(kReplayShotNames, r.shot + r.shot_aux);
    const char* season = table_name(kReplaySeasonNames, r.season);
    const Vec3 pos{32.0f, y, 0.0f};
    if (page_cursor.index == 0) {
        if (r.spell_practice) {
            ascii.printf(pos, "No.%.2d %s %.2d/%.2d/%.2d %.2d:%.2d %s %s SpellPr %3d %2.1f%%", abs + 1, r.name,
                         t.year % 100, t.month, t.day, t.hour, t.minute, shot, season, r.spell_id + 1,
                         double(r.slow_rate));
            return;
        }
        ascii.printf(pos, "No.%.2d %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %s %2.1f%%", abs + 1, r.name, t.year % 100,
                     t.month, t.day, t.hour, t.minute, shot, season, table_name(kReplayDifficultyNames, r.difficulty),
                     table_name(kReplayStageNames, r.stage), double(r.slow_rate));
        return;
    }
    // User 页（0x493708）：格式里有 5 个 %s 而原版只压了 4 个字符串，第 5 个 %s 吃到 (double)慢放率的低
    // 32 位、%2.1f 吃到其高 32 位与栈上保存的 EDI。float 扩成 double 后低位多为 0 → MSVC 打 "(null)"；
    // 高位配上用户态指针构成的 double 远小于 0.05 → "0.0"。低位非 0 时原版读野指针，这里同样按 "(null)" 处理
    ascii.printf(pos, "%s  %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %s %s %2.1f%%", e.ud_name, r.name, t.year % 100,
                 t.month, t.day, t.hour, t.minute, shot, table_name(kReplayDifficultyNames, r.difficulty),
                 table_name(kReplayStageNames, r.stage), season, "(null)", 0.0);
}

void TitleScene::draw_replay_list(AsciiManager& ascii) {
    // 0x451d50：sub 2 当前页 25 槽；sub 4 选中行 10 帧滑到 y = 80，之后列出 7 个关卡起点分数
    if (sub == 2) {
        ascii.style.shadow = true;
        const i32 base = page_cursor.index * 0x19;
        float y = 80.0f;
        for (i32 abs = base; abs < base + 0x19; ++abs) {
            ascii.style.color = cursor.index == abs % 0x19 ? kRowHighlight : kRowNormal;
            if (!replay_slot_live[abs]) {
                if (page_cursor.index == 0) {
                    ascii.printf(Vec3{32.0f, y, 0.0f},
                                 "No.%.2d -------- --/--/-- --:-- ------- ------ ------- --- ---%%", abs + 1);
                } else {
                    ascii.printf(Vec3{32.0f, y, 0.0f},
                                 "User  -------- --/--/-- --:-- ------- ------ ------- --- ---%%", abs + 1);
                }
            } else {
                print_replay_list_row(ascii, y, abs);
            }
            y += 15.0f;
        }
        ascii.style.color = 0xffffffffu;
        ascii.style.shadow = false;
        return;
    }
    if (sub != 4 || replay_choice < 0 || replay_choice >= kReplayListSlots) return;
    float y = 80.0f;
    if (timer < 10) y = ((10.0f - float(timer)) * float((replay_choice % 0x19) * 15)) / 10.0f + 80.0f;
    ascii.style.shadow = true;
    print_replay_list_row(ascii, y, replay_choice);
    if (timer >= 10) {
        static constexpr const char* kStageLong[9] = {"test   ", "Stage 1", "Stage 2", "Stage 3", "Stage 4",
                                                      "Stage 5", "Stage 6", "Extra  ", "Clear  "};
        const ReplayListEntry& e = replay_entries[replay_choice];
        float row = 128.0f;
        for (i32 st = 1; st < 8; ++st) {
            ascii.style.color = cursor.index == st - 1 ? kRowHighlight : kRowNormal;
            if ((replay_stage_mask[replay_choice] & (1 << (st - 1))) == 0) {
                ascii.printf(Vec3{220.0f, row, 0.0f}, "%s  ---------", kStageLong[st]);
            } else {
                ascii.printf(Vec3{220.0f, row, 0.0f}, "%s  %.8d%d", kStageLong[st], e.stage_score[st],
                             e.stage_continues[st]);
            }
            row += 18.0f;
        }
    }
    ascii.style.color = 0xffffffffu;
    ascii.style.shadow = false;
}

void TitleScene::draw_spell_hud(AsciiManager& ascii) {
    // 0x456d50：sub∈(0,3]（画面 0x13 时 sub==3 跳过）在 5 个槽上打 SCORE / GAME MODE
    if (screen != kSpellList && screen != kSpellShot && screen != kSpellStart) return;
    if (sub < 1) return;
    if (sub == 3 && screen == kSpellShot) return;
    if (scores == nullptr) return;

    const i32 shot = spell_shot_cursor.index;
    float y = 191.0f;
    ascii.style.font = AsciiManager::Font::Medium;
    ascii.style.shadow = true;
    ascii.style.group = AsciiManager::Group::Ui;
    for (int i = 0; i < 5; ++i) {
        const i32 id = spell_slot_ids[i];
        // 0x456dd5：cmp id, -1; jl 跳过。-2 是空槽；-1 是未解锁，edi = id*0x9c 落在符卡数组前一条（默认全 0），仍画空 SCORE 行。
        if (id < -1) { y += 44.0f; continue; }
        const bool selected = (screen == kSpellShot && i == cursor.index);
        SpellRecord shot_prefix{}, all_prefix{};
        const SpellRecord* rec = nullptr;
        const SpellRecord* all = nullptr;
        if (id == -1) {
            if (shot >= 0 && shot < ScoreFile::kAllShotsBlock) {
                const auto* base = reinterpret_cast<const u8*>(&scores->blocks[shot]);
                std::memcpy(&shot_prefix, base + offsetof(ScoreBlock, spells) - sizeof(SpellRecord), sizeof(shot_prefix));
                rec = &shot_prefix;
            }
            const auto* all_base = reinterpret_cast<const u8*>(&scores->blocks[ScoreFile::kAllShotsBlock]);
            std::memcpy(&all_prefix, all_base + offsetof(ScoreBlock, spells) - sizeof(SpellRecord), sizeof(all_prefix));
            all = &all_prefix;
        } else {
            rec = scores->spell_record(shot, id);
            all = id < ScoreFile::kSpellRecords ? &scores->blocks[ScoreFile::kAllShotsBlock].spells[id] : nullptr;
        }
        const u32 practice_caps = rec != nullptr ? rec->captures[1] : 0;
        const u32 attempts0 = all != nullptr ? all->attempts[0] : 0;
        const u32 attempts1 = all != nullptr ? all->attempts[1] : 0;
        u32 color = selected ? 0xffb0b0b0u : 0xff404040u;
        if (practice_caps != 0) color = selected ? 0xff90d0ffu : 0xff60a0c0u;
        ascii.style.color = color;
        if (attempts0 == 0 && attempts1 == 0) {
            ascii.printf(Vec3{330.0f, y, 0.0f}, "SCORE        00  ----/----");
        } else {
            const u32 high = rec != nullptr ? rec->high_score : 0;
            const u32 caps = rec != nullptr ? rec->captures[1] : 0;
            const u32 atts = rec != nullptr ? rec->attempts[1] : 0;
            ascii.printf(Vec3{330.0f, y, 0.0f}, "SCORE %8d0  %4u/%4u", int(high), caps, atts);
            // GAME MODE 行（难度 < 5）：正常模式取得/挑战
            if (SpellPracticeTable::spell_difficulty(id) < 5) {
                u32 game_color = 0xff404040u;
                if (rec != nullptr && rec->captures[0] != 0) game_color = 0xff206060u;
                ascii.style.color = game_color;
                ascii.printf(Vec3{330.0f, y + 10.0f, 0.0f}, "GAME MODE        %4u/%4u",
                             rec != nullptr ? rec->captures[0] : 0,
                             rec != nullptr ? rec->attempts[0] : 0);
            }
        }
        y += 44.0f;
    }
    ascii.style.color = 0xffffffffu;
    ascii.style.font = AsciiManager::Font::Default;
    ascii.style.shadow = false;
}

// 主菜单子项：光标前的项（3+i / 13+i）→ 0x1e，光标后 → 0x1f；无通关数据时项 1（4 / 0xe）→ 0x1d
void TitleScene::refresh_main_items(AnmManager& anm) {
    i32 i = 0;
    for (; i < cursor.index; ++i) {
        item_interrupt_now(anm, i + 3, 0x1e);
        item_interrupt_now(anm, i + 0xd, 0x1e);
    }
    if (i + 1 < 10) {
        for (i += 0xe; i < 0x17; ++i) {
            item_interrupt_now(anm, i - 10, 0x1f);
            item_interrupt_now(anm, i, 0x1f);
        }
    }
    if (!extra_unlocked()) {
        item_interrupt(anm, 4, 0x1d);
        item_interrupt(anm, 0xe, 0x1d);
    }
}

// 0x44b5f0
void TitleScene::update_main_menu(AnmManager& anm, const InputState& input) {
    GameGlobals& g = *globals;
    switch (sub) {
    case 0: {
        cursor.count = 10;
        cursor.wrap = true;
        if (!extra_unlocked()) cursor.disable(1);
        const i32 mode = g.mode();
        if (mode == 2) { cursor.set(3); g.set_mode(0); }
        else if (mode != 0) { cursor.set(2); g.set_mode(0); }
        set_sub(1);
        if (first_entry) {
            // 首次进标题（+0x5ce8 bit1）：装饰从头播开场，Timer 从 0 数到 0x78 才出 logo
            spawn_into(anm, 0x61, *title_file, 0x61);
            spawn_into(anm, kDecor65, *title_file, kDecor65);
            first_entry = false;
        } else {
            if (anm.find(vm[0x61]) == nullptr) {
                spawn_into(anm, 0x61, *title_file, 0x61);
                anm.interrupt_now(vm[0x61], 2);
            }
            if (anm.find(vm[kDecor65]) == nullptr) {
                spawn_into(anm, kDecor65, *title_file, kDecor65);
                anm.interrupt_now(vm[kDecor65], 2);
            }
            if (prev_screen != kOption) anm.interrupt_now(vm[0x61], 2);
            timer = 0x78;  // 0x406490(&Timer, 0x78)
        }
        [[fallthrough]];
    }
    case 1:
        if (timer == 0x78) {
            spawn_into(anm, kLogo, *title_file, 0);
            if (anm.find(vm[kTitleV]) == nullptr) {
                vm[kTitleV] = 0;
                spawn_into(anm, kTitleV, *title_v_file, 0);
            }
            refresh_main_items(anm);
        }
        if (timer > 0x82) {
            sub = 2;
            timer = 0;
            anm.interrupt_now(vm[kLogo], 3);
            anm.interrupt(vm[kLogo], to_short(cursor.index + 0x11));
            refresh_main_items(anm);
        }
        break;
    case 2: {
        prev_cursor = cursor.index;
        const u32 edge = input.pressed | input.repeat;
        if (edge & InputState::kUp) cursor.move(-1);
        if (edge & InputState::kDown) cursor.move(1);
        if (prev_cursor != cursor.index) {
            sfx(10);
            anm.interrupt_now(vm[kLogo], 3);
            anm.interrupt(vm[kLogo], to_short(cursor.index + 7));
            refresh_main_items(anm);
        }
        bool confirm = (input.pressed & InputState::kConfirmMask) != 0;
        if (input.pressed & InputState::kCancelMask) {
            if (cursor.index == 9) {
                confirm = true;  // switchD_0044c258_caseD_9
            } else {
                sfx(9);
                cursor.set(9);
                anm.interrupt_now(vm[kLogo], 3);
                anm.interrupt(vm[kLogo], to_short(cursor.index + 7));
                refresh_main_items(anm);
            }
        }
        if (confirm) {
            anm.interrupt(vm[kLogo], 6);
            switch (cursor.index) {
            case 7:
                sfx(7);
                break;
            case 9:
                sfx(9);
                break;
            default:
                sfx(7);
                anm.interrupt(vm[kDecor65], 1);
                vm[kDecor65] = 0;
                anm.interrupt(vm[kTitleV], 1);
                break;
            }
            set_sub(4);
        }
        break;
    }
    case 4:
        if (timer > 0x13) {
            switch (cursor.index) {
            case 0:
                g.set_mode(0);
                anm.interrupt_now(vm[0x61], 3);
                set_screen(kDifficulty);
                cursor.push();
                cursor.set(g.last_difficulty);
                g.difficulty = g.last_difficulty;
                break;
            case 1:
                g.set_mode(0);
                anm.interrupt_now(vm[0x61], 3);
                set_screen(kDifficulty);
                cursor.push();
                g.difficulty = kExtraDifficulty;
                cursor.set(0);
                break;
            case 2:
                g.set_mode(1);
                anm.interrupt_now(vm[0x61], 3);
                cursor.push();
                cursor.set(g.last_difficulty);
                g.difficulty = g.last_difficulty;
                set_screen(kDifficulty);
                break;
            case 3:
                g.set_mode(2);
                anm.interrupt_now(vm[0x61], 3);
                set_screen(kSpellPractice);
                cursor.push();
                spell_shot_cursor.wrap = true;
                spell_shot_cursor.count = 4;
                spell_shot_cursor.set(g.last_shot);
                cursor.set(0);
                break;
            case 4: anm.interrupt_now(vm[0x61], 3); set_screen(kReplay); cursor.push(); break;
            case 5: anm.interrupt_now(vm[0x61], 3); set_screen(kScore); cursor.push(); break;
            case 6: anm.interrupt_now(vm[0x61], 3); set_screen(kMusicRoom); cursor.push(); break;
            case 7: set_screen(kOption); cursor.push(); break;
            case 8: anm.interrupt_now(vm[0x61], 3); set_screen(kManual); cursor.push(); break;
            case 9: set_screen(kFinished); break;
            default: break;
            }
            events.push_back({"screen", screen});
        }
        break;
    default:
        break;
    }
}

// 0x44fe20：难度选择（面板脚本 0x7e 普通 / 0x7f Extra；项脚本 0x74+d；通关标记 0x9e+d / 0xa2）
void TitleScene::update_difficulty(AnmManager& anm, const InputState& input) {
    GameGlobals& g = *globals;
    const bool extra = g.difficulty > 3;
    const i32 panel = (extra ? 1 : 0) + 0x7e;
    switch (sub) {
    case 0: {
        if (vm[kLeafEmitter] == 0 && ascii_file) spawn_into(anm, kLeafEmitter, *ascii_file, kLeafEmitterScript);
        cursor.wrap = false;
        cursor.count = extra ? 1 : 4;
        anm.interrupt(vm[panel], 1);
        vm[panel] = 0;
        spawn_into(anm, panel, *title_file, panel);
        anm.interrupt_now(vm[panel], 3);
        anm.interrupt(vm[panel], to_short(cursor.index + 0xd));
        if (g.title_origin == 4) {
            // 练习返回：按上次难度自动确认，同帧进机体选择
            cursor.set(g.difficulty);
            anm.interrupt_now(vm[panel], 3);
            anm.interrupt_now(vm[panel], to_short(cursor.index + 7));
            anm.interrupt_now(vm[panel], 6);
            anm.interrupt(child_id(anm, panel, cursor.index + 0x74), 2);
            enter_shot_select();
            return;
        }
        spawn_into(anm, kDifficultyBack, *title_file, kDifficultyBack);
        set_sub(1);
        // 通关章：0x44a8e0(d) == 0（并非四机体都未续关通关）→ 隐藏子 0x9e+d；Extra 面板看 d = 4 → 0xa2
        if (!extra) {
            for (i32 d = 0; d < 4; ++d)
                if (!difficulty_cleared_by_all(d)) hide_medal(anm, child_id(anm, panel, 0x9e + d));
        } else if (!difficulty_cleared_by_all(4)) {
            hide_medal(anm, child_id(anm, panel, 0xa2));
        }
        [[fallthrough]];
    }
    case 1:
        if (timer > 6) set_sub(2);
        break;
    case 2: {
        if (!extra) {
            prev_cursor = cursor.index;
            const u32 edge = input.pressed | input.repeat;
            if (edge & (InputState::kUp | InputState::kLeft)) cursor.move(-1);
            if (edge & (InputState::kDown | InputState::kRight)) cursor.move(1);
            if (prev_cursor != cursor.index) {
                sfx(10);
                anm.interrupt_now(vm[panel], 3);
                anm.interrupt(vm[panel], to_short(cursor.index + 7));
            }
        }
        if (input.pressed & InputState::kCancelMask) {
            set_sub(4);
            sfx(9);
            dismiss(anm, panel);
            return;
        }
        if (input.pressed & InputState::kConfirmMask) {
            anm.interrupt(vm[panel], 6);
            anm.interrupt(child_id(anm, panel, extra ? 0x78 : cursor.index + 0x74), 2);
            set_sub(3);
            sfx(7);
        }
        break;
    }
    case 3:
        if (timer > 0xd) {
            dismiss(anm, kDifficultyBack);
            enter_shot_select();
        }
        break;
    case 4:
        if (timer > 5) {
            dismiss(anm, kDifficultyBack);
            if ((g.flags & GameGlobals::kModeMask) == 0) {
                if (!extra) {
                    set_screen(kMainMenu);
                    g.difficulty = cursor.index;
                    g.last_difficulty = cursor.index;
                    cursor.pop();
                    events.push_back({"screen", screen});
                    return;
                }
            } else {
                g.last_difficulty = cursor.index;
            }
            g.difficulty = g.last_difficulty;
            set_screen(kMainMenu);
            dismiss(anm, kLeafEmitter);
            set_screen(kMainMenu);
            cursor.pop();
            events.push_back({"screen", screen});
        }
        break;
    default:
        break;
    }
}

void TitleScene::enter_shot_select() {
    // 0x44fe20 LAB_004501a0
    GameGlobals& g = *globals;
    set_screen(kShot);
    if (g.difficulty < 4) {
        g.last_difficulty = cursor.index;
        g.difficulty = cursor.index;
    }
    cursor.push();
    cursor.wrap = true;
    cursor.count = 4;
    cursor.set(g.last_shot);
    g.shot = g.last_shot;
    events.push_back({"screen", screen});
}

// 0x4502c0：机体选择（面板 0x96 普通 / 0x98 Extra；项 0x80+s / 0x88+s；通关标记 0x9a..0x9d；装饰 0x58/0x59）
void TitleScene::update_shot(AnmManager& anm, const InputState& input) {
    GameGlobals& g = *globals;
    const bool extra = g.difficulty == kExtraDifficulty;
    const i32 panel = (extra ? 2 : 0) + 0x96;
    switch (sub) {
    case 0: {
        cursor.count = 4;
        if (extra) {
            // 0x44a800：光标所在机体未解锁 Extra 时移到首个已解锁机体，再禁用其余未解锁项
            if (!shot_extra_unlocked(cursor.index)) {
                for (i32 s = 0; s < 4; ++s) {
                    if (shot_extra_unlocked(s)) { cursor.set(s); break; }
                }
            }
            for (i32 s = 0; s < 4; ++s)
                if (!shot_extra_unlocked(s)) cursor.disable(s);
        }
        if (anm.find(vm[kShotBack]) == nullptr) spawn_into(anm, kShotBack, *title_file, kShotBack);
        if (anm.find(vm[panel]) == nullptr) {
            anm.interrupt(vm[panel], 1);
            vm[panel] = 0;
            spawn_into(anm, panel, *title_file, panel);
        }
        anm.interrupt_now(vm[panel], 3);
        anm.interrupt(vm[panel], to_short(cursor.index + 7));
        set_sub(1);
        // 通关章：机体 k 在当前难度无未续关通关（+0x5184 + k × 0x5318 + d × 4 == 0）→ 隐藏 0x9a+k
        for (i32 k = 0; k < 4; ++k)
            if (scores == nullptr || scores->blocks[k].no_continue_clears[g.difficulty] == 0)
                hide_medal(anm, child_id(anm, panel, 0x9a + k));
        if (g.title_origin == 4) {
            cursor.set(g.shot);
            anm.interrupt_now(vm[panel], to_short(cursor.index + 7));
            anm.interrupt(child_id(anm, panel, cursor.index + 0x80), 6);
            anm.interrupt(child_id(anm, panel, cursor.index + 0x88), 6);
            anm.interrupt(child_id(anm, panel, 0x58), 6);
            anm.interrupt(child_id(anm, panel, 0x59), 6);
            enter_season_select(anm, panel);
            return;
        }
        [[fallthrough]];
    }
    case 1:
        if (timer > 6) set_sub(2);
        break;
    case 2: {
        prev_cursor = cursor.index;
        const u32 edge = input.pressed | input.repeat;
        if (edge & InputState::kLeft) {
            dismiss(anm, kShotPortrait);
            sfx(10);
            anm.interrupt_now(vm[panel], to_short(cursor.index + 0x19));
            cursor.move(-1);
            anm.interrupt(vm[panel], to_short(cursor.index + 0xd));
        }
        if (edge & InputState::kRight) {
            dismiss(anm, kShotPortrait);
            sfx(10);
            anm.interrupt_now(vm[panel], to_short(cursor.index + 0x13));
            cursor.move(1);
            anm.interrupt(vm[panel], to_short(cursor.index + 7));
        }
        if (input.pressed & InputState::kCancelMask) {
            set_sub(4);
            sfx(9);
            return;
        }
        if (input.pressed & InputState::kConfirmMask) {
            anm.interrupt(child_id(anm, panel, cursor.index + 0x80), 6);
            anm.interrupt(child_id(anm, panel, cursor.index + 0x88), 6);
            anm.interrupt(child_id(anm, panel, 0x58), 6);
            anm.interrupt(child_id(anm, panel, 0x59), 6);
            sfx(7);
            set_sub(3);
        }
        break;
    }
    case 3:
        if (timer < 0xe) return;
        enter_season_select(anm, panel);
        break;
    case 4:
        if (timer > 5) {
            dismiss(anm, panel);
            dismiss(anm, kShotBack);
            set_screen(kDifficulty);
            g.shot = cursor.index;
            cursor.pop();
            g.last_shot = g.shot;
            events.push_back({"screen", screen});
        }
        break;
    default:
        break;
    }
}

void TitleScene::enter_season_select(AnmManager& anm, i32 panel) {
    // 0x4502c0 LAB_004509e1
    GameGlobals& g = *globals;
    set_screen(kSeason);
    g.shot = cursor.index;
    g.last_shot = g.shot;
    cursor.push();
    cursor.wrap = true;
    cursor.count = 5;
    cursor.set(g.shot);
    anm.interrupt(vm[panel], 6);
    dismiss(anm, kShotBack);
    events.push_back({"screen", screen});
}

// 0x450af0：子季节选择（面板 0x97 普通 / 0x99 Extra）
void TitleScene::update_season(AnmManager& anm, const InputState& input) {
    GameGlobals& g = *globals;
    const bool extra = g.difficulty == kExtraDifficulty;
    const i32 panel = (extra ? 2 : 0) + 0x97;
    switch (sub) {
    case 0:
        cursor.count = 4;
        if (extra) { cursor.set(0); cursor.count = 1; }
        anm.interrupt(vm[panel], 1);
        spawn_into(anm, panel, *title_file, panel);
        anm.interrupt_now(vm[panel], 3);
        anm.interrupt_now(vm[panel], to_short(cursor.index + 7));
        anm.interrupt_now(vm[panel], to_short(g.shot + 0x1f));
        set_sub(1);
        spawn_into(anm, kSeasonBack, *title_file, kSeasonBack);
        if (g.title_origin == 4) {
            cursor.set(g.season);
            enter_practice_stage(anm, panel);
            return;
        }
        [[fallthrough]];
    case 1:
        if (timer > 6) set_sub(2);
        break;
    case 2: {
        prev_cursor = cursor.index;
        if (!extra) {
            const u32 edge = input.pressed | input.repeat;
            if (edge & InputState::kLeft) {
                sfx(10);
                anm.interrupt_now(vm[panel], to_short(cursor.index + 0x19));
                cursor.move(-1);
                anm.interrupt(vm[panel], to_short(cursor.index + 0xd));
            }
            if (edge & InputState::kRight) {
                sfx(10);
                anm.interrupt_now(vm[panel], to_short(cursor.index + 0x13));
                cursor.move(1);
                anm.interrupt(vm[panel], to_short(cursor.index + 7));
            }
        }
        if (input.pressed & InputState::kCancelMask) {
            set_sub(4);
            sfx(9);
            return;
        }
        if (input.pressed & InputState::kConfirmMask) {
            sfx(7);
            set_sub(3);
            sfx(0x32);
            if ((g.flags & GameGlobals::kModeMask) == 0) bgm_fade();  // 0x43c470
        }
        break;
    }
    case 3:
        if (timer == 10) {
            if ((g.flags & GameGlobals::kModeMask) != 0) {
                enter_practice_stage(anm, panel);
                return;
            }
            transition_effect();
        }
        if (timer > 0x27) {
            g.season = extra ? 4 : cursor.index;
            cursor.push();
            g.replay_slot = -1;
            set_screen(kFinished);
            if (extra) request_game(7, 1);
            else request_game(1, 0);
        }
        break;
    case 4:
        if (timer > 5) {
            g.season = cursor.index;
            dismiss(anm, panel);
            dismiss(anm, kSeasonBack);
            set_screen(kShot);
            cursor.pop();
            events.push_back({"screen", screen});
        }
        break;
    default:
        break;
    }
}

void TitleScene::enter_practice_stage(AnmManager& anm, i32 panel) {
    // 0x450af0 LAB_00450d8f：练习 / 符卡练习 → 关卡选择
    globals->season = cursor.index;
    cursor.push();
    dismiss(anm, panel);
    dismiss(anm, kSeasonBack);
    set_screen(kStage);
    events.push_back({"screen", screen});
}

// 0x450ef0：练习关卡选择（底 0x71；6 关）
void TitleScene::update_stage(AnmManager& anm, const InputState& input) {
    GameGlobals& g = *globals;
    switch (sub) {
    case 0:
        cursor.count = 6;
        cursor.set(g.last_practice_stage);
        spawn_into(anm, kStageBack, *title_file, kStageBack);
        set_sub(1);
        if (g.title_origin == 4) g.title_origin = 1;
        [[fallthrough]];
    case 1:
        if (timer > 10) set_sub(2);
        break;
    case 2: {
        prev_cursor = cursor.index;
        const u32 edge = input.pressed | input.repeat;
        if (edge & InputState::kUp) cursor.move(-1);
        if (edge & InputState::kDown) cursor.move(1);
        if (prev_cursor != cursor.index) sfx(10);
        if (input.pressed & InputState::kCancelMask) {
            set_sub(4);
            sfx(9);
            g.last_practice_stage = cursor.index;
            return;
        }
        if (input.pressed & InputState::kConfirmMask) {
            // +0x51a5：所选关卡（光标 + 1）在该机体 × 难度下非回放到达过（0x42cb60 +0x519d）
            const i32 shot = g.shot + g.shot_aux;
            const i32 slot = g.difficulty * 8 + cursor.index + 1;
            const bool unlocked = scores != nullptr && shot >= 0 && shot < ScoreFile::kShots && slot >= 0 &&
                                  slot < ScoreFile::kPracticeSlots && scores->blocks[shot].practice[slot].reached != 0;
            if (!unlocked) { sfx(0x10); return; }
            set_sub(3);
            sfx(7);
            sfx(0x32);
            g.last_practice_stage = cursor.index;
            // DAT_004a5bf8：0x402130 快照（DirectInput 分支，DIK 下标）里按住的首个数字键 1..9（DIK 0x02..0x0a）
            // → 练习初始残机 = 其值-1（0x42cb60）；未按为 0 → 9 残机。原版无焦点返回 0 走 VK 下标分支，快照已清零，等价无效
            g.practice_lives = 0;
            if (keyboard_state != nullptr) {
                for (i32 digit = 1; digit <= 9; ++digit) {
                    if (keyboard_state[1 + digit] & 0x80) { g.practice_lives = digit; break; }
                }
            }
            bgm_fade();
        }
        break;
    }
    case 3:
        if (timer == 10) transition_effect();
        if (timer > 0x27) {
            cursor.push();
            set_screen(kFinished);
            g.title_origin = 4;
            g.last_practice_stage = cursor.index;
            request_game(cursor.index + 1, 2);
        }
        break;
    case 4:
        if (timer > 5) {
            dismiss(anm, kStageBack);
            set_screen(kSeason);
            cursor.pop();
            events.push_back({"screen", screen});
        }
        break;
    default:
        break;
    }
}

void TitleScene::panel_item_interrupt_now(AnmManager& anm, i32 script, i32 value) {
    const u32 id = child_id(anm, 1, script);  // Option 面板 +0x2c4 = vm[1]
    AnmVm* item = anm.find(id);
    if (item == nullptr) return;
    item->pending_interrupt = value;
    anm.run(*item);
}

void TitleScene::refresh_option_items(AnmManager& anm) {
    // 0x44c8c0：光标前项脚本 i+0x17 / i+0x1c → 0x1e；光标后项 → 0x1f（选中由面板 interrupt 负责）
    if (vm[1] == 0) return;
    for (i32 i = 0; i < cursor.index; ++i) {
        panel_item_interrupt_now(anm, i + 0x17, 0x1e);
        panel_item_interrupt_now(anm, i + 0x1c, 0x1e);
    }
    for (i32 i = cursor.index + 1; i < 5; ++i) {
        panel_item_interrupt_now(anm, i + 0x17, 0x1f);
        panel_item_interrupt_now(anm, i + 0x1c, 0x1f);
    }
}

void TitleScene::apply_option_volume(AnmManager& anm) {
    // 0x44dc70：DAT_004df49c = BGM 音量 → 0x45ed00(8, "SetVol")；DAT_004df4a0 = SE 音量；随后把两组数字写进面板子脚本 0x21.. 的 ANM
    if (globals != nullptr) {
        if (bgm != nullptr) bgm->volume = globals->bgm_volume;
        if (sound != nullptr) sound->master_volume = globals->se_volume;
    }
    if (bgm != nullptr) bgm->enqueue(8, 0, "SetVol");
    if (globals == nullptr) return;

    // 面板子脚本：BGM 百/十/个位 0x21..0x23（精灵 +0x2a）与 0x25..0x27（精灵 +0x35，高亮色）；
    // SE 对应 0x29..0x2b / 0x2d..0x2f。再按位数藏/显前导零（flags_lo bit1）。
    auto paint = [&](i32 script, i32 sprite) {
        AnmVm* digit = anm.find(child_id(anm, kOptionPanel, script));
        if (digit == nullptr) return;
        if (AnmFile* file = anm.file(digit->file_slot)) anm_set_sprite(*file, *digit, sprite);
    };
    auto show = [&](i32 script, bool on) {
        AnmVm* digit = anm.find(child_id(anm, kOptionPanel, script));
        if (digit == nullptr) return;
        if (on) digit->flags_lo |= AnmVm::kFlag2;
        else digit->flags_lo &= ~AnmVm::kFlag2;
    };
    const i32 bgm_vol = globals->bgm_volume;
    const i32 se_vol = globals->se_volume;
    for (i32 base : {0x21, 0x25}) {
        const i32 sprite = base == 0x21 ? 0x2a : 0x35;
        paint(base + 0, bgm_vol / 100 + sprite);
        paint(base + 1, (bgm_vol / 10) % 10 + sprite);
        paint(base + 2, bgm_vol % 10 + sprite);
        paint(base + 8, se_vol / 100 + sprite);
        paint(base + 9, (se_vol / 10) % 10 + sprite);
        paint(base + 10, se_vol % 10 + sprite);
    }
    for (i32 base : {0x21, 0x25}) {
        show(base + 0, bgm_vol >= 100);
        show(base + 1, bgm_vol >= 10);
        show(base + 8, se_vol >= 100);
        show(base + 9, se_vol >= 10);
    }
}

// 0x44c570：Option（BGM / SE / KeyConfig / Default / Quit）
void TitleScene::update_option(AnmManager& anm, const InputState& input) {
    GameGlobals& g = *globals;
    switch (sub) {
    case 0:
        cursor.count = 5;
        cursor.wrap = true;
        cursor.set(0);
        if (title_file != nullptr) spawn_into(anm, 1, *title_file, 1);  // +0x2c4 面板脚本 1
        apply_option_volume(anm);
        refresh_option_items(anm);
        set_sub(1);
        events.push_back({"option", 0});
        [[fallthrough]];
    case 1:
        if (timer > 6) {
            set_sub(2);
            if (vm[1]) {
                anm.interrupt_now(vm[1], 3);
                anm.interrupt(vm[1], cursor.index + 0x11);
            }
            refresh_option_items(anm);
        }
        break;
    case 2: {
        const i32 prev = cursor.index;
        const u32 edge = input.pressed | input.repeat;
        if (edge & InputState::kUp) cursor.move(-1);
        if (edge & InputState::kDown) cursor.move(1);
        if (cursor.index != prev) {
            sfx(10);
            if (vm[1]) {
                anm.interrupt_now(vm[1], 3);
                anm.interrupt(vm[1], to_short(cursor.index + 7));
            }
            refresh_option_items(anm);
        }
        // 取消：非 Quit 项时先跳到 Quit（4），再取消才退出——与原版 0x102 分支一致
        if (input.pressed & InputState::kCancelMask) {
            if (cursor.index != 4) {
                sfx(9);
                cursor.set(4);
                if (vm[1]) {
                    anm.interrupt_now(vm[1], 3);
                    anm.interrupt(vm[1], to_short(cursor.index + 7));
                }
                refresh_option_items(anm);
                break;
            }
            if (vm[1]) anm.interrupt(vm[1], 6);
            sfx(9);
            set_sub(4);
            break;
        }
        // 左右调音量（项 0 BGM / 1 SE），步进 5，夹到 0..100（0x4186f0）
        if (edge & InputState::kLeft) {
            if (cursor.index == 0) {
                g.bgm_volume = u8(g.bgm_volume < 5 ? 0 : g.bgm_volume - 5);
                apply_option_volume(anm);
            } else if (cursor.index == 1) {
                g.se_volume = u8(g.se_volume < 5 ? 0 : g.se_volume - 5);
                apply_option_volume(anm);
            }
        }
        if (edge & InputState::kRight) {
            if (cursor.index == 0) {
                g.bgm_volume = u8(g.bgm_volume + 5 > 100 ? 100 : g.bgm_volume + 5);
                apply_option_volume(anm);
            } else if (cursor.index == 1) {
                g.se_volume = u8(g.se_volume + 5 > 100 ? 100 : g.se_volume + 5);
                apply_option_volume(anm);
            }
        }
        if (input.pressed & InputState::kConfirmMask) {
            if (cursor.index == 2) {
                if (vm[1]) anm.interrupt(vm[1], 6);
                sfx(7);
                set_sub(4);
            } else if (cursor.index == 3) {
                g.bgm_volume = 100;
                g.se_volume = 80;  // 0x5064
                g.option_flags = 0;
                apply_option_volume(anm);
                sfx(7);
            } else if (cursor.index == 4) {
                if (vm[1]) anm.interrupt(vm[1], 6);
                sfx(9);
                set_sub(4);
            }
        }
        break;
    }
    case 4:
        if (timer > 9) {
            if (cursor.index == 2) {
                dismiss(anm, 1);
                set_screen(kKeyConfig);
                cursor.push();
                events.push_back({"screen", screen});
            } else if (cursor.index == 4) {
                dismiss(anm, 1);
                set_screen(kMainMenu);
                cursor.pop();
                events.push_back({"screen", screen});
            }
        }
        break;
    default:
        break;
    }
}

namespace {
i32 load_i32_at(const std::vector<u8>& bytes, size_t off) {
    i32 v = 0;
    if (bytes.size() >= off + 4) std::memcpy(&v, bytes.data() + off, 4);
    return v;
}

// th16_ud????.rpy：FindFirstFileA 的 '?' 在扩展名前可匹配 0..1 个字符
bool is_user_replay_name(const char* n) {
    const size_t len = std::strlen(n);
    if (len < 11 || len > 15) return false;
    if (std::strncmp(n, "th16_ud", 7) != 0) return false;
    return std::strcmp(n + len - 4, ".rpy") == 0;
}
}  // namespace

void TitleScene::scan_replay_slots() {
    // 同步完成原版 0x402fb0 工作线程 0x451560
    auto fill = [this](i32 i, const char* path) {
        ReplayFile file;
        if (!file.open_file(path) || file.user.size() < ReplayFile::kUserSize) return;
        ReplayListEntry& e = replay_entries[i];
        e.path = path;
        std::memcpy(e.summary.name, file.user.data(), 8);
        e.summary.name[8] = '\0';
        e.summary.timestamp = load_i32_at(file.user, 0xc);
        e.summary.spell_practice = (file.user[0xa] & 2) != 0;
        e.summary.shot = load_i32_at(file.user, 0x84);
        e.summary.shot_aux = load_i32_at(file.user, 0x88);
        e.summary.difficulty = load_i32_at(file.user, 0x8c);
        e.summary.stage = load_i32_at(file.user, 0x90);
        e.summary.spell_id = load_i32_at(file.user, 0x98);
        e.summary.season = load_i32_at(file.user, 0x9c);
        std::memcpy(&e.summary.slow_rate, file.user.data() + 0x7c, sizeof(float));
        const ReplayStageSegment* by_stage[8] = {};
        for (const ReplayStageSegment& seg : file.stages) {
            if (seg.stage >= 1 && seg.stage <= 7) {
                replay_stage_mask[i] |= 1 << (seg.stage - 1);
                by_stage[seg.stage] = &seg;
            }
        }
        for (i32 st = 1; st < 8; ++st) {
            const ReplayStageSegment* next = st < 6 ? by_stage[st + 1] : nullptr;
            if (next != nullptr) {
                e.stage_score[st] = load_i32_at(next->header, 0x34);
                e.stage_continues[st] = load_i32_at(next->header, 0x3c);
            } else {
                e.stage_score[st] = load_i32_at(file.user, 0x14);
                e.stage_continues[st] = load_i32_at(file.user, 0x94);
            }
        }
        replay_slot_live[i] = true;
        replay_spell_practice[i] = e.summary.spell_practice;
    };
    for (i32 i = 0; i < 100; ++i) {
        replay_slot_live[i] = false;
        replay_stage_mask[i] = 0;
        replay_spell_practice[i] = false;
    }
    for (ReplayListEntry& e : replay_entries) e = ReplayListEntry{};
    if (!replay_directory.empty()) {
        for (i32 i = 0; i < 0x19; ++i) {
            char path[512];
            if (replay_slot_path(replay_directory.c_str(), i + 1, path, sizeof(path))) fill(i, path);
        }
        std::vector<std::string> names;
        const std::string dir = replay_directory + "/replay";
        if (DIR* d = opendir(dir.c_str())) {
            while (dirent* ent = readdir(d)) {
                if (is_user_replay_name(ent->d_name)) names.emplace_back(ent->d_name);
            }
            closedir(d);
        }
        // NTFS 目录枚举按大小写不敏感的名字序
        std::sort(names.begin(), names.end(), [](const std::string& x, const std::string& y) {
            return strcasecmp(x.c_str(), y.c_str()) < 0;
        });
        i32 slot = 0x19;
        for (const std::string& n : names) {
            if (slot >= kReplayListSlots) break;
            const std::string path = dir + "/" + n;
            fill(slot, path.c_str());
            if (replay_slot_live[slot]) std::memcpy(replay_entries[slot].ud_name, n.c_str() + 7, 4);
            ++slot;
        }
    }
    replay_ui_flags = (replay_ui_flags & 0xfffffff3u) | 8u;  // 清 bit2、置 bit3 扫描完成
}

// 0x451750：标题 Replay（25 槽 × 3 页；确认 → 关卡选择 → 场景 0xd）
void TitleScene::update_replay(AnmManager& anm, const InputState& input) {
    switch (sub) {
    case 0: {
        cursor.count = 0x19;
        cursor.wrap = true;
        cursor.set(last_replay_slot % 0x19);
        page_cursor.count = 3;
        page_cursor.wrap = true;
        page_cursor.set(last_replay_slot / 0x19);
        // *(+0x2a4)=1：原版列表滚动相关；端口不需要独立字段
        last_replay_slot = 0;
        if (vm[kLeafEmitter] == 0 && ascii_file) spawn_into(anm, kLeafEmitter, *ascii_file, kLeafEmitterScript);
        if (title_file) spawn_into(anm, 0x6c, *title_file, 0x6c);
        set_sub(1);
        for (bool& live : replay_slot_live) live = false;
        replay_ui_flags &= 0xfffffff3u;
        replay_choice = -1;
        scan_replay_slots();
        if (vm[0x61] == 0 && title_file) {
            spawn_into(anm, 0x61, *title_file, 0x61);
            anm.interrupt_now(vm[0x61], 3);
        }
        events.push_back({"replay", 0});
        [[fallthrough]];
    }
    case 1:
        if (timer > 6) set_sub(2);
        break;
    case 2: {
        const i32 prev_slot = cursor.index;
        const i32 prev_page = page_cursor.index;
        const u32 edge = input.pressed | input.repeat;
        if (edge & InputState::kUp) cursor.move(-1);
        if (edge & InputState::kDown) cursor.move(1);
        if (edge & InputState::kLeft) page_cursor.move(-1);
        if (edge & InputState::kRight) page_cursor.move(1);
        if (page_cursor.index != prev_page) sfx(10);
        if (cursor.index != prev_slot) sfx(10);
        if (input.pressed & InputState::kCancelMask) {
            set_sub(5);
            sfx(9);
            replay_ui_flags |= 4u;
            break;
        }
        if (input.pressed & InputState::kConfirmMask) {
            const i32 abs = replay_abs_slot();
            if (abs < 0 || abs >= 100 || !replay_slot_live[abs]) break;
            set_sub(4);
            replay_choice = abs;
            cursor.push();
            sfx(7);
            cursor.count = 7;
            cursor.set(0);
            for (i32 i = 0; i < 7; ++i) {
                if ((replay_stage_mask[abs] & (1 << i)) == 0) cursor.disable(i);
            }
            // 原版：落在首个可用项上（move -1/+1 跳过禁用）
            cursor.move(-1);
            cursor.move(1);
            if (replay_spell_practice[abs]) {
                replay_stage_choice = cursor.index;
                set_sub(3);
                replay_ui_flags |= 4u;
                sfx(0x32);
                bgm_fade();
            }
        }
        break;
    }
    case 3:
        if (timer == 2) transition_effect();
        if (timer > 0x1f && (replay_ui_flags & 8u) != 0) {
            set_screen(kFinished);  // 原版 set_screen(2) 后写 DAT_004c17c4=0xd
            pending_replay_path = (replay_choice >= 0 && replay_choice < kReplayListSlots)
                                      ? replay_entries[replay_choice].path
                                      : std::string{};
            last_replay_slot = replay_choice;
            globals->scene_request = 0xd;
            globals->replay_slot = replay_stage_choice;  // DAT_004a57c8 路径相关；阶段选择另见 pending
            events.push_back({"scene", 0xd});
            events.push_back({"replay-play", replay_choice});
        }
        break;
    case 4:
        if (timer > 0xe) {
            const i32 prev = cursor.index;
            const u32 edge = input.pressed | input.repeat;
            if (edge & InputState::kUp) cursor.move(-1);
            if (edge & InputState::kDown) cursor.move(1);
            if (cursor.index != prev) sfx(10);
            if (input.pressed & InputState::kCancelMask) {
                cursor.pop();
                cursor.count = 0x19;
                set_sub(2);
                sfx(9);
                break;
            }
            if (input.pressed & InputState::kConfirmMask) {
                replay_stage_choice = cursor.index;
                set_sub(3);
                replay_ui_flags |= 4u;
                sfx(0x32);
                bgm_fade();
            }
        }
        break;
    case 5:
        if (timer > 5 && (replay_ui_flags & 8u) != 0) {
            dismiss(anm, 0x6c);
            dismiss(anm, kLeafEmitter);
            set_screen(kMainMenu);
            cursor.pop();
            events.push_back({"screen", screen});
        }
        break;
    default:
        break;
    }
}

void TitleScene::close_help() {
    delete help;
    help = nullptr;
}

// 0x4545a0：Manual → 0x42ea30 HelpScreen；关闭后回主菜单
void TitleScene::update_manual(AnmManager& anm, const InputState& input) {
    switch (sub) {
    case 0: {
        if (vm[kLeafEmitter] == 0 && ascii_file) spawn_into(anm, kLeafEmitter, *ascii_file, kLeafEmitterScript);
        if (title_file) spawn_into(anm, 0x72, *title_file, 0x72);  // +0x488
        close_help();
        help = new HelpScreen();
        // 0x42e760 在装载线程里写 +0x12c。标题路径不跑该线程，help.anm 保持未装，目录项不生成。
        // 局内暂停菜单有自己的装载回调，不走这里。
        help->anm = nullptr;
        help->scroll = 112.0f;  // 0x43000000
        set_sub(1);
        events.push_back({"manual", 0});
        break;
    }
    case 1:
        if (help != nullptr) {
            if (help->anm != nullptr) {
                help->update(anm, input, 1.0f);
                for (i32 id : help->sfx_queue) sfx(id);
                help->sfx_queue.clear();
            } else if (input.pressed & InputState::kCancelMask) {
                // help.anm 未装上时允许取消退出（harness 无 help 资源仍可回归）
                sfx(9);
                help->closed = true;
            }
            if (help->closed) {
                close_help();
                dismiss(anm, kLeafEmitter);
                dismiss(anm, 0x72);
                set_screen(kMainMenu);
                cursor.pop();
                events.push_back({"screen", screen});
            }
        }
        break;
    default:
        break;
    }
}

namespace {

// SJIS（th16.exe .rdata）：DAT_004935e0 未听过曲名；PTR_DAT_004936b8 未听过曲目的 8 行提示
constexpr char kMusicLockedTitle[] = "No.%2d  ""\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48";
constexpr const char* kMusicNotice[8] = {
    // （空行）
    "\x81\x40",
    // ＊＊選択した曲はまだゲーム中で再生されていません＊＊
    "\x81\x40\x81\x40\x81\x96\x81\x96\x91\x49\x91\xf0\x82\xb5\x82\xbd\x8b\xc8\x82\xcd\x82\xdc\x82\xbe"
    "\x83\x51\x81\x5b\x83\x80\x92\x86\x82\xc5\x8d\xc4\x90\xb6\x82\xb3\x82\xea\x82\xc4\x82\xa2\x82\xdc"
    "\x82\xb9\x82\xf1\x81\x96\x81\x96",
    // （空行）
    "\x20",
    // 曲のコメントがネタバレになる恐れがあります。
    "\x81\x40\x81\x40\x81\x40\x81\x40\x8b\xc8\x82\xcc\x83\x52\x83\x81\x83\x93\x83\x67\x82\xaa\x83\x6c"
    "\x83\x5e\x83\x6f\x83\x8c\x82\xc9\x82\xc8\x82\xe9\x8b\xb0\x82\xea\x82\xaa\x82\xa0\x82\xe8\x82\xdc"
    "\x82\xb7\x81\x42",
    // それでも再生しますか？
    "\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40\x82\xbb\x82\xea\x82\xc5"
    "\x82\xe0\x8d\xc4\x90\xb6\x82\xb5\x82\xdc\x82\xb7\x82\xa9\x81\x48",
    // （空行）
    "\x81\x40",
    // 再生したい方は、もう一度決定ボタンを押してください。
    "\x81\x40\x81\x40\x81\x40\x8d\xc4\x90\xb6\x82\xb5\x82\xbd\x82\xa2\x95\xfb\x82\xcd\x81\x41\x82\xe0"
    "\x82\xa4\x88\xea\x93\x78\x8c\x88\x92\xe8\x83\x7b\x83\x5e\x83\x93\x82\xf0\x89\x9f\x82\xb5\x82\xc4"
    "\x82\xad\x82\xbe\x82\xb3\x82\xa2\x81\x42",
    // 再生したくない方は、カーソルを移動してください。
    "\x81\x40\x81\x40\x81\x40\x8d\xc4\x90\xb6\x82\xb5\x82\xbd\x82\xad\x82\xc8\x82\xa2\x95\xfb\x82\xcd"
    "\x81\x41\x83\x4a\x81\x5b\x83\x5c\x83\x8b\x82\xf0\x88\xda\x93\xae\x82\xb5\x82\xc4\x82\xad\x82\xbe"
    "\x82\xb3\x82\xa2\x81\x42",
};
constexpr i32 kMusicTrackScript = 0xbc;   // title.anm：曲名行
constexpr i32 kMusicCommentScript = 0x13; // text.anm：注释行
constexpr i32 kMusicPanel = 0x6e;         // title.anm：+0x478

bool is_newline(char c) { return c == '\n' || c == '\r'; }

// 0x455330：跳过本行及其后连续换行
const char* music_skip_line(const char* p, i32& len) {
    while (!is_newline(*p)) {
        if (len == 0) return p;
        ++p;
        --len;
    }
    if (len != 0) {
        while (is_newline(*p) && len != 0) {
            ++p;
            --len;
        }
    }
    return p;
}

// 0x455370：本行 strcpy 到 dst（行尾写 NUL），再跳过连续换行
char* music_copy_line(char* dst, size_t cap, char* p, i32& len) {
    char* q = p;
    while (!is_newline(*q)) {
        if (len == 0) return q;
        ++q;
        --len;
    }
    if (len != 0) {
        *q = '\0';
        if (dst != nullptr) {
            std::strncpy(dst, p, cap - 1);
            dst[cap - 1] = '\0';
        }
        do {
            --len;
            ++q;
            if (!is_newline(*q)) return q;
        } while (len != 0);
    }
    return q;
}

}  // namespace

void TitleScene::parse_music_tracks() {
    for (MusicTrack& t : music) t = MusicTrack{};
    // '@' 行以 p+1 起拷贝却不减 len，末尾会越过原文若干字节：按 '@' 数补零
    std::vector<char> buf(music_cmt.begin(), music_cmt.end());
    buf.resize(buf.size() + size_t(std::count(music_cmt.begin(), music_cmt.end(), '@')) + 16, '\0');
    i32 len = i32(music_cmt.size());
    char* p = buf.data();
    i32 n = 0;
    while (len > 0) {
        if (*p == '#' || *p != '@') {
            p = const_cast<char*>(music_skip_line(p, len));
            continue;
        }
        MusicTrack* t = n < kMusicTracks ? &music[n] : nullptr;
        p = music_copy_line(t ? t->path : nullptr, sizeof(MusicTrack::path), p + 1, len);
        p = music_copy_line(t ? t->title : nullptr, sizeof(MusicTrack::title), p, len);
        for (int k = 0; k < 8; ++k) p = music_copy_line(t ? t->comment[k] : nullptr, sizeof(MusicTrack::comment[0]), p, len);
        ++n;
    }
    music_count = std::min(n, kMusicTracks);
}

void TitleScene::play_bgm_op(i32 op) {
    // 0x45ed00(&DAT_004d9e10, op, 0, "dummy")：2 播放已载入曲 / 3 停止 / 4（DAT_004c12cc bit4 时先发）
    events.push_back({"bgm-op", op});
    if (bgm != nullptr) bgm->enqueue(op, 0, "dummy");
}

void TitleScene::bgm_default() {
    events.push_back({"bgm-default", 1});   // FUN_0043c370(0, "th16_01")
    if (bgm != nullptr) bgm->register_track(0, "th16_01");
}

void TitleScene::bgm_name_entry() {
    events.push_back({"bgm-name-entry", 0});   // FUN_0043c370(0, "th128_08")
    if (bgm != nullptr) bgm->register_track(0, "th128_08");
}

void TitleScene::bgm_fade() {
    // 0x43c470(0.05)：标题各处进游戏 / 练习前的淡出——0.05 秒 ÷ 游戏速度截断为 0 帧
    events.push_back({"bgm-fade", 0});
    if (bgm != nullptr) bgm->fade_out(0.05f, game_speed != nullptr ? *game_speed : 1.0f);
}

void TitleScene::tick_title_bgm() {
    // 0x44af80 LAB_0044b0c6：进标题后第 10 帧停旧曲 → 载 th16_01 → 播放，并置音乐室 0 号已听过
    if (!title_bgm_pending || ++title_bgm_delay <= 9) return;
    const bool alt = globals != nullptr && (globals->input_config & 0x10u) != 0;
    play_bgm_op(alt ? 4 : 3);
    if (bgm != nullptr) bgm->current.clear();   // DAT_004dc0f0 = 0
    bgm_default();  // FUN_0043c370(0,"th16_01")
    if (alt) play_bgm_op(4);
    play_bgm_op(2);
    if (scores != nullptr) scores->mark_bgm_heard(0);
    title_bgm_pending = false;
    title_bgm_delay = 0;
}

void TitleScene::layout_music_items(AnmManager& anm) {
    // 0x4546f0 case 2 光标移动：10 行窗口内显示，选中行左移 8；与目标相距 ≥80 直接落位，否则 4 帧插值
    float x = 128.0f;
    float y = 96.0f - float(music_scroll) * 20.0f;
    y = y + y;
    for (i32 i = 0; i < music_count && i < kMusicTrackVms; ++i) {
        AnmVm* v = anm.find(music_track_vms[i]);
        if (v == nullptr) {
            music_track_vms[i] = 0;
            y += 40.0f;
            continue;
        }
        if (i < music_scroll || i >= music_scroll + 10) AnmManager::hide_tree(*v);
        else AnmManager::show_tree(*v);
        if (i == cursor.index) x = float(double(x) - 8.0);
        const Vec3 goal{x, y, 0.0f};
        if (std::fabs(v->pos.y - y) >= 80.0f) v->pos = goal;
        else anm_begin_vec3_interp(v->pos_interp, 4, 0, v->pos, goal);
        if (i == cursor.index) x = float(double(x) + 8.0);
        y += 40.0f;
        anm.request_interrupt(*v, i != cursor.index ? 3 : 2);
    }
}

void TitleScene::paint_music_comment(AnmManager& anm) {
    // 0x4546f0 case 1/2：偶数帧刷一行（8 行）；未听过且已提示 → 提示文案（0x8080ff），否则曲目注释
    if ((timer & 1) != 0 || music_comment_line >= 8) return;
    AnmVm* v = anm.find(music_comment_vms[music_comment_line]);
    if (v == nullptr) {
        music_comment_vms[music_comment_line] = 0;
    } else {
        const bool heard = scores != nullptr && music_comment_track >= 0 && music_comment_track < ScoreFile::kBgmFlags &&
                           scores->stats.bgm_heard[music_comment_track] != 0;
        if (!heard && music_notice != 0) {
            text_paint_vm(anm, *v, 0x8080ffu, 0, 0, 0, 0, kMusicNotice[music_comment_line], text_paints);
        } else {
            const char* line = music_comment_track >= 0 && music_comment_track < kMusicTracks
                ? music[music_comment_track].comment[music_comment_line] : "";
            text_paint_vm(anm, *v, 0xffffffu, 0, 0, 0, 0, line, text_paints);
        }
        anm.request_interrupt(*v, 2);
    }
    ++music_comment_line;
}

void TitleScene::leave_music(AnmManager& anm) {
    // LAB_00455228：曲目 / 注释 VM 中断 1 收起，SE 9，→ 子状态 3
    for (i32 i = 0; i < music_count && i < kMusicTrackVms; ++i) anm.interrupt(music_track_vms[i], 1);
    for (u32 id : music_comment_vms) anm.interrupt(id, 1);
    sfx(9);
    set_sub(3);
}

// 0x4546f0：Music Room
void TitleScene::update_music(AnmManager& anm, const InputState& input) {
    switch (sub) {
    case 0: {
        if (timer == 1) {
            cursor.count = 6;
            cursor.set(0);
            if (vm[kLeafEmitter] == 0 && ascii_file) spawn_into(anm, kLeafEmitter, *ascii_file, kLeafEmitterScript);
            spawn_into(anm, kMusicPanel, *title_file, kMusicPanel);
            if (music_cmt.empty()) {
                leave_music(anm);
                return;
            }
            parse_music_tracks();
            cursor.count = music_count;
            cursor.set(0);
            music_scroll = 0;
            AnmFile* text = anm.file(0);
            for (i32 k = 0; k < 8; ++k) music_comment_vms[k] = text ? anm.spawn(*text, kMusicCommentScript + k, -1) : 0;
            music_comment_line = 0;
            music_comment_track = 0;
            music_notice = 0;
            events.push_back({"music", music_count});
        }
        if (timer < 10) {
            // 每帧生成两行曲名（下标 timer×2−2 起），4 帧插值滑到 (128 或 120, 2×(96 − 20×scroll + 20×i))
            float y = (96.0f - float(music_scroll) * 20.0f) + float(timer * 0x28 - 0x28);
            y = y + y;
            float x = 128.0f;
            for (i32 i = timer * 2 - 2; i < timer * 2; ++i) {
                if (i >= music_count || i >= kMusicTrackVms) break;
                AnmVm* v = nullptr;
                music_track_vms[i] = anm.spawn(*title_file, kMusicTrackScript + i, -1, &v);
                if (v == nullptr) {
                    music_track_vms[i] = 0;
                    y += 40.0f;
                    continue;
                }
                const bool heard = scores != nullptr && i < ScoreFile::kBgmFlags && scores->stats.bgm_heard[i] != 0;
                if (heard) {
                    text_paint_vm(anm, *v, 0xffffffu, 0, 0, 0, 0, music[i].title, text_paints);
                } else {
                    char line[0x40];
                    std::snprintf(line, sizeof(line), kMusicLockedTitle, i + 1);
                    text_paint_vm(anm, *v, 0xffffffu, 0, 0, 0, 0, line, text_paints);
                }
                if (i < music_scroll || i >= music_scroll + 10) AnmManager::hide_tree(*v);
                else AnmManager::show_tree(*v);
                if (i == cursor.index) x = float(double(x) - 8.0);
                anm_begin_vec3_interp(v->pos_interp, 4, 0, v->pos, Vec3{x, y, 0.0f});
                if (i == cursor.index) x = float(double(x) + 8.0);
                y += 40.0f;
                anm.request_interrupt(*v, i != cursor.index ? 3 : 2);
            }
            return;
        }
        set_sub(1);
        break;
    }
    case 1:
        paint_music_comment(anm);
        if (timer > 4) set_sub(2);
        break;
    case 2: {
        paint_music_comment(anm);
        const i32 prev = cursor.index;
        const u32 edge = input.pressed | input.repeat;
        if (edge & InputState::kUp) cursor.move(-1);
        if (edge & InputState::kDown) cursor.move(1);
        if (cursor.index != prev) {
            sfx(10);
            if (cursor.index < music_scroll) music_scroll = cursor.index;
            else if (cursor.index >= music_scroll + 10) music_scroll = cursor.index - 9;
            layout_music_items(anm);
            if (music_comment_line > 7) music_notice = 0;
        }
        if (input.pressed & InputState::kConfirmMask) {
            for (u32 id : music_comment_vms) anm.interrupt(id, 3);
            music_comment_track = cursor.index;
            music_comment_line = 0;
            timer = 0;
            const bool alt = globals != nullptr && (globals->input_config & 0x10u) != 0;
            const bool heard = scores != nullptr && cursor.index >= 0 && cursor.index < ScoreFile::kBgmFlags &&
                               scores->stats.bgm_heard[cursor.index] != 0;
            if (!heard && music_notice == 0) {
                play_bgm_op(alt ? 4 : 3);
                music_notice = 1;
                events.push_back({"music-notice", cursor.index});
                return;
            }
            // 0x43c370(0, 路径)：载入所选曲；随后 0x45ed00(2) 播放。原版此处只置 0 号已听过位
            events.push_back({"music-play", cursor.index});
            if (bgm != nullptr) bgm->register_track(0, music[cursor.index].path);
            if (alt) play_bgm_op(4);
            play_bgm_op(2);
            if (scores != nullptr) scores->mark_bgm_heard(0);
            music_notice = 0;
            return;
        }
        if (input.pressed & InputState::kCancelMask) leave_music(anm);
        break;
    }
    case 3:
        if (timer > 9) {
            dismiss(anm, kMusicPanel);
            dismiss(anm, kLeafEmitter);
            set_screen(kMainMenu);
            bgm_default();  // FUN_0043c370(0,"th16_01")
            // 0x43c3f0(0, 0)
            if (globals != nullptr && (globals->input_config & 0x10u) != 0) play_bgm_op(4);
            play_bgm_op(2);
            if (scores != nullptr) scores->mark_bgm_heard(0);
            cursor.pop();
            events.push_back({"screen", screen});
        }
        break;
    default:
        break;
    }
}

// 0x455790：符卡练习 3 页共用的机体副光标（←→）；0x12 / 0x13 页随之重刷名行
void TitleScene::move_spell_shot(AnmManager& anm, const InputState& input) {
    const u32 edge = input.pressed | input.repeat;
    for (const i32 delta : {-1, 1}) {
        if ((edge & (delta < 0 ? InputState::kLeft : InputState::kRight)) == 0) continue;
        sfx(10);
        anm.interrupt_now(vm[0x11c], to_short(spell_shot_cursor.index + (delta < 0 ? 0x19 : 0x13)));
        spell_shot_cursor.move(delta);
        anm.interrupt_now(vm[0x11c], to_short(spell_shot_cursor.index + (delta < 0 ? 0xd : 7)));
        globals->shot = spell_shot_cursor.index;
        if (screen == kSpellList) {
            refresh_spell_slot_names(anm, cursor.index);
        } else if (screen == kSpellShot) {
            refresh_spell_slot_names(anm, spell_list_index);
            highlight_spell_entry(anm, cursor.index);
        }
    }
}

// 0x4569a0：条目页高亮——选中名 VM 中断 2、其余 3；对应难度角标 +0x6f4 同步
void TitleScene::highlight_spell_entry(AnmManager& anm, i32 entry) {
    for (i32 i = 0; i < 5; ++i) {
        const i32 value = i == entry ? 2 : 3;
        if (anm.find(row_text_vms[i]) == nullptr) row_text_vms[i] = 0;
        else anm.interrupt(row_text_vms[i], value);
        anm.interrupt_now(vm[0x10d + SpellPracticeTable::spell_difficulty(spell_slot_ids[i])], value);
    }
}

// 0x456a20 / 0x455d50 case 3：开战参数；DAT_004a2970 / 0049f2dc / 0049f2d8 记下回标题时要还原的分组 / 列表 / 条目
void TitleScene::launch_spell_practice(i32 entry, i32 season) {
    GameGlobals& g = *globals;
    cursor.push();
    set_screen(kFinished);
    const i32 sid = spell_slot_ids[entry];
    g.stage = spell_group + 1;
    g.title_origin = 5;
    g.stage_copy = g.stage;
    g.replay_slot = sid;  // DAT_004a57c8
    g.shot = spell_shot_cursor.index;
    g.shot_aux = 0;
    g.season = season;
    g.scene_request = kSceneGame;
    g.difficulty = SpellPracticeTable::spell_difficulty(sid);  // DAT_00491700
    g.spell_return_group = spell_group;
    g.spell_return_list = spell_list_index;
    g.spell_return_entry = entry;
    events.push_back({"scene", kSceneGame});
}

// 0x4553d0：符卡练习分组选择（7 组；从符卡练习返回时直达原列表）
void TitleScene::update_spell_practice(AnmManager& anm, const InputState& input) {
    GameGlobals& g = *globals;
    switch (sub) {
    case 0:
        if (vm[kLeafEmitter] == 0 && ascii_file) spawn_into(anm, kLeafEmitter, *ascii_file, kLeafEmitterScript);
        cursor.count = 7;
        if (anm.find(vm[0x11c]) == nullptr) spawn_into(anm, 0x11c, *title_file, 0x11c);  // +0x730
        if (anm.find(vm[0xd7]) == nullptr) spawn_into(anm, 0xd7, *title_file, 0xd7);     // +0x61c
        set_sub(1);
        if (g.spell_return_group >= 0) {
            cursor.set(g.spell_return_group);
            g.spell_return_group = -1;
            spell_shot_cursor.wrap = true;
            spell_shot_cursor.count = 4;
            spell_shot_cursor.set(g.shot + g.shot_aux);
            anm.interrupt_now(vm[0xd7], 3);
            anm.interrupt_now(vm[0xd7], to_short(cursor.index + 7));
            anm.interrupt_now(vm[0xd7], 6);
            anm.interrupt_now(vm[0x11c], 3);
            anm.interrupt(vm[0x11c], to_short(spell_shot_cursor.index + 7));
            enter_spell_list(anm);
            return;
        }
        spawn_into(anm, 0x71, *title_file, 0x71);  // +0x484 底图
        events.push_back({"spell-practice", 0});
        [[fallthrough]];
    case 1:
        if (timer > 10) {
            set_sub(2);
            anm.interrupt_now(vm[0xd7], 3);
            anm.interrupt(vm[0xd7], to_short(cursor.index + 7));
            anm.interrupt_now(vm[0x11c], 3);
            anm.interrupt(vm[0x11c], to_short(spell_shot_cursor.index + 7));
        }
        break;
    case 2: {
        const i32 prev = cursor.index;
        const u32 edge = input.pressed | input.repeat;
        if (edge & InputState::kUp) cursor.move(-1);
        if (edge & InputState::kDown) cursor.move(1);
        if (cursor.index != prev) {
            sfx(10);
            anm.interrupt_now(vm[0xd7], 3);
            anm.interrupt(vm[0xd7], to_short(cursor.index + 7));
        }
        move_spell_shot(anm, input);
        if (input.pressed & InputState::kCancelMask) {
            set_sub(4);
            sfx(9);
            g.last_practice_stage = cursor.index;  // DAT_004a2974
            break;
        }
        if (input.pressed & InputState::kConfirmMask) {
            anm.interrupt(vm[0xd7], 6);
            set_sub(3);
            sfx(7);
            g.last_practice_stage = cursor.index;
        }
        break;
    }
    case 3:
        if (timer > 0x13) enter_spell_list(anm);
        break;
    case 4:
        if (timer > 5) {
            dismiss(anm, 0x71);
            dismiss(anm, 0xd7);
            dismiss(anm, 0x11c);
            set_screen(kMainMenu);
            cursor.pop();
            dismiss(anm, kLeafEmitter);
            events.push_back({"screen", screen});
        }
        break;
    default:
        break;
    }
}

void TitleScene::enter_spell_list(AnmManager& anm) {
    // LAB_004556b1
    dismiss(anm, 0x71);
    set_screen(kSpellList);
    spell_group = cursor.index;
    cursor.push();
    cursor.set(0);
    events.push_back({"screen", screen});
}

// 0x452330：Player Data（难度←→ / 机体↑↓ / 翻页确定；取消回主菜单）
void TitleScene::update_score(AnmManager& anm, const InputState& input) {
    MenuCursor& shot = cursor;
    MenuCursor& difficulty = score_difficulty_cursor;
    switch (sub) {
    case 0: {
        shot.count = 4;
        shot.set(0);
        difficulty.count = 5;
        difficulty.set(1);
        difficulty.wrap = true;
        score_page_cursor.count = (SpellPracticeTable::count_with_difficulty(difficulty.index) + 9) / 10 + 1;
        score_page_cursor.set(0);
        score_page_cursor.wrap = true;
        if (vm[kLeafEmitter] == 0 && ascii_file) spawn_into(anm, kLeafEmitter, *ascii_file, kLeafEmitterScript);
        if (title_file) {
            spawn_into(anm, 0x6d, *title_file, 0x6d);  // +0x474
            set_sub(1);
            spawn_into(anm, kScoreShotLabel + shot.index, *title_file, kScoreShotLabel + shot.index);
            spawn_into(anm, kScoreDifficultyLabel + difficulty.index, *title_file,
                       kScoreDifficultyLabel + difficulty.index);
            for (i32 s : {0xb7, 0xb8, 0xb9, 0xba, 0xb4, 0xb5, 0xb6, 0xbb}) spawn_into(anm, s, *title_file, s);
        } else {
            set_sub(1);
        }
        events.push_back({"score", 0});
        [[fallthrough]];
    }
    case 1:
        if (timer > 6) set_sub(2);
        break;
    case 2: {
        const i32 prev_shot = shot.index;
        const i32 prev_difficulty = difficulty.index;
        const u32 edge = input.pressed | input.repeat;
        if (edge & InputState::kUp) {
            difficulty.move(-1);
            anm.interrupt_now(vm[0xb9], 2);
        }
        if (edge & InputState::kDown) {
            difficulty.move(1);
            anm.interrupt_now(vm[0xba], 2);
        }
        if (difficulty.index != prev_difficulty) {
            sfx(10);
            dismiss(anm, kScoreDifficultyLabel + prev_difficulty);
            if (title_file) spawn_into(anm, kScoreDifficultyLabel + difficulty.index, *title_file,
                                       kScoreDifficultyLabel + difficulty.index);
            if (score_page_cursor.index > 0) {
                score_page_cursor.set(1);
                refresh_score_spell_page(anm);
            }
            score_page_cursor.count = (SpellPracticeTable::count_with_difficulty(difficulty.index) + 9) / 10 + 1;
        }
        if (edge & InputState::kLeft) {
            shot.move(-1);
            anm.interrupt_now(vm[0xb7], 2);
        }
        if (edge & InputState::kRight) {
            shot.move(1);
            anm.interrupt_now(vm[0xb8], 2);
        }
        if (shot.index != prev_shot) {
            sfx(10);
            dismiss(anm, kScoreShotLabel + prev_shot);
            if (title_file) spawn_into(anm, kScoreShotLabel + shot.index, *title_file, kScoreShotLabel + shot.index);
            if (score_page_cursor.index > 0) refresh_score_spell_page(anm);
        }
        if (input.pressed & InputState::kConfirmMask) {
            // 排名页翻到符卡页时先从 text.anm 生成 10 行；翻回排名页时打断 1 收起
            AnmFile* text = anm.file(0);
            if (score_page_cursor.index == 0) {
                for (i32 i = 0; i < 10; ++i) row_text_vms[i] = text ? anm.spawn(*text, 3 + i, -1) : 0;
            }
            score_page_cursor.move(1);
            if (score_page_cursor.index == 0) {
                dismiss_score_page_rows(anm);
            } else {
                refresh_score_spell_page(anm);
            }
            sfx(7);
            events.push_back({"score-page", score_page_cursor.index});
        }
        if (difficulty.index == 4 && shot.index == 3) update_score_secret(input);
        if (input.pressed & InputState::kCancelMask) {
            set_sub(3);
            sfx(9);
            dismiss(anm, kScoreDifficultyLabel + difficulty.index);
            dismiss(anm, kScoreShotLabel + shot.index);
            for (i32 s : {0xb7, 0xb8, 0xb9, 0xba, 0xb4, 0xb5, 0xb6, 0xbb}) dismiss(anm, s);
            dismiss_score_page_rows(anm);
        }
        break;
    }
    case 3:
        if (timer > 5) {
            dismiss(anm, 0x6d);
            dismiss(anm, kLeafEmitter);
            set_screen(kMainMenu);
            cursor.pop();
            events.push_back({"screen", screen});
        }
        break;
    default:
        break;
    }
}

void TitleScene::update_score_secret(const InputState& input) {
    // DAT_004936d8：DIK 序列 I L O V E B E E R
    static constexpr u8 kSequence[9] = {0x17, 0x26, 0x18, 0x2f, 0x12, 0x30, 0x12, 0x12, 0x13};
    // 0x402130 默认路径（DAT_004c1800 bit10 清）：GetKeyboardState 后只把 VK 'A'..'Z' 搬到对应 DIK 位置
    static constexpr u8 kLetterDik[26] = {0x1e, 0x30, 0x2e, 0x20, 0x12, 0x21, 0x22, 0x23, 0x17, 0x24, 0x25, 0x26, 0x32,
                                          0x31, 0x18, 0x19, 0x10, 0x13, 0x1f, 0x14, 0x16, 0x2f, 0x11, 0x2d, 0x15, 0x2c};
    if (input.pressed & (InputState::kConfirmMask | InputState::kCancelMask)) {
        secret_progress = 0;
        secret_timer = 0;
    }
    std::memcpy(secret_prev_keys, secret_keys, sizeof(secret_keys));
    std::memset(secret_keys, 0, sizeof(secret_keys));
    if (keyboard_state != nullptr) {
        for (u8 dik : kLetterDik) secret_keys[dik] = keyboard_state[dik];
        u8 pressed[256];
        for (int i = 0; i < 256; ++i) pressed[i] = u8((secret_prev_keys[i] ^ secret_keys[i]) & secret_keys[i]);
        if (secret_progress < 9) {
            if (pressed[kSequence[secret_progress]] & 0x80) {
                ++secret_progress;
                secret_timer = 0;
            } else {
                u8 any = 0;
                for (int i = 0; i < 0x39; ++i) any |= pressed[i];
                if (any & 0x80) secret_progress = 0;
            }
        } else {
            if (scores != nullptr) scores->unlock_all();  // 0x44a930
            sfx(0x11);
            events.push_back({"score-unlock", 1});
            secret_progress = 0;
        }
    }
    if (++secret_timer > 300) {
        secret_progress = 0;
        secret_timer = 0;
    }
}

void TitleScene::dismiss_score_page_rows(AnmManager& anm) {
    // 0x46f0b0(…, 1)：句柄不清零（与符卡练习名行共用 +0x740）
    for (u32 id : row_text_vms) anm.interrupt(id, 1);
}

void TitleScene::refresh_score_spell_page(AnmManager& anm) {
    // 0x452c30：第 page 页 = 该难度（DAT_00491700）第 (page−1)×10 条起的 10 张符卡；
    // 名字取合计块（全机体见过才显示），收取 / 挑战次数取当前机体块的通常模式计数
    static constexpr char kDigits[10][3] = {"\x82\x4f", "\x82\x50", "\x82\x51", "\x82\x52", "\x82\x53",
                                            "\x82\x54", "\x82\x55", "\x82\x56", "\x82\x57", "\x82\x58"};
    static constexpr char kWideSpace[] = "\x81\x40";  // DAT_00493358
    static constexpr char kSeenFmt[] = "No.%s%s%s %s %4d/%4d";  // DAT_0049335c
    static constexpr char kUnseenFmt[] =  // DAT_00493374
        "No.%s%s%s \x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48"
        "\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48 %4d/%4d";
    static constexpr size_t kNameWidth = 0x2a;

    const i32 difficulty = score_difficulty_cursor.index;
    const i32 skip = score_page_cursor.index * 10 - 10;
    i32 id = 0;
    for (i32 seen = 0; seen < skip; ++id) {
        if (SpellPracticeTable::spell_difficulty(id) == difficulty) ++seen;
    }
    i32 row = 0;
    for (; row < 10; ++row, ++id) {
        while (id < ScoreFile::kSpellRecords && SpellPracticeTable::spell_difficulty(id) != difficulty) ++id;
        if (id >= ScoreFile::kSpellRecords) break;
        AnmVm* v = anm.find(row_text_vms[row]);
        if (v == nullptr) row_text_vms[row] = 0;
        const i32 number = id + 1;
        const char* hundreds = number / 100 != 0 ? kDigits[number / 100] : kWideSpace;
        const char* tens = (number / 10) % 10 != 0 || number / 100 != 0 ? kDigits[(number / 10) % 10] : kWideSpace;
        const char* ones = kDigits[number % 10];
        const SpellRecord& own = scores->blocks[cursor.index].spells[id];
        const SpellRecord& all = scores->blocks[ScoreFile::kAllShotsBlock].spells[id];
        char line[0xc0];
        if (all.attempts[0] == 0) {
            std::snprintf(line, sizeof(line), kUnseenFmt, hundreds, tens, ones, int(own.captures[0]),
                          int(own.attempts[0]));
            if (v) text_paint_vm_middle(anm, *v, 0x808080u, 0, 0, 0, line, text_paints);
            continue;
        }
        char name[0xa8];
        std::snprintf(name, sizeof(name), "%.*s", int(sizeof(all.name)), all.name);
        size_t len = std::strlen(name);
        if (len < kNameWidth) {
            for (size_t pad = ((kNameWidth - 1 - len) >> 1) + 1; pad != 0; --pad) {
                std::memcpy(name + len, kWideSpace, 3);
                len += 2;
            }
        }
        name[len] = '\0';
        std::snprintf(line, sizeof(line), kSeenFmt, hundreds, tens, ones, name, int(own.captures[0]),
                      int(own.attempts[0]));
        if (v) text_paint_vm_middle(anm, *v, own.captures[0] != 0 ? 0xffff80u : 0xefefefu, 0, 0, 0, line, text_paints);
    }
    for (; row < 10; ++row) {
        AnmVm* v = anm.find(row_text_vms[row]);
        if (v == nullptr) row_text_vms[row] = 0;
        if (v) text_paint_vm_middle(anm, *v, 0xffffffffu, 0, 0, 0, " ", text_paints);
    }
}

// 0x455900：分组内符卡列表（确定 → 0x13 / 取消 → 0x11）。0x4560b0 填 id，0x456060 解锁。
void TitleScene::update_spell_list(AnmManager& anm, const InputState& input) {
    GameGlobals& g = *globals;
    switch (sub) {
    case 0: {
        cursor.count = SpellPracticeTable::group_count(spell_group);
        if (anm.find(vm[0x6b]) == nullptr) spawn_into(anm, 0x6b, *title_file, 0x6b);  // +0x46c
        if (anm.find(vm[0xd8]) == nullptr) spawn_into(anm, 0xd8, *title_file, 0xd8);  // +0x620
        set_sub(1);
        // 列表框按条数收起多余的行（脚本 0x27 往下）
        for (i32 i = 0; i < 0xd - cursor.count; ++i) anm.interrupt_now(vm[0xd8], to_short(0x27 - i));
        events.push_back({"spell-list", spell_group});
        if (g.spell_return_list >= 0) {
            cursor.set(g.spell_return_list);
            g.spell_return_list = -1;
            refresh_spell_slot_names(anm, cursor.index);
            anm.interrupt_now(vm[0xd8], 3);
            anm.interrupt_now(vm[0xd8], to_short(cursor.index + 7));
            anm.interrupt_now(vm[0xd8], 6);
            enter_spell_entries(anm);
            return;
        }
        refresh_spell_slot_names(anm, cursor.index);
        [[fallthrough]];
    }
    case 1:
        if (timer > 10) {
            set_sub(2);
            anm.interrupt_now(vm[0xd8], 3);
            anm.interrupt(vm[0xd8], to_short(cursor.index + 7));
        }
        break;
    case 2: {
        const i32 prev = cursor.index;
        const u32 edge = input.pressed | input.repeat;
        if (edge & InputState::kUp) cursor.move(-1);
        if (edge & InputState::kDown) cursor.move(1);
        if (cursor.index != prev) {
            sfx(10);
            anm.interrupt_now(vm[0xd8], 3);
            anm.interrupt(vm[0xd8], to_short(cursor.index + 7));
            refresh_spell_slot_names(anm, cursor.index);
        }
        move_spell_shot(anm, input);
        if (input.pressed & InputState::kCancelMask) {
            set_sub(4);
            sfx(9);
        } else if (input.pressed & InputState::kConfirmMask) {
            if (scores != nullptr && !SpellPracticeTable::unlocked(spell_group, cursor.index, *scores)) {
                sfx(0x10);
            } else {
                anm.interrupt(vm[0xd8], 6);
                set_sub(3);
                sfx(7);
            }
        }
        break;
    }
    case 3:
        if (timer >= 0xe) enter_spell_entries(anm);
        break;
    case 4:
        if (timer > 5) {
            for (u32 id : row_text_vms) anm.interrupt(id, 1);
            for (i32 i = 0; i < 7; ++i) anm.kill(vm[0x10d + i]);
            dismiss(anm, 0x6b);
            dismiss(anm, 0xd8);
            dismiss(anm, 0xd7);
            set_screen(kSpellPractice);
            cursor.pop();
            events.push_back({"screen", screen});
        }
        break;
    default:
        break;
    }
}

void TitleScene::enter_spell_entries(AnmManager& anm) {
    // LAB_00455bd4
    set_screen(kSpellShot);
    spell_list_index = cursor.index;
    cursor.push();
    dismiss(anm, 0x6b);
    cursor.set(0);
    events.push_back({"screen", screen});
}

// 0x456a20：符卡条目页（Extra 组直接开战；否则 → 0x14；取消 → 0x12）
void TitleScene::update_spell_shot(AnmManager& anm, const InputState& input) {
    switch (sub) {
    case 0:
        cursor.count = 5;
        for (i32 i = 0; i < 5; ++i) {
            if (spell_slot_ids[i] < 0) cursor.disable(i);
        }
        cursor.set(0);
        if (globals->spell_return_entry >= 0) {
            cursor.set(globals->spell_return_entry);
            globals->spell_return_entry = -1;
        }
        highlight_spell_entry(anm, cursor.index);
        set_sub(1);
        events.push_back({"spell-shot", spell_group});
        [[fallthrough]];
    case 1:
        if (timer > 10) set_sub(2);
        break;
    case 2: {
        const i32 prev = cursor.index;
        const u32 edge = input.pressed | input.repeat;
        if (edge & InputState::kUp) cursor.move(-1);
        if (edge & InputState::kDown) cursor.move(1);
        if (cursor.index != prev) {
            sfx(10);
            highlight_spell_entry(anm, cursor.index);
        }
        move_spell_shot(anm, input);
        if (input.pressed & InputState::kCancelMask) {
            set_sub(4);
            sfx(9);
            break;
        }
        if (input.pressed & InputState::kConfirmMask) {
            anm.interrupt(row_text_vms[cursor.index], 6);
            set_sub(3);
            sfx(7);
            if (spell_group == 6) {
                bgm_fade();  // 0x43c470
                sfx(0x32);
            }
        }
        break;
    }
    case 3:
        if (spell_group == 6) {
            if (timer == 10) transition_effect();
            if (timer > 0x27) launch_spell_practice(cursor.index, 4);
        } else if (timer > 0xd) {
            spell_entry_index = cursor.index;  // +0x5dcc
            set_screen(kSpellStart);
            cursor.push();
            dismiss(anm, 0x6b);
            cursor.set(0);
            events.push_back({"screen", screen});
        }
        break;
    case 4:
        if (timer > 5) {
            dismiss(anm, 0xd8);
            set_screen(kSpellList);
            cursor.pop();
            events.push_back({"screen", screen});
        }
        break;
    default:
        break;
    }
}

// 0x455d50：符卡练习子季节页（画面 0x14；确定开战；取消 → 0x13）
void TitleScene::update_spell_start(AnmManager& anm, const InputState& input) {
    switch (sub) {
    case 0:
        cursor.count = 4;
        cursor.set(0);
        if (anm.find(vm[0xd9]) == nullptr) spawn_into(anm, 0xd9, *title_file, 0xd9);  // +0x624
        set_sub(1);
        events.push_back({"spell-start", spell_group});
        [[fallthrough]];
    case 1:
        if (timer > 10) {
            set_sub(2);
            anm.interrupt_now(vm[0xd9], 3);
            anm.interrupt(vm[0xd9], to_short(cursor.index + 7));
        }
        break;
    case 2: {
        const i32 prev = cursor.index;
        const u32 edge = input.pressed | input.repeat;
        if (edge & InputState::kUp) cursor.move(-1);
        if (edge & InputState::kDown) cursor.move(1);
        if (cursor.index != prev) {
            sfx(10);
            anm.interrupt_now(vm[0xd9], 3);
            anm.interrupt(vm[0xd9], to_short(cursor.index + 7));
        }
        move_spell_shot(anm, input);
        if (input.pressed & InputState::kCancelMask) {
            set_sub(4);
            sfx(9);
            break;
        }
        if (input.pressed & InputState::kConfirmMask) {
            anm.interrupt(vm[0xd9], 6);
            set_sub(3);
            sfx(7);
            bgm_fade();  // 0x43c470
            sfx(0x32);
        }
        break;
    }
    case 3:
        if (timer == 10) transition_effect();
        if (timer > 0x27) launch_spell_practice(spell_entry_index, cursor.index);
        break;
    case 4:
        if (timer > 5) {
            dismiss(anm, 0x6a);
            dismiss(anm, 0xd9);
            set_screen(kSpellShot);
            cursor.pop();
            events.push_back({"screen", screen});
        }
        break;
    default:
        break;
    }
}

// 0x44e930：Key Config——7 项：Shot / Bomb / Release / Slow / Pause / Default / Quit。配置的是手柄按钮号
// （DAT_004a52e4 表），每帧取 0x401c30 快照中首个按下的按钮写入光标槽（0x44f710）；键盘键位固定（0x401d50）
void TitleScene::update_key_config(AnmManager& anm, const InputState& input) {
    switch (sub) {
    case 0:
        cursor.count = 7;
        cursor.wrap = true;
        cursor.set(0);
        if (title_file) spawn_into(anm, 2, *title_file, 2);  // +0x2c8 面板脚本 2
        set_sub(1);
        load_key_table();
        paint_key_table(anm);
        events.push_back({"key-config", 0});
        [[fallthrough]];
    case 1:
        if (timer > 6) {
            set_sub(2);
            if (vm[2]) {
                anm.interrupt_now(vm[2], 3);
                anm.interrupt(vm[2], to_short(cursor.index + 0x11));
            }
            refresh_key_config_items(anm);
        }
        break;
    case 2: {
        const i32 prev = cursor.index;
        const u32 edge = input.pressed | input.repeat;
        if (edge & InputState::kUp) cursor.move(-1);
        if (edge & InputState::kDown) cursor.move(1);
        if (cursor.index != prev) {
            sfx(10);
            if (vm[2]) {
                anm.interrupt_now(vm[2], 3);
                anm.interrupt(vm[2], to_short(cursor.index + 7));
            }
            refresh_key_config_items(anm);
        }
        // 0x401c30：首个按下的手柄按钮（0..0x1e）→ 当前键位槽
        if (pad_buttons != nullptr) {
            for (i32 button = 0; button < 0x1f; ++button) {
                if ((*pad_buttons >> button & 1u) == 0) continue;
                if (cursor.index < 5) assign_key_button(anm, cursor.index, button);
                break;
            }
        }
        // 取消只在 Quit(6) 上生效：丢弃改动并退出
        if ((input.pressed & InputState::kCancelMask) && cursor.index == 6) {
            load_key_table();
            paint_key_table(anm);
            sfx(9);
            if (vm[2]) anm.interrupt(vm[2], 6);
            set_sub(4);
            break;
        }
        if (input.pressed & InputState::kConfirmMask) {
            if (cursor.index == 5) {
                // Default：回到进入时的全局表 DAT_004a52e4（不是 exe 初值）
                load_key_table();
                paint_key_table(anm);
                sfx(7);
                events.push_back({"key-default", 0});
            } else if (cursor.index == 6) {
                commit_key_table();
                events.push_back({"key-commit", 0});
                sfx(9);
                if (vm[2]) anm.interrupt(vm[2], 6);
                set_sub(4);
            }
        }
        break;
    }
    case 4:
        if (timer > 9) {
            dismiss(anm, 2);
            set_screen(kOption);
            cursor.pop();
            events.push_back({"screen", screen});
        }
        break;
    default:
        break;
    }
}

void TitleScene::load_key_table() {
    if (globals == nullptr) return;
    key_table[0] = globals->pad_buttons[0];
    key_table[1] = globals->pad_buttons[1];
    key_table[2] = globals->pad_buttons[9];
    key_table[3] = globals->pad_buttons[2];
    key_table[4] = globals->pad_buttons[3];
}

void TitleScene::commit_key_table() {
    if (globals == nullptr) return;
    globals->pad_buttons[0] = key_table[0];
    globals->pad_buttons[1] = key_table[1];
    globals->pad_buttons[9] = key_table[2];
    globals->pad_buttons[2] = key_table[3];
    globals->pad_buttons[3] = key_table[4];
}

// 0x44f710
void TitleScene::assign_key_button(AnmManager& anm, i32 slot, i32 button) {
    if (slot < 0 || slot >= kKeyTableCount) return;
    const i16 old = key_table[slot];
    if (old == button) return;
    for (int i = 0; i < kKeyTableCount; ++i) {
        if (i != slot && key_table[i] == button) key_table[i] = old;
    }
    key_table[slot] = i16(button);
    paint_key_table(anm);
    sfx(7);
    events.push_back({"key-bind", slot});
    events.push_back({"key-button", button});
}

// 0x44ec60：槽 i 的十位 / 个位 → 子脚本 0x3f+2i / 0x40+2i 与 0x49+2i / 0x4a+2i（精灵 0x2a 起；-1 按 C 截断得 0 / -1）
void TitleScene::paint_key_table(AnmManager& anm) {
    if (vm[2] == 0) return;
    auto paint = [&](i32 script, i32 sprite) {
        AnmVm* digit = anm.find(child_id(anm, 2, script));
        if (digit == nullptr) return;
        if (AnmFile* file = anm.file(digit->file_slot)) anm_set_sprite(*file, *digit, sprite);
    };
    for (i32 i = 0; i < 5; ++i) {
        const i32 value = key_table[i];
        paint(0x3f + 2 * i, value / 10 + 0x2a);
        paint(0x40 + 2 * i, value % 10 + 0x2a);
        paint(0x49 + 2 * i, value / 10 + 0x2a);
        paint(0x4a + 2 * i, value % 10 + 0x2a);
    }
}

// 0x44f810：光标前的项（标签 i+0x31 / i+0x38 与数字 0x3f+2i..）→ 0x1e，光标后的 → 0x1f
void TitleScene::refresh_key_config_items(AnmManager& anm) {
    if (vm[2] == 0) return;
    auto interrupt_now = [&](i32 script, i32 value) {
        AnmVm* item = anm.find(child_id(anm, 2, script));
        if (item == nullptr) return;
        item->pending_interrupt = value;
        anm.run(*item);
    };
    auto mark = [&](i32 i, i32 value) {
        interrupt_now(i + 0x31, value);
        interrupt_now(i + 0x38, value);
        if (i < 5) {
            interrupt_now(0x3f + 2 * i, value);
            interrupt_now(0x40 + 2 * i, value);
            interrupt_now(0x49 + 2 * i, value);
            interrupt_now(0x4a + 2 * i, value);
        }
    };
    for (i32 i = 0; i < cursor.index; ++i) mark(i, 0x1e);
    for (i32 i = cursor.index + 1; i < 7; ++i) mark(i, 0x1f);
}

void TitleScene::begin_name_input() {
    name_pick.set(0);
    name_pick.count = kNameCharCount;
    name_pick.wrap = true;
    std::memcpy(name_entry, scores != nullptr ? scores->stats.last_name : ScoreFile::kDefaultName, 9);
    if (std::strcmp(name_entry, ScoreFile::kDefaultName) != 0) name_pick.move(-1);
    i32 len = 8;
    while (len > 0 && name_entry[len - 1] == ' ') --len;
    name_cursor = len;
}

void TitleScene::navigate_name_pick(const InputState& input) {
    const u32 edge = input.pressed | input.repeat;
    name_pick_prev = name_pick.index;
    if (edge & InputState::kUp) name_pick.move(-kNameCols);
    if (edge & InputState::kDown) name_pick.move(kNameCols);
    if (edge & InputState::kLeft) name_pick.move(name_pick.index % kNameCols == 0 ? kNameCols - 1 : -1);
    if (edge & InputState::kRight) name_pick.move(name_pick.index % kNameCols == kNameCols - 1 ? -(kNameCols - 1) : 1);
    if (name_pick_prev != name_pick.index) sfx(10);
}

bool TitleScene::apply_name_pick() {
    const i32 pick = name_pick.index;
    if (pick <= kNameSpace) {
        const char c = pick < kNameSpace ? kNameChars[pick] : ' ';
        if (name_cursor < 8) {
            name_entry[name_cursor] = c;
            if (++name_cursor > 7) name_pick.set(kNameEnd);
        } else {
            name_entry[name_cursor - 1] = c;
        }
    } else if (pick == kNameBack) {
        if (name_cursor == 0) return false;
        --name_cursor;
        name_entry[name_cursor] = ' ';
    }
    return true;
}

// 0x4532f0：通关回标题的名字输入 / 排名登记
void TitleScene::update_name_entry(AnmManager& anm, const InputState& input) {
    switch (sub) {
    case 0: {
        cursor.count = 0x1e;
        bgm_name_entry();  // FUN_0043c370(0,"th128_08")
        if (scores != nullptr) scores->mark_bgm_heard(0x11);   // 0x43c3f0(0, 0x11)
        if (vm[kLeafEmitter] == 0 && ascii_file) spawn_into(anm, kLeafEmitter, *ascii_file, kLeafEmitterScript);
        if (title_file) spawn_into(anm, 0x6f, *title_file, 0x6f);
        set_sub(1);
        const i32 shot = globals != nullptr ? globals->shot + globals->shot_aux : 0;
        const i32 diff = globals != nullptr ? globals->difficulty : 0;
        if (title_file) spawn_into(anm, shot + 0xa7, *title_file, shot + 0xa7);
        if (title_file) spawn_into(anm, diff + 0xaf, *title_file, diff + 0xaf);
        if (anm.find(vm[kDecor61]) == nullptr && title_file) {
            spawn_into(anm, kDecor61, *title_file, kDecor61);
            anm.interrupt_now(vm[kDecor61], 3);
        }
        name_rank = -1;
        if (scores != nullptr && globals != nullptr) {
            // 0x43e250 期间 DAT_004a5790 = 8（入榜关卡记 "All"），之后关卡 / 关卡表复位
            name_rank = scores->insert_rank(shot, diff, u32(globals->score), 8, u8(globals->continues),
                                            globals->season, 0, 0);
            globals->stage = 0;
            globals->stage_copy = 0;
        }
        if (name_rank < 0) {
            cursor.set(-1);
            name_no_rank = 1;
        } else {
            timer = 0;
            cursor.set(name_rank);
            begin_name_input();
            name_no_rank = 0;
        }
        events.push_back({"name-entry", name_rank});
        [[fallthrough]];
    }
    case 1:
        if (timer > 6) set_sub(2);
        break;
    case 2:
        if (name_no_rank == 0) navigate_name_pick(input);
        if (input.pressed & InputState::kConfirmMask) {
            if (name_no_rank == 0) {
                if (name_pick.index == kNameEnd) {
                    name_entry[8] = '\0';
                    if (scores != nullptr && globals != nullptr && name_rank >= 0) {
                        ScoreEntry& e = scores->blocks[globals->shot + globals->shot_aux].ranks[globals->difficulty][name_rank];
                        std::memcpy(e.name, name_entry, 9);
                    }
                    if (scores != nullptr) std::memcpy(scores->stats.last_name, name_entry, 9);
                    events.push_back({"name-done", name_rank});
                    set_sub(3);
                } else if (!apply_name_pick()) {
                    break;
                }
            } else {
                set_sub(3);
            }
            sfx(7);
        }
        if (input.pressed & InputState::kCancelMask) {
            if (name_no_rank != 0) {
                set_sub(3);
                sfx(7);
                break;
            }
            if (name_cursor != 0) {
                sfx(9);
                --name_cursor;
                name_entry[name_cursor] = ' ';
            }
        }
        break;
    case 3:
        if (timer > 5) {
            const i32 shot = globals != nullptr ? globals->shot + globals->shot_aux : 0;
            const i32 diff = globals != nullptr ? globals->difficulty : 0;
            dismiss(anm, 0x6f);
            dismiss(anm, shot + 0xa7);
            dismiss(anm, diff + 0xaf);
            if (globals != nullptr && globals->continues == 0) {
                set_screen(kNameConfirm);
                events.push_back({"screen", screen});
                break;
            }
            dismiss(anm, kLeafEmitter);
            cursor.pop();
            set_screen(kMainMenu);
            bgm_default();  // FUN_0043c370(0,"th16_01")
            if (scores != nullptr) scores->mark_bgm_heard(0);   // 0x43c3f0(0, 0)
            events.push_back({"screen", screen});
        }
        break;
    default:
        break;
    }
}

// 0x453c10：未续关通关后存回放（25 槽列表 → 名字输入 → 0x448400 写 th16_%.2d.rpy）
void TitleScene::update_name_confirm(AnmManager& anm, const InputState& input) {
    switch (sub) {
    case 0:
        cursor.count = 0x19;
        cursor.set(0);
        if (globals != nullptr) {
            globals->stage = 8;
            globals->stage_copy = 8;
        }
        for (i32 i = 0; i < 0x19; ++i) {
            save_slots[i] = ReplaySummary{};
            save_slot_live[i] = read_replay_slot && read_replay_slot(i + 1, save_slots[i]);
        }
        if (anm.find(vm[kDecor61]) == nullptr && title_file) {
            spawn_into(anm, kDecor61, *title_file, kDecor61);
            anm.interrupt_now(vm[kDecor61], 3);
        }
        if (title_file) spawn_into(anm, 0x70, *title_file, 0x70);
        set_sub(1);
        events.push_back({"replay-save", 0});
        [[fallthrough]];
    case 1:
        if (timer > 6) set_sub(2);
        break;
    case 2: {
        const u32 edge = input.pressed | input.repeat;
        prev_cursor = cursor.index;
        if (edge & InputState::kUp) cursor.move(-1);
        if (edge & InputState::kDown) cursor.move(1);
        if (prev_cursor != cursor.index) sfx(10);
        if (input.pressed & InputState::kCancelMask) {
            set_sub(4);
            sfx(9);
        } else if (input.pressed & InputState::kConfirmMask) {
            save_slot_choice = cursor.index;
            if (on_replay_full_run) on_replay_full_run();  // 0x4483b0(1)
            begin_name_input();
            sfx(7);
            set_sub(3);
        }
        break;
    }
    case 3:
        navigate_name_pick(input);
        if (input.pressed & InputState::kConfirmMask) {
            if (name_pick.index == kNameEnd) {
                sfx(0x11);
                name_entry[8] = '\0';
                const i32 slot = cursor.index + 1;
                if (write_replay_slot) write_replay_slot(slot, name_entry);
                save_slots[cursor.index] = ReplaySummary{};
                save_slot_live[cursor.index] = read_replay_slot && read_replay_slot(slot, save_slots[cursor.index]);
                if (scores != nullptr) std::memcpy(scores->stats.last_name, name_entry, 9);
                events.push_back({"replay-saved", slot});
                set_sub(2);
            } else if (!apply_name_pick()) {
                break;
            }
            sfx(7);
        }
        if ((input.pressed & InputState::kCancelMask) == 0) break;
        if (name_cursor != 0) {
            sfx(9);
            --name_cursor;
            name_entry[name_cursor] = ' ';
            break;
        }
        set_sub(2);
        break;
    case 4:
        if (timer > 5) {
            dismiss(anm, 0x70);
            dismiss(anm, kLeafEmitter);
            set_screen(kMainMenu);
            cursor.pop();
            bgm_default();  // FUN_0043c370(0,"th16_01")
            events.push_back({"screen", screen});
            for (i32 i = 0; i < 0x19; ++i) {
                save_slot_live[i] = false;
                save_slots[i] = ReplaySummary{};
            }
        }
        break;
    default:
        break;
    }
}

// 0x44af80 case 9 / 0xc：原版无对应画面函数，只每帧 0x43c440 停 BGM（不读输入、不切画面）；
// 本作标题流程不会置这两个值，保留以与分派表同构
void TitleScene::update_stub(AnmManager& anm, const InputState& input) {
    (void)anm;
    (void)input;
    if (sub == 0) {
        set_sub(1);
        events.push_back({"stub", screen});
    }
    play_bgm_op(3);
}

}  // namespace th16
