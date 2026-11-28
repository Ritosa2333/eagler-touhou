#pragma once
#include "Types.hpp"
#include <string>
#include <vector>

namespace th13 {

struct ReplayFrame {
    u16 state = 0, pressed = 0, released = 0;
    bool eof() const { return state == 0xFFFF && pressed == 0xFFFF && released == 0xFFFF; }
};

struct ReplayStage {
    u16 number = 0;
    u16 stamp = 0;
    u32 frame_count = 0;
    u32 blob_size = 0;
    u32 score = 0;
    std::vector<u8> header;
    std::vector<u8> input;
    std::vector<u8> events;
    ReplayFrame frame(u32 index) const;
    u8 slow_rate(u32 frame) const {
        const u32 tick = frame / 30;
        return tick < events.size() ? events[tick] : 0;
    }
};

class ReplayFile {
public:
    bool decode(const u8* data, size_t size);
    const std::string& error() const { return error_; }

    u16 version = 0;
    u32 user_offset = 0, comp_size = 0, uncomp_size = 0;
    std::string player_name;
    i32 score = 0, character = 0, shot_type = 0, rank = 0, practice_stage = 0;
    float slow_rate = 0.0f;
    u32 stage_count = 0;
    std::vector<u8> top_header;
    std::vector<ReplayStage> stages;
    std::vector<std::string> user_blocks;

    static void xor_encrypt(u8* buf, size_t size, u32 init_key, u32 step, u32 block);
    static void xor_decrypt(u8* buf, size_t size, u32 init_key, u32 step, u32 block);
    static bool inflate(const u8* src, size_t src_size, std::vector<u8>& out, size_t limit);

    static std::vector<u8> deflate(const u8* src, size_t size);

private:
    std::string error_;
    bool fail(const std::string& message) {
        error_ = message;
        return false;
    }
};

class LzssEncoder {
public:
    void begin(const u8* src, size_t size);
    bool step(u32 tokens);
    bool finished() const { return finished_; }
    bool active() const { return active_; }
    std::vector<u8> take();

private:
    void put_bit(u32 bit);
    u32 tree_insert(u32 p);
    void advance(u32 count);

    std::vector<u8> src_;
    std::vector<u8> out_;
    std::vector<u8> ring_;
    std::vector<i32> dad_, lft_, rgt_;
    u32 acc_ = 0, mask_ = 0x80;
    u32 lookahead_ = 0, wp_ = 1, match_len_ = 0, match_pos_ = 0;
    size_t fill_ = 0;
    bool active_ = false, finished_ = false;
};

class ReplayInputCursor {
public:
    explicit ReplayInputCursor(const ReplayStage& stage) : stage_(&stage) {}
    bool valid() const { return stage_ && index_ < stage_->frame_count && !eof_reached_; }
    u32 frame_index() const { return index_; }
    const ReplayStage* stage() const { return stage_; }
    ReplayFrame next() {
        if (!valid()) return ReplayFrame{};
        const ReplayFrame result = stage_->frame(index_++);
        if (result.eof()) eof_reached_ = true;
        return result;
    }
    u8 current_slow_rate() const { return stage_ ? stage_->slow_rate(index_) : 0; }

private:
    const ReplayStage* stage_ = nullptr;
    u32 index_ = 0;
    bool eof_reached_ = false;
};

}
