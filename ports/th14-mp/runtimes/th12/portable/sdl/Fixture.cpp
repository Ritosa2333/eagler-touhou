#include "Renderer.hpp"
#include "LegacyGraphics.hpp"
#include "AssetPixelFormat.hpp"
#include <memory>
#include <cstdlib>
#include <cstring>
using namespace touhou::sdl;
namespace {
struct Image{Surface s;std::vector<u8> bytes;};std::map<u32,Image> images;std::unique_ptr<Renderer> renderer;
Surface resolve(void*,u32 id){return images.at(id).s;}
u32 next=3;std::vector<u8> expanded;
u32 create(u32 id,u32 w,u32 h,u32 format){auto& im=images[id];u32 bpp=format>=23&&format<=26?2:format==28||format==50?1:4;im.bytes.resize(w*h*bpp);im.s={id,w,h,asset_pixel_format(format),w*bpp,im.bytes.data(),u32(im.bytes.size()),0};return id;}
}
#define EX(name) extern "C" __attribute__((export_name(name)))
EX("allocate") void* allocate(u32 n){return std::calloc(n,1);}EX("deallocate") void deallocate(void* p){std::free(p);}
EX("fixture_create") int fixture_create(int version,u32 w,u32 h,u32 format){images.clear();next=3;SDL_SetHint(SDL_HINT_EMSCRIPTEN_CANVAS_SELECTOR,"#screen");renderer=std::make_unique<Renderer>(version,resolve,nullptr);if(!renderer->initialize())return 0;create(1,w,h,format);create(2,w,h,80);renderer->state.target=1;renderer->state.depth=2;renderer->state.viewport={0,0,w,h,0,1};return 1;}
EX("fixture_texture") u32 fixture_texture(u32 w,u32 h,u32 format){return create(next++,w,h,format);}
EX("fixture_pixels") u8* fixture_pixels(u32 id){return images.at(id).bytes.data();}
EX("fixture_changed") void fixture_changed(u32 id){images.at(id).s.version++;}
EX("fixture_state") void fixture_state(u32 format,u32 texture){const auto v=renderer->state.viewport;renderer->state=State{};renderer->state.layout=legacy::vertices(format);renderer->state.texture=texture;renderer->state.target=1;renderer->state.depth=2;renderer->state.viewport=v;}
EX("fixture_render") void fixture_render(u32 k,u32 v){legacy::render(renderer->pipeline(),k,v);}
EX("fixture_stage") void fixture_stage(u32 k,u32 v){legacy::stage(renderer->pipeline(),k,v);}
EX("fixture_transform") void fixture_transform(u32 k,const void* p){renderer->transform(legacy::matrix(k),p);}
EX("fixture_viewport") void fixture_viewport(const Viewport* p){renderer->viewport(*p);}
EX("fixture_clear") void fixture_clear(u32 flags,u32 color,float depth,u32 stencil,const i32* r,u32 count){renderer->clear(flags,color,depth,stencil,r,count);}
EX("fixture_copy") void fixture_copy(u32 source,const i32* rect,const i32* point){renderer->copy(source,rect,1,point);images.at(1).s.version++;}
EX("fixture_draw") void fixture_draw(u32 primitive,u32 count,const u8* data,u32 stride,const void* indices,u32 format){
 if(indices){u32 n=primitive==4?count*3:count+2;expanded.resize(n*stride);for(u32 i=0;i<n;i++){u32 ix=format==101?static_cast<const uint16_t*>(indices)[i]:static_cast<const u32*>(indices)[i];std::memcpy(expanded.data()+i*stride,data+ix*stride,stride);}data=expanded.data();}renderer->draw(legacy::topology(primitive),count,data,stride);
}
EX("fixture_read") u8* fixture_read(){renderer->flush();renderer->read(1);return images.at(1).bytes.data();}
EX("fixture_present") void fixture_present(){renderer->present(1);}
EX("fixture_destroy") void fixture_destroy(){renderer.reset();images.clear();}
