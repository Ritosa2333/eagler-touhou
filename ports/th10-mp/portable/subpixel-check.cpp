#include "../th10_web/cpp/game/AnmRenderer.hpp"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <memory>
using namespace th10;
struct Backend final:AnmRenderEnvironment {
    PipelineState state; AnmVertex vertices[4]{}; RenderViewport view{0,0,640,480};
    Backend(){quad=vertices;viewport=&view;}
    PipelineState& pipeline() override{return state;}
    void set_texture(void*) override{}
    void vertex_format(LayoutParameter) override{}
    void draw_triangles(TopologyParameter,u32,const void*,u32) override{}
    void set_transform(MatrixParameter,const Matrix4&) override{}
    void stream_source(void*,u32) override{}
    void draw_buffer(TopologyParameter,u32,u32) override{}
    i32 special_draw(AnmManager&,AnmVm&,u32) override{return 0;}
};
static void near(float a,float b){assert(std::fabs(a-b)<.0001f);}
int main(){
    arithmetic_mode(Precision::Single,Rounding::NearestEven);
    auto manager=std::make_unique<AnmManager>();Backend backend;AnmRenderer renderer{*manager,backend};
    AnmSprite sprite{};sprite.u1=sprite.v1=1;
    AnmVm vm{};vm.flags=3;vm.color=0xffffffff;vm.sprite=&sprite;
    vm.sprite_size={16,24};vm.scale={1,1};vm.child_position.z=.125f;vm.position.z=.25f;
    unsigned checks=0;
    for(unsigned anchor=0;anchor<3;anchor++)for(float scale:{1.f,1.125f,-1.f})for(unsigned frame=0;frame<64;frame++){
        vm.flags=3|(anchor<<18)|(anchor<<20);vm.scale={scale,scale};
        vm.position.x=100.f+frame*.125f;vm.position.y=120.f+frame*.25f;
        const auto before=vm;renderer.begin_frame();renderer.draw(vm);
        near(backend.quad[0].position.x,vm.position.x-16*scale*(anchor==0?.5f:anchor==1?0.f:1.f)-.5f);
        near(backend.quad[0].position.y,vm.position.y-24*scale*(anchor==0?.5f:anchor==1?0.f:1.f)-.5f);
        near(backend.quad[1].position.x-backend.quad[0].position.x,16*scale);
        near(backend.quad[0].position.z,.375f);
        assert(std::memcmp(&before,&vm,sizeof(vm))==0);assert(manager->batch_quads==1);
        const auto first=backend.quad[0];renderer.draw(vm);near(first.position.x,backend.quad[0].position.x);++checks;
    }
    // Existing non-pixel and rotated draw modes must remain independent.
    for(unsigned mode:{1u,2u,3u}){vm.flags=3|(mode<<22);vm.rotation.z=mode==2?0:.25f;
        vm.position={100.125f,120.25f,.25f};renderer.begin_frame();renderer.draw(vm);const auto first=backend.quad[0];
        vm.position.x+=.125f;vm.position.y+=.25f;renderer.draw(vm);
        near(backend.quad[0].position.x-first.position.x,.125f);near(backend.quad[0].position.y-first.position.y,.25f);++checks;}
    std::printf("TH10 subpixel renderer: %u cases passed; VM unchanged, depth/size preserved\n",checks);
}
