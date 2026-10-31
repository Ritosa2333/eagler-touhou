#include "../../src/game/AnmLogic.hpp"
#include "../../src/game/GameTypes.hpp"
#include <iomanip>
#include <sstream>

namespace {
th12::anm_logic::Registry registry;
th12::anm_logic::Runtime runtime;
th12::anm_logic::VM machine;
th12::Random game, display;
unsigned unsupported = 0;
std::string snapshot;
} // namespace
extern "C" {
int irq_load(const uint8_t* bytes, int length) {
  std::string error;
  return registry.loadBank(0, bytes, length, error);
}
int irq_setup(float delta, int fixedRate) {
  machine = {};
  game = {0x1234, 0};
  display = {0xbeef, 0};
  unsupported = 0;
  runtime = {};
  runtime.delta = delta;
  runtime.gameRandom32 = [] { return game.next32(); };
  runtime.displayRandom32 = [] { return display.next32(); };
  runtime.unsupported = [](int, int, int, const char*) { ++unsupported; };
  const int bound = machine.bind(registry, 0, 0, runtime);
  if (fixedRate)
    machine.control().flags |= 0x80000000u;
  return bound;
}
void irq_tick(int interrupt, int paused) {
  auto& state = machine.control();
  state.flags = paused ? state.flags | 0x40000u : state.flags & ~0x40000u;
  if (interrupt)
    machine.interrupt(interrupt);
  machine.tick(runtime);
}
const char* irq_snapshot() {
  const auto& s = machine.state();
  const auto d = machine.diagnostics();
  std::ostringstream out;
  out << std::setprecision(9) << "{\"clock\":" << s.clock << ",\"time\":" << d.time
      << ",\"pc\":" << d.byteOffset << ",\"savedPC\":";
  if (d.hasSavedTimer)
    out << d.savedByteOffset;
  else
    out << "null";
  out << ",\"savedTime\":" << d.savedTimer << ",\"pendingInterrupt\":" << d.pendingInterrupt
      << ",\"flags\":" << s.flags << ",\"scale\":[" << s.sx << ',' << s.sy << "],\"rotation\":["
      << s.rx << ',' << s.ry << ',' << s.rotation << "],\"uv\":[" << s.u << ',' << s.v
      << "],\"float0\":" << s.floats[0] << ",\"gameRate\":" << runtime.delta
      << ",\"gameSeedCounter\":[" << game.seed << ',' << game.calls << "],\"displaySeedCounter\":["
      << display.seed << ',' << display.calls << "],\"unsupported\":" << unsupported << '}';
  snapshot = out.str();
  return snapshot.c_str();
}
}
