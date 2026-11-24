#include "game/ItemHost.hpp"
#include "game/PerfectHud.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <vector>
struct Sound{int identifier;float position;};
struct Popup{float x,y,z,value,age;};
static std::vector<Sound> grazeSounds;
static std::vector<Popup> scorePopups;
static int motivationRaw=0,motivationFlag=0,scoreRaw=0,pointValueRaw=100000;
static bool playerFireConfigured=true,perfectHudConfigured=true;
static std::unique_ptr<th128::hud::PerfectHud> perfectHud;
struct WorldSeam{th128::item::Position playerAimPosition(){return {24,400,0};}};
static WorldSeam worldSeam;static auto* world=&worldSeam;
#include "collect-callback.inc"
static unsigned assertions=0,cases=0;
static void require(bool condition,const char* message){++assertions;if(!condition)throw std::runtime_error(message);}
static unsigned scriptCount(int script){const auto s=perfectHud->snapshots();return std::count_if(s.begin(),s.end(),[&](const auto& row){return row.state.script==script;});}
static std::vector<std::uint8_t> read(const char* path){std::ifstream f(path,std::ios::binary);return {std::istreambuf_iterator<char>(f),{}};}
int main(int argc,char** argv){try{
 require(argc==3,"full original front/bullet ANM required");const auto front=read(argv[1]),bullet=read(argv[2]);
 require(!front.empty()&&!bullet.empty(),"original assets not empty");std::string error;
 perfectHud=std::make_unique<th128::hud::PerfectHud>();require(perfectHud->load(front,error),error.c_str());require(perfectHud->boot(0,error),error.c_str());
 auto collect=[&](int value,int flag,int initialScore,std::size_t journalSize=0){
  motivationRaw=value;motivationFlag=flag;scoreRaw=initialScore;grazeSounds.assign(journalSize,{99,0});error.clear();
  th128::item::ItemHost items;require(items.load(bullet,error),error.c_str());require(items.enableIceItems(5,error),error.c_str());
  unsigned actualCollections=0;require(items.setCollectHost([&](int kind,th128::item::Position p,int state,std::string& e){
   ++actualCollections;return actualCollect(kind,p,state,e);}),"real collect callback attached");
  require(items.spawn(9,{24,400,0},0,0,error),error.c_str());
  bool success=true;for(unsigned tick=0;tick<6&&actualCollections==0;++tick){success=items.tick({24,400,0},1,false,error);if(!success)break;}
  require(actualCollections==1,"exactly one actual item contact reaches production callback");++cases;
  if(success)require(items.snapshots().empty(),"collected original ANM item released");return success;
 };
 require(collect(29999,0,100),error.c_str());
 require(motivationRaw==30000&&scoreRaw==101,"threshold motivation and original score increment");
 require(grazeSounds.size()==2&&grazeSounds[0].identifier==18&&grazeSounds[0].position==0
  &&grazeSounds[1].identifier==38&&std::abs(grazeSounds[1].position-24)<.001f,"original notice sound18 precedes positioned collection sound38");
 require(scriptCount(49)==1&&scriptCount(47)==0,"item threshold uses original front49");
 require(perfectHud->notice(error),error.c_str());require(scriptCount(47)==1&&scriptCount(49)==0,"charge notification owns shared original slot");
 require(collect(39999,0,10),error.c_str());require(scriptCount(49)==1&&scriptCount(47)==0,"item notice replaces charge47 at shared handle");
 require(collect(49999,0,20),error.c_str());require(scriptCount(49)==1,"repeated threshold does not duplicate notice");
 require(collect(99999,0,30),error.c_str());require(motivationRaw==100000&&grazeSounds.size()==2,"threshold before100000 cap");
 require(collect(100000,0,40),error.c_str());require(motivationRaw==100000&&grazeSounds.size()==1&&grazeSounds[0].identifier==38,"saturation does not retrigger threshold");
 require(collect(29998,0,50),error.c_str());require(motivationRaw==29999&&grazeSounds.size()==1,"below threshold only collection sound");
 require(collect(29999,1,60),error.c_str());require(motivationRaw==29999&&scoreRaw==61&&grazeSounds.size()==1,"motivation disable flag preserves collection reward");
 require(collect(50000,0,999999999),error.c_str());require(scoreRaw==999999999,"score saturation preserved");
 perfectHudConfigured=false;require(!collect(29999,0,80),"missing real HUD owner must reject");require(grazeSounds.empty()&&scoreRaw==80,"missing HUD cannot issue receipt or final collection reward");
 perfectHudConfigured=true;require(!collect(29999,0,90,65535),"both required sounds must fit journal");require(grazeSounds.size()==65535&&scoreRaw==90,"journal budget rejects before partial audio batch");
 playerFireConfigured=false;require(!collect(100,0,100),"unbound player/ice context remains rejected");require(grazeSounds.empty()&&scoreRaw==100,"unbound context has no reward");playerFireConfigured=true;
 for(unsigned tick=0;tick<60;++tick)require(perfectHud->tick(error),error.c_str());
 std::cout<<"{\"passed\":true,\"actualItemCollectionCases\":"<<cases<<",\"checks\":"<<assertions
  <<",\"hudTicks\":60,\"wholeProductionCollectCallback\":true,\"originalANM\":true,\"WorldStarts\":0,\"syntheticCallerState\":true}"<<'\n';
 return 0;
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
