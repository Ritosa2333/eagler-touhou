// TH12 1.00b laser subsystem; hand-written recovery of 428450/428270 and
// 429610/42ada0/42c770. Animation bytecode/RNG belongs to the World bridge.
#pragma once
#include "../bullet.hpp"
#include <array>
#include <cstdint>
#include <list>
#include <memory>
#include <vector>

namespace th12::laser {
struct Vec3 {
  float x = 0, y = 0, z = 0;
};
enum class Kind : uint32_t { Moving = 0, Timed = 1, Curve = 2 };
enum class Role : uint32_t { Main = 0, Cap = 1 };
enum class Collision : uint32_t { None = 0, Hit = 1, Graze = 2 };
enum class Phase : uint32_t {
  Removed = 0,
  Retiring = 1,
  Active = 2,
  Warning = 3,
  Growing = 4,
  Shrinking = 5
};
struct Timer {
  int previous = -1, current = 0;
  float value = 0;
  void reset(int);
  void advance(float);
};
struct Parameters {
  Vec3 position{}, velocity{};
  float angle = 0, width = 0, speed = 0, initialLength = 0, maxLength = 0, travelDistance = 0;
  float initialOffset = 0, angularVelocity = 0;
  int delay = 0, grow = 0, hold = 0, shrink = 0, nodeCount = 0, type = 0, color = 0, lookupId = 0;
  int sound = -1, transformSound = -1;
  uint32_t flags = 0;
  eb::Program program{};
};
// Typed construction inputs for ECL602/603/611. ECL525 has already encoded
// fixedOrigin as originX>=-990 (the native raw Z marker is0/1).
struct FactoryInput {
  Parameters config{};
  Vec3 offset{}, origin{};
  bool fixedOrigin = false;
  float angle = 0, speed = 0, initialOffset = 0;
  int type = 0, color = 0, sound = -1, transformSound = -1;
  eb::Program program{};
};
Parameters makeParameters(Kind, const FactoryInput&, const Vec3& enemyPosition, int lookupId = 0);
struct Node {
  Vec3 position{};
  float angle = 0, speed = 0;
};
struct AnimationBinding {
  int script = 0, type = 0, color = 0, interrupt = 2;
  // Main straight appearance is a native type/color bind; CurveMain is
  // script type+217. Cap uses script color+53. bind itself includes its first
  // bytecode tick; native then interrupt2 + one additional embedded tick.
  bool typeColorAppearance = false;
  bool customGeometry = true;
};
struct AnimationGeometry {
  Vec3 position{};
  float angle = 0, width = 0, length = 0, headOffset = 0;
};
struct Acceleration {
  Timer timer{};
  int duration = 0;
  float linear = 0, angular = 0;
  Vec3 vector{};
};
struct State {
  uint32_t serialId = 0;
  int lookupId = 0;
  Kind kind = Kind::Moving;
  Phase phase = Phase::Removed;
  Vec3 position{}, velocity{}, initialPosition{};
  float angle = 0, length = 0, width = 0, speed = 0, headOffset = 0;
  Timer age{}, grazeAge{}, extensionAge{}, offscreenGrace{}, wait{};
  bool cancellable = false;
  uint8_t retirePending = 0;
  int protection = 0;
  uint32_t activeExtensions = 0;
  int cursor = 0;
  float mirrorSpeed = 0;
  int mirrorRemaining = 0, mirrorFlags = 0;
  Parameters parameters{};
  Acceleration scalarAcceleration{}, polarAcceleration{};
  std::vector<Node> nodes{};
};
class Laser;
class World {
public:
  virtual ~World() = default;
  virtual float gameRate() const { return 1.f; }
  virtual Vec3 playerPosition() const { return {}; }
  virtual bool bossPosition(Vec3&) const { return false; }
  // Exactly the parameters passed to native 437a80: position (not always a
  // visual midpoint), angle, transverse half size, longitudinal size.
  virtual Collision lineCollision(const Vec3&, float, float, float) { return Collision::None; }
  virtual void graze(const Vec3&) {}
  virtual void bindAnimation(Laser&, Role, const AnimationBinding&) {}
  virtual void interruptAnimation(Laser&, Role, int) {}
  virtual void updateAnimationGeometry(Laser&, Role, const AnimationGeometry&) {}
  virtual bool tickAnimation(Laser&, Role) { return false; }
  virtual void finishAnimationBirth(Laser&, Role) {}
  virtual void blendControl(Laser&, int) {}
  virtual void retireAnimation(Laser&, Role) {}
  virtual void sound(int, float) {}
  virtual void emitBullet(const eb::Emission&) {}
  virtual uint32_t next32() { return 0; }
  virtual float unit() { return float(double(next32()) / 4294967295.0); }
  virtual void spawnCancelEffect(const Vec3&, int /*bullet script*/, float /*angle*/,
                                 float /*width*/) {}
  virtual void pointItem(const Vec3&, int /*type*/, float /*angle*/, float /*speed*/) {}
  virtual void unsupported(uint32_t /*native extension*/) {}
};
class Manager;
class Laser {
public:
  State state{};
  virtual ~Laser() = default;
  virtual void initialize(World&) = 0;
  // true asks Manager to unlink immediately, before advancing age/cancellable.
  virtual bool update(Manager&, World&) = 0;
  virtual int query(const Vec3&, float radius) const;
  virtual int clearAll(Manager&, World&, bool convert, bool force);
  virtual int clearRectangle(Manager&, World&, const Vec3&, const Vec3&, bool convert, bool force);
  virtual int clearCircle(Manager&, World&, const Vec3&, float, uint32_t cancelFlags, bool force);
  void bind(World&);
  void retire(World&);

protected:
  void updateStraightAnimation(World&);
  void checkStraightCollision(Manager&, World&, bool moving);
  bool extensions(Manager&, World&);
};
class MovingLaser final : public Laser {
public:
  void initialize(World&) override;
  bool update(Manager&, World&) override;
};
class TimedLaser final : public Laser {
public:
  void initialize(World&) override;
  bool update(Manager&, World&) override;
};
class CurveLaser final : public Laser {
public:
  void initialize(World&) override;
  bool update(Manager&, World&) override;
  int clearAll(Manager&, World&, bool, bool) override;
  int clearRectangle(Manager&, World&, const Vec3&, const Vec3&, bool, bool) override;
  int clearCircle(Manager&, World&, const Vec3&, float, uint32_t, bool) override;
};
class Manager {
public:
  int spawn(Kind, const Parameters&, World&);
  void tick(World&);
  Laser* find(int);
  const Laser* find(int) const;
  std::vector<Laser*> ordered();
  std::vector<const Laser*> ordered() const;
  int clearAll(World&, bool convert = false, bool force = false);
  void clearPhaseEnd(World&); // ECL445: reset protection/grace, include retiring nodes.
  int clearRectangle(World&, const Vec3&, const Vec3&, bool convert = false, bool force = false);
  int clearCircle(World&, const Vec3&, float, uint32_t cancelFlags = 0, bool force = false);
  int query(const Vec3&, float) const;
  int clearId(World&, int lookupId);
  void reset(World* world = nullptr);
  size_t size() const { return objects_.size(); }
  uint32_t serial() const { return serial_; }
  // Used by native TimedLaser partial clearing to create a MovingLaser tail.
  Laser* appendMovingFragment(const Laser&, const Vec3&, float length, World&);

private:
  std::list<std::unique_ptr<Laser>> objects_{};
  uint32_t serial_ = 0x10000;
};
} // namespace th12::laser
