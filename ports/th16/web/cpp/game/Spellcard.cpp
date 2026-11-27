// th16 Spellcard——原版 0x4178a0 / 0x417f00 / 0x417930 / 0x4182f0（DAT_004a6db0）。
#include "Spellcard.hpp"

#include <cstring>

#include "AnmManager.hpp"
#include "Ascii.hpp"
#include "Enemy.hpp"
#include "GameGlobals.hpp"
#include "Gui.hpp"
#include "Player.hpp"
#include "ScoreFile.hpp"
#include "Stage.hpp"
#include "StageTable.hpp"
#include "TextPaint.hpp"

namespace th16 {

namespace {

// 0x417f00 内联：难度 → 符卡基础奖励（× 关卡号）
constexpr i32 kBonusByDifficulty[5] = {500000, 1000000, 1500000, 2000000, 1000000};

}  // namespace

// 0x4178a0
void Spellcard::reset() {
    // 0x4178a0：整块清零（`*_Dst |= 2` 落在 +0 的链表字段而非 +0x78 标志）；0x417700 末尾 Timer::reset（previous = -1）
    for (u32& id : vm_ids) id = 0;
    timer = Timer{};
    timer.reset();
    std::memset(name, 0, sizeof(name));
    id = 0;
    flags = 0;
    bonus = bonus_initial = time_limit = 0;
    capture_count = 0;
    state = 0;
    boss_pos = Vec3{};
    sfx_queue.clear();
}

void Spellcard::destroy() {
    // 0x417790
    if (animations != nullptr) {
        for (int i = 1; i <= 3; ++i) animations->kill(vm_ids[i]);
    }
    reset();
}

// 0x417f00 / 0x417930：EnemyManager+0x48 槽 0 的 Boss（0x417580；未命中落在链表最后一只）
Vec3 Spellcard::boss_position() const {
    const Enemy* boss = enemies != nullptr ? enemies->boss_slot(0) : nullptr;
    return boss != nullptr ? boss->pos : Vec3{};
}

// 0x417f00
void Spellcard::start(i32 spell_id, const char* spell_name, i32 limit, i32 anm_index) {
    timer.reset();
    id = spell_id;
    std::strncpy(name, spell_name, sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';
    flags = (flags & ~(kSurvival | kNoTimeout | kTimedOut)) | (kActive | kBonusAvailable);
    if (scores != nullptr && !replay_playback) scores->record_spell_attempt(record_shot(), id, is_spell_practice(), name);
    if (gui != nullptr) gui->set_boss_timer_style(Gui::TimerStyle::Spell);   // 0x4175d0
    state = 1;
    flags &= ~(kBombUsed | kTiming);

    AnmManager* anm = animations;
    AnmFile* ascii = anm != nullptr ? anm->file(AnmManager::kAsciiSlot) : nullptr;
    AnmFile* text = anm != nullptr ? anm->file(0) : nullptr;
    AnmFile* effect = anm != nullptr ? anm->file(8) : nullptr;
    if (anm != nullptr) {
        vm_ids[1] = ascii != nullptr ? anm->spawn(*ascii, 0, -1) : 0;
        vm_ids[2] = text != nullptr ? anm->spawn(*text, 2, -1) : 0;
        vm_ids[3] = ascii != nullptr ? anm->spawn(*ascii, 1, -1) : 0;
        if (AnmVm* name_vm = anm->find(vm_ids[2])) {
            // 0x417f00 → 0x46dab0(AnmManager, name_vm, 0xffffff, 0, font=0, spacing=0, name)
            text_paint_vm_centered(*anm, *name_vm, 0xffffffu, 0u, 0, 0, name, text_paints);
        } else {
            vm_ids[2] = 0;
        }
    }
    play_sfx(0x21);

    // effect.anm 0xd 倒计时：entity_offset = Boss 坐标，子 VM（脚本 0xb / 0xc）的 $I2 = 时限
    boss_pos = boss_position();
    vm_ids[4] = 0;
    if (anm != nullptr && effect != nullptr) {
        vm_ids[4] = anm->spawn(*effect, 0xd, -1);
        if (AnmVm* vm = anm->find(vm_ids[4])) {
            vm->entity_offset = boss_pos;
            if (AnmVm* child = AnmManager::find_child(*vm, 0xb, 0)) child->int_vars[2] = limit;
            if (AnmVm* child = AnmManager::find_child(*vm, 0xc, 0)) child->int_vars[2] = limit;
        } else {
            vm_ids[4] = 0;
        }
    }
    time_limit = limit;

    const i32 difficulty = globals != nullptr ? globals->difficulty : 1;
    const i32 stage_number = globals != nullptr ? globals->stage : 1;
    i32 value = kBonusByDifficulty[(difficulty < 0 || difficulty > 4) ? 4 : difficulty] * stage_number;
    bonus = value;
    if (value > 999999999) value = 999999999;
    bonus_initial = value;
    if (anm != nullptr && effect != nullptr) anm->spawn(*effect, 0x14, -1);

    // 关卡表 Boss 块：spell_id < 0x2b（道中 Boss 段）且块 1 有背景 → 块 1，否则块 0
    if (entry != nullptr) {
        const i32 current_spell = globals != nullptr ? globals->spell_id : 0;
        const int block = (current_spell < 0x2b && entry->boss_anm[1].background_slot != -1) ? 1 : 0;
        const StageBossAnm& bg = entry->boss_anm[block];
        vm_ids[0] = 0;
        if (anm != nullptr && enemies != nullptr) {
            if (AnmFile* file = enemies->anm_file(bg.background_slot)) vm_ids[0] = anm->spawn(*file, bg.background_script, -1);
        }
        flags = (flags & ~kStageBackground) | (bg.background_keeps_stage != 0 ? kStageBackground : 0u);
        if (anm_index >= 0 && anm_index < 3) {
            const StageBossAnm& cutin = entry->boss_anm[anm_index];
            if (cutin.cutin_slot != -1 && anm != nullptr && enemies != nullptr) {
                if (AnmFile* file = enemies->anm_file(cutin.cutin_slot)) anm->spawn(*file, cutin.cutin_script, -1);
            }
        }
    }
}

// 0x417930
void Spellcard::update(float game_speed) {
    if ((flags & kActive) == 0) return;
    ++state;
    if (timer.current > 0x3b && (flags & kStageBackground) == 0 && stage != nullptr) stage->flags &= ~1u;
    if (timer.current > 299 && (flags & kSurvival) == 0) {
        bonus = ((bonus - (bonus_initial - bonus_initial / 3) / (time_limit - 300)) / 10) * 10;
    }
    timer.tick(game_speed);

    // 名牌翻转：自机 y < 96 → 名牌下移（中断 3），> 128 → 回到上方（中断 2）；kFlipped 时阈值 352 / 320
    if (timer.current > 0x77 && player != nullptr && animations != nullptr) {
        const float y = player->pos.y;
        const bool flipped = (flags & kFlipped) != 0;
        if ((flags & kNameLowered) == 0) {
            if ((!flipped && y < 96.0f) || (flipped && y > 352.0f)) {
                for (int i = 1; i <= 3; ++i) animations->interrupt(vm_ids[i], 3);
                flags |= kNameLowered;
            }
        } else if ((!flipped && y > 128.0f) || (flipped && y < 320.0f)) {
            for (int i = 1; i <= 3; ++i) animations->interrupt(vm_ids[i], 2);
            flags &= ~kNameLowered;
        }
    }

    const Vec3 target = boss_position();
    boss_pos.x = (target.x - boss_pos.x) * 0.05f + boss_pos.x;
    boss_pos.y = (target.y - boss_pos.y) * 0.05f + boss_pos.y;
    boss_pos.z = (target.z - boss_pos.z) * 0.05f + boss_pos.z;
    if (animations != nullptr) {
        if (AnmVm* vm = animations->find(vm_ids[4])) vm->entity_offset = boss_pos;
    }
    if ((flags & kBombUsed) != 0 && (enemies == nullptr || !enemies->bomb_active)) flags &= ~kBombUsed;
}

// 成绩文件机体块下标 DAT_004a57a4 + DAT_004a57a8；符卡练习（模式 2）记在第二组计数
int Spellcard::record_shot() const { return globals != nullptr ? globals->shot + globals->shot_aux : 0; }
bool Spellcard::is_spell_practice() const { return globals != nullptr && globals->mode() == 2; }

i32 Spellcard::encode_spell_time(double seconds) {
    if (seconds < 0) seconds = 0;
    i32 whole = i32(seconds);
    if (whole > 999) whole = 999;
    const i32 hundredths = i32((seconds - double(whole)) * 100.0);
    // ((s + 0x16 + h) * 1000 + (s + 0x42) % 1000) * 100 + (h + 0x21) % 100
    return ((whole + 0x16 + hundredths) * 1000 + (whole + 0x42) % 1000) * 100 + (hundredths + 0x21) % 100;
}

// 0x417bc0（由 0x45a9f0 族帧泵在更新链后调用）
void Spellcard::tick_realtime() {
    // Unicorn oracle / harness 默认 wall_clock_seconds<0：完全空操作（不置 kTiming、不改 +0xa4）
    if (wall_clock_seconds < 0) return;
    const double now = wall_clock_seconds;
    if ((flags & kActive) != 0) {
        if ((flags & kTiming) == 0) {
            flags |= kTiming;
            timing_start = now;
        }
        return;
    }
    if ((flags & kTiming) == 0) return;
    timed_frames = state;
    double duration = now - timing_start;
    // 原版再减 Sleep 残余（FUN_00487aca）；无消息泵时按 ≥0.00835 补一帧 0.0167
    if (duration >= 0.00835) duration += 0.0167;
    time_record = encode_spell_time(duration);
    timing_scratch = 0;
    flags &= ~kTiming;
    if (!replay_playback) {
        if (record_time) record_time(capture_count, time_record);
    } else {
        time_record = replay_time ? replay_time(capture_count) : 0;
        if (spell_time_invalid(time_record)) time_record = 0x6ad1584;
    }
    ++capture_count;
}

bool Spellcard::spell_time_invalid(i32 value) {
    return value / 100000 - 0x16 != ((value / 100) % 1000 + 0x3a6) % 1000 + (value % 100 + 0x43) % 100;
}

// 0x4182f0
void Spellcard::end() {
    if ((flags & kActive) == 0) return;
    if (stage != nullptr) stage->flags |= 1u;
    if (animations != nullptr) {
        for (int i = 1; i <= 3; ++i) animations->interrupt(vm_ids[i], 1);
    }
    flags &= ~kActive;
    if (animations != nullptr) animations->kill(vm_ids[0]);
    vm_ids[0] = 0;
    flags &= ~kBombUsed;
    if (gui != nullptr) gui->set_boss_timer_style(Gui::TimerStyle::Normal);   // 0x417650
    if (animations != nullptr) animations->kill(vm_ids[4]);
    vm_ids[4] = 0;
    if ((flags & kBonusAvailable) == 0) {
        if (gui != nullptr) gui->show_notice(Gui::Notice::SpellFailed, 0);
    } else {
        if (globals != nullptr) globals->add_score(bonus, [this] { if (gui != nullptr) gui->celebrate_extend(); });   // 0x43e080
        if (gui != nullptr) gui->show_notice(Gui::Notice::SpellBonus, bonus);
        if (scores != nullptr && !replay_playback) scores->record_spell_capture(record_shot(), id, is_spell_practice());
        play_sfx(0x2e);
    }
    if ((flags & kTimedOut) != 0) play_sfx(0x45);
}

// 0x417d70
void Spellcard::draw_text(AsciiManager& text) const {
    if ((flags & kActive) == 0 || animations == nullptr) return;
    const AnmVm* label = animations->find(vm_ids[3]);
    if (label == nullptr) return;
    AsciiManager::Style& style = text.style;
    style.font = AsciiManager::Font::Medium;
    style.group = AsciiManager::Group::FieldLate;
    text.set_alpha(label->color[3]);
    if ((flags & kBonusAvailable) != 0) {
        text.printf({266.0f, 35.0f, 0.0f}, "%8d", bonus);
    } else {
        text.printf({282.0f, 35.0f, 0.0f}, "$");
    }
    const SpellRecord* record = scores != nullptr ? scores->spell_record(record_shot(), id) : nullptr;
    const u32 captures = record != nullptr ? record->captures[is_spell_practice()] : 0;
    const u32 attempts = record != nullptr ? record->attempts[is_spell_practice()] : 0;
    const Vec3 record_pos{360.0f, 35.0f, 0.0f};
    if (captures >= 100) {
        text.printf(record_pos, "MASTER");
    } else if (attempts >= 100) {
        text.printf(record_pos, "%.2d/99+", captures);
    } else {
        text.printf(record_pos, "%.2d/%.2d", captures, attempts);
    }
    style.font = AsciiManager::Font::Default;
    style.group = AsciiManager::Group::Ui;
    text.set_alpha(0xff);
}

}  // namespace th16
