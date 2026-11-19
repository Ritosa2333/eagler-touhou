#pragma once
#include "../../cpp/game/GamePresentation.hpp"
#include "game-resources-fixture.hpp"
namespace renderer_test {
using namespace th09;
struct Media:GameMedia {
    bool centered=false;u32 text_calls=0;
    bool render_text(AnmVm&,const char*,u32,u32,bool center=false)override{centered=center;++text_calls;return true;}
    void sound(i32,i32)override{}
    void positioned_sound(i32,float)override{}
    void music(i32)override{}
    void fade_music()override{}
    void encountered(i32)override{}
    void defeated(i32)override{}
    void overlay()override{}
};
struct Fixture:ZunGraphics {
    game_resources_test::Fixture resources;
    Media media;
    GamePresentation presentation;
    AnmRenderer& renderer;
    Topology last_topology=Topology::Triangles;
    u32 clear_calls=0,clear_flags=0;
    AnmVm vm;AnmLoadedSprite sprite{};std::vector<u8> submitted;u32 calls=0,last_texture=0;PipelineState last_state;Matrix4 matrices[4]{};
    Fixture():presentation(*this,resources.resources,media),renderer(presentation.renderer){std::memset(&vm,0,sizeof(vm));vm.loadedSprite=&sprite;}
    void viewport(const Viewport&)override{}
    void transform(MatrixKind kind,const Matrix4& m)override{matrices[u32(kind)]=m;}
    void clear(bool color,bool depth,u32,float)override{++clear_calls;clear_flags=u32(color)|(u32(depth)<<1);}
    void draw(const PipelineState& s,u32 t,Topology primitive,VertexLayout l,const void* p,u32 n)override{++calls;last_topology=primitive;last_texture=t;last_state=s;const auto* b=static_cast<const u8*>(p);submitted.insert(submitted.end(),b,b+n*stride(l));}
};
}
