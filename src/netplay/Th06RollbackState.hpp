#pragma once

#include <cstddef>
#include <cstdint>

struct Bullet;
struct Effect;
struct Enemy;
struct Item;
struct Laser;

namespace Netplay::Th06Rollback
{
struct Config
{
    std::size_t maxFrames = 16;
    std::size_t maxBytesPerFrame = 8 * 1024 * 1024;
    std::size_t maxBlocksPerFrame = 4096;
    std::size_t maxScreenEffectsPerFrame = 128;
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
std::size_t CapturedScreenEffects(std::uint32_t frame);
std::uint64_t DebugStateHash();

// Spawn paths call these immediately before overwriting an inactive reusable
// slot. Calls outside an open rollback frame are deliberately cheap no-ops.
bool TouchEnemy(Enemy *enemy);
bool TouchBullet(Bullet *bullet);
bool TouchLaser(Laser *laser);
bool TouchItem(Item *item);
bool TouchEffect(Effect *effect);
} // namespace Netplay::Th06Rollback
