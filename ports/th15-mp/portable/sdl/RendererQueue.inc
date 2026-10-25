// Included inside touhou::sdl. Native Draw and all VM/RNG mutations have already
// executed. Only host GPU commands proved overwritten before any read may drop.
namespace {
using QueueRect=deferred::Rect;
constexpr std::size_t command_limit=4096,command_byte_limit=8*1024*1024;
bool command_extent(const Surface& surface,QueueRect& rect){
 const std::uint64_t width=std::uint64_t(surface.width)*surface.renderScale;
 const std::uint64_t height=std::uint64_t(surface.height)*surface.renderScale;
 if(!surface.handle||!width||!height||width>0x7fffffffu||height>0x7fffffffu)return false;
 rect={0,0,std::int64_t(width),std::int64_t(height)};return true;
}
QueueRect command_clip(QueueRect rect,QueueRect extent){
 return {std::max(rect.left,extent.left),std::max(rect.top,extent.top),
         std::min(rect.right,extent.right),std::min(rect.bottom,extent.bottom)};
}
std::uint64_t command_plane(u32 handle,unsigned aspect){return (std::uint64_t(handle)<<2)|aspect;}
bool command_viewport(const Viewport& view,u32 scale,QueueRect& rect){
 const std::uint64_t left=std::uint64_t(view.x)*scale,top=std::uint64_t(view.y)*scale;
 const std::uint64_t width=std::uint64_t(view.width)*scale,height=std::uint64_t(view.height)*scale;
 if(!scale||left>0x7fffffffu||top>0x7fffffffu||width>0x7fffffffu||height>0x7fffffffu||left+width>0x7fffffffu||top+height>0x7fffffffu)return false;
 rect={std::int64_t(left),std::int64_t(top),std::int64_t(left+width),std::int64_t(top+height)};return true;
}
bool command_depth_compatible(const Surface& color,const Surface& depth){
 if(!depth.handle)return true;
 return depth.renderScale==color.renderScale&&depth.width==color.width&&depth.height==color.height&&
  (depth.format==PixelFormat::Depth16||depth.format==PixelFormat::Depth24||depth.format==PixelFormat::Depth24Stencil8);
}
}
bool Renderer::command_surface_ready(u32 handle)const{
 if(!handle)return true;
 const auto found=surfaces.find(handle);
 return found!=surfaces.end()&&found->second.version!=~0u&&found->second.version==resolve(owner,handle).version;
}
Renderer::DeferredCommand& Renderer::command(){
 if(command_count==commands.size())commands.emplace_back();
 auto& c=commands[command_count++];c.clear=false;c.vertexBytes=c.indexBytes=c.instanceBytes=0;c.rectangles.clear();
 c.effect.draw=c.effect.barrier=false;c.effect.reads.clear();c.effect.writes.clear();c.effect.overwrites.clear();return c;
}
void Renderer::synchronize(){TH15_HOST_SCOPE;flush();drain_commands();}
void Renderer::drain_commands(){TH15_HOST_SCOPE;
 if(!command_count||draining_commands)return;
 draining_commands=true;retained_commands.resize(command_count);deferred::Liveness liveness;
 for(std::size_t i=command_count;i-->0;)retained_commands[i]=liveness.retain(commands[i].effect);
 const auto saved=state;
 for(std::size_t i=0;i<command_count;++i){auto& c=commands[i];
  if(!retained_commands[i]){++pruned_draws;continue;}
  if(c.clear){state=c.state;clear_now(c.flags,c.color,c.depth,c.stencil,c.rectangles.empty()?nullptr:c.rectangles.data(),u32(c.rectangles.size()/4));}
  else {issue_now(c.state,c.topology,c.count,command_storage.data()+c.vertexOffset,c.vertexBytes,c.indexBytes?command_storage.data()+c.indexOffset:nullptr,c.indexFormat,c.instanceBytes?command_storage.data()+c.instanceOffset:nullptr,c.instanceBytes);++drained_draws;}
 }
 state=saved;command_count=command_bytes=0;command_storage.clear();draining_commands=false;
}
void Renderer::issue(const State& s,Topology primitive,u32 count,const void* data,u32 size,const void* index,IndexType format,const void* instance,u32 instanceSize){
 if(!defer_gpu_commands){if(command_count)drain_commands();issue_now(s,primitive,count,data,size,index,format,instance,instanceSize);return;}
 TH15_HOST_SCOPE;
 const auto target_surface=resolve(owner,s.target);QueueRect target_extent,viewport,depth_extent,texture_extent;
 const auto depth_surface=s.depth?resolve(owner,s.depth):Surface{};
 const auto texture_surface=s.texture?resolve(owner,s.texture):Surface{};
 const std::uint64_t index_size=index?std::uint64_t(vertex_count(primitive,count))*(format==IndexType::UInt16?2:4):0;
 const std::uint64_t bytes=std::uint64_t(size)+index_size+instanceSize;
 const bool regular=primitive>=Topology::Triangles&&primitive<=Topology::Fan;
 bool valid=defer_gpu_commands&&!draining_commands&&count&&data&&size&&bytes<=command_byte_limit&&
  (!instanceSize||instance)&&primitive<=Topology::Fan&&command_depth_compatible(target_surface,depth_surface)&&
  // glClear inherits the device dither state. A draw that changes it must
  // execute immediately, even if all of that draw's pixels are later cleared.
  drawState.valid&&drawState.pipeline.dither==s.pipeline.dither&&
  command_surface_ready(s.target)&&command_surface_ready(s.texture)&&
  command_extent(target_surface,target_extent)&&command_viewport(s.viewport,target_surface.renderScale,viewport)&&
  (!s.depth||command_extent(depth_surface,depth_extent))&&(!s.texture||command_extent(texture_surface,texture_extent))&&
  s.texture!=s.target&&format<=IndexType::UInt32&&std::isfinite(s.viewport.min)&&std::isfinite(s.viewport.max);
 if(!valid){drain_commands();issue_now(s,primitive,count,data,size,index,format,instance,instanceSize);return;}
 if(command_count>=command_limit||command_bytes+bytes>command_byte_limit)drain_commands();
 auto& c=command();c.state=s;c.topology=primitive;c.count=count;c.indexFormat=format;
 // One bounded byte arena avoids a second per-command vertex allocation.
 c.vertexOffset=u32(command_bytes);c.vertexBytes=size;
 c.indexOffset=c.vertexOffset+size;c.indexBytes=u32(index_size);
 c.instanceOffset=c.indexOffset+c.indexBytes;c.instanceBytes=instanceSize;
 command_storage.resize(command_bytes+std::size_t(bytes));
 std::memcpy(command_storage.data()+c.vertexOffset,data,size);
 if(index_size)std::memcpy(command_storage.data()+c.indexOffset,index,std::size_t(index_size));
 if(instanceSize)std::memcpy(command_storage.data()+c.instanceOffset,instance,instanceSize);
 command_bytes+=std::size_t(bytes);++queued_draws;c.effect.draw=true;
 // Triangle clipping stays within the viewport. Points/lines can extend past
 // its edge, so use the entire attachment as their conservative footprint.
 const auto color_bounds=regular?command_clip(viewport,target_extent):target_extent;
 const auto depth_bounds=regular?command_clip(viewport,depth_extent):depth_extent;
 if(s.pipeline.colorMask){
  c.effect.writes.push_back({command_plane(s.target,0),color_bounds});
  // Retain all possible destination-color dependencies, even opaque draws.
  c.effect.reads.push_back({command_plane(s.target,0),color_bounds});
 }
 if(s.depth&&s.pipeline.depthTest){
  c.effect.reads.push_back({command_plane(s.depth,1),depth_bounds});
  if(s.pipeline.depthWrite)c.effect.writes.push_back({command_plane(s.depth,1),depth_bounds});
 }
 if(s.texture)c.effect.reads.push_back({command_plane(s.texture,0),texture_extent});
}
void Renderer::clear(u32 flags,u32 color,float depth,u32 stencil,const i32* rects,u32 count){
 if(!defer_gpu_commands){flush();if(command_count)drain_commands();clear_now(flags,color,depth,stencil,rects,count);return;}
 TH15_HOST_SCOPE;
 flush();const auto target_surface=resolve(owner,state.target);QueueRect target_extent,depth_extent,viewport;
 const auto depth_surface=state.depth?resolve(owner,state.depth):Surface{};
 const bool explicit_rects=rects!=nullptr;
 bool valid=defer_gpu_commands&&!draining_commands&&command_surface_ready(state.target)&&command_extent(target_surface,target_extent)&&
  command_viewport(state.viewport,target_surface.renderScale,viewport)&&(!state.depth||command_extent(depth_surface,depth_extent))&&
  (!explicit_rects||count<=64)&&std::isfinite(depth)&&command_depth_compatible(target_surface,depth_surface);
 for(u32 i=0;valid&&explicit_rects&&i<count;++i){const auto* p=rects+4*i;
  const auto w=(std::int64_t(p[2])-p[0])*target_surface.renderScale,h=(std::int64_t(p[3])-p[1])*target_surface.renderScale;
  const auto x=std::int64_t(p[0])*target_surface.renderScale,y=std::int64_t(p[1])*target_surface.renderScale;
  if(w<0||h<0||w>0x7fffffff||h>0x7fffffff||x<(-2147483647LL-1)||x>2147483647LL||y<(-2147483647LL-1)||y>2147483647LL)valid=false;
 }
 if(!valid){drain_commands();clear_now(flags,color,depth,stencil,rects,count);return;}
 if(command_count>=command_limit)drain_commands();
 auto& c=command();c.clear=true;c.state=state;c.flags=flags;c.color=color;c.depth=depth;c.stencil=stencil;
 if(explicit_rects&&count)c.rectangles.assign(rects,rects+4*count);
 // An explicit zero-rectangle clear performs no clear but still sets device
 // masks. Preserve that rare case immediately rather than substituting viewport.
 if(explicit_rects&&!count){--command_count;drain_commands();clear_now(flags,color,depth,stencil,rects,count);return;}
 const u32 regions=explicit_rects?count:1;
 for(u32 i=0;i<regions;++i){QueueRect r=viewport;
  if(explicit_rects){const auto* p=rects+i*4;const std::int64_t scale=target_surface.renderScale;r={p[0]*scale,p[1]*scale,p[2]*scale,p[3]*scale};}
  if(flags&1)c.effect.overwrites.push_back({command_plane(state.target,0),command_clip(r,target_extent)});
  if(state.depth&&(flags&2))c.effect.overwrites.push_back({command_plane(state.depth,1),command_clip(r,depth_extent)});
  // Current regular draws have no stencil effects. Keep all stencil clears in
  // execution order without using them to remove any draw.
 }
}
