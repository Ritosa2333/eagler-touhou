#include "StageRenderer.hpp"
namespace th10 {
namespace {
void set_timer(Timer& timer,u32& flags,float* rate,i32 value){if(!(flags&1)){timer.rate=rate;flags|=1;}timer.current=value;timer.previous=wrapping_add(value,-1);timer.fractional=Extended::from_int(value).to_float();}
}
void StageRenderEnvironment::activate_world(bool flat){*active=world;if(flat)world->configure_flat(*camera);else world->configure_world(*camera);camera->set_viewport((*active)->viewport);*screen_space=0;}
void StageRenderEnvironment::depth_mask(bool value){camera->flush();RenderCommands(*camera->render_environment).SetDepthMask(value);}
void StageRenderEnvironment::depth_func(DepthFunc value){camera->flush();RenderCommands(*camera->render_environment).SetDepthFunc(value);}
void StageRenderEnvironment::fog_color(u32 value){camera->flush();RenderCommands(*camera->render_environment).SetFogColor(value);}
void StageRenderEnvironment::fog_range(float near_plane,float far_plane){camera->flush();RenderCommands(*camera->render_environment).SetFogRange(near_plane,far_plane);}
void StageRenderEnvironment::fog(bool enabled){if(*fog_enabled!=static_cast<u32>(enabled)){camera->flush();*fog_enabled=enabled;RenderCommands(*camera->render_environment).SetFogEnabled(enabled);}}
// 0x403090. Reject by squared distance, then project all eight bounds corners.
// The original min/maximum scan deliberately uses an else-if after each new
// minimum and seeds the bounds with a margin around the playfield.
i32 StageRenderer::culled(const StageObject& object,const Vec3& instance,const Camera& camera,float limit,StageRenderEnvironment& environment){
    const auto x=number(Scalar::add(object.position.x,instance.x))-(number(camera.eye_offset.x)+number(camera.position.x));
    const auto y=number(Scalar::add(object.position.y,instance.y))-(number(camera.eye_offset.y)+number(camera.position.y));
    const auto z=number(Scalar::add(object.position.z,instance.z))-number(Scalar::add(camera.eye_offset.z,camera.position.z));
    const auto dx=number(x.to_float()),dy=number(y.to_float()),dz=number(z.to_float());
    if(number(limit)<dz*dz+dy*dy+dx*dx)return 1;
    const Vec3 half{Scalar::mul(object.size.x,.5f),Scalar::mul(object.size.y,.5f),Scalar::mul(object.size.z,.5f)};
    const Vec3 high{Scalar::add(half.x,object.position.x),Scalar::add(half.y,object.position.y),Scalar::add(half.z,object.position.z)};
    const Vec3 low{Scalar::sub(object.position.x,half.x),Scalar::sub(object.position.y,half.y),Scalar::sub(object.position.z,half.z)};
    Vec3 corners[8],projected[8];for(u32 i=0;i<8;++i)corners[i]={i&4?low.x:high.x,i&2?low.y:high.y,i&1?low.z:high.z};
    Matrix4 world;world.identity();environment.translation(world,instance);environment.project_points(projected,corners,8,camera,world);
    float min_x=424,max_x=24,min_y=472,max_y=8;
    for(const auto& p:projected){if(!(p.z>=0&&p.z<=1))continue;if(p.x<min_x)min_x=p.x;else if(p.x>max_x)max_x=p.x;if(p.y<min_y)min_y=p.y;else if(p.y>max_y)max_y=p.y;}
    return max_x>=32&&min_x<=416&&max_y>=16&&min_y<=464?0:1;
}
// 0x403a30. STD object layers are signed bytes and primitive animation indices
// are signed shorts. Type 0 draws an ANM; other primitive types are traversed.
i32 StageRenderer::draw_objects(i32 layer){
    environment.activate_world(false);(*environment.camera->animation_manager)->cached_draw_state[4]=1;
    for(auto* instance=stage.instances;instance->object>=0;++instance){auto& object=*stage.objects[instance->object];if(static_cast<std::int8_t>(object.layer)!=layer)continue;
        if(culled(object,instance->position,*environment.world,stage.draw_distance_squared,environment)){++stage.culled_objects;continue;}
        object.flags|=2;
        for(auto* primitive=object.primitives();primitive->type>=0;primitive=primitive->next()){
            auto& vm=stage.object_animations[primitive->animation];if(primitive->type!=0)continue;
            if((vm.flags&0x3c00000)>=0x1000000){
                vm.position={Scalar::add(primitive->position.x,instance->position.x),Scalar::add(primitive->position.y,instance->position.y),Scalar::add(primitive->position.z,instance->position.z)};
                if(primitive->size.x!=0){vm.flags|=8;vm.scale.x=Scalar::div(primitive->size.x,vm.sprite->width);}
                if(primitive->size.y!=0){vm.flags|=8;vm.scale.y=Scalar::div(primitive->size.y,vm.sprite->height);}
            }
            environment.fog((vm.flags&0x3c00000)==0x2000000);environment.draw_animation(vm);++stage.drawn_primitives;
        }++stage.drawn_objects;
    }return 0;
}
void StageRenderer::copy_camera(){environment.camera->flush();stage.camera.draw_offset=environment.world->draw_offset;*environment.world=stage.camera;environment.activate_world(false);}
void StageRenderer::reset_tint(){auto& manager=**environment.camera->animation_manager;manager.tint_enabled=0;manager.tint=0x80808080;if(stage.frame_effect){manager.tint_enabled=1;manager.tint=0xff404040;}}
void StageRenderer::apply_fog(){environment.fog_color(stage.camera.fog.packed_color);environment.fog_range(stage.camera.fog.near_distance,stage.camera.fog.far_distance);}
// 0x402850. Background passes include embedded screen-space ANMs followed by
// STD layers 0..7, with the original transition and depth-buffer behavior.
i32 StageRenderer::draw_background(){
    if(stage.draw_flags&8)return 1;
    if(!(stage.draw_flags&4)||stage.fade_timer.current<60){
        copy_camera();environment.depth_mask(true);environment.depth_func(DepthFunc::LessEqual);apply_fog();environment.clear(2,0,nullptr);
        const StageClearRect rectangle{32,16,416,464};const auto color=(stage.draw_flags&4)&&static_cast<i32>(stage.frame_count)<34?0:stage.camera.fog.packed_color&0xffffff;environment.clear(1,color,&rectangle);
    }
    if(stage.draw_flags&4){if(stage.fade_timer.current<30){environment.fade(3,30);stage.draw_flags|=1;set_timer(stage.fade_timer,stage.fade_timer_flags,environment.rate,1);}else{stage.draw_flags&=~1u;stage.fade_color&=0xffffff;}}
    if(stage.fade_color>>24){auto& manager=**environment.camera->animation_manager;manager.tint_enabled=1;manager.tint=stage.fade_color;}
    stage.drawn_objects=stage.culled_objects=stage.drawn_primitives=0;
    if(stage.draw_flags&1){
        if(stage.script_animations[0].sprite){environment.activate_world(true);environment.fog(false);environment.camera->flush();environment.depth_mask(false);for(auto& vm:stage.script_animations)if(vm.sprite)environment.draw_animation(vm);environment.depth_mask(true);environment.activate_world(false);}
        environment.fog(true);for(i32 layer=0;layer<8;++layer)draw_objects(layer);environment.camera->flush();if(stage.effects_enabled)++stage.effects_enabled;
    }
    reset_tint();environment.depth_mask(false);environment.depth_func(DepthFunc::Always);return 1;
}
// 0x402ca0. Foreground ANM layers 17/18 precede STD layers 8..11. Fade time
// advances here once per drawn frame, including the final visibility changes.
i32 StageRenderer::draw_foreground(){
    if(stage.draw_flags&8)return 1;
    if(!(stage.draw_flags&4)||stage.fade_timer.current<60){copy_camera();environment.fog(true);environment.depth_mask(false);environment.depth_func(DepthFunc::Always);environment.draw_layer(17);environment.depth_func(DepthFunc::LessEqual);environment.fog(false);environment.draw_layer(18);environment.depth_func(DepthFunc::LessEqual);apply_fog();}
    if((stage.draw_flags&4)&&stage.fade_timer.current>=30)stage.fade_color&=0xffffff;
    if(stage.draw_flags&1){environment.depth_mask(false);environment.fog(true);for(i32 layer=8;layer<12;++layer)draw_objects(layer);environment.camera->flush();}
    reset_tint();environment.depth_mask(false);environment.depth_func(DepthFunc::Always);
    if(stage.fade_timer.current>0){stage.fade_timer.advance(-1);if(stage.fade_timer.current<=0){if(stage.draw_flags&2)stage.draw_flags|=8;stage.fade_color=0xffffff;stage.draw_flags&=~6u;}}
    environment.depth_mask(false);environment.depth_func(DepthFunc::Always);environment.fog(false);return 1;
}
}
