#pragma once

#include "Types.hpp"

#include <string>
#include <vector>

namespace th13 {

struct MsgEntry {
    u32 offset = 0;
    u32 id = 0;
};

class MsgFile {
public:
    bool open(const u8* data, u32 size, std::string& error);
    u32 entry_count() const { return u32(entries_.size()); }
    const MsgEntry& entry(u32 index) const { return entries_[index]; }
    const u8* bytes() const { return bytes_.data(); }
    u32 size() const { return u32(bytes_.size()); }

private:
    std::vector<u8> bytes_;
    std::vector<MsgEntry> entries_;
};

std::string msg_decrypt_text(const u8* args, u32 arg_bytes);
std::string cp932_to_utf8(const u8* data, u32 size);
bool build_cp932_font_mapping(std::vector<u8>& out);

struct MsgLine {
    std::string text_cp932;
    std::string text_utf8;
    bool has_ruby = false;
    i32 ruby_x = 0;
    i32 ruby_spacing = 0;
    std::string ruby_cp932;
    std::string ruby_utf8;
    float bubble_width = 0.0f;
    u32 block = 0;
    i32 side = 0;
    u32 text_attr = 0;
};

struct MsgDialogState {
    bool active = false;
    bool done = false;
    i32 entry = 0;
    i32 side = 0;
    float box_x = 192.0f, box_y = 320.0f;
    bool auto_advance = false;
    u32 text_attr = 0;
    bool skip_request = false;
    float bubble_width = 0.0f;
    u32 block = 0;
    std::vector<std::string> lines;
    std::vector<MsgLine> lines_meta;
};

struct MsgAdvance {
    bool z_pressed = false;
    u32 held = 0;
    u32 z_frames = 0;
    u32 ctrl_frames = 0;
    u32 pressed = 0;
};

class MsgHost {
public:
    virtual ~MsgHost() = default;
    virtual void msg_bgm(i32 opcode) = 0;
    virtual void msg_portrait_load() = 0;
    virtual void msg_boss_battle() {}
    virtual void msg_stage_clear() = 0;
    virtual void msg_portrait_interrupt(i32 opcode, i32 label) {
        (void)opcode;
        (void)label;
    }
    virtual void msg_speaker_switch(i32 opcode) { (void)opcode; }
    virtual void msg_release(i32 opcode) { (void)opcode; }
    virtual void msg_music_fade(i32 opcode) { (void)opcode; }
    virtual void msg_music_fade(i32 opcode, float seconds) {
        (void)seconds;
        msg_music_fade(opcode);
    }
    virtual void msg_text_anchor_y(float value) { (void)value; }
    virtual void msg_advance_blip() {}
    virtual void msg_dialog_end() {}
};

class MsgPlayer {
public:
    bool open(MsgFile& file, u32 entry_index, MsgHost& host, std::string& error);
    bool step(const MsgAdvance& input);
    bool step(bool z_pressed);
    bool active() const { return state_.active; }
    bool done() const { return state_.done; }
    u32 clock() const { return clock_; }
    bool ecl_wait_released() const { return !active() || command_delay_ != 0; }
    const MsgDialogState& state() const { return state_; }

    struct Snapshot {
        static constexpr u32 kNoCursor = 0xFFFFFFFFu;
        u32 cursor_offset = kNoCursor;
        MsgDialogState state;
        u32 clock = 0;
        i32 line_index = 0, block_open = 0, wait_timeout = 0, command_delay = 0;
        bool pending_ruby = false;
        i32 pending_ruby_x = 0, pending_ruby_spacing = 0;
        std::string pending_ruby_cp932, pending_ruby_utf8;
    };
    Snapshot capture() const;
    void restore(const Snapshot& source);

private:
    enum class Flow { Advance, Blocked, Finished };
    Flow execute(u8 opcode, const u8* args, u32 arg_bytes, const MsgAdvance& input);
    void finish();

    MsgFile* file_ = nullptr;
    MsgHost* host_ = nullptr;
    const u8* cursor_ = nullptr;
    MsgDialogState state_;
    u32 clock_ = 0;
    i32 line_index_ = 0;
    i32 block_open_ = 0;
    i32 wait_timeout_ = 0;
    i32 command_delay_ = 0;
    bool pending_ruby_ = false;
    i32 pending_ruby_x_ = 0;
    i32 pending_ruby_spacing_ = 0;
    std::string pending_ruby_cp932_;
    std::string pending_ruby_utf8_;
};

}
