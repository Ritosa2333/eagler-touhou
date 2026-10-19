#pragma once
#include "ScoreFile.hpp"
#include "SessionCompletion.hpp"
#include "TitleRecords.hpp"
#include <functional>
namespace th15 {
struct HighScoreEntry {i32 score=0;u8 stage=0;i8 continues=0;std::array<char,9> name{};u8 reserved=0;std::array<i32,4> details{};};
struct RunScoreSubmission {i32 score=0,stage=0,continues=0;std::array<i32,2> timestamp{};float slowdown=0;i32 deaths=0;};
inline float score_slowdown(double delivered,double requested)noexcept{return float(100.f-float(float(delivered/requested)*100.f));}
struct SpellRecord {std::array<char,128> name{};std::array<i32,2> captures{},attempts{};i32 identifier=0,difficulty=0,score=0;};
struct ModeRecords {std::array<std::array<HighScoreEntry,10>,6> scores{};std::array<SpellRecord,119> spells{};i32 plays=0;u64 play_centiseconds=0;std::array<i32,5> clears{},without_continue{};};
struct PracticeRecord {i32 score=0;u8 cleared=0,visited=0;};
struct CharacterRecords {std::array<ModeRecords,2> modes{};std::array<std::array<PracticeRecord,8>,5> practice{};};
// File layouts live only in import/export. Gameplay and title menus share these
// named records; no original object addresses or packed executable structures.
class RecordStore final:public SessionRecords,public CompletionRecords {
 ScoreFile file;bool fail(const char*);bool character_valid(i32)const noexcept;bool stage_valid(i32,i32,i32)const noexcept;
 void decode();void encode();static void count(i32&,i32)noexcept;
 ModeRecords& mode(i32 character,bool legacy){return characters[character].modes[legacy?1:0];}
public:
 std::array<CharacterRecords,5> characters{};std::array<u8,20> music{};std::array<char,9> name{};u64 total_play_centiseconds=0;std::string error;
 std::array<u8,8> endings{};u8 ending_seen=0;
 std::function<bool(i32,i32)> checkpoint_remove;
 void reset(Rng&);bool open(const u8*,u32);bool save(std::vector<u8>&);
 SessionHighScore high_score(const SessionRecordQuery&)override;
 bool mark_stage(i32,i32,i32)override;bool count_play(i32,bool)override;
 bool stage_clear(i32,i32,i32)override;bool finished_run(i32,bool,i32,bool)override;
 bool spell_score(i32,bool,i32,i32)override;bool remove_checkpoint(i32,i32)override;
 bool spell_begin(i32 character,bool legacy,i32 spell,bool practice,const std::string&);
 bool spell_capture(i32 character,bool legacy,i32 spell,bool practice);
 i32 insert_score(i32 character,bool legacy,i32 difficulty,const RunScoreSubmission&);
 i32 score_rank(i32 character,bool legacy,i32 difficulty,i32 score)const noexcept;
 bool practice_score(i32 character,i32 difficulty,i32 stage,i32 score);
 bool score_name(i32 character,bool legacy,i32 difficulty,i32 slot,const std::array<char,9>&);
 bool unlock_music(i32);bool unlock_all();u32 music_mask()const noexcept;TitleRecords title_records()const noexcept;
 bool begin_ending(i32 index,i32 difficulty,u32& first_seen_flags);
 bool add_play_time(i32 character,bool legacy,u64 centiseconds);
 const ScoreFile& saved_file()const noexcept{return file;}
 // Only the file codec and comparison fixture need this synchronization point.
 bool import_blocks(const u8* characters,const u8* settings);const ScoreFile& export_blocks(){encode();return file;}
};
}
