#pragma once
#include "ScoreFile.hpp"
namespace th08 {
// The record checksum deliberately sums signed CP932 bytes and all six
// history arrays, then retains only its low byte (004152a0 / 004161b0).
u8 spell_history_checksum(const SpellRecord&)noexcept;
void validate_spell_history(SpellRecord&)noexcept;
void encounter_spell(SpellRecord&,u32 character,bool practice,u8 difficulty)noexcept;
void capture_spell(SpellRecord&,u32 character,bool practice,u8 difficulty,u32 bonus)noexcept;
}
