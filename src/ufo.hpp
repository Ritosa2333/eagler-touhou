// TH12 1.00b UFO inventory, native enemy bridge and rewards. No synthetic movement.
#pragma once
#include "items.hpp"
#include <functional>
#include <string>

namespace th12::ufo_system {
using item_system::Vec2;
using item_system::SpawnRequest;
using item_system::ResourceState;
using item_system::RewardDelta;
enum class Kind : int { Rainbow=-1,None=0,Red=1,Blue=2,Green=3 };
struct Inventory {
    std::array<int,3> colors{}; // Retail encoding: red 1, blue 2, green 3.
    int count=0,displayState=0,gateTimer=0,combination=0;
    int append(int color){
        if(color<=0)return 0;
        gateTimer=0;if(colors[2])return 0;
        if(!colors[0]){colors[0]=color;count=displayState=1;return 0;}
        if(!colors[1]){colors[1]=color;count=displayState=2;return 0;}
        colors[2]=color;count=displayState=3;
        if(colors[0]==color&&colors[1]==color){combination=color;return color;}
        if(colors[0]!=color&&colors[1]!=color&&colors[0]!=colors[1]){combination=-1;return -1;}
        colors={colors[1],color,0};count=2;displayState=4;return 0;
    }
    void clearSlots(){colors={};count=displayState=0;}
    bool ready()const{return displayState==3&&gateTimer>=0;}
};
struct RewardPlan {
    // Current resources are changed only by collectToken(); finish/absorb spawn pickups.
    // scoreStored is displayed score / 10; pointValueStored is displayed PIV * 100.
    RewardDelta immediate{};
    std::vector<SpawnRequest> spawns;
    bool inventoryChanged=false,filledNow=false;
};
struct EnemySpawn {std::string routine;Vec2 position{0,128};int health=1600;};
struct EnemyView {bool active=false;uint32_t id=0;Vec2 position{};int health=0;};
struct EnemyHooks {
    std::function<uint32_t(const EnemySpawn&)> spawn;
    std::function<EnemyView(uint32_t)> query;
    // index 0 maps to original enemy-private ECL integer -9985.
    std::function<void(uint32_t,int,int)> setPrivateInteger;
};
struct WorldView {int power=100,stage=1;bool bossPresent=false,dialogue=false;float rate=1;};
enum class Phase { Inactive,Active,Finished };

class Manager {
public:
    Inventory inventory{};
    Phase phase=Phase::Inactive;
    uint32_t enemyId=0,spawnFailures=0,invalidEvents=0;
    int kind=0,age=0,duration=720,bucketA=0,bucketB=0;
    float elapsed=0,fill=0;
    Vec2 position{0,128};
    bool rewardIssued=false;
    // Semantic values displayed by native HUD after a burst (display score units).
    int rewardDisplayFrames=0,rewardDisplayScore=0;
    float rewardDisplayMultiplier=0;
    void reset(){*this=Manager{};}
    RewardPlan collectToken(int color,ResourceState& resources){
        RewardPlan plan;if(color<0||color>2){++invalidEvents;return plan;}
        if(inventory.count>=3){const ResourceState before=resources;resources.addPointValue(100000);plan.immediate=item_system::difference(before,resources);return plan;}
        inventory.append(color+1);plan.inventoryChanged=true;return plan;
    }
    void tick(const WorldView& world,const EnemyHooks& hooks){
        if(phase!=Phase::Inactive){
            const EnemyView enemy=hooks.query?hooks.query(enemyId):EnemyView{};
            if(!enemy.active){phase=Phase::Inactive;enemyId=0;inventory.clearSlots();}
            else{
                position=enemy.position;
                if(hooks.setPrivateInteger){
                    if(age==duration-300||world.bossPresent)hooks.setPrivateInteger(enemyId,0,999);
                    else if(age<duration-300)hooks.setPrivateInteger(enemyId,0,-999);
                }
            }
        }else if(inventory.ready()){
            const int selected=inventory.combination;
            if(selected!=-1&&selected!=1&&selected!=2&&selected!=3){++invalidEvents;return;}
            static const char* names[4]={"UFO_Red","UFO_Blue","UFO_Green","UFO_Rainbow"};
            constexpr int health[5]={1600,1900,2100,2300,2500};
            EnemySpawn request;request.routine=names[selected==-1?3:selected-1];request.health=health[std::clamp(world.power/100,0,4)];
            const uint32_t id=hooks.spawn?hooks.spawn(request):0;
            if(!id){++spawnFailures;return;}
            enemyId=id;kind=selected;phase=Phase::Active;position=request.position;age=0;elapsed=0;fill=0;bucketA=bucketB=0;rewardIssued=false;
        }
        if(rewardDisplayFrames>0)--rewardDisplayFrames;
        elapsed=item_system::add(elapsed,world.rate);age=int(elapsed);
    }
    item_system::UfoView view()const{return {phase==Phase::Active,kind,age,position};}
    item_system::UfoView view(const EnemyHooks& hooks)const{
        auto result=view();if(result.active&&hooks.query){const auto e=hooks.query(enemyId);result.active=e.active;result.position=e.position;}return result;
    }
    RewardPlan absorb(int type,int stage=1){
        RewardPlan plan;if(phase!=Phase::Active){++invalidEvents;return plan;}
        if(type==1){if(kind==-1)++bucketB;else ++bucketA;}
        else if(type==2){if(kind==-1)++bucketA;else ++bucketB;}
        else if(type<10||type>12){++invalidEvents;return plan;}
        if(fill>=1)return plan;
        const double increment=(type==1||type==2)?0.03200000151991844-0.0020000000949949026*stage:0.10000000149011612;
        fill=float(double(fill)+increment);
        if(fill<1)return plan;
        fill=1;plan.filledNow=true;
        int typeToSpawn=kind==1?4:kind==3?7:kind==-1?changingTokenType():0;
        if(typeToSpawn)plan.spawns.push_back(SpawnRequest{typeToSpawn,position});
        return plan;
    }
    float multiplier(const ResourceState& resources)const{
        switch(kind){
        case 1:return resources.power>=resources.maxPower?2.f:1.f;
        case 2:return fill>=1?8.f:fill>=.5f?6.f:float(2+double(fill)*8);
        case 3:return 2.f;
        case -1:return fill>=1?4.f:fill>=.5f?3.f:float(2+double(fill)*2);
        default:return 1.f;
        }
    }
    RewardPlan finish(uint32_t killedId,const ResourceState& resources){
        RewardPlan plan;if(killedId!=enemyId||phase!=Phase::Active||rewardIssued)return plan;
        rewardIssued=true;phase=Phase::Finished;const float factor=multiplier(resources);
        if(bucketA||bucketB){
            const int type=bucketA&&bucketB?18:bucketA?16:17;
            SpawnRequest aggregate{type,position,-item_system::pi/2,0};aggregate.aggregateKind=kind;aggregate.bucketA=bucketA;aggregate.bucketB=bucketB;aggregate.multiplier=factor;
            plan.spawns.push_back(aggregate);
            rewardDisplayFrames=120;rewardDisplayMultiplier=factor;
            const int pointValue=resources.pointValue();
            rewardDisplayScore=kind==1?int(double(pointValue*bucketA)*factor)+pointValue*bucketB:int(double(pointValue*bucketB)*factor);
        }
        if(kind==1){plan.spawns.push_back(SpawnRequest{4,position});plan.spawns.push_back(SpawnRequest{13,position});}
        else if(kind==2)plan.spawns.push_back(SpawnRequest{14,position});
        else if(kind==3){plan.spawns.push_back(SpawnRequest{5,position});plan.spawns.push_back(SpawnRequest{15,position});}
        else if(kind==-1){const int token=changingTokenType();if(token)plan.spawns.push_back(SpawnRequest{token,position});}
        return plan;
    }
private:
    int changingTokenType(){const int color=inventory.colors[2];if(color<1||color>3){++invalidEvents;return 0;}return color+12;}
};
} // namespace th12::ufo_system
