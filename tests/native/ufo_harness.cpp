#include "../../src/items.hpp"
#include "../../src/ufo.hpp"
#include <array>
using namespace th12;
item_system::Manager itemManager;
ufo_system::Manager ufoManager;
item_system::ResourceState resources;
item_system::PlayerView player;
ufo_system::EnemyView enemy;
uint32_t enemySequence=1;
int privateInteger=0,lastHealth=0,spawnedKind=0,pendingDeathFlag=0,effectsIssued=0;
std::array<float,32> snapshot;
std::vector<std::array<uint32_t,2>> animationVisits;
ufo_system::EnemyHooks hooks(){
    ufo_system::EnemyHooks h;
    h.spawn=[](const ufo_system::EnemySpawn& r){lastHealth=r.health;spawnedKind=r.routine=="UFO_Red"?1:r.routine=="UFO_Blue"?2:r.routine=="UFO_Green"?3:-1;enemy={true,enemySequence++,r.position,r.health};return enemy.id;};
    h.query=[](uint32_t id){return id==enemy.id?enemy:ufo_system::EnemyView{};};
    h.setPrivateInteger=[](uint32_t,int,int value){privateInteger=value;};return h;
}
void execute(const ufo_system::RewardPlan& plan){effectsIssued+=int(plan.effects.size());for(const auto& s:plan.spawns)itemManager.spawn(s);}
void ufoFrame(bool boss,bool dialogue){
    ufo_system::WorldView world;world.power=resources.power;world.powerStep=resources.powerStep;world.bossPresent=boss;world.dialogue=dialogue;
    const auto result=ufoManager.tick(world,hooks());
    for(const auto& action:result.actions){
        if(action.kind==ufo_system::EnemyActionKind::SetPrivateInteger)privateInteger=action.value;
        else pendingDeathFlag|=action.value; // Deliberately deferred to a separate Enemy update.
    }
}
extern "C" {
void reset(int difficulty,int power){itemManager.reset();ufoManager.reset();resources=item_system::ResourceState::initial(difficulty);resources.power=power;player=item_system::PlayerView::native(0,{0,400});enemy={};enemySequence=1;privateInteger=lastHealth=spawnedKind=pendingDeathFlag=effectsIssued=0;}
int spawn(int type,float x,float y){auto* item=itemManager.spawn(type,{x,y});return item?int(item->id):0;}
void set_player(int character,float x,float y,int focus,int state){player=item_system::PlayerView::native(character,{x,y},focus,state);}
void item_tick(){animationVisits.clear();item_system::FrameView f;f.player=player;f.ufo=ufoManager.view(hooks());f.inventoryColors=&ufoManager.inventory.colors;f.inventoryCount=&ufoManager.inventory.count;
    f.animate=[](const item_system::Entity& e){animationVisits.push_back({e.id,uint32_t(e.age)});};
    itemManager.tick(f,[](const item_system::Event& event){using item_system::EventKind;switch(event.kind){
    case EventKind::CollectOrdinary:item_system::collectOrdinary(resources,event.entity,player);break;
    case EventKind::CollectAggregate:item_system::collectAggregate(resources,event.entity);break;
    case EventKind::CollectToken:execute(ufoManager.collectToken(event.value,resources));break;
    case EventKind::Absorbed:execute(ufoManager.absorb(event.value,1));break;
    case EventKind::RankDelta:resources.addRank(event.value);break;
    default:break;}});
}
void ufo_tick(int boss){ufoFrame(boss,false);}
void ufo_tick_dialogue(int boss,int dialogue){ufoFrame(boss,dialogue);}
void token(int color){execute(ufoManager.collectToken(color,resources));}
void absorb(int type,int stage){execute(ufoManager.absorb(type,stage));}
void fixture_ufo(int kind,float fill,int a,int b,int power,int color){ufoManager.phase=ufo_system::Phase::Active;ufoManager.kind=kind;ufoManager.fill=fill;ufoManager.bucketA=a;ufoManager.bucketB=b;ufoManager.position={0,128};ufoManager.inventory.colors={1,2,color};ufoManager.inventory.count=3;enemy={true,77,{0,128},1000};ufoManager.enemyId=77;resources.power=power;}
void finish(){execute(ufoManager.finish(ufoManager.enemyId,resources));enemy.active=false;}
void apply_enemy_death(){if(pendingDeathFlag&&enemy.active){execute(ufoManager.finish(enemy.id,resources,ufo_system::DeathReason::Dialogue));enemy.active=false;pendingDeathFlag=0;}}
void removed(int reason){ufoManager.notifyEnemyRemoved(enemy.id,ufo_system::RemovalReason(reason));enemy.active=false;}
void direct_reset(){ufoManager.directReset();}
int enemy_alive(){return enemy.active;}
int death_requested(){return pendingDeathFlag;}
int effects(){return effectsIssued;}
int reward_frames(){return ufoManager.rewardDisplayFrames;}
void fixture_inventory(int a,int b,int c,int count){ufoManager.inventory.colors={a,b,c};ufoManager.inventory.count=count;}
int hud_rotation(){return ufoManager.inventory.rotation;}
int hud_event_count(){return int(ufoManager.inventory.hudEvents.size());}
const float* hud_event(int index){snapshot={};if(index>=0&&index<int(ufoManager.inventory.hudEvents.size())){const auto& e=ufoManager.inventory.hudEvents[index];snapshot={float(e.sequence),float(e.target),float(e.operation),float(e.logicalSlot),float(e.physicalSlot),float(e.script),float(e.layer),float(e.value),float(e.overlayId),float(e.immediate)};}return snapshot.data();}
int visual_event_count(){return int(ufoManager.visualEvents.size());}
const float* visual_event(int index){snapshot={};if(index>=0&&index<int(ufoManager.visualEvents.size())){const auto& e=ufoManager.visualEvents[index];snapshot={float(e.sequence),float(e.kind),float(e.enemyId),e.position.x,e.position.y,float(e.ufoKind),float(e.age),e.fill,e.multiplier,float(e.scoreDisplayed),float(e.interrupt),float(e.bulletScript),float(e.noticeScript),float(e.panelScript),float(e.healthScript),float(e.sound)};}return snapshot.data();}
void clear_events(){ufoManager.inventory.drainHudEvents();ufoManager.drainVisualEvents();}
int item_event_count(){return int(itemManager.events.size());}
int item_event_kind(int index){return index>=0&&index<int(itemManager.events.size())?int(itemManager.events[index].kind):-1;}
int animation_count(){return int(animationVisits.size());}
const float* animation_visit(int index){snapshot={};if(index>=0&&index<int(animationVisits.size()))snapshot={float(animationVisits[index][0]),float(animationVisits[index][1])};return snapshot.data();}
void fixture_item(int id,int age,int timer,float vx,float vy){if(auto* item=itemManager.find(id)){item->elapsed=float(age);item->age=age;item->colorElapsed=float(timer);item->colorTimer=timer;item->velocity={vx,vy};}}
void fixture_state(int id,int state,float attractionSpeed,float aggregateSpeed){if(auto* item=itemManager.find(id)){item->state=item_system::State(state);item->attractionSpeed=attractionSpeed;item->aggregateSpeed=aggregateSpeed;}}
void fixture_ufo_age(int age){ufoManager.age=age;ufoManager.elapsed=float(age);}
void fixture_private(int value){privateInteger=value;}
void fixture_fragments(int life,int bomb){resources.lifeFragments=life;resources.bombFragments=bomb;}
void death_drops(float x,float y){const auto plan=item_system::deathPowerDrops(resources,{x,y});for(const auto& s:plan.spawns)itemManager.spawn(s);}
void collect_aggregate(int kind,int a,int b,float multiplier,int power){resources=item_system::ResourceState::initial(1);resources.power=power;item_system::Entity e;e.aggregateKind=kind;e.bucketA=a;e.bucketB=b;e.multiplier=multiplier;item_system::collectAggregate(resources,e);}
const float* item(int id){snapshot={};if(const auto* e=itemManager.find(id)){snapshot={float(e->id),float(e->type),float(int(e->state)),e->position.x,e->position.y,e->velocity.x,e->velocity.y,float(e->age),float(e->colorTimer),float(e->visualScript),float(e->visualAge),float(e->visualInterrupt),float(e->near),float(e->aggregateKind),float(e->bucketA),float(e->bucketB),e->multiplier,e->alpha,float(e->delay),e->attractionSpeed,e->aggregateSpeed,float(e->hintActive),float(e->hintInterrupt),float(e->hintGeneration)};}return snapshot.data();}
const float* state(){snapshot={float(itemManager.activeCount()),float(ufoManager.inventory.colors[0]),float(ufoManager.inventory.colors[1]),float(ufoManager.inventory.colors[2]),float(ufoManager.inventory.count),float(ufoManager.inventory.displayState),float(ufoManager.inventory.combination),float(ufoManager.phase),float(ufoManager.kind),float(ufoManager.age),ufoManager.fill,float(ufoManager.bucketA),float(ufoManager.bucketB),float(resources.power),float(resources.scoreStored),float(resources.pointValueStored),float(resources.lives),float(resources.lifeFragments),float(resources.bombs),float(resources.bombFragments),float(resources.rank),float(lastHealth),float(privateInteger),float(itemManager.poolMisses)};return snapshot.data();}
int nth_id(int n){for(const auto& e:itemManager.entities)if(e.active()&&n--==0)return e.id;return 0;}
}
