// SpellPresentation 实现：0x413270/0x413a60 的行为移植（证据行号见头注释）。
#include "SpellPresentation.hpp"
#include "AnmText.hpp"

#include <algorithm>
#include <cstdio>
#include <cmath>

namespace th13 {

// 0x4bb4f0 + stage*0x7c 行 1..7（EXE .data 实读；A 行=章节<6 的中Boss族，
// 见 SpellPresentation.hpp 注释）。s5 A cardbg 与 B 同为 bank3/script13，
// 差异在装饰（A=st05menm script9）。
const SpellPresentation::CardBgRow SpellPresentation::kCardBgRows[7] = {
    /*stage1*/ {-1, -1, 3, 11, -1, -1, 3, 18},
    /*stage2*/ {-1, -1, 3, 12, -1, -1, 3, 19},
    /*stage3*/ {4, 12, 3, 12, 4, 19, 3, 19},
    /*stage4*/ {-1, -1, 3, 12, -1, -1, 3, 19},
    /*stage5*/ {3, 13, 3, 13, 4, 9, 3, 19},
    /*stage6*/ {-1, -1, 3, 20, -1, -1, 3, 27},
    /*stage7*/ {4, 14, 3, 24, -1, -1, 3, 30},
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
        // 00413a60 的结算分支：捕获 00413b03（xor eax,eax）→ 0042a430 case0，
        // 失败 00413b9a（ebx=1）→ case1。两个 case 都先从 stage+0x2a8（front
        // bank）经 00403800 拷贝模板：case0 脚本 0x3b=59（0042a47c），case1
        // 脚本 0x3c=60（0042a8b7），旧句柄 stage+0xcc 先经 00470110 回收
        // （0042a449/0042a884）。脚本自设位置 (320,128)/(320,144)、层19
        // （anm-list 7580-7720；sprite48/49 = front00.png）。
        if (animations_.resource(banks_.front)) {
            if (banner_ && !animations_.retire(banner_))
                return fail("Spell banner retire failed: " + animations_.error);
            banner_ = animations_.create(banks_.front, event.captured() ? 59 : 60, -1, 0);
            if (!banner_)
                return fail("Spell bonus banner animation unavailable: " + animations_.error);
        }
        // 0x42a4e2-0x42a876（case0 捕获分支）：结算数字与捕获特效。
        // 8 个数字 VM = ascii script 4+i（0x42a54b lea eax,[edi+4]），除数
        // 从 10^7（0x989680 @0x42a4ff）递除 10（0x42a5fd 乘法魔数），digit
        // = 当前值/除数（0x42a5b0 idiv），0x461a60 直选 sprite 270+digit
        //（0x42a5d6 lea ecx,[ebx+0x10e]）；前导零（[ebp-0xc]==0，商非零
        // 才置 1 @0x42a5bc）经 0x470030 递归清 bit1 隐藏（0x42a5ea）。
        // ≥10^6（0xf4240）建 script12 → stage+0xc4、≥10^3（0x3e8）建
        // script13 → stage+0xc8（0x42a66e/0x42a742 eax=0xc/0xd，创建后
        // 0x461a60 设 sprite 0x11c=284 分隔符）；尾建 front script95
        //（0x42a804 mov eax,0x5f）→ stage+0x118，stage+0x120=1。全部
        // VM 位置/anmArg 为 0、active 位 0x200、主链尾插（0x46f3f0）。
        // 显示值 = 结算瞬间的 bonus（snapshot_.current_bonus，sync 已取）。
        if (event.captured() && animations_.resource(banks_.ascii) &&
            animations_.resource(banks_.front)) {
            for (u32& handle : bonus_digits_)
                if (handle && !animations_.retire(handle))
                    return fail("Spell bonus digit retire failed: " + animations_.error);
            if ((bonus_million_ && !animations_.retire(bonus_million_)) ||
                (bonus_thousand_ && !animations_.retire(bonus_thousand_)) ||
                (bonus_capture_effect_ && !animations_.retire(bonus_capture_effect_)))
                return fail("Spell bonus extra retire failed: " + animations_.error);
            for (u32& handle : bonus_digits_) handle = 0;
            bonus_million_ = bonus_thousand_ = bonus_capture_effect_ = 0;
            i32 divisor = 10000000, value = snapshot_.current_bonus;
            bool leading = true;
            for (i32 i = 0; i < 8; ++i) {
                const i32 digit = value / divisor;
                value %= divisor;
                if (digit != 0) leading = false;
                bonus_digits_[i] = animations_.create(banks_.ascii, 4 + i, -1, 0);
                AnmVm* vm = animations_.registry.find(bonus_digits_[i]);
                if (!vm) return fail("Spell bonus digit animation unavailable");
                if (!vm->select_sprite(270 + digit))
                    return fail("Spell bonus digit sprite unavailable");
                if (leading) vm->visual.flags &= ~2u;  // 0x470030 清 bit1
                divisor /= 10;
            }
            if (snapshot_.current_bonus >= 1000000) {
                bonus_million_ = animations_.create(banks_.ascii, 12, -1, 0);
                if (auto* vm = animations_.registry.find(bonus_million_)) {
                    if (!vm->select_sprite(284))
                        return fail("Spell bonus separator sprite unavailable");
                } else
                    return fail("Spell bonus separator animation unavailable");
            }
            if (snapshot_.current_bonus >= 1000) {
                bonus_thousand_ = animations_.create(banks_.ascii, 13, -1, 0);
                if (auto* vm = animations_.registry.find(bonus_thousand_)) {
                    if (!vm->select_sprite(284))
                        return fail("Spell bonus separator sprite unavailable");
                } else
                    return fail("Spell bonus separator animation unavailable");
            }
            bonus_capture_effect_ = animations_.create(banks_.front, 95, -1, 0);
            if (!bonus_capture_effect_)
                return fail("Spell capture effect animation unavailable: " +
                            animations_.error);
        }
        flags_ &= ~(kSpellActive | kSpellBombProtection);
        snapshot_.flags = flags_;
        return true;
    }
    if (event.kind != SpellEventKind::Begin) return fail("Unknown spell presentation event");
    // 0x41381e-0x4139b6（capstone 直读）：章节 <6 且 A 行在 → A 对（cardbg
    // +0x34/+0x38、装饰 +0x44/+0x48）；否则 B 对（+0x3c/+0x40、+0x4c/+0x50）。
    // 装饰槽 -1 不建（0x4138df/0x4139a7 各自判 -1 跳过，EX A 行即如此）。
    // 旧实现只按舞台号硬编码 B 行：S3 中Boss 小傘（章节4）的宣言画成
    // st03enm 的 cdbg03 芳香背景——本表即修此身份错。
    if (banks_.stage_number < 1 || banks_.stage_number > 7)
        return fail("Spell presentation stage row out of range");
    const CardBgRow& row = kCardBgRows[banks_.stage_number - 1];
    const bool use_a = chapter < 6 && row.a_bank != -1;
    const i32 cardbg_bank = use_a ? row.a_bank : row.b_bank;
    const i32 cardbg_script = use_a ? row.a_script : row.b_script;
    const i32 decor_bank = use_a ? row.decor_a_bank : row.decor_b_bank;
    const i32 decor_script = use_a ? row.decor_a_script : row.decor_b_script;
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
    // 第三个根（ascii script1 = “Bonus/History” 标签）在原版带 +320/+16 的
    // 游戏坐标偏移：原生 draw 捕获 terra-reimu-s1 frame 6486 中 sprite299 的
    // quad 右缘 463.5、顶 34.5 = 脚本位 (144,19) + (320,16)；同帧 sprite298
    // 右缘 511.5 = 脚本 512、text74 右缘 523.5 = 脚本 524，二者均无偏移。
    // 0x466a80:32-36 对 VM+0x1c bit18 的根加 (+320,+16)（fadds 320.0/16.0）。
    if (auto* vm = animations_.registry.find(displays_[2]))
        vm->visual.render_flags |= 0x40000u;
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
    // The merged ANIM bank appends each ANIM-list file after the previous
    // one; a drop in source_id marks the next file's first script. Collect
    // every file base so bank slots 2..4 (0x4c2188+0x80 file table) resolve
    // to global script indices, preserving archive source IDs and order.
    auto* resource = animations_.resource(banks_.stage);
    std::vector<i32> file_bases;  // file_bases[n] = 名单第 n 项（槽 2+n）基号
    if (resource) {
        file_bases.push_back(0);
        for (size_t i = 1; i < resource->scripts.size(); ++i)
            if (resource->scripts[i].source_id < resource->scripts[i - 1].source_id)
                file_bases.push_back(i32(i));
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
        decoration_ = 0;  // 0x4138df/0x4139a7：装饰槽 -1 → 不建（EX A 行）
    }
    if (!stage_background_ || (decor_bank != -1 && !decoration_)) {
        retire_production();
        return fail("Stage spell background animation unavailable: " +
                    animations_.error);
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
    // 413080 sets the font (AsciiInf+0x19178=2) and leaves the alignment
    // fields at their default 1 (left/top, 0x42814F/0x428155), restored by
    // 427910 after HUD text. It also writes +0x1917c=1 (coordinate space 1),
    // but the native draw capture terra-reimu-s1 f6486 puts the bonus digits
    // at x=363.5/y=34.5, i.e. the screen-space pen (357,35) + glyph bearing:
    // space 1 must NOT add the port's (32,16) playfield offset here. Keep the
    // queued position in screen space (coordinate_space 0); adding (32,16)
    // shifted the bonus and counter 32px right / 16px down (bug: panel lost).
    style.alignment = 1;
    style.coordinate_space = 0;
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
    // 004135c3 calls 0046e970 (0x46e9c5-0x46ea03). Its x argument is
    // sprite_width - (17-1)*strlen/2 = 320 - 8*len in the 662-wide GDI
    // scratch space (0x46e810 -> 0x45aef0 param_4); D3DX squeezes that
    // scratch onto the 320x17 sprite, so the ink origin is
    // (320-8*len)*320/662. The port's centre form draws at offset*2 =
    // 320-8*len scratch columns with source_width 662 -> bit-identical to
    // 0046e970. Keep Center; BUG10 (if any) is in the VM screen placement /
    // scale / anchor, not this texture-local x formula.
    return rasterize_anm_text(*vm, sink, rasterize, 0xffffffu, snapshot_.name,
                              AnmTextAlignment::Center, 17, &error);
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
    if (banner_ && !animations_.retire(banner_))
        error = "Spell banner retire failed";
    banner_ = 0;
    // 0x42a430 case0 的数字/分隔符/特效 VM 随场景卸载回收
    //（RunScene.cpp:264 场景析构路径）。
    for (u32& handle : bonus_digits_) {
        if (handle && !animations_.retire(handle))
            error = "Spell bonus digit retire failed";
        handle = 0;
    }
    if (bonus_million_ && !animations_.retire(bonus_million_))
        error = "Spell bonus separator retire failed";
    if (bonus_thousand_ && !animations_.retire(bonus_thousand_))
        error = "Spell bonus separator retire failed";
    if (bonus_capture_effect_ && !animations_.retire(bonus_capture_effect_))
        error = "Spell capture effect retire failed";
    bonus_million_ = bonus_thousand_ = bonus_capture_effect_ = 0;
    if (background_ && !animations_.retire(background_))
        error = "Spell card background retire failed";
    background_ = 0;
    flags_ = 0;
}

}  // namespace th13
