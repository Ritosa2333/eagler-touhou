// TH12 ECL 529 / 534 callbacks verified against retail 1.00b.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>

namespace th12::special_enemy {

// These values are the native 4af0fc table indices. The laser callbacks 3..5
// are implemented by LaserEnemyCallbacks; they are deliberately not aliases.
enum class Callback : int32_t {
  None = 0,
  AttractBullets = 1,
  ExtraUfoScore = 2,
  LinkedBulletLine = 6,
};

struct Position {
  float x = 0, y = 0, z = 0;
};

struct EnemyView {
  uint32_t id = 0;
  Position position;
  int32_t age = 0; // Native EnemyState+270, read before the tail timer tick.
  std::array<int32_t, 4> privateIntegers{}; // Native +23c, +240, +244, +248.
};

struct BulletView {
  // Bullet+0 is the object flags word; it is neither VM+47c nor the active
  // extension mask (+528). Callbacks require bit 0 and native phase 1.
  uint32_t objectFlags = 0;
  uint16_t phase = 0;
  int32_t tag = 0; // Bullet+50c, assigned by extension 0x200000 from record.c.
  Position position;
  float vx = 0, vy = 0, heading = 0, nominalSpeed = 0;
};

struct VelocityHeading {
  float vx = 0, vy = 0, heading = 0;
};

struct ScorePopup {
  Position screenPosition;    // World position translated by (+224,+16,0).
  int32_t displayedValue = 0; // Display units; scoreStored is one tenth.
  int32_t font = 5;
  uint32_t color = 0xd0ffffffu;
  float scaleX = 2, scaleY = 2;
  int32_t horizontalAlignment = 0, verticalAlignment = 0;
  int32_t nativeTextFlag = 1; // FontManager+18f9c; retained without guessing.
};

// Slots must be visited in the native pool order, including inactive holes.
// The retail pool has 2000 slots; a compact adapter may preserve their order.
// Writes are synchronous: a later ECL instruction observes the result.
struct World {
  std::function<size_t()> bulletSlotCount;
  std::function<bool(size_t, BulletView&)> readBullet;
  std::function<void(size_t, const VelocityHeading&)> writeBulletVelocity;
  // May alias the current enemy's private integer 3. Native callback 2 reads
  // it only when private integer 0 == 1, then advances it modulo 15.
  std::function<int32_t*()> primaryBossRewardCounter;
  int32_t* scoreStored = nullptr;
  std::function<void(const ScorePopup&)> showScorePopup;
  // Set the main VM's spriteWidth (+58), preserving spriteHeight and scale,
  // and OR its flags (+47c) with 8. Return false when that VM does not exist.
  std::function<bool(uint32_t, float)> writePrimaryAnimationWidth;
};

enum class Fault {
  None,
  UnsupportedSelector,
  MissingBulletReader,
  MissingBulletWriter,
  MissingPrimaryBoss,
  InvalidRewardIndex,
  MissingScoreStorage,
  MissingPopupWriter,
  MissingPrimaryAnimation,
};

struct Result {
  // 413c76 honors nonzero as ordinary enemy removal. ECL534 ignores this
  // return value; it does not turn callback removal into a death reward.
  int32_t nativeReturn = 0;
  Fault fault = Fault::None;
  size_t affectedBullets = 0;
};

inline constexpr std::array<int32_t, 15> extraScoreTable = {100, 100, 100, 50,  150, 100, 100, 50,
                                                            50,  100, 150, 100, 100, 50,  300};

bool supports(int32_t selector);
Result run(int32_t selector, EnemyView& enemy, const World& world);

} // namespace th12::special_enemy
