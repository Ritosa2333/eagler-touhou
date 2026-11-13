#pragma once
#include "AnmVm.hpp"
#if PORT_TITLE == 14
namespace th11 {
// Original type-0 transition callbacks 40a530/40a680/40acf0.
struct TransitionType0Th14 {
 AnmVm embedded[5]{};u32 state=0,counter=0;AnmEnvironment* environment=nullptr;AnmResource* resource_owner=nullptr;bool failed=false;
 bool initialize(AnmVm& root,AnmResource& resource,AnmEnvironment& env){
  environment=&env;resource_owner=&resource;failed=false;root.layer=0;state=counter=0;
  for(u32 i=0;i<4;++i){
   if(!embedded[i].copy_script(resource,3+i,8,&env.rate)||embedded[i].update(env)<0)return false;
   embedded[i].position={320,240,0};
  }
  root.layer=29;
  return embedded[4].copy_script(resource,195,8,&env.rate)&&embedded[4].update(env)>=0;
 }
 int update(AnmEnvironment& env){
  if(failed)return -1;
  u32 finished=0;
  for(u32 i=0;i<4;++i){const int result=embedded[i].update(env);if(result<0)return -1;if(result)++finished;}
  if(finished==4)return 1;
  if(embedded[4].update(env)<0)return -1;
  ++counter;return 0;
 }
 bool interrupt(AnmVm& root,i32 label,AnmResource& resource,AnmEnvironment& env){
  if(label==1){for(u32 i=0;i<4;++i)if(!embedded[i].copy_script(resource,7+i,8,&env.rate)||embedded[i].update(env)<0)return false;}
  else if(label>=7&&label<=10){
   state=label-7;constexpr u32 layers[]={29,23,20,29};root.layer=layers[state];
   for(u32 i=0;i<4;++i)embedded[i].position={320,240,0};
  }
  return true;
 }
};
}
#endif
