#pragma once
#include "../../cpp/game/AnmRenderer.hpp"
namespace renderer_test {
using namespace th09;
struct Fixture:ZunGraphics {
    AnmRenderer renderer;AnmVm vm;AnmLoadedSprite sprite{};std::vector<u8> submitted;u32 calls=0,last_texture=0;PipelineState last_state;Matrix4 matrices[4]{};
    Fixture():renderer(*this){std::memset(&vm,0,sizeof(vm));vm.loadedSprite=&sprite;}
    void viewport(const Viewport&)override{}
    void transform(MatrixKind kind,const Matrix4& m)override{matrices[u32(kind)]=m;}
    void clear(bool,bool,u32,float)override{}
    void draw(const PipelineState& s,u32 t,Topology,VertexLayout l,const void* p,u32 n)override{++calls;last_texture=t;last_state=s;const auto* b=static_cast<const u8*>(p);submitted.insert(submitted.end(),b,b+n*stride(l));}
};
}
