#pragma once
#include "ResourceCodec.hpp"
#include "Rng.hpp"
#include "SpellCard.hpp"
namespace th10 {
struct ScoreFileEnvironment;
struct ScoreChunkHeader {u16 signature,version;u32 checksum,length;};
struct HighScore {i32 score;u8 stage,score_units;char name[9];u8 reserved_00f;i32 timestamp;float slow_rate;};
static_assert(sizeof(HighScore)==0x18);
struct CharacterRecord {
    ScoreChunkHeader header;
    i32 character;
    HighScore high_scores[5][10];
    u8 statistics[0xdc];
    SpellRecord spells[110];
    void initialize(const std::int8_t* difficulties) noexcept;
};
static_assert(offsetof(CharacterRecord,spells)==0x59c && sizeof(CharacterRecord)==0x437c);
struct ScoreSettings {
    ScoreChunkHeader header;
    char last_name[9];
    u8 statistics[0x31];
    u16 random_words[512];
    u16 reserved_446;
    void initialize(Rng& random) noexcept;
};
static_assert(offsetof(ScoreSettings,random_words)==0x46 && sizeof(ScoreSettings)==0x448);
struct ScoreFileHeader {u32 signature,length;u16 version,reserved_00a;u32 version_code,packed_bytes,unpacked_bytes;};
static_assert(sizeof(ScoreFileHeader)==0x18);
struct ScoreData {
    ScoreFileHeader* file;
    u8* unpacked;
    CharacterRecord characters[7];
    ScoreSettings settings;
    void initialize(ScoreFileEnvironment& environment);
    i32 load(ScoreFileEnvironment& environment);
    i32 save(ScoreFileEnvironment& environment);
    void release(ScoreFileEnvironment& environment);
    static void create(ScoreFileEnvironment& environment);
    static void destroy(ScoreFileEnvironment& environment);
};
static_assert(offsetof(ScoreData,settings)==0x1d86c && sizeof(ScoreData)==0x1dcb4);
struct ScoreFileEnvironment : CodecMemory {
    const char* filename="scoreth10.dat";
    ScoreData** current;
    Rng* random;
    const std::int8_t* spell_difficulties;
    LzssSearch search;
    virtual ScoreData* allocate_score()=0;
    virtual void delete_score(ScoreData* score)=0;
    virtual u8* read_file(const char* name,u32& length)=0;
    virtual i32 open(const char* name)=0;
    virtual bool is_open()=0;
    virtual u32 write(const void* bytes,u32 length)=0;
    virtual void close_and_unlock()=0;
};
u32 score_checksum(const void* bytes,u32 length) noexcept;
}
