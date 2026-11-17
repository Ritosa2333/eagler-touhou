#pragma once
#include "PlayerHitRules.hpp"
#include "PlayerRules.hpp"
#include "TouchMotion.hpp"
#include "PlayerDeathRules.hpp"
#include "animation/AnmLogic.hpp"
#include "animation/AnmScene.hpp"
#include <functional>

namespace th128::player {
inline constexpr std::uint32_t kPlayerStageClearFlag=0x10;
inline constexpr std::uint32_t kPlayerStageClearCounterLimit=0x1e;
struct HitHostSnapshot {
    hit::PlayerGate gate;
    hit::Timer stateTimer,heldTimer,protection;
    hit::SoundControl soundControl;
    hit::TailState tail;
    hit::TailControl tailControl;
    std::uint32_t stageClearCounter438f0=0;
    std::int32_t laserContactCounter43a90=0;
};
struct LaserContactEffect {std::uint32_t id=0;anm::State state;anm::SpriteInfo sprite;float x=0,y=0,z=0;};
struct PowerOutline {anm::State state;anm::SpriteInfo sprite;};
class PlayerHitHost {
public:
    using EffectHost=std::function<bool(int,hit::Position,int,std::string&)>;
    using SoundHost=std::function<bool(int,std::string&)>;
    using SoundOperationHost=std::function<bool(int,int,std::string&)>;
    bool configure(const void* animation,std::size_t size,HitHostSnapshot snapshot,
                   EffectHost effects,SoundHost sound,std::string& error);
    std::optional<int> confirmedOverlap(hit::Position player,hit::HitControl control,std::string& error);
    bool assignPerfectProtection200(std::string& error);
    bool assignPerfectProtection10(std::string& error);
    bool stepProtection(float clock,std::string& error);
    bool advanceWindowUnit(hit::WindowControl control,std::string& error);
    bool advanceAnimationUnit(std::string& error);
    bool advanceNormalTimersUnit(std::string& error);
    int focusCounter() const;
    bool setHeldTimer(hit::Timer timer,std::string& error);
    bool syncPlayerFlags(std::uint32_t flags,std::string& error);
    bool applyStageClearStart(std::string& error);
    bool advanceStageClearUnit(std::string& error);
    bool setSoundOperationHost(SoundOperationHost host,std::string& error);
    bool commitDeathState(int state,hit::Timer timer,hit::Timer protection,std::string& error);
    bool advancePostDeathUnit(std::string& error);
    using RespawnCallHost=std::function<bool(const death::CallRequest&,std::string&)>;
    using EntryHost=std::function<bool(int,float,std::string&)>;
    std::optional<death::RespawnPositionPrefix> beginRespawn(death::Position position,int motivation,int mode04,RespawnCallHost calls,std::string& error);
    std::optional<RawPosition> entryUnit(EntryHost entry,std::string& error);
    bool advanceEntryTimersUnit(std::string& error);
    std::optional<Movement> moveUnit(RawPosition position,Input input,FocusContext focus,std::string& error,TouchMotion touch={});
    void setPracticeInvulnerable(bool value) noexcept {practiceInvulnerable_=value;}
    bool commitPracticeDeathbomb(std::string& error);
    HitHostSnapshot snapshot() const;
    const anm::State* animationState() const;
    const anm::SpriteInfo* spriteInfo() const;
    bool configureLaserEffects(std::function<std::uint32_t()> random,std::string& error);
    bool beginLaserContactPhase(std::string& error);
    bool laserColumnContactRequired(unsigned columns) const;
    bool applyLaserColumnContact(unsigned columns,std::int32_t& motivation,std::int32_t& flag,std::string& error);
    bool endLaserContactPhase(hit::Position position,std::string& error);
    bool tickLaserEffects(std::string& error);
    std::vector<LaserContactEffect> laserEffectSnapshots(std::string& error) const;
    std::optional<PowerOutline> powerOutline(int power) const;
private:
    anm::Registry registry_;
    anm::VM animation_;
    anm::Scene laserEffects_{anm::Scene::AllocationPolicy::Native4096PoolWithHeapFallback};
    std::function<std::uint32_t()> laserRandom_;
    std::uint32_t laserEffectSerial_=1;
    std::int32_t laserPhasePrevious_=0;
    bool laserEffectsConfigured_=false,laserPhaseOpen_=false;
    bool checkLaserEffects(std::string& error);
    HitHostSnapshot state_{};
    EffectHost effects_;
    SoundHost sound_;
    SoundOperationHost soundOperation_;
    std::string error_;
    bool configured_=false,executing_=false,practiceInvulnerable_=false;
    std::int32_t previousHorizontal_=0;
    std::int32_t focusCounter_=0;
    std::array<std::uint32_t,8> entryDirty_{};
    bool fail(std::string message,std::string& output);
    anm::Runtime runtime();
    bool tickAnimation(std::string& error);
};
}
