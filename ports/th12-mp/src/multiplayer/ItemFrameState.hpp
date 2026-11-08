#pragma once
#include "../game/Items.hpp"
#include <algorithm>
#include <cstring>
#include <memory>
#include <type_traits>

namespace th12::multiplayer {
// Keep inactive slot bytes too: token generations and slot reuse survive rollback.
class ItemFrameState {
  static constexpr size_t pageSize = 256;
  static constexpr size_t pageCount = (item_system::Manager::totalCapacity + pageSize - 1) / pageSize;
  using Page = std::array<item_system::Entity, pageSize>;
  std::array<std::shared_ptr<Page>, pageCount> pages;
public:
  using Cache = std::array<std::shared_ptr<Page>, pageCount>;
  void capture(const item_system::Manager& source, Cache& cache) {
    static_assert(std::is_trivially_copyable_v<item_system::Entity>);
    for (size_t i = 0; i < pageCount; ++i) {
      const auto start = i * pageSize;
      const auto bytes = std::min(pageSize, source.entities.size() - start) * sizeof(item_system::Entity);
      auto spare = std::move(pages[i]);
      auto& current = cache[i];
      if (!current || std::memcmp(current->data(), source.entities.data() + start, bytes)) {
        if (spare == current) spare.reset();
        if (!current || current.use_count() != 1) {
          if (spare && spare.use_count() == 1) current = std::move(spare);
          else current = std::make_shared<Page>();
        }
        std::memcpy(current->data(), source.entities.data() + start, bytes);
      }
      pages[i] = current;
    }
  }
  void restore(item_system::Manager& target) const {
    for (size_t i = 0; i < pageCount; ++i) {
      const auto start = i * pageSize;
      const auto bytes = std::min(pageSize, target.entities.size() - start) * sizeof(item_system::Entity);
      std::memcpy(target.entities.data() + start, pages[i]->data(), bytes);
    }
  }
};
}
