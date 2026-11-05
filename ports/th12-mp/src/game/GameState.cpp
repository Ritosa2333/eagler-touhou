// TH12 1.00b portable C++ runtime. See NOTICE.md for rights.
#include "GameState.hpp"

namespace th12 {
std::array<Loadout, 6> loadouts;


Random random;
Random displayRandom{0x1234, 0};
anm_logic::Scene animationScene;
u32 nextDetachedAnimation = 1;
std::deque<std::unique_ptr<Enemy>> enemies;
std::deque<Projectile> bullets;
item_system::Manager itemManager;
ufo_system::Manager ufoManager;

std::array<message_system::File, 6> messageFiles;
message_system::VM messageVM;
spell_system::State spellState;
std::array<uint64_t, 7> spellAnimationKeys{};
// Native MSG handles +40/+44/+48/+4c/+50/+54/+58/+5c. Clearing a
// handle preserves a VM that is still playing its exit animation.
std::array<uint64_t, 8> messageAnimationKeys{};
std::vector<uint64_t> messageAnimationOwners;
std::vector<MessageEvent> messageEvents;
std::array<u32, 1024> unsupported{};
std::unordered_map<u32, u32> animationUnsupported;
std::unordered_map<uint64_t, u32> referenceFaults;
std::unordered_map<uint64_t, u32> runtimeFaults;
void recordRuntimeFault(uint32_t system, uint32_t code) {
  const auto key = (uint64_t(system) << 32) | code;
  if (runtimeFaults.size() < 256 || runtimeFaults.contains(key))
    ++runtimeFaults[key];
}

Program program;
std::unordered_map<int, Value> globals;
std::array<u32, 3> phasePlayerCounters{};
int frame = 0, difficulty = 1, stageNumber = 1;
int backgroundFrame = 0;
float gameSpeed = 1;
bool backgroundVisible = true;
stage_logic::Program stageProgram;
stage_logic::State stageState;
int score = 0;
int eventBits = 0, dialogue = 0, spell = 0, ended = 0;




std::array<int, 4> replayStageGlobals{};

int oracleFixtureFlags = 0; // Explicit partial-world diagnostics only; normal runtime stays zero.
u32 nextEnemyId = 1, nextProjectileId = 1, nextItemId = 1;
u32 temporaryEnemyOwner = 0x7fffffffu;
u32 lastSpawnedEnemyId = 0;

} // namespace th12
