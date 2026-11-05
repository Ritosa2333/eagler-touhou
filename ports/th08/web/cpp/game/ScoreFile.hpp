#pragma once
#include "GameConfiguration.hpp"
#include "Lzss.hpp"
#include "Rng.hpp"
namespace th08 {
constexpr u32 spell_count=222,shot_count=12,stage_count=9,difficulty_count=5,last_word_count=17;
constexpr u32 fourcc(char a,char b,char c,char d){return u32(u8(a))|(u32(u8(b))<<8)|(u32(u8(c))<<16)|(u32(u8(d))<<24);}
struct ScoreChapter {u32 magic=0;u16 length=0,copy_length=0;u8 version=0,unknown=0;u16 padding=0;};
struct SpellHistory {i32 max_bonus[13]{};u32 attempts[13]{},captures[13]{};};
struct SpellRecord {
    ScoreChapter header;u16 number=0;u8 unknown=0,difficulty=0;
    char name[48]{},owner[48]{},comment1[64]{},comment2[64]{};
    SpellHistory game,practice;i32 unknown228=0;
};
struct ClearRecord {ScoreChapter header;u16 without_retries[5]{},with_retries[5]{};u8 unknown=0,shot=0;u16 padding=0;};
struct PracticeRecord {ScoreChapter header;i32 attempts[9][5]{},high_scores[9][5]{};u8 shot=0,used=0;u16 padding=0;};
struct PlayCounts {u32 total=0,characters[12]{},restarts=0,clears=0,continues=0,practices=0;};
struct PlayRecord {ScoreChapter header;u32 total_time[4]{},game_time[4]{};PlayCounts counts[7];i8 music_unlocked[32]{};};
struct LastWords {ScoreChapter header;u8 unlocked[17]{},padding[3]{};};
struct LastName {ScoreChapter header;char name[9]{};u8 padding[3]{};};
struct VersionRecord {ScoreChapter header;char version[8]{};u32 exe_size=0,exe_checksum=0;};
struct HighScore {
    ScoreChapter header;u32 score=0;float lag=0;u8 character=0,difficulty=0,stage=0;
    char name[9]{},date[6]{};i8 retries=0;u8 unknown27=0;
    GameConfiguration config;
    i32 play_frames=0,points=0,unknown6c=0,deaths=0,bombs=0,last_spells=0,pauses=0,time_orbs=0,humanity=0;
    u8 spell_counters[222]{},padding[2]{};
};
struct ScoreHeader {
    u8 unknown=0,random1=0;u16 checksum=0,version=1;u8 random2=0,padding=0;
    u32 header_size=28,list_reserved=0,total_size=28,payload_size=0,compressed_size=0;
};
static_assert(sizeof(ScoreHeader)==0x1c&&sizeof(SpellRecord)==0x22c&&sizeof(ClearRecord)==0x24);
static_assert(sizeof(PracticeRecord)==0x178&&sizeof(PlayRecord)==0x228&&sizeof(LastWords)==0x20);
static_assert(sizeof(HighScore)==0x168&&sizeof(LastName)==0x18&&sizeof(VersionRecord)==0x1c);
class ScoreFile {
public:
    bool decode(const u8* data,u32 size);
    bool assign_decoded(const u8* data,u32 size);
    std::vector<u8> encode(Rng& rng);
    const std::vector<u8>& decoded() const noexcept {return bytes;}
    bool copy_chapter(u32 magic,u8 version,void* output,u32 size,bool first) const;
    void spells(SpellRecord* out) const;
    void clears(ClearRecord* out) const;
    void practice(PracticeRecord* out) const;
    std::vector<HighScore> high_scores(u32 character,u32 difficulty) const;
    u32 high_score(u32 character,u32 difficulty,u8& retries) const;
private:
    bool valid() const;
    std::vector<u8> bytes;
    Lzss codec;
};
}
