#include "Stage.hpp"
#include "AnmRenderer.hpp"
#include "ScreenFade.hpp"
namespace th11 {
namespace {
void camera_fog(AnmRenderer& renderer,const SceneCamera& camera){auto& p=renderer.pipeline();p.fogColor=camera.fog.color;p.fogNear=camera.fog.near_distance;p.fogFar=camera.fog.far_distance;}
void restore_tint(AnmRenderer& renderer,i32 frame_effect){renderer.flush();renderer.tint_enabled=frame_effect!=0;renderer.tint=frame_effect?0xff404040:0x80808080;}
}
void Stage::fade_out(ScreenFades& fades){fades.start(2,30,0,10,&manager.rate);state.fade_timer.set(30,&manager.rate);state.draw_flags|=2;}
bool Stage::draw_background(AnmRenderer& renderer,SceneCamera& camera,ScreenFades& fades){
    using touhou::graphics::Compare;
    if(state.draw_flags&8)return true;
    if(!(state.draw_flags&4)||state.fade_timer.current<60){
        state.camera.offset=camera.offset;camera=state.camera;renderer.set_camera(camera,false);
        auto& p=renderer.pipeline();p.depthWrite=true;p.depthCompare=Compare::LessEqual;camera_fog(renderer,state.camera);
        const u32 clear=(state.draw_flags&4)&&state.frame_count<=33?0:state.camera.fog.color;
        if(!renderer.clear_target(clear,&camera.viewport)){error="stage background clear failed";return false;}
    }
    if(state.draw_flags&4){
        if(state.fade_timer.current<30){fades.start(3,30,0,10,&manager.rate);state.draw_flags|=1;state.fade_timer.set(1,&manager.rate);}
        else{state.tint&=0xffffff;state.draw_flags&=~1u;}
    }
    if(state.tint>>24){renderer.tint_enabled=true;renderer.tint=state.tint;}
    state.drawn_objects=state.culled_objects=state.drawn_primitives=0;
    if(state.draw_flags&1){
        if(state.script_animations[0].sprite){
            renderer.set_camera(camera,true);auto& p=renderer.pipeline();p.fog=false;p.depthWrite=false;
            for(auto& vm:state.script_animations)if(vm.sprite&&renderer.draw(vm)==-2){error="stage standalone draw failed";return false;}
            renderer.pipeline().depthWrite=true;renderer.set_camera(camera,false);
        }
        renderer.pipeline().fog=true;
        for(u32 layer=0;layer<8;++layer)if(!draw_layer(renderer,camera,layer))return false;
        renderer.flush();if(state.effects_enabled)++state.effects_enabled;
    }
    restore_tint(renderer,state.frame_effect);auto& p=renderer.pipeline();p.depthWrite=false;p.depthCompare=Compare::Always;return true;
}
bool Stage::draw_foreground(AnmRenderer& renderer,SceneCamera& camera){
    using touhou::graphics::Compare;
    if(state.draw_flags&8)return true;
    if(!(state.draw_flags&4)||state.fade_timer.current<60){
        state.camera.offset=camera.offset;camera=state.camera;renderer.set_camera(camera,false);
        auto& p=renderer.pipeline();p.fog=false;p.depthWrite=false;p.depthCompare=Compare::Always;
        if(renderer.draw_layer(manager.layer_first(25))==-2){error="stage layer 25 draw failed";return false;}
        renderer.pipeline().depthCompare=Compare::LessEqual;
        if(renderer.draw_layer(manager.layer_first(26))==-2){error="stage layer 26 draw failed";return false;}
        renderer.pipeline().depthCompare=Compare::LessEqual;camera_fog(renderer,state.camera);
    }
    if((state.draw_flags&4)&&state.fade_timer.current>=30)state.tint&=0xffffff;
    if(state.draw_flags&1){auto& p=renderer.pipeline();p.depthWrite=false;p.fog=true;
        for(u32 layer=8;layer<12;++layer)if(!draw_layer(renderer,camera,layer))return false;
    }
    restore_tint(renderer,state.frame_effect);
    if(state.fade_timer.current>0){state.fade_timer.advance(-1);if(state.fade_timer.current<1){
        if(state.draw_flags&2)state.draw_flags|=8;state.draw_flags&=~6u;state.tint=0xffffff;
    }}
    auto& p=renderer.pipeline();p.depthWrite=false;p.depthCompare=Compare::Always;p.fog=false;return true;
}
bool stage_visible(const StageObject& object,Vec3 instance,const SceneCamera& camera,float maximum) noexcept {
    const auto add=[](Vec3 a,Vec3 b){return Vec3{float(double(a.x)+b.x),float(double(a.y)+b.y),float(double(a.z)+b.z)};};
    const auto sub=[](Vec3 a,Vec3 b){return Vec3{float(double(a.x)-b.x),float(double(a.y)-b.y),float(double(a.z)-b.z)};};
    const Vec3 distance=sub(add(object.position,instance),add(camera.position,camera.eye_offset));
    const float squared=float((double(distance.z)*distance.z+double(distance.x)*distance.x)+double(distance.y)*distance.y);
    if(!(squared<=maximum))return false;
    const Vec3 half{float(double(object.size.x)*.5),float(double(object.size.y)*.5),float(double(object.size.z)*.5)};
    const auto hi=add(object.position,half),lo=sub(object.position,half);
    const Vec3 points[]={
        {hi.x,hi.y,hi.z},{hi.x,hi.y,lo.z},{hi.x,lo.y,hi.z},{hi.x,lo.y,lo.z},
        {lo.x,hi.y,hi.z},{lo.x,hi.y,lo.z},{lo.x,lo.y,hi.z},{lo.x,lo.y,lo.z},
        {object.position.x,lo.y,lo.z},{object.position.x,hi.y,lo.z},{object.position.x,lo.y,hi.z},{object.position.x,hi.y,hi.z}
    };
    Matrix4 world;world.identity();world.m[12]=instance.x;world.m[13]=instance.y;world.m[14]=instance.z;
    float min_x=424,max_x=24,min_y=472,max_y=8;
    for(const auto& point:points){const auto p=GraphicsMath::project(point,camera.viewport,camera.projection,camera.view,world);
        if(p.z>0&&p.z<=1){
            // The original's mutually exclusive branches deliberately retain
            // these initial bounds; replacing them by min/max changes culling.
            if(min_x<=p.x){if(max_x<p.x)max_x=p.x;}else min_x=p.x;
            if(min_y<=p.y){if(max_y<p.y)max_y=p.y;}else min_y=p.y;
        }
    }
    return max_x>=32&&min_x<=416&&max_y>=16&&min_y<=464;
}
bool Stage::draw_layer(AnmRenderer& renderer,SceneCamera& camera,u32 layer){
    renderer.set_camera(camera,false);
    // 404190 keeps this cache separate from the actual device depth mask.
    // In the foreground its initial value is still true, even with writes off.
    bool depth_write=true;
    for(auto& instance:resource.instances){auto& object=resource.objects[instance.object];if(object.layer!=layer)continue;
        if(!stage_visible(object,instance.position,camera,state.draw_distance_squared)){instance.flags&=~1; ++state.culled_objects;continue;}
        object.flags|=2;
        for(const auto& primitive:object.primitives){if(primitive.type!=0)continue;auto& vm=object_animations[primitive.animation];
            const u32 mode=(vm.flags>>22)&15;
            if(mode>=4){vm.position={float(double(primitive.position.x)+instance.position.x),float(double(primitive.position.y)+instance.position.y),float(double(primitive.position.z)+instance.position.z)};
                if((primitive.size.x!=0||primitive.size.y!=0)&&!vm.sprite){error="stage primitive has no sprite";return false;}
                if(primitive.size.x!=0){vm.scale.x=float(double(primitive.size.x)/vm.sprite->width);vm.flags|=8;}
                if(primitive.size.y!=0){vm.scale.y=float(double(primitive.size.y)/vm.sprite->height);vm.flags|=8;}
            }
            auto& pipeline=renderer.pipeline();pipeline.fog=mode==8;
            const bool write=(vm.flags&0x800)==0;if(write!=depth_write){pipeline.depthWrite=write;depth_write=write;}
            if(renderer.draw(vm)==-2){error="stage primitive draw failed";return false;}++state.drawn_primitives;
        }
        instance.flags|=1;++state.drawn_objects;
    }
    if(!depth_write)renderer.pipeline().depthWrite=true;return true;
}
}
