#pragma once
#include "ScoreFile.hpp"
#include <array>
namespace th09 {
struct PlayTime {u32 hours=0,minutes=0,seconds=0,milliseconds=0;void add(u32 elapsed);};
struct ScoreEntry {
    std::array<u8,3> reserved_header{};
    u32 points=0,unknown=0;
    u8 character=0,difficulty=0,rank=0,stage=0;
    std::array<char,9> name{'N','o',' ','N','a','m','e',' ',0};
    std::array<char,10> date{'-','-','/','-','-',0};
    u8 continues=0;
    void read(const u8*);void write(u8*)const;
};
// Native score.dat chapter formats are encoded explicitly at the file boundary.
// Game code uses named records and never reads process addresses or file offsets.
class PlayerRecords {
    std::array<u8,24> container_header{};
    std::array<u8,3> profile_reserved{},name_reserved{};
    void read_profile(const u8*);
public:
    PlayTime application_time,game_time;
    std::array<u8,32> music_unlocked{};
    std::array<u8,16> versus_unlocked{},story_unlocked{},extra_unlocked{};
    std::array<std::array<u32,6>,16> clear_counts{};
    std::array<std::array<std::array<ScoreEntry,5>,5>,16> scores{};
    std::array<char,12> last_name{};
    u32 application_clock=0,game_clock=0;
    PlayerRecords(){reset();}
    void reset();
    bool load(const u8*,u32);
    bool load_plain(const u8*,u32);
    std::vector<u8> save(Rng&,u32 version_time_a=0,u32 version_time_b=0)const;
    std::vector<u8> serialize(Rng* order=nullptr,u32 version_time_a=0,u32 version_time_b=0)const;
    void write_profile(u8*)const;void write_last_name(u8*)const;
    u32 clear_count(i32 character,i32 difficulty)const;
    bool cleared(i32 character)const;
    void count_clear(i32 character,i32 difficulty);
    void count_encounter(i32 character);
    void unlock_after_ending(i32 character,i32 difficulty,bool count=true);
    i32 insert(const ScoreEntry&);
    const ScoreEntry* score(i32 character,i32 difficulty,i32 rank=0)const;
    void update_application_clock(u32 now);
    void update_game_clock(u32 now);
};
}
