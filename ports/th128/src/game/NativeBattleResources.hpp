#pragma once
#include "StageWorld.hpp"
#include "BulletVisuals.hpp"
#include "EnemyVisuals.hpp"
#include "PlayerHitHost.hpp"
#include "PlayerShotHost.hpp"
#include "PlayerShotGroups.hpp"
#include "ItemHost.hpp"
#include "PerfectHud.hpp"
#include "BossHud.hpp"
#include "Geometry526Host.hpp"
#include "DamageFieldPool.hpp"
#include "FocusIndicator.hpp"
#include "PlayerFireHost.hpp"
#include "PerfectFreezeHost.hpp"
#include "IceHost.hpp"
#include "CancellationEffects.hpp"
#include "MessageHost.hpp"
#include "MessageVisuals.hpp"
#include "RouteChoiceHost.hpp"
#include "StageTransitionHost.hpp"
#include "StageSettlementHost.hpp"
#include "StageIntroLogos.hpp"
#include "SpellVisuals.hpp"
#include "ending/EndingRuntime.hpp"
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace th128::postrun {

class BattleResources {
    bool attempted_=false,retired_=false;
    unsigned completedOwners_=0;
    std::string failure_;
    std::vector<unsigned> order_;
public:
    std::unique_ptr<StageWorld> world=std::make_unique<StageWorld>();
    std::unique_ptr<laser::LineHost> laserLines=std::make_unique<laser::LineHost>();
    std::unique_ptr<stageintro::Host> stageIntro=std::make_unique<stageintro::Host>();
    std::unique_ptr<bullet::BulletVisuals> bulletVisuals=std::make_unique<bullet::BulletVisuals>();
    std::unordered_map<std::uint32_t,bullet::BulletVisuals::Visual> bulletAnimations;
    std::unordered_map<int,bullet::Profile> bulletProfiles;
    std::unordered_map<int,std::unordered_map<int,enemy::Bounds>> enemyAppearances;
    std::unique_ptr<enemy::EnemyVisuals> enemyVisuals=std::make_unique<enemy::EnemyVisuals>();
    std::unique_ptr<effects::CancellationEffects> cancellationEffects=std::make_unique<effects::CancellationEffects>();
    std::unique_ptr<player::PlayerHitHost> playerHit=std::make_unique<player::PlayerHitHost>();
    std::unique_ptr<playershot::PlayerShotHost> playerShots=std::make_unique<playershot::PlayerShotHost>();
    std::unique_ptr<playershot::PlayerShotGroups> playerShotGroups;
    std::unique_ptr<item::ItemHost> itemHost=std::make_unique<item::ItemHost>();
    std::unique_ptr<hud::PerfectHud> perfectHud=std::make_unique<hud::PerfectHud>();
    std::unique_ptr<hud::BossHud> bossHud=std::make_unique<hud::BossHud>();
    std::unique_ptr<geometry526::Host> geometryEffects=std::make_unique<geometry526::Host>();
    std::unique_ptr<damagefield::DamageFieldPool> damageFields=std::make_unique<damagefield::DamageFieldPool>();
    std::unique_ptr<player::FocusIndicator> focusIndicator=std::make_unique<player::FocusIndicator>();
    std::unique_ptr<player::fire::Host> playerFire=std::make_unique<player::fire::Host>();
    std::unique_ptr<ice::Host> iceHost=std::make_unique<ice::Host>();
    std::unique_ptr<perfect::Host> perfectHost=std::make_unique<perfect::Host>();
    std::unique_ptr<message::Host> messages=std::make_unique<message::Host>();
    std::unique_ptr<message::Visuals> messageVisuals=std::make_unique<message::Visuals>();
    std::unique_ptr<routechoice::Host> routeChoice=std::make_unique<routechoice::Host>();
    std::unique_ptr<stage::StageTransitionHost> stageTransition=std::make_unique<stage::StageTransitionHost>();
    std::unique_ptr<ending::Runtime15> endingRuntime15=std::make_unique<ending::Runtime15>();
    std::unique_ptr<stage::StageSettlementHost> debugTerminalSettlement=std::make_unique<stage::StageSettlementHost>();
    std::unique_ptr<spell::Visuals> spellVisuals=std::make_unique<spell::Visuals>();
    std::vector<std::array<float,5>> scorePopups;
    anm::Registry scoreFont;

    bool retire(std::string& error){
        error.clear();
        if(attempted_||!world||world->faulted()){error=failure_.empty()?"battle resource retirement is not fresh/healthy":failure_;return false;}
        attempted_=true;
        try{
            const auto frame=world->frame();const auto random=world->gameRandomState();
            for(unsigned phase=0;phase<14;++phase){
                if(phase==0){}
                else if(phase==1){}
                else if(phase==2)endingRuntime15.reset();
                else if(phase==3){
                    if(stageIntro)stageIntro->cleanup();stageIntro.reset();
                    messages.reset();messageVisuals.reset();routeChoice.reset();stageTransition.reset();
                    debugTerminalSettlement.reset();perfectHud.reset();bossHud.reset();
                }else if(phase==4){
                    if(playerShotGroups&&playerShotGroups->state().configured&&!playerShotGroups->clear(1,error))throw std::runtime_error(error);
                    playerShotGroups.reset();playerShots.reset();playerHit.reset();playerFire.reset();focusIndicator.reset();
                }else if(phase==5){
                    bulletAnimations.clear();
                    if(!world->retireBulletManagerForSession(error))throw std::runtime_error(error);
                    bulletProfiles.clear();
                }else if(phase==6)itemHost.reset();
                else if(phase==7){if(laserLines)laserLines->reset();}
                else if(phase==8){scorePopups.clear();scoreFont=anm::Registry{};}
                else if(phase==9){
                    if(!world->retireEntitiesForSession(error))throw std::runtime_error(error);
                    if(enemyVisuals){
                        std::unordered_set<unsigned> owners;
                        for(const auto& visual:enemyVisuals->snapshots())owners.insert(visual.entity);
                        for(const auto owner:owners)if(!enemyVisuals->removeOwner(owner))throw std::runtime_error("concrete remaining enemy visual removal rejected");
                    }
                    enemyVisuals.reset();geometryEffects.reset();enemyAppearances.clear();
                }else if(phase==10){bulletVisuals.reset();cancellationEffects.reset();laserLines.reset();}
                else if(phase==11){if(perfectHost&&!perfectHost->shutdown(error))throw std::runtime_error(error);perfectHost.reset();}
                else if(phase==12){spellVisuals.reset();damageFields.reset();}
                else if(phase==13)iceHost.reset();
                order_.push_back(phase);++completedOwners_;
            }
            const auto after=world->gameRandomState();
            if(world->frame()!=frame||after.seed!=random.seed||after.calls!=random.calls)
                throw std::runtime_error("resource retirement mutated final gameplay audit state");
            retired_=true;return true;
        }catch(const std::exception& reason){if(failure_.empty())failure_=reason.what();error=failure_;return false;}
    }
    bool attempted()const{return attempted_;}
    bool retired()const{return retired_;}
    unsigned completedOwners()const{return completedOwners_;}
    const std::vector<unsigned>& releaseOrder()const{return order_;}
    const std::string& error()const{return failure_;}
};

}
