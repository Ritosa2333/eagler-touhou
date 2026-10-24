// Native VM ownership and saved-successor traversal. Simulation-only.
#pragma once
#include "AnmLogic.hpp"
#include <list>
namespace th12::anm_logic {
enum class Membership { Embedded, Primary, Secondary };
struct Node {
  uint32_t id = 0, owner = 0, parent = 0;
  uint64_t key = 0;
  Membership membership = Membership::Primary;
  VM vm;
  bool alive = true, detached = false, customGeometry = false;
  float viewportX = 224, viewportY = 16;
  std::function<int(int)> spriteRemap;
  std::function<bool()> beforeTick;
  std::function<void(int)> onInterrupt;
  // Native VM+10 is an intrusive node: +14/+18 are reused as child-list head
  // or sibling next/previous links. The geometry parent is separate (VM+3c).
  uint32_t childNext = 0, childPrevious = 0;
};
class Scene {
  std::list<std::shared_ptr<Node>> primary_, secondary_, embedded_;
  std::unordered_map<uint64_t, std::shared_ptr<Node>> roots_;
  std::unordered_map<uint32_t, std::shared_ptr<Node>> nodes_;
  Node* executing_ = nullptr;
  uint32_t nextId_ = 1;
  Runtime runtimeFor(Node& node);
  void tickNode(Node& node);
  void traverse(std::list<std::shared_ptr<Node>>& list);

public:
  Registry registry;
  Runtime runtime;
  void reset();
  Node* find(uint64_t key);
  const std::unordered_map<uint32_t, std::shared_ptr<Node>>& nodes() const;
  std::vector<const Node*> chain(Membership membership) const;
  std::vector<const Node*> children(uint32_t id) const;
  Node* bind(uint64_t key, uint32_t owner, int bank, int script,
             Membership membership = Membership::Primary, float x = 0, float y = 0,
             std::function<int(int)> remap = {}, bool head = false,
             const std::array<float, 3>* scriptPosition = nullptr, float viewportX = 224,
             float viewportY = 16, int layer = 0, bool withoutReset = false, float engineZ = 0);
  void interrupt(uint64_t key, int code, bool immediate = false);
  void interruptNode(uint32_t id, int code, bool immediate = false);
  void position(uint64_t key, float x, float y);
  void drawEnabled(uint64_t key, bool enabled);
  void tickEmbedded(uint64_t key);
  void custom(uint64_t key, std::function<bool()> update);
  void customInterrupt(uint64_t key, std::function<void(int)> callback);
  bool rebind(uint64_t key, int bank, int script, bool withoutReset = true);
  void tickPrimary();
  void tickSecondary();
  void remove(uint64_t key);
  void removeOwner(uint32_t owner);
  // Relabel existing Enemy roots/children after its synchronous constructor.
  // No bind/tick runs, and node identity/intrusive ordering stays unchanged.
  bool reassignEnemyOwner(uint32_t previousOwner, uint32_t nextOwner);
  void collect();
};
} // namespace th12::anm_logic
