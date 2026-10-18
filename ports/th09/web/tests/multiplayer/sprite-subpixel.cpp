#include "../../cpp/game/AnmRenderer.hpp"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
using namespace th09;
struct Backend:ZunGraphics {
    std::vector<SpriteVertex> submitted;
    void viewport(const Viewport&)override{}
    void transform(MatrixKind,const Matrix4&)override{}
    void clear(bool,bool,u32,float)override{}
    void draw(const PipelineState&,u32,Topology,VertexLayout,const void* data,u32 n)override {
        const auto* p=static_cast<const SpriteVertex*>(data);submitted.insert(submitted.end(),p,p+n);
    }
};
int main(){
    unsigned checks=0;
    for(unsigned anchor=0;anchor<4;++anchor)for(bool mirror:{false,true})for(float step:{0.f,.125f,.25f,.5f,.75f}){
        Backend backend;AnmRenderer renderer(backend);AnmLoadedSprite sprite{};sprite.texture=1;sprite.uvStart={.125f,.25f};sprite.uvEnd={.625f,.75f};
        AnmVm vm{};vm.loadedSprite=&sprite;vm.flags=3;vm.color1.d3dColor=0xffffffff;vm.anchor=anchor;
        vm.spriteSize={16,20};vm.scale={1.25f,.75f};vm.pos={100+step,120-step,.25f};renderer.shake={.125f,-.25f};
        const auto before=vm;
        assert(renderer.draw_no_rotation(vm,true,mirror)==0);renderer.flush();assert(backend.submitted.size()==6);
        const float x=vm.spriteSize.x*vm.scale.x*.5f,y=vm.spriteSize.y*vm.scale.y*.5f;
        const float left=vm.pos.x-((anchor&1)?0:x)+renderer.shake.x-.5f;
        const float right=vm.pos.x+((anchor&1)?2*x:x)+renderer.shake.x-.5f;
        const float top=vm.pos.y-((anchor&2)?0:y)+renderer.shake.y-.5f;
        const float bottom=vm.pos.y+((anchor&2)?2*y:y)+renderer.shake.y-.5f;
        const auto& vertices=backend.submitted;
        assert(vertices[0].position.x==left&&vertices[1].position.x==right);
        assert(vertices[0].position.y==top&&vertices[2].position.y==bottom);
        assert(vertices[0].position.z==vm.pos.z);
        assert(vertices[0].uv.x==(mirror?sprite.uvEnd.x:sprite.uvStart.x));
        assert(vertices[1].uv.x==(mirror?sprite.uvStart.x:sprite.uvEnd.x));
        assert(std::memcmp(&vm,&before,sizeof(vm))==0);
        // Repeated draw is identical and does not accumulate the half-pixel offset.
        renderer.draw_no_rotation(vm,true,mirror);renderer.flush();assert(backend.submitted.size()==12);
        assert(std::memcmp(vertices.data(),vertices.data()+6,6*sizeof(SpriteVertex))==0);
        assert(std::memcmp(&vm,&before,sizeof(vm))==0);
        // Explicit unrounded geometry retains its existing raster convention.
        renderer.draw_no_rotation(vm,false,mirror);renderer.flush();
        assert(vertices[12].position.x==left+.5f&&vertices[12].position.y==top+.5f);
        ++checks;
    }
    // With integral edges and no shake, the original rounded raster is unchanged.
    Backend backend;AnmRenderer renderer(backend);AnmLoadedSprite sprite{};AnmVm vm{};
    vm.loadedSprite=&sprite;vm.flags=3;vm.color1.d3dColor=0xffffffff;vm.spriteSize={16,16};vm.scale={1,1};vm.pos={100,100,.5f};
    renderer.draw_2d(vm);renderer.flush();assert(backend.submitted[0].position.x==91.5f&&backend.submitted[0].position.y==91.5f);
    std::printf("TH09 subpixel sprites: %u fractional/anchor/mirror cases, repeat-draw/VM preservation and integral raster PASS\n",checks);
}
