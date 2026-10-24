#pragma once
#include "AnmScene.hpp"
#include "Items.hpp"
#include "Ufo.hpp"
#include <unordered_map>

namespace th12 {

// Simulation-side ANM ownership. Drawing reads Scene poses and never advances
// these VMs. Event order follows Item22 and UFO19's original native callbacks.
class ItemPresentation {
  struct ItemBinding {
    uint64_t body = 0, arrow = 0, hint = 0;
    int script = 0, lastVisualAge = 0;
    uint32_t interruptVersion = 0, hintGeneration = 0;
    bool aggregate = false, bodyReleased = false;
  };
  std::unordered_map<uint32_t, ItemBinding> items_;
  std::array<uint64_t, 3> stock_{};
  std::unordered_map<uint32_t, uint64_t> overlays_;
  uint64_t ring_ = 0, panel_ = 0, health_ = 0;
  uint32_t serial_ = 1;
  int lastStockFrame_ = -1;
  ufo_system::VisualEvent lastUfoPose_{};
  bool hasUfoPose_ = false;
  uint64_t transientKey();
  anm_logic::Node* bindScreen(anm_logic::Scene&, int bank, int script);
  ItemBinding* ensureItem(anm_logic::Scene&, const item_system::Entity&);
  void updateItemPose(anm_logic::Scene&, ItemBinding&, const item_system::Entity&);
  void releaseItem(anm_logic::Scene&, ItemBinding&);
  void inventoryEvent(anm_logic::Scene&, const ufo_system::InventoryHudEvent&);
  void visualEvent(anm_logic::Scene&, const ufo_system::VisualEvent&);
  void updateUfoPose(anm_logic::Scene&, const ufo_system::Manager&);

public:
  static constexpr uint32_t StockOwner = 0xfffe0000u;
  void reset(anm_logic::Scene&, const ufo_system::Inventory&);
  void consume(anm_logic::Scene&, ufo_system::Manager&);
  void plan(anm_logic::Scene&, const ufo_system::RewardPlan&);
  void event(anm_logic::Scene&, const item_system::Event&);
  void itemTail(anm_logic::Scene&, const item_system::Entity&);
  void sync(anm_logic::Scene&, const item_system::Manager&, const ufo_system::Manager&, int frame);
};

extern ItemPresentation itemPresentation;
void resetItemPresentation();
void consumeUfoPresentation();
void itemPresentationPlan(const ufo_system::RewardPlan&);
void itemPresentationEvent(const item_system::Event&);
void itemPresentationTick(const item_system::Entity&);
void syncItemPresentation();
} // namespace th12
