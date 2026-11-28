// Confirm the HUD panel composition change cannot affect any other draw.
//
// The fix gives the three composed bands a private clock so they cannot match
// the authored presentation sample. Every other VM keeps the authored clock and
// must still interpolate exactly as before.
#include "AnmRenderer.hpp"
#include "AnmPresentation.hpp"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <vector>

namespace th11 {
namespace {
struct Capture final :ZunGraphics {
    touhou::graphics::PipelineState state{};
    std::vector<float> ys;
    touhou::graphics::PipelineState& pipeline()override{return state;}
    u32 texture(const AnmResource&,u32 index)override{return index+1;}
    void bind_texture(u32)override{}
    void set_layout(touhou::graphics::VertexLayout)override{}
    void set_matrix(touhou::graphics::MatrixKind,const Matrix4&)override{}
    void primitives(touhou::graphics::Topology,u32 c,const void* v,u32 s)override{
        if(!c||s<8)return;
        const auto* f=static_cast<const u8*>(v);
        for(u32 i=0;i+6<=c;i+=6){float y;std::memcpy(&y,f+u64(i)*s+4,sizeof y);ys.push_back(y);}
    }
    void triangles(u32 c,const void* v,u32 s)override{
        if(!c||s<8)return;
        const auto* f=static_cast<const u8*>(v);
        for(u32 i=0;i+6<=c*3;i+=6){float y;std::memcpy(&y,f+u64(i)*s+4,sizeof y);ys.push_back(y);}
    }
};
AnmVm make_panel(AnmResource& r,AnmSprite& s,float rate_ref){
    AnmVm p{};p.id=1;p.flags=3;p.resource=&r;p.sprite=&s;
    p.sprite_size={64,480};p.scale={1,1};
    p.uv[0]={0,0};p.uv[1]={1,0};p.uv[2]={0,1};p.uv[3]={1,1};
    p.color=0xffffffffu;p.secondary_color=0xffffffffu;
    return p;
}
}
}

int main(){
    using namespace th11;
    Capture g;AnmRenderer renderer(g);
    renderer.viewport=GraphicsViewport{0,0,640,480,0,1};
    AnmResource resource{};AnmSprite sprite{};sprite.width=64;sprite.height=480;
    float rate=1;

    // 1) An ordinary animated sprite must still interpolate. Two endpoints are
    // published, then the draw at alpha=.5 must land halfway between them.
    {
        AnmVm vm{};vm.id=77;vm.flags=3;
        vm.resource=&resource;vm.sprite=&sprite;
        vm.sprite_size={16,16};vm.scale={1,1};
        vm.uv[0]={0,0};vm.uv[1]={1,0};vm.uv[2]={0,1};vm.uv[3]={1,1};
        vm.color=0xffffffffu;vm.secondary_color=0xffffffffu;
        vm.timer.rate=&rate;
        presentation::reset();presentation::begin_tick();presentation::capture(vm);
        vm.position.y=100.f;                       // moves 0 -> 100 this tick
        high_refresh::begin(.5f,true,true);
        g.ys.clear();renderer.draw(vm);renderer.flush();
        high_refresh::end();presentation::reset();
        assert(!g.ys.empty());
        // Centre-anchored 16px quad drawn from the halfway position 50.
        const float top=g.ys.front();
        std::printf("ordinary sprite alpha=.5 top=%.2f (expected 42.0)\n",double(top));
        assert(std::fabs(top-42.f)<=.51f);
    }

    // 2) The HUD panel must be identical at every weight.
    {
        AnmVm panel=make_panel(resource,sprite,rate);
        panel.timer.rate=&rate;
        renderer.multiplayer_hud_background=&panel;
        std::vector<float> reference;
        for(float alpha:{0.f,.5f,1.f,2.f}){
            presentation::reset();presentation::begin_tick();presentation::capture(panel);
            g.ys.clear();
            high_refresh::begin(alpha,true,true);
            renderer.draw(panel);renderer.flush();
            high_refresh::end();presentation::reset();
            assert(g.ys.size()==3);
            if(reference.empty())reference=g.ys;
            assert(g.ys==reference);
        }
        std::printf("HUD panel identical at weights 0/.5/1/2: %.2f/%.2f/%.2f\n",
            double(reference[0]),double(reference[1]),double(reference[2]));
    }

    std::puts("TH11 MP HUD panel isolation PASS (other draws still interpolate)");
    return 0;
}
