// Executes the production queue and batching implementation. The terminal GPU
// operations are independent state-grid semantics, not a GLES/pixel oracle.
#include <algorithm>
#include <array>
#include <cstdint>
#include <cmath>
#include <cstring>
#include <map>
#include <vector>
#include <string>
#include <tuple>
#include <random>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <GpuCommandLiveness.hpp>
#define private public
#include "Renderer.hpp"
#undef private
using namespace touhou::sdl;
using namespace touhou::graphics;
constexpr unsigned Width=24,Height=24,Handles=8;
struct Grid {
 std::array<std::array<u32,Width*Height>,Handles> color{},depth{};
 bool operator==(const Grid& b)const{return color==b.color&&depth==b.depth;}
};
struct World {Grid actual,expected;std::array<u32,Handles> versions{};u32 scale=1;unsigned terminal=0;bool expectedDither=false;};
std::uint64_t checks=0,cases=0,removed=0,queued=0;
void require(bool value,const char* label){++checks;if(!value){std::fprintf(stderr,"FAIL %s case=%llu checks=%llu\n",label,(unsigned long long)cases,(unsigned long long)checks);std::exit(1);}}
Surface resolver(void* owner,u32 id){auto& w=*static_cast<World*>(owner);require(id<Handles,"resource handle");return {id,Width/w.scale,Height/w.scale,id>=5?PixelFormat::Depth16:PixelFormat::Bgra8,Width*4,nullptr,0,w.versions[id],w.scale};}
void initial(World& w){for(unsigned id=0;id<Handles;++id)for(unsigned i=0;i<Width*Height;++i){w.actual.color[id][i]=w.expected.color[id][i]=(id*109+i*13)%256;w.actual.depth[id][i]=w.expected.depth[id][i]=255;}}
void clear_grid(Grid& g,const State& state,u32 scale,u32 flags,u32 color,float depth,const i32* rects,u32 count){
 std::array<i32,4> viewport{int(state.viewport.x),int(state.viewport.y),int(state.viewport.x+state.viewport.width),int(state.viewport.y+state.viewport.height)};
 if(!rects){rects=viewport.data();count=1;}
 for(unsigned n=0;n<count;++n){const i32* p=rects+n*4;const auto left=std::int64_t(p[0])*scale,top=std::int64_t(p[1])*scale,right=std::int64_t(p[2])*scale,bottom=std::int64_t(p[3])*scale;
  if(left>right||top>bottom)continue;
  for(int y=0;y<int(Height);++y)for(int x=0;x<int(Width);++x)if(x>=left&&x<right&&y>=top&&y<bottom){const unsigned i=y*Width+x;if(flags&1)g.color[state.target][i]=color;if((flags&2)&&state.depth)g.depth[state.depth][i]=u32(depth*255);}
 }
}
u32 source_vertex(Topology kind,u32 primitive){return kind==Topology::Triangles?primitive*3:kind==Topology::Fan?primitive+1:kind==Topology::Lines?primitive*2:primitive;}
void draw_grid(Grid& g,const State& s,u32 scale,Topology kind,u32 count,const void* data,const void* index,IndexType format,const void* instances,u32 instanceSize){
 const auto* bytes=static_cast<const u8*>(data);const unsigned repetitions=instanceSize?instanceSize/68:1;
 for(unsigned instance=0;instance<repetitions;++instance)for(unsigned n=0;n<count;++n){
  u32 vertex=source_vertex(kind,n);
  if(index){if(format==IndexType::UInt16){std::uint16_t i;std::memcpy(&i,static_cast<const u8*>(index)+vertex*2,2);vertex=i;}else std::memcpy(&vertex,static_cast<const u8*>(index)+vertex*4,4);}
  u32 seed;std::memcpy(&seed,bytes+std::size_t(vertex)*s.stride,4);
  if(instances){u32 extra;std::memcpy(&extra,static_cast<const u8*>(instances)+instance*68+64,4);seed^=extra;}
  const bool regular=kind>=Topology::Triangles&&kind<=Topology::Fan;
  // Points/lines deliberately reach outside the viewport. Triangles have
  // holes inside it; this does not assume full geometry coverage.
  for(unsigned y=0;y<Height;++y)for(unsigned x=0;x<Width;++x){
   if(regular&&(x<std::uint64_t(s.viewport.x)*scale||x>=(std::uint64_t(s.viewport.x)+s.viewport.width)*scale||y<std::uint64_t(s.viewport.y)*scale||y>=(std::uint64_t(s.viewport.y)+s.viewport.height)*scale))continue;
   unsigned i=y*Width+x;if((i*17+seed)%11<3)continue;
   auto& target=g.color[s.target][i];auto& depth=g.depth[s.depth][i];const u32 z=(seed+i*7)%256;
   if(s.depth&&s.pipeline.depthTest&&z>depth)continue;
   const u32 source=s.texture?g.color[s.texture][(i+seed)%(Width*Height)]:0;
   const u32 result=s.pipeline.blend?((target*3+source+seed)&255):((source^seed)&255);
   if(s.pipeline.colorMask)target=result;
   if(s.depth&&s.pipeline.depthTest&&s.pipeline.depthWrite)depth=z;
  }
 }
}
namespace touhou::sdl {
State::State(){for(auto& m:matrix)m={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};}
namespace {struct QueueDrawState {PipelineState pipeline{};bool valid=false;} drawState;}
Renderer::Renderer(int v,Resolve r,void* p):version(v),resolve(r),owner(p){drawState={};}
Renderer::~Renderer(){discard();}
void Renderer::issue_now(const State& s,Topology kind,u32 count,const void* data,u32,const void* indices,IndexType format,const void* instances,u32 instanceSize){
 auto& w=*static_cast<World*>(owner);draw_grid(w.actual,s,w.scale,kind,count,data,indices,format,instances,instanceSize);++w.terminal;drawState.valid=true;drawState.pipeline=s.pipeline;
 for(u32 id:{s.target,s.texture})if(id)surfaces[id].version=w.versions[id];
}
void Renderer::clear_now(u32 flags,u32 color,float depth,u32,const i32* rects,u32 count){
 auto& w=*static_cast<World*>(owner);clear_grid(w.actual,state,w.scale,flags,color^(drawState.pipeline.dither?1u:0u),depth,rects,count);++w.terminal;surfaces[state.target].version=w.versions[state.target];
}
#include "RendererBatch.inc"
#include "RendererQueue.inc"
}
State setup(u32 scale){State s;s.target=1;s.depth=5;s.texture=3;s.stride=28;s.layout=attributes(VertexLayout::ScreenColorUv);s.viewport={0,0,Width/scale,Height/scale,0,1};return s;}
void warm(Renderer& r){r.defer_gpu_commands=true;drawState.valid=true;for(u32 id=1;id<Handles;++id)r.surfaces[id].version=0;}
void draw(Renderer& r,u32 seed,Topology kind=Topology::Triangles,u32 count=1,bool direct=false){
 auto& w=*static_cast<World*>(r.owner);std::vector<u8> bytes(std::size_t(vertex_count(kind,count))*r.state.stride);
 for(unsigned i=0;i<vertex_count(kind,count);++i)std::memcpy(bytes.data()+i*r.state.stride,&seed,4);
 w.expectedDither=r.state.pipeline.dither;draw_grid(w.expected,r.state,w.scale,kind,count,bytes.data(),nullptr,IndexType::UInt16,nullptr,0);
 if(direct)r.draw_batch(count,bytes.data(),r.state.stride);else r.draw(kind,count,bytes.data(),r.state.stride);
 std::fill(bytes.begin(),bytes.end(),0xee);
}
void clear(Renderer& r,u32 flags,u32 color,const i32* rects=nullptr,u32 count=0){auto& w=*static_cast<World*>(r.owner);clear_grid(w.expected,r.state,w.scale,flags,color^(w.expectedDither?1u:0u),1,rects,count);r.clear(flags,color,1,0,rects,count);}
void observe(Renderer& r,const char* label){r.synchronize();const auto& w=*static_cast<World*>(r.owner);require(w.actual==w.expected,label);require(r.command_count==0,"drained count");}
void finish(Renderer& r){observe(r,"final grid");removed+=r.pruned_draws;queued+=r.queued_draws;++cases;}
int main(){
 {
  World w;initial(w);Renderer r(10,resolver,&w);warm(r);r.state=setup(1);
  draw(r,19);clear(r,3,0);observe(r,"full overwrite");require(r.pruned_draws==1,"dead draw not pruned");finish(r);
 }
 {
  World w;initial(w);Renderer r(10,resolver,&w);warm(r);r.state=setup(1);
  draw(r,19);r.state.target=2;r.state.texture=1;draw(r,17);r.state.target=1;r.state.texture=3;clear(r,3,0);
  observe(r,"texture read dependency");require(r.pruned_draws==0,"live texture producer removed");finish(r);
 }
 {
  World w;initial(w);Renderer r(10,resolver,&w);warm(r);r.state=setup(1);r.state.pipeline.depthTest=r.state.pipeline.depthWrite=true;
  draw(r,39);clear(r,1,3);draw(r,5);observe(r,"depth dependency");require(r.pruned_draws==0,"depth producer removed");finish(r);
 }
 for(u32 scale:{1u,2u}){
  World w;w.scale=scale;initial(w);Renderer r(10,resolver,&w);warm(r);r.state=setup(scale);draw(r,17,Topology::Triangles,3,true);
  const i32 rect[]{0,0,4,4};clear(r,3,2,rect,1);observe(r,"partial clear");require(r.pruned_draws==0,"partially covered draw removed");finish(r);
 }
 {
  World w;initial(w);Renderer r(10,resolver,&w);r.state=setup(1);draw(r,5,Topology::Triangles,1,true);
  require(w.terminal==1,"first texture upload was queued");observe(r,"first upload");finish(r);
 }
 {
  World w;initial(w);Renderer r(10,resolver,&w);warm(r);r.state=setup(1);draw(r,9);r.flush();
  const auto before=r.queued_draws;require(before==1,"pre-cancel queued");
  auto b=std::array<u8,84>{};r.draw(Topology::Triangles,1,b.data(),28);
#if TH15_MULTIPLAYER
  r.cancel_present();
#else
  r.discard();r.pending=0;
#endif
  observe(r,"cancel preserves already submitted work only");require(r.queued_draws==before,"canceled CPU batch reached queue");finish(r);
 }
 {
  World w;initial(w);Renderer r(10,resolver,&w);warm(r);r.state=setup(1);draw(r,9);r.flush();
  require(r.command_count==1,"initial queued work");observe(r,"readback barrier before overwrite");clear(r,3,0);finish(r);
 }
 {
  World w;initial(w);Renderer r(10,resolver,&w);warm(r);r.state=setup(1);draw(r,9,Topology::Lines,1);r.state.viewport={4,4,4,4,0,1};clear(r,3,0);
  observe(r,"line extends beyond viewport");require(r.pruned_draws==0,"line extent underestimated");finish(r);
 }
 {
  World w;initial(w);Renderer r(10,resolver,&w);warm(r);r.state=setup(1);draw(r,9);const i32 rect[]{0,0,24,24};clear(r,3,0,rect,0);
  observe(r,"explicit zero rect");require(r.pruned_draws==0,"zero rectangle invented clear");finish(r);
 }
 {
  World w;initial(w);Renderer r(10,resolver,&w);warm(r);r.state=setup(1);
  for(unsigned n=0;n<4100;++n){draw(r,n);r.flush();}
  require(w.terminal>0,"bounded command queue did not drain");finish(r);
 }
 {
  World w;initial(w);Renderer r(10,resolver,&w);warm(r);r.state=setup(1);
  r.state.pipeline.dither=true;draw(r,19);clear(r,3,7);
  observe(r,"clear inherits dither from overwritten draw");require(r.pruned_draws==0,"dither-changing draw must execute");finish(r);
 }
 {
  World w;initial(w);Renderer r(10,resolver,&w);warm(r);r.defer_gpu_commands=false;r.state=setup(1);
  draw(r,19);clear(r,3,7);observe(r,"ordinary/exact path stays immediate");
  require(r.queued_draws==0&&r.pruned_draws==0,"inactive queue recorded work");finish(r);
 }
 std::mt19937 random(0x735139);
 for(unsigned sequence=0;sequence<1000;++sequence){
  World w;w.scale=sequence%2+1;initial(w);Renderer r(10,resolver,&w);warm(r);r.state=setup(w.scale);
  for(unsigned n=0;n<80;++n){
   if(n%19==0)observe(r,"intermediate observation");
   r.state.target=1+random()%3;r.state.texture=1+(r.state.target+random()%2)%3;r.state.depth=5+random()%2;
   const unsigned size=Width/w.scale;
   r.state.viewport=n%7?Viewport{u32(random()%4),u32(random()%4),size-4,size-4,0,1}:Viewport{0,0,size,size,0,1};
   r.state.pipeline.dither=random()&1;r.state.pipeline.depthTest=random()&1;r.state.pipeline.depthWrite=random()&1;r.state.pipeline.blend=random()&1;r.state.pipeline.colorMask=random()%17;
   if(n%5==0){const unsigned flags=1+random()%3,color=random()%256;
    if(n%10==0){const i32 rects[]{-2,1,int(size),int(size-1)};clear(r,flags,color,rects,1);}else clear(r,flags,color);
   }else {const unsigned seed=random()%10000;const auto kind=Topology(random()%6);const unsigned count=1+random()%5;draw(r,seed,kind,count,kind==Topology::Triangles);}
  }
  finish(r);
 }
 require(removed>100,"no useful overwritten commands exercised");
 std::printf("TH15_DEFERRED_COMMAND {\"sequences\":%llu,\"checks\":%llu,\"queuedDraws\":%llu,\"prunedDraws\":%llu,\"gpuPixels\":false}\n",(unsigned long long)cases,(unsigned long long)checks,(unsigned long long)queued,(unsigned long long)removed);
}
