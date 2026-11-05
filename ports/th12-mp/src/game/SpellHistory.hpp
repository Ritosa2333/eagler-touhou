#pragma once
#include "../spell.hpp"
#include <cstdint>
#include <string>

namespace th12 {
bool loadSpellHistory(const int32_t* words, int count);
const std::string& captureSpellHistory();
extern std::string spellHistorySnapshotBuffer;
void restoreSpellHistory(spell_system::Start& start);
void rememberSpellStart();
void rememberSpellResult(const spell_system::Result& result);
} // namespace th12
