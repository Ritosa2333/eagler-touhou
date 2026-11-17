#include "StageWorld.hpp"
#include "EnemyBulletCallbackRules.hpp"
#include "EnemyRandomMovement.hpp"
#include "EnemyMotion327.hpp"
#include "practice/Thprac128Source.hpp"
#include <algorithm>
#include <bit>
#include <cmath>
#include <cstring>
#include <limits>
#include <sstream>

namespace th128 {
namespace {
int spellStageDepthForEcl(const std::string& name) noexcept {
    constexpr std::array<const char*,6> second{
        "st_a1_2.ecl","st_a2_2.ecl","st_b1_2.ecl","st_b2_2.ecl","st_c1_2.ecl","st_c2_2.ecl"};
    constexpr std::array<const char*,6> third{
        "st_a1_3.ecl","st_a2_3.ecl","st_b1_3.ecl","st_b2_3.ecl","st_c1_3.ecl","st_c2_3.ecl"};
    for(const auto* ecl:second)if(name==ecl)return 2;
    for(const auto* ecl:third)if(name==ecl)return 3;
    return 1;
}
}
struct StageWorld::Entity : enemy::Motion327Owner {
    EnemySnapshot state;
    enemy::Bounds hitbox,hurtbox;
    std::optional<enemy::MovementArea> movementArea;
    enemy::AnimationState animation;
    std::array<std::uint32_t,12> variables{};
    std::unordered_map<int,std::string> registrations;
    std::array<bullet::Emitter,16> emitters{};
    std::unique_ptr<ecl::Vm> vm;
    Entity* next=nullptr;
    bool linked=false;
    std::int32_t bossSlot=0;
    std::int32_t callbackSelector=0;
    std::string deathSubroutine;
    std::uint32_t parentIdentifier=0;
    float exclusionRadiusSquared=0;
    bool geometryActive=false;
};

StageWorld::StageWorld()=default;
StageWorld::~StageWorld()=default;
const std::string& StageWorld::error() const {return error_.empty()?operationError_:error_;}
bool StageWorld::faulted() const{return !error_.empty();}
bool StageWorld::reportRuntimeFault(std::string error){return fail(std::move(error));}
std::uint32_t StageWorld::frame() const {return frame_;}
std::uint32_t StageWorld::spawnCount() const {return spawnCount_;}
std::uint32_t StageWorld::lastCreatedOriginalId() const {return originalIdentities_.lastCreated;}
std::uint32_t StageWorld::randomCalls() const {return random_.calls;}
std::uint32_t StageWorld::effectGameRandom32(){return random_.next32();}
Random StageWorld::gameRandomState() const {return random_;}
bool StageWorld::restoreGameRandomState(Random state,std::string& error){
    error.clear();
    if(!configurationOpen()||spawnCount_!=0){error="game random restore requires a fresh started world";return false;}
    random_=state;return true;
}
std::size_t StageWorld::linkedSources() const {return linked_.sourceCount();}
const std::vector<AnimationBirth>& StageWorld::animationBirths() const {return animations_;}
const std::vector<std::string>& StageWorld::animationFiles() const {return linked_.resource().animationIncludes;}
const enemy::BossGaugeState& StageWorld::bossGauges() const{return bossGauges_;}
const enemy::BossSlots& StageWorld::bossSlots() const{return bosses_;}
std::array<std::uint32_t,2> StageWorld::phaseCountdown() const{return phaseCountdown_;}
enemy::PhaseTimer StageWorld::enemySweepTimer() const{return enemySweepTimer_;}
std::optional<EnemySnapshot> StageWorld::bossEntity(std::size_t slot) const{
    if(slot>=bosses_.owners.size()||!bosses_.owners[slot])return std::nullopt;
    for(const auto& entity:entities_)if(entity->state.id==bosses_.owners[slot]&&entity->state.active)return entity->state;
    return std::nullopt;
}
bool StageWorld::prepareBossHudPass(std::string& error){
    error.clear();if(!started_||executing_||!error_.empty()){error="Boss HUD fields require a completed world pass";return false;}
    enemy::clearUnownedBossGauges(bossGauges_,bosses_);return true;
}

bool StageWorld::fail(std::string message) {
    if(error_.empty())error_=std::move(message);
    return false;
}

void StageWorld::reset() {
    if(executing_){fail("world reset is not reentrant");return;}
    if(geometryRemover_)for(auto& entity:entities_)if(entity->geometryActive){
        std::string reason;
        try{if(!geometryRemover_(entity->state.id,reason)){fail(reason.empty()?"geometry reset retirement rejected":reason);return;}}
        catch(...){fail("geometry reset retirement host threw");return;}
        entity->geometryActive=false;
    }
    entities_.clear();animations_.clear();linked_=ecl::Program{};
    thpracPreparedRoot_=ecl::Program{};pendingThpracChapter_.reset();activeThpracChapter_.reset();
    error_.clear();operationError_.clear();frame_=spawnCount_=animationSerial_=0;nextId_=1;birthDepth_=0;
    started_=executing_=false;random_=Random{};spellStageDepth_=1;extraStage16Rank4SpellDomain_=false;
    entityRetirementAttempted_=entityRetirementCompleted_=bulletRetirementAttempted_=false;retiredEntityOwners_=0;
    instructionsThisTick_=0;managerVariables_={};originalIdentities_={};bosses_={};bossGauges_={};phaseCountdown_={0xffffffffu,0};phaseSpell_.reset();spellHost_={};spellTitle_.clear();phaseTransitions_.clear();screenShake_={};shakeStates_.clear();
    bullets_.reset();bulletType25Conversion_={};bulletProfiles_.clear();playerPosition_={};playerAimConfigured_=false;bulletsConfigured_=false;
    localHead_=localTail_=nullptr;localEnemyPasses_=false;enemySweepTimer_={-1,0,0,0x4b6f58,1};
    appearanceBounds_={};primaryAppearanceBounds_={};
    animationHost_={};animationRemover_={};
    geometryHost_={};geometryUpdater_={};geometryRemover_={};
    enemyContactHost_={};
    enemyDamageHost_={};enemyHitFeedbackHost_={};damageStats_={};
    enemyDeathHost_={};eclDropHost_={};eclRadiusCancellation_={};laserObjectCount_.reset();laserHost_=nullptr;additionalDropsSupported_=false;
    animationVisibilityHost_={};eclEffectHost_={};sceneControl_={};
    eclSoundSink_={};messageStart_={};messageGate_={};messageCancellation_={};
    stageTransitionOwner_=nullptr;stageTransitionOwnerId_=stageTransitionOwnerProgress_=0;ordinaryTerminalDrain_.reset();
}

const bullet::Manager& StageWorld::bullets() const {return bullets_;}
bool StageWorld::setBulletType25Conversion(bullet::Manager::Constructor owner,std::string& error){
    error.clear();
    if(!configurationOpen()||!bulletsConfigured_||bulletType25Conversion_||!owner){
        error="type25 conversion owner requires an unexecuted actual bullet runtime";return false;
    }
    bulletType25Conversion_=std::move(owner);return true;
}
bool StageWorld::consumeFrozenBullet(std::uint32_t slot,std::uint32_t identifier,std::string& error){
    error.clear();
    if(executing_||!started_||!bulletsConfigured_||!error_.empty()){
        error="frozen bullet transfer requires a healthy world before enemy passes";return false;
    }
    if(!bullets_.consume(slot,identifier,error))return false;
    if(phaseSpell_)spell::frozen(*phaseSpell_);return true;
}
bool StageWorld::cancelAllBullets(bullet::Manager::Cancellation cancellation,std::string& error){
    error.clear();
    if(executing_||!started_||!bulletsConfigured_||!error_.empty()){error="bullet cancellation requires a healthy world before enemy passes";return false;}
    if(!bullets_.cancelAll(std::move(cancellation),error))return false;
    if(laserHost_)return laserHost_->cancelLive(error);
    return true;
}
bool StageWorld::bindStageTransitionOwner(const stage::StageTransitionHost& owner,int currentId,int progress,std::string& error){
    error.clear();const auto state=owner.state();
    if(!started_||executing_||frame_!=0||spawnCount_!=0||!error_.empty()||stageTransitionOwner_||ordinaryTerminalDrain_.bound()
        ||!stage::StageSettlementHost::supportedStage(currentId)
        ||!state.loaded||state.begun||state.failed||state.requested||state.committed||state.messageDone){
        error="ordinary stage drain owner requires a fresh matching loaded transition";return false;
    }
    stageTransitionOwner_=&owner;stageTransitionOwnerId_=currentId;stageTransitionOwnerProgress_=progress;return true;
}
bool StageWorld::bindOrdinaryTerminalDrain(const record::NativeOrdinaryTerminalOwner& owner,int stage,int progress,
        th128::stage::OrdinaryTerminalDrain::Validator validator,std::string& error){
    error.clear();
    if(!started_||executing_||frame_!=0||!error_.empty()||stageTransitionOwner_
        ||ordinaryTerminalDrain_.bound()||entityRetirementAttempted_||bulletRetirementAttempted_){
        error="ordinary terminal World drain binding requires its fresh healthy native session";return false;
    }
    return ordinaryTerminalDrain_.bind(owner,stage,progress,std::move(validator),error);
}
bool StageWorld::positionIntegrated() const {return localEnemyPasses_;}
bool StageWorld::setEclSoundSink(EclSoundSink sink,std::string& error){
    error.clear();if(!started_||executing_||frame_!=0||spawnCount_!=0||!error_.empty()||eclSoundSink_||!sink){error="ECL sound sink requires a fresh stage";return false;}
    eclSoundSink_=std::move(sink);return true;
}
bool StageWorld::configurationOpen() const {return started_&&!executing_&&!entityRetirementAttempted_&&!bulletRetirementAttempted_&&frame_==0&&!localEnemyPasses_&&error_.empty();}
bullet::Position StageWorld::playerAimPosition() const {return playerPosition_;}
bool StageWorld::setPlayerAimPosition(bullet::Position position,std::string& error){
    error.clear();
    if(executing_||!started_||!error_.empty()||!bullet::targetAnglePrefix({},position)){error="invalid or reentrant player position update";return false;}
    playerPosition_=position;playerAimConfigured_=true;return true;
}

bool StageWorld::spawnBulletExtensionEnemy(const bullet::Birth& birth,const bullet::ExtensionRaw& record,int index,std::string& error){
    error.clear();
    if(!started_||!executing_||!localEnemyPasses_||!error_.empty()||!bullets_.isCurrentUpdatingBirth(birth)
        ||index<0||index>=18||index!=birth.state.extensionIndex||record.type!=0x01000000||!record.callbackName){
        error="ETEX16777216 enemy creation requires the canonical current bullet update owner";return false;
    }
    const auto& actual=index==0?birth.state.extension0:birth.state.laterExtensions[std::size_t(index-1)];
    if(actual.callbackName!=record.callbackName||bullet::serializeExtension(actual)!=bullet::serializeExtension(record)
        ||(record.control==0&&(birth.state.activeExtensions&~0x100u))||(std::uint32_t(record.type)&birth.state.activeExtensions)){
        error="ETEX16777216 enemy creation lost its actual dispatchable record";return false;
    }
    enemy::SpawnRequest request;request.subroutine=*record.callbackName;
    request.position={birth.state.position.x,birth.state.position.y,birth.state.position.z};request.life=10000;
    request.inheritedWords={std::uint32_t(record.parameterA),std::uint32_t(record.parameterB),std::uint32_t(record.parameterC),std::uint32_t(record.parameterD),
        std::bit_cast<std::uint32_t>(record.parameterR),std::bit_cast<std::uint32_t>(record.parameterS),
        std::bit_cast<std::uint32_t>(record.parameterT),std::bit_cast<std::uint32_t>(record.parameterU),0,0,0,0};
    if(!create(request,false)){error=error_;return false;}
    return true;
}

bool StageWorld::configureBulletUpdater(bullet::Manager::Updater updater,std::string& error) {
    error.clear();
    if(!bulletUpdateConfigurationOpen()||!updater){error="bullet update host requires configuration before the first manager pass";return false;}
    return bullets_.setUpdater(std::move(updater));
}
bool StageWorld::configureBulletUpdater(bullet::Manager::LegacyUpdater updater,std::string& error) {
    error.clear();
    if(!bulletUpdateConfigurationOpen()||!updater){error="bullet update host requires configuration before the first manager pass";return false;}
    return bullets_.setUpdater(std::move(updater));
}
bool StageWorld::setBulletSoundSink(bullet::Manager::SoundSink sink,std::string& error){
    error.clear();if(!started_||executing_||frame_!=0||!error_.empty()||!sink){error="bullet sound sink requires fresh world";return false;}
    return bullets_.setSoundSink(std::move(sink));
}
bool StageWorld::bulletUpdateConfigurationOpen() const {
    return started_&&!executing_&&frame_==0&&error_.empty()&&bulletsConfigured_&&!bullets_.hasUpdater();
}
std::optional<std::uint32_t> StageWorld::laserObjectCount() const {
    if(laserHost_)return laserHost_->actualCount();
    return laserObjectCount_;
}
bool StageWorld::setLaserHost(laser::LineHost* host,std::string& error){
    error.clear();
    if(!configurationOpen()||laserHost_||!host||!host->healthy()||host->actualCount()!=0){
        error="laser owner install requires a fresh current resource epoch and real empty manager";return false;
    }
    laserHost_=host;return true;
}
bool StageWorld::advanceLaserPhase(){
    if(!laserHost_)return true;
    std::string reason;
    if(!laserHost_->tick(reason))return fail(reason.empty()?"actual laser manager tick rejected":reason);
    return true;
}


std::optional<laser::selector3::FrameInput> StageWorld::selector3FrameInput() const{
    if(!started_||!error_.empty()||!playerAimConfigured_)return std::nullopt;
    laser::selector3::FrameInput input;input.player={playerPosition_.x,playerPosition_.y,playerPosition_.z};
    if(const auto boss=bossEntity(0))input.boss0=laser::selector3::Position{boss->x,boss->y,boss->z};
    for(const auto* entity=localHead_;entity;entity=entity->next)if(entity->state.flags&0x1000u)
        input.enemyBlockers.push_back({{entity->state.x,entity->state.y,entity->state.z},float(double(entity->hurtbox.width)*.5)});
    return input;
}
bool StageWorld::applyLaserSpellPenalty(unsigned columns,std::string& error){
    error.clear();if(!executing_||!phaseSpell_||columns==0||columns>512){error="selector3 spell penalty requires actual current phase spell during manager update";return false;}
    phaseSpell_->baseBonus=std::max(std::bit_cast<std::int32_t>(std::uint32_t(phaseSpell_->baseBonus)-columns*20u),10);
    phaseSpell_->flags|=0x100u;return true;
}

bool StageWorld::advanceBulletPhase() {
    if(!bullets_.hasUpdater())return true;
    std::string error;
    if(!bullets_.tick(error))return fail(error);
    return error_.empty();
}

bool StageWorld::setAppearanceBoundsProvider(AppearanceBoundsProvider provider,std::string& error) {
    error.clear();
    if(!configurationOpen()){error="appearance bounds require configuration before local execution";return false;}
    appearanceBounds_=std::move(provider);return true;
}
bool StageWorld::setPrimaryAppearanceBoundsProvider(PrimaryAppearanceBoundsProvider provider,std::string& error){
    error.clear();
    if(!configurationOpen()||primaryAppearanceBounds_||!provider){error="primary appearance owner requires fresh configuration";return false;}
    primaryAppearanceBounds_=std::move(provider);return true;
}

bool StageWorld::setAnimationHost(AnimationConstructor create,std::function<void(std::uint32_t)> remove,std::string& error) {
    error.clear();
    if(!configurationOpen()||!create||!remove){error="enemy animation host requires a fresh unexecuted world";return false;}
    animationHost_=std::move(create);animationRemover_=std::move(remove);return true;
}
bool StageWorld::setGeometryHost(GeometryConstructor create,GeometryUpdater update,GeometryRemover remove,std::string& error){
    error.clear();
    if(!configurationOpen()||geometryHost_||!create||!update||!remove){error="geometry526 host requires fresh explicit owning callbacks";return false;}
    for(auto& entity:entities_)
        if(!entity->vm->registerHostOpcode(526,[this,current=entity.get()](ecl::HostCall& call){return host(*current,call);},error))return false;
    geometryHost_=std::move(create);geometryUpdater_=std::move(update);geometryRemover_=std::move(remove);return true;
}
bool StageWorld::setEnemyContactHost(EnemyContactHost host,std::string& error){
    error.clear();
    if(!started_||executing_||frame_!=0||spawnCount_!=0||!error_.empty()||enemyContactHost_||enemyDamageHost_||!host){
        error="enemy contact host requires a fresh pre-pass world";return false;
    }
    enemyContactHost_=std::move(host);return true;
}
bool StageWorld::setEnemyDamageHost(EnemyDamageHost host,std::string& error){
    error.clear();
    if(!started_||executing_||frame_!=0||spawnCount_!=0||!error_.empty()||enemyContactHost_||enemyDamageHost_||!host){
        error="enemy damage host requires a fresh pre-pass world";return false;
    }
    enemyDamageHost_=std::move(host);return true;
}
std::array<std::uint32_t,2> StageWorld::damageStats() const{return damageStats_;}
bool StageWorld::setEnemyDeathHost(EnemyDeathHost host,std::string& error,bool additionalDrops){
    error.clear();
    if(!started_||executing_||frame_!=0||spawnCount_!=0||!error_.empty()||enemyDeathHost_||!host){
        error="enemy death host requires fresh explicit context";return false;
    }
    enemyDeathHost_=std::move(host);additionalDropsSupported_=additionalDrops;return true;
}
bool StageWorld::setEnemyHitFeedbackHost(EnemyHitFeedbackHost host,std::string& error){
    error.clear();
    if(!configurationOpen()||enemyHitFeedbackHost_||!host){error="enemy hit feedback requires a fresh actual primary-animation owner";return false;}
    enemyHitFeedbackHost_=std::move(host);return true;
}

bool StageWorld::setEclDropHost(EclDropHost host,std::string& error){
    error.clear();
    if(!started_||executing_||frame_!=0||spawnCount_!=0||!error_.empty()||eclDropHost_||!host){error="ECL drops require a fresh explicit owning item host";return false;}
    eclDropHost_=std::move(host);return true;
}
bool StageWorld::setEclRadiusCancellation(bullet::Manager::Cancellation host,std::uint32_t liveLaserObjects,std::string& error){
    error.clear();
    if(!started_||executing_||frame_!=0||spawnCount_!=0||!error_.empty()||eclRadiusCancellation_||!host||liveLaserObjects!=0){
        error="ECL radius cancellation requires a fresh effect host and an explicitly empty laser manager";return false;
    }
    eclRadiusCancellation_=std::move(host);laserObjectCount_=liveLaserObjects;return true;
}
bool StageWorld::setPhaseSpellContext(enemy::PhaseSpell context,std::string& error){
    error.clear();
    if(!started_||executing_||frame_!=0||spawnCount_!=0||!error_.empty()||phaseSpell_){error="phase spell context requires an explicit fresh state";return false;}
    phaseSpell_=context;return true;
}
std::optional<enemy::PhaseSpell> StageWorld::phaseSpellContext() const{return phaseSpell_;}
const std::vector<std::uint8_t>& StageWorld::spellTitle() const{return spellTitle_;}
const std::vector<PhaseTransition>& StageWorld::phaseTransitions() const{return phaseTransitions_;}
bool StageWorld::setSpellHost(SpellHost host,std::string& error){
    error.clear();if(!started_||executing_||frame_!=0||!error_.empty()||spellHost_||!host){error="spell host requires a fresh world";return false;}spellHost_=std::move(host);return true;
}
bool StageWorld::applySpellTail(std::uint32_t flags,std::uint32_t bonus,std::string& error){
    error.clear();if(executing_||!phaseSpell_){error="spell tail requires an explicit idle owning context";return false;}phaseSpell_->flags=flags;phaseSpell_->value80=bonus;return true;
}
std::array<float,2> StageWorld::screenShake() const{return screenShake_;}
std::array<int,2> StageWorld::sceneControl() const{return sceneControl_;}
bool StageWorld::setMessageHost(MessageStart start,MessageGate gate,bullet::Manager::Cancellation cancellation,std::string& error){
    error.clear();
    if(!started_||executing_||frame_!=0||spawnCount_!=0||!error_.empty()||messageStart_||!start||!gate||!cancellation){error="MSG host requires fresh real constructor, gate and cancellation owners";return false;}
    messageStart_=std::move(start);messageGate_=std::move(gate);messageCancellation_=std::move(cancellation);return true;
}
bool StageWorld::setEclEffectHost(EclEffectHost host,std::string& error){
    error.clear();
    if(!started_||executing_||frame_!=0||spawnCount_!=0||!error_.empty()||eclEffectHost_||!host){error="ECL effect host requires a fresh world";return false;}
    eclEffectHost_=std::move(host);return true;
}
bool StageWorld::setAnimationVisibilityHost(AnimationVisibilityHost host,std::string& error){
    error.clear();
    if(!started_||executing_||frame_!=0||spawnCount_!=0||!error_.empty()||animationVisibilityHost_||!host){
        error="animation visibility requires a fresh pre-pass world";return false;
    }
    animationVisibilityHost_=std::move(host);return true;
}

void StageWorld::appendLocal(Entity& entity) {
    entity.linked=true;
    entity.next=nullptr;
    if(localTail_)localTail_->next=&entity;else localHead_=&entity;
    localTail_=&entity;
}

void StageWorld::retireLocal(Entity& entity) {
    if(entity.geometryActive&&geometryRemover_){
        std::string reason;
        try{if(!geometryRemover_(entity.state.id,reason)){fail(reason.empty()?"geometry526 owning retirement rejected":reason);return;}}
        catch(...){fail("geometry526 retirement host threw");return;}
        entity.geometryActive=false;
    }
    if(entity.state.flags&0x1000000u){
        const auto slot=entity.bossSlot;
        if(slot>=0&&slot<int(bosses_.owners.size())&&bosses_.owners[std::size_t(slot)]==entity.state.id
            &&enemy::setBossSlot(bosses_,entity.state.flags,entity.bossSlot,0,-1))
            enemy::clearUnownedBossGauges(bossGauges_,bosses_);
        else entity.state.flags&=~0x1000000u;
    }
    if(entity.linked) {
        Entity* previous=nullptr;
        auto* current=localHead_;
        while(current&&current!=&entity){previous=current;current=current->next;}
        if(current) {
            if(previous)previous->next=current->next;else localHead_=current->next;
            if(localTail_==current)localTail_=previous;
        }
    }
    entity.next=nullptr;entity.linked=false;entity.state.active=false;entity.callbackSelector=0;
    if(animationRemover_) {
        try{animationRemover_(entity.state.id);}catch(...){fail("enemy animation removal host threw");return;}
    }
    entity.vm.reset();
    entity.state.animation.fill(-1);entity.state.animationBanks.fill(-1);
}

EntityRetirementState StageWorld::entityRetirementState() const {
    EntityRetirementState result{entityRetirementAttempted_,entityRetirementCompleted_,retiredEntityOwners_};
    for(const auto& entity:entities_){
        if(entity->vm)++result.vmOwners;
        if(entity->vm||entity->linked||entity->geometryActive)++result.remaining;
    }
    return result;
}

bool StageWorld::retireBulletManagerForSession(std::string& error) {
    error.clear();
    if(!started_||executing_||bulletRetirementAttempted_||entityRetirementAttempted_||!error_.empty()){
        error=error_.empty()?"bullet-manager retirement lifecycle rejected":error_;return false;
    }
    bulletRetirementAttempted_=true;bullets_.reset();bulletProfiles_.clear();bulletType25Conversion_={};
    return bullets_.activeCount()==0;
}

bool StageWorld::retireEntitiesForSession(std::string& error) {
    error.clear();
    if(!started_||executing_||entityRetirementAttempted_||!error_.empty()
        ||bullets_.activeCount()!=0||(laserHost_&&laserHost_->actualCount()!=0)){
        error=error_.empty()?"entity retirement requires its healthy owned manager and retired bullet/laser predecessors":error_;
        return false;
    }
    for(const auto& entity:entities_)if(entity->geometryActive&&!geometryRemover_){
        error="entity retirement lost its concrete geometry owner";return false;
    }
    entityRetirementAttempted_=true;executing_=true;
    auto* current=localHead_;
    while(current){
        auto* successor=current->next;
        retireLocal(*current);
        if(!error_.empty()){executing_=false;error=error_;return false;}
        ++retiredEntityOwners_;current=successor;
    }
    for(auto& entity:entities_)if(entity->vm||entity->linked||entity->geometryActive){
        retireLocal(*entity);
        if(!error_.empty()){executing_=false;error=error_;return false;}
        ++retiredEntityOwners_;
    }
    if(localHead_||localTail_){
        fail("entity retirement retained an intrusive list owner");executing_=false;error=error_;return false;
    }
    entities_.clear();animations_.clear();linked_=ecl::Program{};sources_.clear();bytes_=0;
    thpracPreparedRoot_=ecl::Program{};pendingThpracChapter_.reset();activeThpracChapter_.reset();
    appearanceBounds_={};primaryAppearanceBounds_={};animationHost_={};animationRemover_={};
    geometryHost_={};geometryUpdater_={};geometryRemover_={};
    enemyContactHost_={};enemyDamageHost_={};enemyHitFeedbackHost_={};
    enemyDeathHost_={};eclDropHost_={};animationVisibilityHost_={};
    spellHost_={};eclEffectHost_={};eclSoundSink_={};messageStart_={};messageGate_={};
    messageCancellation_={};eclRadiusCancellation_={};stageTransitionOwner_=nullptr;ordinaryTerminalDrain_.reset();
    started_=false;executing_=false;entityRetirementCompleted_=true;return true;
}

bool StageWorld::advancePhase(Entity& entity,bool immediate) {
    enemy::PhaseContext context;
    context.records=entity.state.phases;context.timer=entity.state.phaseTimer;
    context.life=entity.state.life;context.remaining=entity.state.phaseRemaining;context.threshold=entity.state.phaseThreshold;
    context.flags=entity.state.flags;context.countdown=phaseCountdown_;context.managerWord18=managerVariables_[2];context.spell=phaseSpell_;
    const auto decision=enemy::queryPhase(context);
    if(!decision)return fail("phase timeout requires explicit spell/player continuation state at 0x41e04d");
    entity.state.phases=decision->next.records;entity.state.phaseTimer=decision->next.timer;
    entity.state.phaseRemaining=decision->next.remaining;entity.state.phaseThreshold=decision->next.threshold;
    entity.state.flags=decision->next.flags;entity.state.life=decision->next.life;entity.state.lifeWords[0]=std::uint32_t(decision->next.life);
    phaseCountdown_=decision->next.countdown;managerVariables_[2]=decision->next.managerWord18;
    phaseSpell_=decision->next.spell;
    if(!decision->target.empty()){
        if(phaseTransitions_.size()>=1024)return fail("phase transition journal budget");
        phaseTransitions_.push_back({frame_,entity.state.id,decision->target,entity.state.life,decision->timedOut});
        const auto* target=linked_.find(decision->target);
        if(!target||target->instructions.empty())return fail("invalid phase continuation entry: "+decision->target);
        if(!entity.vm->start(decision->target,std::uint8_t(1u<<difficulty_)))return fail("phase ECL context reset failed: "+decision->target);
        entity.state.entry=decision->target;
        entity.state.active=true;
        if(immediate&&!run(entity))return false;
    }
    return true;
}

bool StageWorld::synchronizeBoss(Entity& entity,const std::string& target){
    const auto* subroutine=linked_.find(target);
    if(!subroutine||subroutine->instructions.empty())return fail("ECL613 cross-Boss target is missing: "+target);
    entity.state.entry=target;
    entity.state.active=true;
    entity.state.phaseTimer={-1,0,0,0x4b6f58,1};
    entity.state.phaseRemaining=0;entity.state.phaseThreshold=0;
    entity.state.flags&=~0x2000000u;
    entity.registrations.clear();
    if(!entity.vm->start(target,std::uint8_t(1u<<difficulty_)))
        return fail("ECL613 cross-Boss restart failed: "+target+": "+entity.vm->error().message);
    return true;
}

bool StageWorld::updateLocal(Entity& entity) {
    if(entity.state.flags&0x80000U)return true;
    if(entity.state.flags&0x4000000U){retireLocal(entity);return true;}
    if(entity.state.flags&0x8000000U)return fail("unsupported enemy transform update");
    entity.state.flags|=0x80000U;
    if(!enemy::stepPolarInterpolation(entity.polar,entity.polarInterpolation))return fail("unsupported enemy polar interpolation update");
    if(!enemy::stepPolarInterpolation(entity.relativePolar,entity.relativePolarInterpolation))return fail("unsupported enemy relative polar interpolation update");
    entity.remainingMotionDurations[1]=entity.relativePolarInterpolation.duration;
    entity.remainingMotionDurations[3]=entity.relativePolarInterpolation.duration;
    auto& absoluteVelocity=entity.absoluteVelocity;
    auto& relativeVelocity=entity.relativeVelocity;
    auto& relativePolar=entity.relativePolar;
    enemy::Position combinedVelocity;
    if(entity.positionInterpolation.duration>0){
        if(!enemy::advanceInterpolatedPosition(entity.absolute,absoluteVelocity,entity.positionInterpolation))return fail("unsupported enemy301 position interpolation");
    }else if(entity.polar.modeFlags&1u){
        if(!enemy::advanceCircularPosition(entity.absolute,absoluteVelocity,entity.polar))return fail("unsupported enemy circular motion update");
    }else if(!enemy::advancePolarPosition(entity.absolute,absoluteVelocity,entity.polar)){
        std::ostringstream diagnostic;
        diagnostic.precision(9);diagnostic<<"unsupported enemy motion update entity="<<entity.state.id
            <<" entry="<<entity.state.entry<<" polarMode="<<entity.polar.modeFlags
            <<" angle="<<entity.polar.angle<<" speed="<<entity.polar.speed
            <<" absolute="<<entity.absolute.x<<","<<entity.absolute.y<<","<<entity.absolute.z
            <<" interpolationMode="<<entity.polarInterpolation.mode<<" elapsed="<<entity.polarInterpolation.elapsed
            <<" duration="<<entity.polarInterpolation.duration<<" startAngle="<<entity.polarInterpolation.startAngle
            <<" angleTarget="<<entity.polarInterpolation.targetAngle<<" stateFlags="<<entity.state.flags;
        return fail(diagnostic.str());
    }
    if(relativePolar.modeFlags&1u){
        if(!enemy::advanceCircularPosition(entity.relative,relativeVelocity,relativePolar))return fail("unsupported enemy relative circular motion update");
    }else if(!enemy::advancePolarPosition(entity.relative,relativeVelocity,relativePolar))return fail("unsupported enemy relative motion update");
    enemy::Position combined{entity.state.x,entity.state.y,entity.state.z};
    if(entity.state.flags&0x40000u){
        if(!entity.movementArea||!enemy::combinePositionWithArea(combined,combinedVelocity,entity.absolute,entity.relative,*entity.movementArea))
            return fail("unsupported enemy404 movement area update");
    }else if(!enemy::combinePosition(combined,combinedVelocity,entity.absolute,entity.relative,entity.state.flags))return fail("unsupported enemy combined position update");
    entity.state.x=combined.x;entity.state.y=combined.y;entity.state.z=combined.z;
    entity.state.angle=entity.polar.angle;entity.state.speed=entity.polar.speed;
    enemy::Bounds appearance;
    if(primaryAppearanceBounds_){
        appearance={entity.state.appearanceWidth,entity.state.appearanceHeight};
        if(entity.state.animation[0]>=0){
            std::string reason;std::optional<enemy::Bounds> resolved;
            try{resolved=primaryAppearanceBounds_(entity.state,reason);}
            catch(...){return fail("actual primary appearance owner threw");}
            if(!reason.empty()||!error_.empty())return fail(reason.empty()?"actual primary appearance owner faulted":reason);
            if(resolved)appearance=*resolved;
        }
    }else if(entity.state.animation[0]>=0) {
        if(!appearanceBounds_)return fail("enemy appearance bounds are unavailable before ECL");
        std::optional<enemy::Bounds> resolved;
        try{resolved=appearanceBounds_(entity.state.animationBanks[0],entity.state.animation[0]);}
        catch(...){return fail("enemy appearance bounds provider threw");}
        if(!resolved)return fail("unsupported enemy animated appearance bounds before ECL");
        appearance=*resolved;
    }
    if(!std::isfinite(appearance.width)||!std::isfinite(appearance.height)||appearance.width<0||appearance.height<0
        ||appearance.width>4096||appearance.height>4096)return fail("invalid enemy appearance bounds");
    entity.state.appearanceWidth=appearance.width;entity.state.appearanceHeight=appearance.height;
    const auto halfWidth=double(appearance.width)*.5;
    const auto halfHeight=double(appearance.height)*.5;
    const bool outsideX=double(entity.state.x)+halfWidth< -192.0||double(entity.state.x)-halfWidth>192.0;
    const bool outsideY=double(entity.state.y)+halfHeight<0.0||double(entity.state.y)-halfHeight>448.0;
    if(outsideX) {
        if((entity.state.flags&0x20000U)&&!(entity.state.flags&4U)){retireLocal(entity);return true;}
    } else if(outsideY) {
        if((entity.state.flags&0x20000U)&&!(entity.state.flags&8U)){retireLocal(entity);return true;}
    } else entity.state.flags|=0x20000U;
    if(!error_.empty())return false;
    if(!run(entity))return false;
    entity.state.flags&=~0x400000u;
    if(!entity.state.root&&entity.state.active&&!(entity.state.flags&0x21u)&&enemyContactHost_){
        std::string reason;
        try{if(!enemyContactHost_(entity.state,entity.hitbox,reason))return fail(reason.empty()?"enemy contact host rejected":reason);}
        catch(...){return fail("enemy contact host threw");}
    }
    bool phaseSwitched=false;
    if(!entity.state.root&&entity.state.active&&!(entity.state.flags&0x21u)&&enemyDamageHost_){
        std::string reason;std::optional<EnemyDamageSample> sample;
        try{sample=enemyDamageHost_(entity.state,entity.hitbox,reason);}
        catch(...){return fail("enemy damage host threw");}
        if(!sample||!reason.empty()||!error_.empty())return fail(reason.empty()?"enemy damage host rejected":reason);
        const auto plan=shotenemy::planEnemyDamageAfterCompletedQuery(sample->query,{sample->playerState,sample->controlDamageBypass,
            entity.state.flags,entity.state.protection,entity.state.lifeWords,damageStats_,sample->spellModeFlags});
        entity.state.lifeWords=plan.nextLifeWords;entity.state.life=std::bit_cast<std::int32_t>(plan.nextLifeWords[0]);
        damageStats_=plan.nextManagerStats;
        if(plan.continuation==shotenemy::DamageContinuation::NeedsCompletedQuery)return fail("enemy damage requires a completed real shot query");
        if(plan.continuation==shotenemy::DamageContinuation::NeedsSpellModeSnapshot)return fail("unresolved spell-mode snapshot at 0x416619");
        if(plan.continuation==shotenemy::DamageContinuation::NeedsNativeContinuation){
            const auto transitions=phaseTransitions_.size();
            if(!advancePhase(entity,true))return false;
            phaseSwitched=phaseTransitions_.size()!=transitions;
            if(!entity.state.active){retireLocal(entity);return error_.empty();}
            entity.state.lifeWords[2]=entity.state.lifeWords[0];entity.state.lifeWords[4]=0;
            if(entity.state.life<=0&&!(entity.state.flags&0x80u)){
                if(entity.state.flags&0x1000000u){
                    if(!phaseSwitched)return fail("registered boss death/phase continuation has no authored target");
                }else{
                    if(!additionalDropsSupported_&&std::any_of(entity.state.drops.words.begin()+1,entity.state.drops.words.end(),[](auto count){return count!=0;}))
                        return fail("unimplemented additional drop list lifecycle at 0x4148e0");
                    if(!enemyDeathHost_)return fail("unimplemented enemy death lifecycle at 0x417370");
                    try{if(!enemyDeathHost_(entity.state,reason)||!reason.empty()||!error_.empty())return fail(reason.empty()?"enemy death host rejected":reason);}
                    catch(...){return fail("enemy death host threw");}
                    if(additionalDropsSupported_)enemy::resetAdditionalDrops(entity.state.drops);
                    if(!entity.deathSubroutine.empty()){
                        const std::string target=entity.deathSubroutine;
                        entity.state.entry=target;entity.state.active=true;
                        if(!entity.vm->start(target,std::uint8_t(1u<<difficulty_)))return fail("original463 deathscript VM restart failed: "+target);
                        if(!run(entity,false))return false;
                        entity.deathSubroutine.clear();
                    }
                    retireLocal(entity);return error_.empty();
                }
            }
            entity.state.flags|=0x400000u;
        }
    }
    if(!phaseSwitched&&!advancePhase(entity))return false;
    if(entity.geometryActive){
        std::string reason;
        try{if(!geometryUpdater_||!geometryUpdater_(entity.state.id,{entity.state.x,entity.state.y,entity.state.z},reason))
            return fail(reason.empty()?"geometry526 owning update rejected":reason);}
        catch(...){return fail("geometry526 update host threw");}
    }
    if((entity.state.flags&0x200000u)&&entity.state.animation[0]>=0){
        // Original 0x4168cb selects authored motion scripts after the enemy pass.
        const int direction=combinedVelocity.x< -0.029999999329447746?-1:
            combinedVelocity.x>0.029999999329447746?1:0;
        const int previous=entity.animation.motionSelector;
        if(previous< -1||previous>1)return fail("invalid original primary animation motion selector");
        if(direction!=previous){
            const int relative=previous==0?(direction<0?1:2):
                previous<0?(direction==0?3:2):(direction==0?4:1);
            const auto script=std::int64_t(entity.animation.baseScript)+relative;
            if(script<0||script>std::numeric_limits<int>::max()||!animationHost_)
                return fail("original primary motion animation requires its actual owner");
            auto binding=entity.animation;binding.bank=entity.state.animationBanks[0];
            const auto request=enemy::animationSlot(binding,0,int(script),entity.state.flags);
            if(!request)return fail("invalid original primary motion animation binding");
            std::string reason;const AnimationBirth birth{entity.state.id,animationSerial_+1,*request};
            try{if(!animationHost_(birth,reason))return fail(reason.empty()?"original primary motion animation owner rejected":reason);}
            catch(...){return fail("original primary motion animation owner threw");}
            entity.animation.motionSelector=direction;
            entity.state.animation[0]=request->script;entity.state.animationBanks[0]=request->bank;
            animations_.push_back({entity.state.id,++animationSerial_,*request});
        }
    }
    if(enemyHitFeedbackHost_){
        enemy::HitFeedbackInput input;input.soundOverride=entity.state.hitSoundOverride;input.counter=entity.state.hitFeedbackCounter;
        input.enemyFlags=entity.state.flags;input.auxiliaryFlags=entity.state.hitFeedbackAuxiliaryFlags;
        input.lifeWord500c=std::bit_cast<std::int32_t>(entity.state.lifeWords[3]);input.phaseClock=entity.state.phaseTimer.current;
        if(phaseSpell_)input.phaseFlags=phaseSpell_->flags;
        std::string reason;std::optional<enemy::HitFeedbackPlan> feedback;
        try{feedback=enemyHitFeedbackHost_(entity.state,input,reason);}
        catch(...){return fail("actual enemy hit-feedback owner threw");}
        if(!feedback||!reason.empty()||!error_.empty())return fail(reason.empty()?"actual primary hit-feedback owner rejected":reason);
        entity.state.hitFeedbackCounter=feedback->counter;
        if(feedback->sound){
            if(!eclSoundSink_)return fail("original hit feedback lost its real sound request sink");
            try{if(!eclSoundSink_(*feedback->sound,entity.state.x,reason)||!reason.empty()||!error_.empty())return fail(reason.empty()?"original hit sound request rejected":reason);}
            catch(...){return fail("original hit sound request owner threw");}
        }
    }
    if(entity.state.phaseTimer.current==std::numeric_limits<std::int32_t>::max())return fail("native phase timer exceeded bounded integer clock");
    entity.state.phaseTimer.previous=entity.state.phaseTimer.current;
    ++entity.state.phaseTimer.current;entity.state.phaseTimer.fraction=float(double(entity.state.phaseTimer.fraction)+1.0);
    if(entity.state.protection>0)--entity.state.protection;
    if(!entity.state.active) {
        if(entity.linked)retireLocal(entity);
        else entity.state.active=true;
    }
    return true;
}

bool StageWorld::enableLocalEnemyPasses(std::string& error) {
    error.clear();
    if(executing_||!started_||frame_!=0||spawnCount_!=0||entities_.size()!=1||!error_.empty()||localEnemyPasses_) {
        error="local enemy passes must be enabled once before the first world tick";return false;
    }
    localEnemyPasses_=true;instructionsThisTick_=0;executing_=true;
    const bool success=updateLocal(*entities_.front());
    if(success)appendLocal(*entities_.front());
    executing_=false;
    if(!success)error=error_;
    return success;
}

bool StageWorld::configureBullets(const void* profile,std::size_t size,bullet::Position player,
                                  bullet::Manager::Constructor constructor,std::string& error) {
    error.clear();
    if(!configurationOpen()||!constructor) {
        error="bullet runtime must be configured before local enemy execution or the first world tick";return false;
    }
    bullet::Profile candidate;
    if(!candidate.load(profile,size,error))return false;
    if(!bullet::targetAnglePrefix({},player)){error="invalid explicit player position";return false;}
    bulletProfiles_[8]=std::move(candidate);playerPosition_=player;playerAimConfigured_=true;
    bullets_.setConstructor(std::move(constructor));bulletsConfigured_=true;
    return true;
}
bool StageWorld::addBulletProfile(int type,const void* bytes,std::size_t size,std::string& error){
    error.clear();
    if(!started_||executing_||frame_!=0||spawnCount_!=0||!bulletsConfigured_||!error_.empty()){
        error="typed bullet profiles require an unexecuted healthy stage";return false;
    }
    bullet::Profile candidate;
    if(!candidate.loadTyped(type,bytes,size,error))return false;
    bulletProfiles_[type]=std::move(candidate);return true;
}

bool StageWorld::load(const std::string& name,const void* bytes,std::size_t size,std::string& error) {
    if(started_||executing_||entityRetirementAttempted_||pendingThpracChapter_||name.size()>200||name.empty()||name.find_first_of("/\\:")!=std::string::npos||name.find("..")!=std::string::npos) {
        error="world resource loading requires reset and a flat resource name";
        return false;
    }
    if(sources_.size()>=64&&!sources_.contains(name)){error="world ECL resource-count budget";return false;}
    const auto found=sources_.find(name);
    const std::size_t previous=found==sources_.end()?0:found->second.sourceBytes();
    if(size>8u*1024u*1024u-(bytes_-previous)){error="world ECL aggregate byte budget";return false;}
    ecl::Program candidate;
    if(!candidate.load(bytes,size,error))return false;
    sources_[name]=std::move(candidate);bytes_=bytes_-previous+size;
    return true;
}

bool StageWorld::prepareThpracChapter(unsigned stageId,unsigned chapter,int difficulty,std::string& error) {
    error.clear();
    if(started_||executing_||entityRetirementAttempted_||bulletRetirementAttempted_
        ||faulted()||pendingThpracChapter_||activeThpracChapter_) {
        error="TH128 thprac chapter requires a fresh healthy unconfigured World";return false;
    }
    practice::ChapterPlan plan;
    if(!practice::chapterPlan(stageId,chapter,plan,error))return false;
    if(difficulty<0||difficulty>(stageId==16?4:3)||(stageId==16&&difficulty!=4)) {
        error="TH128 thprac stage/rank mismatch";return false;
    }
    const auto found=sources_.find(plan.sourceName);
    if(found==sources_.end()) {error="TH128 thprac original stage resource is missing";return false;}
    if(!practice::originalChapterSource(stageId,found->second,error))return false;
    ecl::Program candidate;
    if(!practice::prepareChapter(found->second,plan.sourceName,found->second.resource().bytes,
        stageId,chapter,candidate,error))return false;
    thpracPreparedRoot_=std::move(candidate);
    pendingThpracChapter_=ThpracChapter{stageId,chapter,difficulty,true};
    return true;
}

bool StageWorld::start(const std::string& stage,int difficulty,std::uint16_t seed) {
    if(entityRetirementAttempted_){operationError_="retired entity manager cannot restart";return false;}
    if(executing_)return fail("world start is not reentrant");
    if(activeThpracChapter_) {operationError_="active TH128 thprac World requires explicit reset";return false;}
    if(pendingThpracChapter_) {
        practice::ChapterPlan plan;std::string reason;
        if(!practice::chapterPlan(pendingThpracChapter_->stageId,pendingThpracChapter_->chapter,plan,reason)
            ||stage!=plan.sourceName||difficulty!=pendingThpracChapter_->difficulty) {
            operationError_="TH128 thprac start does not match its pending owner";return false;
        }
    }
    const auto source=sources_.find(stage);
    if(source==sources_.end()||difficulty<0||difficulty>4){operationError_="missing stage or invalid difficulty";return false;}
    auto candidate=pendingThpracChapter_?thpracPreparedRoot_:source->second;
    const auto practiceOwner=pendingThpracChapter_;
    std::vector<std::pair<std::string,ecl::Program>> providers(sources_.begin(),sources_.end());
    std::string error;
    if(!candidate.linkIncludes(providers,error)){operationError_=error;return false;}
    const auto* entry=candidate.find("main");
    if(!entry||entry->instructions.empty()){operationError_="stage main is missing or empty";return false;}
    reset();
    linked_=std::move(candidate);
    difficulty_=difficulty;random_.seed=seed;
    spellStageDepth_=spellStageDepthForEcl(stage);
    extraStage16Rank4SpellDomain_=stage=="st_ex.ecl"&&difficulty==4;
    enemy::SpawnRequest root;
    root.subroutine="main";
    started_=true;
    if(!create(root,true)||!error_.empty())return false;
    activeThpracChapter_=practiceOwner;
    return true;
}

StageWorld::Entity* StageWorld::create(const enemy::SpawnRequest& request,bool root) {
    if(entities_.size()>=4096||birthDepth_>=32||nextId_==0)return fail("world entity/birth-depth budget"),nullptr;
    if(request.mode>1)return fail("spawn mode lifecycle is not implemented: "+std::to_string(request.mode)),nullptr;
    auto entity=std::make_unique<Entity>();
    auto& state=entity->state;
    state.id=nextId_++;state.entry=request.subroutine;state.root=root;
    state.originalIdentifier=originalIdentities_.construct();
    if(request.mode==1)state.flags|=0x100000U;
    if(request.life>=1000)state.flags|=0x80000000U;
    state.x=request.position.x;state.y=request.position.y;state.z=request.position.z;
    state.life=request.life;state.score=request.score;state.drop=request.drop;
    state.drops.words[0]=request.drop;
    state.lifeWords={std::uint32_t(request.life),std::uint32_t(request.life),0,0,0,0};
    state.deathSound=3+int((state.id-2u)&1u);
    state.animation.fill(-1);state.animationBanks.fill(-1);
    entity->absolute=request.position;
    entity->variables=request.inheritedWords;
    entity->parentIdentifier=request.contextTag;
    ecl::Limits gameVmLimits;gameVmLimits.maxThreads=32;
    entity->vm=std::make_unique<ecl::Vm>(linked_,gameVmLimits);
    {
        std::string hostDomainError;
        if(!entity->vm->enableOriginalHostMissDomain(hostDomainError))return fail(hostDomainError),nullptr;
        if(extraStage16Rank4SpellDomain_&&!entity->vm->enableOriginalAsyncRegistry(hostDomainError))return fail(hostDomainError),nullptr;
    }
    auto* result=entity.get();
    entities_.push_back(std::move(entity));
    if(!linked_.find(request.subroutine))return fail("missing spawn subroutine: "+std::string(request.subroutine)),nullptr;
    if(!install(*result)||!result->vm->start(state.entry,std::uint8_t(1u<<difficulty_))) {
        fail("unable to install or start world ECL context");return nullptr;
    }
    if(!root) {
        ++spawnCount_;++birthDepth_;
        const bool success=localEnemyPasses_?updateLocal(*result):run(*result);
        --birthDepth_;
        if(!success)return nullptr;
        if(localEnemyPasses_&&result->state.active)appendLocal(*result);
    }
    return result;
}

bool StageWorld::install(Entity& entity) {
    std::string error;
    if(!entity.vm->setReferenceResolver([this,&entity](std::int32_t id,bool floating){return reference(entity,id,floating);},
        [this,&entity](std::int32_t id,ecl::Value value){
            if(id>=-9949&&id<=-9947&&value.type==ecl::ValueType::Integer){managerVariables_[std::size_t(id+9949)]=value.asInteger();return true;}
            if(id>=-9985&&id<=-9982&&value.type==ecl::ValueType::Integer){entity.variables[std::size_t(id+9985)]=value.bits;return true;}
            if(id==-9979||id==-9978){
                if(value.type!=ecl::ValueType::Float||!std::isfinite(value.asFloat()))return false;
                entity.variables[std::size_t(6+id+9979)]=value.bits;return true;
            }
            if((id!=-9981&&id!=-9980)||value.type!=ecl::ValueType::Float||!std::isfinite(value.asFloat()))return false;
            entity.variables[std::size_t(4+id+9981)]=value.bits;return true;
        },error))return fail(error);
    for(const auto opcode:{22,256,257,258,259,260,261,262,263,269,274,283,300,301,308,309,310,311,312,327,400,401,402,403,404,405,406,407,408,409,410,411,412,413,414,415,416,417,418,419,420,421,422,423,424,425,426,427,428,613,304,305,435,436,437,438,439,440,442,443,445,448,452,453,463,500,501,502,503,504,505,506,507,508,509,510,511,512,513,521,522,523,524,525,529,534,537,538,600,601,602,604,608,609,614,615})
        if(!entity.vm->registerHostOpcode(std::uint16_t(opcode),[this,&entity](ecl::HostCall& call){return host(entity,call);},error))return fail(error);
    if(geometryHost_&&!entity.vm->registerHostOpcode(526,[this,&entity](ecl::HostCall& call){return host(entity,call);},error))return fail(error);
    return true;
}

std::optional<ecl::Value> StageWorld::reference(Entity& entity,std::int32_t identifier,bool floating) {
    if(identifier==-9988){
        const auto& timer=entity.state.phaseTimer;
        if(!floating)return ecl::Value::integer(timer.current);
        if(!std::isfinite(timer.fraction))return std::nullopt;
        return ecl::Value::floating(timer.fraction);
    }
    if(identifier==-9971){
        const float angle=entity.polar.angle;
        if(!std::isfinite(angle))return std::nullopt;
        if(floating)return ecl::Value::floating(angle);
        const double wide=angle;
        if(wide<double(std::numeric_limits<std::int32_t>::min())||wide>double(std::numeric_limits<std::int32_t>::max()))return std::nullopt;
        return ecl::Value::integer(static_cast<std::int32_t>(wide));
    }
    if(identifier==-9931)return floating?ecl::Value::floating(originalIdentities_.floating()):ecl::Value::integer(originalIdentities_.integer());
    if(identifier==-9986)return ecl::Value::integer((entity.state.flags>>25)&1u);
    if(identifier==-9909)return floating?ecl::Value::floating(float(entity.parentIdentifier)):
        ecl::Value::integer(std::bit_cast<std::int32_t>(entity.parentIdentifier));
    if(identifier>=-9949&&identifier<=-9947)return ecl::Value::integer(managerVariables_[std::size_t(identifier+9949)]);
    if(identifier==-9946){
        std::uint32_t count=0;
        for(const auto& candidate:entities_)if(candidate->state.active)++count;
        return floating?ecl::Value::floating(float(count)):ecl::Value::integer(std::int32_t(count));
    }
    if(identifier==-9944){
        if(!playerAimConfigured_)return std::nullopt;
        const float dx=playerPosition_.x-entity.state.x,dy=playerPosition_.y-entity.state.y;
        if(!std::isfinite(dx)||!std::isfinite(dy))return std::nullopt;
        const float distance=float(std::sqrt(double(dx*dx+dy*dy)));
        if(!std::isfinite(distance))return std::nullopt;
        if(floating)return ecl::Value::floating(distance);
        const double wide=distance;
        if(wide<double(std::numeric_limits<std::int32_t>::min())
            ||wide>double(std::numeric_limits<std::int32_t>::max()))return std::nullopt;
        return ecl::Value::integer(static_cast<std::int32_t>(wide));
    }
    if(identifier==-9943){
        const auto owner=bosses_.owners[0];
        if(owner==0)return floating?ecl::Value::floating(0.f):ecl::Value::integer(0);
        for(const auto& candidate:entities_)if(candidate->state.id==owner){
            if(!candidate->state.active)return std::nullopt;
            const auto value=std::bit_cast<std::int32_t>(candidate->variables[0]);
            return floating?ecl::Value::floating(float(value)):ecl::Value::integer(value);
        }
        return std::nullopt;
    }
    if(identifier==-9942){
        const auto owner=bosses_.owners[0];
        if(owner==0)return floating?ecl::Value::floating(0.f):ecl::Value::integer(0);
        for(const auto& candidate:entities_)if(candidate->state.id==owner){
            if(!candidate->state.active)return std::nullopt;
            const auto value=std::bit_cast<std::int32_t>(candidate->variables[1]);
            return floating?ecl::Value::floating(float(value)):ecl::Value::integer(value);
        }
        return std::nullopt;
    }
    if(identifier==-9940){
        const auto owner=bosses_.owners[0];
        if(owner==0)return ecl::Value::integer(0);
        for(const auto& candidate:entities_)if(candidate->state.id==owner){
            if(!candidate->state.active)return std::nullopt;
            return ecl::Value::integer(std::bit_cast<std::int32_t>(candidate->variables[3]));
        }
        return std::nullopt;
    }
    if(identifier==-9954)return ecl::Value::integer(std::bit_cast<std::int32_t>(entity.state.lifeWords[0]));
    if(identifier==-9959)return ecl::Value::integer(difficulty_);
    if(identifier>=-9953&&identifier<=-9950)return ecl::Value::integer(difficulty_==identifier+9953);
    if(identifier>=-9985&&identifier<=-9982)return ecl::Value::integer(std::bit_cast<std::int32_t>(entity.variables[std::size_t(identifier+9985)]));
    if(identifier==-9979||identifier==-9978){
        const float value=std::bit_cast<float>(entity.variables[std::size_t(6+identifier+9979)]);
        if(floating)return ecl::Value::floating(value);
        const double wide=value;
        if(!std::isfinite(wide)||wide<double(std::numeric_limits<std::int32_t>::min())
            ||wide>double(std::numeric_limits<std::int32_t>::max()))return std::nullopt;
        return ecl::Value::integer(static_cast<std::int32_t>(wide));
    }
    if(!floating) {
        if(identifier==-10000)return ecl::Value::integer(std::int32_t(random_.next32()&0x7FFFFFFF));
        return std::nullopt;
    }
    if(playerAimConfigured_&&(identifier==-9965||identifier==-9964))return ecl::Value::floating(identifier==-9965?playerPosition_.x:playerPosition_.y);
    if(identifier==-9963||identifier==-9962){
        const std::uint32_t owner=bosses_.owners[0];
        if(owner)
            for(const auto& candidate:entities_)
                if(candidate->state.id==owner)
                    return ecl::Value::floating(identifier==-9963?candidate->state.x:candidate->state.y);
        return ecl::Value::floating(identifier==-9963?0.f:128.f);
    }
    switch(identifier) {
    case -10000:return ecl::Value::floating(float(random_.next32()&0x7FFFFFFF));
    case -9999:return ecl::Value::floating(random_.unit());
    case -9998:return ecl::Value::floating(random_.signedAngle());
    case -9987:return ecl::Value::floating(random_.signedUnit());
    case -9997:return ecl::Value::floating(entity.state.x);
    case -9996:return ecl::Value::floating(entity.state.y);
    case -9995:return ecl::Value::floating(entity.absolute.x);
    case -9994:return ecl::Value::floating(entity.absolute.y);
    case -9993:return ecl::Value::floating(entity.relative.x);
    case -9992:return ecl::Value::floating(entity.relative.y);
    case -9981:case -9980:return ecl::Value::floating(std::bit_cast<float>(entity.variables[std::size_t(4+identifier+9981)]));
    case -9989:{
        if(!bulletsConfigured_)return std::nullopt;
        const auto angle=bullet::targetAnglePrefix({entity.state.x,entity.state.y,entity.state.z},playerPosition_);
        if(!angle)return std::nullopt;
        return ecl::Value::floating(*angle);
    }
    default:return std::nullopt;
    }
}

bool StageWorld::host(Entity& entity,ecl::HostCall& call) {
    const auto& instruction=call.instruction();
    const auto opcode=instruction.opcode;
    const auto fixed=[&](std::size_t parameters){return instruction.parameterCount==parameters&&instruction.rawWords.size()==parameters;};
    if(opcode==418){
        const auto liveLasers=laserObjectCount();
        int entry=0;if(!fixed(1)||!call.readInteger(0,entry)||!messageStart_||!messageCancellation_||(!liveLasers&&bulletsConfigured_))return false;
        std::string reason;
        try{if(!messageStart_(entry,reason)||!reason.empty()||!error_.empty())return fail(reason.empty()?"original MSG constructor rejected":reason);}
        catch(...){return fail("original MSG constructor threw");}
        if(!bullets_.cancelAll(messageCancellation_,reason))return fail(reason);
        if(liveLasers&&*liveLasers!=0){
            if(!laserHost_||!laserHost_->healthy())return fail("MSG start requires actual current42a820laserowner");
            if(!laserHost_->cancelLive(reason,false,false,true))return fail(reason.empty()?"originalMSG42a820lasercancellationrejected":reason);
        }
        for(auto* target=localHead_;target;target=target->next){
            const auto flags=target->state.flags;
            if((flags&0x100u)||(!(flags&0xa0u)&&!(flags&0x1800400u))){
                if(target->state.deathScript>=0&&eclEffectHost_&&!eclEffectHost_(0,target->state.deathScript,{target->state.x,target->state.y,target->state.z},4,false,reason))return fail(reason);
                target->state.flags|=0x4000000u;
            }
        }
        return true;
    }
    if(opcode==426){
        float radius=0;if(!fixed(1)||!call.readFloat(0,radius)||!std::isfinite(radius))return false;
        const float squared=float(double(radius)*radius);
        if(!std::isfinite(squared))return fail("ECL426 squared exclusion overflow");
        entity.exclusionRadiusSquared=squared;return true;
    }
    if(opcode==425){
        if(!fixed(0)||!localEnemyPasses_)return false;
        if(enemySweepTimer_.current==std::numeric_limits<std::int32_t>::max())return fail("ECL425 manager timer exceeded bounded integer clock");
        std::vector<Entity*> selected;
        for(auto* target=localHead_;target;target=target->next){
            const auto flags=target->state.flags;
            if(!(flags&0x100u)&&((flags&0xa0u)||(flags&0x1800400u)))continue;
            if(!target->registrations.empty())return fail("ECL425 deferred ECL restart owner is not registered");
            if(target->state.deathScript>=0&&!eclEffectHost_)return fail("ECL425 requires the original enemy effect owner");
            selected.push_back(target);
        }
        for(auto* target:selected){
            std::string reason;
            if(target->state.deathScript>=0){
                try{if(!eclEffectHost_(0,target->state.deathScript,{target->state.x,target->state.y,target->state.z},4,false,reason)
                    ||!reason.empty()||!error_.empty())return fail(reason.empty()?"ECL425 owned enemy effect rejected":reason);}
                catch(...){return fail("ECL425 owned enemy effect threw");}
            }
        }
        for(auto* target:selected)target->state.flags|=0x4000000u;
        enemySweepTimer_.previous=enemySweepTimer_.current;
        ++enemySweepTimer_.current;enemySweepTimer_.fraction=float(double(enemySweepTimer_.fraction)+1.0);
        return true;
    }
    if(opcode==600){
        std::int32_t slot=0;laser::LineConfiguration configuration;
        if(!fixed(5)||!call.readInteger(0,slot)||slot<0||slot>=int(entity.emitters.size())
            ||!call.readFloat(1,configuration.initialLength)||!call.readFloat(2,configuration.maximumLength)
            ||!call.readFloat(3,configuration.travelLimit)||!call.readFloat(4,configuration.width)
            ||!std::isfinite(configuration.initialLength)||!std::isfinite(configuration.maximumLength)
            ||!std::isfinite(configuration.travelLimit)||!std::isfinite(configuration.width))return false;
        auto& emitter=entity.emitters[std::size_t(slot)];
        emitter.positionX=configuration.initialLength;emitter.positionY=configuration.maximumLength;
        emitter.positionZ=configuration.travelLimit;emitter.laserWidth=configuration.width;
        entity.state.emitter0=entity.emitters[0];return true;
    }
    if(opcode==601){
        int slot=0;if(!fixed(6)||!call.readInteger(0,slot)||slot<0||slot>=int(entity.emitters.size()))return false;
        for(std::size_t index=0;index<5;++index)if(!call.readInteger(index+1,entity.emitters[std::size_t(slot)].laserTimers[index]))return false;
        entity.state.emitter0=entity.emitters[0];return true;
    }
    if(opcode==614){
        int slot=0,identifier=0;if(!fixed(2)||!call.readInteger(0,slot)||!call.readInteger(1,identifier)||slot<0||slot>=int(entity.emitters.size()))return false;
        if(!laserHost_||!laserHost_->healthy())return fail("ECL614 requires actual selector3 manager");
        const auto& emitter=entity.emitters[std::size_t(slot)];laser::selector3::Descriptor descriptor;
        const auto origin=bullet::emissionOrigin(emitter,{entity.state.x,entity.state.y,entity.state.z});
        descriptor.source={origin.x,origin.y,origin.z};descriptor.angle=emitter.angle;descriptor.length=emitter.positionZ;
        descriptor.width=emitter.laserWidth;descriptor.offset=emitter.spawnRadius;descriptor.lifetime=emitter.laserTimers[0];
        descriptor.identifier=identifier;descriptor.color=std::uint32_t(emitter.color);
        std::string reason;if(!laserHost_->birthSelector3(entity.state.id,descriptor,reason))return fail(reason);return true;
    }
    if(opcode==604){
        int identifier=0;if(!fixed(3)||!call.readInteger(0,identifier)||!laserHost_||!laserHost_->healthy())return false;
        std::string reason;
        if(!laserHost_->setSourceFromIdentifier(identifier,[&call](float& horizontal,float& vertical){return call.readFloat(1,horizontal)&&call.readFloat(2,vertical);},reason))
            return fail(reason.empty()?"original604 actual source owner rejected":reason);
        return true;
    }
    if(opcode==608){
        int identifier=0;if(!fixed(2)||!call.readInteger(0,identifier)||!laserHost_||!laserHost_->healthy())return false;
        std::string reason;
        if(!laserHost_->setAngleFromIdentifier(identifier,[&call](float& angle){return call.readFloat(1,angle);},reason))
            return fail(reason.empty()?"original608 actual angle owner rejected":reason);
        return true;
    }
    if(opcode==609||opcode==615){
        int identifier=0;if(!fixed(2)||!call.readInteger(0,identifier)||!laserHost_||!laserHost_->healthy())return false;
        std::string reason;
        if(opcode==609){float value=0;if(!call.readFloat(1,value)||!laserHost_->setSelector3AngularVelocity(identifier,value,reason))return fail(reason.empty()?"ECL609 actual owner rejected":reason);}
        else{int value=0;if(!call.readInteger(1,value)||!laserHost_->setSelector3FollowBoss(identifier,value,reason))return fail(reason.empty()?"ECL615 actual owner rejected":reason);}
        return true;
    }
    if(opcode==602){
        std::int32_t slot=0;
        if(!fixed(1)||!call.readInteger(0,slot)||slot<0||slot>=int(entity.emitters.size()))return false;
        if(!laserHost_||!laserHost_->healthy())return fail("ECL602 requires actual type0 manager/ANM resource owner 0x42a400");
        const auto& emitter=entity.emitters[std::size_t(slot)];
        const auto profile=bulletProfiles_.find(emitter.type);
        if(profile==bulletProfiles_.end())return fail("ECL602 requires its actual decoded profile, not the508 sound slot");
        laser::LineDescriptor descriptor;
        descriptor.source=bullet::emissionOrigin(emitter,{entity.state.x,entity.state.y,entity.state.z});
        descriptor.angle=emitter.angle;descriptor.speed=emitter.speed;descriptor.emitter=emitter;
        descriptor.configuration={emitter.positionX,emitter.positionY,emitter.positionZ,emitter.laserWidth};
        std::string reason;
        if(!laserHost_->birth(entity.state.id,descriptor,profile->second,reason))return fail(reason);
        return true;
    }
    if(opcode==508){
        std::int32_t slot=0,shot=0,transform=0;
        if(!fixed(3)||!call.readInteger(0,slot)||!call.readInteger(1,shot)||!call.readInteger(2,transform)
            ||slot<0||slot>=int(entity.emitters.size()))return false;
        entity.emitters[std::size_t(slot)].shotSound=shot;
        entity.emitters[std::size_t(slot)].transformSound=transform;
        entity.state.emitter0=entity.emitters[0];
        return true;
    }
    if(opcode==419){
        if(!fixed(0)||!messageGate_)return false;std::string reason;std::optional<bool> held;
        try{held=messageGate_(reason);}catch(...){return fail("original MSG gate owner threw");}
        if(!held||!reason.empty()||!error_.empty())return fail(reason.empty()?"original MSG gate is unavailable":reason);
        return !*held||call.holdCurrentCommand();
    }
    if(opcode==263){
        int bank=0,script=0;if(!fixed(2)||!call.readInteger(0,bank)||!call.readInteger(1,script)||!eclEffectHost_)return false;
        if(entity.state.flags&0x8000000u)return fail("mirrored263 effect constructor is not registered");
        std::string reason;
        try{if(!eclEffectHost_(bank,script,{entity.state.x,entity.state.y,entity.state.z},9,true,reason)||!reason.empty()||!error_.empty())return fail(reason.empty()?"ECL263 owned effect rejected":reason);}
        catch(...){return fail("ECL263 owned effect threw");}
        return true;
    }
    if(opcode==424){
        int value=0;if(!fixed(1)||!call.readInteger(0,value))return false;
        if(sceneControl_[0]!=value)sceneControl_[1]=0;sceneControl_[0]=value;return true;
    }
    if(opcode==405){
        if(!fixed(0))return false;
        entity.state.flags&=~0x40000u;return true;
    }
    if(opcode==417){
        int duration=0,initial=0,final=0;
        if(!fixed(3)||!call.readInteger(2,final)||!call.readInteger(1,initial)||!call.readInteger(0,duration)
            ||duration<1||duration>65535||std::abs(double(initial))>4096||std::abs(double(final))>4096)return false;
        if(shakeStates_.size()>=4096)return fail("screen shake owner budget");
        shakeStates_.insert(shakeStates_.begin(),{duration,initial,final,0});return true;
    }
    if(opcode==422||opcode==428||opcode==437||opcode==438||opcode==439){
        int identifier=0,limit=0,unused=0;
        if(!phaseSpell_||!spellHost_||instruction.parameterCount!=4||instruction.rawWords.size()<5||!call.readInteger(0,identifier)||!call.readInteger(2,unused)||!call.readInteger(1,limit))return false;
        const auto length=std::size_t(instruction.rawWords[3]);if(length<4||length>64||length%4||instruction.rawWords.size()!=4+length/4)return false;
        std::vector<std::uint8_t> title;std::uint8_t key=0x77,step=7;bool ended=false;
        for(std::size_t index=0;index<length;++index){const auto value=std::uint8_t(instruction.rawWords[4+index/4]>>((index%4)*8))^key;key=std::uint8_t(key+step);step=std::uint8_t(step+16);if(value==0)ended=true;else if(!ended)title.push_back(std::uint8_t(value));}
        if(!ended||title.empty())return false;
        if(opcode==437||opcode==438||opcode==439)identifier+=difficulty_-(opcode==438?1:opcode==439?2:0);
        auto next=*phaseSpell_;
        if(extraStage16Rank4SpellDomain_){
            if((opcode!=422&&opcode!=428)||!spell::beginExtraStage16Rank4(next,identifier,limit))return false;
        }else if(!spell::begin(next,identifier,limit,difficulty_,spellStageDepth_))return false;
        std::string reason;SpellEvent event;event.starting=true;event.state=next;event.title=title;event.position={entity.state.x,entity.state.y,entity.state.z};
        try{if(!spellHost_(event,reason)||!reason.empty())return fail(reason.empty()?"required spell constructor rejected":reason);}catch(...){return fail("spell constructor threw");}
        *phaseSpell_=next;spellTitle_=std::move(title);entity.state.lifeWords[5]|=1u;entity.state.lifeWords[3]=entity.state.lifeWords[0]*7u;entity.state.phaseTimer={-1,0,0,0x4b6f58,1};return true;
    }
    if(opcode==442||opcode==443){
        if(!fixed(0)||!phaseSpell_)return false;
        phaseSpell_->flags|=opcode==442?0x10u:0x20u;
        if(opcode==442)return true;
        if(!spellHost_)return fail("original443 requires its actual spell follow-effect owner");
        SpellEvent event;event.retireFollowEffect=true;event.state=*phaseSpell_;
        std::string reason;
        try{if(!spellHost_(event,reason)||!reason.empty())return fail(reason.empty()?"original443 follow-effect retirement rejected":reason);}
        catch(...){return fail("original443 follow-effect retirement threw");}
        return true;
    }
    if(opcode==423){
        if(!fixed(0)||!phaseSpell_)return false;
        if(phaseSpell_->flags&1u){
            if(!spellHost_)return fail("active spell requires its owning end host");
            auto next=*phaseSpell_;SpellEvent event;event.state=next;event.title=spellTitle_;event.result=spell::end(next);std::string reason;
            try{if(!spellHost_(event,reason)||!reason.empty())return fail(reason.empty()?"required spell end rejected":reason);}catch(...){return fail("spell end threw");}
            *phaseSpell_=next;
        }
        entity.state.lifeWords[5]&=~1u;return true;
    }
    if(opcode==445){
        if(!fixed(0))return false;
        const auto liveLasers=laserObjectCount();
        if(!liveLasers)return fail("ECL445 requires the original owned laser manager");
        if(*liveLasers!=0){
            if(!laserHost_)return fail("ECL445 live count has no current typed laser owner");
            std::string reason;
            if(!laserHost_->cancelLive(reason,false,true))return fail(reason.empty()?"original42a740 laser cancellation rejected":reason);
        }
        return true;
    }
    if(opcode==510){
        if(!fixed(0)||!eclRadiusCancellation_)return false;
        std::string reason;
        if(!bullets_.cancelAll(eclRadiusCancellation_,reason)||!reason.empty()||!error_.empty())
            return fail(reason.empty()?"original510 bullet cancellation rejected":reason);
        const auto liveLasers=laserObjectCount();
        if(!liveLasers)return fail("original510 requires the actual current laser manager");
        if(*liveLasers!=0){
            if(!laserHost_||!laserHost_->healthy())return fail("original510 requires actual42a820laserowner");
            if(!laserHost_->cancelLive(reason,false,false,true)||!reason.empty()||!error_.empty())
                return fail(reason.empty()?"original51042a820 laser cancellation rejected":reason);
        }
        return true;
    }
    if(opcode==283){
        std::int32_t identifier=0;
        if(!fixed(3)||(instruction.parameterMask&3u)!=3u||(instruction.parameterMask&~7u)
            ||!call.readInteger(2,identifier))return false;
        std::array<std::int32_t,2> destinations{};
        for(std::size_t index=0;index<destinations.size();++index){
            const float address=std::bit_cast<float>(instruction.rawWords[index]);
            if(!std::isfinite(address)||std::trunc(address)!=address
                ||double(address)<-2147483648.0||double(address)>=2147483648.0)return false;
            destinations[index]=std::int32_t(address);
        }
        const Entity* selected=nullptr;
        if(identifier!=0)for(const auto* owner=localHead_;owner;owner=owner->next)
            if(owner->state.originalIdentifier==std::uint32_t(identifier)){selected=owner;break;}
        const float horizontal=selected?selected->state.x:entity.state.x;
        const float vertical=selected?selected->state.y:entity.state.y;
        return call.storeFloat(destinations[0],horizontal)&&call.storeFloat(destinations[1],vertical);
    }
    if(opcode==463){
        std::string target;
        if(instruction.parameterCount!=1||instruction.rawWords.empty()||instruction.parameterMask
            ||instruction.rawWords[0]%4||instruction.rawWords.size()!=1+instruction.rawWords[0]/4
            ||!call.readString(0,target,0)||target.empty()||target.size()>200||!linked_.find(target))return false;
        entity.deathSubroutine=std::move(target);return true;
    }
    if(opcode==421){
        std::int32_t slot=0;std::string target;
        if(instruction.parameterCount!=2||instruction.rawWords.size()<2||!call.readInteger(0,slot)||slot<0||slot>=int(entity.state.phases.size())
            ||!call.readString(1,target,1)||target.empty()||target.size()>200||!linked_.find(target))return false;
        entity.state.phases[std::size_t(slot)].timeTarget=std::move(target);return true;
    }
    if(opcode==512||opcode==513){
        const auto liveLasers=laserObjectCount();
        float radius=0;if(!fixed(1)||!call.readFloat(0,radius)||!eclRadiusCancellation_||!liveLasers)return false;
        if(*liveLasers!=0&&(!laserHost_||!laserHost_->healthy()))return fail("ECL512/513 live laser radius cancellation requires healthy current 0x42a7c0/virtual+0x24 owner");
        const bullet::Position center{entity.state.x,entity.state.y,entity.state.z};
        std::string reason;
        if(!bullets_.cancelRadius(center,radius,eclRadiusCancellation_,reason))return fail(reason);
        if(*liveLasers!=0&&!laserHost_->cancelEntryRadius(center,radius,true,reason))return fail(reason);
        return true;
    }
    if(opcode==413){
        if(!fixed(0))return false;
        auto& timer=entity.state.phaseTimer;
        if(!(timer.flags&1u)){timer.flags|=1u;timer.clockSource=0x4b6f58;}
        timer.previous=-1;timer.current=0;timer.fraction=0;return true;
    }
    if(opcode==415){
        std::int32_t duration=0;if(!fixed(1)||!call.readInteger(0,duration))return false;
        entity.state.protection=duration;return true;
    }
    if(opcode==409){
        if(!fixed(0)||!eclDropHost_)return false;
        std::string reason;
        try{if(!eclDropHost_(entity.state,reason)||!reason.empty()||!error_.empty())return fail(reason.empty()?"required ECL409 item emission rejected":reason);}
        catch(...){return fail("ECL409 item host threw");}
        entity.state.drops.words[0]=0;entity.state.drop=0;enemy::resetAdditionalDrops(entity.state.drops);return true;
    }
    if(opcode==416){
        std::int32_t sound=0;if(!fixed(1)||!call.readInteger(0,sound)||!eclSoundSink_)return false;
        std::string reason;if(!eclSoundSink_(sound,entity.state.x,reason))return fail(reason.empty()?"ECL sound sink rejected":reason);return true;
    }
    if(opcode==448){
        std::array<std::int32_t,4> delays{};if(!fixed(4))return false;
        for(std::size_t index=0;index<delays.size();++index)if(!call.readInteger(index,delays[index]))return false;
        const auto selected=bullet::chooseDifficulty(difficulty_,delays);return selected&&call.subtractTime(*selected);
    }
    if(opcode==420)return fixed(0)&&(!enemy::bossWaitPending(bosses_)||call.holdCurrentCommand());
    if(opcode==312){
        if(!fixed(3))return false;
        if(!entity.movementArea)return fail("enemy312 requires explicit original404 movement area");
        if(!playerAimConfigured_)return fail("enemy312 requires explicit actual player context");
        const auto direction=enemy::randomMovementDirection({entity.state.x,entity.state.y,entity.state.z},*entity.movementArea,playerPosition_.x,random_);
        if(!direction)return fail("unsupported enemy312 original direction domain");
        std::int32_t mode=0,duration=0;float speed=0;
        if(!call.readInteger(1,mode)||!call.readFloat(2,speed)||!call.readInteger(0,duration))return false;
        return enemy::beginRandomMovement(entity.polar,entity.polarInterpolation,*direction,duration,mode,speed);
    }
    if(opcode==404){
        std::array<float,4> values{};
        if(!fixed(4))return false;
        entity.state.flags|=0x40000u;
        for(std::size_t index=0;index<values.size();++index)if(!call.readFloat(index,values[index]))return false;
        const auto area=enemy::prepareMovementArea(values[0],values[1],values[2],values[3]);
        if(!area)return false;
        entity.movementArea=*area;return true;
    }
    if(opcode==526){
        if(!fixed(2)||!geometryHost_||!geometryRemover_)return false;
        std::string reason;
        if(entity.geometryActive){
            try{if(!geometryRemover_(entity.state.id,reason))return fail(reason.empty()?"geometry526 previous-owner retirement rejected":reason);}
            catch(...){return fail("geometry526 previous-owner retirement host threw");}
            entity.geometryActive=false;
        }
        float radius=0;std::int32_t color=0;
        if(!call.readFloat(0,radius)||!call.readInteger(1,color))return false;
        try{if(!geometryHost_(entity.state.id,radius,std::uint32_t(color),reason))return fail(reason.empty()?"geometry526 owning creation rejected":reason);}
        catch(...){return fail("geometry526 creation host threw");}
        entity.geometryActive=radius>0;return true;
    }
    if(opcode==308||opcode==309||opcode==310||opcode==311){
        if(!fixed((opcode==309||opcode==311)?5:4))return false;
        const bool relative=opcode==310||opcode==311;
        auto& position=relative?entity.relative:entity.absolute;
        auto& polar=relative?entity.relativePolar:entity.polar;
        auto& interpolation=relative?entity.relativePolarInterpolation:entity.polarInterpolation;
        bool accepted=false;
        if(opcode==308||opcode==310){
            std::array<float,4> values{};for(std::size_t index=0;index<values.size();++index)if(!call.readFloat(index,values[index]))return false;
            accepted=enemy::beginCircularMotion(position,polar,values[0],values[1],values[2],values[3]);
        }else{
            std::array<float,3> values{};for(std::size_t index=0;index<values.size();++index)if(!call.readFloat(index+2,values[index]))return false;
            std::int32_t duration=0,mode=0;if(!call.readInteger(0,duration)||!call.readInteger(1,mode))return false;
            accepted=enemy::beginCircularInterpolation(polar,interpolation,duration,mode,values[0],values[1],values[2]);
        }
        if(!accepted)return fail("unsupported original circular movement domain");
        if(opcode==311){
            entity.remainingMotionDurations[1]=entity.relativePolarInterpolation.duration;
            entity.remainingMotionDurations[3]=entity.relativePolarInterpolation.duration;
        }
        enemy::Position combined{entity.state.x,entity.state.y,entity.state.z},velocity;
        if(entity.state.flags&0x40000u){
            if(!entity.movementArea||!enemy::combinePositionWithArea(combined,velocity,entity.absolute,entity.relative,*entity.movementArea))return fail("circular movement requires the original404 area owner");
        }else if(!enemy::combinePosition(combined,velocity,entity.absolute,entity.relative,entity.state.flags))return fail("circular movement position combine rejected");
        entity.state.x=combined.x;entity.state.y=combined.y;entity.state.z=combined.z;
        return true;
    }
    if(opcode==327){
        if(!fixed(0)||!entity.foldRelativeAndStop())return false;
        entity.state.speed=entity.polar.speed;return true;
    }
    if(opcode==300){
        float horizontal=0,vertical=0;
        if(!fixed(2)||!call.readFloat(0,horizontal)||!call.readFloat(1,vertical))return false;
        if(horizontal>enemy::UnchangedPolar)entity.absolute.x=horizontal;
        if(vertical>enemy::UnchangedPolar)entity.absolute.y=vertical;
        entity.polar.modeFlags&=~3u;
        enemy::Position combined{float(double(entity.absolute.x)+entity.relative.x),
            float(double(entity.absolute.y)+entity.relative.y),float(double(entity.absolute.z)+entity.relative.z)},velocity;
        if(entity.state.flags&0x40000u){
            if(!entity.movementArea||!enemy::combinePositionWithArea(combined,velocity,entity.absolute,entity.relative,*entity.movementArea))return fail("original300 requires actual404 area owner");
        }else if(!enemy::combinePosition(combined,velocity,entity.absolute,entity.relative,entity.state.flags))return fail("original300 combined position rejected");
        entity.state.x=combined.x;entity.state.y=combined.y;entity.state.z=combined.z;return true;
    }
    if(opcode==301){
        std::int32_t duration=0,mode=0;float x=0,y=0;
        if(!fixed(4)||!call.readFloat(2,x)||!call.readFloat(3,y)||!call.readInteger(0,duration))return false;
        if(duration<=0)return enemy::beginPositionInterpolation(entity.positionInterpolation,entity.absolute,duration,0,x,y);
        if(!call.readInteger(0,duration)||duration<=0||!call.readInteger(1,mode)
            ||!enemy::beginPositionInterpolation(entity.positionInterpolation,entity.absolute,duration,mode,x,y))
            return fail("unsupported enemy301 position interpolation mode="+std::to_string(mode)+" duration="+std::to_string(duration));
        entity.polar.modeFlags&=~3u;return true;
    }
    if(opcode==414){
        std::int32_t slot=0,threshold=0,limit=0;std::string target;
        if(instruction.parameterCount!=4||instruction.rawWords.size()<4||!call.readInteger(2,limit)||!call.readInteger(1,threshold)||!call.readInteger(0,slot))return false;
        if(threshold>=0){
            const auto length=std::size_t(instruction.rawWords[3]);
            if(length%4||length/4+4!=instruction.rawWords.size()||!call.readString(3,target,3)||!linked_.find(target))return false;
        }
        return enemy::registerPhase(entity.state.phases,slot,threshold,limit,std::move(target));
    }
    if(opcode==427){
        std::int32_t slot=0,color=0;float life=0;
        return fixed(3)&&call.readInteger(2,color)&&call.readFloat(1,life)&&call.readInteger(0,slot)
            &&enemy::setBossGauge(bossGauges_,entity.bossSlot,slot,life,std::bit_cast<std::int32_t>(entity.state.lifeWords[1]),std::uint32_t(color));
    }
    if(opcode==440)return fixed(1);
    if(opcode==412){
        std::int32_t slot=0;
        return fixed(1)&&call.readInteger(0,slot)&&enemy::setBossSlot(bosses_,entity.state.flags,entity.bossSlot,entity.state.id,slot);
    }
    if(opcode==613){
        std::int32_t slot=0;
        if(instruction.parameterCount!=1||instruction.rawWords.empty()||!call.readInteger(0,slot)
            ||slot<0||slot>=int(entity.state.phases.size()))return false;
        for(std::size_t index=0;index<3;++index){
            if(int(index)==entity.bossSlot)continue;
            const auto owner=bosses_.owners[index];
            if(!owner)continue;
            Entity* other=nullptr;
            for(auto& candidate:entities_)
                if(candidate->state.id==owner&&candidate->linked){other=candidate.get();break;}
            if(!other||other==&entity)continue;
            const auto& record=other->state.phases[std::size_t(slot)];
            if(record.threshold<0||record.healthTarget.empty())continue;
            if(!synchronizeBoss(*other,record.healthTarget))return false;
        }
        return true;
    }
    if(opcode==453){
        std::int32_t identifier=0;if(!fixed(1)||!call.readInteger(0,identifier))return false;
        entity.state.hitSoundOverride=identifier;return true;
    }
    if(opcode==452){
        std::int32_t offset=0;
        if(instruction.parameterCount!=1||instruction.rawWords.empty()||!call.readInteger(0,offset))return false;
        entity.animation.layerOffset=offset;entity.state.animationLayerOffset=offset;return true;
    }
    if(opcode==529){
        std::int32_t selector=0;
        if(instruction.parameterCount!=1||instruction.rawWords.empty()||!call.readInteger(0,selector))return false;
        if(selector==0){entity.callbackSelector=0;return true;}
        if(selector==2){entity.callbackSelector=2;return true;}
        if(selector==4){entity.callbackSelector=4;return true;}
        if(selector==3&&bulletType25Conversion_&&bulletProfiles_.contains(25)){entity.callbackSelector=3;return true;}
        return fail("ECL529 unsupported native enemy-update callback selector: "+std::to_string(selector));
    }
    if(opcode==411){
        std::int32_t life=0;
        if(!fixed(1)||!call.readInteger(0,life))return false;
        enemy::setLife(entity.state.lifeWords,entity.state.flags,life);entity.state.life=life;return true;
    }
    if(opcode==22) {
        const auto* registration=call.callbackRegistration();
        if(!registration)return false;
        if(entity.registrations.size()>=32&&!entity.registrations.contains(registration->slot))return false;
        entity.registrations[registration->slot]=registration->subroutine;
        return true;
    }
    if(opcode==435) {
        if(!fixed(5)||!(instruction.parameterMask&1))return false;
        std::array<std::int32_t,4> choices{};
        for(std::size_t index=0;index<choices.size();++index)
            if(!call.readInteger(index+1,choices[index]))return false;
        const auto selected=bullet::chooseDifficulty(difficulty_,choices);
        return selected&&call.storeInteger(std::int32_t(instruction.rawWords[0]),*selected);
    }
    if(opcode==436){
        if(!fixed(5)||!(instruction.parameterMask&1)||difficulty_<0||difficulty_>4)return false;
        std::array<float,4> choices{};
        for(std::size_t index=0;index<choices.size();++index)if(!call.readFloat(index+1,choices[index]))return false;
        const float address=std::bit_cast<float>(instruction.rawWords[0]);
        if(!std::isfinite(address)||double(address)<-2147483648.0||double(address)>=2147483648.0||std::trunc(address)!=address)return false;
        return call.storeFloat(std::int32_t(address),choices[std::size_t(std::min(difficulty_,3))]);
    }
    if(opcode==509) {
        std::int32_t slot=0,index=0;
        bullet::ExtensionRaw extension;
        if(!fixed(8)||!call.readInteger(0,slot)||slot<0||slot>=int(entity.emitters.size())||!call.readInteger(1,index)
            ||!call.readInteger(2,extension.control)||!call.readInteger(3,extension.type)
            ||!call.readInteger(4,extension.parameterA)||!call.readInteger(5,extension.parameterB)
            ||!call.readFloat(6,extension.parameterR)||!call.readFloat(7,extension.parameterS))return false;
        if(!bullet::setLegacyExtension(entity.emitters[slot],index,extension))return false;
        entity.state.emitter0=entity.emitters[0];return true;
    }
    if(opcode==511) {
        std::int32_t destination=0,source=0;
        if(!fixed(2)||!call.readInteger(0,destination)||!call.readInteger(1,source)
            ||destination<0||destination>=int(entity.emitters.size())
            ||source<0||source>=int(entity.emitters.size()))return false;
        entity.emitters[std::size_t(destination)]=entity.emitters[std::size_t(source)];
        entity.state.emitter0=entity.emitters[0];return true;
    }
    if(opcode==538){
        std::int32_t slot=0,index=0;std::string name;
        if(instruction.parameterCount!=3||!call.readInteger(0,slot)||slot<0||slot>=int(entity.emitters.size())
            ||!call.readInteger(1,index)||!call.readString(2,name,2)||instruction.rawWords.size()<3
            ||instruction.rawWords[2]/4+3!=instruction.rawWords.size())return false;
        auto record=bullet::extensionRecord(entity.emitters[std::size_t(slot)],index);if(!record)return false;
        record->callbackName=std::make_shared<const std::string>(std::move(name));
        if(!bullet::setExtensionRecord(entity.emitters[std::size_t(slot)],index,*record))return false;
        entity.state.emitter0=entity.emitters[0];return true;
    }
    if(opcode==537){
        std::int32_t slot=0,index=0;bullet::ExtensionRaw extension;
        if(!fixed(12)||!call.readInteger(0,slot)||slot<0||slot>=int(entity.emitters.size())||!call.readInteger(1,index)
            ||!call.readInteger(2,extension.control)||!call.readInteger(3,extension.type)||!call.readInteger(4,extension.parameterA)
            ||!call.readInteger(5,extension.parameterB)||!call.readInteger(6,extension.parameterC)||!call.readInteger(7,extension.parameterD)
            ||!call.readFloat(8,extension.parameterR)||!call.readFloat(9,extension.parameterS)||!call.readFloat(10,extension.parameterT)
            ||!call.readFloat(11,extension.parameterU)||!bullet::setExtendedExtension(entity.emitters[slot],index,extension))return false;
        entity.state.emitter0=entity.emitters[0];return true;
    }
    if(opcode==524){
        int slot=0;float radius=0;
        if(!fixed(2)||!call.readFloat(1,radius)||!call.readInteger(0,slot)||slot<0||slot>=int(entity.emitters.size()))return false;
        if(!bullet::setSpawnRadius(entity.emitters[std::size_t(slot)],radius))return false;
        entity.state.emitter0=entity.emitters[0];return true;
    }
    if(opcode==503||opcode==523||opcode==525){
        int slot=0;float first=0,second=0;
        if(!fixed(3)||!call.readInteger(0,slot)||slot<0||slot>=int(entity.emitters.size())||!call.readFloat(1,first)||!call.readFloat(2,second))return false;
        auto& emitter=entity.emitters[std::size_t(slot)];
        const bool accepted=opcode==503?bullet::setEmissionOffset(emitter,first,second):opcode==523?bullet::setPolarEmissionOffset(emitter,first,second):bullet::setAbsoluteEmissionOrigin(emitter,first,second);
        entity.state.emitter0=entity.emitters[0];return accepted;
    }
    if(opcode==501) {
        if(!bulletsConfigured_)return false;
        std::int32_t slot=0;
        if(!fixed(1)||!call.readInteger(0,slot)||slot<0||slot>=int(entity.emitters.size()))return false;
        auto emitter=entity.emitters[std::size_t(slot)];
        const auto origin=bullet::emissionOrigin(emitter,{entity.state.x,entity.state.y,entity.state.z});
        emitter.positionX=origin.x;emitter.positionY=origin.y;emitter.positionZ=origin.z;
        std::string error;
        const auto profile=bulletProfiles_.find(emitter.type);
        if(profile==bulletProfiles_.end())return fail("unimplemented bullet type profile: "+std::to_string(emitter.type));
        if(!bullets_.emit(emitter,profile->second,playerPosition_,entity.exclusionRadiusSquared,error))return fail(error);
        entity.emitters[std::size_t(slot)]=emitter;
        entity.state.emitter0=entity.emitters[0];
        return true;
    }
    if(opcode==534){
        std::int32_t selector=0;
        if(!fixed(1)||!call.readInteger(0,selector))return false;
        if(selector<1||selector>4)return fail("ECL534 selector "+std::to_string(selector)+" is outside the native callback table domain 1..4");
        if(selector!=1)return fail("ECL534 native callback "+std::to_string(selector)+" is not ported");
        const auto sources=bullets_.snapshots();
        std::vector<const bullet::Birth*> eligible;
        for(const auto& source:sources){
            if(source.state.motionState!=1||(source.type!=23&&source.type!=25)||source.age<30)continue;
            if(!std::isfinite(source.state.position.x)||!std::isfinite(source.state.position.y)||!std::isfinite(source.state.position.z))
                return fail("ECL534 eligible source position is not finite");
            eligible.push_back(&source);
        }
        if(eligible.empty())return true;
        if(!bulletsConfigured_||!playerAimConfigured_)return fail("ECL534 selector1 emission requires the original bullet manager and player owners");
        auto& emitter=entity.emitters[0];
        const auto profile=bulletProfiles_.find(emitter.type);
        if(profile==bulletProfiles_.end())return fail("ECL534 requires the original typed profile for emitter type "+std::to_string(emitter.type));
        const auto volley=bullet::prefixCount(emitter);
        if(!volley)return fail("ECL534 emitter volley exceeds the original prefix budget");
        const std::size_t free=std::size_t(bullet::Manager::Capacity-bullets_.activeCount());
        if(*volley!=0&&eligible.size()>free/std::size_t(*volley))return fail("ECL534 owned emission exceeds the free bullet-slot budget");
        for(const auto* source:eligible){
            const auto origin=bullet::emissionOrigin(emitter,source->state.position);
            if(!std::isfinite(origin.x)||!std::isfinite(origin.y)||!std::isfinite(origin.z))return fail("ECL534 emission origin is not finite");
        }
        std::string reason;
        for(const auto* source:eligible){
            const auto origin=bullet::emissionOrigin(emitter,source->state.position);
            emitter.positionX=origin.x;emitter.positionY=origin.y;emitter.positionZ=origin.z;
            if(!bullets_.emit(emitter,profile->second,playerPosition_,0,reason))return fail(reason.empty()?"ECL534 owned bullet emission rejected":reason);
        }
        entity.state.emitter0=emitter;
        return true;
    }
    if(opcode==500||opcode==502||opcode==504||opcode==505||opcode==506||opcode==507||opcode==521||opcode==522) {
        const std::size_t parameters=opcode==500?1:opcode==507?2:opcode==502||opcode==504||opcode==505||opcode==506?3:9;
        std::int32_t slot=0;
        if(!fixed(parameters)||!call.readInteger(0,slot)||slot<0||slot>=int(entity.emitters.size()))return false;
        auto& emitter=entity.emitters[std::size_t(slot)];
        if(opcode==500)emitter=bullet::resetEmitter();
        else if(opcode==502) {
            std::int32_t type=0,color=0;if(!call.readInteger(1,type)||!call.readInteger(2,color))return false;
            bullet::setTypeColor(emitter,type,color);
        } else if(opcode==507) {
            std::int32_t mode=0;if(!call.readInteger(1,mode))return false;bullet::setAimMode(emitter,mode);
        } else if(opcode==504) {
            float angle=0,spacing=0;if(!call.readFloat(1,angle)||!call.readFloat(2,spacing))return false;
            bullet::setAngles(emitter,angle,spacing);
        }else if(opcode==505){
            float speed=0,endSpeed=0;if(!call.readFloat(1,speed)||!call.readFloat(2,endSpeed))return false;
            emitter.speed=speed;emitter.endSpeed=endSpeed;
        }else if(opcode==506){
            std::int32_t count=0,layers=0;if(!call.readInteger(1,count)||!call.readInteger(2,layers))return false;
            bullet::setDifficultyCounts(emitter,0,{count,0,0,0},{layers,0,0,0});
        } else {
            const auto index=std::size_t(std::min(difficulty_,3));
            if(opcode==521) {
                std::array<float,4> speeds{},endSpeeds{};
                if(!call.readFloat(index+1,speeds[index])||!call.readFloat(index+5,endSpeeds[index]))return false;
                bullet::setDifficultySpeeds(emitter,difficulty_,speeds,endSpeeds);
            } else {
                std::array<std::int32_t,4> counts{},layers{};
                if(!call.readInteger(index+1,counts[index])||!call.readInteger(index+5,layers[index]))return false;
                bullet::setDifficultyCounts(emitter,difficulty_,counts,layers);
            }
        }
        entity.state.emitter0=entity.emitters[0];
        return true;
    }
    if(opcode==402||opcode==403) {
        std::int32_t mask=0;
        if(!fixed(1)||!call.readInteger(0,mask))return false;
        const auto result=opcode==402?enemy::setFlags(entity.state.flags,std::uint32_t(mask)):enemy::clearFlags(entity.state.flags,std::uint32_t(mask));
        entity.state.flags=result.flags;
        if(result.visibility!=enemy::VisibilityEffect::None&&animationVisibilityHost_){
            std::string error;
            try{if(!animationVisibilityHost_(entity.state.id,result.visibility,error))return fail(error.empty()?"enemy visibility host rejected":error);}
            catch(...){return fail("enemy visibility host threw");}
        }
        return true;
    }
    if(opcode==400||opcode==401) {
        float width=0,height=0;
        if(!fixed(2)||!call.readFloat(0,width)||!call.readFloat(1,height))return false;
        return enemy::setBounds(opcode==400?entity.hitbox:entity.hurtbox,width,height);
    }
    if(opcode==406){
        if(!fixed(0))return false;
        enemy::resetAdditionalDrops(entity.state.drops);return true;
    }
    if(opcode==410){
        std::int32_t kind=0;
        if(!fixed(1)||!call.readInteger(0,kind)||!enemy::setDropWord(entity.state.drops,0,kind))return false;
        entity.state.drop=kind;return true;
    }
    if(opcode==407){
        std::int32_t index=0,value=0;
        if(!fixed(2)||!call.readInteger(0,index)||!call.readInteger(1,value)||!enemy::setDropWord(entity.state.drops,index,value))return false;
        entity.state.drop=entity.state.drops.words[0];return true;
    }
    if(opcode==408){
        float width=0,height=0;
        if(!fixed(2)||!call.readFloat(0,width)||!call.readFloat(1,height))return false;
        return enemy::setDropSpread(entity.state.drops,width,height);
    }
    if(opcode==258) {
        std::int32_t bank=0;if(!fixed(1)||!call.readInteger(0,bank)||bank<0||bank>255)return false;
        enemy::setAnimationBank(entity.animation,bank);return true;
    }
    if(opcode==269||opcode==274){
        int slot=0,relativeScript=0;
        if(!fixed(opcode==274?2:1)||!call.readInteger(0,slot)||slot<0||slot>=int(enemy::AnimationSlotCount)||!animationHost_)return false;
        if(opcode==274&&!call.readInteger(1,relativeScript))return false;
        const auto script=std::int64_t(entity.animation.baseScript)+5+relativeScript;
        if(script<0||script>std::numeric_limits<int>::max())return false;
        auto request=enemy::animationSlot(entity.animation,slot,int(script),entity.state.flags);if(!request)return false;request->layer=8;
        std::string reason;const AnimationBirth birth{entity.state.id,animationSerial_+1,*request};
        if(!animationHost_(birth,reason))return fail(reason.empty()?"native269/274 animation constructor rejected":reason);
        entity.state.animation[std::size_t(slot)]=request->script;entity.state.animationBanks[std::size_t(slot)]=request->bank;
        animations_.push_back({entity.state.id,++animationSerial_,*request});return true;
    }
    if(opcode==259||opcode==262) {
        std::int32_t slot=0,script=0;
        if(!fixed(2)||!call.readInteger(0,slot)||!call.readInteger(1,script))return false;
        if(slot<0||slot>=int(enemy::AnimationSlotCount))return false;
        if(opcode==259&&script<0){
            if(!animationHost_)return fail("native259 slot retirement requires the actual animation owner");
            const enemy::AnimationRequest retirement{entity.animation.bank,slot,-1,0,enemy::VisibilityEffect::None,slot==0};
            const AnimationBirth request{entity.state.id,0,retirement};std::string reason;
            try{if(!animationHost_(request,reason))return fail(reason.empty()?"native259 animation slot retirement rejected":reason);}
            catch(...){return fail("native259 animation slot retirement host threw");}
            entity.state.animation[slot]=-1;entity.state.animationBanks[slot]=-1;return true;
        }
        const auto request=enemy::animationSlot(entity.animation,slot,script,entity.state.flags);
        if(!request||animations_.size()>=65536)return false;
        const AnimationBirth birth{entity.state.id,animationSerial_+1,*request};
        if(animationHost_) {
            std::string error;
            try{if(!animationHost_(birth,error))return fail(error.empty()?"enemy animation construction rejected":error);}
            catch(...){return fail("enemy animation construction host threw");}
        }
        entity.state.animation[slot]=script;entity.state.animationBanks[slot]=request->bank;
        if(opcode==262&&slot==0){
            entity.animation.baseScript=script;entity.animation.motionSelector=0;
            entity.state.flags|=0x200000u;
        }
        animations_.push_back({entity.state.id,++animationSerial_,*request});return true;
    }
    if(opcode==304) {
        float angle=0,speed=0;
        if(!fixed(2)||!call.readFloat(0,angle)||!call.readFloat(1,speed))return false;
        if(!enemy::setPolar(entity.polar,angle,speed,entity.state.flags))return false;
        entity.state.angle=entity.polar.angle;entity.state.speed=entity.polar.speed;return true;
    }
    if(opcode==305) {
        std::int32_t duration=0,mode=0;float angle=0,speed=0;
        if(!fixed(4)||!call.readFloat(2,angle)||!call.readFloat(3,speed)||!call.readInteger(0,duration))return false;
        if(duration<=0)return enemy::beginPolarInterpolation(entity.polar,entity.polarInterpolation,duration,0,angle,speed,entity.state.flags);
        if(!call.readInteger(1,mode)||!call.readInteger(0,duration))return false;
        if(duration<=0)return false;
        return enemy::beginPolarInterpolation(entity.polar,entity.polarInterpolation,duration,mode,angle,speed,entity.state.flags);
    }
    if(opcode==256||opcode==257||opcode==260||opcode==261) {
        if(instruction.parameterCount!=6||instruction.rawWords.empty())return false;
        std::string sub;float positionX=0,positionY=0;std::int32_t life=0,score=0,drop=0;
        const std::size_t numeric=std::size_t(instruction.rawWords[0])/4+1;
        if(numeric>instruction.rawWords.size()||instruction.rawWords.size()-numeric!=5||!call.readString(0,sub,0)
            ||!call.readFloat(numeric,positionX,1)||!call.readFloat(numeric+1,positionY,2)||!call.readInteger(numeric+2,life,3)
            ||!call.readInteger(numeric+3,score,4)||!call.readInteger(numeric+4,drop,5))return false;
        enemy::SpawnParent parent;
        parent.combinedPosition={entity.state.x,entity.state.y,entity.state.z};parent.flags=entity.state.flags;
        parent.inheritedWords=entity.variables;parent.contextTag=entity.state.originalIdentifier;
        const auto request=enemy::prepareSpawn(opcode,sub,parent,positionX,positionY,life,score,drop);
        return request&&create(*request,false)!=nullptr;
    }
    return false;
}

bool StageWorld::runEnemyCallback(Entity& entity) {
    if(entity.callbackSelector!=2&&entity.callbackSelector!=3&&entity.callbackSelector!=4)return true;
    std::vector<std::optional<bullet::CallbackSnapshot>> snapshots(bullet::Manager::Capacity);
    std::vector<enemybullet::BulletState> states(bullet::Manager::Capacity);
    for(std::size_t slot=0;slot<bullet::Manager::Capacity;++slot){
        auto snapshot=bullets_.callbackSnapshot(slot);
        if(!snapshot)continue;
        snapshots[slot]=snapshot;
        auto& state=states[slot];
        state.active=true;state.type=snapshot->type;state.motionState=snapshot->motionState;
        state.position={snapshot->position.x,snapshot->position.y};
        state.speed=snapshot->speed;state.angle=snapshot->angle;
        state.velocity={snapshot->velocity.x,snapshot->velocity.y};
    }
    const enemybullet::EntityContext context{
        entity.state.x,entity.state.y,
        std::bit_cast<float>(entity.variables[4]),std::bit_cast<float>(entity.variables[5])};
    const auto selector=entity.callbackSelector==3?enemybullet::Selector::Type25Conversion:entity.callbackSelector==4?enemybullet::Selector::Accelerate:enemybullet::Selector::Curve;
    const auto plan=enemybullet::evaluate(selector,context,states);
    if(!plan.supported)return fail("ECL529 selector2 callback rejected: "+plan.reason);
    if(!std::isfinite(plan.nextVariable4))return fail("ECL529 selector2 variable4 update is not finite");
    if(entity.callbackSelector==3){
        const auto profile=bulletProfiles_.find(25);
        if(profile==bulletProfiles_.end()||!bulletType25Conversion_)return fail("type25 callback lost its real profile/visual owner");
        for(const auto& effect:plan.effects){
            if(!effect.matched)continue;
            if(effect.index>=snapshots.size()||!snapshots[effect.index])return fail("type25 callback referenced a vanished slot");
            const auto& snapshot=*snapshots[effect.index];
            std::string reason;
            if(!bullets_.convertCallbackType25(snapshot.slot,snapshot.id,profile->second,bulletType25Conversion_,reason))
                return fail(reason.empty()?"original type25 callback real owner failed":reason);
        }
        entity.variables[4]=std::bit_cast<std::uint32_t>(plan.nextVariable4);return true;
    }
    std::vector<bullet::CallbackMutation> mutations;
    for(const auto& effect:plan.effects){
        if(!effect.matched)continue;
        if(effect.index>=snapshots.size()||!snapshots[effect.index])
            return fail("ECL529 selector2 callback referenced a vanished bullet slot");
        const auto& snapshot=*snapshots[effect.index];
        mutations.push_back({snapshot.slot,snapshot.id,effect.angle,{effect.velocity.x,effect.velocity.y,snapshot.velocity.z},
            entity.callbackSelector==4?std::optional<float>(effect.speed):std::nullopt});
    }
    std::string reason;
    if(!bullets_.applyCallbackMutations(mutations,reason))
        return fail(reason.empty()?"ECL529 selector2 owner mutation rejected":reason);
    entity.variables[4]=std::bit_cast<std::uint32_t>(plan.nextVariable4);
    return true;
}
bool StageWorld::run(Entity& entity,bool allowEnemyCallback) {
    const auto result=entity.vm->runTick();
    instructionsThisTick_+=result.instructions;
    if(instructionsThisTick_>100000)return fail("world aggregate instruction budget exceeded");
    if(result.status==ecl::VmStatus::Faulted) {
        const auto& fault=entity.vm->error();
        std::ostringstream message;message<<"entity="<<entity.state.id<<" entry="<<entity.state.entry<<" sub="<<fault.subroutine
            <<" offset="<<fault.offset<<" opcode="<<fault.opcode<<": "<<fault.message;
        return fail(message.str());
    }
    if(result.status==ecl::VmStatus::Finished)entity.state.active=false;
    if(entity.state.active&&allowEnemyCallback&&!runEnemyCallback(entity))return false;
    return error_.empty();
}

bool StageWorld::tick() {
    if(entityRetirementAttempted_||bulletRetirementAttempted_){operationError_="retired gameplay managers cannot update";return false;}
    if(executing_)return fail("world tick is not reentrant");
    if(!started_)return fail("world has not started");
    if(!error_.empty())return false;
    if(bullets_.activeCount()!=0&&!bullets_.hasUpdater())return fail("bullet update collision/animation/ETEX scheduling is not implemented");
    operationError_.clear();
    instructionsThisTick_=0;
    screenShake_={};
    for(auto current=shakeStates_.begin();current!=shakeStates_.end();){
        const int time=++current->elapsed;
        if(time>=current->duration){current=shakeStates_.erase(current);continue;}
        const float delta=float(current->final-current->initial);
        const float scaled=float(double(delta)*time),ratio=float(double(scaled)/current->duration);
        const float amount=float(double(ratio)+current->initial);
        for(auto& coordinate:screenShake_){const auto selector=random_.next32()%3u;coordinate=selector==0?0:selector==1?amount:-amount;}
        ++current;
    }
    executing_=true;
    if(localEnemyPasses_) {
        auto* current=localHead_;
        std::size_t visits=0;
        while(current&&error_.empty()) {
            if(++visits>4096){fail("enemy manager visit budget");break;}
            auto* successor=current->next;
            if(current->state.flags&0x4000000U){retireLocal(*current);current=successor;continue;}
            if(!updateLocal(*current))break;
            current->state.flags&=~0x80000U;
            current=successor;
        }
        if(error_.empty())advanceLaserPhase();
        if(error_.empty())advanceBulletPhase();
        if(error_.empty()&&phaseSpell_&&!spell::tick(*phaseSpell_))fail("spell scalar update rejected");
        if(error_.empty()&&!localHead_&&bullets_.activeCount()==0){
            bool ready=false;
            if(stageTransitionOwner_){
                const auto state=stageTransitionOwner_->state();
                ready=state.loaded&&state.begun&&!state.failed&&state.error.empty()
                    &&state.settlementDone&&state.controlApplied&&state.routePlanned&&state.requested
                    &&!state.committed&&state.messageDone&&state.currentId==stageTransitionOwnerId_
                    &&state.progress==stageTransitionOwnerProgress_&&state.nextId>=1&&state.nextId<=16
                    &&state.controlActionRecreate&&!state.controlGate&&state.controlCurrent==7&&state.controlPending==7;
            }
            if(!ready&&ordinaryTerminalDrain_.bound()){
                std::string reason;ready=ordinaryTerminalDrain_.ready(reason);
                if(!ready&&!reason.empty())fail(reason);
            }
            if(!ready)fail("stage controller transition is not implemented");
        }
        executing_=false;
        if(!error_.empty())return false;
        ++frame_;return true;
    }
    const auto count=entities_.size();
    for(std::size_t index=0;index<count&&error_.empty();++index) {
        auto& entity=*entities_[index];
        if(entity.state.active)run(entity);
    }
    if(error_.empty())advanceLaserPhase();
    if(error_.empty())advanceBulletPhase();
    executing_=false;
    if(!error_.empty())return false;
    ++frame_;
    return true;
}

std::vector<EnemySnapshot> StageWorld::entities() const {
    std::vector<EnemySnapshot> result;result.reserve(entities_.size());
    for(const auto& entity:entities_)result.push_back(entity->state);
    return result;
}
}
