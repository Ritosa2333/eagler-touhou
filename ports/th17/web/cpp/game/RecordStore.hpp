#pragma once
#include "ScoreFile.hpp"
#include "GameProgression.hpp"
#include "SpellRecords.hpp"
namespace th17 {
struct HighScoreEntry {i32 score=0;u8 stage=0;i8 continues=0;std::array<char,9> name{};u8 reserved=0;std::array<i32,4> details{};};
static_assert(sizeof(HighScoreEntry)==32);
struct RunScoreSubmission {i32 score=0,stage=0,continues=0;std::array<i32,2> timestamp{};float slowdown=0;};
// Serialized blocks stay at this boundary. Menus and gameplay consume the same
// named records, including all nine loadouts and the aggregate spell bank.
class RecordStore {
    ScoreFile file;bool fail(const char*);void decode();void encode();
public:
    GameRecords game;SpellRecords spells;
    std::array<std::array<std::array<HighScoreEntry,10>,6>,10> scores{};
    std::array<std::array<i32,101>,10> spell_difficulty{};
    std::array<u8,18> music{};std::array<u8,16> endings{};std::array<u8,40> trophies{};
    std::array<char,9> name{};std::array<u64,10> play_centiseconds{};u64 total_play_centiseconds=0;
    std::string error;
    void reset(Rng&);bool open(const u8*,u32);bool save(std::vector<u8>&);
    i32 insert_score(i32 loadout,i32 difficulty,const RunScoreSubmission&);
    bool score_name(i32 loadout,i32 difficulty,i32 slot,const std::array<char,9>&);
    bool add_play_time(i32 loadout,u64);bool unlock_music(i32);bool trophy(i32);bool unlock_all();
    bool import_blocks(const u8*,const u8*);const ScoreFile& export_blocks(){encode();return file;}
};
}
