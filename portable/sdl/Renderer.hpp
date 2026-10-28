// GLES 3 port of the drawing half of upstream src/renderer.mjs: one
// interleaved vertex batch (XY, UV, RGBA, clip W, fog), the seven native
// blend modes, scissor clips and the stage-surface copy. Everything is drawn
// in the original 640x480 coordinates into an offscreen target `scale` times
// that size, then fitted to the screen with black bars.
#pragma once
#include "Anm.hpp"
#include <array>
#include <memory>

namespace th12host {
struct Texture {u32 name=0;float w=1,h=1;bool flip_y=false,repeat=true,available=true;i32 address_flags=-1;};
struct Color {float r=1,g=1,b=1,a=1;};
inline Color argb_color(u32 argb,float alpha=1){return {((argb>>16)&255)/255.f,((argb>>8)&255)/255.f,(argb&255)/255.f,((argb>>24)&255)/255.f*alpha};}
struct Vertex {float x,y,u,v,r,g,b,a,depth,fog;};
using Corners=std::array<std::array<float,2>,4>;

// Upstream quadCorners / nativeQuadCorners / nativePixel.
float native_pixel(float n);
Corners quad_corners(float x,float y,float w,float h,float angle,i32 anchor_x,i32 anchor_y,bool pixel);
Corners native_quad_corners(float x,float y,float w,float h,float angle,i32 anchor_x,i32 anchor_y,i32 mode,const float* offset);

class Renderer {
public:
    enum Primitive {Triangles,Lines,LineStrip};
    bool initialize(u32 scale,std::string& error);
    void shutdown();
    u32 scale()const{return scale_;}
    // Textures
    void upload(AnmFile& file);       // every entry with pixels; frees the CPU copy
    bool upload_next(AnmFile& file);  // one such entry (false: none left)
    static void release(AnmFile& file);
    Texture texture_for(const AnmEntry& e)const{return {e.texture,float(e.texture_width),float(e.texture_height),false,true,e.texture!=0,i32(e.address_flags)};}
    u32 create_texture(u32 w,u32 h,const u8* rgba,bool clamp);
    void delete_texture(u32 name);
    const Texture& solid()const{return solid_;}
    // Frame
    void begin(Color clear={.035f,.035f,.055f,1});
    void flush();
    void material(i32 blend);
    void bind(const Texture& t,bool point,i32 address_flags=-1);
    void primitive(Primitive p);
    void push(const Vertex& v);
    void quad(const Corners& c,float u0,float u1,float v0,float v1,bool flip_y,const Color& color,float ox=0,float oy=0);
    void fog_color(float r,float g,float b);
    // Clip 0 = full 640x480, 1 = playfield 384x448 at (32,16), 2 = stage 422x480 at (13,0).
    void clip(i32 mode);
    void scissor(i32 x,i32 y,i32 w,i32 h);
    void clear(Color c);
    // Upstream region(): one textured sprite quad.
    struct RegionOptions {i32 anchor_x=0,anchor_y=0,mode=-1;bool point=false,pixel=false;float u=0,v=0,uv_scale_x=1,uv_scale_y=1;const float* shake=nullptr;};
    void region(const Texture& t,float sx,float sy,float sw,float sh,float x,float y,float w,float h,float angle,float alpha,const Color& tint,i32 blend,const RegionOptions& o);
    void solid_triangles(const Vertex* v,const u32* indices,u32 count,i32 blend);
    // Stage surface (text.anm entry 2) and pause capture (text.anm entry 1).
    Texture& stage_surface(){return surface_;}
    void copy_stage_surface();
    Texture& pause_surface(){return pause_;}
    void capture_pause_field();
    // Final output.
    void present(i32 window_w,i32 window_h);
    const u8* read_pixels(); // 640*scale x 480*scale RGBA, top row first
    u32 draws=0,vertices=0; // this frame's draw calls and vertices
private:
    u32 scale_=2,program=0,vbo=0,vao=0,target=0,target_texture=0,pause_target=0;i32 fog_uniform=-1;
    size_t stream_capacity=0,stream_used=0; // bytes: one orphaned vertex buffer per frame, appended to by each draw
    Texture solid_,surface_,pause_;std::vector<Vertex> batch;std::vector<u8> pixels;
    i32 blend_=-2;u32 current=0;bool point_=false;bool wrap_u=true,wrap_v=true;Primitive primitive_=Triangles;bool surface_ready=false;
    void texture_parameters(u32 name,bool point,bool wu,bool wv);
    struct TexState {bool point,wu,wv;};std::unordered_map<u32,TexState> states;
};
}
