// SpellPresentation 实现：0x413270/0x413a60 的行为移植（证据行号见头注释）。
#include "SpellPresentation.hpp"
#include "AnmText.hpp"

#include <algorithm>
#include <cstdio>
#include <cmath>

namespace th13 {

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
                                const std::function<bool(i32)>& sound) {
    if (!event.emitted()) return true;
    if (!error.empty()) return false;
    if (event.kind == SpellEventKind::End) {
        sync(event.state);
        // 00413a60 uses 0046fc30 with EBX=1, which schedules label1 rather
        // than deleting these roots. Their real scripts animate the exit.
        for (u32& handle : displays_) {
            if (handle && !animations_.interrupt(handle, 1))
                return fail("Spell display exit interrupt failed: " + animations_.error);
            if (handle) retiring_displays_.push_back(handle);
            handle = 0;
        }
        if ((background_ && !animations_.retire(background_)) ||
            (stage_background_ && !animations_.retire(stage_background_)))
            return fail("Spell background retire failed: " + animations_.error);
        // The original does not retain either transient effect20 or the stage
        // decoration handle. Their scripts terminate them independently.
        decoration_ = 0;
        if (event.captured() && sound && !sound(0x2e))
            return fail("Spell card capture sound service failed");
        // 00413a60 00413ba9: timeout sound is outside the capture branch.
        if ((event.state.flags & kSpellTimedOut) && sound && !sound(0x45))
            return fail("Spell card timeout sound service failed");
        flags_ &= ~(kSpellActive | kSpellBombProtection);
        snapshot_.flags = flags_;
        return true;
    }
    if (event.kind != SpellEventKind::Begin) return fail("Unknown spell presentation event");
    // 4525b0 selects4bb4f0+stage*7c. Stages1/2/4/6 have no alternate
    // background row (+34=-1), so +3c/+40 and +4c/+50 apply directly.
    i32 background_script = -1, decoration_script = -1;
    switch (banks_.stage_number) {
    case 1: background_script=11;decoration_script=18;break;
    case 2: case 4: background_script=12;decoration_script=19;break;
    case 6: background_script=20;decoration_script=27;break;
    case 3: background_script=12;decoration_script=19;break;
    case 5: background_script=13;decoration_script=19;break;
    case 7: background_script=24;decoration_script=30;break;
    default: background_script=11;decoration_script=18;break;
    }
    if (background_script < 0)
        return fail("Spell presentation stage background mapping unavailable");
    if (!boss_position) return fail("Spell presentation boss position unavailable");
    // Previously ended display roots may still be animating out. Preserve
    // them across a new spell; only scene unload retires the saved handles.
    if (active() && !retire_production()) return false;
    production_ = true;
    sync(event.state);
    for (i32 i = 0; i < 3; ++i) anchor_[i] = boss_position[i];

    // 00413270 00413404/0041349b/00413538: these are independent roots,
    // created in this order, not three children of the stage background.
    displays_[0] = animations_.create(banks_.ascii, 0, -1, 0);
    displays_[1] = animations_.create(banks_.text, 74, -1, 0);
    displays_[2] = animations_.create(banks_.ascii, 1, -1, 0);
    if (!displays_[0] || !displays_[1] || !displays_[2]) {
        retire_production();
        return fail("Spell display animation unavailable: " + animations_.error);
    }
    if (sound && !sound(0x21)) return fail("Spell card start sound service failed");
    // 0041360a: effect.anm script13. Its direct children 11/12 consume
    // duration through integer work variable2 (+0x4b4), in ECL frame units.
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
    // 004137b8: independent, self-terminating effect20; it is not retired by
    // spellEnd, which does not retain its handle in the original object.
    if (!animations_.create(banks_.effects, 20, -1, 0)) {
        retire_production();
        return fail("Spell start effect animation unavailable: " + animations_.error);
    }
    // The merged ANIM bank appends file3 after file2. Locate its actual
    // script boundary, preserving the archive's source IDs and order.
    auto* resource = animations_.resource(banks_.stage);
    i32 offset = banks_.stage_number == 1 ? 139 : -1;
    if (resource) for (size_t i=1;i<resource->scripts.size();++i)
        if (resource->scripts[i].source_id < resource->scripts[i-1].source_id) {
            offset=i32(i);break;
        }
    if (offset < 0) return fail("Spell stage ANIM file3 boundary unavailable");
    stage_background_ = animations_.create(banks_.stage, offset + background_script, -1, 0);
    decoration_ = animations_.create(banks_.stage, offset + decoration_script, -1, 0);
    if (!stage_background_ || !decoration_) {
        retire_production();
        return fail("Stage spell background animation unavailable: " + animations_.error);
    }
    return true;
}

void SpellPresentation::sync(const SpellSnapshot& state, const float* boss_position) {
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
        if (!animations_.retire(background_))
            fail("Spell hidden background retire failed: " + animations_.error);
    }
    if (boss_position && state.active()) {
        for (i32 i = 0; i < 3; ++i)
            anchor_[i] = anchor_[i] + (boss_position[i] - anchor_[i]) * 0.05f;
        if (auto* vm = animations_.registry.find(background_))
            vm->visual.translation = {anchor_[0], anchor_[1], anchor_[2]};
    }
}

bool SpellPresentation::draw(HudDrawServices& services, i32 captures, i32 attempts) {
    if (!error.empty()) return false;
    if (!snapshot_.active()) return true;
    const auto* vm = animations_.registry.find(displays_[2]);
    if (!vm) return true; // 00413080 missing +0x1c VM skips this numeric pass
    HudTextStyle style;
    style.font = 2;
    // 00413080 uses font2/queue1;00427910 restores left/top alignment.
    // These are intermediate-RT positions, not final-screen coordinates:
    // queue1 draws at priority51, then text.anm84 at53 maps x128..512
    // to x32..416. Keep that composition path rather than subtracting96 here.
    style.alignment = 1;
    style.coordinate_space = 1;
    style.color = (vm->visual.color & 0xff000000u) | 0x00ffffffu;
    char text[32];
    float x = 371.0f;
    if (snapshot_.captured()) {
        x = 357.0f;
        std::snprintf(text, sizeof text, "%8d", snapshot_.current_bonus);
    } else {
        std::snprintf(text, sizeof text, "$");
    }
    if (!services.hud_text({{x, 35.0f, 0}, style, text}))
        return fail("Spell bonus text service failed");
    if (captures >= 100) {
        std::snprintf(text, sizeof text, "MASTER");
    } else if (attempts >= 100) {
        std::snprintf(text, sizeof text, "%.2d/99+", captures);
    } else {
        std::snprintf(text, sizeof text, "%.2d/%.2d", captures, attempts);
    }
    if (!services.hud_text({{452.0f, 35.0f, 0}, style, text}))
        return fail("Spell capture counter text service failed");
    return true;
}

bool SpellPresentation::rasterize_name(DialogueTextureSink& sink,
                                      const NameRasterizer& rasterize) {
    if (!error.empty()) return false;
    auto* vm = animations_.registry.find(displays_[1]);
    if (!production_ || !vm) return fail("Spell name display sprite unavailable");
    // 004135c3 calls the right-aligned writer0046e970: sprite[+38]
    // minus (nominal_height-1)*CP932_byte_length/2. 0046e230 scales the
    // sprite bounds by actual/header texture size, and0046e770 divides
    // their difference by the same ratio: +38 is the logical sprite width.
    // The half-width/quarter-length formula belongs to0046ea30 (center).
    return rasterize_anm_text(*vm, sink, rasterize, 0xffffffu, snapshot_.name,
                              AnmTextAlignment::Right, 17, &error);
}
bool SpellPresentation::begin(i32 spell_id, const std::string& name, i32 duration_frames,
                              i32 stage_number, i32 difficulty, const float boss_position[3],
                              const std::function<bool(i32)>& sound) {
    if (active()) {
        // Historical diagnostic adapter contract; SpellState::begin follows
        // the original's replacement semantics on the production path.
        error = "Spell card started while previous card active";
        return false;
    }
    // Legacy local diagnostic adapter. Production uses consume/sync above.
    production_ = false;
    // 0x413270:34/42：名字、符卡号、活动位 + 取得位置位。
    spell_id_ = spell_id;
    name_ = name;
    flags_ = 0x3u;  // bit0 | bit1（旧值无关：上一卡已清）
    // 0x413270:224-229：bonus 基值 = (stage + difficulty) * 1000000，
    // 显示值封顶 99999999（DAT_004be81c + DAT_004be7c4 同式）。
    bonus_base_ = (stage_number + difficulty) * 1000000;
    bonus_cap_ = std::min(bonus_base_, 99999999);
    duration_ = duration_frames;  // +0x88, ECL frame units
    for (i32 i = 0; i < 3; i++) anchor_[i] = boss_position[i];
    // 0x413270:158-186：背景枚 VM（合并 stage bank 的 cardbg 族），
    // 位置 = boss 快照位（+0x574.. 直写）。子 VM 时长变量由脚本族内
    // ins_500 派生，不在此旁路。
    background_ = animations_.create(banks_.stage, kStage1CardBgScript, -1, 0);
    if (!background_) {
        error = "Spell card background animation unavailable: " + animations_.error;
        return false;
    }
    if (auto* vm = animations_.registry.find(background_)) {
        vm->visual.render_flags &= ~0xc000u;
        vm->visual.translation = {anchor_[0], anchor_[1], anchor_[2]};
    }
    // 0x413270:157：开场声 0x21。
    if (sound && !sound(0x21)) {
        error = "Spell card start sound service failed";
        return false;
    }
    return true;
}

bool SpellPresentation::end(bool captured, bool last_spell,
                            const std::function<bool(i32)>& sound,
                            const std::function<void(i32)>& add_score) {
    if (!active()) return true;  // 无卡：原版 0x413a60 头门（+0x7c bit0）
    // 0x413a60:15-27：展示/背景 VM 回收 + 活动位清。
    if (background_ && !animations_.retire(background_))
        error = "Spell card background retire failed";
    background_ = 0;
    const bool award = captured && (flags_ & 0x2u) != 0;  // bit1 未破坏（0x413a60:33）
    if (award) {
        // 0x413a60:34-36：score += bonus/10（封顶由宿主分数字段承担）。
        if (add_score) add_score(bonus_base_ / 10);
        // 0x413a60:60：取得声 0x2e。
        if (sound && !sound(0x2e)) error = "Spell card capture sound service failed";
        // 0x413a60:64-66：最终符（bit0x80）追加声 0x45。
        if (last_spell && sound && !sound(0x45))
            error = "Spell card last-spell sound service failed";
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
    if (background_ && !animations_.retire(background_))
        error = "Spell card background retire failed";
    background_ = 0;
    flags_ = 0;
}

}  // namespace th13
