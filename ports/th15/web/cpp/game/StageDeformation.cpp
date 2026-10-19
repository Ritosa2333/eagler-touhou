#include "StageDeformation.hpp"
#include "AnmVisualState.hpp"
#include <cmath>
namespace th15 {
namespace {
Vec3 normalized(Vec3 v){const float sq=float(float(float(v.x*v.x)+float(v.y*v.y))+float(v.z*v.z));if(sq<0x1p-46f)return {};const float inv=float(1.f/float(std::sqrt(double(sq)))),fix=float(3.f-float(float(sq*inv)*inv)),scale=float(float(.5f*inv)*fix);return {float(v.x*scale),float(v.y*scale),float(v.z*scale)};}
}
bool StageDeformation::begin(i32 mode){if(!enabled)return mesh.retire(animations);if(!mesh.initialize(animations,text_bank,mode==1?7:17)){error=animations.error;return false;}return true;}
bool StageDeformation::upload(){for(u32 i=0;i<mesh.strip_handles.size();i++){auto* vm=animations.registry.find(mesh.strip_handles[i]);if(!vm||!mesh.grid.strip(i,vm->geometry.screen_vertices)){error="Background deformation strip unavailable";return false;}}return true;}
bool StageDeformation::update(StageScriptState& state,bool spell_active){
 if(!enabled||!mesh.grid.column_count())return true;auto& grid=mesh.grid;float xphase=state.phase_x,yphase=state.phase_y;const u32 columns=grid.column_count(),rows=grid.row_count();
 if(state.deformation_mode==1){if(spell_active)return true;if(!grid.place(viewport,{-192,0},384,128))return false;if(!upload())return false;
  for(u32 column=0;column<columns;column++){const float ysine=float(std::sin(double(yphase)));for(u32 row=0;row<rows;row++){const u32 index=column*rows+row;auto& v=grid.vertices[index];v.color=(v.color&0xffffffu)|0xc0000000;const float strength=float(24.f-float(float(row)*24.f)/float(rows-1));const float x=float(float(std::sin(double(xphase)))*strength),y=float(ysine*strength);if(column&&row&&column!=columns-1&&row!=rows-1){v.position.x=float(v.position.x+x);v.position.y=float(v.position.y+y);v.position.z=grid.sampling[index].z=0;}xphase=normalize_angle(float(xphase+.6684240102767944f));}yphase=normalize_angle(float(yphase-1.4959965944290161f));}
  state.phase_x=normalize_angle(float(state.phase_x+.04908738657832146f));state.phase_y=normalize_angle(float(state.phase_y+.039269909262657166f));return true;
 }
 if(state.deformation_mode==2){const float radius=state.deformation_radius;if(radius>state.deformation_target)state.deformation_radius=float(radius-2.f);if(!grid.place(viewport,{-radius,float(224.f-radius)},float(radius*2.f),float(radius*2.f)))return false;if(!upload())return false;const float squared=float(radius*radius);
  for(u32 index=0;index<grid.vertices.size();index++){auto& v=grid.vertices[index];auto& sample=grid.sampling[index];Vec3 delta{float(sample.x-224.f),float(sample.y-240.f),0};const float distance=float(float(delta.y*delta.y)+float(delta.x*delta.x)),difference=float(squared-distance);
   if(difference>=0){const float weight=float(difference/squared);v.color=0x60ffffff;const float strength=float(weight*32.f);delta=normalized(delta);const float x=float(float(strength*delta.x)+float(float(float(std::sin(double(xphase)))*weight)*8.f)),y=float(float(strength*delta.y)+float(float(float(std::sin(double(yphase)))*weight)*8.f));v.position.x=float(v.position.x+x);v.position.y=float(v.position.y+y);v.position.z=sample.z=0;}else v.color&=0xffffffu;
   xphase=normalize_angle(float(xphase+1.5707963705062866f));yphase=normalize_angle(float(yphase-.6981317400932312f));
  }
  state.phase_x=normalize_angle(float(state.phase_x+.04908738657832146f));const float random=float(float(visual.unit()*3.1415927410125732f)/40.f),delta=float(random+.039269909262657166f);state.phase_y=normalize_angle(float(state.phase_y+delta));return true;
 }
 return true;
}
}
