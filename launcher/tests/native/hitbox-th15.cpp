#include "AnmManager.hpp"
#include "HitboxDisplay.hpp"
#include "AnmCoordinates.hpp"
#include <fstream>
#include <iterator>
#include <iostream>
#include <cassert>
#include <cmath>
using namespace th15;
void eq(float a,float b,const char* field,int frame,int side){if(std::abs(a-b)>0.00001f){std::cerr<<field<<" frame="<<frame<<" part="<<side<<" expected="<<a<<" actual="<<b<<"\n";std::abort();}}
void compare(AnmVm native,AnmVm visual,int f,int part){
 eq(native.variables.rotation.z,visual.variables.rotation.z,"rotation",f,part);
 eq(native.visual.scale.x,visual.visual.scale.x,"scale.x",f,part);eq(native.visual.scale.y,visual.visual.scale.y,"scale.y",f,part);
 assert(native.visual.color==visual.visual.color);assert(native.sprite==visual.sprite);
 Vec3 a[4],b[4];assert(anm_quad_positions(native,a)&&anm_quad_positions(visual,b));
 for(int i=0;i<4;i++){eq(a[i].x,b[i].x,"vertex.x",f,part);eq(a[i].y,b[i].y,"vertex.y",f,part);}
}
int main(int argc,char** argv){assert(argc==2);std::ifstream in(argv[1],std::ios::binary);std::vector<u8> bytes((std::istreambuf_iterator<char>(in)),{});assert(!bytes.empty());
 for(float rate:{1.f,.5f})for(float scale:{1.f,2.f}){
  Rng random;AnmEnvironment env;env.screen_offsets={0,0,320,16};AnmResource fallback;fallback.sprites.resize(259);fallback.textures.resize(1);fallback.textures[0].width=1;fallback.textures[0].height=1;env.fallback_sprite_resource=&fallback;AnmManager manager(random,env);
  if(!manager.load(0,bytes.data(),bytes.size())){std::cerr<<manager.error;return 1;}
  manager.rate=rate;const auto handle=manager.create(0,26,14,0);assert(handle);
  auto* root=manager.registry.find(handle);auto* child=manager.registry.find_child_script(*root,25,0);assert(child);
  presentation::HitboxDisplay display;float time=0;
  for(int f=0;f<180;f++){
   env.screen_offsets[2]=f>=90?352:320;
   const Vec3 p{float(f)/10,300,0};root->visual.translation=p;root->visual.secondary_scale={scale,scale};root->visual.flags|=8;
   auto before=random;auto gamebefore=env.game_rng;const auto count=manager.registry.count();
   auto& markers=display.sample(*manager.script_template(0,25),*manager.script_template(0,26),p,scale,time,1,rate);
   assert(random.seed==before.seed&&random.calls==before.calls&&env.game_rng.calls==gamebefore.calls&&manager.registry.count()==count);
   compare(*child,markers[0],f,0);compare(*root,markers[1],f,1);
   auto rotation=markers[0].variables.rotation.z;display.sample(*manager.script_template(0,25),*manager.script_template(0,26),p,scale,time,1,rate);assert(markers[0].variables.rotation.z==rotation);
   assert(manager.update(false));time+=rate;
  }
 }
 std::cout<<"TH15 stock effect scripts: 720 frame pairs, matching quads/scale/rotation/alpha, no shared RNG or registry writes\n";
}
