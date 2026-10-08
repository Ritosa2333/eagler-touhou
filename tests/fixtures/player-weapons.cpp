#include "../../src/game/GameState.hpp"
#include "../../src/game/FriendlyPool.hpp"
namespace th12 {
float snapshot[16];
pw::Leaf fixtureLeaf;
pw::ReimuABeam fixtureBeam;
pw::ReimuBOrb fixtureOrb;
extern "C" {
void test_reset(int c,int seed,float x,float y,float angle,float speed,int extra,int update,int animation,int hitAnimation,int damage){
  bullets.clear();resetFriendlyPool();enemies.clear();playerDamageSources.reset();character=c;random={uint16_t(seed),0};nextProjectileId=1;
  Projectile b{x,y,angle,speed,12,extra?0:4,0,0,damage,animation,true};b.physicalSlot=0;ShotSpec s;s.w=s.h=24;s.extraCallback=extra;s.update=update;s.hitAnimation=hitAnimation-5;initializeFriendlyProjectile(b,s);bullets.push_back(b);attachFriendlySlot(bullets.back());
}
int test_query(float x,float y,float w,float h){Enemy e;e.x=x;e.y=y;e.hitWidth=w;e.hitHeight=h;int damage=0;for(size_t i=0;i<bullets.size();++i)damage+=friendlyProjectileDamage(bullets[i],e);return std::min(80,damage+playerSourceDamage(e));}
void test_update(){tickPlayerDamageSources();for(auto& b:bullets)if(b.active){updateFriendlyProjectile(b);++b.age;}}
const float* test_state(int i){const auto& b=bullets.at(i);snapshot[0]=b.x;snapshot[1]=b.y;snapshot[2]=b.angle;snapshot[3]=b.speed;snapshot[4]=int(b.weapon.phase);snapshot[5]=b.animation;snapshot[6]=b.weapon.visualAge;snapshot[7]=b.weapon.interrupt;snapshot[8]=b.age;snapshot[9]=b.damage;snapshot[10]=b.active;snapshot[11]=b.hitAnimation;return snapshot;}
int test_count(){return bullets.size();}
int test_rng_seed(){return random.seed;}
int test_rng_calls(){return random.calls;}
void test_leaf_order_begin(){playerBomb.reset();random={0x1234,0};nextProjectileId=1;for(int i=0;i<2;++i){pw::Leaf leaf;leaf.begin(nextProjectileId++,20,300,random);playerBomb.leaves.push_back(leaf);}}
void test_leaf_order_tick(int born){if(born){pw::Leaf leaf;leaf.begin(nextProjectileId++,20,300,random);playerBomb.leaves.push_back(leaf);}tickPlayerBombEffects();}
const float* test_leaf_order_state(int i){if(i<0){snapshot[0]=playerBomb.leaves.size();return snapshot;}const auto& leaf=playerBomb.leaves.at(i);snapshot[0]=leaf.id;snapshot[1]=leaf.age;snapshot[2]=leaf.heading;return snapshot;}
void test_source_reset(){playerDamageSources.reset();playerDamageSources.circle(0,120,12,1.4f,15,4);}
void test_source_tick(){tickPlayerDamageSources();}
const float* test_source(){const auto& s=playerDamageSources.sources[0];snapshot[0]=s.radius;snapshot[1]=s.growth;snapshot[2]=s.previousTicks;snapshot[3]=s.ticks;snapshot[4]=s.damage;snapshot[5]=s.flags;return snapshot;}
int test_source_damage(float x,float y,float w,float h){Enemy e;e.x=x;e.y=y;e.hitWidth=w;e.hitHeight=h;return playerSourceDamage(e);}
int test_cadence(int age,int previous,int type){return pw::projectileCadence(age,previous,type);}
int test_rect(int type,float x,float y,float w,float h){return pw::rectangle(0,120,24,24,x,y,w,h,type==2);}
int test_marisa_b_damage(int age,float y){return pw::marisaBDamage(age,y);}
const float* test_marisa_b_area(int age){auto a=pw::marisaBArea(age);snapshot[0]=a.enabled;snapshot[1]=a.x;snapshot[2]=a.y;snapshot[3]=a.width;snapshot[4]=a.height;return snapshot;}
void test_leaf_begin(){random={};fixtureLeaf.begin(1,20,300,random);}
void test_leaf_tick(){fixtureLeaf.tick(random);}
const float* test_leaf_state(){snapshot[0]=fixtureLeaf.age;snapshot[1]=fixtureLeaf.heading;snapshot[2]=fixtureLeaf.active;snapshot[3]=random.seed;snapshot[4]=random.calls/2;return snapshot;}
float test_sanae_b_radius(int age){return pw::sanaeBClearRadius(age);}
int test_marisa_a_damage(int age,float x,float y,float w,float h){return pw::marisaADamage(age,age-1,20,300,x,y,w,h);}
const float* test_marisa_a_area(int i){auto a=pw::marisaAAreas(20,300)[i];snapshot[0]=a.x;snapshot[1]=a.y;snapshot[2]=a.width;snapshot[3]=a.height;return snapshot;}
void test_reimu_a_beam_begin(float offset){playerDamageSources.reset();fixtureBeam.begin(1,20+offset,20,playerDamageSources);}
void test_reimu_a_beam_tick(){fixtureBeam.tick(playerDamageSources);}
const float* test_reimu_a_beam_state(){const auto& s=playerDamageSources.sources[0];snapshot[0]=fixtureBeam.x;snapshot[1]=fixtureBeam.y;snapshot[2]=fixtureBeam.heading;snapshot[3]=fixtureBeam.age;snapshot[4]=s.x;snapshot[5]=s.y;snapshot[6]=s.angle;snapshot[7]=s.width;snapshot[8]=s.height;snapshot[9]=s.flags;return snapshot;}
void test_reimu_a_emitter_begin(){playerBomb.reset();playerDamageSources.reset();playerBomb.loadout=0;playerBomb.active=true;playerBomb.x=20;playerBomb.y=300;}
void test_reimu_a_emitter_tick(){tickPlayerBombEffects();}
const float* test_reimu_a_emitter_state(){const auto& s=playerBomb.reimuAEmitter;snapshot[0]=s.age;snapshot[1]=s.first;snapshot[2]=s.second;snapshot[3]=s.firstStep;snapshot[4]=s.secondStep;snapshot[5]=s.active;return snapshot;}
void test_reimu_b_begin(int i){playerDamageSources.reset();fixtureOrb.begin(1,i,20,300,playerDamageSources);}
void test_reimu_b_tick(int target){fixtureOrb.tick(20,300,target,40,120);}
const float* test_reimu_b_state(){snapshot[0]=fixtureOrb.x;snapshot[1]=fixtureOrb.y;snapshot[2]=fixtureOrb.centerX;snapshot[3]=fixtureOrb.centerY;snapshot[4]=fixtureOrb.heading;snapshot[5]=fixtureOrb.radius;snapshot[6]=fixtureOrb.speed;snapshot[7]=fixtureOrb.deltaX;snapshot[8]=fixtureOrb.deltaY;snapshot[9]=fixtureOrb.polar;snapshot[10]=fixtureOrb.age;return snapshot;}
int test_sanae_a_damage(float y){return pw::sanaeADamage(y);}
void test_sanae_a_begin(){playerBomb.reset();character=2;shot=0;startPlayerBomb();}
const float* test_bomb_tick(){tickPlayerBomb();snapshot[0]=playerBomb.age;snapshot[1]=playerBomb.active;snapshot[2]=invuln;snapshot[3]=playerBomb.rectangles.size();snapshot[4]=playerBomb.visuals.empty()?-1:playerBomb.visuals[0].interrupt;return snapshot;}
const float* test_reimu_b_explosion(){playerBomb.reset();playerDamageSources.reset();fixtureOrb.begin(1,0,20,300,playerDamageSources);fixtureOrb.x=20;fixtureOrb.y=300;explodeReimuBOrb(fixtureOrb);const auto& s=playerDamageSources.sources[1];snapshot[0]=playerBomb.circles[0].radius;snapshot[1]=s.radius;snapshot[2]=s.growth;snapshot[3]=s.ticks;snapshot[4]=s.damage;snapshot[5]=s.flags;snapshot[6]=playerDamageSources.sources[0].flags;snapshot[7]=fixtureOrb.active;return snapshot;}
const float* test_effect_restart(){playerBomb.reset();playerDamageSources.reset();random={};pw::Leaf leaf;leaf.begin(1,20,300,random);playerBomb.leaves.push_back(leaf);pw::ReimuABeam beam;beam.begin(2,20,20,playerDamageSources);playerBomb.reimuABeams.push_back(beam);playerBomb.loadout=5;character=2;shot=1;startPlayerBomb();tickPlayerBombEffects();snapshot[0]=playerBomb.leaves.size();snapshot[1]=playerBomb.leaves[0].age;snapshot[2]=random.seed;snapshot[3]=random.calls/2;snapshot[4]=playerBomb.reimuABeams.size();snapshot[5]=playerBomb.reimuABeams[0].y;snapshot[6]=playerDamageSources.sources[0].angle;return snapshot;}
}
}
