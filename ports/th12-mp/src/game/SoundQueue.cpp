#include "SoundQueue.hpp"
#include <cmath>

namespace th12::sound_system {
Queue queue;
std::vector<Event> drawState;
void Queue::reset() {
  slots_ = {};
}
void Queue::play(int id, float worldX, bool positioned) {
  if (id < 0 || id >= 60 || !std::isfinite(worldX))
    return;
  const int pan = positioned ? int(double(worldX) * 1000.0 / 192.0) : 0;
  for (auto& slot : slots_) {
    if (slot.id == id || slot.id < 0) {
      if (slot.id < 0)
        slot.id = id;
      if (slot.count >= 128)
        return;
      // A play following a queued stop increments -1 to0 in453e20. Its
      // write lies before the active pan range and does not enter the mean.
      if (slot.count >= 0)
        slot.pans[size_t(slot.count)] = pan;
      ++slot.count;
      return;
    }
  }
}
void Queue::stop(int id) {
  if (id < 0 || id >= 60)
    return;
  for (auto& slot : slots_)
    if (slot.id == id || slot.id < 0) {
      slot.id = id;
      slot.count = -1;
      return;
    }
}
std::vector<Event> Queue::events() const {
  std::vector<Event> result;
  for (const auto& slot : slots_) {
    if (slot.id < 0)
      break;
    int sum = 0;
    for (int i = 0; i < slot.count; ++i)
      sum += slot.pans[size_t(i)];
    result.push_back(
        {slot.id, slot.count > 0 ? sum / slot.count : 0, slot.count, slot.count < 0 ? 1 : 0});
  }
  return result;
}
int capture() {
  drawState = queue.events();
  return int(drawState.size());
}
} // namespace th12::sound_system
