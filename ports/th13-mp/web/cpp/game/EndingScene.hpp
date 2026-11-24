#pragma once

#include "GameInput.hpp"
#include "SessionState.hpp"
#include "Types.hpp"

#include <array>
#include <string>
#include <vector>

namespace th13 {

class EndingSaveBytes {
public:
    virtual ~EndingSaveBytes() = default;
    virtual u8 read(i32 index) const = 0;
    virtual void write(i32 index, u8 value) = 0;
};

i32 ending_index_for(const SessionState& progress, const EndingSaveBytes& save);

struct EndingPlatform {
    virtual ~EndingPlatform() = default;
    virtual bool read_asset(const std::string& name, std::vector<u8>& bytes, bool required = true) = 0;
    virtual EndingSaveBytes* save_bytes() = 0;
    virtual bool request_destination(i32 raw) = 0;
    virtual bool queue_music(i32 code, i32 value, const std::string& name) = 0;
    virtual bool record_music_unlocked(i32) { return true; }
    virtual bool fade_out_music(i32 frames) = 0;
    virtual bool advance_sound() = 0;
    virtual bool load_ending_anm(i32, const std::string&) { return true; }
    virtual bool cue_ending_anm(i32, i32 bank_slot, i32 script) { return cue_ending_anm(bank_slot, script); }
    virtual bool cue_ending_anm(i32, i32) { return true; }
    virtual void release_ending_anm(i32) {}
    virtual bool open_ending_window(i32, i32) { return true; }
    virtual bool ending_page_reset() { return true; }
    virtual bool ending_page_line(i32 row, u32 color, const std::string& cp932) = 0;
    virtual bool ending_page_clear() { return true; }
    virtual void release_ending_page() {}
};

class EndingScene {
public:
    EndingScene(SessionState& progress, EndingPlatform& platform, bool allow_first_view_fast_forward = false);

    bool initialize();
    bool update(const GameInput& input);
    bool should_restart(const GameInput& input) const;
    void teardown();

    bool finished() const { return finished_; }
    i32 ending_index() const { return ending_index_; }
    u32 scene_flags() const { return scene_flags_; }
    i32 line() const { return line_.target; }
    i32 frames() const { return frames_; }
    const std::string& message_name() const { return message_name_; }

    std::string error;

private:
    bool fail(const std::string& why) {
        if (error.empty()) error = why.empty() ? "Ending scene failed" : why;
        return false;
    }
    struct Interp {
        i32 current = -999999, target = 0;
        float acc = 0;
    };
    static void interp_set(Interp& it, i32 param);
    static void interp_tick(Interp& it, float delta);
    void advance_line();

    struct Record {
        u32 line = 0;
        u8 op = 0, length = 0;
        const u8* payload = nullptr;
    };
    bool record_at(u32 offset, Record& out) const;
    i32 payload_i32(const Record& r, u32 index) const;
    std::string payload_cstring(const Record& r, u32 byte_offset = 0) const;

    bool op_page_line(const Record& r);
    bool op_page_clear();
    bool op_text_wait(const GameInput& input, const Record& r, bool page_break);
    bool op_anm_load(const Record& r);
    bool op_anm_vm(const Record& r);
    bool op_music(const Record& r);
    bool op_reload_next(const Record& r);

    SessionState& progress_;
    EndingPlatform& platform_;
    bool allow_first_view_fast_forward_;

    i32 ending_index_ = 0;
    u32 scene_flags_ = 0;
    i32 frames_ = 0;

    std::vector<u8> msg_bytes_;
    std::string message_name_;
    u32 record_ = 0;
    Interp line_;
    Interp text_wait_;
    u32 state_flags_ = 1;
    u32 color_ = 0x00ffffff;
    i32 page_lines_ = 0;
    std::array<std::string, 5> page_text_;

    bool finished_ = false;
};

}
