#include "SpellPresentation.hpp"
#include "AnmText.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>

namespace th13 {

const SpellPresentation::CardBgRow SpellPresentation::kCardBgRows[7] = {
    {-1, -1, 3, 11, -1, -1, 3, 18}, {-1, -1, 3, 12, -1, -1, 3, 19}, {4, 12, 3, 12, 4, 19, 3, 19},
    {-1, -1, 3, 12, -1, -1, 3, 19}, {3, 13, 3, 13, 4, 9, 3, 19},    {-1, -1, 3, 20, -1, -1, 3, 27},
    {4, 14, 3, 24, -1, -1, 3, 30},
};

bool SpellPresentation::fail(const std::string& reason) {
    if (error.empty()) error = reason;
    return false;
}

bool SpellPresentation::retire_production() {
    bool ok = true;
    for (u32& handle : displays_)
        if (handle && !animations_.retire(handle)) ok = false;
    for (u32* handle : {&background_, &stage_background_, &decoration_})
        if (*handle && !animations_.retire(*handle)) ok = false;
    for (u32& handle : retiring_displays_)
        if (handle && !animations_.retire(handle)) ok = false;
    retiring_displays_.clear();
    return ok || fail("Spell animation retire failed: " + animations_.error);
}

bool SpellPresentation::consume(const SpellEvent& event, const float boss_position[3],
                                const std::function<bool(i32)>& sound, i32 chapter) {
    if (!event.emitted()) return true;
    if (!error.empty()) return false;
    if (event.kind == SpellEventKind::End) {
        capture_frames_ = event.state.active_frames;
        sync(event.state);
        for (u32& handle : displays_) {
            if (handle && !animations_.interrupt(handle, 1))
                return fail("Spell display exit interrupt failed: " + animations_.error);
            if (handle) retiring_displays_.push_back(handle);
            handle = 0;
        }
        if ((background_ && !animations_.retire(background_)) ||
            (stage_background_ && !animations_.retire(stage_background_)))
            return fail("Spell background retire failed: " + animations_.error);
        decoration_ = 0;
        if (event.captured() && sound && !sound(0x2e)) return fail("Spell card capture sound service failed");
        if ((event.state.flags & kSpellTimedOut) && sound && !sound(0x45))
            return fail("Spell card timeout sound service failed");
        if (animations_.resource(banks_.front)) {
            if (banner_ && !animations_.retire(banner_))
                return fail("Spell banner retire failed: " + animations_.error);
            banner_ = animations_.create(banks_.front, event.captured() ? 59 : 60, -1, 0);
            if (!banner_) return fail("Spell bonus banner animation unavailable: " + animations_.error);
        }
        if (event.captured() && animations_.resource(banks_.ascii) && animations_.resource(banks_.front)) {
            for (u32& handle : bonus_digits_)
                if (handle && !animations_.retire(handle))
                    return fail("Spell bonus digit retire failed: " + animations_.error);
            if ((bonus_million_ && !animations_.retire(bonus_million_)) ||
                (bonus_thousand_ && !animations_.retire(bonus_thousand_)))
                return fail("Spell bonus extra retire failed: " + animations_.error);
            for (u32& handle : bonus_digits_)
                handle = 0;
            bonus_million_ = bonus_thousand_ = 0;
            i32 divisor = 10000000, value = snapshot_.current_bonus;
            bool leading = true;
            for (i32 i = 0; i < 8; ++i) {
                const i32 digit = value / divisor;
                value %= divisor;
                if (digit != 0) leading = false;
                bonus_digits_[i] = animations_.create(banks_.ascii, 4 + i, -1, 0);
                AnmVm* vm = animations_.registry.find(bonus_digits_[i]);
                if (!vm) return fail("Spell bonus digit animation unavailable");
                if (!vm->select_sprite(270 + digit)) return fail("Spell bonus digit sprite unavailable");
                if (leading) vm->visual.flags &= ~2u;
                divisor /= 10;
            }
            if (snapshot_.current_bonus >= 1000000) {
                bonus_million_ = animations_.create(banks_.ascii, 12, -1, 0);
                if (auto* vm = animations_.registry.find(bonus_million_)) {
                    if (!vm->select_sprite(284)) return fail("Spell bonus separator sprite unavailable");
                } else
                    return fail("Spell bonus separator animation unavailable");
            }
            if (snapshot_.current_bonus >= 1000) {
                bonus_thousand_ = animations_.create(banks_.ascii, 13, -1, 0);
                if (auto* vm = animations_.registry.find(bonus_thousand_)) {
                    if (!vm->select_sprite(284)) return fail("Spell bonus separator sprite unavailable");
                } else
                    return fail("Spell bonus separator animation unavailable");
            }
        }
        if (animations_.resource(banks_.front)) {
            bonus_capture_effect_ = animations_.create(banks_.front, 95, -1, 0);
            if (!bonus_capture_effect_) return fail("Spell capture effect animation unavailable: " + animations_.error);
        }
        flags_ &= ~(kSpellActive | kSpellBombProtection);
        snapshot_.flags = flags_;
        return true;
    }
    if (event.kind != SpellEventKind::Begin) return fail("Unknown spell presentation event");
    if (banks_.stage_number < 1 || banks_.stage_number > 7) return fail("Spell presentation stage row out of range");
    const CardBgRow& row = kCardBgRows[banks_.stage_number - 1];
    const bool use_a = chapter < 6 && row.a_bank != -1;
    const i32 cardbg_bank = use_a ? row.a_bank : row.b_bank;
    const i32 cardbg_script = use_a ? row.a_script : row.b_script;
    const i32 decor_bank = use_a ? row.decor_a_bank : row.decor_b_bank;
    const i32 decor_script = use_a ? row.decor_a_script : row.decor_b_script;
    if (!boss_position) return fail("Spell presentation boss position unavailable");
    if (active() && !retire_production()) return false;
    production_ = true;
    sync(event.state);
    for (i32 i = 0; i < 3; ++i)
        anchor_[i] = boss_position[i];

    displays_[0] = animations_.create(banks_.ascii, 0, -1, 0);
    displays_[1] = animations_.create(banks_.text, 74, -1, 0);
    displays_[2] = animations_.create(banks_.ascii, 1, -1, 0);
    if (!displays_[0] || !displays_[1] || !displays_[2]) {
        retire_production();
        return fail("Spell display animation unavailable: " + animations_.error);
    }
    if (auto* vm = animations_.registry.find(displays_[2])) vm->visual.render_flags |= 0x40000u;
    if (sound && !sound(0x21)) return fail("Spell card start sound service failed");
    background_ = animations_.create(banks_.effects, 13, -1, 0);
    auto* background = animations_.registry.find(background_);
    auto* first = background ? animations_.registry.find_child_script(*background, 11, 0) : nullptr;
    auto* second = background ? animations_.registry.find_child_script(*background, 12, 0) : nullptr;
    if (!background || !first || !second) {
        retire_production();
        return fail("Spell effect background or duration children unavailable");
    }
    background->visual.translation = {anchor_[0], anchor_[1], anchor_[2]};
    first->variables.integers[2] = event.state.duration_frames;
    second->variables.integers[2] = event.state.duration_frames;
    if (!animations_.create(banks_.effects, 20, -1, 0)) {
        retire_production();
        return fail("Spell start effect animation unavailable: " + animations_.error);
    }
    auto* resource = animations_.resource(banks_.stage);
    std::vector<i32> file_bases;
    if (resource) {
        file_bases.push_back(0);
        for (size_t i = 1; i < resource->scripts.size(); ++i)
            if (resource->scripts[i].source_id < resource->scripts[i - 1].source_id) file_bases.push_back(i32(i));
    }
    const auto global_script = [&](i32 bank, i32 script) -> i32 {
        const size_t file = size_t(bank - 2);
        if (file >= file_bases.size()) {
            fail("Spell stage ANM file slot unavailable: bank " + std::to_string(bank));
            return -1;
        }
        return file_bases[file] + script;
    };
    const i32 background_global = global_script(cardbg_bank, cardbg_script);
    if (background_global < 0) {
        retire_production();
        return false;
    }
    stage_background_ = animations_.create(banks_.stage, background_global, -1, 0);
    if (decor_bank != -1) {
        const i32 decoration_global = global_script(decor_bank, decor_script);
        if (decoration_global < 0) {
            retire_production();
            return false;
        }
        decoration_ = animations_.create(banks_.stage, decoration_global, -1, 0);
    } else {
        decoration_ = 0;
    }
    if (!stage_background_ || (decor_bank != -1 && !decoration_)) {
        retire_production();
        return fail("Stage spell background animation unavailable: " + animations_.error);
    }
    return true;
}

bool SpellPresentation::show_notice(i32 front_script) {
    if (front_script != 61 && front_script != 63) return true;
    if (animations_.resource(banks_.front)) {
        if (notice_) animations_.retire(notice_);
        notice_ = animations_.create(banks_.front, front_script, -1, 0);
    } else {
        notice_ = 0;
    }
    return true;
}

bool SpellPresentation::show_spirit_notice(i32 level, float x, float y, float z) {
    if (level < 1 || level > 3) return true;
    const i32 script = level + 0x6b;
    spirit_notices_.erase(std::remove_if(spirit_notices_.begin(), spirit_notices_.end(),
                                         [this](u32 handle) { return !animations_.registry.find(handle); }),
                          spirit_notices_.end());
    if (!animations_.resource(banks_.front)) return true;
    const u32 handle = animations_.create(banks_.front, script, -1, 0, Vec3{x, y - 32.f, z});
    if (!handle) return true;
    spirit_notices_.push_back(handle);
    spirit_notice_ = handle;
    return true;
}

void SpellPresentation::sync(const SpellSnapshot& state) {
    const u32 old_flags = flags_;
    snapshot_ = state;
    flags_ = state.flags;
    spell_id_ = state.id;
    name_ = state.name;
    bonus_base_ = state.current_bonus;
    bonus_cap_ = state.initial_bonus;
    duration_ = state.duration_frames;
    if (!production_) return;
    if (state.active() && ((old_flags ^ state.flags) & kSpellDisplayHidden)) {
        const i32 label = state.flags & kSpellDisplayHidden ? 3 : 2;
        for (u32 handle : displays_)
            if (handle && !animations_.interrupt(handle, label))
                fail("Spell display visibility interrupt failed: " + animations_.error);
    }
    if ((state.flags & kSpellHiddenBackground) && background_) {
        if (!animations_.retire(background_)) fail("Spell hidden background retire failed: " + animations_.error);
    }
}

void SpellPresentation::update_background(const float boss_position[3]) {
    if (!production_ || !active() || !error.empty() || !boss_position) return;
    for (i32 i = 0; i < 3; ++i) {
        const float delta = float(boss_position[i] - anchor_[i]);
        const float movement = float(double(delta) * double(0.05f));
        anchor_[i] = float(anchor_[i] + movement);
    }
    if (auto* vm = animations_.registry.find(background_))
        vm->visual.translation = {anchor_[0], anchor_[1], anchor_[2]};
}

bool SpellPresentation::draw(HudDrawServices& services, i32 captures, i32 attempts) {
    if (!error.empty()) return false;
    if (!snapshot_.active()) return true;
    const auto* vm = animations_.registry.find(displays_[2]);
    if (!vm) return true;
    HudTextStyle style;
    style.font = 2;
    style.coordinate_space = 1;
    style.alignment = 1;
    style.color = (vm->visual.color & 0xff000000u) | 0x00ffffffu;
    char text[32];
    float x = 371.0f;
    if (snapshot_.captured()) {
        x = 357.0f;
        std::snprintf(text, sizeof text, "%8d", snapshot_.current_bonus);
    } else {
        std::snprintf(text, sizeof text, "$");
    }
    if (!services.hud_text({{x, 35.0f, 0}, style, text})) return fail("Spell bonus text service failed");
    if (captures >= 100) {
        std::snprintf(text, sizeof text, "MASTER");
    } else if (attempts >= 100) {
        std::snprintf(text, sizeof text, "%.2d/99+", captures);
    } else {
        std::snprintf(text, sizeof text, "%.2d/%.2d", captures, attempts);
    }
    if (!services.hud_text({{452.0f, 35.0f, 0}, style, text})) return fail("Spell capture counter text service failed");
    return true;
}

bool SpellPresentation::draw_capture_times(HudDrawServices& services, i32 packed_time) {
    if (!error.empty()) return false;
    if (!bonus_capture_effect_) return true;
    const auto* effect = animations_.registry.find(bonus_capture_effect_);
    if (!effect) {
        bonus_capture_effect_ = 0;
        return true;
    }
    const u32 vm_alpha = effect->visual.color & 0xff000000u;
    HudTextStyle style;
    style.font = 4;
    style.coordinate_space = 1;
    style.alignment = style.vertical_alignment = 1;
    char text[32];
    {
        const i32 frames = capture_frames_;
        i32 seconds = frames / 60;
        const i32 hundredths = (frames - seconds * 60) * 100 / 60;
        if (seconds > 999) seconds = 999;
        style.color = vm_alpha | 0x00ffffffu;
        style.scale = {1.0f, 1.0f};
        std::snprintf(text, sizeof text, "%3d.", seconds);
        if (!services.hud_text({{318.0f, 192.0f, 0}, style, text})) return fail("Spell capture time service failed");
        style.scale = {0.6f, 0.6f};
        std::snprintf(text, sizeof text, "%.2ds", hundredths);
        if (!services.hud_text({{362.0f, 198.0f, 0}, style, text})) return fail("Spell capture time service failed");
    }
    {
        i32 seconds = 999, hundredths = 99;
        const i32 v = packed_time;
        if (v / 100000 - 0x16 == ((v / 100) % 1000 + 0x3a6) % 1000 + (v % 100 + 0x43) % 100) {
            seconds = ((v / 100) % 1000 + 0x3a6) % 1000;
            hundredths = (v % 100 + 0x43) % 100;
        }
        style.color = vm_alpha | 0x00808080u;
        style.scale = {1.f, 1.f};
        std::snprintf(text, sizeof text, "%3d.", seconds);
        if (!services.hud_text({{318.0f, 208.0f, 0}, style, text})) return fail("Spell record time service failed");
        style.scale = {.6f, .6f};
        std::snprintf(text, sizeof text, "%.2ds", hundredths);
        if (!services.hud_text({{362.0f, 214.0f, 0}, style, text})) return fail("Spell record time service failed");
    }
    return true;
}

bool SpellPresentation::rasterize_name(DialogueTextureSink& sink, const NameRasterizer& rasterize) {
    if (!error.empty()) return false;
    auto* vm = animations_.registry.find(displays_[1]);
    if (!production_ || !vm) return fail("Spell name display sprite unavailable");
    return rasterize_anm_text(*vm, sink, rasterize, 0xffffffu, snapshot_.name, AnmTextAlignment::Right, 17, &error);
}
bool SpellPresentation::begin(i32 spell_id, const std::string& name, i32 duration_frames, i32 stage_number,
                              i32 difficulty, const float boss_position[3], const std::function<bool(i32)>& sound) {
    if (active()) {
        error = "Spell card started while previous card active";
        return false;
    }
    production_ = false;
    spell_id_ = spell_id;
    name_ = name;
    flags_ = 0x3u;
    bonus_base_ = (stage_number + difficulty) * 1000000;
    bonus_cap_ = std::min(bonus_base_, 99999999);
    duration_ = duration_frames;
    for (i32 i = 0; i < 3; i++)
        anchor_[i] = boss_position[i];
    background_ = animations_.create(banks_.stage, kStage1CardBgScript, -1, 0);
    if (!background_) {
        error = "Spell card background animation unavailable: " + animations_.error;
        return false;
    }
    if (auto* vm = animations_.registry.find(background_)) {
        vm->visual.render_flags &= ~0xc000u;
        vm->visual.translation = {anchor_[0], anchor_[1], anchor_[2]};
    }
    if (sound && !sound(0x21)) {
        error = "Spell card start sound service failed";
        return false;
    }
    return true;
}

bool SpellPresentation::end(bool captured, bool last_spell, const std::function<bool(i32)>& sound,
                            const std::function<void(i32)>& add_score) {
    if (!active()) return true;
    if (background_ && !animations_.retire(background_)) error = "Spell card background retire failed";
    background_ = 0;
    const bool award = captured && (flags_ & 0x2u) != 0;
    if (award) {
        if (add_score) add_score(bonus_base_ / 10);
        if (sound && !sound(0x2e)) error = "Spell card capture sound service failed";
        if (last_spell && sound && !sound(0x45)) error = "Spell card last-spell sound service failed";
    }
    flags_ = 0;
    spell_id_ = -1;
    name_.clear();
    bonus_base_ = bonus_cap_ = 0;
    duration_ = 0;
    return error.empty();
}

void SpellPresentation::release() {
    if (production_) {
        retire_production();
        snapshot_ = {};
    }
    if (banner_ && !animations_.retire(banner_)) error = "Spell banner retire failed";
    banner_ = 0;
    if (notice_ && !animations_.retire(notice_)) error = "Spell notice retire failed";
    notice_ = 0;
    for (u32 handle : spirit_notices_)
        if (handle && !animations_.retire(handle)) error = "Spirit notice retire failed";
    spirit_notices_.clear();
    spirit_notice_ = 0;
    for (u32& handle : bonus_digits_) {
        if (handle && !animations_.retire(handle)) error = "Spell bonus digit retire failed";
        handle = 0;
    }
    if (bonus_million_ && !animations_.retire(bonus_million_)) error = "Spell bonus separator retire failed";
    if (bonus_thousand_ && !animations_.retire(bonus_thousand_)) error = "Spell bonus separator retire failed";
    if (bonus_capture_effect_ && !animations_.retire(bonus_capture_effect_))
        error = "Spell capture effect retire failed";
    bonus_million_ = bonus_thousand_ = bonus_capture_effect_ = 0;
    capture_frames_ = 0;
    if (background_ && !animations_.retire(background_)) error = "Spell card background retire failed";
    background_ = 0;
    flags_ = 0;
}

}
