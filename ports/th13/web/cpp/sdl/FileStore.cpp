// FileStore: v6 little-endian layout; accepts v1 (36), v2 (52), v3 (64 bytes)
// and both prior v4 spell-record layouts.
// "TH13SAVE" | u32 version | u32 cleared_mask | i32 bgm | i32 effect | u32[3]
// v2 appends 16 stage-played bytes (character*4+rank, bits0..5).
// v3 appends 5*i16 pad bindings and 2 padding bytes.
// v4 preserves offsets 0..63, separates Extra at +64, stores legacy ambiguity
// at +68, then 127 records of name[128] + five columns of four i32 counters.
// v5 keeps the complete v4 prefix and appends four ordinary character columns
// (six original ranking tables, play/time/clear counters) and an immutable full
// archive of the imported v1-v4 file. Missing old ordinary history is explicit.
// v6 retains that prefix, appends END1, history-complete(u32), 13 ending bytes,
// then an immutable full v5 import (u32 length and a fixed v5-sized archive).
// Old saves have unknown ending history; no progress bits manufacture it.
// 读档：magic/版本/尺寸不符 = 默认值（原版坏 scoreth13.dat 语义，不报错）。
// 写档：任何 I/O 失败 = false + error。web 走 localStorage（EM_JS try/catch），
// native/WASI 走真实文件；两侧同一序列化，互不伪造成功。
#include "FileStore.hpp"
#include "../game/TitlePlayerDataMenu.hpp"
#include "../game/SpellCatalog.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
// localStorage 持久化（宿主无文件系统持久卷）：base64 文本。任何异常/限额
// 都返回 false —— 失败可见，不吞。
EM_JS(bool, th13_save_write_js, (const char* data, int length), {
    try {
        let binary = '';
        const bytes = new Uint8Array(HEAPU8.buffer, data, length);
        for (let i = 0; i < length; ++i) binary += String.fromCharCode(bytes[i]);
        localStorage.setItem('th13.save', btoa(binary));
        return true;
    } catch (e) {
        return false;
    }
});
EM_JS(int, th13_save_read_js, (char* out, int capacity), {
    try {
        const text = localStorage.getItem('th13.save');
        if (text === null || text === '') return 0;
        const binary = atob(text);
        if (binary.length > capacity) return 0;
        const bytes = new Uint8Array(HEAPU8.buffer, out, binary.length);
        for (let i = 0; i < binary.length; ++i) bytes[i] = binary.charCodeAt(i);
        return binary.length;
    } catch (e) {
        // 读侧异常按缺档处理（原版读不到 score.dat = 默认值照常开局）；
        // 持久化失败在写侧（save）如实暴露，不在此阻断启动。
        return 0;
    }
});
// scoreth13.dat 原版格式通道（base64，同上语义）。
EM_JS(bool, th13_scoredat_write_js, (const char* data, int length), {
    try {
        let binary = '';
        const bytes = new Uint8Array(HEAPU8.buffer, data, length);
        for (let i = 0; i < length; ++i) binary += String.fromCharCode(bytes[i]);
        localStorage.setItem('th13.scoredat', btoa(binary));
        return true;
    } catch (e) {
        return false;
    }
});
EM_JS(int, th13_scoredat_read_js, (char* out, int capacity), {
    try {
        const text = localStorage.getItem('th13.scoredat');
        if (text === null || text === '') return 0;
        const binary = atob(text);
        if (binary.length > capacity) return 0;
        const bytes = new Uint8Array(HEAPU8.buffer, out, binary.length);
        for (let i = 0; i < binary.length; ++i) bytes[i] = binary.charCodeAt(i);
        return binary.length;
    } catch (e) {
        return 0;
    }
});
#endif

namespace th13::sdl {
namespace {
constexpr char kSaveMagic[8] = {'T', 'H', '1', '3', 'S', 'A', 'V', 'E'};
constexpr u32 kSaveVersion = 6;
constexpr size_t kLegacySaveBytes = 36;
constexpr size_t kV2SaveBytes = 52;
constexpr size_t kV3SaveBytes = 64;
constexpr size_t kSpellNameBytes = 128;
constexpr size_t kSpellRecordBytes = kSpellNameBytes + kSaveSpellColumns * 16;
constexpr size_t kSpellRecordsOffset = 72;
constexpr size_t kV4BaseBytes = kSpellRecordsOffset + kSaveSpellSlots * kSpellRecordBytes;
// Keep the initial v4 layout readable; append provenance and best scores.
constexpr size_t kLegacyArchiveOffset = kV4BaseBytes + 8;
constexpr size_t kBestScoresOffset = kLegacyArchiveOffset + 64;
constexpr size_t kV4SaveBytes = kBestScoresOffset + kSaveSpellSlots * kSaveSpellColumns * 4;
constexpr size_t kSummaryOffset = kV4SaveBytes + 8;
constexpr size_t kOrdinaryColumnBytes = 60 + 6 * 10 * 28;
constexpr size_t kMigratedArchiveOffset = kSummaryOffset + kSaveCharacters * kOrdinaryColumnBytes;
constexpr size_t kV5SaveBytes = kMigratedArchiveOffset + 4 + kV4SaveBytes;
constexpr size_t kEndingBytesOffset = kV5SaveBytes + 8;
constexpr size_t kPreEndingSizeOffset = kEndingBytesOffset + 13;
constexpr size_t kPreEndingArchiveOffset = kPreEndingSizeOffset + 4;
constexpr size_t kSaveBytes = kPreEndingArchiveOffset + kV5SaveBytes;

// v1.00c data at VA 004bbd30, consumed by 00412970/00453b30/00456b50.
// Slot rank is intrinsic to the spell; Extra=4 and Overdrive=5.

void put32(u8* at, u32 value) {
    for (size_t i = 0; i < 4; ++i) at[i] = u8(value >> (i * 8));
}
u32 get32(const u8* at) {
    u32 value = 0;
    for (size_t i = 0; i < 4; ++i) value |= u32(at[i]) << (i * 8);
    return value;
}
bool spell_position_valid(i32 character, i32 slot) {
    return character >= 0 && character < kSaveCharacters &&
           slot >= 0 && slot < kSaveSpellSlots;
}
void increment_counter(i32& counter) {
    if (counter < 99999) ++counter;
}
}  // namespace

SavedOrdinaryRecords::SavedOrdinaryRecords() {
    // 00449760: original defaults are explicit score rows, not fake play data.
    for (auto& rank : ranking) for (size_t i = 0; i < rank.size(); ++i) {
        rank[i].score = 100000 - i32(i) * 10000;
        rank[i].stage = 1;
        rank[i].name = "--------";
        rank[i].available = true;
    }
}

bool FileStore::player_data_summary(i32 difficulty, i32 character,
                                    TitlePlayerDataSummary& out) const {
    if (character < 0 || character >= kSaveCharacters || difficulty < 0 ||
        difficulty >= kPlayerDataDifficulties) return false;
    const auto& records = ordinary_records[size_t(character)];
    out = {};
    out.ranking = records.ranking[size_t(difficulty)];
    out.play_count = records.play_count;
    out.total_frames = records.total_frames;
    out.clear_count = records.clear_count[size_t(difficulty)];
    out.no_continue_clear_count = records.no_continue_clear_count[size_t(difficulty)];
    out.available = true;
    out.history_complete = records.history_complete;
    return true;
}

bool FileStore::record_run_start(i32 character) {
    if (character < 0 || character >= kSaveCharacters) return false;
    increment_counter(ordinary_records[size_t(character)].play_count);
    return true;
}

bool FileStore::record_play_frame(i32 character) {
    if (character < 0 || character >= kSaveCharacters) return false;
    auto& frames = ordinary_records[size_t(character)].total_frames;
    if (frames < 215999999) ++frames; // 0042c550: active, unpaused, non-Replay tick
    return true;
}

bool FileStore::record_run_clear(i32 character, i32 difficulty, bool no_continue) {
    if (character < 0 || character >= kSaveCharacters || difficulty < 0 || difficulty >= 5)
        return false;
    auto& records = ordinary_records[size_t(character)];
    increment_counter(records.clear_count[size_t(difficulty)]);
    if (no_continue) {
        increment_counter(records.no_continue_clear_count[size_t(difficulty)]);
        set_cleared(character, difficulty);
    }
    return true;
}

i32 FileStore::record_high_score(i32 character, i32 difficulty,
                                 const TitlePlayerDataScore& score) {
    if (character < 0 || character >= kSaveCharacters || difficulty < 0 || difficulty >= 5 ||
        score.score < 0 || score.stage < 0 || score.stage > 9 || score.continues < 0 ||
        score.continues > 9 || !std::isfinite(score.progress_percent)) return -1;
    auto& ranking = ordinary_records[size_t(character)].ranking[size_t(difficulty)];
    for (size_t i = 0; i < ranking.size(); ++i) {
        if (ranking[i].score > score.score) continue;
        for (size_t next = ranking.size() - 1; next > i; --next) ranking[next] = ranking[next - 1];
        ranking[i] = score;
        ranking[i].name = score.name.substr(0, 8); // original 8 name bytes + NUL
        ranking[i].available = true;
        return i32(i);
    }
    return -1;
}

bool FileStore::record_spell_attempt(i32 character, i32 slot, const std::string& name,
                                    bool spell_practice) {
    if (!spell_position_valid(character, slot)) return false;
    auto& record = spell_records[size_t(slot)];
    record.name = name.substr(0, kSpellNameBytes - 1);
    const size_t mode = spell_practice ? 1 : 0;
    increment_counter(record.columns[size_t(character)].attempts[mode]);
    increment_counter(record.columns[kSaveCharacters].attempts[mode]);
    return true;
}

bool FileStore::record_spell_capture(i32 character, i32 slot, bool spell_practice) {
    if (!spell_position_valid(character, slot)) return false;
    auto& record = spell_records[size_t(slot)];
    const size_t mode = spell_practice ? 1 : 0;
    increment_counter(record.columns[size_t(character)].captures[mode]);
    increment_counter(record.columns[kSaveCharacters].captures[mode]);
    return true;
}

bool FileStore::record_spell_practice_score(i32 character, i32 slot, i32 internal_score) {
    if (!spell_position_valid(character, slot) || internal_score < 0) return false;
    // DAT_004be7c0 is stored in tens; 0042cd40 rounds it before record+0x98.
    const i32 score = internal_score / 10 * 10;
    auto& record = spell_records[size_t(slot)];
    auto& personal = record.columns[size_t(character)].practice_best_score;
    personal = std::max(personal, score);
    return true;
}

const SavedSpellCounters* FileStore::spell_counters(i32 character, i32 slot) const {
    if (character < 0 || character >= kSaveSpellColumns || slot < 0 || slot >= kSaveSpellSlots)
        return nullptr;
    return &spell_records[size_t(slot)].columns[size_t(character)];
}

i32 FileStore::spell_difficulty(i32 slot) {
    return SpellCatalog::difficulty(slot);
}

i32 FileStore::player_data_record_count(i32 difficulty) {
    if (difficulty < 0 || difficulty >= kPlayerDataDifficulties) return 0;
    i32 count = 0;
    for (i32 slot = 0; slot < kSaveSpellSlots; ++slot)
        if (spell_difficulty(slot) == difficulty) ++count;
    return count;
}

bool FileStore::player_data_record(i32 difficulty, i32 character, i32 ordinal,
                                  TitlePlayerDataRecord& out) const {
    if (difficulty < 0 || difficulty >= kPlayerDataDifficulties || character < 0 ||
        character >= kSaveSpellColumns || ordinal < 0) return false;
    for (i32 slot = 0; slot < kSaveSpellSlots; ++slot) {
        if (spell_difficulty(slot) != difficulty) continue;
        if (ordinal-- != 0) continue;
        const auto& record = spell_records[size_t(slot)];
        const auto& counters = record.columns[size_t(character)];
        const auto& all = record.columns[kSaveCharacters];
        out = {};
        out.slot = slot;
        out.name = record.name;
        out.normal_captures = counters.captures[0];
        out.normal_attempts = counters.attempts[0];
        out.practice_captures = counters.captures[1];
        out.practice_attempts = counters.attempts[1];
        out.practice_best_score = counters.practice_best_score;
        out.recorded = all.attempts[0] != 0;
        out.character_flagged = counters.captures[0] != 0;
        // Preserve the legacy DTO fields for existing callers. +0x16578 is
        // ordinary attempts, never a score; production rendering uses named counters.
        out.score = all.attempts[0];
        out.number_a = all.captures[0];
        out.number_b = all.captures[1];
        return true;
    }
    return false;
}

bool FileStore::cleared(i32 character, i32 difficulty) const {
    if (character < 0 || character >= kSaveCharacters || difficulty < 0 ||
        difficulty >= kSaveDifficulties)
        return false;
    return (cleared_mask >> u32(character * kSaveDifficulties + difficulty)) & 1u;
}

void FileStore::set_cleared(i32 character, i32 difficulty) {
    if (character < 0 || character >= kSaveCharacters || difficulty < 0 ||
        difficulty >= kSaveDifficulties)
        return;
    cleared_mask |= 1u << u32(character * kSaveDifficulties + difficulty);
}

bool FileStore::practice_stage_played(i32 character, i32 difficulty, i32 stage) const {
    if (character < 0 || character >= 4 || difficulty < 0 || difficulty >= 4 ||
        stage < 1 || stage > 6) return false;
    return (practice_played[u32(character * 4 + difficulty)] & (1u << (stage - 1))) != 0;
}

void FileStore::mark_practice_stage_played(i32 character, i32 difficulty, i32 stage) {
    if (character < 0 || character >= 4 || difficulty < 0 || difficulty >= 4 ||
        stage < 1 || stage > 6) return;
    practice_played[u32(character * 4 + difficulty)] |= u8(1u << (stage - 1));
}

bool FileStore::initialize() {
    error.clear();
    loaded = false;
    bgm_volume = 100;
    effect_volume = 80;
    cleared_mask = heard_mask = 0;
    practice_played.fill(0);
    extra_unlocked_mask = 0;
    legacy_practice_extra_overlap = 0;
    legacy_save_size_ = 0;
    legacy_save_bytes_.fill(0);
    pad_bindings = {0, 1, 2, 3, 4};
    spell_records = {};
    ordinary_records = {};
    ending_seen_bytes.fill(0);
    committed_ending_bytes_.fill(0);
    ending_history_complete = true;
    migrated_save_bytes_.clear();
    pre_ending_save_bytes_.clear();
    score_dat_loaded_ = false;
    // ST LCG boot 种子：原版 boot 取 (u16)timeGetTime()（0x43a040），此处取
    // steady 时钟毫秒 u16（同为无符号截断的易变源，语义对照）。表生成一次后，
    // 文件 ST 块校验通过则整体覆盖回文件值——首存后随存档持久（0x449b00→0x449cd0）。
    const auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now().time_since_epoch())
                            .count();
    score_dat_seed = u16(u64(now_ms) & 0xFFFF);
    score_dat.set_defaults(score_dat_seed);
    if (!load_v6()) return false;
    // scoreth13.dat 原版格式通道（仅 web 激活；native 构建读侧恒缺档）。语义：
    // v6 有效 ⇒ v6 是应用侧唯一权威，dat 导入仅限 (a) v6 缺失/无效、(b) dat
    // 明显更新（全条目最大 u64 时间戳 > 榜表最大时间戳）；否则 dat 永不覆盖，
    // 也不在装载期落盘（镜像只由 save_score() 写出）。parse 在局部副本上进行，
    // 弃用的 dat 不污染 save_score 待写镜像。
    std::vector<u8> dat;
    ScoreDat candidate;
    if (read_score_dat(dat) && candidate.parse_file(dat.data(), dat.size()) &&
        score_dat_should_import(candidate.max_entry_timestamp())) {
        score_dat = candidate;
        score_dat_loaded_ = true;
        overlay_score_face_from_dat();
    }
    return true;
}

bool FileStore::load_v6() {
    // One extra byte detects overlong native saves instead of accepting a prefix.
    std::vector<u8> buffer(kSaveBytes + 1);
#ifdef __EMSCRIPTEN__
    const int length = th13_save_read_js(reinterpret_cast<char*>(buffer.data()),
                                         int(buffer.size()));
    if (length == 0) return true;         // 缺档/读异常 = 新档默认值（原版语义）
    if (size_t(length) != kLegacySaveBytes && size_t(length) != kV2SaveBytes &&
        size_t(length) != kV3SaveBytes && size_t(length) != kV4BaseBytes &&
        size_t(length) != kV4SaveBytes && size_t(length) != kV5SaveBytes &&
        size_t(length) != kSaveBytes) return true;
#else
    const char* override_path = std::getenv("TH13_SAVE_PATH");
    const std::string path = override_path ? override_path : "th13-save.bin";
    std::FILE* file = std::fopen(path.c_str(), "rb");
    if (!file) return true;  // 缺档 = 新档默认值（原版语义，非错误）
    const size_t length = std::fread(buffer.data(), 1, buffer.size(), file);
    const bool failed = std::ferror(file) != 0;
    std::fclose(file);
    if (failed) {
        error = "Save file read failed: " + path;
        return false;
    }
    if (length != kLegacySaveBytes && length != kV2SaveBytes &&
        length != kV3SaveBytes && length != kV4BaseBytes && length != kV4SaveBytes &&
        length != kV5SaveBytes && length != kSaveBytes) return true;
#endif
    const u32 version = get32(buffer.data() + 8);
    if (std::memcmp(buffer.data(), kSaveMagic, sizeof(kSaveMagic)) != 0 ||
        !((version == 1 && size_t(length) == kLegacySaveBytes) ||
          (version == 2 && size_t(length) == kV2SaveBytes) ||
          (version == 3 && size_t(length) == kV3SaveBytes) ||
          (version == 4 && (size_t(length) == kV4BaseBytes || size_t(length) == kV4SaveBytes)) ||
          (version == 5 && size_t(length) == kV5SaveBytes) ||
          (version == kSaveVersion && size_t(length) == kSaveBytes)))
        return true;  // 坏档 = 默认值（原版语义）
    if (version >= 6 &&
        (std::memcmp(buffer.data() + kV5SaveBytes, "END1", 4) != 0 ||
         get32(buffer.data() + kV5SaveBytes + 4) > 1 ||
         (get32(buffer.data() + kPreEndingSizeOffset) != 0 &&
          get32(buffer.data() + kPreEndingSizeOffset) != kV5SaveBytes))) return true;
    if (version >= 4 && size_t(length) != kV4BaseBytes) {
        const u32 legacy_size = get32(buffer.data() + kV4BaseBytes + 4);
        if (std::memcmp(buffer.data() + kV4BaseBytes, "REC1", 4) != 0 ||
            (legacy_size != 0 && legacy_size != kLegacySaveBytes &&
             legacy_size != kV2SaveBytes && legacy_size != kV3SaveBytes)) return true;
        legacy_save_size_ = legacy_size;
        std::copy_n(buffer.data() + kLegacyArchiveOffset, 64, legacy_save_bytes_.begin());
    } else if (version < 4) {
        legacy_save_size_ = u32(length);
        std::copy_n(buffer.data(), length, legacy_save_bytes_.begin());
    }
    if (version < 5) {
        migrated_save_bytes_.assign(buffer.begin(), buffer.begin() + length);
        for (auto& records : ordinary_records) {
            records.history_complete = false;
            for (auto& rank : records.ranking) for (auto& score : rank) score.available = false;
        }
    } else {
        if (std::memcmp(buffer.data() + kV4SaveBytes, "SUM1", 4) != 0 ||
            get32(buffer.data() + kV4SaveBytes + 4) != kSaveCharacters) return true;
        const u32 migrated_length = get32(buffer.data() + kMigratedArchiveOffset);
        if (migrated_length > kV4SaveBytes) return true;
        migrated_save_bytes_.assign(buffer.begin() + kMigratedArchiveOffset + 4,
                                   buffer.begin() + kMigratedArchiveOffset + 4 + migrated_length);
        for (size_t column = 0; column < ordinary_records.size(); ++column) {
            auto& records = ordinary_records[column];
            const u8* data = buffer.data() + kSummaryOffset + column * kOrdinaryColumnBytes;
            records.play_count = i32(std::min(get32(data), 99999u));
            records.total_frames = i32(std::min(get32(data + 4), 215999999u));
            for (size_t rank = 0; rank < 6; ++rank) {
                records.clear_count[rank] = i32(std::min(get32(data + 8 + rank * 4), 99999u));
                records.no_continue_clear_count[rank] = i32(std::min(get32(data + 32 + rank * 4), 99999u));
                for (size_t row = 0; row < 10; ++row) {
                    const u8* entry = data + 60 + (rank * 10 + row) * 28;
                    auto& score = records.ranking[rank][row];
                    score.score = i32(get32(entry));
                    score.stage = i8(entry[4]);
                    score.continues = i8(entry[5]);
                    size_t len = 0;
                    while (len < 8 && entry[6 + len]) ++len;
                    score.name.assign(reinterpret_cast<const char*>(entry + 6), len);
                    score.timestamp = i64(u64(get32(entry + 16)) | (u64(get32(entry + 20)) << 32));
                    score.progress_percent = float_from_bits(get32(entry + 24));
                    score.available = entry[15] != 0;
                }
            }
            records.history_complete = get32(data + 56) != 0;
        }
    }
    cleared_mask = get32(buffer.data() + 12);
    bgm_volume = i32(get32(buffer.data() + 16));
    effect_volume = i32(get32(buffer.data() + 20));
    heard_mask = get32(buffer.data() + 24);  // reserved[0]：31 曲位
    if (version >= 2) {
        for (size_t i = 0; i < practice_played.size(); ++i)
            practice_played[i] = buffer[kLegacySaveBytes + i] & 0x3fu;
        if (version < 4) {
            extra_unlocked_mask = get32(buffer.data() + kLegacySaveBytes) & 0xfu;
            legacy_practice_extra_overlap = extra_unlocked_mask;
        }
    }
    if (version >= 3) {
        for (size_t i = 0; i < 5; ++i) {
            const size_t at = kV2SaveBytes + i * 2;
            const i32 val = i32(buffer[at]) | (i32(buffer[at + 1]) << 8);
            if (val >= 0 && val < 32) pad_bindings[i] = val;
        }
    }
    if (version >= 4) {
        extra_unlocked_mask = get32(buffer.data() + kV3SaveBytes) & 0xfu;
        legacy_practice_extra_overlap = get32(buffer.data() + 68) & 0xfu;
        for (size_t slot = 0; slot < spell_records.size(); ++slot) {
            const size_t at = kSpellRecordsOffset + slot * kSpellRecordBytes;
            auto& record = spell_records[slot];
            const char* name = reinterpret_cast<const char*>(buffer.data() + at);
            size_t name_length = 0;
            while (name_length + 1 < kSpellNameBytes && name[name_length] != '\0') ++name_length;
            record.name.assign(name, name_length);
            for (size_t column = 0; column < kSaveSpellColumns; ++column) {
                auto& counters = record.columns[column];
                const u8* values = buffer.data() + at + kSpellNameBytes + column * 16;
                for (size_t mode = 0; mode < 2; ++mode) {
                    counters.captures[mode] = i32(std::min(get32(values + mode * 4), 99999u));
                    counters.attempts[mode] = i32(std::min(get32(values + 8 + mode * 4), 99999u));
                }
                if (size_t(length) != kV4BaseBytes) {
                    const u8* score = buffer.data() + kBestScoresOffset +
                                      (slot * kSaveSpellColumns + column) * 4;
                    counters.practice_best_score = i32(std::min(get32(score), u32(INT32_MAX)));
                }
            }
        }
    }
    if (bgm_volume < 0 || bgm_volume > 100) bgm_volume = 100;
    if (effect_volume < 0 || effect_volume > 100) effect_volume = 80;
    if (version >= 6) {
        std::copy_n(buffer.data() + kEndingBytesOffset, ending_seen_bytes.size(),
                    ending_seen_bytes.begin());
        ending_history_complete = get32(buffer.data() + kV5SaveBytes + 4) != 0;
        const u32 archived_length = get32(buffer.data() + kPreEndingSizeOffset);
        pre_ending_save_bytes_.assign(buffer.begin() + kPreEndingArchiveOffset,
                                     buffer.begin() + kPreEndingArchiveOffset + archived_length);
    } else {
        ending_history_complete = false;
        if (version == 5)
            pre_ending_save_bytes_.assign(buffer.begin(), buffer.begin() + length);
    }
    loaded = true;
    committed_ending_bytes_ = ending_seen_bytes;
    return true;
}

bool FileStore::save() {
    return write_save(false);
}

bool FileStore::save_score() {
    // 原版保存点（0x42bfa0/0x4399e0 → 0x449e50）：v6 应用档与 scoreth13.dat
    // 得分面镜像同一提交点。v6 档失败即失败；web 通道镜像写失败同如实报错；
    // native 构建通道不激活（write_score_dat no-op），语义与旧版逐字节一致。
    if (!write_save(true)) return false;
    sync_score_face_to_dat();
    return write_score_dat();
}

i64 FileStore::max_ranking_timestamp() const {
    i64 newest = 0;
    for (const auto& records : ordinary_records) {
        for (const auto& rank : records.ranking) {
            for (const auto& score : rank) newest = std::max(newest, score.timestamp);
        }
    }
    return newest;
}

bool FileStore::write_save(bool commit_ending) {
    error.clear();
    std::vector<u8> buffer(kSaveBytes);
    std::copy_n(legacy_save_bytes_.data(), legacy_save_size_, buffer.data());
    std::memcpy(buffer.data(), kSaveMagic, sizeof(kSaveMagic));
    put32(buffer.data() + 8, kSaveVersion);
    put32(buffer.data() + 12, cleared_mask);
    put32(buffer.data() + 16, u32(bgm_volume));
    put32(buffer.data() + 20, u32(effect_volume));
    put32(buffer.data() + 24, heard_mask);  // reserved[0]（上限 31 曲）
    for (size_t i = 0; i < practice_played.size(); ++i)
        buffer[kLegacySaveBytes + i] = practice_played[i] & 0x3fu;
    put32(buffer.data() + kV3SaveBytes, extra_unlocked_mask & 0xfu);
    put32(buffer.data() + 68, legacy_practice_extra_overlap & 0xfu);
    std::memcpy(buffer.data() + kV4BaseBytes, "REC1", 4);
    put32(buffer.data() + kV4BaseBytes + 4, legacy_save_size_);
    std::copy(legacy_save_bytes_.begin(), legacy_save_bytes_.end(),
              buffer.data() + kLegacyArchiveOffset);
    for (size_t i = 0; i < 5; ++i) {
        const u16 value = u16(pad_bindings[i]);
        buffer[kV2SaveBytes + i * 2] = u8(value);
        buffer[kV2SaveBytes + i * 2 + 1] = u8(value >> 8);
    }
    for (size_t slot = 0; slot < spell_records.size(); ++slot) {
        const size_t at = kSpellRecordsOffset + slot * kSpellRecordBytes;
        const auto& record = spell_records[slot];
        std::memcpy(buffer.data() + at, record.name.data(),
                    std::min(record.name.size(), kSpellNameBytes - 1));
        for (size_t column = 0; column < kSaveSpellColumns; ++column) {
            const auto& counters = record.columns[column];
            u8* values = buffer.data() + at + kSpellNameBytes + column * 16;
            for (size_t mode = 0; mode < 2; ++mode) {
                put32(values + mode * 4, u32(std::clamp(counters.captures[mode], 0, 99999)));
                put32(values + 8 + mode * 4, u32(std::clamp(counters.attempts[mode], 0, 99999)));
            }
            const u32 score = u32(std::max(counters.practice_best_score, 0));
            u8* best = buffer.data() + kBestScoresOffset +
                       (slot * kSaveSpellColumns + column) * 4;
            put32(best, score);
        }
    }
    std::memcpy(buffer.data() + kV4SaveBytes, "SUM1", 4);
    put32(buffer.data() + kV4SaveBytes + 4, kSaveCharacters);
    put32(buffer.data() + kMigratedArchiveOffset, u32(migrated_save_bytes_.size()));
    std::copy(migrated_save_bytes_.begin(), migrated_save_bytes_.end(),
              buffer.data() + kMigratedArchiveOffset + 4);
    for (size_t column = 0; column < ordinary_records.size(); ++column) {
        const auto& records = ordinary_records[column];
        u8* data = buffer.data() + kSummaryOffset + column * kOrdinaryColumnBytes;
        put32(data, u32(records.play_count));
        put32(data + 4, u32(records.total_frames));
        put32(data + 56, records.history_complete ? 1 : 0);
        for (size_t rank = 0; rank < 6; ++rank) {
            put32(data + 8 + rank * 4, u32(records.clear_count[rank]));
            put32(data + 32 + rank * 4, u32(records.no_continue_clear_count[rank]));
            for (size_t row = 0; row < 10; ++row) {
                u8* entry = data + 60 + (rank * 10 + row) * 28;
                const auto& score = records.ranking[rank][row];
                put32(entry, u32(score.score));
                entry[4] = u8(score.stage); entry[5] = u8(score.continues);
                std::memcpy(entry + 6, score.name.data(), std::min(score.name.size(), size_t(8)));
                entry[15] = score.available ? 1 : 0;
                put32(entry + 16, u32(score.timestamp));
                put32(entry + 20, u32(u64(score.timestamp) >> 32));
                put32(entry + 24, float_to_bits(score.progress_percent));
            }
        }
    }
    std::memcpy(buffer.data() + kV5SaveBytes, "END1", 4);
    put32(buffer.data() + kV5SaveBytes + 4, ending_history_complete ? 1 : 0);
    const auto& ending_bytes = commit_ending ? ending_seen_bytes : committed_ending_bytes_;
    std::copy(ending_bytes.begin(), ending_bytes.end(),
              buffer.data() + kEndingBytesOffset);
    put32(buffer.data() + kPreEndingSizeOffset, u32(pre_ending_save_bytes_.size()));
    std::copy(pre_ending_save_bytes_.begin(), pre_ending_save_bytes_.end(),
              buffer.data() + kPreEndingArchiveOffset);
#ifdef __EMSCRIPTEN__
    if (!th13_save_write_js(reinterpret_cast<const char*>(buffer.data()),
                            int(buffer.size()))) {
        error = "Save storage write failed";
        return false;
    }
    if (commit_ending) committed_ending_bytes_ = ending_seen_bytes;
    return true;
#else
    const char* override_path = std::getenv("TH13_SAVE_PATH");
    const std::string path = override_path ? override_path : "th13-save.bin";
    std::FILE* file = std::fopen(path.c_str(), "wb");
    if (!file) {
        error = "Save file open failed: " + path;
        return false;
    }
    const bool written = std::fwrite(buffer.data(), 1, buffer.size(), file) == buffer.size();
    const bool closed = std::fclose(file) == 0;
    const bool ok = written && closed;
    if (!ok) error = "Save file write failed: " + path;
    if (ok && commit_ending) committed_ending_bytes_ = ending_seen_bytes;
    return ok;
#endif
}

bool FileStore::score_dat_file_ready() {
    // ReplayFile::deflate（bit-exact 0x458c30）已交付并经黄金整文件逐字节
    // 验证（tests/score/score_dat_test.cpp build_file_round_trip_golden），
    // th13.scoredat 通道读写全线接通。
    return true;
}

i32 FileStore::score_dat_chara(i32 character) {
    switch (character) {
    case 0: return 0;   // 霊夢 → ReimuA（IPhoneDia chara 0..5 惯例序）
    case 1: return 2;   // 魔理沙 → MarisaA
    case 2: return 4;   // 早苗 → SanaeA
    default: return -1; // 妖夢：端口扩展角色，原版 6 chara 无对应，不写不覆盖
    }
}

bool FileStore::read_score_dat(std::vector<u8>& out) {
    out.clear();
#ifdef __EMSCRIPTEN__
    std::vector<char> buffer(1 << 20);
    const int length = th13_scoredat_read_js(buffer.data(), int(buffer.size()));
    if (length <= 0) return false;
    out.assign(buffer.begin(), buffer.begin() + length);
    return true;
#else
    // native/WASI 构建通道完全不激活：无 localStorage，也不做任何文件访问
    // （native 仅单测/诊断用；历史上按 cwd 找 scoreth13.dat 曾把无关文件卷进
    // 装载并互相改写）。恒缺档 = 走 v6/默认路径，行为与无此通道版本一致。
    return false;
#endif
}

bool FileStore::write_score_dat() {
#ifdef __EMSCRIPTEN__
    if (!score_dat_file_ready()) return true;  // 阶段 B 门（见上）
    std::vector<u8> file;
    if (!score_dat.build_file(file)) {
        error = "ScoreDat build failed: " + score_dat.error;
        return false;
    }
    if (!th13_scoredat_write_js(reinterpret_cast<const char*>(file.data()),
                                int(file.size()))) {
        error = "ScoreDat storage write failed";
        return false;
    }
    return true;
#else
    // native/WASI 构建通道不激活：不落盘（save_score 语义 = 仅写 v6，与旧版
    // 逐字节一致）。镜像仍由 sync_score_face_to_dat() 留在内存供测试检视。
    return true;
#endif
}

void FileStore::sync_score_face_to_dat() {
    // FileStore 字段 → ScoreDat 镜像。只写有原版写入点依据的字段；端口扩展
    // （continues 位、per-char 练习最好成绩、total_frames、Extra 解禁位、
    // spell 计数）无证据不入档，保持 parse/默认透传。字段映射详见
    // docs/plans/scoredat.md。
    for (int diff = 0; diff < kSaveDifficulties; ++diff) {
        if (!score_dat.cr_present[size_t(diff)]) {
            // 缺席块（剔除怪癖/未落档）：等价 0x449b00 boot 重建默认块再存。
            score_dat.cr_present[size_t(diff)] = true;
            score_dat.set_default_cr_block(u32(diff));
        }
        u8* block = score_dat.cr[size_t(diff)].data();
        for (i32 character = 0; character < kSaveCharacters; ++character) {
            const i32 chara = score_dat_chara(character);
            if (chara < 0) continue;
            const auto& records = ordinary_records[size_t(character)];
            for (size_t rank = 0; rank < 10; ++rank) {
                const auto& score = records.ranking[size_t(diff)][rank];
                ScoreDat::Entry entry;
                entry.score = u32(std::max(score.score, 0));
                entry.stage = u8(score.stage);
                // v6 +5 存的是 continues，原版条目 +5 是通关旗（0x43d6b0 写
                // DAT_004be7c8）：端口未跟踪 per-entry 通关旗，恒 0 不臆造。
                entry.clear = 0;
                const size_t name_bytes = std::min(score.name.size(), size_t(8));
                std::memcpy(entry.name.data(), score.name.data(), name_bytes);
                std::fill(entry.name.begin() + ptrdiff_t(name_bytes), entry.name.end(), u8(0));
                entry.time_lo = u32(u64(score.timestamp));
                entry.time_hi = u32(u64(score.timestamp) >> 32);
                entry.rate_bits = float_to_bits(score.progress_percent);
                ScoreDat::write_entry(block, chara, i32(rank), entry);
            }
            // 进度计数：0x42cd40（通关/无续关通关，上限 99999）。原版寻址为
            // “所玩难度块内 +0x7C0/+0x7DC”（0042cd40.c: (diff)*0x56dc+0x7c0/…，
            // 结构常量 = 块内 +8），端口 clear_count 本就按难度分列，一一对应。
            // 游戏回数（+0x7B8，0x42b990 同为难度块内）端口无难度维度——v6
            // play_count 是全难度合计，写入每块会臆造 per-difficulty 计数，
            // 故两侧都不映射（v6 自留、dat 保持 parse/默认）。
            ScoreDat::set_progress_u32(block, 0x7C0, chara,
                                       u32(std::min(records.clear_count[size_t(diff)], 99999)));
            ScoreDat::set_progress_u32(
                block, 0x7DC, chara,
                u32(std::min(records.no_continue_clear_count[size_t(diff)], 99999)));
            // 进度行 +5 游玩旗（0x42b990）← practice_played（diff 0..3）。
            if (diff < 4) {
                for (i32 stage = 1; stage <= 6; ++stage) {
                    if (practice_stage_played(character, diff, stage))
                        ScoreDat::set_progress_row_flag(block, chara, stage - 1, 1, 1);
                }
            }
        }
        // 练习记录最好成绩（0x42cd40 对 +0x98 的 max 写）：原版每难度每符单值，
        // 端口 5 列（4 角色+合计）取 max 合流；已 parse 的更大历史值保留。
        for (i32 slot = 0; slot < kSaveSpellSlots; ++slot) {
            u32 best = 0;
            for (i32 column = 0; column < kSaveSpellColumns; ++column) {
                best = std::max(best,
                                u32(std::max(spell_records[size_t(slot)]
                                                 .columns[size_t(column)]
                                                 .practice_best_score,
                                             0)));
            }
            const u32 current = ScoreDat::practice_u32(block, slot, 0x98);
            ScoreDat::set_practice_u32(block, slot, 0x98, std::max(current, best));
        }
    }
    // ending seen 13 字节（0x416130 读写 score+0x1b26a..0x1b276 = ST+0x16..0x22）。
    u8 seen[ScoreDat::kEndingBytes];
    std::copy(ending_seen_bytes.begin(), ending_seen_bytes.end(), seen);
    ScoreDat::write_ending_bytes(score_dat.st.data(), seen);
    // ST 上次名（+0x0C）：端口暂无名牌输入追踪，保持 parse/默认透传不臆造。
}

void FileStore::overlay_score_face_from_dat() {
    // ScoreDat 镜像 → FileStore 得分字段（装载覆盖；v6 设置类字段不动）。
    for (int diff = 0; diff < kSaveDifficulties; ++diff) {
        if (!score_dat.cr_present[size_t(diff)]) continue;  // 缺席块：该难度保持 v6/默认
        const u8* block = score_dat.cr[size_t(diff)].data();
        for (i32 character = 0; character < kSaveCharacters; ++character) {
            const i32 chara = score_dat_chara(character);
            if (chara < 0) continue;
            auto& records = ordinary_records[size_t(character)];
            for (size_t rank = 0; rank < 10; ++rank) {
                const ScoreDat::Entry entry = ScoreDat::read_entry(block, chara, i32(rank));
                auto& score = records.ranking[size_t(diff)][rank];
                score.score = i32(entry.score);
                score.stage = entry.stage;
                score.continues = 0;  // 原版条目无续关数字段（+5 是通关旗）
                size_t length = 0;
                while (length < 8 && entry.name[length] != 0) ++length;
                score.name.assign(reinterpret_cast<const char*>(entry.name.data()), length);
                score.timestamp =
                    i64(u64(entry.time_lo) | u64(entry.time_hi) << 32);
                score.progress_percent = float_from_bits(entry.rate_bits);
                score.available = true;  // 原版全 60 行常在（默认行即 100000..10000）
            }
            // 游戏回数（+0x7B8）不映射（见 sync 侧说明）；通关计数按难度覆盖。
            records.clear_count[size_t(diff)] =
                i32(std::min(ScoreDat::progress_u32(block, 0x7C0, chara), 99999u));
            const u32 no_continue =
                std::min(ScoreDat::progress_u32(block, 0x7DC, chara), 99999u);
            records.no_continue_clear_count[size_t(diff)] = i32(no_continue);
            // cleared 位镜像 v6 写语义（record_run_clear 的 no-continue 置位）；
            // 该位是派生得分面，随存档覆盖重算。
            const u32 bit = 1u << u32(character * kSaveDifficulties + diff);
            cleared_mask &= ~bit;
            if (no_continue != 0) cleared_mask |= bit;
            records.history_complete = true;  // 原版档即完整历史
            if (diff < 4) {
                u8& played = practice_played[size_t(character * 4 + diff)];
                for (i32 stage = 1; stage <= 6; ++stage) {
                    if (ScoreDat::progress_row_flag(block, chara, stage - 1, 1) != 0)
                        played |= u8(1u << (stage - 1));
                }
            }
        }
        // 练习最好成绩：原版每难度每符单值 → 合计列（per-char 列为端口扩展，
        // 保持 v6 值不臆造归属）。多难度块取 max（端口无 per-difficulty 维度）。
        for (i32 slot = 0; slot < kSaveSpellSlots; ++slot) {
            const u32 best = ScoreDat::practice_u32(block, slot, 0x98);
            auto& aggregate =
                spell_records[size_t(slot)].columns[size_t(kSaveCharacters)].practice_best_score;
            aggregate = std::max(aggregate, i32(std::min(best, u32(INT32_MAX))));
        }
    }
    // ending seen 13 字节与完整历史旗。
    u8 seen[ScoreDat::kEndingBytes];
    ScoreDat::read_ending_bytes(score_dat.st.data(), seen);
    std::copy(seen, seen + ScoreDat::kEndingBytes, ending_seen_bytes.begin());
    ending_history_complete = true;
    // ST 上次名留在 score_dat.st（端口读侧未接名牌显示，透传保真）。
}

}  // namespace th13::sdl
