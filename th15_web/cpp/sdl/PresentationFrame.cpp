#include "PresentationFrame.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>
namespace th15::sdl {
namespace {
bool same(Vec3 a,Vec3 b){return a.x==b.x&&a.y==b.y&&a.z==b.z;}
bool same(Vec2 a,Vec2 b){return a.x==b.x&&a.y==b.y;}
float mix(float a,float b,float alpha){return a+(b-a)*alpha;}
u32 color(u32 a,u32 b,float alpha,u32 mask){u32 out=b;for(u32 shift=0;shift<32;shift+=8)if(mask&(1u<<(shift/8))){const auto channel=u32(std::clamp(mix(float(a>>shift&255),float(b>>shift&255),alpha),0.f,255.f));out=(out&~(255u<<shift))|(channel<<shift);}return out;}
}
void PresentationFrame::begin(){
 if(!enabled){reset();return;}previous.swap(current);written=0;pending.clear();occurrences.clear();endpoints.clear();
 for(u32 i=0;i<previous.size();++i)for(u32 j=0;j<previous[i].samples.size();++j)endpoints.push_back({previous[i].samples[j].key,{i,j}});
 std::sort(endpoints.begin(),endpoints.end(),[](const auto& a,const auto& b){return a.first<b.first;});recording=true;ready=false;
}
PresentationFrame::Command& PresentationFrame::next(){
 if(written==current.size())current.emplace_back();auto& c=current[written++];c.clear=c.direct=c.camera=false;c.bytes.clear();c.samples.clear();c.rect.clear();return c;
}
void PresentationFrame::range(const AnmVm& vm,u32 first,u32 count,u32 instance){
 if(!recording)return;
 const auto& v=vm.visual;const auto& curves=vm.interpolators;instance^=vm.presentation_part<<16;
 Sample s;s.first=first;s.count=count;s.script=vm.source_script;s.sprite=v.sprite;s.age=vm.age.current;s.flags=v.flags;s.resource=uintptr_t(vm.sprite_resource?vm.sprite_resource:vm.resource);
 u64 generation=vm.presentation_generation;s.offset=offset;
 bool motion=vm.presentation_motion||(v.flags&0x8000);s.world=motion;
 u32 depth=0;for(auto* parent=vm.rotation_parent;parent&&depth++<64;parent=parent->rotation_parent){generation=(generation^parent->presentation_generation^uintptr_t(parent))*1099511628211ull;motion|=parent->presentation_motion||parent->interpolators.position.duration>0;s.world|=parent->presentation_motion;if(parent->interpolators.rotation.duration>0||!same(parent->visual.angular_velocity,{}))s.fields|=2;if(parent->interpolators.scale.duration>0||parent->interpolators.secondary_scale.duration>0||!same(parent->visual.scale_velocity,{}))s.fields|=4;if(parent->interpolators.color.duration>0)s.fields|=8;if(parent->interpolators.alpha.duration>0)s.fields|=16;if(parent==&vm)break;}
 if(vm.creation_parent&&vm.creation_parent!=vm.rotation_parent)generation=(generation^vm.creation_parent->presentation_generation^uintptr_t(vm.creation_parent))*1099511628211ull;
 const auto owner=std::make_tuple(uintptr_t(&vm),generation,instance);s.key={uintptr_t(&vm),generation,instance,occurrences[owner]++};
 if(motion||curves.position.duration>0)s.fields|=1;
 if(motion||curves.rotation.duration>0||curves.angle.duration>0||!same(v.angular_velocity,{}))s.fields|=2;
 if(motion||curves.scale.duration>0||curves.secondary_scale.duration>0||!same(v.scale_velocity,{}))s.fields|=4;
 if(curves.color.duration>0||curves.secondary_color.duration>0)s.fields|=8;
 if(curves.alpha.duration>0||curves.secondary_alpha.duration>0)s.fields|=16;
 if(curves.uv_x.duration>0||curves.uv_y.duration>0||!same(v.uv_velocity,{}))s.fields|=32;
 if(vm.source_script<0&&!s.fields)return;
 if(curves.uv_scale.duration>0)s.fields|=64;
 s.position={vm.variables.position.x+v.translation.x+v.child_anchor.x,vm.variables.position.y+v.translation.y+v.child_anchor.y,vm.variables.position.z+v.translation.z+v.child_anchor.z};s.rotation=vm.variables.rotation;s.scale={v.scale.x*v.secondary_scale.x,v.scale.y*v.secondary_scale.y};s.size=v.sprite_size;
 pending.push_back(s);
}
void PresentationFrame::draw(const touhou::sdl::State& state,touhou::graphics::Topology topology,u32 count,const void* bytes,u32 stride,bool direct){
 if(!recording)return;auto& c=next();c.state=state;c.topology=topology;c.count=count;c.stride=stride;c.direct=direct;c.camera=camera;
 const u32 length=touhou::graphics::vertex_count(topology,count)*stride;const auto* data=static_cast<const u8*>(bytes);c.bytes.assign(data,data+length);c.samples.swap(pending);pending.clear();
}
void PresentationFrame::clear(const touhou::sdl::State& state,u32 flags,u32 value,const i32* rect){
 if(!recording)return;auto& c=next();c.state=state;c.clear=true;c.flags=flags;c.color=value;if(rect)c.rect.assign(rect,rect+4);
}
bool PresentationFrame::present(touhou::sdl::Renderer& renderer,float alpha,bool frozen){
 if(!ready||!std::isfinite(alpha)||alpha<0||alpha>1)return false;
 if(negative_control)alpha=1;
 using namespace touhou::graphics;
 const auto saved=renderer.state;renderer.flush();sampled=0;last_alpha=alpha;
 for(const auto& c:current){renderer.state=c.state;
  if(c.clear){renderer.clear(c.flags,c.color,1,0,c.rect.empty()?nullptr:c.rect.data(),c.rect.empty()?0:1);continue;}
  const void* data=c.bytes.data();bool copied=false;
  for(const auto& s:c.samples){if(std::get<0>(s.key)==observed&&s.first*c.stride+4<=c.bytes.size())std::memcpy(&last_x,c.bytes.data()+s.first*c.stride,4);auto found=std::lower_bound(endpoints.begin(),endpoints.end(),s.key,[](const auto& a,const Key& key){return a.first<key;});if(found==endpoints.end()||found->first!=s.key)continue;const auto& old=previous[found->second.first];const auto& before=old.samples[found->second.second];const float weight=frozen&&(s.world||c.camera)?1:alpha;
   if(weight==1||s.script!=before.script||s.resource!=before.resource||s.age<before.age||(s.flags&0x3fe00003u)!=(before.flags&0x3fe00003u)||s.count!=before.count||c.stride!=old.stride||c.state.target!=old.state.target||c.state.texture!=old.state.texture||c.state.layout!=old.state.layout)continue;
   if(!same(s.size,before.size))continue;
   const u32 fields=s.fields|before.fields;if(!fields&&!c.camera)continue;
   const float dx=s.position.x-before.position.x,dy=s.position.y-before.position.y,dz=s.position.z-before.position.z;
   if(dx*dx+dy*dy+dz*dz>16384)continue;
   const u32 mode=s.flags>>25&31;const bool geometry=((fields&7)||(c.camera&&c.state.layout.screen&&(mode==4||mode==6)))&&s.scale.x*before.scale.x>=0&&s.scale.y*before.scale.y>=0&&((fields&1)||same(s.position,before.position))&&((fields&2)||same(s.rotation,before.rotation))&&((fields&4)||same(s.scale,before.scale));
   if(!copied){scratch=c.bytes;data=scratch.data();copied=true;}
   if((s.first+s.count)*c.stride>scratch.size()||(before.first+before.count)*old.stride>old.bytes.size())continue;
   for(u32 i=0;i<s.count;i++){u8* out=scratch.data()+(s.first+i)*c.stride;const u8* in=old.bytes.data()+(before.first+i)*old.stride;
    if(geometry)for(u32 j=0;j<3;j++){float a,b;std::memcpy(&a,in+j*4,4);std::memcpy(&b,out+j*4,4);const float previousOffset=j==0?before.offset.x:j==1?before.offset.y:0,currentOffset=j==0?s.offset.x:j==1?s.offset.y:0;b=mix(a-previousOffset,b-currentOffset,weight)+currentOffset;std::memcpy(out+j*4,&b,4);if(!i&&!j&&std::get<0>(s.key)==observed)last_x=b;}
    if(c.state.layout.diffuse!=VertexAttributes::absent){const u32 at=c.state.layout.diffuse;u32 a,b;std::memcpy(&a,in+at,4);std::memcpy(&b,out+at,4);b=color(a,b,weight,((fields&8)?7:0)|((fields&16)?8:0));std::memcpy(out+at,&b,4);}
    if((fields&96)&&s.sprite==before.sprite&&c.state.layout.uv!=VertexAttributes::absent)for(u32 j=0;j<2;j++){const u32 at=c.state.layout.uv+j*4;float a,b;std::memcpy(&a,in+at,4);std::memcpy(&b,out+at,4);float delta=b-a;if(fields&32){if(delta>.5f)delta-=1;else if(delta<-.5f)delta+=1;}b=a+delta*weight;std::memcpy(out+at,&b,4);}
   }
   if(!c.state.layout.screen&&c.samples.size()==1){renderer.state.pipeline.textureFactor=color(old.state.pipeline.textureFactor,c.state.pipeline.textureFactor,weight,((fields&8)?7:0)|((fields&16)?8:0));if((fields&96)&&s.sprite==before.sprite)for(u32 j=0;j<16;j++){float a=old.state.matrix[3][j],delta=c.state.matrix[3][j]-a;if((fields&32)&&(j==12||j==13)){if(delta>.5f)delta-=1;else if(delta<-.5f)delta+=1;}renderer.state.matrix[3][j]=a+delta*weight;}}
   if(!c.state.layout.screen&&c.samples.size()==1&&geometry){for(u32 j=0;j<16;j++)renderer.state.matrix[0][j]=mix(old.state.matrix[0][j],c.state.matrix[0][j],weight);}
   if(c.camera&&old.camera&&c.samples.size()==1&&(!frozen||!s.world)){for(u32 k=1;k<3;k++)for(u32 j=0;j<16;j++)if(std::abs(c.state.matrix[k][j]-old.state.matrix[k][j])<128)renderer.state.matrix[k][j]=mix(old.state.matrix[k][j],c.state.matrix[k][j],weight);}
   ++sampled;
  }
  if(c.direct)renderer.draw_batch(c.count,data,c.stride);else renderer.draw(c.topology,c.count,data,c.stride);
 }
 renderer.flush();renderer.state=saved;renderer.present(1);return true;
}
}

namespace th15::sdl {
std::array<float,5> PresentationFrame::reference(uintptr_t identity){
 observed=identity;std::array<float,5> result{};
 for(const auto& c:current)for(const auto& s:c.samples)if(std::get<0>(s.key)==identity&&s.first*c.stride+4<=c.bytes.size()){
  std::memcpy(&result[1],c.bytes.data()+s.first*c.stride,4);result[2]=last_x;result[3]=float(std::get<1>(s.key));
  for(const auto& old:previous)for(const auto& before:old.samples)if(before.key==s.key&&before.first*old.stride+4<=old.bytes.size()){std::memcpy(&result[0],old.bytes.data()+before.first*old.stride,4);result[4]=1;return result;}
 }return result;
}
}
