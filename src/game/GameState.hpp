#pragma once
#include "GameTypes.hpp"
#include "PlayerSystem.hpp"
#include "Presentation.hpp"
#include "Snapshot.hpp"

namespace th12 {

extern Program program;
extern std::array<Loadout, 6> loadouts;
extern PlayerMotion playerMotion;
extern ShotSchedule shotSchedule;
extern Random random, displayRandom;
extern anm_logic::Scene animationScene;
extern u32 nextDetachedAnimation;
extern std::deque<std::unique_ptr<Enemy>> enemies;
extern std::deque<Projectile> bullets;
extern item_system::Manager itemManager;
extern ufo_system::Manager ufoManager;
extern item_system::ResourceState resources;
extern std::array<message_system::File, 6> messageFiles;
extern message_system::VM messageVM;
extern spell_system::State spellState;
extern pw::Manager playerDamageSources;
extern pw::BombState playerBomb;
extern std::array<uint64_t, 7> spellAnimationKeys;
extern std::array<uint64_t, 8> messageAnimationKeys;
struct MessageEvent {
  int op = 0, frame = 0;
  std::vector<u8> payload;
};
extern std::vector<MessageEvent> messageEvents;
extern std::array<u32, 1024> unsupported;
extern std::unordered_map<u32, u32> animationUnsupported;
extern std::unordered_map<int, Value> globals;
extern int frame, character, shot, difficulty, stageNumber, px, py, graze;
extern int backgroundFrame;
extern bool backgroundVisible;
extern stage_logic::Program stageProgram;
extern stage_logic::State stageState;
extern eb::World& bulletWorld;
extern int &power, &lives, &bombs, &score;
extern int invuln, deathWindow, bombTimer, previousHeld, fireFrame, eventBits, dialogue, spell,
    ended;
extern int playerState, playerStateTicks, oracleFixtureFlags;
extern u32 playerFlags;
extern u32 nextEnemyId, nextProjectileId, nextItemId;
extern std::function<void(Projectile&)> friendlyBirthHook, friendlyHitHook;
extern std::function<void(Projectile&, int, bool)> friendlyInterruptHook;
extern std::function<void(const pw::BombVisual&)> playerBombBindHook;
extern std::function<void(const pw::ReimuBOrb&)> reimuBOrbBindHook;
extern std::function<void(int, u32, float, float)> playerBombCustomBindHook;

uint64_t enemyAnimationKey(u32 id, int slot);
void configureAnimations();
void bindEnemyAnimation(Enemy&, int slot, int script);
void detachedAnimation(int bank, int script, const Enemy&, bool head);
uint64_t bindScreenAnimation(int bank, int script, int layer = 23);
void startMessageAnimations();
void messageCommand(const message_system::Command&);
void startSpellAnimations();
void endSpellAnimations(const spell_system::Result&);
bool enemyOutside(Enemy&);
uint64_t stagePrimitiveKey(int slot);
uint64_t stageEmbeddedKey(int slot);
void initializeStageAnimations();
void tickStage();
float aim(float x, float y);
float normalize(float angle);
float mirrorAngle(float angle);
void combinePosition(Enemy&);
float movementCoordinate(double value);
void circularPosition(Enemy&, bool relative);
Enemy* primaryBoss();
float easing(float value, int mode);
void runContext(Enemy&, Context&);
void resetPhase(Enemy&, const std::string&);
void dropItems(Enemy&);
void drop(Enemy&);
void emit(Enemy&, Shooter&);
Enemy* spawn(const std::string&, float x, float y, int life, int points, int drop,
             bool mirror = false, const Enemy* parent = nullptr);
ufo_system::EnemyHooks ufoHooks();
void applyUfoPlan(const ufo_system::RewardPlan&);
void tickUfo();
void tickItems(bool focus);
void tickEnemies();
void tickBullets();
void tickPlayer(int held, int pressed);
void tickBomb();
void tick(int held, int pressed);
void tickMessage(int held, int pressed);
void tickSpell();
void configurePlayerAnimations();
void initializePlayerAnimation();
void tickPlayerAnimation();
void playerMissCollision();
void syncBombAnimations();
void fire();

} // namespace th12
