#pragma once
#include "GameTypes.hpp"
#include "AdaptedInput.hpp"

namespace th12::multiplayer {
constexpr unsigned MaxPlayers = 3;
struct Options {
  unsigned count = 2, local = 0, seed = 1, stage = 1;
  int difficulty = 1;
  bool challenge = false;
  std::array<int, MaxPlayers> selections{};
  bool valid() const;
};
struct Input {
  uint32_t held = 0;
  int motion = 0, x = 0, y = 0;
  bool pause = false;
};
// Permanent per-seat owners. The world never copies one player's state into
// another player's globals. Scope selects callbacks' owner, including ANM tails.
struct Pilot {
  unsigned seat = 0;
  PlayerMotion playerMotion{};
  ShotSchedule shotSchedule{};
  item_system::ResourceState resources{};
  pw::Manager playerDamageSources{};
  pw::BombState playerBomb{};
  AdaptedMotion adaptedMotion{};
  int character = 0, shot = 0, px = 0, py = 51200, graze = 0;
  int invuln = 120, deathWindow = 0, bombTimer = 0, previousHeld = 0, fireFrame = 0;
  int playerState = 1, playerStateTicks = 0, playerPreviousStateTicks = -1;
  float playerStateTime = 0, playerInvulnerabilityTime = 120, playerTotalTime = 0;
  uint32_t playerFlags = 0;
  uint64_t playerFocusAnimation = 0;
  bool playerFocused = false, ghost = false, lifeLatched = false, pauseHeld = false;
  unsigned lifeTaps = 0, powerTaps = 0, powerGap = 25, ghostClock = 0, misses = 0;
  uint32_t ghostRandom = 1;
  int ghostDx = 0, ghostDy = 0;
  std::unordered_map<uint64_t, int> bombDeliveredInterrupts;
  std::array<float, 3> respawnClearOrigin{};
  Input input{};
  uint32_t gestureHeld = 0;
  uint64_t bodyKey() const { return 0xa00000000ull + seat; }
  uint32_t owner() const { return 0xffff0000u + seat * 0x100u; }
};
extern std::array<Pilot, MaxPlayers> pilots;
extern Options options;
extern bool active, paused, finished;
extern bool restartRequested,returnRequested,saveRequested,menuConfirm,menuInputReady;
extern unsigned menuSelection,menuAction,menuPreviousHeld,menuAge,menuHoldAge,storyPreviousHeld;
extern unsigned currentSeat, tickNumber, wipeFrames, lifeFragments, bombFragments;
extern int teamRank;
extern std::unordered_map<uint32_t, unsigned> damageRemainders;
extern std::unordered_map<uint32_t, uint8_t> bulletGrazeMasks;
Pilot& pilot();
struct Scope {
  unsigned previous;
  explicit Scope(unsigned seat);
  ~Scope();
};
bool begin(const Options&);
bool update(const std::array<Input, MaxPlayers>&);
void initializePilots(bool nextStage = false);
void tickPlayers();
void tickBombs();
void tickRules();
void revive(unsigned seat, bool rescued);
bool operable(unsigned seat);
unsigned operableCount();
int nearest(float x, float y, bool needsPower = false);
std::array<float, 2> target(float x, float y);
bool anyBomb();
void shareInvulnerability(unsigned source, int ticks);
void becameGhost();
int deathbombFrames();
void syncScore(int previous);
void syncRank(int previous);
int bossDamage(const Enemy&, int damage);
void collectSharedPiece(int type);
void collectItem(const item_system::Event&, const item_system::PlayerView&);
void itemPlayer(item_system::Entity&, item_system::PlayerView&);
eb::Collision bulletCollision(eb::State&);
laser::Collision laserCollision(const laser::Vec3&, float, float, float, uint8_t* grazers=nullptr);
int allPlayerDamage(const Enemy&, float, float, float, float);
void enemyCollision(Enemy&);
uint32_t canonicalHash();
const std::string& snapshot();
} // namespace th12::multiplayer
namespace th12 { using multiplayer::pilot; }
