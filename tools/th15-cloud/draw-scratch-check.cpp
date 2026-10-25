// Real instrumented Wasm ownership/undo regression. GPU output is a CPU sink.
#include "AnmRenderer.hpp"
#include "AnmOverlay.hpp"
#include <eagler/netplay/WriteJournal.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <array>
#include <algorithm>
namespace journal=Netplay::WriteJournal;
using namespace th15;
using namespace touhou::graphics;
static unsigned checks=0;
static void need(bool ok,const char* message){++checks;if(!ok){std::fprintf(stderr,"FAIL %s | journal=%s\n",message,journal::Error());std::exit(1);}}
struct Sink:ZunGraphics {
 PipelineState state;u32 calls=0;std::uint64_t hash=1469598103934665603ull;
 PipelineState& pipeline()override{return state;}
 u32 texture(const AnmResource&,u32 n)override{return n+1;}
 void bind_texture(u32)override{}
 void set_layout(VertexLayout)override{}
 void set_matrix(MatrixKind,const Matrix4&)override{}
 void triangles(u32 n,const void* p,u32 stride)override{primitives(Topology::Triangles,n,p,stride);}
 void primitives(Topology topology,u32 n,const void* p,u32 stride)override{
  ++calls;const auto* bytes=static_cast<const unsigned char*>(p);
  for(std::size_t i=0;i<std::size_t(vertex_count(topology,n))*stride;++i)hash=(hash^bytes[i])*1099511628211ull;
 }
 bool select_target(const AnmResource*,u32)override{return true;}
 bool clear_target(u32,const GraphicsViewport*)override{return true;}
 bool clear_depth(const GraphicsViewport*)override{return true;}
 void set_viewport(const GraphicsViewport&)override{}
 void reset(){calls=0;hash=1469598103934665603ull;}
};
namespace th15 {
struct AnmRendererScratchProbe {
 static void inspect(const AnmRenderer& r){
  need(journal::OwnedByGameHeap(&r),"renderer itself must remain in simulation heap");
  need(!journal::OwnedByGameHeap(r.scratch.get()),"scratch owner in game heap");
  const auto check=[](const auto& v){
   need(!journal::OwnedByGameHeap(&v),"vector metadata in game heap");
   if(v.capacity())need(!journal::OwnedByGameHeap(v.data()),"vector backing store in game heap");
  };
  check(r.vertices);check(r.shapes);check(r.opacity_screen_vertices);check(r.opacity_world_vertices);
 }
 static std::array<std::size_t,4> capacities(const AnmRenderer& r){return {r.vertices.capacity(),r.shapes.capacity(),r.opacity_screen_vertices.capacity(),r.opacity_world_vertices.capacity()};}
};
}
static void setup(AnmVm& vm,AnmResource& resource,AnmEnvironment& env,unsigned mode){
 vm.resource=&resource;vm.sprite=&resource.sprites[0];vm.environment=&env;
 vm.visual.flags=3u|(mode<<25);vm.visual.render_flags=0;vm.visual.sprite=0;
 vm.visual.sprite_size={16,12};vm.visual.color=0xffe1b39d;vm.visual.secondary_color=0x7f3579bd;
 vm.visual.sprite_matrix.identity();vm.visual.transform_matrix.identity();vm.visual.uv_matrix.identity();
 vm.visual.uv[0]={0,0};vm.visual.uv[1]={1,0};vm.visual.uv[2]={0,1};vm.visual.uv[3]={1,1};
 vm.variables.position={160,210,.5f};vm.presentation_motion=true;
}
static bool same(Vec3 a,Vec3 b){return std::memcmp(&a,&b,sizeof a)==0;}
int main(){
 need(journal::Initialize(nullptr,0,16*1024*1024),"initialize instrumented journal");
 Sink sink;sink.configure_game(1);AnmResource resource;resource.sprites.resize(1);resource.textures.resize(1);
 resource.sprites[0].texture=0;resource.sprites[0].width=16;resource.sprites[0].height=12;
 AnmRenderer* renderer;AnmVm *vm,*parent,*grandparent;AnmEnvironment* env;
 {journal::GameScope game;renderer=new AnmRenderer(sink);vm=new AnmVm;parent=new AnmVm;grandparent=new AnmVm;env=new AnmEnvironment;}
 auto& r=*renderer;setup(*vm,resource,*env,1);setup(*parent,resource,*env,1);setup(*grandparent,resource,*env,1);
 vm->rotation_parent=parent;parent->rotation_parent=grandparent;
 vm->variables.rotation={9,8,7};parent->variables.rotation={7,-8,9};grandparent->variables.rotation={3,2,1};
 parent->variables.position={0,0,0};grandparent->variables.position={0,0,0};
 r.set_viewport({0,0,640,480,0,1});r.set_camera(AnmCamera{});
 AnmRendererScratchProbe::inspect(r);
 const auto oldVm=vm->variables.rotation,oldParent=parent->variables.rotation;const auto oldQuad=vm->visual.quad;
 need(journal::BeginFrame(1),"begin parent mutation frame");
 {journal::GameScope game;need(r.draw(*vm)==0,"draw with mutable parent rotation");}
 need(!same(vm->variables.rotation,oldVm)&&!same(parent->variables.rotation,oldParent),"real Draw must execute rotation normalization");
 need(r.pending().size()==6,"authored quad missing before flush");
 need(journal::EndFrame(),"seal parent mutation frame");
 need(journal::RestoreTo(1),"undo parent mutation");
 need(same(vm->variables.rotation,oldVm)&&same(parent->variables.rotation,oldParent),"rotation state escaped write journal");
 need(std::memcmp(vm->visual.quad.data(),oldQuad.data(),sizeof oldQuad)==0,"quad state escaped write journal");
 need(r.pending().size()==6,"host output unexpectedly restored by simulation journal");
 const auto before=sink.calls;r.discard_draw_scratch();r.invalidate();
 need(r.pending().empty()&&sink.calls==before,"rewind flushed discarded future vertices");
 // Repeated rewinds change draw counts and grow all three dynamic payload types.
 std::array<std::size_t,4> maxCapacity{};unsigned long long maxBlocks=0;
 for(unsigned iteration=0;iteration<18;++iteration){
  const unsigned count=16u<<(iteration%6),frame=100+iteration;
  vm->rotation_parent=parent->rotation_parent=nullptr;
  vm->variables.rotation={};vm->variables.integers[0]=int(count);
  const unsigned mode=iteration%3==0?9:iteration%3==1?24:17;
  vm->visual.flags=3u|(mode<<25);vm->visual.render_flags=0;
  {journal::GameScope game;
   vm->geometry.screen_vertices.resize(count*2);vm->geometry.world_vertices.resize(count*2);
   for(unsigned i=0;i<count*2;++i){vm->geometry.screen_vertices[i]={{float(i),float(i%17),.25f},1,0xff735139,{.25f,.5f}};
    vm->geometry.world_vertices[i]={{float(i)*.001f,float(i%17)*.001f,.25f},0xff735139,{.25f,.5f}};}
  }
  r.multiplayer_player_opacity[vm]=72;sink.reset();
  need(journal::BeginFrame(frame),"begin growth frame");
  {journal::GameScope game;need(r.draw(*vm)==0,"draw dynamic payload");r.flush();}
  need(journal::EndFrame(),"seal growth frame");const auto expectedHash=sink.hash;const auto expectedCalls=sink.calls;
  maxBlocks=std::max(maxBlocks,(unsigned long long)journal::LastFrameBlocks());
  const auto capacity=AnmRendererScratchProbe::capacities(r);
  for(unsigned i=0;i<4;++i){need(capacity[i]>=maxCapacity[i],"host capacity regressed before undo");maxCapacity[i]=capacity[i];}
  AnmRendererScratchProbe::inspect(r);need(journal::RestoreTo(frame),"undo grown buffer frame");
  r.discard_draw_scratch();r.invalidate();
  need(AnmRendererScratchProbe::capacities(r)==capacity,"undo resurrected old host allocation");
  sink.reset();need(journal::BeginFrame(frame),"begin corrected growth frame");
  {journal::GameScope game;need(r.draw(*vm)==0,"redraw corrected payload");r.flush();}
  need(journal::EndFrame(),"seal corrected growth frame");
  need(sink.hash==expectedHash&&sink.calls==expectedCalls,"corrected submissions differ");
  journal::DiscardBefore(frame+1);
 }
 // Invalid shape paths write renderer.error, which must remain reversible.
 r.multiplayer_player_opacity.erase(vm);vm->visual.flags=3u|(17u<<25);vm->variables.integers[0]=0;r.error.clear();
 need(journal::BeginFrame(200),"begin invalid shape frame");
 {journal::GameScope game;need(r.draw(*vm)==-2&&!r.error.empty(),"invalid shape must report error");}
 need(journal::EndFrame(),"seal invalid shape");need(journal::RestoreTo(200),"restore invalid shape");
 need(r.error.empty(),"shape error assignment escaped journal");r.discard_draw_scratch();r.invalidate();
 need(journal::BeginFrame(201),"begin shape anchor error");
 {journal::GameScope game;vm->visual.flags=3u|(21u<<25)|(3u<<21);need(r.draw(*vm)==-2&&!r.error.empty(),"anchor error absent");}
 need(journal::EndFrame(),"seal anchor error");need(journal::RestoreTo(201),"restore anchor error");need(r.error.empty(),"anchor error escaped journal");
 AnmRendererScratchProbe::inspect(r);
 {journal::GameScope game;delete renderer;delete vm;delete parent;delete grandparent;delete env;}
 need(!journal::Failed(),"journal failed during cleanup");
 std::printf("TH15_DRAW_SCRATCH {\"checks\":%u,\"growthIterations\":18,\"maxJournalBlocks\":%llu,\"parentUndo\":true,\"unfinishedBatchDiscard\":true,\"errorUndo\":true,\"instrumentedWasm\":true,\"gpu\":false}\n",checks,maxBlocks);
}
