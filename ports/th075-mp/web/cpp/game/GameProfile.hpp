#pragma once
#include "CharacterSelectScene.hpp"
#include "RoundRules.hpp"
namespace th075 {
struct ScoreRank {std::array<u8,8> name{};u8 month=0,day=0;std::array<u8,2> reserved{};u32 score=0;};
struct SpellStatistics {u32 highestScore=0;u16 captures=0,attempts=0;u8 perfect=0;};
struct DifficultyProgress {
 std::array<u32,4> counters{};std::array<SpellStatistics,150> cards{};
 u8 completedStages=0;std::array<u8,5> reserved{};std::array<ScoreRank,10> ranking{};
 u8 insert_score(u32 score,u8 month,u8 day);
};
struct CharacterProgress {std::array<DifficultyProgress,4> difficulty;};
class GameProfile {
public:
 std::array<CharacterProgress,15> characters;
 std::array<u8,8> workingName{};std::array<std::array<u32,15>,15> victories{};
 std::array<u32,10> unlockBits{};std::array<u8,256> noise{};
 u8 selectedCharacter=0,selectedDifficulty=0;std::string error;
 GameProfile();bool load(const void*,std::size_t);std::vector<u8> encode()const;std::vector<u8> save(ReplayRandom&);
 void select(u8 character,u8 difficulty){selectedCharacter=character;selectedDifficulty=difficulty;}
 DifficultyProgress& selected(){return characters[selectedCharacter].difficulty[selectedDifficulty];}
 const DifficultyProgress& selected()const{return characters[selectedCharacter].difficulty[selectedDifficulty];}
 void complete_stage(u8);void begin_attempt();bool unlocked(i16)const;void unlock(i16);
 SelectionUnlocks selection_unlocks()const;SelectionProgress selection_progress()const;
 void load_spell_records(RoundRules&)const;void store_spell_records(const RoundRules&);
};
}
