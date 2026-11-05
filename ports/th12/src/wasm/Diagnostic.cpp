// No diagnostic ABI or partial-world setters exist in a player build.
#if defined(TH12_DIAGNOSTIC) && TH12_DIAGNOSTIC
#include "Api.hpp"
#include "../game/GameState.hpp"
#include "../game/GameSession.hpp"
extern "C" {
int th12_diagnostic_build() { return 1; }
const char* th12_diagnostic_state() { return th12::captureState().c_str(); }
int th12_diagnostic_state_size() { return int(th12::snapshotBuffer.size()); }
void th12_oracle_fixture(int flags) { th12::oracleFixtureFlags = flags & 15; }
void th12_set_initial(int x, int y, int power, int lives, int bombs, int score) {
  th12::setInitial(x, y, power, lives, bombs, score);
}
void th12_set_replay_economy(int piv, int lifeFragments, int bombFragments, int red, int blue,
                           int green, int rank) {
  th12::setReplayEconomy(piv, lifeFragments, bombFragments, red, blue, green, rank);
}
const uint32_t* th12_unsupported() { return th12::unsupported.data(); }
uint32_t th12_rng16(int seed) {
  th12::Random random{uint16_t(seed), 0};
  return random.next16();
}
uint32_t th12_rng32(int seed) {
  th12::Random random{uint16_t(seed), 0};
  return random.next32();
}
}
#endif
