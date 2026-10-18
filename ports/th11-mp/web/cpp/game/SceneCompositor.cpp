#include "SceneCompositor.hpp"
namespace th11 {
void SceneCompositor::reset_cameras(bool restricted){
    full_camera={};full_camera.mode=1;play_camera={};play_camera.position.z=1000;world_camera=play_camera;
    play_camera.viewport=restricted?GraphicsViewport{32,16,384,448,0,1}:GraphicsViewport{};
    region=restricted?GraphicsViewport{13,0,422,480,0,1}:GraphicsViewport{0,0,512,512,0,1};world_camera.viewport=region;
}
void SceneCompositor::reset(){for(auto& vm:copies){manager.release_geometry(vm);vm={};}resource=nullptr;clear_color=0xff000000;}
bool SceneCompositor::initialize(AnmResource& text){
    reset();
    reset_cameras();
    if(text.textures.size()<4||text.textures[2].kind!=AnmTexture::Kind::RenderTarget||text.textures[3].kind!=AnmTexture::Kind::RenderTarget)return false;
    for(u32 i=0;i<3;++i)if(!copies[i].bind_script(text,i==1?82:81,0,&manager.rate)||copies[i].update(manager)<0){reset();return false;}
    resource=&text;return true;
}
bool SceneCompositor::stage_start(){if(!resource)return false;copies[0].pending_interrupt=2;return copies[0].update(manager)>=0;}
bool SceneCompositor::draw(AnmRenderer& renderer,SceneDrawKind kind){
    if(kind==SceneDrawKind::End){renderer.flush();world_camera.offset={};return true;}
    if(kind==SceneDrawKind::Begin){
        renderer.invalidate();
        if(!renderer.clear_target(resource?0xffffffff:clear_color))return false;
        if(resource&&(!renderer.select_target(resource,2)||!renderer.clear_target(clear_color,&region)))return false;
        renderer.offset={};renderer.tint_enabled=false;renderer.tint=0x80808080;
        renderer.set_camera(full_camera,false);return true;
    }
    if(!resource)return true;
    switch(kind){
    case SceneDrawKind::TargetB:return renderer.select_target(resource,3)&&renderer.clear_target(clear_color,&region);
    case SceneDrawKind::TargetA:return renderer.select_target(resource,2)&&renderer.clear_target(clear_color,&region);
    case SceneDrawKind::TargetScreen:return renderer.select_target(nullptr,0)&&renderer.clear_target(clear_color);
    case SceneDrawKind::CompositeA:case SceneDrawKind::CompositeB:{
        const bool second=kind==SceneDrawKind::CompositeB;
        if(renderer.draw(copies[second?1:0])==-2||renderer.draw_layer(manager.layer_first(second?28:27))==-2)return false;
        renderer.flush();return true;
    }
    case SceneDrawKind::CompositeScreen:{const int result=renderer.draw(copies[2]);copies[2].color=0xffffffff;renderer.flush();return result!=-2;}
    default:return false;
    }
}
}
