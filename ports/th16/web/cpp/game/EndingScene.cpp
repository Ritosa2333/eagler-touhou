#include "EndingScene.hpp"

#include "AnmManager.hpp"
#include "ScoreFile.hpp"

#include <cstring>

namespace th16 {
namespace {

struct EndingIns {
    u16 time;
    u8 op;
    u8 size;
};

const EndingIns* read_ins(const u8* pc) { return reinterpret_cast<const EndingIns*>(pc); }

i32 arg_i32(const u8* pc, int i) {
    i32 v = 0;
    std::memcpy(&v, pc + 4 + i * 4, 4);
    return v;
}

}  // namespace

std::string EndingScene::decrypt_text(const u8* src) {
    // 与 Gui::decrypt_text（0x42bbe0）同口径
    std::string out;
    u8 key = 0x77, step = 7;
    for (;;) {
        const u8 c = u8(*src++ ^ key);
        key = u8(key + step);
        step = u8(step + 0x10);
        if (c == 0) break;
        out.push_back(char(c));
    }
    return out;
}

void EndingScene::advance_pc() {
    if (script_pc == nullptr) return;
    const EndingIns* ins = read_ins(script_pc);
    script_pc = script_pc + 4 + ins->size;
}

bool EndingScene::waiting_for_input(const InputState& input) const {
    // 0x4199f0 case 5/6：确认按下立即可过；否则需长按射击/跳过 ≥0x14 且外壳 flags bit0=0
    if ((input.pressed & InputState::kConfirmMask) != 0) return false;
    if ((flags & 1u) != 0) return true;
    if ((input.held & InputState::kSkip) != 0) return false;
    if ((input.held & InputState::kShot) != 0 && input.total_frames[0] >= 0x14) return false;
    return true;
}

void EndingScene::paint_line(u32 vm_id, const char* text, i32 font, u32 color) {
    if (animations == nullptr || vm_id == 0) return;
    AnmVm* vm = animations->find(vm_id);
    if (vm == nullptr) return;
    if (text_paint_vm(*animations, *vm, color, 0, font, 0, 0, text, text_paints)) {
        ++text_paint_count;
    }
}

bool EndingScene::switch_to_staff() {
    // 0x4199f0 case 0xc：清行 VM → 按难度装 staffN.msg → 复位剧本态（flags|=2）
    if (animations != nullptr) {
        for (u32& id : line_vms) {
            if (id != 0) animations->kill(id);
            id = 0;
        }
        for (u32& id : anm_vms) {
            if (id != 0) {
                if (kill_ending_anm) kill_ending_anm(id);
                else animations->kill(id);
            }
            id = 0;
        }
    }
    for (AnmFile*& f : ending_anms) f = nullptr;

    const i32 idx = (difficulty >= 1 && difficulty <= 3) ? difficulty : 0;
    // Easy(0)→staff1；Normal(1)→staff2；Hard(2)→staff3；Lunatic(3)→staff4
    const char* name = kStaffNames[idx];
    std::vector<u8> bytes;
    if (!read_archive || !read_archive(name, bytes) || bytes.empty()) {
        events.push_back("staff-missing");
        return false;
    }
    load_msg(bytes.data(), u32(bytes.size()), name);
    script_timer.reset();
    wait_timer.reset();
    script_flags = 2;  // 原版 memset 后 |2；bit2 装载中不置
    text_color = 0xffffff;
    lines_written = 0;
    events.push_back("staff");
    return true;
}

void EndingScene::begin(i32 shot, i32 shot_aux, i32 continues, ScoreFile* scores, i32 difficulty_in) {
    auto load_cb = std::move(load_ending_anm);
    auto spawn_cb = std::move(spawn_ending_anm);
    auto kill_cb = std::move(kill_ending_anm);
    auto read_cb = std::move(read_archive);
    auto fade_cb = std::move(create_fade);
    AnmManager* anm = animations;
    auto* paints = text_paints;
    *this = EndingScene{};
    load_ending_anm = std::move(load_cb);
    spawn_ending_anm = std::move(spawn_cb);
    kill_ending_anm = std::move(kill_cb);
    read_archive = std::move(read_cb);
    create_fade = std::move(fade_cb);
    animations = anm;
    text_paints = paints;
    this->scores = scores;
    live = true;
    difficulty = difficulty_in;
    const i32 combo = shot + shot_aux;
    ending_index = combo * 2 + (continues == 0 ? 0 : 1);
    if (ending_index < 0) ending_index = 0;
    if (ending_index > 7) ending_index = 7;

    if (scores != nullptr) {
        if (scores->stats.ending_seen[ending_index] == 0) flags |= 1u;
        if (!scores->stats.any_ending_seen) flags |= 2u;
        scores->stats.ending_seen[ending_index] = u8(scores->stats.ending_seen[ending_index] | 1);
        if (difficulty == 1) scores->stats.ending_seen[ending_index] = u8(scores->stats.ending_seen[ending_index] | 2);
        else if (difficulty == 2) scores->stats.ending_seen[ending_index] = u8(scores->stats.ending_seen[ending_index] | 4);
        else if (difficulty == 3) scores->stats.ending_seen[ending_index] = u8(scores->stats.ending_seen[ending_index] | 8);
        if (ending_index < 8) scores->stats.any_ending_seen = 1;
    }
    msg_name = kMsgNames[ending_index];
    events.push_back("begin");
}

bool EndingScene::load_msg(const u8* bytes, u32 size, const char* name) {
    if (bytes == nullptr || size == 0) {
        events.push_back("msg-missing");
        return false;
    }
    msg_bytes.assign(bytes, bytes + size);
    if (name != nullptr) msg_name = name;
    if (msg_bytes.size() >= 8) {
        u32 count = 0, offset = 0;
        std::memcpy(&count, msg_bytes.data(), 4);
        std::memcpy(&offset, msg_bytes.data() + 4, 4);
        if (count > 0 && offset < msg_bytes.size()) script_pc = msg_bytes.data() + offset;
        else script_pc = msg_bytes.data();
    } else {
        script_pc = msg_bytes.data();
    }
    script_timer.reset();
    wait_timer.reset();
    if ((script_flags & 2u) == 0) script_flags = 1;
    text_color = 0xffffff;
    lines_written = 0;
    events.push_back("msg-loaded");
    return true;
}

void EndingScene::spawn_line_vms(AnmManager& anm, AnmFile* text_anm) {
    animations = &anm;
    if (text_anm == nullptr) return;
    for (int i = 0; i < 5; ++i) {
        AnmVm* vm = nullptr;
        line_vms[i] = anm.spawn(*text_anm, 0x2e + i, -1, &vm);
        if (vm != nullptr) {
            // 0x4197c0 writes the two VM bytes without changing the script's drawing layer.
            vm->unknown_52c[0] = 0x10;
            vm->unknown_52c[1] = 0x10;
            vm->flags_hi &= ~0x1000u;
        }
    }
    if (ending_index >= 8) flags |= 2u;
    events.push_back("vms");
}

void EndingScene::destroy_line_vms(AnmManager& anm) {
    for (u32& id : line_vms) {
        if (id != 0) anm.kill(id);
        id = 0;
    }
    for (u32& id : anm_vms) {
        if (id != 0) {
            if (kill_ending_anm) kill_ending_anm(id);
            else anm.kill(id);
        }
        id = 0;
    }
}

EndingScene::ScriptResult EndingScene::step_script(const InputState& input) {
    // 0x4199f0：flags bit2 = 装载中则空转
    if ((script_flags & 4u) != 0) return ScriptResult::Running;
    if (script_pc == nullptr || msg_bytes.empty()) return ScriptResult::Finished;

    for (;;) {
        if (script_pc < msg_bytes.data() || script_pc + 4 > msg_bytes.data() + msg_bytes.size()) {
            return ScriptResult::Finished;
        }
        const EndingIns* ins = read_ins(script_pc);
        if (script_timer.current < i32(ins->time)) {
            script_timer.tick(1.0f);
            return ScriptResult::Running;
        }

        switch (ins->op) {
        case 0:
            return ScriptResult::Finished;

        case 3: {
            last_text = decrypt_text(script_pc + 4);
            events.push_back("text");
            if (lines_written == 0) {
                // 0x419a60 retires the previous page's line animations before painting the first new line.
                for (u32 id : line_vms) {
                    paint_line(id, " ", 0, 0xffffff);
                    if (animations != nullptr && id != 0) animations->interrupt(id, 3);
                }
                paint_line(line_vms[0], last_text.c_str(), 0, text_color);
                if (animations != nullptr && line_vms[0] != 0) animations->interrupt(line_vms[0], 2);
                ++lines_written;
                advance_pc();
                continue;
            }
            {
                const i32 slot = lines_written;
                if (slot >= 0 && slot < 5) {
                    paint_line(line_vms[slot], last_text.c_str(), 0, text_color);
                    if (animations != nullptr && line_vms[slot] != 0) animations->interrupt(line_vms[slot], 2);
                }
                ++lines_written;
                if (lines_written > 4) lines_written = 0;
            }
            advance_pc();
            continue;
        }

        case 4:
            if (animations != nullptr) {
                for (u32 id : line_vms) {
                    if (id != 0) animations->interrupt(id, 3);
                }
            }
            lines_written = 0;
            advance_pc();
            continue;

        case 5:
        case 6: {
            const i32 wait_arg = ins->size >= 4 ? arg_i32(script_pc, 0) : 0;
            if (wait_timer.current < 1) wait_timer.set(wait_arg);
            wait_timer.untick(1.0f);
            if (wait_arg < 0) wait_timer.set(999);

            if (waiting_for_input(input) && wait_timer.current > 0) {
                const bool can_ff = (flags & 1u) == 0 &&
                    (((input.held & InputState::kSkip) != 0) ||
                     ((input.held & InputState::kShot) != 0 && input.total_frames[0] >= 0x14));
                if (!can_ff || (wait_timer.current % 6) != 0) {
                    return ScriptResult::Running;
                }
            }
            wait_timer.set(0);
            if (ins->op == 6) lines_written = 0;
            events.push_back("wait");
            advance_pc();
            continue;
        }

        case 7: {
            const i32 slot_arg = ins->size >= 4 ? arg_i32(script_pc, 0) : 0;
            pending_anm_slot = slot_arg + 0x14;
            pending_anm_name.clear();
            if (ins->size > 4) pending_anm_name.assign(reinterpret_cast<const char*>(script_pc + 8));
            script_flags |= 4u;
            AnmFile* file = nullptr;
            if (load_ending_anm && !pending_anm_name.empty()) {
                file = load_ending_anm(pending_anm_slot, pending_anm_name.c_str());
                events.push_back(file != nullptr ? "anm-loaded" : "anm-missing");
            }
            if (slot_arg >= 0 && slot_arg < 4) ending_anms[slot_arg] = file;
            advance_pc();
            return ScriptResult::Running;
        }

        case 8: {
            const i32 vm_slot = ins->size >= 4 ? arg_i32(script_pc, 0) : 0;
            const i32 anm_index = ins->size >= 8 ? arg_i32(script_pc, 1) : 0;
            const i32 script_id = ins->size >= 12 ? arg_i32(script_pc, 2) : 0;
            if (vm_slot >= 0 && vm_slot < 8) {
                if (anm_vms[vm_slot] != 0 && kill_ending_anm) {
                    kill_ending_anm(anm_vms[vm_slot]);
                    anm_vms[vm_slot] = 0;
                }
                AnmFile* file = (anm_index >= 0 && anm_index < 4) ? ending_anms[anm_index] : nullptr;
                if (file != nullptr && spawn_ending_anm) {
                    anm_vms[vm_slot] = spawn_ending_anm(file, script_id, vm_slot);
                }
            }
            events.push_back("anm-script");
            advance_pc();
            continue;
        }

        case 9:
            if (ins->size >= 4) {
                u32 c = 0;
                std::memcpy(&c, script_pc + 4, 4);
                text_color = c;
            }
            events.push_back("color");
            advance_pc();
            continue;

        case 10:
            last_bgm.assign(reinterpret_cast<const char*>(script_pc + 4));
            events.push_back("bgm");
            // 0x43c370(0, 名) + 0x43c3f0(0, 曲目)：曲名为 bgm/th16_14 记 0xf 号，否则 0x10 号已听过
            if (music != nullptr) {
                music->register_track(0, last_bgm.c_str());
                music->play_slot(0);
            }
            if (scores != nullptr) scores->mark_bgm_heard(last_bgm == "bgm/th16_14" ? 0xf : 0x10);
            advance_pc();
            continue;

        case 11:
            // 0x45ed00(5, 3 秒 ÷ 游戏速度)
            if (music != nullptr) music->fade_out(3.0f, game_speed != nullptr ? *game_speed : 1.0f);
            script_flags &= ~1u;
            events.push_back("fade");
            advance_pc();
            continue;

        case 12:
            advance_pc();
            if (!switch_to_staff()) return ScriptResult::Finished;
            continue;

        case 0xf:
        case 0x10:
        case 0x11: {
            // 难度条件精灵：仅当 difficulty 匹配时执行与 op8 相同的 spawn
            const i32 need = ins->op - 0xf + 1;  // 0xf→1, 0x10→2, 0x11→3
            if (difficulty == need) {
                const i32 vm_slot = ins->size >= 4 ? arg_i32(script_pc, 0) : 0;
                const i32 anm_index = ins->size >= 8 ? arg_i32(script_pc, 1) : 0;
                const i32 script_id = ins->size >= 12 ? arg_i32(script_pc, 2) : 0;
                if (vm_slot >= 0 && vm_slot < 8) {
                    if (anm_vms[vm_slot] != 0 && kill_ending_anm) {
                        kill_ending_anm(anm_vms[vm_slot]);
                        anm_vms[vm_slot] = 0;
                    }
                    AnmFile* file = (anm_index >= 0 && anm_index < 4) ? ending_anms[anm_index] : nullptr;
                    if (file != nullptr && spawn_ending_anm) {
                        anm_vms[vm_slot] = spawn_ending_anm(file, script_id, vm_slot);
                    }
                }
            }
            advance_pc();
            continue;
        }

        case 0xd:
        case 0xe: {
            // 0x45d150：op0xd → kind 0（0x45c630 淡入）；op0xe → kind 5（0x45c900 淡出）
            const i32 duration = ins->size >= 4 ? arg_i32(script_pc, 0) : 0;
            const i32 kind = ins->op == 0xd ? 0 : 5;
            if (create_fade) create_fade(kind, duration, 0);
            events.push_back(ins->op == 0xd ? "fade-in" : "fade-out");
            advance_pc();
            continue;
        }

        default:
            advance_pc();
            continue;
        }
    }
}

bool EndingScene::finish_animation_load() {
    if ((script_flags & 4u) == 0) return false;
    // 宿主已经同步读完动画。下一帧开始时提交装载完成。
    script_flags &= ~4u;
    return true;
}

void EndingScene::end() {
    live = false;
    finished = true;
    events.push_back("end");
}

bool EndingScene::update(const InputState& input) {
    if (!live) return false;
    ++timer;

    if (step_script(input) == ScriptResult::Finished) {
        end();
        return true;
    }

    // 烟雾 / 调试口径：>0x78 确认结束整段（完整 staff 跑完才退是原版路径）
    if (smoke_skip && timer > kSkipFrames &&
        (input.pressed & (InputState::kConfirmMask | InputState::kCancelMask)) != 0) {
        end();
        return true;
    }
    if (smoke_skip && timer > kAutoFrames) {
        end();
        return true;
    }
    return false;
}

}  // namespace th16
