// Compiles the production RendererBatch.inc and Renderer.hpp. Only the terminal
// GL issue and platform lifetime are replaced by an ordered byte/state sink.
// No pixels, Wasm execution, gameplay or frame rate is asserted here.
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <map>
#include <vector>
#include <string>
#include <tuple>
#include <random>
#include <cstdio>
#include <cstdlib>
#include <limits>
#define private public
#include "Renderer.hpp"
#undef private
namespace touhou::sdl {
State::State(){for(auto& m:matrix)m={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};}
Renderer::Renderer(int v,Resolve r,void* p):version(v),resolve(r),owner(p){}
Renderer::~Renderer(){discard();}
struct Packet {State state;Topology kind;u32 count;std::vector<u8> bytes,index,instances;IndexType indexFormat;};
std::vector<Packet> issued;
void Renderer::issue(const State& s,Topology kind,u32 count,const void* data,u32 size,const void* index,IndexType format,const void* instance,u32 instanceSize){
 ++stats.batches;Packet packet{s,kind,count,{}, {},{},format};
 if(size)packet.bytes.assign(static_cast<const u8*>(data),static_cast<const u8*>(data)+size);
 if(index){const auto* p=static_cast<const u8*>(index);packet.index.assign(p,p+vertex_count(kind,count)*(format==IndexType::UInt16?2:4));}
 if(instanceSize){const auto* p=static_cast<const u8*>(instance);packet.instances.assign(p,p+instanceSize);}
 // Model only texture initialization/version acknowledgement, not GL output.
 for(u32 id:{s.target,s.texture})if(id)surfaces[id].version=resolve(owner,id).version;
 issued.push_back(std::move(packet));
}
#include "RendererBatch.inc"
}
using namespace touhou::sdl;
using namespace touhou::graphics;
std::uint64_t checks=0,scenarios=0,logicalDraws=0,submittedPackets=0;
void need(bool b,const char* text){++checks;if(!b){std::fprintf(stderr,"FAIL %s check=%llu scenario=%llu\n",text,(unsigned long long)checks,(unsigned long long)scenarios);std::exit(1);}}
struct Resources {std::array<u32,8> version{};};
Surface resolve_resource(void* owner,u32 id){auto& resources=*static_cast<Resources*>(owner);need(id<8,"unexpected resource");return {id,640,480,PixelFormat::Bgra8,2560,nullptr,0,resources.version[id],1};}
void warm(Renderer& r){for(u32 id:{1,3,4})r.surfaces[id].version=0;}
State screen(){State s;s.target=1;s.depth=2;s.texture=3;s.layout=attributes(VertexLayout::ScreenColorUv);s.stride=28;return s;}
bool same_state(const State& a,const State& b){
 return a.layout==b.layout&&a.stride==b.stride&&a.target==b.target&&a.texture==b.texture&&a.depth==b.depth&&
  a.viewport.x==b.viewport.x&&a.viewport.y==b.viewport.y&&a.viewport.width==b.viewport.width&&a.viewport.height==b.viewport.height&&
  a.viewport.min==b.viewport.min&&a.viewport.max==b.viewport.max&&a.pipeline.compatible(b.pipeline)&&
  (a.layout.screen||a.matrix==b.matrix);
}
struct Atom {State state;Topology kind;std::vector<u8> bytes;};
std::vector<Atom> expected,actual;
// Independent triangle/line expansion; not the in-place production packing loop.
void expand(std::vector<Atom>& out,const State& state,Topology kind,u32 count,const std::vector<u8>& data,const std::vector<u8>& indices={},IndexType format=IndexType::UInt16){
 auto vertex=[&](u32 i){if(!indices.empty()){
   if(format==IndexType::UInt16){std::uint16_t n;std::memcpy(&n,indices.data()+i*2,2);i=n;}
   else std::memcpy(&i,indices.data()+i*4,4);
  }need((std::uint64_t(i)+1)*state.stride<=data.size(),"reference vertex extent");return data.data()+size_t(i)*state.stride;};
 for(u32 n=0;n<count;++n){std::array<u32,3> ids{};u32 arity=3;Topology base=Topology::Triangles;
  switch(kind){
   case Topology::Points:ids={n,0,0};arity=1;base=Topology::Points;break;
   case Topology::Lines:ids={n*2,n*2+1,0};arity=2;base=Topology::Lines;break;
   case Topology::LineStrip:ids={n,n+1,0};arity=2;base=Topology::Lines;break;
   case Topology::Triangles:ids={n*3,n*3+1,n*3+2};break;
   case Topology::Strip:ids=n%2?std::array<u32,3>{n+1,n,n+2}:std::array<u32,3>{n,n+1,n+2};break;
   case Topology::Fan:ids={0,n+1,n+2};break;
  }
  Atom atom{state,base,{}};for(u32 i=0;i<arity;++i){const auto* p=vertex(ids[i]);atom.bytes.insert(atom.bytes.end(),p,p+state.stride);}out.push_back(std::move(atom));
 }
}
void expected_draw(const State& state,Topology kind,u32 count,const std::vector<u8>& bytes,const std::vector<u8>& indices={},IndexType format=IndexType::UInt16){++logicalDraws;expand(expected,state,kind,count,bytes,indices,format);}
void check_output(){
 actual.clear();for(const auto& p:issued){
  if(p.instances.empty())expand(actual,p.state,p.kind,p.count,p.bytes,p.index,p.indexFormat);
  else {
   need(p.instances.size()%68==0,"instance extent");
   for(size_t i=0;i<p.instances.size();i+=68){auto s=p.state;std::memcpy(s.matrix[0].data(),p.instances.data()+i,64);std::memcpy(&s.pipeline.textureFactor,p.instances.data()+i+64,4);expand(actual,s,p.kind,p.count,p.bytes);}
  }
 }
 need(actual.size()==expected.size(),"primitive count/order");
 for(size_t i=0;i<actual.size();++i){need(actual[i].kind==expected[i].kind,"primitive topology");need(same_state(actual[i].state,expected[i].state),"effective pipeline state");need(actual[i].bytes==expected[i].bytes,"vertex bytes or winding changed");}
 submittedPackets+=issued.size();issued.clear();expected.clear();actual.clear();++scenarios;
}
std::vector<u8> bytes(Topology kind,u32 count,u32 stride,std::uint32_t seed){std::vector<u8> b(size_t(vertex_count(kind,count))*stride);std::mt19937 r(seed);for(auto& v:b)v=u8(r());return b;}
void emit(Renderer& r,bool direct,Topology kind,u32 count,u32 seed){
 auto b=bytes(kind,count,r.state.stride,seed);expected_draw(r.state,kind,count,b);
 if(direct)r.draw_batch(count,b.data(),r.state.stride);else r.draw(kind,count,b.data(),r.state.stride);
 // A reused caller arena must not affect any deferred batch's vertex bytes.
 std::fill(b.begin(),b.end(),0xee);
}
int main(){
 {
  Resources resources;Renderer r(10,resolve_resource,&resources);warm(r);r.state=screen();
  emit(r,true,Topology::Triangles,1,1);emit(r,true,Topology::Triangles,1,2);
  need(issued.empty(),"same-state draws should remain queued");r.flush();need(issued.size()==1,"adjacent direct draws did not merge");need(r.stats.calls==2,"logical draw counter doubled");check_output();
 }
 {
  Resources resources;Renderer r(10,resolve_resource,&resources);r.state=screen();
  emit(r,true,Topology::Triangles,1,1);need(issued.size()==1,"first upload was deferred");
  emit(r,true,Topology::Triangles,1,2);emit(r,true,Topology::Triangles,1,3);r.flush();need(issued.size()==2,"warm draws did not merge after initialization");check_output();
  ++resources.version[3];emit(r,true,Topology::Triangles,1,4);need(issued.size()==1,"stale texture version was deferred");check_output();
 }
 for(unsigned field=0;field<36;++field){
  Resources resources;Renderer r(10,resolve_resource,&resources);warm(r);r.state=screen();emit(r,true,Topology::Triangles,2,field);auto& p=r.state.pipeline;
  switch(field){
   case 0:r.state.texture=4;break;case 1:r.state.target=4;break;case 2:r.state.depth=0;break;
   case 3:r.state.viewport.x=7;break;case 4:r.state.viewport.width=320;break;case 5:r.state.viewport.min=.25f;break;
   case 6:p.blend=true;break;case 7:p.alphaTest=true;break;case 8:p.depthWrite=false;break;case 9:p.depthTest=true;break;
   case 10:p.depthCompare=Compare::Greater;break;case 11:p.alphaCompare=Compare::Never;break;
   case 12:p.sourceBlend=BlendFactor::SourceColor;break;case 13:p.destinationBlend=BlendFactor::One;break;
   case 14:p.blendEquation=BlendEquation::Maximum;break;case 15:p.separateAlphaBlend=true;break;
   case 16:p.sourceAlphaBlend=BlendFactor::SourceAlpha;break;case 17:p.destinationAlphaBlend=BlendFactor::DestinationAlpha;break;
   case 18:p.alphaBlendEquation=BlendEquation::ReverseSubtract;break;case 19:p.cull=Cull::Front;break;
   case 20:p.colorMask=3;break;case 21:p.alphaReference=64;break;case 22:p.fogColor=0x12345678;break;
   case 23:p.textureFactor=0x12345678;break;case 24:p.fogMode=FogMode::Linear;break;case 25:p.fogNear=64;break;
   case 26:p.fogFar=64;break;case 27:p.fogDensity=64;break;case 28:p.textureTransform=true;break;
   case 29:p.minFilter=Filter::Nearest;break;case 30:p.magFilter=Filter::Nearest;break;
   case 31:p.addressU=Address::Clamp;break;case 32:p.addressV=Address::Mirror;break;
   case 33:p.color.first.alphaOnly=true;break;case 34:p.alpha.second.complement=true;break;case 35:p.dither=true;break;
  }
  emit(r,true,Topology::Triangles,3,field+1);r.flush();need(issued.size()==2,"incompatible states coalesced");check_output();
 }
 for(auto kind:{Topology::Strip,Topology::Fan}){
  Resources resources;Renderer r(10,resolve_resource,&resources);warm(r);r.state=screen();
  emit(r,false,kind,31,3);emit(r,true,Topology::Triangles,4,7);r.flush();need(issued.size()==1,"direct triangle did not join original-topology batch");check_output();
 }
 {
  Resources resources;Renderer r(10,resolve_resource,&resources);warm(r);r.state=screen();
  r.state.layout=attributes(VertexLayout::WorldUv);r.state.stride=20;
  for(u32 i=0;i<7;++i){r.state.matrix[0][12]=float(i);r.state.pipeline.textureFactor=0xff000000|i;emit(r,false,Topology::Strip,2,9);}
  need(issued.empty(),"compatible world instances flushed early");
  r.state=screen();emit(r,true,Topology::Triangles,3,10);r.flush();need(issued.size()==2,"world/direct boundary changed");check_output();
 }
 {
  Resources resources;Renderer r(10,resolve_resource,&resources);warm(r);r.state=screen();
  for(u32 i=0;i<9;++i)emit(r,true,Topology::Triangles,2048,i);
  need(!issued.empty(),"bounded vertex buffer never flushed");r.flush();check_output();
 }
 {
  Resources resources;Renderer r(10,resolve_resource,&resources);warm(r);r.state=screen();
  emit(r,true,Topology::Triangles,2,8);r.flush();check_output();
  auto b=bytes(Topology::Triangles,3,28,8);r.draw_batch(3,b.data(),28);r.discard();r.flush();need(issued.empty(),"discarded future scratch reached the sink");++scenarios;
 }
 {
  Resources resources;Renderer r(10,resolve_resource,&resources);warm(r);r.state=screen();
  auto b=bytes(Topology::Triangles,2,28,8);r.state.pipeline.fogNear=std::numeric_limits<float>::quiet_NaN();
  r.draw_batch(2,b.data(),28);r.draw_batch(2,b.data(),28);r.flush();need(issued.size()==2,"NaN compatibility changed");issued.clear();++scenarios;
 }
 std::mt19937 rng(0x735139);
 for(unsigned epoch=0;epoch<100;++epoch){
  Resources resources;Renderer r(10,resolve_resource,&resources);warm(r);r.state=screen();
  for(unsigned op=0;op<250;++op){
   if(op%29==0)r.flush();
   if(op%11==0){r.state.texture=(rng()&1)?3:4;r.state.pipeline.sourceBlend=BlendFactor(rng()%11);r.state.pipeline.destinationBlend=BlendFactor(rng()%11);r.state.pipeline.blend=bool(rng()&1);}
   const auto kind=Topology(rng()%6);const unsigned count=rng()%25+1;
   emit(r,kind==Topology::Triangles,kind,count,rng());
  }r.flush();check_output();need(r.stats.calls==250,"logical calls changed");
 }
 std::printf("TH15_ORDERED_BATCH {\"scenarios\":%llu,\"checks\":%llu,\"logicalDraws\":%llu,\"gpuPacketsInSink\":%llu,\"source\":\"production-RendererBatch.inc\",\"gpuValidated\":false}\n",(unsigned long long)scenarios,(unsigned long long)checks,(unsigned long long)logicalDraws,(unsigned long long)submittedPackets);
}
