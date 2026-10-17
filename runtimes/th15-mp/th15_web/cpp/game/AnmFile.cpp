#include "AnmFile.hpp"
#include "AnmOverlay.hpp"
namespace th15 {
namespace {
template<class T>void put(u8* p,u32 at,const T& v){std::memcpy(p+at,&v,sizeof v);}
template<class T>void get(const u8* p,u32 at,T& v){std::memcpy(&v,p+at,sizeof v);}
u32 word(const u8* p,u32 at){u32 v;get(p,at,v);return v;}
}
static_assert(sizeof(AnmInterpolators)==0x31c);
bool AnmFile::write(const AnmVm& v,const AnmManager& m,Block& out,bool allow_unbound,bool initialized_unbound){
    error.clear();out.fill(0);auto* b=out.data();const i32 bank=m.resource_id(v.resource);
    if(allow_unbound&&!initialized_unbound&&!v.resource&&v.instruction_offset<0){put(b,0x2c,i32(-1));put(b,0x34,i32(-1));put(b,0x54c,v.timer);put(b,0x560,v.age);return true;}
    const bool unbound=allow_unbound&&initialized_unbound&&!v.resource;
    if(!unbound&&(bank<0||!v.script||v.source_script<0)){error="Checkpoint animation resource is unavailable";return false;}
    put(b,0,v.saved_timer);put(b,0x14,v.saved_offset);
    put(b,0x18,v.visual.flags);put(b,0x1c,v.visual.render_flags);put(b,0x24,v.visual.layer);put(b,0x28,unbound?0:bank);
    const i32 sprite=v.visual.sprite;
    put(b,0x2c,sprite);put(b,0x30,unbound?0:v.source_script);put(b,0x34,v.instruction_offset);
    put(b,0x38,v.variables.position);put(b,0x44,v.variables.rotation);put(b,0x50,v.visual.angular_velocity);
    put(b,0x5c,v.visual.scale);put(b,0x64,v.visual.secondary_scale);put(b,0x6c,v.visual.scale_velocity);
    put(b,0x74,v.visual.uv_scale);put(b,0x7c,v.visual.sprite_size);put(b,0x84,v.visual.uv_offset);put(b,0x8c,v.visual.size);
    put(b,0x98,v.interpolators);put(b,0x3b4,v.visual.uv);put(b,0x3d4,v.visual.uv_velocity);
    put(b,0x3dc,v.visual.sprite_matrix);put(b,0x41c,v.visual.transform_matrix);put(b,0x45c,v.visual.uv_matrix);
    put(b,0x49c,v.pending_interrupt);put(b,0x4a0,v.visual.sprite_frame);
    const i16 source=unbound?0:i16(v.source_script);put(b,0x4a8,source);
    std::memcpy(b+0x4ac,&v.variables,64);put(b,0x4ec,v.visual.child_anchor);put(b,0x4f8,v.visual.quad);
    put(b,0x538,v.visual.color);put(b,0x53c,v.visual.secondary_color);put(b,0x540,v.visual.inherited_color);
    put(b,0x534,v.ordering);put(b,0x54c,v.timer);put(b,0x560,v.age);put(b,0x5c0,v.slowdown);
    put(b,0x5c4,v.geometry.allocation_bytes?u32(1):u32(0));put(b,0x5c8,v.geometry.allocation_bytes);
    put(b,0x5d0,v.geometry.overlay?u32(1):v.geometry.gather?u32(2):v.geometry.trail?u32(3):v.geometry.distortion?u32(4):u32(0));put(b,0x5e4,v.geometry.gather?u32(1):u32(0));put(b,0x5ec,v.visual.translation);
    return true;
}
bool AnmFile::read(const u8* b,u32 size,AnmVm& v,AnmManager& manager,bool allow_unbound,bool initialized_unbound){
    error.clear();if(!b||size<0x608){error="Truncated checkpoint animation record";return false;}
    const i32 bank=signed_bits(word(b,0x28)),script=signed_bits(word(b,0x30)),offset=signed_bits(word(b,0x34));
    auto* resource=manager.resource(manager.checkpoint_bank(bank));
    const bool unbound=allow_unbound&&(initialized_unbound||(offset<0&&bank==0&&script==0&&word(b,0x18)==0));
    if(!unbound){if(!resource||script<0||u32(script)>=resource->scripts.size()){error="Checkpoint animation resource/script is unavailable";return false;}const auto& instructions=resource->scripts[u32(script)].bytes;if(offset>=0&&(u32(offset)>instructions.size()||instructions.size()-u32(offset)<8)){error="Checkpoint animation instruction is outside its script";return false;}}
    AnmVm next;next.environment=&manager.scene_environment();next.object_host=&manager;
    if(!unbound&&!next.bind(*resource,u32(script))){error="Checkpoint animation binding failed";return false;}
    const i32 sprite=signed_bits(word(b,0x2c));const auto* fallback=manager.scene_environment().fallback_sprite_resource;
    bool use_fallback=false;if(!unbound&&sprite==258&&fallback&&fallback!=resource&&fallback->sprites.size()>258){Vec2 dimensions;get(b,0x7c,dimensions);const auto& f=fallback->sprites[258];use_fallback=dimensions.x==f.width&&dimensions.y==f.height&&(u32(sprite)>=resource->sprites.size()||resource->sprites[u32(sprite)].width!=dimensions.x||resource->sprites[u32(sprite)].height!=dimensions.y);}
    if(!unbound&&(use_fallback||sprite<0||u32(sprite)<resource->sprites.size())&&!next.select_sprite(use_fallback?-1:sprite)){error="Checkpoint animation sprite is unavailable";return false;}
    if(unbound){next.source_script=-1;next.visual.sprite=sprite;}
    get(b,0,next.saved_timer);get(b,0x14,next.saved_offset);get(b,0x18,next.visual.flags);get(b,0x1c,next.visual.render_flags);get(b,0x24,next.visual.layer);
    next.instruction_offset=offset;next.visible=next.visual.visible();
    get(b,0x38,next.variables.position);get(b,0x44,next.variables.rotation);get(b,0x50,next.visual.angular_velocity);
    get(b,0x5c,next.visual.scale);get(b,0x64,next.visual.secondary_scale);get(b,0x6c,next.visual.scale_velocity);
    get(b,0x74,next.visual.uv_scale);get(b,0x7c,next.visual.sprite_size);get(b,0x84,next.visual.uv_offset);get(b,0x8c,next.visual.size);
    get(b,0x98,next.interpolators);get(b,0x3b4,next.visual.uv);get(b,0x3d4,next.visual.uv_velocity);
    get(b,0x3dc,next.visual.sprite_matrix);get(b,0x41c,next.visual.transform_matrix);get(b,0x45c,next.visual.uv_matrix);
    get(b,0x49c,next.pending_interrupt);get(b,0x4a0,next.visual.sprite_frame);
    std::memcpy(&next.variables,b+0x4ac,64);get(b,0x4ec,next.visual.child_anchor);get(b,0x4f8,next.visual.quad);
    get(b,0x538,next.visual.color);get(b,0x53c,next.visual.secondary_color);get(b,0x540,next.visual.inherited_color);
    get(b,0x534,next.ordering);get(b,0x54c,next.timer);get(b,0x560,next.age);get(b,0x5c0,next.slowdown);get(b,0x5ec,next.visual.translation);
    // Geometry and child records are read by the owning module immediately
    // after this fixed block; no stored host pointer is attached here.
    next.geometry.allocation_bytes=word(b,0x5c8);v=std::move(next);return true;
}
bool AnmFile::write_geometry(const AnmVm& v,const AnmManager& manager,std::vector<u8>& output,const TreeWriter& tree){
    error.clear();const auto& g=v.geometry;const u32 size=g.allocation_bytes;
    if(!size)return true;if(size>16*1024*1024){error="Checkpoint animation geometry exceeds limit";return false;}
    const size_t at=output.size();output.resize(at+size,0);
    auto append=[&](const void* data,size_t bytes)->bool{if(bytes!=size){error="Checkpoint animation geometry length differs from its type";return false;}std::memcpy(output.data()+at,data,bytes);return true;};
    if(g.gather){
        if(!append(g.gather.get(),sizeof(AnmGatherEffect)))return false;
        // The original uses 0xff presence markers followed by each complete
        // particle animation tree in fixed handle order.
        for(u32 i=0;i<200;i++)put(output.data()+at,i*4,g.gather->handles[i]?u32(0xff):u32(0));
        for(const u32 h:g.gather->handles)if(h&&(!tree||!tree(h,output))){error="Checkpoint gathering animation tree unavailable";return false;}
        return true;
    }
    if(g.trail)return append(g.trail.get(),sizeof(AnmTrail));
    if(g.distortion){
        if(size!=1200){error="Checkpoint distortion allocation differs from original";return false;}
        auto* b=output.data()+at;put(b,0,g.distortion->vertices);put(b,0x3a0,g.distortion->radius);put(b,0x424,g.distortion->radial_speed);put(b,0x4a4,g.distortion->uv_speed);return true;
    }
    if(g.overlay){
        if(size!=0x1e30){error="Checkpoint overlay allocation differs from original";return false;}
        for(u32 i=0;i<5;i++){if(!g.overlay->panels[i]||g.overlay->panels[i]->geometry.allocation_bytes){error="Checkpoint embedded overlay geometry is unavailable";return false;}Block block;if(!write(*g.overlay->panels[i],manager,block))return false;std::memcpy(output.data()+at+i*0x608,block.data(),block.size());}
        put(output.data()+at,0x1e28,g.overlay->mode);put(output.data()+at,0x1e2c,g.overlay->frames);return true;
    }
    if(!g.world_vertices.empty())return append(g.world_vertices.data(),g.world_vertices.size()*sizeof(AnmWorldVertex));
    if(!g.screen_vertices.empty())return append(g.screen_vertices.data(),g.screen_vertices.size()*sizeof(AnmGeometryVertex));
    error="Checkpoint animation geometry owner is unavailable";return false;
}
bool AnmFile::read_geometry(const u8* record,const u8* data,u32 size,AnmVm& v,AnmManager& manager,u32& consumed,const TreeReader& tree){
    error.clear();consumed=0;const u32 bytes=word(record,0x5c8),kind=word(record,0x5d0);if(!bytes){v.geometry.clear();return true;}
    if(!data||bytes>size||bytes>16*1024*1024){error="Truncated checkpoint animation geometry";return false;}
    v.geometry.clear();auto& g=v.geometry;g.allocation_bytes=bytes;consumed=bytes;
    if(kind==2||word(record,0x5e4)==1){
        if(bytes!=sizeof(AnmGatherEffect)){error="Invalid checkpoint gathering state size";return false;}
        g.gather=std::make_unique<AnmGatherEffect>();std::memcpy(g.gather.get(),data,bytes);
        for(auto& h:g.gather->handles){if(!h)continue;u32 read=0;const u32 restored=tree?tree(data+consumed,size-consumed,read):0;if(!restored||read>size-consumed){error="Invalid checkpoint gathering animation tree";return false;}h=restored;consumed+=read;}
        return true;
    }
    if(kind==3){if(bytes!=sizeof(AnmTrail)){error="Invalid checkpoint trail size";return false;}g.trail=std::make_unique<AnmTrail>();std::memcpy(g.trail.get(),data,bytes);return true;}
    if(kind==4){if(bytes!=1200){error="Invalid checkpoint distortion size";return false;}g.distortion=std::make_unique<AnmDistortion>();get(data,0,g.distortion->vertices);get(data,0x3a0,g.distortion->radius);get(data,0x424,g.distortion->radial_speed);get(data,0x4a4,g.distortion->uv_speed);return true;}
    if(kind==1){if(bytes!=0x1e30){error="Invalid checkpoint overlay size";return false;}g.overlay=std::make_unique<AnmOverlay>();for(u32 i=0;i<5;i++){auto p=std::make_unique<AnmVm>();if(!read(data+i*0x608,0x608,*p,manager)||p->geometry.allocation_bytes)return false;g.overlay->panels[i]=std::move(p);}get(data,0x1e28,g.overlay->mode);get(data,0x1e2c,g.overlay->frames);return true;}
    if(kind){error="Unknown checkpoint animation geometry callback";return false;}
    const u32 mode=v.visual.draw_mode();const bool world=mode==15||mode==24||mode==25;
    if(world){if(bytes%sizeof(AnmWorldVertex)){error="Invalid checkpoint world vertex size";return false;}g.world_vertices.resize(bytes/sizeof(AnmWorldVertex));std::memcpy(g.world_vertices.data(),data,bytes);}
    else{if(bytes%sizeof(AnmGeometryVertex)){error="Invalid checkpoint screen vertex size";return false;}g.screen_vertices.resize(bytes/sizeof(AnmGeometryVertex));std::memcpy(g.screen_vertices.data(),data,bytes);}
    return true;
}

}
