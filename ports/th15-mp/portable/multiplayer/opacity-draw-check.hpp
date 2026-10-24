#pragma once
#include "../../web/cpp/game/AnmOverlay.hpp"
#include <map>
#include <set>
#include <type_traits>

// Compare the production opacity entry point with the former full-copy recipe.
// This records CPU graphics submissions; the browser oracle remains responsible
// for actual GPU pixels, interpolation playback and journal restore.
namespace opacity_draw_check {
using namespace touhou::graphics;
using Bytes=std::vector<u8>;
template<class T>void put(Bytes& out,const T& value){
 static_assert(std::is_trivially_copyable_v<T>);
 const auto* p=reinterpret_cast<const u8*>(&value);out.insert(out.end(),p,p+sizeof value);
}
template<class... T>void fields(Bytes& out,const T&... value){(put(out,value),...);}
inline void raw(Bytes& out,const void* data,std::size_t length){
 if(length){const auto* p=static_cast<const u8*>(data);out.insert(out.end(),p,p+length);}
}
inline void pipeline_bytes(Bytes& out,const PipelineState& p){
 fields(out,p.depthTest,p.depthWrite,p.blend,p.alphaTest,p.fog,p.rangeFog,p.dither,
  p.depthCompare,p.alphaCompare,p.sourceBlend,p.destinationBlend,p.blendEquation,
  p.separateAlphaBlend,p.sourceAlphaBlend,p.destinationAlphaBlend,p.alphaBlendEquation,
  p.cull,p.colorMask,p.alphaReference,p.fogColor,p.textureFactor,p.fogMode,
  p.fogNear,p.fogFar,p.fogDensity,p.textureTransform,p.minFilter,p.magFilter,p.addressU,p.addressV);
 for(const auto* c:{&p.color,&p.alpha})fields(out,c->operation,c->first.source,c->first.complement,
  c->first.alphaOnly,c->second.source,c->second.complement,c->second.alphaOnly);
}
struct Event {
 std::string kind;Bytes arguments,state,vertices;std::uintptr_t owner=0;
 u64 generation=0;VertexLayout layout=VertexLayout::ScreenColorUv;
};
struct RecordingGraphics final:ZunGraphics {
 PipelineState state;std::array<Matrix4,4> matrices;VertexLayout layout=VertexLayout::ScreenColorUv;
 u32 bound=0,target=0;GraphicsViewport viewport;Vec2 offset{};bool camera=false;
 std::vector<Event> events;
 RecordingGraphics(){for(auto& m:matrices)m.identity();configure_game(1);}
 PipelineState& pipeline()override{return state;}
 Bytes state_bytes()const{
  Bytes out;pipeline_bytes(out,state);for(const auto& m:matrices)put(out,m.m);
  fields(out,layout,bound,target,viewport.x,viewport.y,viewport.width,viewport.height,
   viewport.near_depth,viewport.far_depth,offset.x,offset.y,camera);return out;
 }
 Event& event(const char* name){events.push_back({});auto& e=events.back();e.kind=name;e.state=state_bytes();e.layout=layout;return e;}
 u32 texture(const AnmResource& resource,u32 index)override{
  auto& e=event("texture");fields(e.arguments,std::uintptr_t(&resource),index);return index+1;
 }
 void bind_texture(u32 value)override{bound=value;put(event("bind").arguments,value);}
 void set_layout(VertexLayout value)override{layout=value;put(event("layout").arguments,value);}
 void set_matrix(MatrixKind kind,const Matrix4& value)override{
  matrices[unsigned(kind)]=value;fields(event("matrix").arguments,kind,value.m);
 }
 void triangles(u32 count,const void* data,u32 stride)override{draw("triangles",Topology::Triangles,count,data,stride);}
 void primitives(Topology kind,u32 count,const void* data,u32 stride)override{draw("primitives",kind,count,data,stride);}
 void draw(const char* label,Topology kind,u32 count,const void* data,u32 stride){
  auto& e=event(label);fields(e.arguments,kind,count,stride);raw(e.vertices,data,std::size_t(vertex_count(kind,count))*stride);
 }
 bool select_target(const AnmResource* resource,u32 value)override{
  target=value;fields(event("target").arguments,std::uintptr_t(resource),value);return true;
 }
 bool clear_target(u32 color,const GraphicsViewport* rect)override{
  auto& e=event("clear");fields(e.arguments,color,bool(rect));
  if(rect)fields(e.arguments,rect->x,rect->y,rect->width,rect->height,rect->near_depth,rect->far_depth);return true;
 }
 bool clear_depth(const GraphicsViewport* rect)override{
  auto& e=event("depth");put(e.arguments,bool(rect));
  if(rect)fields(e.arguments,rect->x,rect->y,rect->width,rect->height,rect->near_depth,rect->far_depth);return true;
 }
 void set_viewport(const GraphicsViewport& v)override{viewport=v;event("viewport");}
 void presentation_offset(Vec2 value)override{offset=value;event("offset");}
 void presentation_camera(bool value)override{camera=value;event("camera");}
 void presentation_range(const AnmVm& vm,u32 first,u32 count,u32 instance)override{
  auto& e=event("range");e.owner=std::uintptr_t(&vm);e.generation=vm.presentation_generation;
  const auto& v=vm.visual;const auto& c=vm.interpolators;
  fields(e.arguments,first,count,instance,vm.presentation_generation,vm.presentation_part,vm.presentation_motion,
   vm.source_script,vm.age.current,v.sprite,v.flags,v.render_flags,
   std::uintptr_t(vm.resource),std::uintptr_t(vm.sprite_resource),std::uintptr_t(vm.sprite),
   std::uintptr_t(vm.rotation_parent),std::uintptr_t(vm.creation_parent),
   vm.variables.position,vm.variables.rotation,v.translation,v.child_anchor,v.scale,v.secondary_scale,
   v.sprite_size,v.angular_velocity,v.scale_velocity,v.uv_velocity,v.color,v.secondary_color,v.inherited_color,
   v.quad,v.sprite_matrix.m,v.transform_matrix.m,v.uv_matrix.m,
   c.position.duration,c.rotation.duration,c.angle.duration,c.scale.duration,c.secondary_scale.duration,
   c.color.duration,c.secondary_color.duration,c.alpha.duration,c.secondary_alpha.duration,
   c.uv_x.duration,c.uv_y.duration,c.uv_scale.duration);
  u32 depth=0;
  for(auto* p=vm.rotation_parent;p&&depth++<64;p=p->rotation_parent){
   fields(e.arguments,std::uintptr_t(p),p->presentation_generation,p->presentation_motion,
    p->interpolators.position.duration,p->interpolators.rotation.duration,
    p->interpolators.scale.duration,p->interpolators.secondary_scale.duration,
    p->interpolators.color.duration,p->interpolators.alpha.duration,p->visual.angular_velocity,p->visual.scale_velocity,
    p==&vm);
   if(p==&vm)break;
  }
  put(e.arguments,depth);
 }
};
inline Bytes vm_image(const AnmVm& vm){
 Bytes out;put(out,std::uintptr_t(&vm));raw(out,&vm,sizeof vm);
 const auto& g=vm.geometry;
 raw(out,g.screen_vertices.data(),g.screen_vertices.size()*sizeof(AnmGeometryVertex));
 raw(out,g.world_vertices.data(),g.world_vertices.size()*sizeof(AnmWorldVertex));
 if(g.distortion)put(out,*g.distortion);if(g.trail)put(out,*g.trail);if(g.gather)put(out,*g.gather);
 if(g.overlay)raw(out,g.overlay.get(),sizeof(AnmOverlay));
 if(vm.environment)raw(out,vm.environment,sizeof(AnmEnvironment));return out;
}
struct SourceGraph {
 std::vector<AnmVm*> nodes;std::vector<Vec3> rotations;
 explicit SourceGraph(AnmVm& root){add(&root);for(auto* vm:nodes)rotations.push_back(vm->variables.rotation);}
 void add(AnmVm* vm){
  if(!vm||std::find(nodes.begin(),nodes.end(),vm)!=nodes.end())return;nodes.push_back(vm);
  if(vm->geometry.overlay)for(auto& p:vm->geometry.overlay->panels)add(p.get());
  add(vm->rotation_parent);add(vm->creation_parent);
 }
 std::vector<Bytes> image()const{std::vector<Bytes> out;for(auto* vm:nodes)out.push_back(vm_image(*vm));return out;}
 void reset_rotations(){for(std::size_t i=0;i<nodes.size();++i)nodes[i]->variables.rotation=rotations[i];}
 bool owns(std::uintptr_t address)const{return std::any_of(nodes.begin(),nodes.end(),[&](auto* vm){return std::uintptr_t(vm)==address;});}
};
struct Settings {bool tint=false;u32 tint_color=0x80808080;bool fog=false;bool overlay_traps=false;unsigned repeats=2;};
// Deliberately independent of RenderCopyTag and the new draw_body/scratch path.
__attribute__((noinline)) inline int old_full_copy(AnmRenderer& renderer,const AnmVm& source,u32 opacity){
 auto copy=source;
 const auto fade=[&](u32 color){return (color&0xffffffu)|(((color>>24)*opacity/255)<<24);};
 copy.visual.color=fade(copy.visual.color);copy.visual.secondary_color=fade(copy.visual.secondary_color);
 if(copy.visual.blend_mode()!=1)copy.visual.flags&=~0x1e0u;
 for(auto& vertex:copy.geometry.screen_vertices)vertex.color=fade(vertex.color);
 for(auto& vertex:copy.geometry.world_vertices)vertex.color=fade(vertex.color);
 return renderer.draw(copy);
}
struct Run {RecordingGraphics graphics;std::vector<int> results;std::vector<std::string> errors;};
inline Run run(AnmVm& source,u32 opacity,const Settings& config,bool reference){
 Run out;AnmRenderer renderer(out.graphics);renderer.set_viewport({0,0,640,480,0,1});
 renderer.offset={.25f,-.125f};renderer.tint_enabled=config.tint;renderer.tint=config.tint_color;
 AnmCamera camera;camera.reference={.001f,0,0};
 if(config.fog){camera.eye={0,0,-1};camera.fog_near=1;camera.fog_far=3;camera.fog_rgb={31,79,131};}
 renderer.set_camera(camera);renderer.presentation_instance=7;
 renderer.multiplayer_player_opacity[&source]=u16(opacity);
 if(config.overlay_traps&&source.geometry.overlay)for(auto& panel:source.geometry.overlay->panels){
  renderer.multiplayer_player_opacity[panel.get()]=0;renderer.multiplayer_hidden_hud.push_back(panel.get());
 }
 const auto generation=AnmVm::presentation_counter;
 for(unsigned n=0;n<config.repeats;++n){
  out.results.push_back(reference?old_full_copy(renderer,source,opacity):renderer.draw(source));
  out.errors.push_back(renderer.error);renderer.flush();
 }
 require(AnmVm::presentation_counter==generation,"Opacity Draw advanced presentation generation");
 out.graphics.event("final");return out;
}
struct Totals {unsigned cases=0,draws=0,events=0,submissions=0,identities=0,parent_changes=0,independent_alpha_checks=0;} ;
inline const Event& submission(const Run& r,unsigned n=0){
 for(const auto& e:r.graphics.events)if(!e.vertices.empty()){if(n--==0)return e;}
 require(false,"Opacity oracle expected an actual vertex submission");std::abort();
}
inline u32 vertex_color(const Event& e,unsigned index=0){
 const auto offset=attributes(e.layout).diffuse;require(offset!=VertexAttributes::absent,"Opacity oracle needs diffuse color");
 const auto step=touhou::graphics::stride(e.layout);require((index+1)*step<=e.vertices.size(),"Opacity oracle vertex out of range");
 u32 value;std::memcpy(&value,e.vertices.data()+index*step+offset,sizeof value);return value;
}
inline Run compare(const std::string& label,AnmVm& source,u32 opacity,Totals& total,Settings config={}){
 const auto prefix="Opacity "+label+" alpha="+std::to_string(opacity)+": ";
 SourceGraph graph(source);const auto before=graph.image();const auto expected=run(source,opacity,config,true);
 const auto after_reference=graph.image();if(after_reference!=before)++total.parent_changes;
 graph.reset_rotations();require(graph.image()==before,prefix+"legacy recipe changed more than original-parent rotation");
 auto actual=run(source,opacity,config,false);
 require(graph.image()==after_reference,prefix+"source VM/geometry/parent/RNG post-state differs");
 require(expected.results==actual.results&&expected.errors==actual.errors,prefix+"return/error differs");
 const auto& a=expected.graphics.events;const auto& b=actual.graphics.events;
 require(a.size()==b.size(),prefix+"graphics boundary count differs");
 std::map<std::uintptr_t,std::uintptr_t> owners,reverse;
 for(std::size_t i=0;i<a.size();++i){
  const auto where=prefix+"event "+std::to_string(i)+" ("+a[i].kind+") ";
  require(a[i].kind==b[i].kind&&a[i].arguments==b[i].arguments&&a[i].state==b[i].state&&a[i].vertices==b[i].vertices,
   where+"topology/vertices/pipeline/matrices/presentation fields differ");
  require(bool(a[i].owner)==bool(b[i].owner),where+"presentation owner presence differs");
  if(a[i].owner){
   require(!graph.owns(a[i].owner)&&!graph.owns(b[i].owner),where+"opacity presentation owner escaped to original VM");
   const auto left=owners.emplace(a[i].owner,b[i].owner),right=reverse.emplace(b[i].owner,a[i].owner);
   require(left.first->second==b[i].owner&&right.first->second==a[i].owner,where+"presentation owner alias/reuse differs");
   ++total.identities;
  }
  if(!a[i].vertices.empty())++total.submissions;
 }
 total.events+=unsigned(a.size());total.draws+=config.repeats;++total.cases;
 graph.reset_rotations();require(graph.image()==before,prefix+"failed to restore comparison-only parent setup");
 return actual;
}
inline void geometry(AnmVm& vm){
 vm.geometry.screen_vertices.resize(10);vm.geometry.world_vertices.resize(10);vm.geometry.allocation_bytes=560;
 for(unsigned i=0;i<10;++i){
  const u32 color=((5u+i*23u)<<24)|(0x010307u*i);
  vm.geometry.screen_vertices[i]={{float(100+i*3),float(160+i%2*9),.25f},1,color,{float(i)*.07f,float(i%2)}};
  vm.geometry.world_vertices[i]={{float(i)*.1f,float(i%2)*.2f,.25f},color,{float(i)*.07f,float(i%2)}};
 }
 vm.variables.integers[0]=3;
}
inline AnmVm synthetic(const AnmVm& retail,unsigned mode){
 AnmVm vm=retail;vm.geometry.clear();vm.rotation_parent=vm.creation_parent=nullptr;
 vm.variables={};vm.variables.position={160,210,.5f};vm.variables.rotation={0,0,.125f};
 vm.visual={};auto& v=vm.visual;v.flags=3u|(mode<<25)|(3u<<5);v.render_flags=0;
 v.sprite=0;v.sprite_size={16,12};v.color=0xffe1b39d;v.secondary_color=0x7f3579bd;v.inherited_color=0x13579bdf;
 v.sprite_matrix.identity();v.transform_matrix.identity();v.uv_matrix.identity();
 v.uv[0]={0,0};v.uv[1]={1,0};v.uv[2]={0,1};v.uv[3]={1,1};v.quad.fill({-13,-17,-19});
 vm.presentation_motion=true;vm.presentation_part=3;vm.age.set(7);
 vm.interpolators.position.duration=11;vm.interpolators.alpha.duration=9;vm.interpolators.color.duration=13;
 geometry(vm);return vm;
}
inline void identity_transition(const AnmVm& retail,Totals& total){
 auto source=synthetic(retail,0);RecordingGraphics graphics;AnmRenderer renderer(graphics);
 renderer.set_viewport({0,0,640,480,0,1});std::vector<std::uintptr_t> identities;
 const auto counter=AnmVm::presentation_counter;
 for(int alpha:{72,-1,160}){
  if(alpha<0)renderer.multiplayer_player_opacity.erase(&source);else renderer.multiplayer_player_opacity[&source]=u16(alpha);
  const auto begin=graphics.events.size();require(renderer.draw(source)==0,"Opacity view identity draw failed");renderer.flush();
  for(std::size_t i=begin;i<graphics.events.size();++i)if(graphics.events[i].owner)identities.push_back(graphics.events[i].owner);
 }
 require(identities.size()==3&&identities[0]!=std::uintptr_t(&source)&&identities[1]==std::uintptr_t(&source)&&
  identities[2]==identities[0],"Opacity/view change collapsed temporary and simulation presentation identities");
 require(AnmVm::presentation_counter==counter,"Opacity view change advanced generation");
 ++total.cases;total.draws+=3;total.identities+=3;total.events+=unsigned(graphics.events.size());
}
inline void check(AnmVm& retail){
 Totals total;constexpr u32 opacities[]={0,1,72,128,160,254,255};
 for(u32 alpha:opacities){
  auto native=retail;compare("retail-root",native,alpha,total);
  for(unsigned mode:{0u,1u,3u,4u,6u,8u,15u,17u,21u,22u,24u,25u,9u,11u,14u}){
   auto vm=synthetic(retail,mode);Settings cfg;
   if(mode==4||mode==6){vm.variables.position={0,0,.5f};cfg.fog=mode==6;}
   if(mode==17)vm.variables.integers[0]=5;
   if(mode==21||mode==22){vm.visual.color=0x03abcdef;vm.visual.secondary_color=0x073579bd;}
   auto actual=compare("mode-"+std::to_string(mode),vm,alpha,total,cfg);
   if(mode==0&&alpha==128){require(actual.graphics.state.sourceBlend==BlendFactor::SourceAlpha&&
    actual.graphics.state.destinationBlend==BlendFactor::InverseSourceAlpha,"Nonadditive opacity blend was not normalized");++total.independent_alpha_checks;}
   if(mode==6&&alpha==128){require((vertex_color(submission(actual))>>24)==126,"Fog did not preserve fade-before-cubic-alpha order");++total.independent_alpha_checks;}
   if((mode==21||mode==22)&&alpha==254){
    require((vertex_color(submission(actual,0))>>24)==1&&(vertex_color(submission(actual,1))>>24)==2,
     "Shape outline did not preserve integer fade-before-half order");++total.independent_alpha_checks;
   }
  }
  {
   auto parent=synthetic(retail,0);parent.variables.position={320,240,.5f};parent.visual.inherited_color=0xffffffff;parent.variables.rotation={7,-8,9};
   auto vm=synthetic(retail,1);vm.rotation_parent=vm.creation_parent=&parent;vm.variables.rotation={9,8,7};
   vm.visual.render_flags|=0x2000000u;
   auto actual=compare("parent-inherited",vm,alpha,total);
   if(alpha==128){require((vertex_color(submission(actual))>>24)==255,"Inherited alpha multiplication moved before opacity");++total.independent_alpha_checks;}
  }
  {
   auto vm=synthetic(retail,0);Settings cfg;cfg.tint=true;cfg.tint_color=0xff808080;
   auto actual=compare("tint",vm,alpha,total,cfg);
   if(alpha==128){require((vertex_color(submission(actual))>>24)==255,"Tint alpha multiplication moved before opacity");++total.independent_alpha_checks;}
  }
  {
   auto vm=synthetic(retail,0);vm.visual.flags=(vm.visual.flags&~0x1e0u)|(1u<<5);
   auto actual=compare("additive",vm,alpha,total);
   if(alpha==128){require(!submission(actual).vertices.empty()&&actual.graphics.state.sourceBlend==BlendFactor::SourceAlpha&&
    actual.graphics.state.destinationBlend==BlendFactor::One,"Opacity changed additive blending");++total.independent_alpha_checks;}
  }
  {
   auto vm=synthetic(retail,0);vm.geometry.trail=std::make_unique<AnmTrail>();vm.geometry.trail->age.set(3);
   for(unsigned i=0;i<3;++i){vm.geometry.trail->points[i]={float(i)*2,float(i)};vm.geometry.trail->colors[i]=(u32(51+i*67)<<24)|0xabcdef;}
   auto actual=compare("trail-unscaled",vm,alpha,total);
   require(vertex_color(submission(actual))==0x33abcdef,"Opacity was newly applied to trail vertex color");++total.independent_alpha_checks;
  }
  {
   auto vm=synthetic(retail,23);vm.geometry.distortion=std::make_unique<AnmDistortion>();
   for(unsigned i=0;i<33;++i)vm.geometry.distortion->vertices[i]={{float(i),float(i+3),.25f},1,0x7b123456,{.25f,.5f}};
   auto actual=compare("distortion-unscaled",vm,alpha,total);
   require(vertex_color(submission(actual))==0x7b123456,"Opacity was newly applied to distortion vertex color");++total.independent_alpha_checks;
  }
  {
   auto vm=synthetic(retail,0);vm.geometry.gather=std::make_unique<AnmGatherEffect>();vm.geometry.gather->reserved=0x735139;
   compare("unused-gather",vm,alpha,total);
  }
  for(int mask:{1,2}){
   auto vm=synthetic(retail,0);vm.geometry.overlay=std::make_unique<AnmOverlay>();vm.geometry.overlay->mode=mask;
   auto parent=synthetic(retail,0);parent.variables.rotation={8,9,-7};
   for(unsigned i=0;i<5;++i){
    vm.geometry.overlay->panels[i]=std::make_unique<AnmVm>(synthetic(retail,1));
    auto& panel=*vm.geometry.overlay->panels[i];panel.presentation_generation=vm.presentation_generation+100+i;
    panel.rotation_parent=&parent;panel.creation_parent=&vm;panel.variables.position.x+=float(i)*9;
   }
   Settings cfg;cfg.overlay_traps=true;cfg.repeats=1;
   compare("overlay-"+std::to_string(mask),vm,alpha,total,cfg);
  }
 }
 for(unsigned kind=0;kind<5;++kind){
  auto vm=synthetic(retail,kind==0?24:kind==1?9:kind==2?0:kind==3?1:7);
  if(kind<2)vm.variables.integers[0]=100;
  if(kind==2)vm.visual.sprite=0x7fffffff;
  if(kind==3)vm.visual.flags|=3u<<21;
  const auto actual=compare("invalid-"+std::to_string(kind),vm,72,total);
  require(actual.results.front()==-2&&!actual.errors.front().empty(),"Opacity invalid-input fixture missed its error path");
 }
 {
  auto vm=synthetic(retail,1);vm.rotation_parent=&vm;vm.variables.rotation={9,-8,7};
  compare("self-parent-alias",vm,128,total);
 }
 identity_transition(retail,total);
 require(total.parent_changes>0&&total.submissions>0&&total.identities>0&&total.independent_alpha_checks>=20,
  "Opacity reference coverage did not execute its required paths");
 std::printf("TH15_OPACITY_REFERENCE {\"cases\":%u,\"draws\":%u,\"events\":%u,\"submissions\":%u,\"presentationIdentities\":%u,\"parentMutationCases\":%u,\"independentAlphaChecks\":%u,\"alphaValues\":[0,1,72,128,160,254,255],\"reference\":\"old-full-copy\",\"cpuSubmissionsOnly\":true}\n",
  total.cases,total.draws,total.events,total.submissions,total.identities,total.parent_changes,total.independent_alpha_checks);
}
} // namespace opacity_draw_check

