#include "GameWorld.hpp"
#include "GlobalRank.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
int main(int argc,char**argv){
 using namespace th19;
 for(int d=0;d<4;++d)for(bool vs:{false,true}){
  GlobalRank r;r.begin(d,vs,1);int start=d<2?0:d==2?30:50;
  assert(r.value==start);int period=r.interval;
  for(int i=1;i<period;++i)r.tick();assert(r.value==start);
  r.tick();assert(r.value==start+1);
  for(int i=period;i<10800;++i)r.tick();
  if(vs)assert(r.cap==(d==0?100:200)&&r.interval==240);
 }
 assert(argc>1);std::ifstream f(argv[1],std::ios::binary);
 std::vector<u8> bytes((std::istreambuf_iterator<char>(f)),{});
 EclProgram p;assert(p.load(bytes.data(),bytes.size()));
 for(int rank:{0,19,20,30,50})for(float y:{329.f,330.f}){
  std::unordered_map<i32,AnmScriptLoc> animations;
  GameWorld w;w.bullet_scripts=&animations;w.attach_program(p);w.spirit_rank=rank;w.spirit_rank_cap=100;
  auto&e=w.spawn_enemy("",0,y,2800,0,0);
  EclVm vm;vm.attach_program(p);vm.attach_world(&w);vm.host_id=e.id;vm.has_host=true;
  assert(vm.start("ExAttackFunc"));int total=0,first=-1;
  for(int t=0;t<800;++t){vm.step();assert(!vm.error());if(!w.bullets.empty()){if(first<0)first=t;total+=w.bullets.size();for(auto&b:w.bullets)assert(b.kind==12&&b.color==2);w.bullets.clear();}}
  std::cout<<"rank="<<rank<<" y="<<y<<" first="<<first<<" total="<<total<<"\n";
  assert(total==((rank>=20&&y<330)?30:0));if(total)assert(first==int(120.f-(rank/200.f)*60.f));assert(w.unhandled_opcodes.empty());
 }
 std::cout<<"Orin original ECL / rank checks PASS\n";
}
