// Right-hand HUD panel background must be a static frame.
//
// The multiplayer HUD artwork carries the title/watermark baked into the right
// frame's sprite. The renderer fills the interior by submitting three copies of
// the same VM: a 16-pixel top border, a stretched interior and a 16-pixel bottom
// border, each displaced by a constant vertical offset.
//
// Those copies keep the original VM's id and clock. When high-refresh
// presentation is active they are therefore matched against the sample
// published for the *original* VM, so the displacement constant is fed through
// the presentation weight instead of being applied once. A constant offset then
// becomes weight * offset and the three strips slide by different amounts on
// every presented frame: the panel visibly twitches.
//
// This contract pins the strips to their authored positions at every weight.
#include "AnmRenderer.hpp"
#include "AnmPresentation.hpp"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <vector>

namespace th11 {
namespace {
// Records the vertical extent of every submitted quad, in submission order.
struct Capture final :ZunGraphics {
    touhou::graphics::PipelineState state{};
    std::vector<float> top, bottom;
    touhou::graphics::PipelineState& pipeline()override{return state;}
    u32 texture(const AnmResource&,u32 index)override{return index+1;}
    void bind_texture(u32)override{}
    void set_layout(touhou::graphics::VertexLayout)override{}
    void set_matrix(touhou::graphics::MatrixKind,const Matrix4&)override{}
    void primitives(touhou::graphics::Topology,u32 count,const void* vertices,u32 stride)override{
        // primitives() is handed a vertex count.
        record(count,static_cast<const u8*>(vertices),stride);
    }
    void triangles(u32 count,const void* vertices,u32 stride)override{
        // flush() is handed a triangle count; each quad is two triangles.
        record(count*3u,static_cast<const u8*>(vertices),stride);
    }
    void record(u32 count,const u8* first,u32 stride){
        if(!count||stride<8)return;
        constexpr u32 vertices_per_quad=6;
        for(u32 base=0;base+vertices_per_quad<=count;base+=vertices_per_quad){
            auto y_of=[&](u32 i){float y;std::memcpy(&y,first+u64(i)*stride+4,sizeof y);return y;};
            float low=y_of(base),high=low;
            for(u32 i=1;i<vertices_per_quad;++i){const float y=y_of(base+i);low=std::min(low,y);high=std::max(high,y);}
            top.push_back(low);bottom.push_back(high);
        }
    }
};
}
}

int main(){
    using namespace th11;
    Capture graphics;
    AnmRenderer renderer(graphics);
    renderer.viewport=GraphicsViewport{0,0,640,480,0,1};

    AnmResource resource{};
    AnmSprite sprite{};
    sprite.width=64;sprite.height=480;
    AnmVm panel{};
    panel.id=1;panel.flags=3;
    panel.resource=&resource;panel.sprite=&sprite;
    panel.sprite_size={64,480};panel.scale={1,1};
    panel.position={};panel.script_position={};panel.child_position={};
    panel.uv[0]={0,0};panel.uv[1]={1,0};panel.uv[2]={0,1};panel.uv[3]={1,1};
    panel.color=0xffffffffu;panel.secondary_color=0xffffffffu;
    float rate=1;panel.timer.rate=&rate;

    renderer.multiplayer_hud_background=&panel;

    // Authored placements of the three strips. The source VM is anchored at its
    // centre, so each strip's submitted top edge is (offset - height/2) shifted
    // by the D3D half-pixel convention. Record those expected values directly.
    constexpr float expected_top=-8.f;          // 0 - 16/2
    constexpr float expected_interior=-208.f;   // 16 - 448/2
    constexpr float expected_bottom=456.f;      // 464 - 16/2
    constexpr float tolerance=.51f;             // half-pixel snapping only

    // Present the same static logical state at the full range of display
    // weights. High-refresh uses [0,2]; 1 is the authoritative endpoint.
    // Every weight must produce the identical submission: a panel that shifts
    // with the presentation weight is the reported twitch.
    unsigned samples=0,failures=0;
    std::vector<float> reference;
    for(float alpha:{0.f,.25f,.5f,.75f,1.f,1.25f,1.5f,2.f}){
        presentation::reset();
        presentation::begin_tick();
        presentation::capture(panel);
        graphics.top.clear();graphics.bottom.clear();
        high_refresh::begin(alpha,true,true);
        const int result=renderer.draw(panel);
        renderer.flush();
        high_refresh::end();
        presentation::reset();
        assert(result!=-2);
        if(graphics.top.size()!=3){
            std::printf("alpha=%.2f submitted %zu quads (expected 3)\n",double(alpha),graphics.top.size());
            ++failures;++samples;continue;
        }
        const float observed_top=graphics.top[0];
        const float observed_interior=graphics.top[1];
        const float observed_bottom=graphics.top[2];
        // A drifting interior is the defect: border positions were always
        // stable because their authored offset is zero.
        const bool placed=std::fabs(observed_top-expected_top)<=tolerance&&
                          std::fabs(observed_interior-expected_interior)<=tolerance&&
                          std::fabs(observed_bottom-expected_bottom)<=tolerance;
        // The interior strip drifting against its borders is the defect; the
        // borders never moved because their authored offset is zero.
        const std::vector<float> current{observed_top,observed_interior,observed_bottom};
        if(reference.empty())reference=current;
        const bool identical=current==reference;
        if(!placed||!identical)++failures;
        std::printf("alpha=%.2f strips=%.2f/%.2f/%.2f expected=%.0f/%.0f/%.0f %s\n",
            double(alpha),double(observed_top),double(observed_interior),double(observed_bottom),
            double(expected_top),double(expected_interior),double(expected_bottom),
            (placed&&identical)?"ok":"DRIFT");
        ++samples;
    }
    std::printf("TH11 MP HUD panel background: %u samples, %u drifted\n",samples,failures);
    return failures?1:0;
}
