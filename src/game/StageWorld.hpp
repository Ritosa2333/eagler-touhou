#pragma once
#include "EnemyRules.hpp"
#include "EnemyPhaseRules.hpp"
#include "SpellRules.hpp"
#include "EnemyDropRules.hpp"
#include "BulletRules.hpp"
#include "BulletManager.hpp"
#include "LaserLineHost.hpp"
#include "StageTransitionHost.hpp"
#include "OrdinaryTerminalDrain.hpp"
#include "ShotEnemyCollisionRules.hpp"
#include "ecl/EclVm.hpp"
#include "engine/Random.hpp"
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace th128 {
struct EnemySnapshot {
    std::uint32_t id=0,flags=0,originalIdentifier=0;
    std::string entry;
    float x=0,y=0,z=0,angle=0,speed=0;
    float appearanceWidth=0,appearanceHeight=0;
    std::int32_t life=0,score=0,drop=0,age=0;
    bool root=false,active=true;
    std::array<std::int32_t,16> animation{};
    std::array<std::int32_t,16> animationBanks{};
    bullet::Emitter emitter0;
    std::array<std::uint32_t,6> lifeWords{};
    std::int32_t animationLayerOffset=0;
    int protection=2;
    int deathSound=-1,deathScript=88;
    std::int32_t hitSoundOverride=-1,hitFeedbackCounter=0;
    std::uint32_t hitFeedbackAuxiliaryFlags=0;
    enemy::DropState drops;
    std::array<enemy::PhaseRecord,8> phases{};
    enemy::PhaseTimer phaseTimer{-1,0,0,0x4b6f58,1};
    std::int32_t phaseRemaining=0,phaseThreshold=0;
};

struct EnemyDamageSample {
    shotenemy::ShotEnemyQueryPlan query;
    int playerState=1,controlDamageBypass=0;
    std::optional<std::uint32_t> spellModeFlags;
};
struct SpellEvent {bool starting=false;enemy::PhaseSpell state;std::vector<std::uint8_t> title;std::array<float,3> position{};spell::Result result;bool retireFollowEffect=false;};
struct PhaseTransition {std::uint32_t frame=0,entity=0;std::string target;int life=0;bool timedOut=false;};

struct AnimationBirth {
    std::uint32_t entity=0,serial=0;
    enemy::AnimationRequest request;
};

struct EntityRetirementState {
    bool attempted=false,completed=false;
    std::uint32_t retired=0,remaining=0,vmOwners=0;
};

class StageWorld {
public:
    StageWorld();
    ~StageWorld();
    bool load(const std::string& name,const void* bytes,std::size_t size,std::string& error);
    bool start(const std::string& stage,int difficulty,std::uint16_t seed);
    bool tick();
    bool bindStageTransitionOwner(const stage::StageTransitionHost& owner,int currentId,int progress,std::string& error);
    bool bindOrdinaryTerminalDrain(const record::NativeOrdinaryTerminalOwner& owner,int stage,int progress,
            th128::stage::OrdinaryTerminalDrain::Validator validator,std::string& error);
    bool configureBullets(const void* profile,std::size_t size,bullet::Position player,
                          bullet::Manager::Constructor constructor,std::string& error);
    bool addBulletProfile(int type,const void* bytes,std::size_t size,std::string& error);
    const bullet::Manager& bullets() const;
    bool setBulletType25Conversion(bullet::Manager::Constructor owner,std::string& error);
    bool cancelAllBullets(bullet::Manager::Cancellation cancellation,std::string& error);
    bool consumeFrozenBullet(std::uint32_t slot,std::uint32_t identifier,std::string& error);
    bool enableLocalEnemyPasses(std::string& error);
    bool positionIntegrated() const;
    bool configurationOpen() const;
    using AppearanceBoundsProvider=std::function<std::optional<enemy::Bounds>(int,int)>;
    bool setAppearanceBoundsProvider(AppearanceBoundsProvider provider,std::string& error);
    using PrimaryAppearanceBoundsProvider=std::function<std::optional<enemy::Bounds>(const EnemySnapshot&,std::string&)>;
    bool setPrimaryAppearanceBoundsProvider(PrimaryAppearanceBoundsProvider provider,std::string& error);
    using AnimationConstructor=std::function<bool(const AnimationBirth&,std::string&)>;
    bool setAnimationHost(AnimationConstructor create,std::function<void(std::uint32_t)> remove,std::string& error);
    using GeometryConstructor=std::function<bool(std::uint32_t,float,std::uint32_t,std::string&)>;
    using GeometryUpdater=std::function<bool(std::uint32_t,const std::array<float,3>&,std::string&)>;
    using GeometryRemover=std::function<bool(std::uint32_t,std::string&)>;
    bool setGeometryHost(GeometryConstructor create,GeometryUpdater update,GeometryRemover remove,std::string& error);
    using EnemyContactHost=std::function<bool(const EnemySnapshot&,enemy::Bounds,std::string&)>;
    bool setEnemyContactHost(EnemyContactHost host,std::string& error);
    using EnemyDamageHost=std::function<std::optional<EnemyDamageSample>(const EnemySnapshot&,enemy::Bounds,std::string&)>;
    bool setEnemyDamageHost(EnemyDamageHost host,std::string& error);
    using EnemyHitFeedbackHost=std::function<std::optional<enemy::HitFeedbackPlan>(const EnemySnapshot&,const enemy::HitFeedbackInput&,std::string&)>;
    bool setEnemyHitFeedbackHost(EnemyHitFeedbackHost host,std::string& error);
    std::array<std::uint32_t,2> damageStats() const;
    const enemy::BossGaugeState& bossGauges() const;
    const enemy::BossSlots& bossSlots() const;
    std::array<std::uint32_t,2> phaseCountdown() const;
    enemy::PhaseTimer enemySweepTimer() const;
    std::optional<EnemySnapshot> bossEntity(std::size_t slot) const;
    bool prepareBossHudPass(std::string& error);
    using EnemyDeathHost=std::function<bool(const EnemySnapshot&,std::string&)>;
    bool setEnemyDeathHost(EnemyDeathHost host,std::string& error,bool additionalDrops=false);
    using EclDropHost=std::function<bool(const EnemySnapshot&,std::string&)>;
    bool setEclDropHost(EclDropHost host,std::string& error);
    bool setEclRadiusCancellation(bullet::Manager::Cancellation host,std::uint32_t liveLaserObjects,std::string& error);
    bool setPhaseSpellContext(enemy::PhaseSpell context,std::string& error);
    std::optional<enemy::PhaseSpell> phaseSpellContext() const;
    using SpellHost=std::function<bool(const SpellEvent&,std::string&)>;
    bool setSpellHost(SpellHost host,std::string& error);
    bool applySpellTail(std::uint32_t flags,std::uint32_t bonus,std::string& error);
    const std::vector<std::uint8_t>& spellTitle() const;
    const std::vector<PhaseTransition>& phaseTransitions() const;
    std::array<float,2> screenShake() const;
    using EclEffectHost=std::function<bool(int,int,std::array<float,3>,int,bool,std::string&)>;
    bool setEclEffectHost(EclEffectHost host,std::string& error);
    std::array<int,2> sceneControl() const;
    using MessageStart=std::function<bool(int,std::string&)>;
    using MessageGate=std::function<std::optional<bool>(std::string&)>;
    bool setMessageHost(MessageStart start,MessageGate gate,bullet::Manager::Cancellation cancellation,std::string& error);
    using AnimationVisibilityHost=std::function<bool(std::uint32_t,enemy::VisibilityEffect,std::string&)>;
    bool setAnimationVisibilityHost(AnimationVisibilityHost host,std::string& error);
    bool spawnBulletExtensionEnemy(const bullet::Birth& birth,const bullet::ExtensionRaw& record,int index,std::string& error);
    bool configureBulletUpdater(bullet::Manager::Updater updater,std::string& error);
    bool configureBulletUpdater(bullet::Manager::LegacyUpdater updater,std::string& error);
    bool configureBulletUpdater(std::nullptr_t)=delete;
    bool configureBulletUpdater(std::initializer_list<std::nullptr_t>)=delete;
    bool setBulletSoundSink(bullet::Manager::SoundSink sink,std::string& error);
    using EclSoundSink=std::function<bool(int,float,std::string&)>;
    bool setEclSoundSink(EclSoundSink sink,std::string& error);
    bool bulletUpdateConfigurationOpen() const;
    std::optional<std::uint32_t> laserObjectCount() const;
    bool setLaserHost(laser::LineHost* host,std::string& error);
    std::optional<laser::selector3::FrameInput> selector3FrameInput() const;
    bool applyLaserSpellPenalty(unsigned columns,std::string& error);
    bullet::Position playerAimPosition() const;
    bool setPlayerAimPosition(bullet::Position position,std::string& error);
    void reset();
    bool retireBulletManagerForSession(std::string& error);
    bool retireEntitiesForSession(std::string& error);
    EntityRetirementState entityRetirementState() const;
    const std::string& error() const;
    bool faulted() const;
    bool reportRuntimeFault(std::string error);
    std::uint32_t frame() const;
    std::uint32_t spawnCount() const;
    std::uint32_t lastCreatedOriginalId() const;
    std::uint32_t randomCalls() const;
    std::uint32_t effectGameRandom32();
    Random gameRandomState() const;
    bool restoreGameRandomState(Random state,std::string& error);
    std::size_t linkedSources() const;
    std::vector<EnemySnapshot> entities() const;
    const std::vector<AnimationBirth>& animationBirths() const;
    const std::vector<std::string>& animationFiles() const;
private:
    struct Entity;
    Entity* localHead_=nullptr;
    Entity* localTail_=nullptr;
    bool localEnemyPasses_=false;
    AppearanceBoundsProvider appearanceBounds_;
    PrimaryAppearanceBoundsProvider primaryAppearanceBounds_;
    AnimationConstructor animationHost_;
    std::function<void(std::uint32_t)> animationRemover_;
    GeometryConstructor geometryHost_;
    GeometryUpdater geometryUpdater_;
    GeometryRemover geometryRemover_;
    EnemyContactHost enemyContactHost_;
    EnemyDamageHost enemyDamageHost_;
    EnemyHitFeedbackHost enemyHitFeedbackHost_;
    EnemyDeathHost enemyDeathHost_;
    EclDropHost eclDropHost_;
    bullet::Manager::Cancellation eclRadiusCancellation_;
    std::optional<std::uint32_t> laserObjectCount_;
    laser::LineHost* laserHost_=nullptr;
    bool advanceLaserPhase();
    bool additionalDropsSupported_=false;
    std::array<std::uint32_t,2> damageStats_{};
    AnimationVisibilityHost animationVisibilityHost_;
    EclSoundSink eclSoundSink_;
    std::vector<std::unique_ptr<Entity>> entities_;
    std::unordered_map<std::string,ecl::Program> sources_;
    ecl::Program linked_;
    Random random_;
    bullet::Manager bullets_;
    bullet::Manager::Constructor bulletType25Conversion_;
    std::unordered_map<int,bullet::Profile> bulletProfiles_;
    bullet::Position playerPosition_;
    bool playerAimConfigured_=false;
    bool bulletsConfigured_=false;
    std::string error_;
    std::string operationError_;
    std::vector<AnimationBirth> animations_;
    std::uint32_t frame_=0,spawnCount_=0,nextId_=1,animationSerial_=0;
    std::size_t bytes_=0,birthDepth_=0;
    std::size_t instructionsThisTick_=0;
    std::array<std::int32_t,3> managerVariables_{};
    enemy::OriginalIdentityState originalIdentities_;
    enemy::BossSlots bosses_;
    enemy::BossGaugeState bossGauges_;
    std::array<std::uint32_t,2> phaseCountdown_{0xffffffffu,0};
    enemy::PhaseTimer enemySweepTimer_{-1,0,0,0x4b6f58,1};
    std::optional<enemy::PhaseSpell> phaseSpell_;
    SpellHost spellHost_;
    std::vector<std::uint8_t> spellTitle_;
    std::vector<PhaseTransition> phaseTransitions_;
    std::array<float,2> screenShake_{};
    struct ScreenShakeState {
        int duration=0,initial=0,final=0,elapsed=0;
    };
    std::vector<ScreenShakeState> shakeStates_;
    EclEffectHost eclEffectHost_;
    std::array<int,2> sceneControl_{};
    MessageStart messageStart_;
    MessageGate messageGate_;
    const stage::StageTransitionHost* stageTransitionOwner_=nullptr;
    stage::OrdinaryTerminalDrain ordinaryTerminalDrain_;
    int stageTransitionOwnerId_=0,stageTransitionOwnerProgress_=0;
    bullet::Manager::Cancellation messageCancellation_;
    int difficulty_=1;
    int spellStageDepth_=1;
    bool extraStage16Rank4SpellDomain_=false;
    bool started_=false,executing_=false;
    bool entityRetirementAttempted_=false,entityRetirementCompleted_=false,bulletRetirementAttempted_=false;
    std::uint32_t retiredEntityOwners_=0;
    bool fail(std::string message);
    Entity* create(const enemy::SpawnRequest& request,bool root);
    bool install(Entity& entity);
    bool host(Entity& entity,ecl::HostCall& call);
    std::optional<ecl::Value> reference(Entity& entity,std::int32_t identifier,bool floating);
    bool run(Entity& entity,bool allowEnemyCallback=true);
    bool runEnemyCallback(Entity& entity);
    bool updateLocal(Entity& entity);
    bool advancePhase(Entity& entity,bool immediate=false);
    bool synchronizeBoss(Entity& entity,const std::string& target);
    void appendLocal(Entity& entity);
    void retireLocal(Entity& entity);
    bool advanceBulletPhase();
};
}
