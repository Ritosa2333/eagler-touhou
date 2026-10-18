#include "vertex_buffer.hpp"
#include <cstring>
#include <xmmintrin.h>
namespace th20::source::sprite {
WorldVertex24 world_quad[4]={{0,0,0,0xffffffffu,0,0},{0,0,0,0xffffffffu,0,0},{0,0,0,0xffffffffu,0,0},{0,0,0,0xffffffffu,0,0}};
namespace {
float add(float a,float b){return _mm_cvtss_f32(_mm_add_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
}
void initialize_corner_buffer(Controller& c,Device& device){
    c.corners[0]={-128.f,-128.f,0,0,0};c.corners[1]={128.f,-128.f,0,1,0};
    c.corners[2]={-128.f,128.f,0,0,1};c.corners[3]={128.f,128.f,0,1,1};
    for(unsigned i=0;i<4;++i){auto& dst=world_quad[i];const auto& src=c.corners[i];dst.x=src.x;dst.y=src.y;dst.z=src.z;dst.u=src.u;dst.v=src.v;}
#ifdef TH_SDL3
    TexturedCorner20 quads[36];
    auto copy=[&](unsigned index){std::memcpy(quads+index*4,c.corners,sizeof(c.corners));};
#else
    // Original ignores CreateVertexBuffer/Lock HRESULTs. A failed allocation
    // leaving a null buffer is outside the original non-crashing domain.
    device.CreateVertexBuffer(0x2d0,0,D3DFVF_XYZ|D3DFVF_TEX1,D3DPOOL_MANAGED,&c.corner_buffer,nullptr);
    void* storage=nullptr;c.corner_buffer->Lock(0,0,&storage,0);
    if(!storage)return;
    auto copy=[&](unsigned index){std::memcpy(static_cast<std::uint8_t*>(storage)+index*sizeof(c.corners),c.corners,sizeof(c.corners));};
#endif
    auto move_y=[&](bool backwards){for(auto& vertex:c.corners)vertex.y=backwards?sub(vertex.y,256.f):add(vertex.y,128.f);};
    auto move_x=[&](bool backwards){for(auto& vertex:c.corners)vertex.x=backwards?sub(vertex.x,256.f):add(vertex.x,128.f);};
    copy(0);move_y(false);copy(3);move_y(true);copy(6);
    move_x(false);move_y(false);copy(1);move_y(false);copy(4);move_y(true);copy(7);
    move_x(true);move_y(false);copy(2);move_y(false);copy(5);move_y(true);copy(8);
#ifdef TH_SDL3
    device.initialize_corner_buffer(quads,sizeof(quads),sizeof(TexturedCorner20));
#else
    c.corner_buffer->Unlock();device.SetStreamSource(0,c.corner_buffer,0,sizeof(TexturedCorner20));
#endif
}
}
