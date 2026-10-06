#include "game/Laser.hpp"
#include "wasm/Api.hpp"
#include <array>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

static bool check(bool condition, const char* message) {
  if (!condition)
    std::fprintf(stderr, "%s\n", message);
  return condition;
}
int main() {
  // Four independently captured values from the immutable native RNG corpus.
  struct RngSample {
    int seed;
    uint32_t next16, next32;
  };
  constexpr std::array<RngSample, 4> samples = {{{0, 50036, 0x30ddeff1u},
                                                 {1, 50040, 0x30deeff5u},
                                                 {1234, 46652, 0x2d8fbab9u},
                                                 {65535, 4592, 0x047c226du}}};
  for (const auto& s : samples)
    if (!check(th12_rng16(s.seed) == s.next16 && th12_rng32(s.seed) == s.next32,
               "Native RNG ABI differs from original samples"))
      return 1;

  // Resource ownership and the same actual game core are exercised without DATs.
  std::vector<uint8_t> ecl(100);
  const auto word = [&](size_t at, uint32_t v) { std::memcpy(ecl.data() + at, &v, 4); };
  const auto half = [&](size_t at, uint16_t v) { std::memcpy(ecl.data() + at, &v, 2); };
  word(0, 0x54504353);
  half(4, 1);
  half(16, 1);
  word(36, 48);
  std::memcpy(ecl.data() + 40, "main", 5);
  word(48, 0x484c4345);
  half(68, 83);
  half(70, 20);
  ecl[74] = 63;
  ecl[75] = 1;
  word(80, 1000);
  half(88, 10);
  half(90, 16);
  ecl[94] = 63;
  if (!check(th12_load_ecl(ecl.data(), int(ecl.size())) == 1, "Native C++ ECL load failed"))
    return 1;
  ecl.clear();
  ecl.shrink_to_fit(); // Program owns its copy, not the caller's buffer.
  th12_start(0, 0, 1, 1234);
  for (int i = 0; i < 30; ++i)
    th12_tick(0, 0);
  const std::string before = th12_state();
  for (int i = 0; i < 20; ++i) {
    th12_draw();
    th12_anm_draw();
    th12_laser_draw();
    th12_hud();
  }
  if (!check(before == th12_state(), "Native draw queries advanced game state"))
    return 1;
  if (!check(th12_hud()[0] == 30 && th12_hud_size() == 41 && th12_anm_draw_stride() == 128 &&
                 th12_laser_draw_stride() == 64,
             "Native ABI dimensions differ"))
    return 1;

  th12::laser::Manager lasers;
  th12::laser::World world;
  th12::laser::Parameters p;
  p.position = {0, 100, 0};
  p.width = 32;
  p.initialLength = 10;
  p.maxLength = 100;
  p.speed = 2;
  p.hold = 20;
  p.grow = 10;
  p.delay = 8;
  p.shrink = 12;
  p.nodeCount = 8;
  for (const auto kind :
       {th12::laser::Kind::Moving, th12::laser::Kind::Timed, th12::laser::Kind::Curve})
    lasers.spawn(kind, p, world);
  if (!check(lasers.size() == 3, "Native laser classes failed to construct"))
    return 1;
  for (int i = 0; i < 5; ++i)
    lasers.tick(world);
  if (!check(lasers.size() == 3, "Native laser lifecycle failed"))
    return 1;
  std::puts("Native C++ ABI, resource ownership, read-only presentation and laser classes passed.");
  return 0;
}
