// Hand-restored TH12 1.00b item rules. Evidence: FULL-UFO-AUDIT.md / UFO-TOKENS.md.
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

namespace th12::item_system {
constexpr float pi = 3.1415927410125732f;
struct Vec2 {
  float x = 0, y = 0;
};
float add(float a, float b);
Vec2 polar(float angle, float speed);
enum class State : int {
  Inactive = 0,
  Falling = 1,
  Automatic = 2,
  CollectedAbove = 3,
  CollectedNear = 4,
  DelayedPoint = 5,
  Token = 6,
  Absorbing = 7,
  Aggregate = 8
};
enum class Kind : int {
  None = 0,
  Power = 1,
  Point = 2,
  LargePower = 3,
  LifePiece = 4,
  BombPiece = 5,
  Life = 6,
  Bomb = 7,
  FullPower = 8,
  SmallPoint = 9,
  Red = 10,
  Blue = 11,
  Green = 12,
  ChangingRed = 13,
  ChangingBlue = 14,
  ChangingGreen = 15,
  PowerAggregate = 16,
  PointAggregate = 17,
  MixedAggregate = 18
};
struct Entity {
  uint32_t id = 0;
  int type = 0; // Native numeric kind retained in semantic Oracle snapshots.
  State state = State::Inactive;
  Vec2 position{}, velocity{};
  int age = 0, colorTimer = 0, delay = 0;
  float elapsed = 0, colorElapsed = 0, attractionSpeed = 0, aggregateSpeed = 0;
  bool near = false;
  int aggregateKind = 0, bucketA = 0, bucketB = 0;
  float multiplier = 1;
  int visualScript = 0, visualAge = 0, visualInterrupt = 0, visualInterruptAge = 0;
  uint32_t visualInterruptVersion = 0;
  float alpha = 1;
  bool hintActive = false; // A collected hint's detached ANM can outlive this slot.
  int hintScript = 211, hintInterrupt = 0;
  uint32_t hintGeneration = 0;
  bool active() const { return state != State::Inactive; }
  bool visible() const { return active() && state != State::DelayedPoint; }
  Kind kind() const { return Kind(type); }
};
struct PlayerView {
  Vec2 position{};
  int character = 0, state = 0; // State2 blocks pickup; state4 cancels ordinary attraction.
  bool focus = false;
  float pickupHalfWidth = 30, pickupHalfHeight = 30, nearHalfWidth = 30, nearHalfHeight = 30,
        attractionSpeed = 5;
  static PlayerView native(int character, Vec2 position, bool focus = false, int state = 0);
};
struct UfoView {
  bool active = false;
  int kind = 0, age = 0;
  Vec2 position{};
};
struct FrameView {
  PlayerView player{};
  UfoView ufo{};
  bool dialogue = false;
  float rate = 1;
  // Read after each callback: earlier pickups can affect later tokens in the same pool pass.
  const std::array<int, 3>* inventoryColors = nullptr;
  const int* inventoryCount = nullptr;
  // Native426ecb..426f3f: tick enabled body/secondary, then update hint pose,
  // before the item clock advances. Pickups/releases/expired slots skip this tail.
  std::function<void(const Entity&)> animate;
};
enum class EventKind {
  CollectOrdinary,
  CollectToken,
  Absorbed,
  CollectAggregate,
  RankDelta,
  AnimationInterrupt,
  AnimationRebind,
  HintCreated,
  HintRemoved,
  HintCollected
};
struct Event {
  EventKind kind;
  Entity entity{};
  int value = 0;
};
using EventSink = std::function<void(const Event&)>;
struct SpawnRequest {
  int type = 0;
  Vec2 position{};
  float angle = -pi / 2, speed = 2.2f;
  int aggregateKind = 0, bucketA = 0, bucketB = 0;
  float multiplier = 1;
};

class Manager {
public:
  static constexpr size_t ordinaryCapacity = 600, tokenCapacity = 16, smallPointCapacity = 2048,
                          totalCapacity = 2664;
  std::array<Entity, totalCapacity> entities{};
  uint32_t nextId = 1;
  int tokenSideCounter = 0, smallPointCursor = 0, smallPointSpawns = 0;
  uint32_t poolMisses = 0, unsupportedTypes = 0;
  std::vector<Event> events;
  void reset(int side = 0);
  Entity* find(uint32_t id);
  const Entity* find(uint32_t id) const;
  size_t activeCount() const;
  // Pool exhaustion returns nullptr safely; token sideCounter still advances before allocation.
  Entity* spawn(const SpawnRequest& request);
  Entity* spawn(int type, Vec2 position, float angle = -pi / 2, float speed = 2.2f);
  Entity* spawn(Kind kind, Vec2 position, float angle = -pi / 2, float speed = 2.2f) {
    return spawn(int(kind), position, angle, speed);
  }
  void tick(const FrameView& frame, const EventSink& sink = {});
};

// Retail scoreStored = displayed score /10; pointValueStored = displayed PIV *100.
struct ResourceState {
  int power = 100, maxPower = 400, powerStep = 100, scoreStored = 0, pointValueStored = 1000000,
      maxPointValueStored = 20000000;
  int lives = 2, bombs = 2, lifeFragments = 0, bombFragments = 0, rank = 0, difficulty = 1;
  static ResourceState initial(int difficulty);
  int pointValue() const;
  void score(int displayed);
  void addRank(int amount);
  void addPointValue(int stored);
};
struct RewardDelta {
  int scoreStored = 0, power = 0, pointValueStored = 0, lives = 0, bombs = 0, lifeFragments = 0,
      bombFragments = 0, rank = 0;
};
RewardDelta difference(const ResourceState& before, const ResourceState& after);
void smallPower(ResourceState& resources);
RewardDelta collectOrdinary(ResourceState& resources, const Entity& item, const PlayerView& player);
RewardDelta collectAggregate(ResourceState& resources, const Entity& item);
struct DeathDropPlan {
  RewardDelta immediate{};
  std::vector<SpawnRequest> spawns;
};
DeathDropPlan deathPowerDrops(ResourceState& resources, Vec2 position);
} // namespace th12::item_system
