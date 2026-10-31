#include "game/GameState.hpp"
#include "game/GameSession.hpp"
#include "game/Multiplayer.hpp"
#include "game/StageFlow.hpp"
#include "multiplayer/WorldState.hpp"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>
#include <optional>
using namespace th12;
namespace mp = th12::multiplayer;
namespace {
bool counting = false;
size_t allocations = 0;
uint64_t digest = 1469598103934665603ull;
void require(bool ok, const char* message) {
  if (!ok) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}
void hash(uint32_t value) {
  for (int i = 0; i < 4; ++i) { digest ^= uint8_t(value >> (8 * i)); digest *= 1099511628211ull; }
}
void word(std::vector<uint8_t>& bytes, size_t at, uint32_t value) {
  for (int i = 0; i < 4; ++i) bytes[at + i] = uint8_t(value >> (8 * i));
}
std::vector<uint8_t> bank(float width) {
  std::vector<uint8_t> bytes(88);
  word(bytes, 0, 7); bytes[4] = 1; word(bytes, 64, 68);
  uint32_t bits; std::memcpy(&bits, &width, 4); word(bytes, 80, bits); word(bytes, 84, bits);
  return bytes;
}
void load(anm_logic::Registry& registry, int id, float width) {
  const auto bytes = bank(width); std::string error;
  require(registry.loadBank(id, bytes.data(), bytes.size(), error), "fixture ANM bank");
}
void fill(unsigned count) {
  enemies.clear();
  for (unsigned i = 0; i < count; ++i) {
    auto enemy = std::make_unique<Enemy>();
    enemy->id = 1000 + i; enemy->x = float(i); enemy->y = 120; enemy->life = int(i + 50);
    enemy->variables[3] = i; enemy->shooters[2].count = int(i + 2);
    enemy->interrupts[1].healthScript = "persistent-health-script-" + std::to_string(i);
    for (unsigned c = 0; c < 3; ++c) {
      enemy->contexts.emplace_back(); auto& context = enemy->contexts.back();
      context.id = int(c); context.time = float(i + c); context.locals[17] = i * 5 + c;
      context.stack.resize(12); context.stack[4] = {i + c, true};
      context.calls.resize(2); context.calls[1].locals[23] = i * 7 + c;
    }
    enemies.push_back(std::move(enemy));
  }
}
void start(unsigned seats) {
  selectStage(1); mp::Options options; options.count = seats; options.selections = {0, 3, 5};
  require(mp::begin(options), "start world");
  for (int id = 0; id < 12; ++id) load(animationScene.registry, id, float(16 + id));
}
void verifyWorld(unsigned seats, bool reference) {
  start(seats); fill(64);
  mp::WorldState full; const auto expected = mp::canonicalHash(); hash(expected);
  Enemy* first = enemies.front().get();
  const auto* stack = enemies.front()->contexts.front().stack.data();
  const auto* calls = enemies.front()->contexts.front().calls.data();
  enemies.front()->life = -1; enemies.front()->contexts.front().stack.clear();
  enemies.front()->contexts.front().calls.clear(); th12::random.seed ^= 0x1234;
  animationScene.registry.unloadBank(7); load(animationScene.registry, 8, 99);
  full.restore(); require(mp::canonicalHash() == expected, "restore all world fields and registry");
  require(animationScene.registry.bank(7) && animationScene.registry.bank(8)->sprites[0].width == 24,
          "checkpoint bank bindings survive later unload and replacement");
  if (!reference) {
    require(enemies.front().get() == first, "restore reuses the live enemy object");
    require(enemies.front()->contexts.front().stack.data() == stack, "restore reuses ECL stack storage");
    require(enemies.front()->contexts.front().calls.data() == calls, "restore reuses ECL call storage");
  }
  // A shared checkpoint cannot enter the free pool until its last owner leaves.
  mp::WorldState shared = full;
  full = mp::WorldState();
  fill(8); mp::WorldState small; const auto smallHash = mp::canonicalHash(); hash(smallHash);
  fill(0); mp::WorldState empty; const auto emptyHash = mp::canonicalHash(); hash(emptyHash);
  for (int round = 0; round < 20; ++round) {
    shared.restore(); require(enemies.size() == 64 && mp::canonicalHash() == expected, "grow and restore retained checkpoint");
    small.restore(); require(enemies.size() == 8 && mp::canonicalHash() == smallHash, "shrink without restoring spare snapshot tail");
    empty.restore(); require(enemies.empty() && mp::canonicalHash() == emptyHash, "restore empty enemy list");
    // Reused snapshot slots alternate between many/few enemies and contexts.
    { mp::WorldState recycle; }
  }
  shared.restore();
  for (auto& enemy : enemies) enemy->contexts.clear();
  full = mp::WorldState(); const auto noContexts = mp::canonicalHash(); hash(noContexts);
  for (int i = 0; i < 20; ++i) { full.restore(); shared.restore(); }
  require(mp::canonicalHash() == expected, "context count changes preserve values and order");
  full.restore(); require(mp::canonicalHash() == noContexts, "empty contexts remain empty");
  mp::WorldState::clearPool(); shared.restore();
  require(mp::canonicalHash() == expected, "live checkpoint survives pool clear");
}
}
void* operator new(size_t size) {
  if (counting) ++allocations;
  if (auto* value = std::malloc(size ? size : 1)) return value;
  throw std::bad_alloc();
}
void* operator new[](size_t size) { return ::operator new(size); }
void operator delete(void* value) noexcept { std::free(value); }
void operator delete[](void* value) noexcept { std::free(value); }
void operator delete(void* value, size_t) noexcept { std::free(value); }
void operator delete[](void* value, size_t) noexcept { std::free(value); }
int main(int argc, char** argv) {
  const bool reference = argc > 1 && !std::strcmp(argv[1], "--reference");
  verifyWorld(2, reference); verifyWorld(3, reference);
  anm_logic::Registry original; for (int i = 0; i < 12; ++i) load(original, i, float(i + 10));
  auto retained = original; original.unloadBank(3); load(original, 4, 77);
  require(retained.bank(3) && retained.bank(4)->sprites[0].width == 14, "registry copy is isolated on resource mutation");
  std::string error; require(!original.loadBank(4, nullptr, 0, error), "invalid replacement rejected");
  require(original.bank(4)->sprites[0].width == 77, "failed replacement preserves the current bank");
  auto moved = std::move(original); require(!original.bank(999), "moved-from registry remains readable");
  original.unloadBank(999); load(original, 4, 33);
  require(moved.bank(4)->sprites[0].width == 77, "moved-from registry can be reused independently");
  std::vector<anm_logic::Registry> copies; copies.reserve(1000);
  allocations = 0; counting = true;
  for (int i = 0; i < 1000; ++i) copies.push_back(retained);
  counting = false; const auto registryAllocations = allocations;
  if (!reference) require(!registryAllocations, "bank registry checkpoint copies do not allocate");
  start(3); fill(32); mp::WorldState checkpoint;
  for (int i = 0; i < 16; ++i) { mp::WorldState warm; checkpoint.restore(); }
  allocations = 0; counting = true;
  const auto begin = std::chrono::steady_clock::now();
  for (int i = 0; i < 1000; ++i) checkpoint.restore();
  const auto restoreMs = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - begin).count();
  counting = false; const auto restoreAllocations = allocations;
  if (!reference) require(!restoreAllocations, "steady restores reuse enemy and script storage");
  allocations = 0; counting = true;
  const auto captureBegin = std::chrono::steady_clock::now();
  for (int i = 0; i < 1000; ++i) { mp::WorldState saved; }
  const auto captureMs = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - captureBegin).count();
  counting = false; hash(mp::canonicalHash());
  std::printf("{\"frames\":1000,\"enemies\":32,\"contextsPerEnemy\":3,\"registryCopies\":1000,\"registryAllocations\":%zu,\"restoreMs\":%.3f,\"restoreAllocations\":%zu,\"captureMs\":%.3f,\"captureAllocations\":%zu,\"outputHash\":\"%016llx\"}\n",
               registryAllocations, restoreMs, restoreAllocations, captureMs, allocations, (unsigned long long)digest);
}
