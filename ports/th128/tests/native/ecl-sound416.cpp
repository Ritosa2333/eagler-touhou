#include "game/StageWorld.hpp"
#include "../ecl-fixture.hpp"
#include <cassert>
#include <iostream>
int main(){
 using namespace ecl_fixture;auto spawn=stringWords("boss");spawn.insert(spawn.end(),{floatBits(-32),floatBits(144),100,0,0});
 auto data=program({{"main",join({instruction(257,spawn,0,0,255,6),instruction(0,{},20)})},{"boss",join({instruction(416,{67}),instruction(258,{2}),instruction(262,{0,37}),instruction(269,{0}),instruction(0,{},20)})}});
 th128::StageWorld world;std::string error;int id=-1;float x=0;assert(world.load("sound416.ecl",data.data(),data.size(),error)&&world.start("sound416.ecl",1,0));
 assert(world.setEclSoundSink([&](int identifier,float position,std::string&){id=identifier;x=position;return true;},error));
 std::vector<th128::enemy::AnimationRequest> requests;
 assert(world.setAnimationHost([&](const th128::AnimationBirth& birth,std::string&){requests.push_back(birth.request);return true;},[](unsigned){},error));
 assert(world.setAppearanceBoundsProvider([](int,int){return th128::enemy::Bounds{64,64};},error));
 if(!world.enableLocalEnemyPasses(error)){std::cerr<<world.error()<<' '<<error<<'\n';return 1;}
 assert(world.tick());assert(world.tick());assert(id==67&&x==-32);assert(world.frame()==2);
 assert(requests.size()==2&&requests[0].bank==2&&requests[0].script==37&&requests[1].bank==2&&requests[1].script==42&&requests[1].layer==8);
 for(int rank=0;rank<4;++rank){
  auto ranked=program({{"main",join({instruction(257,spawn,0,0,255,6),instruction(0,{},20)})},{"boss",join({instruction(40,{4}),instruction(436,{floatBits(0),floatBits(1),floatBits(2),floatBits(4),floatBits(8)},0,1),instruction(304,{floatBits(0),floatBits(0)},0,2),instruction(0,{},20)})}});
  th128::StageWorld selected;assert(selected.load("rank436.ecl",ranked.data(),ranked.size(),error)&&selected.start("rank436.ecl",rank,1));
  assert(selected.setAppearanceBoundsProvider([](int,int){return th128::enemy::Bounds{64,64};},error));
  if(!selected.enableLocalEnemyPasses(error)){std::cerr<<selected.error()<<' '<<error<<'\n';return 1;}
  if(!selected.tick()||!selected.tick()){std::cerr<<selected.error()<<'\n';return 1;}
  const auto entities=selected.entities();assert(entities.size()==2&&entities[1].speed==float(1<<rank));
 }
 auto controls=program({{"main",join({instruction(40,{4}),instruction(42,{std::uint32_t(-9986)},0,1),instruction(43,{0},0,1),instruction(413,{},2),instruction(415,{60},2),instruction(0,{},100)})}});
 th128::StageWorld controlled;assert(controlled.load("control.ecl",controls.data(),controls.size(),error)&&controlled.start("control.ecl",1,0));
 if(!controlled.enableLocalEnemyPasses(error)){std::cerr<<controlled.error()<<'\n';return 1;}
 for(int tick=0;tick<3;++tick)if(!controlled.tick()){std::cerr<<controlled.error()<<'\n';return 1;}
 assert(controlled.entities()[0].phaseTimer.current==1&&controlled.entities()[0].protection==59);
 auto drops=program({{"main",join({instruction(410,{1}),instruction(407,{2,3}),instruction(409),instruction(409),instruction(0,{},100)})}});
 th128::StageWorld dropping;assert(dropping.load("drops409.ecl",drops.data(),drops.size(),error)&&dropping.start("drops409.ecl",1,0));unsigned dropCalls=0;
 assert(dropping.setEclDropHost([&](const th128::EnemySnapshot& enemy,std::string&){
  if(dropCalls++==0)assert(enemy.drops.words[0]==1&&enemy.drops.words[2]==3);else assert(enemy.drops.words[0]==0&&enemy.drops.words[2]==0);return true;
 },error));
 assert(dropping.enableLocalEnemyPasses(error)&&dropCalls==2&&dropping.entities()[0].drop==0);
 th128::StageWorld postLocal;auto empty=program({{"main",join({instruction(0,{},100)})}});
 assert(postLocal.load("post-local.ecl",empty.data(),empty.size(),error)&&postLocal.start("post-local.ecl",1,0)&&postLocal.enableLocalEnemyPasses(error));
 assert(postLocal.setEclDropHost([](const th128::EnemySnapshot&,std::string&){return true;},error));
 auto clearing=program({{"main",join({instruction(512,{floatBits(640)}),instruction(513,{floatBits(640)}),instruction(0,{},100)})}});
 th128::StageWorld clear;assert(clear.load("clear.ecl",clearing.data(),clearing.size(),error)&&clear.start("clear.ecl",1,0));
 assert(clear.setEclRadiusCancellation([](th128::bullet::Birth&,std::string&){return true;},0,error));
 if(!clear.enableLocalEnemyPasses(error)){std::cerr<<clear.error()<<'\n';return 1;}
 auto spellEnd=program({{"main",join({instruction(423),instruction(411,{100}),instruction(0,{},100)})}});
 th128::StageWorld ended;assert(ended.load("spell-end.ecl",spellEnd.data(),spellEnd.size(),error)&&ended.start("spell-end.ecl",1,0));
 assert(ended.setPhaseSpellContext({0x10,0,0,0},error));
 if(!ended.enableLocalEnemyPasses(error)){std::cerr<<ended.error()<<'\n';return 1;}
 auto shaking=program({{"main",join({instruction(417,{30,12,0}),instruction(0,{},100)})}});
 th128::StageWorld shake;assert(shake.load("shake.ecl",shaking.data(),shaking.size(),error)&&shake.start("shake.ecl",1,0x1234));
 if(!shake.enableLocalEnemyPasses(error)){std::cerr<<shake.error()<<'\n';return 1;}
 assert(shake.tick()&&shake.randomCalls()==4);assert(shake.screenShake()[0]!=0||shake.screenShake()[1]!=0);
 for(int tick=1;tick<30;++tick)assert(shake.tick());assert((shake.screenShake()==std::array<float,2>{}));
 auto ending=program({{"main",join({instruction(257,spawn,0,0,255,6),instruction(0,{},100)})},{"boss",join({instruction(263,{0,83}),instruction(424,{7}),instruction(405),instruction(0,{},100)})}});
 th128::StageWorld end;assert(end.load("end.ecl",ending.data(),ending.size(),error)&&end.start("end.ecl",1,0));unsigned explosions=0;
 assert(end.setEclEffectHost([&](int bank,int script,std::array<float,3> position,int layer,bool head,std::string&){
  assert(bank==0&&script==83&&position[0]==-32&&position[1]==144&&layer==9&&head);++explosions;return true;
 },error));
 assert(end.enableLocalEnemyPasses(error)&&explosions==1&&end.sceneControl()[0]==7&&end.sceneControl()[1]==0);
 std::cout<<"actual416 ECL sound,269 animation and436 floating local rank selection passed\n";
}
