#include "EndingScene.hpp"

#include "MsgPlayer.hpp"

#include <cmath>
#include <cstring>

namespace th13 {

namespace {
constexpr const char* kEndingMessages[13] = {
    "staff.msg", "e01.msg", "e02.msg", "e03.msg", "e04.msg", "e05.msg", "e06.msg",
    "e07.msg",   "e08.msg", "e09.msg", "e10.msg", "e11.msg", "e12.msg",
};
constexpr i32 kEndingByteBase = 0x1b26a;
constexpr i32 kNegativeWaitFallback = 999;
}

i32 ending_index_for(const SessionState& progress, const EndingSaveBytes& save) {
    const i32 base = (progress.character & 3);
    i32 index = base;
    if (progress.continues != 0) {
        return index + 9;
    }
    index = 2 * base + 1;
    const bool seen_standard = (save.read(2 * base + 1) & 1) != 0;
    if (progress.difficulty >= 1 && progress.good_endings >= 3 && seen_standard) index += 1;
    return index;
}

EndingScene::EndingScene(SessionState& progress, EndingPlatform& platform, bool allow_first_view_fast_forward)
    : progress_(progress), platform_(platform), allow_first_view_fast_forward_(allow_first_view_fast_forward) {}

void EndingScene::interp_set(Interp& it, i32 param) {
    it.current = param - 1;
    it.target = param;
    it.acc = float(param);
}

void EndingScene::interp_tick(Interp& it, float delta) {
    it.current = it.target;
    constexpr float speed = 1.0f;
    if (speed > 0.99f && speed < 1.01f)
        it.acc += delta;
    else
        it.acc += speed * delta;
    it.target = i32(it.acc);
}

void EndingScene::advance_line() {
    line_.current = line_.target;
    line_.acc += 1.0f;
    line_.target += 1;
}

bool EndingScene::record_at(u32 offset, Record& out) const {
    if (offset + 4 > msg_bytes_.size()) return false;
    const u8* raw = msg_bytes_.data() + offset;
    out.line = u32(raw[0]) | u32(raw[1]) << 8;
    out.op = raw[2];
    out.length = raw[3];
    out.payload = raw + 4;
    return offset + 4 + out.length <= msg_bytes_.size();
}

i32 EndingScene::payload_i32(const Record& r, u32 index) const {
    const u32 at = 4 * index;
    if (r.length < at + 4) return 0;
    i32 value = 0;
    std::memcpy(&value, r.payload + at, 4);
    return value;
}

std::string EndingScene::payload_cstring(const Record& r, u32 byte_offset) const {
    if (byte_offset >= r.length) return {};
    const u8* start = r.payload + byte_offset;
    const u8* end = r.payload + r.length;
    const u8* nul = static_cast<const u8*>(std::memchr(start, 0, size_t(end - start)));
    const u8* stop = nul ? nul : end;
    return std::string(reinterpret_cast<const char*>(start), size_t(stop - start));
}

bool EndingScene::initialize() {
    if (finished_) return fail("Ending scene already finished");
    EndingSaveBytes* save = platform_.save_bytes();
    if (!save) return fail("Ending scene has no save view");
    ending_index_ = ending_index_for(progress_, *save);
    if (!allow_first_view_fast_forward_) {
        if (save->read(ending_index_) == 0) scene_flags_ |= 1u;
        if (save->read(0x1b276 - kEndingByteBase) == 0) scene_flags_ |= 2u;
    }
    {
        const u8 seen = u8(save->read(ending_index_) | 1);
        save->write(ending_index_, seen);
        if (progress_.difficulty > 0) save->write(ending_index_, u8(seen | 0x10));
        if (ending_index_ < 8) save->write(0x1b276 - kEndingByteBase, 1);
    }
    message_name_ = kEndingMessages[ending_index_ >= 0 && ending_index_ < 13 ? ending_index_ : 0];
    if (!platform_.read_asset(message_name_, msg_bytes_) || msg_bytes_.empty())
        return fail("Ending message unavailable: " + message_name_);
    msg_bytes_.resize(msg_bytes_.size() + 4, 0);
    line_ = Interp{-1, 0, 0.f};
    text_wait_ = Interp{-1, 0, 0.f};
    state_flags_ = 1;
    color_ = 0x00ffffff;
    page_lines_ = 0;
    page_text_.fill({});
    record_ = 0;
    {
        u32 script = 0;
        if (msg_bytes_.size() >= 8) std::memcpy(&script, msg_bytes_.data() + 4, 4);
        if (script + 4 > msg_bytes_.size()) return fail("Ending message has no script");
        record_ = script;
    }
    return true;
}

bool EndingScene::update(const GameInput& input) {
    if (!error.empty()) return false;
    if (finished_) return true;
    bool ended = false;
    while (true) {
        Record record;
        if (!record_at(record_, record)) {
            return fail("Ending script record out of range: " + message_name_);
        }
        if (record.line > u32(line_.target)) {
            advance_line();
            break;
        }
        bool handled = true;
        switch (record.op) {
        case 0:
            ended = true;
            break;
        case 1:
        case 2:
            break;
        case 3:
            handled = op_page_line(record);
            break;
        case 4:
            handled = op_page_clear();
            break;
        case 5:
            handled = op_text_wait(input, record, false);
            break;
        case 6:
            handled = op_text_wait(input, record, true);
            break;
        case 7:
            handled = op_anm_load(record);
            break;
        case 8:
            handled = op_anm_vm(record);
            break;
        case 9:
            color_ = u32(payload_i32(record, 0));
            break;
        case 10:
            handled = op_music(record);
            break;
        case 11:
            if (!platform_.fade_out_music(180)) return fail("Ending music fade failed");
            state_flags_ &= ~1u;
            break;
        case 12:
            if (!op_reload_next(record)) {
                ended = true;
                break;
            }
            continue;
        case 13:
            handled = platform_.open_ending_window(0, payload_i32(record, 0));
            break;
        case 14:
            handled = platform_.open_ending_window(5, payload_i32(record, 0));
            break;
        case 15:
        case 16:
        case 17: {
            const i32 gate = record.op - 14;
            if (progress_.difficulty == gate) handled = op_anm_vm(record);
            break;
        }
        default:
            break;
        }
        if (ended) break;
        if (!handled) {
            if (!error.empty()) return false;
            break;
        }
        record_ += 4 + record.length;
        if (record.op == 7) break;
    }
    if (ended) {
        (void)platform_.request_destination(0x10);
        finished_ = true;
        return true;
    }
    frames_++;
    return true;
}

bool EndingScene::should_restart(const GameInput& input) const {
    if (finished_ || !error.empty() || (state_flags_ & 4u) || (scene_flags_ & 2u) || !(state_flags_ & 2u)) return false;
    const bool held = (input.held & 0x200u) || ((input.held & 1u) && input.repeat_age[0] >= 20);
    return held && frames_ % 12 != 0;
}

bool EndingScene::op_page_line(const Record& r) {
    std::string text = msg_decrypt_text(r.payload, r.length);
    while (!text.empty() && text.back() == '\0')
        text.pop_back();
    if (page_lines_ == 0) {
        if (!platform_.ending_page_reset()) return fail("Ending page reset failed");
        page_text_.fill({});
    }
    page_text_[u32(page_lines_)] = text;
    if (!platform_.ending_page_line(page_lines_, color_, text)) return fail("Ending page line submission failed");
    page_lines_ = (page_lines_ + 1) % i32(page_text_.size());
    return true;
}

bool EndingScene::op_page_clear() {
    page_text_.fill({});
    if (!platform_.ending_page_clear()) return fail("Ending page clear failed");
    return true;
}

bool EndingScene::op_text_wait(const GameInput& input, const Record& r, bool page_break) {
    if (text_wait_.target <= 0) interp_set(text_wait_, payload_i32(r, 0));
    interp_tick(text_wait_, -1.0f);
    if (!page_break && payload_i32(r, 0) < 0) {
        interp_set(text_wait_, kNegativeWaitFallback);
    }
    if ((input.pressed & (0x1u | 0x80000u)) != 0 || text_wait_.target <= 0) {
        if (!platform_.advance_sound()) return fail("Ending advance sound failed");
        interp_set(text_wait_, 0);
    } else {
        if ((scene_flags_ & 1u) != 0) return false;
        const bool held_ok = (input.held & 0x200u) != 0 || ((input.held & 0x1u) != 0 && input.repeat_age[0] >= 0x14);
        if (!held_ok) return false;
        if (text_wait_.target % 6 != 0) return false;
        interp_set(text_wait_, 0);
    }
    if (page_break) {
        page_lines_ = 0;
    }
    return true;
}

bool EndingScene::op_anm_load(const Record& r) {
    const i32 index = payload_i32(r, 0);
    const std::string name = payload_cstring(r, 4);
    if (index < 0 || index > 3) return fail("Ending ANM slot out of range");
    if (!name.empty() && !platform_.load_ending_anm(0x14 + index, name)) return fail("Ending ANM load failed: " + name);
    return true;
}

bool EndingScene::op_anm_vm(const Record& r) {
    const i32 vm_slot = payload_i32(r, 0);
    const i32 bank_index = payload_i32(r, 1);
    const i32 script = payload_i32(r, 2);
    if (vm_slot < 0 || vm_slot >= 24 || bank_index < 0 || bank_index > 3)
        return fail("Ending ANM cue indices out of range");
    return platform_.cue_ending_anm(vm_slot, 0x14 + bank_index, script) || fail("Ending ANM cue failed");
}

bool EndingScene::op_music(const Record& r) {
    std::string name = payload_cstring(r);
    const i32 mode = name == "bgm/th13_18" ? 0xf : 0x10;
    const auto slash = name.find('/');
    if (slash != std::string::npos) name.erase(0, slash + 1);
    if (name.empty() || name.rfind(".wav", name.size() - 4) != std::string::npos)
        return fail("Ending music name invalid");
    return platform_.queue_music(1, 0, name + ".wav") && platform_.queue_music(2, 0, "") &&
           platform_.record_music_unlocked(mode);
}

bool EndingScene::op_reload_next(const Record& r) {
    const std::string name = payload_cstring(r);
    platform_.release_ending_page();
    std::vector<u8> bytes;
    if (name.empty() || !platform_.read_asset(name, bytes) || bytes.size() < 8) return false;
    msg_bytes_ = std::move(bytes);
    message_name_ = name;
    msg_bytes_.resize(msg_bytes_.size() + 4, 0);
    line_ = Interp{-1, 0, 0.f};
    text_wait_ = Interp{-1, 0, 0.f};
    color_ = 0x00ffffff;
    page_lines_ = 0;
    page_text_.fill({});
    u32 script = 0;
    std::memcpy(&script, msg_bytes_.data() + 4, 4);
    if (script + 4 > msg_bytes_.size()) return false;
    record_ = script;
    state_flags_ = 2u;
    return true;
}

void EndingScene::teardown() {
    platform_.release_ending_page();
    page_text_.fill({});
    page_lines_ = 0;
    for (i32 slot = 0x14; slot < 0x14 + 4; slot++)
        platform_.release_ending_anm(slot);
    msg_bytes_.clear();
    msg_bytes_.shrink_to_fit();
}

}
