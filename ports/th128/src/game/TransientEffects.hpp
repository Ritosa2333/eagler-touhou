#pragma once
#include "BulletRules.hpp"
#include "animation/AnmLogic.hpp"
#include "engine/Random.hpp"
#include <string>
#include <vector>

namespace th128 {
struct EffectSnapshot {
    std::uint32_t id=0;
    int script=-1,sprite=-1,ticks=0;
    float x=0,y=0,z=0;
    anm::State animation;
};
class TransientEffects {
public:
    bool load(const void* bytes,std::size_t size,std::uint16_t displaySeed,std::string& error);
    bool spawn(int script,bullet::Position position,int layer,std::string& error);
    bool tick(std::string& error);
    bool setDisplaySeed(std::uint16_t seed);
    bool setGameRandom(std::function<std::uint32_t()> random);
    std::vector<EffectSnapshot> snapshots() const;
    std::uint32_t displayRandomCalls() const;
    const anm::SpriteInfo* spriteInfo(int sprite) const;
private:
    struct Effect {std::uint32_t id=0;int script=-1;bullet::Position position;anm::VM machine;std::string error;};
    anm::Registry registry_;
    Random displayRandom_;
    std::function<std::uint32_t()> gameRandom_;
    std::vector<Effect> effects_;
    std::uint32_t nextId_=1;
    bool executing_=false;
    anm::Runtime runtime(Effect& effect);
};
}
