#pragma once
#include "PlayerShotBirthRules.hpp"
#include "PlayerShotTickRules.hpp"
#include "ShotEnemyCollisionRules.hpp"
#include "DamageFieldPool.hpp"
#include "animation/AnmLogic.hpp"
#include <functional>

namespace th128::playershot {
struct ShotSnapshot {
    std::uint32_t id=0,slot=0,animationHandle=0;
    int damage=0,script=-1,age=0;
    std::uint32_t hitMarker=0;
    int active=0;
    float speed=0;
    ShotPosition position,velocity;
    anm::State state;
    anm::SpriteInfo sprite;
};
class PlayerShotHost {
public:
    using SoundHost=std::function<bool(int,float,std::string&)>;
    using BoundsHost=std::function<std::optional<ShotAnimationBounds>(const anm::State&,const anm::SpriteInfo&,std::string&)>;
    using ImpactHost=std::function<bool(std::string&)>;
    using DamageSourceHost=std::function<std::optional<damagefield::PoolContact>(const shotenemy::EnemyContactShape&,std::uint32_t,std::string&)>;
    using ScoreHost=std::function<bool(int,std::string&)>;
    bool load(std::span<const std::uint8_t> shot,std::span<const std::uint8_t> animation,std::string& error);
    bool setCallbackBindings(std::array<std::uint32_t,4> addresses);
    bool setSoundHost(SoundHost sound);
    bool setBoundsHost(BoundsHost bounds);
    bool setImpactHost(ImpactHost impact);
    bool setDamageSourceHost(DamageSourceHost sources);
    bool setScoreHost(ScoreHost score);
    bool enableContactAnimations();
    bool emitCycle(float cumulativeArea,int difficulty,int shotCount,ShotPosition position,std::string& error);
    bool tickUnit(std::string& error);
    bool tickAnimationsUnit(std::string& error);
    std::optional<shotenemy::ShotEnemyQueryPlan> queryEnemy(
        const shotenemy::EnemyContactShape& enemy,std::uint32_t previousTime,std::uint32_t currentTime,
        std::span<const std::uint32_t> actualDamageSourceFlags,std::string& error);
    std::vector<ShotSnapshot> snapshots() const;
private:
    struct Slot {FirstShotBirthPlan plan;anm::VM visual;std::uint32_t id=0,visualHandle=0;bool primaryAlive=true;std::string fault;};
    std::unique_ptr<ShtBank> bank_;
    anm::Registry registry_;
    std::vector<std::unique_ptr<Slot>> slots_=std::vector<std::unique_ptr<Slot>>(ShotSlotCount);
    std::optional<std::array<std::uint32_t,4>> callbacks_;
    SoundHost sound_;
    BoundsHost bounds_;
    ImpactHost impact_;
    DamageSourceHost damageSources_;
    ScoreHost score_;
    bool contactAnimations_=false;
    std::string failure_;
    std::uint32_t nextId_=1,nextVisualHandle_=1;
    bool executing_=false;
    bool fail(std::string message,std::string& error);
    anm::Runtime runtime(Slot& slot);
};
}
