#pragma once
#include "../../cpp/game/AnmRenderer.hpp"
namespace th11::test {
struct RecordingGraphics final:ZunGraphics {
    touhou::graphics::PipelineState state{};std::vector<u8> data;
    u32 calls=0,texture_id=0,layout=0;
    Matrix4 world{},uv{};u32 topology=0;
    struct TargetEvent {u32 kind,target,color,count;GraphicsViewport rectangle;};
    std::vector<TargetEvent> target_events;u32 target=0;GraphicsViewport viewport;
    struct Submission {u32 target,texture,start,size,topology,count,stride;};
    std::vector<Submission> submissions;
    struct Snapshot {u32 depth_write,depth_compare,fog,layout,factor;Matrix4 world,uv;};
    std::vector<Snapshot> snapshots;
    void snapshot(){snapshots.push_back({state.depthWrite,u32(state.depthCompare),state.fog,layout,state.textureFactor,world,uv});}
    bool select_target(const AnmResource* file,u32 index)override{target=file?index+1:0;viewport={};target_events.push_back({0,target,0,0,{}});return true;}
    bool clear_target(u32 color,const GraphicsViewport* rect)override{target_events.push_back({1,target,color,rect?1u:0u,rect?*rect:viewport});return true;}
    void set_viewport(const GraphicsViewport& v)override{viewport=v;}
    touhou::graphics::PipelineState& pipeline()override{return state;}
    u32 texture(const AnmResource&,u32 index)override{return index+1;}
    void bind_texture(u32 handle)override{texture_id=handle;}
    void set_layout(touhou::graphics::VertexLayout v)override{layout=u32(v);}
    void set_matrix(touhou::graphics::MatrixKind kind,const Matrix4& m)override{if(kind==touhou::graphics::MatrixKind::World)world=m;if(kind==touhou::graphics::MatrixKind::Texture)uv=m;}
    void triangles(u32 n,const void* v,u32 stride)override{++calls;snapshot();submissions.push_back({target,texture_id,u32(data.size()),n*3*stride,3,n,stride});const auto* b=static_cast<const u8*>(v);data.insert(data.end(),b,b+n*3*stride);}
    void primitives(touhou::graphics::Topology t,u32 n,const void* v,u32 stride)override{++calls;snapshot();topology=u32(t);const u32 size=touhou::graphics::vertex_count(t,n)*stride;submissions.push_back({target,texture_id,u32(data.size()),size,u32(t),n,stride});const auto* b=static_cast<const u8*>(v);data.insert(data.end(),b,b+size);}
};
struct RenderFixture {RecordingGraphics graphics;AnmRenderer renderer{graphics};AnmResource resource;AnmSprite sprite;};
}
