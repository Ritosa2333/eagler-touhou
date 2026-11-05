#pragma once

#include <cstddef>
#include <cstdint>

struct ScreenEffect;
struct Bullet;
struct Effect;
struct Enemy;
struct Item;
struct Laser;
struct Player;
struct PlayerBombInfo;
struct PlayerBullet;

namespace Netplay::Th07Rollback
{
struct Config
{
    std::size_t maxFrames = 16;
    std::size_t checkpointLogicalFrames = 2;
    std::size_t maxBytesPerFrame = 8 * 1024 * 1024;
    std::size_t maxBlocksPerFrame = 4096;
    std::size_t maxBombEffectsPerFrame = 1024;
    bool coalesceBulletRuns = false;
    bool compactBulletSnapshots = false;
    bool fastBulkCopy = false;
    bool coalesceRestore = false;
    bool liveBulletSnapshots = false;
    bool auditLiveBulletBytes = false; // Explicit test-only full-pool oracle.
    bool elideDormantBulletVm = false;
};

bool Reset(const Config &config = Config{});
void Clear();
bool BeginFrame(std::uint32_t frame);
bool EndFrame();
bool RestoreTo(std::uint32_t frame, std::uint32_t *replayFrom = nullptr);
void DiscardBefore(std::uint32_t frame);

bool IsCapturing();
bool Failed();
std::size_t CapturedBytes(std::uint32_t frame);
std::size_t CapturedBlocks(std::uint32_t frame);
std::size_t CapturedBombEffects(std::uint32_t frame);
std::uint64_t RestoreCopiedBytes();
std::uint64_t RestoreSkippedBytes();
std::uint64_t ArenaGrowths();
std::uint64_t DebugStateHash();

// Spawn paths call these immediately before they overwrite a previously
// inactive slot. Calls outside an open rollback frame are cheap no-ops.
bool TouchEnemy(Enemy *enemy);
bool TouchBullet(Bullet *bullet);
void BeforeBulletDespawn(Bullet *bullet);
bool TouchLaser(Laser *laser);
bool TouchItem(Item *item);
bool TouchEffect(Effect *effect);
bool TouchPlayerBullet(PlayerBullet *bullet);
bool TouchPlayerBombInfo(PlayerBombInfo *bombInfo);
bool PatchDirectTouchSnapshots(std::size_t player, std::uint32_t afterFrame,
                               std::uint32_t throughFrame, float dx, float dy);
} // namespace Netplay::Th07Rollback
