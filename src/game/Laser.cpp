#include "Laser.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace th12::laser {
namespace {
constexpr float pi = 3.1415927410125732f, tau = 6.2831854820251465f;
float fadd(float a, float b) {
  return float(double(a) + b);
}
float fmul(float a, float b) {
  return float(double(a) * b);
}
float angleNormalize(float a) {
  int wraps = 0;
  while (a > pi && wraps++ <= 33)
    a = float(double(a) - tau);
  while (a < -pi && wraps++ <= 33)
    a = float(double(a) + tau);
  return a;
}
Vec3 direction(float angle, float distance) {
  return {float(std::cos(double(angle)) * distance), float(std::sin(double(angle)) * distance), 0};
}
Vec3 add(Vec3 a, Vec3 b) {
  return {fadd(a.x, b.x), fadd(a.y, b.y), fadd(a.z, b.z)};
}
Vec3 mul(Vec3 a, float b) {
  return {fmul(a.x, b), fmul(a.y, b), fmul(a.z, b)};
}
bool outside(Vec3 p, float mx, float my) {
  return double(p.x) + mx <= -192 || double(p.x) - mx >= 192 || double(p.y) + my <= 0 ||
         double(p.y) - my >= 448;
}
bool insideRect(Vec3 p, Vec3 c, Vec3 dimensions) {
  const float x1 = float(double(c.x) - dimensions.x / 2.),
              x2 = float(double(c.x) + dimensions.x / 2.),
              y1 = float(double(c.y) - dimensions.y / 2.),
              y2 = float(double(c.y) + dimensions.y / 2.);
  return p.x >= x1 && p.x <= x2 && p.y >= y1 && p.y <= y2;
}
bool insideCircle(Vec3 p, Vec3 c, float r) {
  const float dx = float(double(c.x) - p.x), dy = float(double(c.y) - p.y);
  const float sq = float(double(dx) * dx + double(dy) * dy), rr = float(double(r) * r);
  return sq < rr;
}
AnimationGeometry geometry(const State& s) {
  return {s.position, s.angle, s.width, s.length, s.headOffset};
}
void cancelPoint(World& w, const State& s, Vec3 p, bool convert, bool boundEffect) {
  if (convert && !outside(p, 32, 32))
    w.pointItem(p, 9, -pi / 2, .6000000238418579f); // 4a4060/4a41f4.
  if (!boundEffect || !outside(p, 32, 32))
    w.spawnCancelEffect(p, s.parameters.color * 2 + 4, s.angle, s.width);
}
template <class Pred>
int clearStraight(Laser& object, Manager& manager, World& world, Pred hit, bool convert,
                  bool force) {
  auto& s = object.state;
  if (force && s.protection)
    return 0;
  const bool moving = s.kind == Kind::Moving;
  // 429d21/42b32d: samples at 8,24,... with room for the full 16 unit cell.
  if (moving ? s.length < 16 : s.length <= 16)
    return 0;
  const int count = std::min(256, int(s.length / 16));
  if (count <= 0)
    return 0;
  const Vec3 origin = s.position, step = direction(s.angle, 16), half = direction(s.angle, 8);
  std::array<bool, 256> removed{};
  int hits = 0;
  Vec3 point = add(origin, half);
  for (int i = 0; i < count; ++i) {
    if (hit(point)) {
      removed[i] = true;
      ++hits;
      cancelPoint(world, s, point, convert, !moving);
    }
    point = add(point, step);
  }
  if (!hits)
    return 0;
  if (moving && hits == count) {
    s.retirePending = 1;
    return hits;
  }
  int begin = 0;
  while (begin < count && removed[begin])
    ++begin;
  if (moving && begin) {
    s.position = add(s.position, mul(step, float(begin)));
    s.length = float(double(s.length) - begin * 16.);
    if (s.length > 24) {
      s.parameters.maxLength = s.length;
      s.headOffset = 0;
    } else
      s.retirePending = 1;
  }
  const int first = begin;
  while (begin < count && !removed[begin])
    ++begin;
  if (!moving) {
    s.length = first ? 0.f : float(begin * 16);
    if (first && begin > first)
      manager.appendMovingFragment(object, add(origin, mul(step, float(first))),
                                   float((begin - first) * 16), world);
  } else if (begin < count) {
    const float shortened = float((begin - first) * 16);
    s.parameters.maxLength = float(double(s.parameters.maxLength) - (double(s.length) - shortened));
    s.length = shortened;
    if (s.length <= 24)
      s.retirePending = 1;
  }
  while (begin < count) {
    while (begin < count && removed[begin])
      ++begin;
    const int a = begin;
    while (begin < count && !removed[begin])
      ++begin;
    if (a == begin)
      break;
    const float length = float((begin - a) * 16);
    if (!moving || length > 24)
      manager.appendMovingFragment(object, add(origin, mul(step, float(a))), length, world);
  }
  return hits;
}
template <class Pred>
int clearCurve(CurveLaser& object, Manager&, World& world, Pred hit, bool convert, bool force) {
  auto& s = object.state;
  if (force && s.protection)
    return 0;
  const int count = int(s.nodes.size());
  std::vector<bool> removed(size_t(count), false);
  int hits = 0;
  for (int i = 0; i < count; ++i)
    if (hit(s.nodes[size_t(i)].position)) {
      removed[size_t(i)] = true;
      ++hits;
      if (i % 3 == 0)
        cancelPoint(world, s, s.nodes[size_t(i)].position, convert, false);
    }
  if (!hits)
    return 0;
  if (hits == count) {
    s.retirePending = 1;
    return hits;
  }
  int a = 0;
  while (a < count && removed[size_t(a)])
    ++a;
  if (a) {
    s.nodes.erase(s.nodes.begin(), s.nodes.begin() + a);
    removed.erase(removed.begin(), removed.begin() + a);
  }
  int run = 0;
  while (run < int(s.nodes.size()) && !removed[size_t(run)])
    ++run;
  if (run == int(s.nodes.size()))
    return hits;
  int next = run;
  while (next < int(s.nodes.size()) && removed[size_t(next)])
    ++next;
  if (next == int(s.nodes.size())) {
    s.nodes.resize(size_t(run));
    return hits;
  }
  // 42dd20..42ded3: the disconnected old head is converted/animated and
  // discarded; the native function retains the remaining tail, no new laser.
  for (int i = 0; i < run; i += 3)
    cancelPoint(world, s, s.nodes[size_t(i)].position, convert, false);
  s.nodes.erase(s.nodes.begin(), s.nodes.begin() + next);
  return hits;
}
} // namespace

void Timer::reset(int t) {
  previous = t - 1;
  current = t;
  value = float(t);
}
void Timer::advance(float rate) {
  previous = current;
  value = fadd(value, rate);
  current = int(value);
}
Parameters makeParameters(Kind kind, const FactoryInput& input, const Vec3& enemy, int lookupId) {
  Parameters p = input.config;
  p.position = add(input.fixedOrigin ? input.origin : enemy, input.offset);
  if (input.fixedOrigin)
    p.position.z = 0;
  p.angle = angleNormalize(input.angle);
  p.speed = input.speed;
  p.initialOffset = input.initialOffset;
  p.velocity = {};
  p.type = input.type;
  p.color = input.color;
  p.sound = input.sound;
  p.transformSound = input.transformSound;
  p.program = input.program;
  p.flags = kind == Kind::Timed ? input.config.flags | 2u : 1u;
  p.lookupId = kind == Kind::Timed ? lookupId : 0;
  if (kind == Kind::Curve)
    p.nodeCount = input.config.delay;
  return p;
}

void Laser::bind(World& world) {
  auto& s = state;
  AnimationBinding main{};
  main.type = s.parameters.type;
  main.color = s.parameters.color;
  main.typeColorAppearance = s.kind != Kind::Curve;
  main.script = s.kind == Kind::Curve
                    ? s.parameters.type + 217
                    : eb::appearances[size_t(std::clamp(s.parameters.type, 0, 30))].script;
  world.bindAnimation(*this, Role::Main, main);
  world.interruptAnimation(*this, Role::Main, 2);
  world.tickAnimation(*this, Role::Main);
  world.finishAnimationBirth(*this, Role::Main);
  AnimationBinding cap{};
  cap.script = s.parameters.color + 53;
  cap.color = s.parameters.color;
  cap.type = s.parameters.type;
  world.bindAnimation(*this, Role::Cap, cap);
  world.interruptAnimation(*this, Role::Cap, 2);
  world.tickAnimation(*this, Role::Cap);
  world.finishAnimationBirth(*this, Role::Cap);
}
void Laser::retire(World& world) {
  world.retireAnimation(*this, Role::Main);
  world.retireAnimation(*this, Role::Cap);
  state.phase = Phase::Removed;
}

void MovingLaser::initialize(World& world) {
  auto& s = state;
  s.phase = Phase::Active;
  bind(world);
  const auto& p = s.parameters;
  s.position = add(p.position, direction(p.angle, p.initialOffset));
  s.initialPosition = p.position;
  s.angle = p.angle;
  s.length = p.initialLength;
  s.width = p.width;
  s.speed = p.speed;
  s.velocity = direction(s.angle, s.speed);
  s.headOffset = s.speed > 0 ? .01f : 0;
  s.extensionAge.reset(0);
  if (p.sound >= 0)
    world.sound(p.sound, 0);
}
void TimedLaser::initialize(World& world) {
  auto& s = state;
  s.phase = Phase::Warning;
  bind(world);
  const auto& p = s.parameters;
  s.position = add(p.position, direction(p.angle, p.initialOffset));
  s.initialPosition = p.position;
  s.angle = p.angle;
  s.length = p.initialLength;
  s.width = 2;
  s.speed = p.speed;
  s.velocity = p.velocity;
  if (p.sound >= 0)
    world.sound(p.sound, 0);
}
void CurveLaser::initialize(World& world) {
  auto& s = state;
  s.phase = Phase::Active;
  bind(world);
  const auto& p = s.parameters;
  s.position = add(p.position, direction(p.angle, p.initialOffset));
  s.initialPosition = p.position;
  s.angle = p.angle;
  s.width = p.width;
  s.speed = p.speed;
  s.velocity = direction(s.angle, s.speed);
  s.nodes.assign(size_t(std::max(0, p.nodeCount)), Node{s.position, s.angle, s.speed});
  s.offscreenGrace.reset(30);
  if (p.sound >= 0)
    world.sound(p.sound, 0);
}

bool Laser::extensions(Manager&, World& world) {
  auto& s = state;
  bool repeat = false;
  do {
    repeat = false;
    while (s.cursor < 18) {
      const auto& r = s.parameters.program.records[size_t(s.cursor)];
      if (!r.kind || (!r.concurrent && s.activeExtensions))
        break;
      if (s.activeExtensions & r.kind)
        break;
      ++s.cursor;
      if (r.kind == 0x80000000)
        continue;
      if (r.kind == 0x1000 && s.kind != Kind::Timed) {
        s.activeExtensions |= 0x1000;
        s.wait.reset(r.c);
        break;
      }
      if (r.kind == 0x2000) {
        s.phase = Phase::Warning;
        continue;
      } // native writes phase3.
      if (r.kind == 0x4000 && s.kind != Kind::Timed) {
        world.sound(r.c, s.position.x);
        continue;
      }
      if (r.kind == 0x200) {
        s.protection = r.c;
        continue;
      } // native +44c, not +43c.
      if ((r.kind == 4 || r.kind == 8) && s.kind != Kind::Timed) {
        auto& motion = r.kind == 4 ? s.scalarAcceleration : s.polarAcceleration;
        motion.timer.reset(0);
        motion.duration = r.c;
        motion.linear = r.a;
        motion.angular = r.b;
        if (r.kind == 4) {
          const auto player = world.playerPosition();
          const float angle = r.b <= -990  ? s.angle
                              : r.b >= 990 ? float(std::atan2(double(player.y) - s.position.y,
                                                              double(player.x) - s.position.x))
                                           : r.b;
          motion.vector = direction(angle, r.a);
        }
        s.activeExtensions |= r.kind;
        if (s.cursor > 1 && s.parameters.program.transformSound >= 0)
          world.sound(s.parameters.program.transformSound, 0);
        continue;
      }
      // Every unproved extension is observable; no guessed neighboring-title rule.
      world.unsupported(r.kind);
    }
    const bool wasActive = s.activeExtensions != 0;
    for (const uint32_t kind : {4u, 8u})
      if (s.activeExtensions & kind) {
        // MovingLaser's native polar8 virtual method427e30 is a no-op.
        if (kind == 8 && s.kind != Kind::Curve)
          continue;
        auto& motion = kind == 4 ? s.scalarAcceleration : s.polarAcceleration;
        if (motion.timer.current >= motion.duration) {
          s.activeExtensions &= ~kind;
          repeat = true;
          continue;
        }
        const float rate = world.gameRate();
        s.speed = float(double(motion.linear) * rate + s.speed);
        if (kind == 4) {
          s.velocity = add(s.velocity, mul(motion.vector, rate));
          if (std::abs(s.velocity.x) >= 0.0001 || std::abs(s.velocity.y) >= 0.0001)
            s.angle = float(std::atan2(double(s.velocity.y), double(s.velocity.x)));
        } else {
          s.angle = angleNormalize(fadd(s.angle, fmul(motion.angular, rate)));
          s.velocity = direction(s.angle, s.speed);
        }
        motion.timer.advance(rate);
      }
    if (s.activeExtensions & 0x1000) {
      if (s.wait.current <= 0) {
        s.activeExtensions &= ~0x1000u;
        repeat = true;
      } else {
        s.wait.advance(-world.gameRate());
      }
    }
    if (wasActive && s.protection)
      --s.protection;
  } while (repeat);
  return s.phase == Phase::Retiring;
}
void Laser::updateStraightAnimation(World& world) {
  world.updateAnimationGeometry(*this, Role::Main, geometry(state));
  world.tickAnimation(*this, Role::Main);
  // 429ab9/42b0c4: the cap only advances while head offset is exactly zero.
  if (state.headOffset == 0) {
    world.updateAnimationGeometry(*this, Role::Cap, geometry(state));
    world.tickAnimation(*this, Role::Cap);
  }
}
void Laser::checkStraightCollision(Manager& manager, World& world, bool moving) {
  auto& s = state;
  if (s.length <= 16 || (moving && s.width <= 3))
    return;
  const float half = s.width >= 32 ? s.width * .5f
                     : moving      ? float(double(s.width) - (double(s.width) + 16) * .5)
                                   : float(double(s.width) - (double(s.width) + 16) / 3.);
  const Vec3 origin =
      moving ? add(s.position, direction(s.angle, float(double(s.length) / 2))) : s.position;
  const float size = moving ? float(double(s.length) * 4 / 5) : s.length;
  const auto result = world.lineCollision(origin, s.angle, half, size);
  if (result == Collision::Hit)
    clearRectangle(manager, world, world.playerPosition(), {32, 32, 0}, false, true);
  else if (result == Collision::Graze) {
    if ((moving ? s.grazeAge.current : s.age.current) % 3 == 0)
      world.graze(world.playerPosition());
    if (moving)
      s.grazeAge.advance(world.gameRate());
  }
}
bool MovingLaser::update(Manager& manager, World& world) {
  auto& s = state;
  if (extensions(manager, world))
    return false;
  const float rate = world.gameRate();
  if (s.length < s.parameters.maxLength)
    s.length = std::min(fadd(s.length, fmul(s.speed, rate)), s.parameters.maxLength);
  else {
    s.headOffset = fadd(s.headOffset, fmul(s.speed, rate));
    s.position = add(s.position, mul(s.velocity, rate));
    if (s.parameters.travelDistance > 0 &&
        double(s.length) + s.headOffset > s.parameters.travelDistance) {
      s.length = float(double(s.parameters.travelDistance) - s.headOffset);
      s.parameters.maxLength = s.length;
      if (s.length <= 0)
        return true;
    }
  }
  if (s.offscreenGrace.current > 0)
    s.offscreenGrace.advance(-rate);
  else if (outside(s.position, s.width, s.width) &&
           outside(add(s.position, direction(s.angle, s.length)), s.width, s.width))
    return true;
  checkStraightCollision(manager, world, true);
  updateStraightAnimation(world);
  s.extensionAge.advance(rate);
  return false;
}
bool TimedLaser::update(Manager& manager, World& world) {
  auto& s = state;
  if (extensions(manager, world))
    return false;
  const auto& p = s.parameters;
  const float rate = world.gameRate();
  if (s.length < p.maxLength)
    s.length = std::min(fadd(s.length, fmul(s.speed, rate)), p.maxLength);
  s.angle = angleNormalize(fadd(s.angle, fmul(p.angularVelocity, rate)));
  if (p.flags & 1) {
    Vec3 boss;
    if (world.bossPosition(boss))
      s.position = boss;
  }
  s.position = add(s.position, mul(s.velocity, rate));
  // Native switch deliberately falls through to the next phase in this tick.
  switch (s.phase) {
  case Phase::Active:
    if (s.age.current < p.hold)
      break;
    s.age.reset(0);
    s.phase = Phase::Shrinking;
    [[fallthrough]];
  case Phase::Shrinking:
    if (s.age.current >= p.shrink)
      return true;
    s.width = float(double(p.width) - double(p.width) * s.age.value / p.shrink);
    break;
  case Phase::Warning:
    if (s.age.current < p.delay)
      break;
    s.age.reset(0);
    s.phase = Phase::Growing;
    break;
  case Phase::Growing:
    if (s.age.current >= p.grow) {
      s.age.reset(0);
      s.width = p.width;
      s.phase = Phase::Active;
    } else
      s.width = float(double(p.width) * s.age.value / p.grow);
    break;
  default:
    break;
  }
  if (s.phase == Phase::Active || s.phase == Phase::Growing)
    checkStraightCollision(manager, world, false);
  updateStraightAnimation(world);
  return false;
}
bool CurveLaser::update(Manager& manager, World& world) {
  auto& s = state;
  if (extensions(manager, world))
    return false;
  const float rate = world.gameRate();
  for (size_t i = s.nodes.size(); i > 1; --i)
    s.nodes[i - 1] = s.nodes[i - 2];
  s.position = add(s.position, s.velocity); // native42c965 has no global rate multiply.
  if (!s.nodes.empty())
    s.nodes[0] = {s.position, s.angle, s.speed};
  if (s.offscreenGrace.current > 0)
    s.offscreenGrace.advance(-rate);
  else if (!(s.activeExtensions & 0x400) &&
           std::all_of(s.nodes.begin(), s.nodes.end(),
                       [&](const Node& n) { return outside(n.position, s.width, s.width); }))
    return true;
  float total = 0;
  bool grazed = false;
  for (size_t i = 0; i + 1 < s.nodes.size(); ++i) {
    const auto& node = s.nodes[i];
    total = fadd(total, node.speed);
    if (total <= 16)
      continue;
    auto r = world.lineCollision(add(node.position, direction(node.angle, node.speed * .5f)),
                                 node.angle, s.width * .5f, node.speed);
    if (r == Collision::Hit)
      clearRectangle(manager, world, world.playerPosition(), {32, 32, 0}, false, true);
    else if (r == Collision::Graze && !grazed && s.grazeAge.current % 3 == 0) {
      world.graze(world.playerPosition());
      grazed = true;
    }
  }
  world.updateAnimationGeometry(*this, Role::Main, geometry(s));
  world.updateAnimationGeometry(*this, Role::Cap, geometry(s));
  world.tickAnimation(*this, Role::Cap);
  s.grazeAge.advance(rate);
  return false;
}

int Laser::query(const Vec3& p, float radius) const {
  const auto& s = state;
  const float dx = float(double(p.x) - s.position.x), dy = float(double(p.y) - s.position.y);
  const float c = float(std::cos(double(-s.angle))), q = float(std::sin(double(-s.angle)));
  const float x = float(double(c) * dx - double(q) * dy),
              y = float(double(q) * dx + double(c) * dy);
  const float x1 = float(double(x) - radius), x2 = float(double(x) + radius),
              y1 = float(double(y) - radius), y2 = float(double(y) + radius);
  return double(x1) <= s.length && double(y1) <= double(s.width) * .5 && x2 >= 0 &&
                 double(y2) >= -double(s.width) * .5
             ? 2
             : 0;
}
int Laser::clearAll(Manager&, World& world, bool convert, bool force) {
  auto& s = state;
  if (force && s.protection)
    return 0;
  int count = 0;
  // 42a8a4/42aa2d and42be43/42c07e use strict length>16 and d+8<length.
  // At exact multiples the final cell is omitted (also omits its ANM RNG).
  for (float d = 8; double(d) + 8 < s.length; d = fadd(d, 16)) {
    cancelPoint(world, s, add(s.position, direction(s.angle, d)), convert, false);
    ++count;
    if (count >= 256)
      break;
  }
  s.phase = Phase::Retiring;
  return count;
}
int Laser::clearRectangle(Manager& m, World& w, const Vec3& center, const Vec3& dimensions,
                          bool convert, bool force) {
  return clearStraight(
      *this, m, w, [&](Vec3 p) { return insideRect(p, center, dimensions); }, convert, force);
}
int Laser::clearCircle(Manager& m, World& w, const Vec3& center, float radius, bool convert,
                       bool force) {
  return clearStraight(
      *this, m, w, [&](Vec3 p) { return insideCircle(p, center, radius); }, convert, force);
}
int CurveLaser::clearAll(Manager&, World& w, bool convert, bool force) {
  auto& s = state;
  if (force && s.protection)
    return 0;
  for (size_t i = 0; i < s.nodes.size(); i += 3)
    cancelPoint(w, s, s.nodes[i].position, convert, false);
  s.phase = Phase::Retiring;
  return 0;
}
int CurveLaser::clearRectangle(Manager& m, World& w, const Vec3& c, const Vec3& d, bool convert,
                               bool force) {
  return clearCurve(*this, m, w, [&](Vec3 p) { return insideRect(p, c, d); }, convert, force);
}
int CurveLaser::clearCircle(Manager& m, World& w, const Vec3& c, float r, bool convert,
                            bool force) {
  return clearCurve(*this, m, w, [&](Vec3 p) { return insideCircle(p, c, r); }, convert, force);
}

int Manager::spawn(Kind kind, const Parameters& parameters, World& world) {
  if (objects_.size() >= 256)
    return 0;
  if (!std::isfinite(parameters.angle) || !std::isfinite(parameters.speed) ||
      !std::isfinite(parameters.width) || parameters.nodeCount < 0 || parameters.nodeCount > 256) {
    world.unsupported(0xffffffff);
    return 0;
  }
  std::unique_ptr<Laser> object;
  switch (kind) {
  case Kind::Moving:
    object = std::make_unique<MovingLaser>();
    break;
  case Kind::Timed:
    object = std::make_unique<TimedLaser>();
    break;
  case Kind::Curve:
    object = std::make_unique<CurveLaser>();
    break;
  default:
    world.unsupported(0xfffffffe);
    return 0;
  }
  auto& s = object->state;
  s.serialId = ++serial_;
  s.lookupId = kind == Kind::Timed ? parameters.lookupId : int(s.serialId);
  s.kind = kind;
  s.parameters = parameters;
  Laser* p = object.get();
  objects_.push_back(std::move(object));
  p->initialize(world);
  return int(s.serialId);
}
void Manager::tick(World& world) {
  // Native caches successor before callbacks. A child appended behind a former
  // tail waits until next pass; with an old successor it can be reached this pass.
  auto it = objects_.begin();
  while (it != objects_.end()) {
    auto current = it++;
    auto& object = **current;
    auto& s = object.state;
    bool remove = false;
    if (s.retirePending) {
      ++s.retirePending;
      remove = s.retirePending >= 2;
    }
    if (!remove)
      remove = s.phase == Phase::Retiring || object.update(*this, world);
    if (remove) {
      object.retire(world);
      objects_.erase(current);
    } else {
      s.age.advance(world.gameRate());
      s.cancellable = true;
    }
  }
}
Laser* Manager::find(int id) {
  if (!id)
    return nullptr;
  for (auto& x : objects_)
    if (x->state.lookupId == id)
      return x.get();
  return nullptr;
}
const Laser* Manager::find(int id) const {
  if (!id)
    return nullptr;
  for (const auto& x : objects_)
    if (x->state.lookupId == id)
      return x.get();
  return nullptr;
}
std::vector<Laser*> Manager::ordered() {
  std::vector<Laser*> r;
  for (auto& x : objects_)
    r.push_back(x.get());
  return r;
}
std::vector<const Laser*> Manager::ordered() const {
  std::vector<const Laser*> r;
  for (const auto& x : objects_)
    r.push_back(x.get());
  return r;
}
int Manager::clearAll(World& w, bool convert, bool force) {
  int count = 0;
  auto it = objects_.begin();
  while (it != objects_.end()) {
    auto cur = it++;
    if ((*cur)->state.phase != Phase::Retiring)
      count += (*cur)->clearAll(*this, w, convert, force);
  }
  return count;
}
int Manager::clearRectangle(World& w, const Vec3& c, const Vec3& d, bool convert, bool force) {
  int count = 0;
  auto it = objects_.begin();
  while (it != objects_.end()) {
    auto cur = it++;
    if ((*cur)->state.phase != Phase::Retiring && (*cur)->state.cancellable)
      count += (*cur)->clearRectangle(*this, w, c, d, convert, force);
  }
  return count;
}
int Manager::clearCircle(World& w, const Vec3& c, float radius, bool convert, bool force) {
  int count = 0;
  auto it = objects_.begin();
  while (it != objects_.end()) {
    auto cur = it++;
    if ((*cur)->state.phase != Phase::Retiring)
      count += (*cur)->clearCircle(*this, w, c, radius, convert, force);
  }
  return count;
}
int Manager::query(const Vec3& p, float radius) const {
  int r = 0;
  for (const auto& x : objects_)
    if (x->state.phase != Phase::Retiring)
      r += x->query(p, radius);
  return r;
}
int Manager::clearId(World& w, int id) {
  int count = 0;
  if (!id)
    return 0;
  while (auto* p = find(id)) {
    count += p->clearAll(*this, w, false, false);
    p->state.lookupId = 0;
  }
  return count;
}
void Manager::reset(World* w) {
  if (w)
    for (auto& p : objects_)
      p->retire(*w);
  objects_.clear();
  serial_ = 0x10000;
}
Laser* Manager::appendMovingFragment(const Laser& source, const Vec3& position, float length,
                                     World& world) {
  Parameters p = source.state.parameters;
  p.position = position;
  p.angle = source.state.angle;
  p.width = source.state.width;
  p.initialOffset = 0;
  p.initialLength = length;
  p.maxLength = length;
  p.lookupId = 0;
  if (source.state.kind == Kind::Timed) {
    p.speed = 8;
    p.travelDistance = float(double(source.state.parameters.maxLength) -
                             std::hypot(double(position.x) - source.state.position.x,
                                        double(position.y) - source.state.position.y));
    p.program = {};
    p.flags = 1;
  }
  const int id = spawn(Kind::Moving, p, world);
  return find(id);
}
} // namespace th12::laser
