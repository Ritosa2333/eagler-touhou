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
int privateInteger=0,lastHealth=0,spawnedKind=0;
std::array<float,32> snapshot;
ufo_system::EnemyHooks hooks(){
    ufo_system::EnemyHooks h;
    h.spawn=[](const ufo_system::EnemySpawn& r){lastHealth=r.health;spawnedKind=r.routine=="UFO_Red"?1:r.routine=="UFO_Blue"?2:r.routine=="UFO_Green"?3:-1;enemy={true,enemySequence++,r.position,r.health};return enemy.id;};
    h.query=[](uint32_t id){return id==enemy.id?enemy:ufo_system::EnemyView{};};
    h.setPrivateInteger=[](uint32_t,int,int value){privateInteger=value;};return h;
}
void execute(const ufo_system::RewardPlan& plan){for(const auto& s:plan.spawns)itemManager.spawn(s);}
extern "C" {
void reset(int difficulty,int power){itemManager.reset();ufoManager.reset();resources=item_system::ResourceState::initial(difficulty);resources.power=power;player=item_system::PlayerView::native(0,{0,400});enemy={};enemySequence=1;privateInteger=lastHealth=spawnedKind=0;}
int spawn(int type,float x,float y){auto* item=itemManager.spawn(type,{x,y});return item?int(item->id):0;}
void set_player(int character,float x,float y,int focus,int state){player=item_system::PlayerView::native(character,{x,y},focus,state);}
void item_tick(){item_system::FrameView f;f.player=player;f.ufo=ufoManager.view(hooks());f.inventoryColors=&ufoManager.inventory.colors;f.inventoryCount=&ufoManager.inventory.count;
    itemManager.tick(f,[](const item_system::Event& event){using item_system::EventKind;switch(event.kind){
    case EventKind::CollectOrdinary:item_system::collectOrdinary(resources,event.entity,player);break;
    case EventKind::CollectAggregate:item_system::collectAggregate(resources,event.entity);break;
    case EventKind::CollectToken:execute(ufoManager.collectToken(event.value,resources));break;
    case EventKind::Absorbed:execute(ufoManager.absorb(event.value,1));break;
    case EventKind::RankDelta:resources.addRank(event.value);break;
    default:break;}});
}
void ufo_tick(int boss){ufo_system::WorldView w;w.power=resources.power;w.bossPresent=boss;ufoManager.tick(w,hooks());}
void token(int color){execute(ufoManager.collectToken(color,resources));}
void absorb(int type,int stage){execute(ufoManager.absorb(type,stage));}
void fixture_ufo(int kind,float fill,int a,int b,int power,int color){ufoManager.phase=ufo_system::Phase::Active;ufoManager.kind=kind;ufoManager.fill=fill;ufoManager.bucketA=a;ufoManager.bucketB=b;ufoManager.position={0,128};ufoManager.inventory.colors={1,2,color};ufoManager.inventory.count=3;enemy={true,77,{0,128},1000};ufoManager.enemyId=77;resources.power=power;}
void finish(){execute(ufoManager.finish(ufoManager.enemyId,resources));enemy.active=false;}
void fixture_item(int id,int age,int timer,float vx,float vy){if(auto* item=itemManager.find(id)){item->elapsed=float(age);item->age=age;item->colorElapsed=float(timer);item->colorTimer=timer;item->velocity={vx,vy};}}
void fixture_state(int id,int state,float attractionSpeed,float aggregateSpeed){if(auto* item=itemManager.find(id)){item->state=item_system::State(state);item->attractionSpeed=attractionSpeed;item->aggregateSpeed=aggregateSpeed;}}
void fixture_ufo_age(int age){ufoManager.age=age;ufoManager.elapsed=float(age);}
void fixture_fragments(int life,int bomb){resources.lifeFragments=life;resources.bombFragments=bomb;}
void death_drops(float x,float y){const auto plan=item_system::deathPowerDrops(resources,{x,y});for(const auto& s:plan.spawns)itemManager.spawn(s);}
void collect_aggregate(int kind,int a,int b,float multiplier,int power){resources=item_system::ResourceState::initial(1);resources.power=power;item_system::Entity e;e.aggregateKind=kind;e.bucketA=a;e.bucketB=b;e.multiplier=multiplier;item_system::collectAggregate(resources,e);}
const float* item(int id){snapshot={};if(const auto* e=itemManager.find(id)){snapshot={float(e->id),float(e->type),float(int(e->state)),e->position.x,e->position.y,e->velocity.x,e->velocity.y,float(e->age),float(e->colorTimer),float(e->visualScript),float(e->visualAge),float(e->visualInterrupt),float(e->near),float(e->aggregateKind),float(e->bucketA),float(e->bucketB),e->multiplier,e->alpha,float(e->delay),e->attractionSpeed,e->aggregateSpeed};}return snapshot.data();}
const float* state(){snapshot={float(itemManager.activeCount()),float(ufoManager.inventory.colors[0]),float(ufoManager.inventory.colors[1]),float(ufoManager.inventory.colors[2]),float(ufoManager.inventory.count),float(ufoManager.inventory.displayState),float(ufoManager.inventory.combination),float(ufoManager.phase),float(ufoManager.kind),float(ufoManager.age),ufoManager.fill,float(ufoManager.bucketA),float(ufoManager.bucketB),float(resources.power),float(resources.scoreStored),float(resources.pointValueStored),float(resources.lives),float(resources.lifeFragments),float(resources.bombs),float(resources.bombFragments),float(resources.rank),float(lastHealth),float(privateInteger),float(itemManager.poolMisses)};return snapshot.data();}
int nth_id(int n){for(const auto& e:itemManager.entities)if(e.active()&&n--==0)return e.id;return 0;}
}
