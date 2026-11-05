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
    using Character=std::array<u8,0x68d4>;
    std::array<Character,7> characters{};
    std::array<u8,0x448> settings{};
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
