#pragma once
#include "AnmRenderer.hpp"
#include "BackgroundDraw.hpp"
#include "StageModelRenderer.hpp"
namespace th09 {
// Connects scene composition and CPU model geometry to the shared graphics
// interface. Render state changes flush only when they cross a draw boundary.
class StageRenderer final:public BackgroundDrawServices,private StageDrawServices {
    AnmRenderer& renderer;Background* active=nullptr;
    void sync_tint(){renderer.tint=tint;renderer.tint_enabled=tint_enabled;}
    void begin_models(const Camera&)override{renderer.scene_camera(active->camera);renderer.state.textureTransform=true;}
    void fog_enabled(bool value)override{renderer.flush();renderer.state.fog=value;}
    void world_sprite(AnmVm& vm)override{sync_tint();renderer.draw_3d(vm);}
    void screen_sprite(AnmVm& vm)override{sync_tint();renderer.draw_2d(vm,true);}
    void screen_quad(AnmVm& vm,const SpriteVertex* p)override{sync_tint();renderer.draw_quad(vm,p);}
public:
    explicit StageRenderer(AnmRenderer& r):renderer(r){}
    Viewport field_view;BackgroundCamera culling_camera;
    void (*boss_draw)(void*,Background&)=nullptr;void* boss_context=nullptr;
    void select_field(i32)override{renderer.set_viewport(field_view);}
    void flush()override{renderer.flush();}
    void clear(bool color,bool depth,u32 value)override{renderer.clear(color,depth,value);}
    void rectangle(float l,float t,float r,float b,u32 c)override{renderer.rectangle(l,t,r,b,c,c);}
    void draw_animation(AnmVm& vm)override{sync_tint();renderer.draw_2d(vm);renderer.flush();}
    void models(Background& bg,i32 layer)override{sync_tint();active=&bg;StageDrawServices::viewport=field_view;StageDrawServices::first_camera=culling_camera;StageModelRenderer::draw(bg,layer,*this);}
    void depth_compare(Compare value)override{renderer.flush();renderer.state.depthCompare=value;}
    void fog(bool value)override{renderer.flush();renderer.state.fog=value;}
    void fog_color(u32 value)override{renderer.flush();renderer.state.fogColor=value;}
    void fog_range(float a,float b)override{renderer.flush();renderer.state.fogNear=a;renderer.state.fogFar=b;}
    void texture_mode(bool world)override{renderer.flush();renderer.state.textureTransform=world;}
    void screen_camera()override{renderer.screen_camera();}
    void custom_boss(Background& bg)override{if(boss_draw)boss_draw(boss_context,bg);}
    void draw_base(Background& bg,BackgroundDrawState& state){StageDrawServices::capture_positions=state.capture_positions;BackgroundDraw::base(bg,state,*this);sync_tint();}
    void draw_overlay(Background& bg,BackgroundDrawState& state){StageDrawServices::capture_positions=state.capture_positions;BackgroundDraw::overlay(bg,state,*this);sync_tint();}
    const std::vector<Vec3>& positions()const{return StageDrawServices::captured;}
    void clear_positions(){StageDrawServices::captured.clear();}
};
}
