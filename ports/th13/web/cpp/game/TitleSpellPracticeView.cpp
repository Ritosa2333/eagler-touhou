#include "TitleSpellPracticeView.hpp"

#include <cstdio>

namespace th13 {

TitleSpellPracticeView::~TitleSpellPracticeView() {
    for (u32 handle : owned_) animations_.retire(handle);
}

bool TitleSpellPracticeView::fail(const std::string& reason) {
    if (error.empty()) error = reason;
    return false;
}

bool TitleSpellPracticeView::create(u32& handle, i32 script) {
    handle = animations_.create(bank_, script, -1, 0);
    if (!handle) return fail("Spell Practice animation: " + animations_.error);
    owned_.push_back(handle);
    return true;
}

bool TitleSpellPracticeView::interrupt(u32 handle, i32 label, bool immediate) {
    return !handle || animations_.interrupt(handle, label, immediate) ||
           fail("Spell Practice animation interrupt: " + animations_.error);
}

bool TitleSpellPracticeView::select(u32 handle, i32 cursor) {
    return interrupt(handle, 3, true) && interrupt(handle, 7 + cursor);
}

void TitleSpellPracticeView::release() {
    for (u32* handle : {&stage_, &cards_, &character_, &badge_}) {
        interrupt(*handle, 1);
        *handle = 0;
    }
    if (particles_) { interrupt(particles_, 1); particles_ = 0; }
    for (u32& handle : names_) { interrupt(handle, 1); handle = 0; }
    for (u32& handle : ranks_) { interrupt(handle, 1); handle = 0; }
    opened_ = false;
}

bool TitleSpellPracticeView::confirm(const TitleSpellPracticeMenu& menu) {
    if (menu.level() == SpellPracticeLevel::Stage) return interrupt(stage_, 6);
    if (menu.level() == SpellPracticeLevel::Spell) return interrupt(cards_, 6);
    return interrupt(names_[size_t(menu.cursor())], 6);
}

bool TitleSpellPracticeView::rebuild_names(i32 stage, i32 family, i32 character,
                                          const TitleSpellPracticePlatform& platform,
                                          const Rasterizer& rasterize) {
    for (u32& handle : ranks_) { if (handle) animations_.retire(handle); handle = 0; }
    if (stage != 7) {
        for (i32 rank = 0; rank < 4; ++rank)
            if (!create(ranks_[size_t(rank)], 229 + rank) ||
                !interrupt(ranks_[size_t(rank)], 3, true)) return false;
    }
    i32 last_row = -1;
    for (i32 row = 0; row < 5; ++row)
        if (platform.spell_id(stage, family, row) >= 0) last_row = row;
    for (i32 row = 0; row < 5; ++row) {
        if (row > last_row) {
            if (names_[size_t(row)]) animations_.retire(names_[size_t(row)]);
            names_[size_t(row)] = 0;
            continue;
        }
        if (!names_[size_t(row)] && !create(names_[size_t(row)], 182 + row)) return false;
        const i32 id = platform.spell_id(stage, family, row);
        const i32 rank = platform.spell_rank(stage, family, row);
        const bool available = id >= 0 && rank >= 0 &&
            platform.spell_variant_available(stage, family, row);
        std::string name = " Nothing ... ";
        u32 color = 0xff808080;
        if (id >= 0) {
            char number[24];
            std::snprintf(number, sizeof number, " No.%3d  ", id + 1);
            if (available) {
                SpellPracticeRecord record;
                platform.spell_record(character, id, record);
                auto title = platform.spell_variant_name(stage, family, row);
                if (!record.encountered) {
                    // 00456b50: the unlocked, unattempted Overdrive placeholder.
                    title = rank == 5 ?
                        "\x83\x49\x81\x5b\x83\x6f\x81\x5b\x83\x68\x83\x89\x83\x43\x83\x75\x83\x82\x81\x5b\x83\x68\x92\xa7\x90\xed\x89\xc2\x94\x5c\x81\x49" :
                        "\x92\xa7\x90\xed\x89\xc2\x94\x5c\x81\x49";
                }
                if (title.size() < 42) title.append(42 - title.size(), ' ');
                name = number + title;
                color = record.captures[1] ? 0xffffff80u : 0xffefefefu;
            } else {
                // 004ac99c: the original full-width question-mark name.
                name = number + std::string("\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48");
            }
            const i32 icon = rank == 5 ? (stage == 7 ? 6 : 5) : rank;
            if (icon < 7 && icon >= 0 && !ranks_[size_t(icon)] &&
                !create(ranks_[size_t(icon)], 229 + icon)) return false;
        }
        auto* vm = animations_.registry.find(names_[size_t(row)]);
        if (!vm || !rasterize || !rasterize(*vm, color, name))
            return fail("Spell Practice name rasterization failed");
        if (!interrupt(names_[size_t(row)], 3)) return false;
        if (rank >= 0 && !interrupt(ranks_[size_t(rank == 5 ? (stage == 7 ? 6 : 5) : rank)],
                                     3, true)) return false;
    }
    return true;
}

bool TitleSpellPracticeView::update(const TitleSpellPracticeMenu& menu,
                                   const TitleSpellPracticePlatform& platform,
                                   const Rasterizer& rasterize) {
    if (!error.empty()) return false;
    const auto level = menu.level();
    if (!opened_) {
        // 004560fe: ascii.anm script19 is a separate, persistent particle root.
        particles_ = animations_.create(2, 19, -1, 0);
        if (!particles_) return fail("Spell Practice particle animation unavailable");
        owned_.push_back(particles_);
        if (!create(character_, 240) || !create(stage_, 187) || !create(badge_, 109) ||
            !select(stage_, menu.cursor()) || !select(character_, menu.character())) return false;
        level_ = SpellPracticeLevel::Stage;
        opened_ = true;
    }
    if (level != level_) {
        if (level == SpellPracticeLevel::Stage) {
            // 00456650 case4（0x45697e-0x456a3b，age≥6）：names(+0x694×5)
            // 发 label1（句柄不清）；rank 族 7 柄（+0x65c×7）不发 label，
            // 经 0046fb90 取 VM 后 render_flags|=0x20 连同子体整树硬回收
            //（0x4569b6-0x4569f6，= AnmManager::retire），图标当帧离屏而不播
            // 脚本的 label1 淡出；103/188/187 各发 label1 并清句柄
            //（0x456a04/0x456a16/0x456a28）。0x4560a0 case0 以 0046fb90
            // 判 187 句柄失效后整表重建，case1 再发 label3+label(7+cursor)。
            // 对已 label6 锁定的旧 187 VM 重发 select 不会恢复列表——这正是
            // 「退回 STAGE SELECT 后列表只剩选中行」的成因。
            for (u32& handle : names_) { interrupt(handle, 1); handle = 0; }
            for (u32& handle : ranks_) animations_.retire(handle);
            interrupt(badge_, 1); badge_ = 0;
            interrupt(cards_, 1); cards_ = 0;
            interrupt(stage_, 1); stage_ = 0;
            // 0x4560a0 case0 重建 187 与 109 徽章；选中 label 由下方光标
            // 跟踪器（case1 时序）发给新 VM。
            if (!create(stage_, 187) || !create(badge_, 109)) return false;
        } else if (level == SpellPracticeLevel::Difficulty &&
                   level_ == SpellPracticeLevel::Stage) {
            // [H-SP2] 对局返回级联（0x4561bf→0x45673a）：0x11 重建 187 后
            // label3/label(7+stage)/label6 三连发全部 immediate（0x46fca0，
            // 0x456204-0x456238）；0x12 建 188 同构（0x456774-0x4567a2）。
            // 选中行必须先经 label(7+X) 的选中分支再被 label6 锁定——
            // 旧实现借 select() 把 7+X 挂 pending，被紧随的 label6 立即拍
            // 覆盖，选中行的 label7 状态从未生效。
            interrupt(badge_, 1);
            if (!create(badge_, 103) || !create(cards_, 188)) return false;
            const i32 count = platform.spell_count(menu.selected_stage());
            for (i32 absent = 0; absent < 13 - count; ++absent)
                if (!interrupt(cards_, 39 - absent, true)) return false;
            if (!interrupt(stage_, 3, true) ||
                !interrupt(stage_, 7 + (menu.selected_stage() - 1), true) ||
                !interrupt(stage_, 6, true) ||
                !interrupt(cards_, 3, true) ||
                !interrupt(cards_, 7 + menu.selected_slot(), true) ||
                !interrupt(cards_, 6, true)) return false;
        } else if (level == SpellPracticeLevel::Spell) {
            if (level_ == SpellPracticeLevel::Difficulty) {
                // 0x4576c0 case4（0x457941-0x457977，age≥6）：难度层取消对
                // 188 发 label1 并清零句柄（0x457960），names/ranks/103 不动；
                // 重入 0x12 由 0x456650 case0 以 0046fb90 判 [esi+0x5b8] 失效
                //（0x4566ca-0x4566e6）经 0044bb80 整表重建。不重建则级联
                // label6 锁定过的旧 188 只剩选中行（a 层行已随 label6 退场）。
                interrupt(cards_, 1);
                cards_ = 0;
            }
            if (cards_ == 0) {
                // 0x456650 case0 重建段（0x4566ab-0x456738）：103、188 各自
                // 判句柄失效才重建，随后对缺席槽发 label(39-i)；Stage 确认
                // 来路先收 109（0x4563f3 label1、0x456409 清柄）。
                if (level_ == SpellPracticeLevel::Stage) {
                    interrupt(badge_, 1);
                    badge_ = 0;
                }
                if (badge_ == 0 && !create(badge_, 103)) return false;
                if (!create(cards_, 188)) return false;
                // 00456650 disables nonexistent stage slots through labels39.. .
                const i32 count = platform.spell_count(menu.selected_stage());
                for (i32 absent = 0; absent < 13 - count; ++absent)
                    if (!interrupt(cards_, 39 - absent, true)) return false;
            }
            if (!select(cards_, menu.cursor())) return false;
        }
        level_ = level;
        cursor_ = -1;
    }
    const bool character_changed = menu.character() != character_index_;
    if (character_changed) {
        if (!select(character_, menu.character())) return false;
        character_index_ = menu.character();
    }
    if (level != SpellPracticeLevel::Stage &&
        (selected_stage_ != menu.selected_stage() ||
         slot_ != (level == SpellPracticeLevel::Spell ? menu.cursor() : menu.selected_slot()) ||
         character_changed)) {
        // Spell-level cursor selects the family, Difficulty-level cursor its row.
        const i32 family = level == SpellPracticeLevel::Spell ? menu.cursor() : menu.selected_slot();
        if (!rebuild_names(menu.selected_stage(), family, menu.character(), platform,
                           rasterize)) return false;
        selected_stage_ = menu.selected_stage();
        slot_ = family;
    }
    if (cursor_ != menu.cursor()) {
        if (level == SpellPracticeLevel::Stage) {
            if (!select(stage_, menu.cursor())) return false;
        } else if (level == SpellPracticeLevel::Spell) {
            if (!select(cards_, menu.cursor())) return false;
        } else {
            for (i32 row = 0; row < 5; ++row)
                if (!interrupt(names_[size_t(row)], row == menu.cursor() ? 2 : 3)) return false;
            for (i32 icon = 0; icon < 7; ++icon) {
                const i32 row = menu.selected_stage() == 7 ? (icon == 4 ? 0 : 1)
                                                          : (icon == 5 ? 4 : icon);
                if (!interrupt(ranks_[size_t(icon)], row == menu.cursor() ? 2 : 3, true)) return false;
            }
        }
        cursor_ = menu.cursor();
    }
    return true;
}

bool TitleSpellPracticeView::draw(const TitleSpellPracticeMenu& menu,
                                 const TitleSpellPracticePlatform& platform,
                                 HudDrawServices& draw) {
    if (menu.level() == SpellPracticeLevel::Stage) return true;
    const i32 family = menu.level() == SpellPracticeLevel::Spell ? menu.cursor() : menu.selected_slot();
    // 004579fc sets AsciiInf+0x19178=2 (font), +0x19174=1 (shadow).
    // Alignment remains the default left value1; 00403da0 consumes these slots.
    HudTextStyle style; style.font = 2; style.alignment = 1; style.shadow = true;
    for (i32 row = 0; row < 5; ++row) {
        const i32 id = platform.spell_id(menu.selected_stage(), family, row);
        if (id < 0) continue;
        SpellPracticeRecord record;
        if (!platform.spell_record(menu.character(), id, record))
            return fail("Spell Practice record unavailable");
        const bool selected = menu.level() == SpellPracticeLevel::Difficulty && row == menu.cursor();
        style.color = selected ? (record.captures[1] ? 0xff90d0ff : 0xffb0b0b0)
                               : (record.captures[1] ? 0xff60a0c0 : 0xff404040);
        char text[80];
        if (!record.encountered) std::snprintf(text, sizeof text, "SCORE        00  ----/----");
        else std::snprintf(text, sizeof text, "SCORE %8d0  %4d/%4d", record.best_score,
                            record.captures[1], record.attempts[1]);
        // 004579a0 constants at 004aeb1c/18/08/10: x330,y191,44px,10px.
        const float y = 191.0f + row * 44.0f;
        if (!draw.hud_text({{330, y, 0}, style, text})) return fail("Spell Practice score text failed");
        const i32 rank = platform.spell_rank(menu.selected_stage(), family, row);
        if (record.encountered && rank < 5) {
            style.color = record.captures[0] ? 0xff206060 : 0xff404040;
            std::snprintf(text, sizeof text, "GAME MODE        %4d/%4d", record.captures[0], record.attempts[0]);
            if (!draw.hud_text({{330, y + 10, 0}, style, text})) return fail("Spell Practice game text failed");
        }
    }
    return true;
}

} // namespace th13
