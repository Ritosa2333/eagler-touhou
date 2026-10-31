#include "../../src/game/EclInterpreter.hpp"
#include "../../src/game/GameState.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <regex>
using namespace th12;
static void command2(Enemy& e,int op,int a=0,int b=0){
 std::array<u8,24> bytes{};u16 size=24;std::memcpy(bytes.data()+6,&size,2);
 std::memcpy(bytes.data()+16,&a,4);std::memcpy(bytes.data()+20,&b,4);
 Context c(bytes.data());command(e,c,op);
}
int main(int argc,char** argv){
 assert(argc==2);std::ifstream file(argv[1]);std::string json((std::istreambuf_iterator<char>(file)),{});
 assert(json.find("99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417")!=std::string::npos);
 const std::regex row(R"rx("previous":\s*(-?\d+),\s*"velocity":\s*([-+.\deE]+),\s*"direction":\s*(-?\d+),\s*"scripts":\s*\[\s*(\d*)\s*\])rx");
 // Synthetic, sprite-free bank keeps this original-instruction comparison asset-independent.
 std::array<u8,64> bytes{};u32 version=7;std::memcpy(bytes.data(),&version,4);std::string error;
 for(int bank=1;bank<=8;++bank){assert(animationScene.registry.loadBank(bank,bytes.data(),bytes.size(),error));auto data=animationScene.registry.bank(bank);data->scripts.resize(32);}
 int count=0;
 for(std::sregex_iterator i(json.begin(),json.end(),row),end;i!=end;++i){
  const auto& r=*i;const int previous=std::stoi(r[1]),direction=std::stoi(r[3]);const float velocity=std::stof(r[2]);
  for(int bank=1;bank<=8;++bank){
   Enemy e;e.id=1;e.bank=bank;command2(e,262,0,7);assert(e.flags&0x80000u);
   assert(e.movementAnimation==7&&e.movementDirection==0);
   e.movementDirection=previous;e.animationVelocityX=velocity;
   auto* before=animationScene.find(enemyAnimationKey(1,0));assert(before);auto id=before->id;
   before->vm.control().x=4;before->vm.control().y=-3;before->vm.control().z=2;
   synchronizeEnemyMovementAnimation(e);
   auto* after=animationScene.find(enemyAnimationKey(1,0));assert(after);
   assert(e.movementDirection==direction);
   assert(after->vm.state().bank==bank);
   if(r[4].str().empty())assert(after->id==id);
   else{assert(after->id!=id);assert(after->vm.state().script==std::stoi(r[4]));assert(after->vm.state().layer==8);}
   assert(after->vm.state().x==4&&after->vm.state().y==-3&&after->vm.state().z==2);
   id=after->id;synchronizeEnemyMovementAnimation(e);assert(animationScene.find(enemyAnimationKey(1,0))->id==id);
   // An explicit play does not replace the movement base; 269/274 use +22c.
   command2(e,259,0,20);command2(e,269);assert(e.animation==12);
   command2(e,274,0,2);assert(e.animation==14);
   command2(e,276);assert(!(e.flags&0x80000u));assert(e.animation==7&&e.movementDirection==0);
   e.animationVelocityX=-1;id=animationScene.find(enemyAnimationKey(1,0))->id;
   synchronizeEnemyMovementAnimation(e);assert(animationScene.find(enemyAnimationKey(1,0))->id==id);
  }
  ++count;
 }
 assert(count==27);
 Enemy e;e.x=1;e.ax=1.029f;combinePosition(e);assert(e.animationVelocityX>.028f&&e.animationVelocityX<.030f);
 std::cout<<"PASS: 27 original motion-transition cases across 8 banks, offsets, command ownership and pre-floor velocity\n";
}
