#pragma once
#include "ScoreFile.hpp"
#include "SpellCatalogData.hpp"
namespace th08 {
i32 spell_difficulty(i32 number) noexcept;
bool is_last_spell(i32 number) noexcept;
// Catk holds both game and practice history. The thirteenth clear record is
// the combined record and is deliberately required by the Last Word rules.
class SpellProgress {
public:
    SpellProgress(SpellRecord* spells,ClearRecord* clears,LastWords& last_words):spells(spells),clears(clears),last_words(last_words){}
    bool encountered(i32 number,i32 shot=12)const noexcept;
    bool last_word_available(i32 number)const noexcept;
    void unlock_last_words();
private:
    SpellRecord* spells;ClearRecord* clears;LastWords& last_words;
    bool captured(i32 number,i32 shot=12)const noexcept;
};
}
