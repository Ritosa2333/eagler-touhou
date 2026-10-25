// Included inside touhou::sdl by the production renderer and CPU contract.
// Every queued byte is owned; no game vertex pointer survives the call.
#ifndef TH15_HOST_SCOPE
#define TH15_HOST_SCOPE
#endif
namespace {
constexpr u32 instanceStride=68;
bool equal(const State& a,const State& b,bool world=false){
 if(a.layout!=b.layout||a.stride!=b.stride||a.texture!=b.texture||a.target!=b.target||a.depth!=b.depth||std::memcmp(&a.viewport,&b.viewport,sizeof(Viewport)))return false;
 if(!a.pipeline.compatible(b.pipeline,world))return false;
 if(world){for(int i=1;i<4;i++)if(a.matrix[i]!=b.matrix[i])return false;}
 else if(!a.layout.screen&&a.matrix!=b.matrix)return false;
 return true;
}
}
void Renderer::draw_batch(u32 count,const void* data,u32 stride){TH15_HOST_SCOPE;
 stats.directBytes+=count*3*stride;
 // A first upload must keep the former immediate ordering. Otherwise a later
 // CPU atlas edit could be visible before the queued draw's first texture read.
 // Once uploaded, changed/copy/read/release barriers flush the existing batch.
 const auto ready=[this](u32 id){
  if(!id)return true;
  const auto found=surfaces.find(id);
  return found!=surfaces.end()&&found->second.version!=~0u&&
         found->second.version==resolve(owner,id).version;
 };
 if(!ready(state.target)||!ready(state.texture)){
  flush();++stats.calls;state.stride=stride;
  issue(state,Topology::Triangles,count,data,count*3*stride,nullptr,IndexType::UInt16,nullptr,0);return;
 }
 // The native ANM arena may flush without changing the effective GPU state.
 // Reuse the existing order-preserving triangle batch rather than forcing a
 // buffer replacement and GPU draw for each such arena flush.
 draw(Topology::Triangles,count,data,stride);
}
void Renderer::draw(Topology primitive,u32 count,const void* data,u32 stride,const void* index,IndexType indexFormat){TH15_HOST_SCOPE;
 stats.calls++;state.stride=stride;
 if(index||primitive<Topology::Triangles||primitive>Topology::Fan){flush();issue(state,primitive,count,data,vertex_count(primitive,count)*stride,index,indexFormat,nullptr,0);return;}
 const bool instance=version==10&&state.layout==attributes(VertexLayout::WorldUv)&&stride==20&&primitive==Topology::Strip&&count==2;
 if(instance){if(batching&&(!instancing||!equal(batchState,state,true)||quad.size()!=80||std::memcmp(quad.data(),data,80)))flush();if(!batching){batchState=state;quad.assign(static_cast<const u8*>(data),static_cast<const u8*>(data)+80);batching=instancing=true;}const auto old=worlds.size();worlds.resize(old+instanceStride);std::memcpy(worlds.data()+old,state.matrix[0].data(),64);std::memcpy(worlds.data()+old+64,&state.pipeline.textureFactor,4);if(worlds.size()>=1024*instanceStride)flush();return;}
 if(batching&&(instancing||!equal(batchState,state)))flush();if(!batching){batchState=state;batching=true;instancing=false;batchTopology=Topology::Triangles;}
 // Keep an isolated long strip/fan in its original topology. If a following
 // draw joins the batch, normalize it backwards before appending, preserving
 // triangle order and winding without retaining caller-owned vertex memory.
 if(version==10&&!batchCount&&count>=16&&(primitive==Topology::Strip||primitive==Topology::Fan)){
  const auto* src=static_cast<const u8*>(data);const size_t size=(count+2)*stride;
  batchTopology=primitive;batchBytes.assign(src,src+size);stats.copiedBytes+=size;batchCount=count;if(size>=1048576)flush();return;
 }
 if(batchTopology!=Topology::Triangles){
  batchScratch.resize(3*stride);batchBytes.resize(size_t(batchCount)*3*stride);
  for(u32 i=batchCount;i-->0;){const u32 a=batchTopology==Topology::Fan?0:i&1?i+1:i,b=batchTopology==Topology::Fan?i+1:i&1?i:i+1,c=i+2;u32 j=0;
   for(u32 v:{a,b,c}){std::memcpy(batchScratch.data()+j*stride,batchBytes.data()+v*stride,stride);++j;}
   std::memcpy(batchBytes.data()+size_t(i)*3*stride,batchScratch.data(),3*stride);
  }stats.copiedBytes+=batchCount*3*stride;batchTopology=Topology::Triangles;
 }
 const auto* src=static_cast<const u8*>(data);const size_t start=batchBytes.size(),size=count*3*stride;stats.copiedBytes+=size;batchBytes.resize(start+size);auto* dest=batchBytes.data()+start;
 if(primitive==Topology::Triangles)std::memcpy(dest,src,size);else if(count){std::memcpy(dest,src,3*stride);dest+=3*stride;for(u32 i=1;i<count;i++){u32 a=primitive==Topology::Fan?0:i&1?i+1:i,b=primitive==Topology::Fan?i+1:i&1?i:i+1,c=i+2;for(u32 v:{a,b,c}){std::memcpy(dest,src+v*stride,stride);dest+=stride;}}}batchCount+=count;if(batchBytes.size()>=1048576)flush();
}
void Renderer::flush(){TH15_HOST_SCOPE;if(!batching)return;batching=false;if(instancing)issue(batchState,Topology::Strip,2,quad.data(),quad.size(),nullptr,IndexType::UInt16,worlds.size()>instanceStride?worlds.data():nullptr,worlds.size()>instanceStride?worlds.size():0);else issue(batchState,batchTopology,batchCount,batchBytes.data(),batchBytes.size(),nullptr,IndexType::UInt16,nullptr,0);batchBytes.clear();worlds.clear();batchCount=0;instancing=false;}
void Renderer::discard(){TH15_HOST_SCOPE;batching=instancing=false;batchBytes.clear();worlds.clear();batchCount=0;}
