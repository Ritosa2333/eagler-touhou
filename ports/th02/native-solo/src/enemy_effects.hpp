#pragma once
#include <cstdint>
#include <functional>
namespace soew {
struct EnemyBulletRequest {
    enum Kind {Pellet,Bullet16} kind=Pellet;
    int left=0,top=0;
    std::uint8_t angle=0,group=0,pattern=0;
    unsigned speed=0; // Original Q12.4 raw value, NOT pixels/frame.
};
struct EnemyTileRequest {int left=0;unsigned vram_y=0,image=0;};
// Return true only when the operation was handled (including original legitimate
// clip/full-pool rejection). false means unavailable; do not consume/mutate.
// Handlers must not partially apply an event then return false.
struct EnemyEffectSink {
    std::function<bool(const EnemyBulletRequest&)> bullet;
    std::function<bool(unsigned)> sound;
    std::function<bool(const EnemyTileRequest&)> tile;
};
}
