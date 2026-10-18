#include "../th08_web/cpp/game/AnmRenderer.hpp"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
using namespace th08;
struct Backend final:SpriteBackend {
    PipelineState state;unsigned submitted=0;
    PipelineState& pipeline() override{return state;}
    void bind_texture(u32) override{}
    void destination_blend(BlendParameter) override{}
    void write_depth(bool) override{}
    void triangles(const SpriteVertex*,u32 count) override{submitted+=count;}
    void transform(MatrixParameter,const Matrix4&) override{}
    void set_viewport(const Viewport&) override{}
    void texture_factor(u32) override{}
    void vertex_format(VertexFormat) override{}
    void draw(Primitive,VertexFormat,const void*,u32) override{}
    void clear_target(u32,u32,float,u32) override{}
};
static void near(float a,float b){assert(std::fabs(a-b)<.0001f);}
int main(){
    arithmetic_mode(Precision::Single,Rounding::NearestEven);
    Backend backend;AnmRenderer renderer(backend);AnmLoadedSprite sprite{};sprite.uvEnd={1,1};
    AnmVm vm{};vm.visible=vm.flag1=true;vm.color1.d3dColor=-1;vm.loadedSprite=&sprite;vm.spriteSize={16,24};
    unsigned checks=0;
    for(unsigned anchor=0;anchor<4;anchor++)for(float scale:{1.f,1.125f,-1.f})for(unsigned frame=0;frame<64;frame++){
        vm.anchor=anchor;vm.scale={scale,scale};vm.pos={100.f+frame*.125f,120.f+frame*.25f,.375f};
        const auto before=vm;renderer.draw_2d(vm);
        near(renderer.quad[0].pos.x,vm.pos.x-((anchor&1)?0:8*scale)-.5f);
        near(renderer.quad[0].pos.y,vm.pos.y-((anchor&2)?0:12*scale)-.5f);
        near(renderer.quad[1].pos.x-renderer.quad[0].pos.x,16*scale);near(renderer.quad[0].pos.z,.375f);
        assert(std::memcmp(&before,&vm,sizeof(vm))==0);
        const auto first=renderer.quad[0];renderer.draw_2d(vm);near(first.pos.x,renderer.quad[0].pos.x);renderer.flush();++checks;
    }
    vm.rotation.z=.25f;renderer.draw_2d(vm);const auto first=renderer.quad[0];vm.pos.x+=.125f;vm.pos.y+=.25f;renderer.draw_2d(vm);
    near(renderer.quad[0].pos.x-first.pos.x,.125f);near(renderer.quad[0].pos.y-first.pos.y,.25f);
    assert(backend.submitted==checks*12);
    std::printf("TH08 subpixel renderer: %u cases passed; VM unchanged, depth/size preserved\n",checks+1);
}
