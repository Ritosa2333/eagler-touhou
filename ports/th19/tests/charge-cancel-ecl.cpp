#include "GameWorld.hpp"
#include "EclVm.hpp"
#include <fstream>
#include <cassert>
#include <iostream>
using namespace th19;
int main(int argc,char**argv){
 assert(argc==2);std::ifstream f(argv[1],std::ios::binary);std::vector<u8>b((std::istreambuf_iterator<char>(f)),{});EclProgram p;assert(p.load(b.data(),b.size()));
 for(int tier:{3,4}){
  GameWorld w;w.attach_program(p);w.spawn_enemy(tier==3?"Ecl_EtBreakLv3":"Ecl_EtBreakLv4",0,100,1,0,0);
  for(float x:{8.f,20.f,30.f,38.f,46.f,54.f,80.f}){auto&shot=w.spawn_bullet();shot.x=x;shot.y=100;shot.state=1;shot.hit_width=0;shot.cancel_lock=1;}
  for(int i=0;i<10;++i)w.tick_wave();
  assert(w.unhandled_opcodes.empty());assert(w.cancelled_bullets==(tier==3?3:5));
  assert(w.enemies.empty());std::cout<<"C"<<tier<<" original ECL cancelled "<<w.cancelled_bullets<<" PASS\n";
 }
}
