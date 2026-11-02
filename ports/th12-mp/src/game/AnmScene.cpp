// Native registered VM ownership and saved-successor traversal.
#include "AnmScene.hpp"
#include "Multiplayer.hpp"
#include "PresentationTracking.hpp"
#include <algorithm>

namespace th12::anm_logic {
Scene::Scene(const Scene& other) { *this = other; }
Scene::Snapshot Scene::snapshot() const {
  Snapshot saved;
  snapshot(saved);
  return saved;
}
void Scene::snapshot(Snapshot& saved) const {
  saved.registry = registry; saved.runtime = runtime; saved.nextId = nextId_;
  saved.nodes.resize(nodes_.size());
  size_t index = 0;
  for (const auto& [id, node] : nodes_) saved.nodes[index++] = *node;
  saved.roots.clear();
  saved.roots.reserve(roots_.size());
  for (const auto& [key, node] : roots_) saved.roots.emplace_back(key, node->id);
  const std::array<const std::list<std::shared_ptr<Node>>*, 3> lists{&primary_, &secondary_, &embedded_};
  for (size_t i = 0; i < lists.size(); ++i) {
    saved.chains[i].clear();
    saved.chains[i].reserve(lists[i]->size());
    for (const auto& node : *lists[i]) saved.chains[i].push_back(node->id);
  }
}
void Scene::restore(const Snapshot& saved) {
  registry = saved.registry; runtime = saved.runtime; nextId_ = saved.nextId; executing_ = nullptr;
  // Most IDs survive a short rewind. Retain their VM/vector storage instead
  // of destroying the whole graph, while restoring every semantic value.
  for (const auto& node : saved.nodes) {
    const auto at = nodes_.find(node.id);
    if (at == nodes_.end()) nodes_.emplace(node.id, std::make_shared<Node>(node));
    else *at->second = node;
  }
  if (nodes_.size() > saved.nodes.size()) {
    restoreNodeIds_.clear(); restoreNodeIds_.reserve(saved.nodes.size());
    for (const auto& node : saved.nodes) restoreNodeIds_.push_back(node.id);
    std::sort(restoreNodeIds_.begin(), restoreNodeIds_.end());
    for (auto at = nodes_.begin(); at != nodes_.end();)
      if (!std::binary_search(restoreNodeIds_.begin(), restoreNodeIds_.end(), at->first)) at = nodes_.erase(at);
      else ++at;
  }
  for (const auto& [key, id] : saved.roots) roots_.insert_or_assign(key, nodes_.at(id));
  if (roots_.size() > saved.roots.size()) {
    restoreRootKeys_.clear(); restoreRootKeys_.reserve(saved.roots.size());
    for (const auto& [key, id] : saved.roots) restoreRootKeys_.push_back(key);
    std::sort(restoreRootKeys_.begin(), restoreRootKeys_.end());
    for (auto at = roots_.begin(); at != roots_.end();)
      if (!std::binary_search(restoreRootKeys_.begin(), restoreRootKeys_.end(), at->first)) at = roots_.erase(at);
      else ++at;
  }
  const std::array<std::list<std::shared_ptr<Node>>*, 3> lists{&primary_, &secondary_, &embedded_};
  for (size_t i = 0; i < lists.size(); ++i) {
    auto& list = *lists[i]; auto at = list.begin();
    for (const auto id : saved.chains[i]) {
      if (at == list.end()) list.push_back(nodes_.at(id));
      else *at++ = nodes_.at(id);
    }
    list.erase(at, list.end());
  }
}
Scene& Scene::operator=(const Scene& other) {
  if (this == &other) return *this;
  primary_.clear(); secondary_.clear(); embedded_.clear(); roots_.clear(); nodes_.clear();
  registry = other.registry; runtime = other.runtime; nextId_ = other.nextId_; executing_ = nullptr;
  for (const auto& [id, node] : other.nodes_) nodes_[id] = std::make_shared<Node>(*node);
  for (const auto& [key, node] : other.roots_) roots_[key] = nodes_.at(node->id);
  for (const auto& node : other.primary_) primary_.push_back(nodes_.at(node->id));
  for (const auto& node : other.secondary_) secondary_.push_back(nodes_.at(node->id));
  for (const auto& node : other.embedded_) embedded_.push_back(nodes_.at(node->id));
  return *this;
}

Runtime Scene::runtimeFor(Node& node) {
  Runtime result = runtime;
  result.remapSprite = node.spriteRemap;
  result.spawnChild = [this, &node](const ChildRequest& request) {
    auto child = std::make_shared<Node>();
    child->id = nextId_++;
    child->seat=node.seat;
    child->owner = request.detached ? 0 : node.owner;
    child->parent = request.detached ? 0 : node.id;
    child->detached = request.detached;
    child->viewportX = node.viewportX;
    child->viewportY = node.viewportY;
    child->membership =
        (request.registrationFlags & 4) ? Membership::Secondary : Membership::Primary;
    auto& state = child->vm.control();
    state.engineX = request.x;
    state.engineY = request.y;
    state.engineZ = request.z;
    state.layer = node.vm.state().layer;
    nodes_[child->id] = child;
    auto childRuntime = runtimeFor(*child);
    if (!child->vm.bind(registry, request.bank, request.script, childRuntime)) {
      child->alive = false;
      return;
    }
    if (request.detached && request.parent) {
      auto& pose = child->vm.control();
      const auto& parent = *request.parent;
      // Native461840 copies these AFTER the child's first tick. Its engine
      // is preserved separately, and op97 later replaces only extra XY.
      pose.offsetX = parent.x;
      pose.offsetY = parent.y;
      pose.offsetZ = parent.z;
      pose.rx = parent.rx;
      pose.ry = parent.ry;
      pose.rotation = parent.rotation;
    }
    // The native factory returns before opcode96/97 resolves its parent's
    // masked operands. Returning this pointer preserves child RNG first.
    request.createdState = &child->vm.control();
    auto& list = child->membership == Membership::Secondary ? secondary_ : primary_;
    if (request.registrationFlags & 2)
      list.push_front(child);
    else
      list.push_back(child);
    if (!request.detached) {
      // 4617f4..461810 only overwrites child.next when the old head exists.
      // Birth-time grandchildren can therefore remain on the same physical
      // chain. Do not replace this with recursive tree traversal.
      if (node.childNext) {
        child->childNext = node.childNext;
        nodes_.at(node.childNext)->childPrevious = child->id;
      }
      node.childNext = child->id;
      child->childPrevious = node.id;
    }
  };
  return result;
}

void Scene::tickNode(Node& node) {
  multiplayer::Scope owner(node.seat);
  if (!node.alive)
    return;
  if (node.beforeTick && node.beforeTick()) {
    node.alive = false;
    return;
  }
  auto local = runtimeFor(node);
  const auto presentationRevision = node.vm.presentationRevision();
  const bool ended = node.vm.tick(local);
  if (node.owner && presentationRevision != node.vm.presentationRevision())
    presentation::discontinuity(node.owner);
  if (node.vm.state().deleteRequested || (ended && node.membership != Membership::Embedded))
    node.alive = false;
}

void Scene::traverse(std::list<std::shared_ptr<Node>>& list) {
  for (auto at = list.begin(); at != list.end();) {
    const auto current = *at;
    const auto next = std::next(at);
    tickNode(*current);
    at = next;
  }
}

void Scene::reset() {
  presentation::reset();
  primary_.clear();
  secondary_.clear();
  embedded_.clear();
  roots_.clear();
  nodes_.clear();
  nextId_ = 1;
}

Node* Scene::find(uint64_t key) {
  auto at = roots_.find(key);
  return at == roots_.end() || !at->second->alive ? nullptr : at->second.get();
}

bool Scene::reassignEnemyOwner(uint32_t previousOwner, uint32_t nextOwner) {
  if (previousOwner == nextOwner)
    return true;
  std::vector<std::pair<uint64_t, std::shared_ptr<Node>>> moving;
  for (const auto& [key, node] : roots_)
    if ((key >> 8) == previousOwner && (key & 255) < 16 && node->owner == previousOwner) {
      const uint64_t nextKey = (uint64_t(nextOwner) << 8) | (key & 255);
      if (auto found = roots_.find(nextKey);
          found != roots_.end() && found->second != node && found->second->alive)
        return false;
      moving.emplace_back(key, node);
    }
  for (const auto& [oldKey, node] : moving) {
    const uint64_t nextKey = (uint64_t(nextOwner) << 8) | (oldKey & 255);
    roots_.erase(oldKey);
    roots_[nextKey] = node;
    node->key = nextKey;
  }
  for (auto& [id, node] : nodes_)
    if (node->owner == previousOwner)
      node->owner = nextOwner;
  return true;
}

const std::unordered_map<uint32_t, std::shared_ptr<Node>>& Scene::nodes() const {
  return nodes_;
}

std::vector<const Node*> Scene::chain(Membership membership) const {
  std::vector<const Node*> result;
  appendChain(membership, result);
  return result;
}

void Scene::appendChain(Membership membership, std::vector<const Node*>& output) const {
  const auto& list = membership == Membership::Primary     ? primary_
                     : membership == Membership::Secondary ? secondary_
                                                           : embedded_;
  for (const auto& node : list)
    if (node->alive)
      output.push_back(node.get());
}

std::vector<const Node*> Scene::children(uint32_t id) const {
  std::vector<const Node*> result;
  auto at = nodes_.find(id);
  if (at == nodes_.end())
    return result;
  uint32_t next = at->second->childNext;
  for (size_t budget = 0; next && budget < nodes_.size(); ++budget) {
    at = nodes_.find(next);
    if (at == nodes_.end())
      break;
    if (at->second->alive)
      result.push_back(at->second.get());
    next = at->second->childNext;
  }
  return result;
}

Node* Scene::bind(uint64_t key, uint32_t owner, int bank, int script, Membership membership,
                  float x, float y, std::function<int(int)> remap, bool head,
                  const std::array<float, 3>* scriptPosition, float viewportX, float viewportY,
                  int layer, bool withoutReset, float engineZ) {
  if (!registry.bank(bank))
    return nullptr;
  if (owner) presentation::discontinuity(owner);
  if (auto old = roots_.find(key); old != roots_.end())
    old->second->alive = false;
  auto node = std::make_shared<Node>();
  node->id = nextId_++;
  node->key = key;
  node->owner = owner;
  node->membership = membership;
  node->seat = multiplayer::currentSeat;
  node->spriteRemap = std::move(remap);
  node->viewportX = viewportX;
  node->viewportY = viewportY;
  node->vm.control().engineX = x;
  node->vm.control().engineY = y;
  node->vm.control().engineZ = engineZ;
  node->vm.control().layer = layer;
  nodes_[node->id] = node;
  roots_[key] = node;
  auto local = runtimeFor(*node);
  const bool bound = withoutReset ? node->vm.rebindWithoutReset(registry, bank, script, local)
                                  : node->vm.bind(registry, bank, script, local, scriptPosition);
  if (!bound) {
    node->alive = false;
    return nullptr;
  }
  auto& list = membership == Membership::Primary     ? primary_
               : membership == Membership::Secondary ? secondary_
                                                     : embedded_;
  if (head)
    list.push_front(node);
  else
    list.push_back(node);
  return node.get();
}

void Scene::interrupt(uint64_t key, int code, bool immediate) {
  auto* node = find(key);
  if (!node)
    return;
  interruptNode(node->id, code, immediate);
}

void Scene::interruptNode(uint32_t id, int code, bool immediate) {
  auto at = nodes_.find(id);
  if (at == nodes_.end() || !at->second->alive)
    return;
  // Keep the target alive across callbacks, including a host-triggered
  // collect. Native registered objects are owned independently of handles.
  const auto keepAlive = at->second;
  auto* node = keepAlive.get();
  const auto deliver = [&](Node& target) {
    if (target.owner) presentation::discontinuity(target.owner);
    if (target.onInterrupt)
      target.onInterrupt(int16_t(code));
    target.vm.interrupt(code);
    if (immediate)
      tickNode(target);
  };
  deliver(*node);
  // Native461970/4619e0 tests VM+18 after root delivery. An attached child's
  // nonzero child-list link suppresses descendant propagation. A root's
  // immediate tick may create children; fetch its list AFTER that tick.
  if (node->childPrevious)
    return;
  uint32_t next = node->childNext;
  for (size_t budget = 0; next && budget < nodes_.size(); ++budget) {
    const auto found = nodes_.find(next);
    if (found == nodes_.end())
      break;
    const auto child = found->second;
    if (child->alive)
      deliver(*child);
    // Native reads successor after each child's immediate tick/callback.
    next = child->childNext;
  }
}

void Scene::position(uint64_t key, float x, float y) {
  if (auto* node = find(key)) {
    auto& state = node->vm.control();
    state.engineX = x;
    state.engineY = y;
  }
}
void Scene::drawEnabled(uint64_t key, bool enabled) {
  auto* root = find(key);
  if (!root)
    return;
  const auto set = [enabled](Node& node) {
    if (enabled)
      node.vm.control().flags |= 2u;
    else
      node.vm.control().flags &= ~2u;
  };
  set(*root);
  // 461d40/461d80 follow the intrusive child chain only when+18 is null.
  if (root->childPrevious)
    return;
  for (const auto* child : children(root->id))
    set(*nodes_.at(child->id));
}

void Scene::tickEmbedded(uint64_t key) {
  if (auto* node = find(key))
    tickNode(*node);
}

void Scene::custom(uint64_t key, std::function<bool()> update) {
  if (auto* node = find(key))
    node->beforeTick = std::move(update);
}

void Scene::customInterrupt(uint64_t key, std::function<void(int)> callback) {
  if (auto* node = find(key))
    node->onInterrupt = std::move(callback);
}

bool Scene::rebind(uint64_t key, int bank, int script, bool withoutReset) {
  if (auto* node = find(key)) {
    if (node->owner) presentation::discontinuity(node->owner);
    auto local = runtimeFor(*node);
    return withoutReset ? node->vm.rebindWithoutReset(registry, bank, script, local)
                        : node->vm.bind(registry, bank, script, local);
  }
  return false;
}

void Scene::tickPrimary() {
  traverse(primary_);
}

void Scene::tickSecondary() {
  traverse(secondary_);
}

void Scene::remove(uint64_t key) {
  if (auto* node = find(key))
    node->alive = false;
}

void Scene::removeOwner(uint32_t owner) {
  for (auto& [id, node] : nodes_)
    if (node->owner == owner && !node->detached)
      node->alive = false;
}

void Scene::collect() {
  // Parent retirement also retires attached descendants before the next pass.
  // Detached children have no parent and retain independent ownership.
  bool changed = true;
  while (changed) {
    changed = false;
    for (auto& [id, node] : nodes_)
      if (node->alive && node->parent) {
        auto parent = nodes_.find(node->parent);
        if (parent == nodes_.end() || !parent->second->alive) {
          node->alive = false;
          changed = true;
        }
      }
  }
  for (auto& [id, node] : nodes_)
    if (node->alive) {
      for (auto* link : {&node->childNext, &node->childPrevious}) {
        for (size_t budget = 0; *link && budget < nodes_.size(); ++budget) {
          auto at = nodes_.find(*link);
          if (at == nodes_.end()) {
            *link = 0;
            break;
          }
          if (at->second->alive)
            break;
          *link = link == &node->childNext ? at->second->childNext : at->second->childPrevious;
        }
      }
    }
  auto inactive = [](const auto& node) { return !node->alive; };
  primary_.remove_if(inactive);
  secondary_.remove_if(inactive);
  embedded_.remove_if(inactive);
  for (auto at = roots_.begin(); at != roots_.end();)
    if (!at->second->alive)
      at = roots_.erase(at);
    else
      ++at;
  for (auto at = nodes_.begin(); at != nodes_.end();)
    if (!at->second->alive)
      at = nodes_.erase(at);
    else
      ++at;
}
} // namespace th12::anm_logic
