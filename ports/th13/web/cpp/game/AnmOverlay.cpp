#include "AnmOverlay.hpp"
namespace th13 {
AnmOverlay::AnmOverlay(const AnmOverlay& source):mode(source.mode),frames(source.frames){for(u32 i=0;i<panels.size();i++)if(source.panels[i])panels[i]=std::make_unique<AnmVm>(*source.panels[i]);}
bool AnmOverlay::initialize(AnmResource& effect,AnmResource& astral,AnmVm& owner,GameRandom& random){
 for(u32 i=0;i<5;++i){
  panels[i]=std::make_unique<AnmVm>();auto& vm=*panels[i];
  vm.environment=owner.environment;vm.object_host=owner.object_host;
  if(!vm.bind(i<4?effect:astral,i<4?i+3:0)||vm.tick(random,1)<0)return false;
  if(i<4)vm.visual.translation={320,240,0};
 }
 return true;
}
i32 AnmOverlay::tick(GameRandom& random,float rate){
 i32 finished=0;
 for(u32 i=0;i<4;++i){
  if(panels[i]->instruction_offset<0){++finished;continue;}
  const i32 result=panels[i]->tick(random,rate);if(result<0)return -1;finished+=result!=0;
 }
 if(finished==4)return 1;
 if(panels[4]->tick(random,rate)<0)return -1;
 ++frames;return 0;
}
bool AnmOverlay::interrupt(i32 label,GameRandom& random,float rate){
 if(label==1){ //4035fe replaces the four scripts with7..10, then ticks once.
  for(u32 i=0;i<4;++i){auto& vm=*panels[i];auto* resource=vm.resource;
   if(!resource||!vm.bind(*resource,i+7)||vm.tick(random,rate)<0)return false;
  }
 }else if(label>=7&&label<=9){
  mode=label-7;for(u32 i=0;i<4;++i)panels[i]->visual.translation={320,240,0};
 }
 return true;
}
}
