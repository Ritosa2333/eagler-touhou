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
    return write_save(true);
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

}  // namespace th13::sdl
