#include "MsgPlayer.hpp"

#include "MsgPlayerCp932.hpp"

#include <cstdlib>
#include <cstring>

namespace th13 {

namespace {

bool read_u32(const std::vector<u8>& bytes, u32 at, u32& value) {
    if (at + 4 > bytes.size()) return false;
    std::memcpy(&value, bytes.data() + at, 4);
    return true;
}

void append_utf8(std::string& out, u32 code) {
    if (code < 0x80) {
        out.push_back(char(code));
    } else if (code < 0x800) {
        out.push_back(char(0xC0 | (code >> 6)));
        out.push_back(char(0x80 | (code & 0x3F)));
    } else if (code < 0x10000) {
        out.push_back(char(0xE0 | (code >> 12)));
        out.push_back(char(0x80 | ((code >> 6) & 0x3F)));
        out.push_back(char(0x80 | (code & 0x3F)));
    } else {
        out.push_back(char(0xF0 | (code >> 18)));
        out.push_back(char(0x80 | ((code >> 12) & 0x3F)));
        out.push_back(char(0x80 | ((code >> 6) & 0x3F)));
        out.push_back(char(0x80 | (code & 0x3F)));
    }
}

}

bool MsgFile::open(const u8* data, u32 size, std::string& error) {
    entries_.clear();
    bytes_.assign(data, data + size);
    u32 count = 0;
    if (size < 4 || !read_u32(bytes_, 0, count) || count == 0 || count > 64) {
        error = "msg header entry count invalid";
        return false;
    }
    const u32 table_bytes = 4 + count * 8;
    if (size < table_bytes) {
        error = "msg entry table truncated";
        return false;
    }
    for (u32 index = 0; index < count; index++) {
        MsgEntry entry;
        if (!read_u32(bytes_, 4 + index * 8, entry.offset) || !read_u32(bytes_, 4 + index * 8 + 4, entry.id)) {
            error = "msg entry header read failed";
            return false;
        }
        if (entry.offset < table_bytes || entry.offset >= size) {
            error = "msg entry offset out of range";
            return false;
        }
        entries_.push_back(entry);
    }
    return true;
}

std::string msg_decrypt_text(const u8* args, u32 arg_bytes) {
    std::string out;
    out.reserve(arg_bytes);
    u32 key = 0x77;
    u32 inc = 0x07;
    for (u32 index = 0; index < arg_bytes; index++) {
        const u8 decoded = u8(args[index] ^ key);
        out.push_back(char(decoded));
        if (decoded == 0) break;
        key = (key + inc) & 0xFF;
        inc = (inc + 0x10) & 0xFF;
    }
    return out;
}

std::string cp932_to_utf8(const u8* data, u32 size) {
    std::string out;
    out.reserve(size);
    u32 index = 0;
    while (index < size) {
        const u8 lead = data[index];
        if (lead == 0) break;
        if (lead < 0x80) {
            append_utf8(out, lead);
            index++;
            continue;
        }
        if (lead >= 0xA1 && lead <= 0xDF) {
            append_utf8(out, 0xFF61u + (lead - 0xA1));
            index++;
            continue;
        }
        if (lead < 0x81 || lead > 0xEF || lead >= 0xF0 || index + 1 >= size) {
            append_utf8(out, 0xFFFD);
            index++;
            continue;
        }
        const u8 row = kCp932Row[lead - 0x81];
        const u8 trail = data[index + 1];
        u32 code = 0xFFFD;
        if (row != 0xFF && trail >= kCp932TrailBase && u32(trail) < u32(kCp932TrailBase) + u32(kCp932TrailCols)) {
            const u16 mapped = kCp932ToUnicode[row][trail - kCp932TrailBase];
            if (mapped != 0) code = mapped;
        }
        append_utf8(out, code);
        index += code == 0xFFFD ? 1 : 2;
    }
    return out;
}

bool build_cp932_font_mapping(std::vector<u8>& out) {
    out.assign(131072, 0);
    for (u32 n = 0; n < 0x10000; n++) {
        u16 value = 0x30FB;
        if (n < 0x100) {
            const u8 byte = u8(n);
            if (byte < 0x80)
                value = byte;
            else if (byte >= 0xA1 && byte <= 0xDF)
                value = u16(0xFF61u + (byte - 0xA1));
        } else {
            const u8 lead = u8(n >> 8), trail = u8(n & 0xFF);
            if (lead >= 0x81 && lead <= 0xEF) {
                const u8 row = kCp932Row[lead - 0x81];
                if (row != 0xFF && trail >= kCp932TrailBase) {
                    const u16 mapped = kCp932ToUnicode[row][trail - kCp932TrailBase];
                    if (mapped != 0) value = mapped;
                }
            }
        }
        out[n * 2] = u8(value & 0xFF);
        out[n * 2 + 1] = u8(value >> 8);
    }
    return out.size() == 131072;
}

bool MsgPlayer::open(MsgFile& file, u32 entry_index, MsgHost& host, std::string& error) {
    if (entry_index >= file.entry_count()) {
        error = "msg entry index out of range";
        return false;
    }
    file_ = &file;
    host_ = &host;
    cursor_ = file.bytes() + file.entry(entry_index).offset;
    state_ = MsgDialogState{};
    state_.active = true;
    state_.entry = i32(entry_index);
    clock_ = 0;
    line_index_ = 0;
    block_open_ = 0;
    wait_timeout_ = 0;
    command_delay_ = 0;
    return true;
}

void MsgPlayer::finish() {
    state_.done = true;
    state_.active = false;
    if (host_) host_->msg_dialog_end();
}

bool MsgPlayer::step(const MsgAdvance& input) {
    if (!state_.active) return false;
    if (command_delay_ > 0) --command_delay_;
    const u32 pressed = input.pressed | (input.z_pressed ? 1u : 0u);
    if (pressed & 0x201u) state_.skip_request = true;
    if (state_.auto_advance && state_.skip_request &&
        (((input.held & 1u) && input.z_frames >= 20) || ((input.held & 0x200u) && input.ctrl_frames >= 20)) &&
        cursor_ + 4 <= file_->bytes() + file_->size()) {
        u16 next_time = 0;
        std::memcpy(&next_time, cursor_, 2);
        clock_ = next_time;
    }
    for (;;) {
        if (cursor_ + 4 > file_->bytes() + file_->size()) {
            finish();
            return false;
        }
        u16 time = 0;
        std::memcpy(&time, cursor_, 2);
        const u8 opcode = cursor_[2];
        const u8 arg_bytes = cursor_[3];
        if (cursor_ + 4 + arg_bytes > file_->bytes() + file_->size()) {
            finish();
            return false;
        }
        if (clock_ < time) {
            ++clock_;
            return true;
        }
        if (opcode == 0) {
            finish();
            return false;
        }
        const Flow flow = execute(opcode, cursor_ + 4, arg_bytes, input);
        if (flow == Flow::Blocked) return true;
        cursor_ += 4 + arg_bytes;
    }
    return state_.active;
}

bool MsgPlayer::step(bool z_pressed) {
    MsgAdvance input;
    input.z_pressed = z_pressed;
    input.held = z_pressed ? 1u : 0u;
    return step(input);
}

MsgPlayer::Flow MsgPlayer::execute(u8 opcode, const u8* args, u32 arg_bytes, const MsgAdvance& input) {
    switch (opcode) {
    case 0x01:
        host_->msg_portrait_load();
        return Flow::Advance;
    case 0x02:
    case 0x13:
    case 0x14:
    case 0x1f:
        host_->msg_bgm(i32(opcode));
        return Flow::Advance;
    case 0x04:
    case 0x05:
    case 0x06:
        host_->msg_release(i32(opcode));
        return Flow::Advance;
    case 0x0d:
    case 0x0e: {
        i32 label = 0;
        if (arg_bytes >= 4) std::memcpy(&label, args, 4);
        host_->msg_portrait_interrupt(i32(opcode), label + 0x11);
        return Flow::Advance;
    }
    case 0x0c:
        command_delay_ = 1;
        return Flow::Advance;
    case 0x07:
    case 0x08:
        state_.side = opcode == 0x08 ? 1 : 0;
        state_.text_attr &= ~2u;
        line_index_ = 0;
        block_open_ = 0;
        host_->msg_speaker_switch(i32(opcode));
        return Flow::Advance;
    case 0x09:
        state_.text_attr &= ~2u;
        return Flow::Advance;
    case 0x0a: {
        i32 value = 0;
        if (arg_bytes >= 4) std::memcpy(&value, args, 4);
        state_.auto_advance = (value & 1) != 0;
        return Flow::Advance;
    }
    case 0x0b: {
        if (wait_timeout_ == 0 && arg_bytes >= 4) {
            i32 timeout = 0;
            std::memcpy(&timeout, args, 4);
            wait_timeout_ = timeout;
        }
        const bool pressed_z_enter = (input.pressed & (0x1u | 0x80000u)) != 0 || input.z_pressed;
        const bool timed_out = --wait_timeout_ <= 0;
        if (pressed_z_enter || timed_out) {
            host_->msg_advance_blip();
            line_index_ = 0;
            block_open_ = 0;
            wait_timeout_ = 0;
            return Flow::Advance;
        }
        if (state_.auto_advance && state_.skip_request &&
            (((input.held & 0x200u) && input.ctrl_frames >= 0x14) || ((input.held & 1u) && input.z_frames >= 0x14))) {
            line_index_ = 0;
            block_open_ = 0;
            wait_timeout_ = 0;
            return Flow::Advance;
        }
        return Flow::Blocked;
    }
    case 0x11: {
        std::string decoded = msg_decrypt_text(args, arg_bytes);
        if (!decoded.empty() && decoded[0] == '|') {
            size_t c1 = decoded.find(',', 1);
            size_t c2 = (c1 != std::string::npos) ? decoded.find(',', c1 + 1) : std::string::npos;
            if (c1 != std::string::npos && c2 != std::string::npos) {
                pending_ruby_ = true;
                pending_ruby_x_ = std::atoi(decoded.substr(1, c1 - 1).c_str());
                pending_ruby_spacing_ = std::atoi(decoded.substr(c1 + 1, c2 - c1 - 1).c_str());
                pending_ruby_cp932_ = decoded.substr(c2 + 1);
                const u8* ruby_raw = reinterpret_cast<const u8*>(pending_ruby_cp932_.data());
                pending_ruby_utf8_ = cp932_to_utf8(ruby_raw, u32(pending_ruby_cp932_.size()));
            }
            return Flow::Advance;
        }

        MsgLine line;
        line.text_cp932 = decoded;
        const u8* raw = reinterpret_cast<const u8*>(decoded.data());
        line.text_utf8 = cp932_to_utf8(raw, u32(decoded.size()));

        if (line_index_ == 0 && block_open_ == 0) {
            state_.lines.clear();
            state_.lines_meta.clear();
            state_.bubble_width = 0.0f;
            state_.block++;
        }
        line.block = state_.block;
        u32 len = u32(std::strlen(decoded.c_str()));
        u32 align_width = len > 0 ? ((len * 8) & 0xfffffff0u) : 0;
        float calc_w = float(align_width) - 28.0f;
        if (calc_w > state_.bubble_width) {
            state_.bubble_width = calc_w;
        }
        line.bubble_width = state_.bubble_width;

        if (pending_ruby_) {
            line.has_ruby = true;
            line.ruby_x = pending_ruby_x_;
            line.ruby_spacing = pending_ruby_spacing_;
            line.ruby_cp932 = pending_ruby_cp932_;
            line.ruby_utf8 = pending_ruby_utf8_;
            pending_ruby_ = false;
            pending_ruby_cp932_.clear();
            pending_ruby_utf8_.clear();
        }
        line.text_attr = state_.text_attr;
        line.side = state_.side;

        state_.lines.push_back(line.text_utf8);
        state_.lines_meta.push_back(line);

        if (line_index_ == 0) {
            block_open_ = 1;
            line_index_ = 1;
        } else {
            line_index_ = 0;
            block_open_ = 0;
        }
        return Flow::Advance;
    }
    case 0x12:
    case 0x17:
    case 0x18:
        host_->msg_release(i32(opcode));
        return Flow::Advance;
    case 0x19: {
        if (arg_bytes < 4) return Flow::Advance;
        i32 value = 0;
        std::memcpy(&value, args, sizeof value);
        host_->msg_text_anchor_y(float(value));
        return Flow::Advance;
    }
    case 0x1e:
        return Flow::Advance;
    case 0x1a:
        state_.text_attr |= 2u;
        return Flow::Advance;
    case 0x15:
        host_->msg_stage_clear();
        return Flow::Advance;
    case 0x16:
        host_->msg_music_fade(i32(opcode), 0.0f);
        return Flow::Advance;
    case 0x1b: {
        float seconds = 0;
        if (arg_bytes >= 4) std::memcpy(&seconds, args, sizeof seconds);
        host_->msg_music_fade(i32(opcode), seconds);
        return Flow::Advance;
    }
    case 0x1c: {
        float x = state_.box_x, y = state_.box_y;
        if (arg_bytes >= 4) std::memcpy(&x, args, 4);
        if (arg_bytes >= 8) std::memcpy(&y, args + 4, 4);
        state_.box_x = x;
        state_.box_y = y;
        return Flow::Advance;
    }
    case 0x1d: {
        i32 value = 0;
        if (arg_bytes >= 4) std::memcpy(&value, args, 4);
        state_.text_attr = (state_.text_attr & ~0x3Cu) | (u32(value) * 4 & 0x3Cu);
        return Flow::Advance;
    }
    default:
        return Flow::Advance;
    }
}

MsgPlayer::Snapshot MsgPlayer::capture() const {
    Snapshot out;
    if (cursor_ && file_) {
        const u8* base = file_->bytes();
        out.cursor_offset = u32(cursor_ - base);
    }
    out.state = state_;
    out.clock = clock_;
    out.line_index = line_index_;
    out.block_open = block_open_;
    out.wait_timeout = wait_timeout_;
    out.command_delay = command_delay_;
    out.pending_ruby = pending_ruby_;
    out.pending_ruby_x = pending_ruby_x_;
    out.pending_ruby_spacing = pending_ruby_spacing_;
    out.pending_ruby_cp932 = pending_ruby_cp932_;
    out.pending_ruby_utf8 = pending_ruby_utf8_;
    return out;
}

void MsgPlayer::restore(const Snapshot& source) {
    cursor_ = nullptr;
    if (file_ && source.cursor_offset != Snapshot::kNoCursor && source.cursor_offset < file_->size())
        cursor_ = file_->bytes() + source.cursor_offset;
    state_ = source.state;
    clock_ = source.clock;
    line_index_ = source.line_index;
    block_open_ = source.block_open;
    wait_timeout_ = source.wait_timeout;
    command_delay_ = source.command_delay;
    pending_ruby_ = source.pending_ruby;
    pending_ruby_x_ = source.pending_ruby_x;
    pending_ruby_spacing_ = source.pending_ruby_spacing;
    pending_ruby_cp932_ = source.pending_ruby_cp932;
    pending_ruby_utf8_ = source.pending_ruby_utf8;
}

}
