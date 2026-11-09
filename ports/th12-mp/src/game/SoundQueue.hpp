#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace th12::sound_system {
struct Slot {
  int id = -1, count = 0;
  std::array<int, 128> pans{};
};
struct Event {
  int32_t id = -1, pan = 0, requests = 0, stop = 0;
};
static_assert(sizeof(Event) == 16);
struct Events {
  std::array<Event, 12> values{};
  std::size_t count = 0;
  const Event* begin() const { return values.data(); }
  const Event* end() const { return values.data() + count; }
};
class Queue {
  std::array<Slot, 12> slots_{};

public:
  void reset();
  // Native453e20 positioned,453d90 centered; twelve distinct sounds and
  // 128 pan requests per sound. These calls never consume game/display RNG.
  void play(int id, float worldX = 0, bool positioned = true);
  void stop(int id);
  const auto& slots() const { return slots_; }
  Events captureEvents() const;
  std::vector<Event> events() const;
};
extern Queue queue;
extern std::vector<Event> drawState;
int capture();
} // namespace th12::sound_system
