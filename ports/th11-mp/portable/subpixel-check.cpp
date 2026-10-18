#include "../th11_web/tests/cpp/render-fixture.hpp"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
using namespace th11;
int main(){
    test::RenderFixture f;
    f.renderer.viewport={0,0,640,480,0,1};
    unsigned cases=0;
    for(unsigned mode=0;mode<4;++mode)for(unsigned ax=0;ax<3;++ax)for(unsigned ay=0;ay<3;++ay)
    for(float size:{8.f,16.f,32.f,64.f})for(float angle:{0.f,.3f}){
        AnmVm vm{};vm.flags=3|(mode<<22)|(ax<<18)|(ay<<20);
        vm.color=0xffffffff;vm.resource=&f.resource;vm.sprite=&f.sprite;
        vm.scale={1,1};vm.sprite_size={size,size};vm.rotation.z=angle;
        vm.position={300.125f,220.25f,0};vm.script_position={3.25f,5.125f,0};
        vm.child_position={.125f,.25f,0};f.renderer.offset={.25f,.125f};
        auto draw=[&](bool continuous){
            f.renderer.invalidate();f.graphics.data.clear();const auto before=vm;
            assert(f.renderer.draw(vm,continuous)==0);f.renderer.flush();
            assert(!std::memcmp(&before,&vm,sizeof(vm)));
            assert(f.graphics.data.size()==6*sizeof(AnmVertex));
            return f.graphics.data;
        };
        const auto baseline=draw(true);assert(draw(true)==baseline);
        vm.script_position.x+=.125f;vm.script_position.y+=.25f;
        const auto shifted=draw(true);
        for(unsigned i=0;i<6;++i){
            AnmVertex a,b;std::memcpy(&a,baseline.data()+i*sizeof a,sizeof a);
            std::memcpy(&b,shifted.data()+i*sizeof b,sizeof b);
            assert(std::fabs((b.position.x-a.position.x)-.125f)<.0001f);
            assert(std::fabs((b.position.y-a.position.y)-.25f)<.0001f);
            assert(a.color==b.color&&a.uv.x==b.uv.x&&a.uv.y==b.uv.y);
        }
        ++cases;
    }
    AnmVm vm{};vm.flags=3;vm.color=~0u;vm.scale={1,1};vm.sprite_size={16,16};
    vm.resource=&f.resource;vm.sprite=&f.sprite;vm.position={100.125f,100.25f,0};
    f.renderer.offset={};
    auto first_x=[&](bool continuous,bool ascii){
        f.renderer.invalidate();f.graphics.data.clear();
        assert((ascii?f.renderer.draw_ascii_sprite(vm):f.renderer.draw(vm,continuous))==0);
        f.renderer.flush();AnmVertex v;std::memcpy(&v,f.graphics.data.data(),sizeof v);return v.position.x;
    };
    assert(first_x(true,false)==91.625f); // fractional origin, retained -0.5
    assert(first_x(false,false)==91.5f);
    assert(first_x(false,true)==91.5f); // text remains pixel aligned
    Vec3 logical[4];assert(anm_quad_positions(vm,logical));assert(logical[0].x==92.f);
    vm.position.x=100.f;assert(first_x(true,false)==first_x(false,false));
    std::printf("TH11 bullet subpixel: %u mode/anchor/size/rotation cases; repeated draw, VM purity, UV/color, half-pixel and ASCII/logical alignment PASS\n",cases);
}
