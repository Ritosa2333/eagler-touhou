// Native registered VM ownership and saved-successor traversal.
#include "AnmScene.hpp"

namespace th12::anm_logic {

Runtime Scene::runtimeFor(Node& node) {
  Runtime result = runtime;
  result.remapSprite = node.spriteRemap;
  result.spawnChild = [this, &node](const ChildRequest& request) {
    auto child = std::make_shared<Node>();
    child->id = nextId_++;
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
  if (!node.alive)
    return;
  if (node.beforeTick && node.beforeTick()) {
    node.alive = false;
    return;
  }
  auto local = runtimeFor(node);
  const bool ended = node.vm.tick(local);
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

const std::unordered_map<uint32_t, std::shared_ptr<Node>>& Scene::nodes() const {
  return nodes_;
}

std::vector<const Node*> Scene::chain(Membership membership) const {
  const auto& list = membership == Membership::Primary     ? primary_
                     : membership == Membership::Secondary ? secondary_
                                                           : embedded_;
  std::vector<const Node*> result;
  for (const auto& node : list)
    if (node->alive)
      result.push_back(node.get());
  return result;
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
                  int layer, bool withoutReset) {
  if (!registry.bank(bank))
    return nullptr;
  if (auto old = roots_.find(key); old != roots_.end())
    old->second->alive = false;
  auto node = std::make_shared<Node>();
  node->id = nextId_++;
  node->key = key;
  node->owner = owner;
  node->membership = membership;
  node->spriteRemap = std::move(remap);
  node->viewportX = viewportX;
  node->viewportY = viewportY;
  node->vm.control().engineX = x;
  node->vm.control().engineY = y;
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
