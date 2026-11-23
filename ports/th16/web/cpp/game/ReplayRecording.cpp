#include "ReplayRecording.hpp"

#include "GameGlobals.hpp"
#include "ReplayGlobals.hpp"

#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <sys/stat.h>

namespace th16 {
namespace {

void put_i32(u8* p, i32 v) { std::memcpy(p, &v, 4); }
void put_u32(u8* p, u32 v) { std::memcpy(p, &v, 4); }
void put_f32(u8* p, float v) { std::memcpy(p, &v, 4); }
i32 load_i32(const u8* p) {
    i32 v = 0;
    std::memcpy(&v, p, 4);
    return v;
}

void append_user_block(std::vector<u8>& out, u8 kind, const std::string& text) {
    const u32 size = (12u + u32(text.size()) + 1u + 3u) & ~3u;
    const u32 start = u32(out.size());
    out.resize(start + size, 0);
    put_u32(out.data() + start, 0x52455355u);  // "USER"
    put_u32(out.data() + start + 4, size);
    out[start + 8] = kind;
    std::memcpy(out.data() + start + 12, text.data(), text.size());
}

bool ensure_replay_dir(const char* directory) {
    char path[512];
    std::snprintf(path, sizeof(path), "%s/replay", directory);
#if defined(_WIN32)
    _mkdir(path);
#else
    ::mkdir(path, 0755);
#endif
    return true;
}

}  // namespace

void ReplayRecording::reset() {
    user.assign(ReplayFile::kUserSize, 0);
    stages.clear();
    current = {};
    stage_open = false;
    sealed = false;
    timestamp = 0;
    pending_globals.clear();
    pending_new_game = false;
    error.clear();
}

void ReplayRecording::begin_run(const GameGlobals& globals, i32 time32) {
    reset();
    timestamp = time32;
    put_i32(user.data() + 0xc, time32);
    put_i32(user.data() + 0x84, globals.shot);
    put_i32(user.data() + 0x88, globals.shot_aux);
    put_i32(user.data() + 0x8c, globals.difficulty);
    put_i32(user.data() + 0x90, globals.stage);
    put_i32(user.data() + 0x98, globals.spell_id);
    put_i32(user.data() + 0x9c, globals.season);
    // +0xa bit1：符卡练习
    if ((globals.flags & GameGlobals::kModeMask) == 0x20) user[0xa] = u8(user[0xa] | 2);
}

void ReplayRecording::snapshot_globals(const GameGlobals& globals, bool new_game) {
    pending_globals.assign(kReplayGlobalsSize, 0);
    store_replay_globals(globals, pending_globals.data());
    pending_new_game = new_game;
}

void ReplayRecording::begin_stage(i16 stage, u16 rng_seed) {
    if (sealed) return;
    if (stage_open) close_stage(false);
    current = {};
    current.stage = stage;
    current.rng_seed = rng_seed;
    current.frames = 0;
    current.header.assign(ReplayFile::kStageHeaderSize, 0);
    u8* header = current.header.data();
    if (pending_globals.size() == kReplayGlobalsSize)
        std::memcpy(header + kReplayGlobalsOffset, pending_globals.data(), kReplayGlobalsSize);
    // 0x447760：+0x240 起 0x14 个 dword 填 0, -0x21522153, -2×0x21522153, …
    u32 filler = 0;
    for (i32 i = 0; i < kSpellTimeSlots; ++i, filler -= 0x21522153u) put_u32(header + kSpellTimeOffset + i * 4, filler);
    put_u32(header + 0x290, pending_new_game ? 1u : 0u);
    pending_globals.clear();
    pending_new_game = false;
    stage_open = true;
}

void ReplayRecording::set_stage_globals(const GameGlobals& globals) {
    if (!stage_open || current.header.size() != ReplayFile::kStageHeaderSize) return;
    store_replay_globals(globals, current.header.data() + kReplayGlobalsOffset);
}

void ReplayRecording::restamp_globals(const GameGlobals& globals) {
    if (stage_open) set_stage_globals(globals);
    else if (pending_globals.size() == kReplayGlobalsSize) store_replay_globals(globals, pending_globals.data());
}

void ReplayRecording::set_spell_time(i32 index, i32 value) {
    if (!stage_open || current.header.size() != ReplayFile::kStageHeaderSize) return;
    if (index < 0 || index >= kSpellTimeSlots) return;
    put_i32(current.header.data() + kSpellTimeOffset + index * 4, value);
}

void ReplayRecording::set_stage_start(i32 fixed_x, i32 fixed_y, i32 focus) {
    if (!stage_open || current.header.size() != ReplayFile::kStageHeaderSize) return;
    put_i32(current.header.data() + 0xc, fixed_x);
    put_i32(current.header.data() + 0x10, fixed_y);
    put_i32(current.header.data() + 0x23c, focus);
}

bool ReplayRecording::tick(u16 held, u16 pressed, u16 released, u8 fps_byte) {
    if (sealed || !stage_open) return true;
    if (current.inputs.size() + 6 > kMaxInputBytes) {
        error = "replay input overflow";
        return false;
    }
    if ((current.frames % 30) == 0) current.fps.push_back(fps_byte);
    u8 frame[6];
    std::memcpy(frame + 0, &held, 2);
    std::memcpy(frame + 2, &pressed, 2);
    std::memcpy(frame + 4, &released, 2);
    current.inputs.insert(current.inputs.end(), frame, frame + 6);
    ++current.frames;
    return true;
}

void ReplayRecording::close_stage(bool append_terminator) {
    if (!stage_open) return;
    if (append_terminator) {
        const u16 term = 0xffff;
        u8 frame[6];
        std::memcpy(frame + 0, &term, 2);
        std::memcpy(frame + 2, &term, 2);
        std::memcpy(frame + 4, &term, 2);
        current.inputs.insert(current.inputs.end(), frame, frame + 6);
        ++current.frames;
        // 终止帧也占 FPS 槽：仅当 frames%30==0 时原版在写入前已采样；此处追加后对齐帧数即可
    }
    stages.push_back(std::move(current));
    current = {};
    stage_open = false;
}

void ReplayRecording::mark_full_run(i32 time32) {
    if (user.size() != ReplayFile::kUserSize) user.assign(ReplayFile::kUserSize, 0);
    timestamp = time32;
    put_i32(user.data() + 0xc, time32);
    put_i32(user.data() + 0x90, 8);
}

ReplaySummary ReplayRecording::summary() const {
    ReplaySummary out;
    out.timestamp = timestamp;
    if (user.size() < ReplayFile::kUserSize) return out;
    out.spell_practice = (user[0xa] & 2) != 0;
    out.shot = load_i32(user.data() + 0x84);
    out.shot_aux = load_i32(user.data() + 0x88);
    out.difficulty = load_i32(user.data() + 0x8c);
    out.stage = load_i32(user.data() + 0x90);
    out.spell_id = load_i32(user.data() + 0x98);
    out.season = load_i32(user.data() + 0x9c);
    std::memcpy(&out.slow_rate, user.data() + 0x7c, sizeof(out.slow_rate));
    return out;
}

bool ReplayRecording::encode(std::vector<u8>& out, const char* player_name, i32 score, float slow_rate) {
    if (!error.empty()) return false;
    if (stage_open) close_stage(true);
    if (stages.empty()) {
        error = "no stages";
        return false;
    }
    if (player_name == nullptr) player_name = "";
    char name8[9] = {};
    std::memset(name8, ' ', 8);
    const size_t n = std::strlen(player_name);
    std::memcpy(name8, player_name, n > 8 ? 8 : n);

    ReplayFile file;
    file.user = user;
    if (file.user.size() != ReplayFile::kUserSize) file.user.assign(ReplayFile::kUserSize, 0);
    std::memcpy(file.user.data(), name8, 8);
    put_i32(file.user.data() + 0xc, timestamp);
    put_i32(file.user.data() + 0x14, score);
    put_f32(file.user.data() + 0x7c, slow_rate);
    put_i32(file.user.data() + 0x80, i32(stages.size()));
    file.stages = stages;
    file.stage_count = i32(stages.size());
    std::memcpy(file.name, name8, 8);

    std::vector<u8> body;
    if (!file.encode(body)) {
        error = file.error.empty() ? "encode failed" : file.error;
        return false;
    }

    // 尾部 USER 说明块（0x448400）：kind0 文本摘要 + kind1 日文提示；解码侧忽略
    char info[512];
    std::snprintf(info, sizeof(info),
                  "Touhou 16 replay\r\nVersion 1.00a\r\nName %.8s\r\nScore %d\r\nSlow Rate %2.2f\r\n",
                  name8, score, double(slow_rate));
    append_user_block(body, 0, info);
    append_user_block(body, 1, "Comment placeholder");
    out = std::move(body);
    sealed = true;
    return true;
}

bool ReplayRecording::write_slot(const char* directory, int slot, const char* player_name, i32 score,
                                 float slow_rate) {
    if (directory == nullptr || slot < 1 || slot > 100) {
        error = "bad slot";
        return false;
    }
    std::vector<u8> bytes;
    if (!encode(bytes, player_name, score, slow_rate)) return false;
    ensure_replay_dir(directory);
    char path[512];
    std::snprintf(path, sizeof(path), "%s/replay/th16_%02d.rpy", directory, slot);
    std::ofstream out(path, std::ios::binary);
    if (!out) {
        std::snprintf(path, sizeof(path), "%s/th16_%02d.rpy", directory, slot);
        out.open(path, std::ios::binary);
    }
    if (!out) {
        error = "open for write failed";
        return false;
    }
    out.write(reinterpret_cast<const char*>(bytes.data()), std::streamsize(bytes.size()));
    if (!out) {
        error = "write failed";
        return false;
    }
    return true;
}

bool ReplayRecording::slot_exists(const char* directory, int slot) {
    return replay_slot_exists(directory, slot);
}

bool ReplayRecording::slot_summary(const char* directory, int slot, ReplaySummary& out) {
    char path[512];
    if (!replay_slot_path(directory, slot, path, sizeof(path))) return false;
    ReplayFile file;
    if (!file.open_file(path) || file.user.size() < 0xa0) return false;
    std::memset(out.name, ' ', 8);
    out.name[8] = '\0';
    std::memcpy(out.name, file.user.data(), 8);
    out.timestamp = load_i32(file.user.data() + 0xc);
    out.spell_practice = (file.user[0xa] & 2) != 0;
    out.shot = load_i32(file.user.data() + 0x84);
    out.shot_aux = load_i32(file.user.data() + 0x88);
    out.difficulty = load_i32(file.user.data() + 0x8c);
    out.stage = load_i32(file.user.data() + 0x90);
    out.spell_id = load_i32(file.user.data() + 0x98);
    out.season = load_i32(file.user.data() + 0x9c);
    float slow = 0.0f;
    if (file.user.size() >= 0x80) std::memcpy(&slow, file.user.data() + 0x7c, sizeof(slow));
    out.slow_rate = slow;
    return true;
}

}  // namespace th16
