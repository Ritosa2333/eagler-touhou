#pragma once
#include <memory>
namespace th12::multiplayer {
class WorldState {
  struct Data;
  std::shared_ptr<const Data> data_;
public:
  WorldState();
  void restore() const;
  static void clearPool();
};
}
