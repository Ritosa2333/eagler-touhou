#include "SpellHistory.hpp"
#include "GameState.hpp"
#include "StageFlow.hpp"

namespace th12 {
namespace {
std::array<std::array<spell_system::Record, 7>, 128> history{};
bool enabled() { return !multiplayer::active && fullGameFlow.status != FullGameStatus::Disabled; }
int loadout() { return pilot().character * 2 + pilot().shot; }
bool validCard(int id) { return id >= 0 && id < 128; }
} // namespace
std::string spellHistorySnapshotBuffer;

bool loadSpellHistory(const int32_t* words, int count) {
  if (!words || count != 128 * 7 * 2)
    return false;
  for (int i = 0; i < count; ++i)
    if (words[i] < 0 || words[i] > 99999)
      return false;
  for (int card = 0; card < 128; ++card)
    for (int player = 0; player < 7; ++player) {
      const int at = (card * 7 + player) * 2;
      history[card][player] = {words[at], words[at + 1]};
    }
  return true;
}

void restoreSpellHistory(spell_system::Start& start) {
  if (!enabled() || !validCard(start.cardId))
    return;
  start.loadoutRecord = history[start.cardId][loadout()];
  start.allRecord = history[start.cardId][6];
  start.replay = fullGameFlow.replay;
}

void rememberSpellStart() {
  if (!enabled() || fullGameFlow.replay || !validCard(spellState.cardId))
    return;
  history[spellState.cardId][loadout()] = spellState.loadoutRecord;
  history[spellState.cardId][6] = spellState.allRecord;
}

void rememberSpellResult(const spell_system::Result& result) {
  if (!enabled() || fullGameFlow.replay || !validCard(result.cardId) ||
      result.kind == spell_system::ResultKind::None)
    return;
  history[result.cardId][loadout()] = result.loadoutRecord;
  history[result.cardId][6] = result.allRecord;
}

const std::string& captureSpellHistory() {
  std::ostringstream out;
  out << "{\"records\":[";
  for (int card = 0; card < 128; ++card) {
    if (card)
      out << ',';
    out << '[';
    for (int player = 0; player < 7; ++player) {
      if (player)
        out << ',';
      const auto& r = history[card][player];
      out << '[' << r.attempts << ',' << r.captures << ']';
    }
    out << ']';
  }
  out << "]}";
  spellHistorySnapshotBuffer = out.str();
  return spellHistorySnapshotBuffer;
}
} // namespace th12
