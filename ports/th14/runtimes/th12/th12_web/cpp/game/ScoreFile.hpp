#pragma once
#include "Rng.hpp"
#include "SpellController.hpp"
#include "StageCompletion.hpp"
#include <array>
#include <vector>
#include <string>
namespace th11 {
// Original scoreth11.dat records. Unknown fields are retained across reads and
// writes; named game records are synchronized explicitly at session boundaries.
class ScoreFile {
public:
#if PORT_TITLE == 14
    static constexpr u32 character_size=0x5298,settings_size=0x42c,file_magic=0x31344854,file_version=5;
    static constexpr u32 spell_count=120,spell_offset=0x978,spell_stride=0x9c;
    static constexpr u32 play_count_offset=0x7b8,play_time_offset=0x7bc,clear_count_offset=0x7c0;
    static constexpr u32 practice_score_offset=0x7f0,practice_flags_offset=0x7f4,practice_stage_stride=8,practice_slots=48;
#else
    static constexpr u32 character_size=0x68d4,settings_size=0x448,file_magic=0x31314854,file_version=4;
    static constexpr u32 spell_count=175,spell_offset=0x664,spell_stride=0x90;
    static constexpr u32 play_count_offset=0x588,play_time_offset=0x58c,clear_count_offset=0x590;
    static constexpr u32 practice_score_offset=0x59c,practice_flags_offset=0x5a0,practice_stage_stride=6,practice_slots=24;
#endif
    using Character=std::array<u8,character_size>;
    std::array<Character,7> characters{};
    std::array<u8,settings_size> settings{};
#if PORT_TITLE == 14
    using Th14Character=std::array<u8,0x5298>;
    using Th14Settings=std::array<u8,0x42c>;
    static void th14_initialize_character(Th14Character&);
    static void th14_initialize_settings(Th14Settings&,Rng& gameplay);
    static i32 th14_insert_score(Th14Character&,u32 difficulty,u32 score,i32 stage,i32 continues,u64 timestamp,float slowdown);
#endif
#if PORT_TITLE == 16
    using Th16Character=std::array<u8,0x5318>;
    std::array<Th16Character,5> th16_characters{};
    std::array<char,9> th16_last_name{{' ',' ',' ',' ',' ',' ',' ',' ',0}};
    static void th16_initialize_character(Th16Character&);
    i32 th16_insert_score(u32 selection,u32 difficulty,u32 score,i32 stage,i32 continues,u64 timestamp,float slowdown,u32 subseason);
    bool th16_score_name(u32 selection,u32 difficulty,u32 rank,const char* name);
#endif
    std::string error;
    void initialize(Rng&);
    bool open(const u8*,u32);
    bool save(std::vector<u8>&)const;
    void read_records(SpellRecords&,ClearRecords&)const;
    void write_records(const SpellRecords&,const ClearRecords&);
    i32 high_score(u32 selection,u32 difficulty)const;
    i32 high_continues(u32 selection,u32 difficulty)const;
    i32 insert_score(u32 selection,u32 difficulty,i32 score,i32 stage,i32 continues,u64 timestamp,float slowdown);
    bool score_name(u32 selection,u32 difficulty,u32 rank,const char* name);
    static u32 checksum(const u8*,u32);
};
}
