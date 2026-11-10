#include "GameWorld.hpp"
#include "WorldCheckpoint.hpp"
#include "PlayerDamage.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
int main(int argc,char**argv){
 using namespace th19;
 GameWorld w;Enemy host;host.id=100;host.x=0;host.y=100;
 const int from[]={0,1,4,5,8,9,18,2},to[]={4,4,18,18,20,31,33,2};
 for(int i=0;i<8;i++){auto&b=w.spawn_bullet();b.kind=from[i];b.color=6;b.x=0;b.y=100;b.age=21;b.vx=1;b.vy=2;}
 w.strengthen_tsukasa_bullets(host);
 for(int i=0;i<8;i++){const auto&b=w.bullets[i];assert(b.kind==to[i]);assert(b.color==(i<2||i==7?6:1));assert(b.vx==1&&b.vy==2&&b.age==21);assert(b.last_tsukasa_id==100);if(i!=7)assert(b.hit_width==nativeBulletContactRadius(to[i]));}
 w.strengthen_tsukasa_bullets(host);assert(w.bullets[0].kind==4);
 host.id=101;w.strengthen_tsukasa_bullets(host);assert(w.bullets[0].kind==18);
 host.id=102;w.strengthen_tsukasa_bullets(host);assert(w.bullets[0].kind==33);
 for(int age:{20,21})for(float x:{31.99f,32.f}){
  GameWorld t;auto&b=t.spawn_bullet();b.kind=0;b.age=age;b.x=x;b.y=100;t.strengthen_tsukasa_bullets(host);
  assert(b.kind==(age==21&&x<32?4:0));
 }
 GameWorld right;EclVm root,rr;WorldCheckpoint checkpoint(w,root,right,rr);
 w.bullets[0].age=0;w.bullets[0].last_tsukasa_id=0;checkpoint.restore(w,root,right,rr);
 assert(w.bullets[0].age==21&&w.bullets[0].last_tsukasa_id==102);
 // The native callback runs through the regular world tick, not just the helper.
 GameWorld t;auto&e=t.spawn_enemy("",0,100,200,0,0);e.native_update_callback=2;
 auto&b=t.spawn_bullet();b.kind=4;b.x=0;b.y=100;b.age=20;b.scripted=true;
 t.tick_wave();assert(t.bullets[0].kind==18&&t.bullets[0].age==21);
 if(argc>1){
  std::ifstream f(argv[1],std::ios::binary);std::vector<u8>bytes((std::istreambuf_iterator<char>(f)),{});
  EclProgram p;assert(p.load(bytes.data(),bytes.size()));
  for(const char* name:{"ExAttack","ExAttack2"}){
   GameWorld a;a.attach_program(p);a.spawn_enemy(name,0,100,200,0,0);
   auto&shot=a.spawn_bullet();shot.kind=4;shot.age=21;shot.x=0;shot.y=100;
   a.tick_wave();assert(a.enemies[0].native_update_callback==2);
   assert(a.bullets[0].kind==18);assert(!a.unhandled_opcodes.count(624));
  }
 }
 std::cout<<"Tsukasa native morph boundaries, progression, world tick and rollback PASS\n";
}
