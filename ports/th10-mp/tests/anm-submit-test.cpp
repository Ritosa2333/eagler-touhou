#include "AnmSubmit.hpp"
#include "PresentationAudit.hpp"
namespace th10 {
namespace { Extended unsigned_number(u32 bits){auto result=Extended::from_int(static_cast<i32>(bits));if(bits&0x80000000)result=result+number(4294967296.0f);return result;} }
i32 original_submit(AnmRenderer& renderer,const AnmVm& vm,u32 flags,bool flip_u){
    auto& manager=renderer.manager;auto& environment=renderer.environment;
    auto* q=environment.quad;for(u32 i=0;i<4;++i){q[i].position.x=Scalar::add(q[i].position.x,manager.draw_offset.x);q[i].position.y=Scalar::add(q[i].position.y,manager.draw_offset.y);}
    // Keep fractional motion for every screen-space sprite. The half-pixel
    // raster convention remains; integer-aligned stationary sprites are unchanged.
    if(flags&1){const auto aligned=[](float x){return (number(x)-number(0.5f)).to_float();};q[0].position.x=q[2].position.x=aligned(q[0].position.x);q[1].position.x=q[3].position.x=aligned(q[1].position.x);q[0].position.y=q[1].position.y=aligned(q[0].position.y);q[2].position.y=q[3].position.y=aligned(q[2].position.y);}
    q[0].uv.x=q[2].uv.x=Scalar::add(flip_u?vm.sprite->u1:vm.sprite->u0,vm.uv_offset.x);q[1].uv.x=q[3].uv.x=Scalar::add(flip_u?vm.sprite->u0:vm.sprite->u1,vm.uv_offset.x);q[0].uv.y=q[1].uv.y=Scalar::add(vm.sprite->v0,vm.uv_offset.y);q[2].uv.y=q[3].uv.y=Scalar::add(vm.sprite->v1,vm.uv_offset.y);
    float max_x=q[0].position.x>q[1].position.x?q[0].position.x:q[1].position.x,max_y=q[0].position.y>q[1].position.y?q[0].position.y:q[1].position.y;
    float min_x=q[0].position.x<q[1].position.x?q[0].position.x:q[1].position.x,min_y=q[0].position.y<q[1].position.y?q[0].position.y:q[1].position.y;
    for(u32 i=2;i<4;++i){if(max_x<q[i].position.x)max_x=q[i].position.x;if(max_y<q[i].position.y)max_y=q[i].position.y;if(q[i].position.x<min_x)min_x=q[i].position.x;if(q[i].position.y<min_y)min_y=q[i].position.y;}
    const auto& viewport=*environment.viewport;if(number(max_x)<unsigned_number(viewport.x)||number(max_y)<unsigned_number(viewport.y)||unsigned_number(viewport.x+viewport.width)<number(min_x)||unsigned_number(viewport.y+viewport.height)<number(min_y))return 0;
    if(manager.current_texture!=vm.sprite->texture){manager.current_texture=vm.sprite->texture;renderer.flush();environment.set_texture(manager.current_texture);}
    if(manager.cached_draw_state[2]!=1){renderer.flush();manager.cached_draw_state[2]=1;}
    if(!(flags&2)){u32 color=vm.flags&0x8000?vm.secondary_color:vm.color;if(manager.tint_enabled){u32 result=0;for(u32 shift=0;shift<32;shift+=8)result|=AnmRenderer::modulate_channel(color>>shift,manager.tint>>shift)<<shift;color=result;}for(u32 i=0;i<4;++i)q[i].color=color;}
    presentation_audit::capture(vm,q,4);renderer.apply_state(vm);return renderer.append(q);
}
}

#include <array>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <vector>
using namespace th10;
extern "C" {extern std::uint_fast8_t softfloat_exceptionFlags,softfloat_detectTininess,softfloat_roundingMode,extF80_roundingPrecision;}
namespace th10 {i32 original_submit(AnmRenderer&,const AnmVm&,u32,bool);}
namespace {
float value(u32 b){float v;std::memcpy(&v,&b,4);return v;}
u32 next(u32& s){s^=s<<13;s^=s>>17;s^=s<<5;return s;}
struct Event {u32 code,arg0,arg1,flags,mode,precision;AnmVertex quad[4];};
struct Result {AnmVm vm;AnmSprite sprite;AnmVertex quad[4];RenderViewport viewport;PipelineState pipeline;Event events[32];u32 count,flags,mode,precision,tiny;int error,returned;};
struct Render final:AnmRenderEnvironment {
    PipelineState state;AnmManager* owner;AnmVm* vm;AnmSprite* sprite;u32 count=0,mutate=0;Event events[32]{};
    void event(u32 code,u32 a=0,u32 b=0){
        if(count>=32)std::abort();auto& e=events[count++];e.code=code;e.arg0=a;e.arg1=b;e.flags=softfloat_exceptionFlags;e.mode=softfloat_roundingMode;e.precision=extF80_roundingPrecision;std::memcpy(e.quad,quad,sizeof(e.quad));
        if(mutate&&code==3){vm->flags^=0x80008010u;vm->color^=0x00f03060u;vm->secondary_color^=0x01704639u;owner->tint^=0x50403020u;quad[3].position.z=value(0x80000000u);sprite->texture=reinterpret_cast<void*>(0x2000u);arithmetic_mode(Precision::Double,Rounding::Down);}
        if(mutate&&code==4){vm->flags^=0x80000020u;vm->secondary_color^=0x04020406u;}
    }
    PipelineState& pipeline()override{event(1);return state;}
    void vertex_format(LayoutParameter f)override{event(2,u32(f));}
    void draw_triangles(TopologyParameter p,u32 count,const void* data,u32 stride)override{event(3,u32(p),count);if(stride!=28||data!=owner->vertex_buffer)std::abort();}
    void set_texture(void* texture)override{event(4,u32(reinterpret_cast<std::uintptr_t>(texture)));}
    void set_transform(MatrixParameter,const Matrix4&)override{std::abort();}
    void stream_source(void*,u32)override{std::abort();}
    void draw_buffer(TopologyParameter,u32,u32)override{std::abort();}
    i32 special_draw(AnmManager&,AnmVm&,u32)override{std::abort();}
};
struct Fixture {AnmManager& manager;AnmVm vm{};AnmSprite sprite{};AnmVertex quad[4]{};RenderViewport viewport{};Render render{};u32 flags=0;bool flip=false;};
void init(Fixture& f,u32 index,u32 precision,u32 rounding,u32 tiny){
    auto& m=f.manager;std::memset(&m,0xa5,sizeof(m));std::memset(&f.vm,0,sizeof(f.vm));std::memset(&f.sprite,0,sizeof(f.sprite));std::memset(f.quad,0x5a,sizeof(f.quad));std::memset(&f.render.state,0,sizeof(f.render.state));std::memset(f.render.events,0,sizeof(f.render.events));
    f.render.quad=f.quad;f.render.viewport=&f.viewport;f.render.owner=&m;f.render.vm=&f.vm;f.render.sprite=&f.sprite;f.render.count=0;f.render.mutate=index%3==0;
    f.vm.sprite=&f.sprite;f.vm.flags=(index%4)<<4|((index%2)<<31)|(index%3==0?0x8000:0);f.vm.color=0xabc08040;f.vm.secondary_color=0x904032ff;
    f.sprite.u0=0;f.sprite.u1=1;f.sprite.v0=.25f;f.sprite.v1=.875f;f.sprite.texture=reinterpret_cast<void*>(0x1000u);
    f.viewport={0,0,640,480};f.vm.uv_offset={0,0};m.draw_offset={0,0};
    for(u32 i=0;i<4;++i){f.quad[i].position={i&1?340.f:120.f,i&2?240.f:100.f,.5f};f.quad[i].reciprocal_w=1;}
    const std::array<u32,34> special={0,0x80000000u,1,0x80000001u,0x007fffffu,0x807fffffu,0x00800000u,0x80800000u,0x00800001u,0x80800001u,0x3f000000u,0xbf000000u,0x3f800000u,0xbf800000u,0x7f7fffffu,0xff7fffffu,0x7f800000u,0xff800000u,0x7fc00000u,0xffc00000u,0x7fc12345u,0xffc12345u,0x7f800001u,0xff800001u,0x33800000u,0xb3800000u,0x4b7fffffu,0x4b800000u,0x4b800001u,0xcb800000u,0x4effffffu,0x4f000000u,0x4f7fffffu,0x4f800000u};
    const float v=value(special[(index/4)%special.size()]);
    switch(index%16){
    case 0:break;case 1:m.draw_offset={v,v};break;case 2:f.vm.uv_offset={v,v};break;
    case 3:for(auto& q:f.quad)q.position.x=q.position.y=v;break;
    case 4:f.sprite.u0=f.sprite.v1=v;break;case 5:f.sprite.u1=f.sprite.v0=v;break;
    case 6:f.quad[index%4].position.x=v;break;case 7:f.quad[index%4].position.y=v;break;
    case 8:f.viewport={0x01000000u,0x01000001u,640,480};break;
    case 9:f.viewport={0xffffff00u,0xfffffff0u,640,480};break;
    case 10:m.draw_offset={value(0x80000000u),value(0x80000000u)};break;
    case 11:f.vm.uv_offset={value(0x80000000u),value(0x80000000u)};break;
    case 12:for(auto& q:f.quad)q.position.x=-1;break;
    case 13:for(auto& q:f.quad)q.position.y=481;break;
    case 14:for(auto& q:f.quad)q.position.x=640;break;
    case 15:{u32 s=index+42;for(auto& q:f.quad){q.position.x=value(next(s));q.position.y=value(next(s));}break;}
    }
    m.current_texture=index%2?f.sprite.texture:nullptr;m.cached_draw_state[0]=(index/2)%4;m.cached_draw_state[2]=(index/3)%2;m.cached_draw_state[6]=(index/5)%2;
    m.batch_quads=index%3?1:0;m.vertex_write=m.vertex_buffer+(m.batch_quads?6:0);m.batch_start=m.vertex_buffer;m.submitted_draws=42;m.flushed_batches=16;m.tint=0x706090c0;m.tint_enabled=index%2;
    f.flags=index%4;f.flip=index%2;arithmetic_mode(static_cast<Precision>(precision),static_cast<Rounding>(rounding));softfloat_detectTininess=tiny;softfloat_exceptionFlags=index%32;errno=index%2?ERANGE:EDOM;
}
Result evaluate(Fixture& f,bool candidate){
    Result r{};AnmRenderer renderer{f.manager,f.render};r.returned=candidate?renderer.submit(f.vm,f.flags,f.flip):original_submit(renderer,f.vm,f.flags,f.flip);
    std::memcpy(&r.vm,&f.vm,sizeof(f.vm));std::memcpy(&r.sprite,&f.sprite,sizeof(f.sprite));std::memcpy(r.quad,f.quad,sizeof(f.quad));r.viewport=f.viewport;std::memcpy(&r.pipeline,&f.render.state,sizeof(r.pipeline));std::memcpy(r.events,f.render.events,sizeof(r.events));r.count=f.render.count;r.flags=softfloat_exceptionFlags;r.mode=softfloat_roundingMode;r.precision=extF80_roundingPrecision;r.tiny=softfloat_detectTininess;r.error=errno;return r;
}
}
int main(){
 auto manager=std::make_unique<AnmManager>();std::vector<unsigned char> expected_manager(sizeof(AnmManager));Fixture fixture{*manager};u32 checks=0;
 for(auto precision:{Precision::Single,Precision::Double,Precision::Extended})for(u32 rounding=0;rounding<4;++rounding)for(u32 tiny=0;tiny<2;++tiny)for(u32 i=0;i<96;++i){
    const u32 index=i+rounding*96+tiny*384+unsigned(precision)*17;init(fixture,index,unsigned(precision),rounding,tiny);const Result expected=evaluate(fixture,false);std::memcpy(expected_manager.data(),manager.get(),sizeof(AnmManager));
    init(fixture,index,unsigned(precision),rounding,tiny);const Result actual=evaluate(fixture,true);
    if(std::memcmp(&expected,&actual,sizeof(actual))||std::memcmp(expected_manager.data(),manager.get(),sizeof(AnmManager))){std::fprintf(stderr,"submit mismatch index%u precision%u rounding%u tiny%u flags%u/%u result%d/%d calls%u/%u\n",index,unsigned(precision),rounding,tiny,expected.flags,actual.flags,expected.returned,actual.returned,expected.count,actual.count);return 1;}++checks;
 }
 std::printf("{\"suite\":\"anm-submit-original-body\",\"comparisons\":%u,\"raw_manager_bytes\":%zu,\"all_manager_vm_sprite_quad_bytes_equal\":true,\"callbacks_pipeline_vertices_equal\":true,\"return_errno_arithmetic_equal\":true,\"passed\":true}\n",checks,sizeof(AnmManager));
}
