#pragma once
#include "GameWorld.hpp"
#include "EclVm.hpp"
#include "AnmVm.hpp"
namespace th19 {
// Two fields share ANM RNG; capture them atomically at a frame boundary.
// Immutable ECL/ANM resources are owned by the match and must not be unloaded.
class WorldCheckpoint {
 struct Field {
  GameWorld world;
  EclVm root;
  Field(const GameWorld&w,const EclVm&r):world(w),root(r){rebind();}
  void rebind(){root.restore_world(&world);for(auto&e:world.enemies)if(e.vm)e.vm->restore_world(&world);}
 };
 std::unique_ptr<Field> fields_[2];
 u32 random_,primary_random_;
public:
 WorldCheckpoint(const GameWorld&left,const EclVm&leftRoot,const GameWorld&right,const EclVm&rightRoot)
 :fields_{std::make_unique<Field>(left,leftRoot),std::make_unique<Field>(right,rightRoot)},random_(AnmVm::random_checkpoint()),primary_random_(AnmVm::random_checkpoint(true)){}
 void capture(const GameWorld&left,const EclVm&leftRoot,const GameWorld&right,const EclVm&rightRoot){
  const GameWorld* worlds[]{&left,&right};const EclVm* roots[]{&leftRoot,&rightRoot};
  for(int i=0;i<2;i++){fields_[i]->world=*worlds[i];fields_[i]->root=*roots[i];fields_[i]->rebind();}
  random_=AnmVm::random_checkpoint();
  primary_random_=AnmVm::random_checkpoint(true);
 }
 void restore(GameWorld&left,EclVm&leftRoot,GameWorld&right,EclVm&rightRoot)const{
  GameWorld* worlds[]{&left,&right};EclVm* roots[]{&leftRoot,&rightRoot};
  for(int i=0;i<2;i++){*worlds[i]=fields_[i]->world;*roots[i]=fields_[i]->root;
   roots[i]->restore_world(worlds[i]);for(auto&e:worlds[i]->enemies)if(e.vm)e.vm->restore_world(worlds[i]);}
  AnmVm::restore_random(random_);
  AnmVm::restore_random(primary_random_,true);
 }
};
}
