#include "SceneCompositor.hpp"
namespace th11 {
void SceneCompositor::reset_cameras(bool restricted,u32 width,u32 height){
    if constexpr(kPortTitle==14||kPortTitle==16){
        // Both native reset entries (4465e0 / 446920) use the same four cameras.
        const float scale=float(width)/640.f;
#if PORT_TITLE == 14
        manager.render_space.scale=scale;
#endif
        full_camera={};full_camera.mode=2;full_camera.viewport={0,0,width,height,0,1};
        play_camera={};play_camera.viewport={u32(32*scale),u32(16*scale),u32(384*scale),u32(448*scale),0,1};
        scaled_play_camera=play_camera;scaled_play_camera.mode=1;scaled_play_camera.viewport.x=u32(128*scale);
        world_camera=play_camera;world_camera.mode=3;
        region={u32((i32(width)-408)/2),u32((i32(height)-472)/2),408,472,0,1};world_camera.viewport=region;
#if PORT_TITLE == 14
        for(auto* camera:{&full_camera,&play_camera,&scaled_play_camera,&world_camera}){camera->render_width=width;camera->render_height=height;}
#endif
        full_camera.screen();play_camera.screen();scaled_play_camera.screen();world_camera.screen();
#if PORT_TITLE == 14
        // Retail4468eb..44690e follows the four screen-camera calculations.
        manager.render_space.scaled_origin={float(i32(width)/2),float(i32(float(scale*16.f)))};
#endif
        return;
    }
    full_camera={};full_camera.mode=1;play_camera={};play_camera.position.z=1000;world_camera=play_camera;
    play_camera.viewport=restricted?GraphicsViewport{32,16,384,448,0,1}:GraphicsViewport{};
    region=restricted?(kPortTitle==13?GraphicsViewport{116,2,408,476,0,1}:GraphicsViewport{13,0,422,480,0,1}):(kPortTitle==13?GraphicsViewport{}:GraphicsViewport{0,0,512,512,0,1});world_camera.viewport=region;
}
void SceneCompositor::reset(){for(auto& vm:copies){manager.release_geometry(vm);vm={};}resource=nullptr;clear_color=0xff000000;}
bool SceneCompositor::initialize(AnmResource& text,u32 width,u32 height){
    reset();last_error.clear();
    reset_cameras(true,width,height);
    // The runtime's own accounting of the file, so a disagreement with an offline read is visible in the message.
    std::string table;
    for(const auto& texture:text.textures)table+=" "+std::to_string(texture.first_script)+"/"+std::to_string(texture.script_count);
    if(text.textures.size()<4||text.textures[2].kind!=AnmTexture::Kind::RenderTarget||text.textures[3].kind!=AnmTexture::Kind::RenderTarget){
        last_error="expected four textures with render targets at 2 and 3, found "+std::to_string(text.textures.size())+" (slots"+table+")";return false;
    }
    // The screen copies are the render-target textures' own first scripts. Both titles ship `text.anm` with
    // `@ @ @R @R` in that order, but the slots are not the same number: th12 binds 81 and 82, th13 binds 82 and
    // 83, because th13's second texture declares one script more. The slot is therefore read from the file.
    const i32 screen=i32(text.textures[2].first_script),back=i32(text.textures[3].first_script);
    // TH13 43cd40 binds 82,83,82,84. The fourth copy moves the intermediate
    // playfield at x=128 to the final window at x=32.
    for(u32 i=0;i<sizeof(copies)/sizeof(copies[0]);++i){
        const i32 resolution=kPortTitle==14||kPortTitle==16?(width==960?1:width==1280?2:0):0;
        const i32 slot=kPortTitle==14||kPortTitle==16?(i%2?back:screen)+(i>=2?3:0)+resolution:i==3?back+1:i==1?back:screen;
#if PORT_TITLE == 14
        const bool bound=copies[i].copy_script(text,slot,0,&manager.rate);
#else
        const bool bound=copies[i].bind_script(text,slot,0,&manager.rate);
#endif
        if(!bound){last_error="bind copy "+std::to_string(i)+" to its script "+std::to_string(slot)+" (slots"+table+")";reset();return false;}
        if(copies[i].update(manager)<0){
            const auto* stopped=copies[i].instruction;
            const i32 op=stopped?stopped->opcode:-1;
            last_error="update copy "+std::to_string(i)+" (its script "+std::to_string(slot)+") stopped on ANM opcode "+std::to_string(op);
            // The whole instruction, because which command it is decides the fix: a length of eight
            // with an argument then means the interpreter read the next command's bytes.
            if(stopped){
                last_error+=" [length "+std::to_string(stopped->length)+" time "+std::to_string(stopped->time)
                           +" references "+std::to_string(stopped->references);
                for(u32 n=0;n<4;++n)last_error+=" arg"+std::to_string(n)+" "+std::to_string(u32(stopped->argument<u32>(n)));
                last_error+="]";
            }
            if(op==3)last_error+=" which asks for sprite "+std::to_string(stopped->argument<i32>(0))+" of "+std::to_string(text.sprites.size());
            last_error+=" (slots"+table+")";
            reset();return false;
        }
    }
    // 4476e5 keeps the point filter only at the native 960-pixel mode.
    if constexpr(kPortTitle==14)if(width!=960)copies[2].flags&=~0x80000000u;
    // TH16 43dbe7 clears native flags2 bit11 in the 960-pixel mode.
    if constexpr(kPortTitle==16)if(width==960)copies[2].flags2&=~0x800u;
    resource=&text;return true;
}
bool SceneCompositor::stage_start(){if(!resource)return false;copies[0].interrupt(2);return copies[0].update(manager)>=0;}
bool SceneCompositor::update(){
    if constexpr(kPortTitle==14)if(resource)for(auto& vm:copies)if(vm.update(manager)<0)return false;
    return true;
}
bool SceneCompositor::draw_animation(AnmRenderer& renderer,u32 layer){
    if constexpr(kPortTitle==16){
        // The six camera-changing wrappers are executed by the native oracle.
        if(layer==3){renderer.set_camera(world_camera,true);renderer.pipeline().fog=false;}
        if(layer==20){renderer.set_camera(scaled_play_camera,true);renderer.pipeline().depthWrite=false;renderer.pipeline().depthCompare=touhou::graphics::Compare::Always;}
        if(layer==24||layer==36){renderer.set_camera(full_camera,true);renderer.pipeline().depthWrite=false;renderer.pipeline().depthCompare=touhou::graphics::Compare::Always;if(layer==24)renderer.offset={};}
        if(layer==28||layer==39)renderer.set_camera(play_camera,true);
        const bool result=renderer.draw_layer(manager.layer_first(layer))!=-2;
        if(layer==28||layer==39)renderer.set_camera(full_camera,true);
        return result;
    }
#if PORT_TITLE == 14
    if constexpr(kPortTitle==14){
        if(layer==3){renderer.set_camera(world_camera,true);renderer.pipeline().fog=false;}
        if(layer==20){renderer.upload_camera(scaled_play_camera);renderer.pipeline().depthWrite=false;renderer.pipeline().depthCompare=touhou::graphics::Compare::Always;}
        if(layer==24){renderer.upload_camera(full_camera);renderer.pipeline().depthWrite=false;renderer.pipeline().depthCompare=touhou::graphics::Compare::Always;renderer.offset={};}
        if(layer==34)renderer.upload_camera(full_camera);
        if(layer==27||layer==35)renderer.upload_camera(play_camera);
        const bool result=renderer.draw_layer(manager.layer_first(layer))!=-2;
        if(layer==27||layer==35)renderer.upload_camera(full_camera);
        return result;
    }
#endif
    if(layer==(kPortTitle==13?4:3)){renderer.set_camera(world_camera,true);renderer.pipeline().fog=false;}
    if(layer==(kPortTitle==13?20:19)){world_camera.offset={};renderer.offset={};}
    if(layer==(kPortTitle==13?21:20)||layer==(kPortTitle==13?31:29))renderer.set_camera(full_camera,true);
    if constexpr(kPortTitle==13)if(layer==24||layer==32)renderer.set_camera(play_camera,true);
    const bool result=renderer.draw_layer(manager.layer_first(layer))!=-2;
    if constexpr(kPortTitle==13)if(layer==24||layer==32)renderer.set_camera(full_camera,true);
    return result;
}
bool SceneCompositor::draw(AnmRenderer& renderer,SceneDrawKind kind){
    if(kind==SceneDrawKind::End){renderer.flush();world_camera.offset={};if constexpr(kPortTitle==14)scaled_play_camera.offset={};return true;}
    if(kind==SceneDrawKind::Begin){
        renderer.invalidate();
        if(!renderer.clear_target(resource?0xffffffff:clear_color))return false;
        if(resource&&(!renderer.select_target(resource,2)||!renderer.clear_target(clear_color,&region)))return false;
#if PORT_TITLE == 14
        // Retail446c72..446cb1 writes play origin after target/clear only
        // when the intermediate render target exists. Reset retains it.
        if(resource)manager.render_space.play_origin={float(i32(world_camera.render_width)/2),float((i32(world_camera.render_height)-448)/2)};
#endif
        renderer.offset={};renderer.tint_enabled=false;renderer.tint=0x80808080;
        
#if PORT_TITLE == 14
        renderer.upload_camera(full_camera);
#else
        renderer.set_camera(full_camera,kPortTitle==16);
#endif
        return true;
    }
    if(!resource)return true;
    switch(kind){
    case SceneDrawKind::TargetB:case SceneDrawKind::TargetC:return renderer.select_target(resource,3)&&renderer.clear_target(clear_color,(kPortTitle==14||kPortTitle==16)&&kind==SceneDrawKind::TargetC?&scaled_play_camera.viewport:&region,kPortTitle==14||kPortTitle==16?2:3);
    case SceneDrawKind::TargetA:return renderer.select_target(resource,2)&&renderer.clear_target(clear_color,&region,kPortTitle==14||kPortTitle==16?2:3);
    case SceneDrawKind::TargetScreen:return renderer.select_target(nullptr,0)&&renderer.clear_target(kPortTitle==14||kPortTitle==16?0xff000000:clear_color);
    case SceneDrawKind::CompositeA:case SceneDrawKind::CompositeB:{
        const bool second=kind==SceneDrawKind::CompositeB;
        auto& copy=copies[second?1:0];
        if constexpr(kPortTitle==14||kPortTitle==16){renderer.flush();renderer.pipeline().blendEquation=touhou::graphics::BlendEquation::Add;renderer.pipeline().alphaTest=false;renderer.pipeline().depthWrite=false;
#if PORT_TITLE == 14
            renderer.upload_camera(world_camera);
#else
            renderer.set_camera(world_camera,true);
#endif
        }
        if(renderer.draw(copy)==-2)return false;
        if constexpr(kPortTitle==13)copy.color=0xffffffff;
        if constexpr(kPortTitle==14||kPortTitle==16){if(!second&&renderer.draw_layer(manager.layer_first(kPortTitle==16?34:32))==-2)return false;}
        else if(renderer.draw_layer(manager.layer_first((kPortTitle==13?29:27)+(second?1:0)))==-2)return false;
        renderer.flush();if constexpr(kPortTitle==14||kPortTitle==16){copy.color=0xffffffff;renderer.pipeline().alphaTest=true;}return true;
    }
    case SceneDrawKind::CompositeC:case SceneDrawKind::CompositeScreen:{
        if constexpr(kPortTitle==16)if(kind==SceneDrawKind::CompositeC)renderer.pipeline().depthWrite=false;
        if constexpr(kPortTitle==14||kPortTitle==16){renderer.flush();renderer.pipeline().alphaTest=false;if(kind==SceneDrawKind::CompositeC)renderer.pipeline().blendEquation=touhou::graphics::BlendEquation::Add;}
        auto& copy=copies[(kPortTitle==13||kPortTitle==14||kPortTitle==16)&&kind==SceneDrawKind::CompositeScreen?3:2];const int result=renderer.draw(copy);copy.color=0xffffffff;renderer.flush();
        if constexpr(kPortTitle==14||kPortTitle==16)renderer.pipeline().alphaTest=true;return result!=-2;
    }
    default:return false;
    }
}
}
