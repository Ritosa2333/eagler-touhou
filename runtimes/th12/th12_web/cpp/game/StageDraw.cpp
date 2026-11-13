#include "Stage.hpp"
#include "AnmRenderer.hpp"
#include "ScreenFade.hpp"
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_WORKER)
#include "StageDrawCameraWordsTh14.hpp"
#endif
namespace th11 {
namespace {
void camera_fog(AnmRenderer& renderer,const SceneCamera& camera){auto& p=renderer.pipeline();p.fogColor=camera.fog.color;p.fogNear=camera.fog.near_distance;p.fogFar=camera.fog.far_distance;}
void restore_tint(AnmRenderer& renderer,i32 frame_effect){renderer.flush();renderer.tint_enabled=frame_effect!=0;renderer.tint=frame_effect?0xff404040:0x80808080;}
}
void Stage::fade_out(ScreenFades& fades){fades.start(2,30,0,10,&manager.rate);state.fade_timer.set(30,&manager.rate);state.draw_flags|=2;}
bool Stage::draw_background(AnmRenderer& renderer,SceneCamera& camera,ScreenFades& fades
#if PORT_TITLE == 14
        ,const std::function<bool()>& camera_published
#endif
){
    using touhou::graphics::Compare;
    if(state.draw_flags&8)return true;
    if(!(state.draw_flags&4)||state.fade_timer.current<60){
        state.camera.offset=camera.offset;
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_WORKER)
        th14_publish_draw_camera_words(camera,state.camera);
#else
        camera=state.camera;
#endif
#if PORT_TITLE == 14
        // Retail40e365/40e7f1: the worker can copy the newly published
        // saved camera before446400 replaces its perspective-derived right.
        if(camera_published&&!camera_published()){error="stage camera publication worker failed";return false;}
#endif
        renderer.set_camera(camera,false);
        auto& p=renderer.pipeline();p.depthWrite=true;p.depthCompare=Compare::LessEqual;camera_fog(renderer,state.camera);
        const u32 clear=(state.draw_flags&4)&&state.frame_count<=33?0:state.camera.fog.color;
        if(!renderer.clear_target(clear,&camera.viewport)){error="stage background clear failed";return false;}
    }
    if(state.draw_flags&4){
        if(state.fade_timer.current<30){fades.start(3,30,0,10,&manager.rate);state.draw_flags|=1;state.fade_timer.set(1,&manager.rate);}
        else{state.tint&=0xffffff;state.draw_flags&=~1u;}
    }
    if(state.tint>>24){
        renderer.tint_enabled=true;renderer.tint=state.tint;
        // TH14 40e632 consumes this draw's tint request; the renderer retains
        // the requested color until restore_tint at the end of the pass.
        if constexpr(kPortTitle==14)state.tint&=0xffffff;
    }
    state.drawn_objects=state.culled_objects=state.drawn_primitives=0;
    if(state.draw_flags&1){
        if constexpr(kPortTitle!=14)if(state.script_animations[0].sprite){
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
bool Stage::draw_foreground(AnmRenderer& renderer,SceneCamera& camera
#if PORT_TITLE == 14
        ,const std::function<bool()>& camera_published
#endif
){
    using touhou::graphics::Compare;
    if(state.draw_flags&8)return true;
    if(!(state.draw_flags&4)||state.fade_timer.current<60){
        state.camera.offset=camera.offset;
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_WORKER)
        th14_publish_draw_camera_words(camera,state.camera);
#else
        camera=state.camera;
#endif
#if PORT_TITLE == 14
        // Retail40e365/40e7f1: the worker can copy the newly published
        // saved camera before446400 replaces its perspective-derived right.
        if(camera_published&&!camera_published()){error="stage camera publication worker failed";return false;}
#endif
        renderer.set_camera(camera,false);
        auto& p=renderer.pipeline();p.fog=false;p.depthWrite=false;p.depthCompare=Compare::Always;
        if(renderer.draw_layer(manager.layer_first(kPortTitle==16?32:kPortTitle==14?30:kPortTitle==13?27:25))==-2){error="stage foreground first layer draw failed";return false;}
        renderer.pipeline().depthCompare=Compare::LessEqual;
        if(renderer.draw_layer(manager.layer_first(kPortTitle==16?33:kPortTitle==14?31:kPortTitle==13?28:26))==-2){error="stage foreground second layer draw failed";return false;}
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
    float squared;
    if constexpr(kPortTitle==14){
        // 40ec3b..40ec6e rounds each SSE product and addition: (y*y+x*x)+z*z.
        const float x=float(double(distance.x)*distance.x),y=float(double(distance.y)*distance.y),z=float(double(distance.z)*distance.z);
        squared=float(double(float(double(y)+x))+z);
        if(squared>maximum)return false;
    }else{
        squared=float((double(distance.z)*distance.z+double(distance.x)*distance.x)+double(distance.y)*distance.y);
        if(!(squared<=maximum))return false;
    }
    const Vec3 half{float(double(object.size.x)*.5),float(double(object.size.y)*.5),float(double(object.size.z)*.5)};
    const auto hi=add(object.position,half),lo=sub(object.position,half);
    const Vec3 points[]={
        {hi.x,hi.y,hi.z},{hi.x,hi.y,lo.z},{hi.x,lo.y,hi.z},{hi.x,lo.y,lo.z},
        {lo.x,hi.y,hi.z},{lo.x,hi.y,lo.z},{lo.x,lo.y,hi.z},{lo.x,lo.y,lo.z},
        {object.position.x,lo.y,lo.z},{object.position.x,hi.y,lo.z},{object.position.x,lo.y,hi.z},{object.position.x,hi.y,hi.z},
        {object.position.x,lo.y,object.position.z},{object.position.x,hi.y,object.position.z},
        {object.position.x,lo.y,float(double(object.position.z)-double(half.z)*.5)},
        {object.position.x,hi.y,float(double(object.position.z)+double(half.z)*.5)}
    };
    Matrix4 world;world.identity();world.m[12]=instance.x;world.m[13]=instance.y;world.m[14]=instance.z;
#if PORT_TITLE == 14
    // Retail46aebb..46aee6 centers the fixed384x448 intermediate playfield;
    // 40ef1e..40eff6 culls in that space before its final screen copy.
    const float origin_x=kPortTitle==14?float((i32(camera.render_width)-384)/2):32.f;
    const float origin_y=kPortTitle==14?float((i32(camera.render_height)-448)/2):16.f;
    float min_x=kPortTitle==14?origin_x+392:kPortTitle==13?520:424;
    float max_x=kPortTitle==14?origin_x-8:kPortTitle==13?120:24;
    float min_y=kPortTitle==14?origin_y+456:472,max_y=kPortTitle==14?origin_y-8:8;
#else
    float min_x=kPortTitle==13?520:424,max_x=kPortTitle==13?120:24,min_y=472,max_y=8;
#endif
    // TH14 40eeeb projects all sixteen points; 40ef97..40efbd updates both bounds.
    for(u32 i=0;i<(kPortTitle==13||kPortTitle==14?16:12);++i){const auto p=GraphicsMath::project(points[i],camera.viewport,camera.projection,camera.view,world);
        if((kPortTitle==14?p.z>=0:p.z>0)&&p.z<=1){
            if constexpr(kPortTitle==13||kPortTitle==14){
                if(p.x<min_x)min_x=p.x;if(p.x>max_x)max_x=p.x;
                if(p.y<min_y)min_y=p.y;if(p.y>max_y)max_y=p.y;
            }else{
            // The original's mutually exclusive branches deliberately retain
            // these initial bounds; replacing them by min/max changes culling.
            if(min_x<=p.x){if(max_x<p.x)max_x=p.x;}else min_x=p.x;
            if(min_y<=p.y){if(max_y<p.y)max_y=p.y;}else min_y=p.y;
            }
        }
    }
#if PORT_TITLE == 14
    return max_x>=origin_x&&min_x<=origin_x+384&&max_y>=origin_y&&min_y<=origin_y+448;
#else
    return max_x>=(kPortTitle==13?128:32)&&min_x<=(kPortTitle==13?512:416)&&max_y>=16&&min_y<=464;
#endif
}
bool Stage::draw_layer(AnmRenderer& renderer,SceneCamera& camera,u32 layer){
    bool depth_write=true;
#if PORT_TITLE == 14
    depth_write=renderer.pipeline().depthWrite;
    struct DepthCacheScope{
        AnmRenderer& renderer;bool* previous;
        DepthCacheScope(AnmRenderer& r,bool& value):renderer(r),previous(r.use_stage_depth_cache(&value)){}
        ~DepthCacheScope(){renderer.use_stage_depth_cache(previous);}
    } depth_scope(renderer,depth_write);
    // 4107b0 runs before the object traversal on every layer, including 8..11.
    for(u32 i=0;i<8;++i){auto& vm=state.script_animations[i];
        if(!vm.sprite||state.script_animation_layers[i]!=i32(layer))continue;
        if(layer==0){renderer.upload_camera(camera);renderer.pipeline().fog=false;renderer.set_stage_depth_write(false);}
        if(renderer.draw(vm)==-2){error="stage standalone draw failed";return false;}
        if(layer==0){renderer.set_stage_depth_write(true);renderer.set_camera(camera,false);}
    }
#endif
    renderer.set_camera(camera,false);
    if constexpr(kPortTitle==14)renderer.pipeline().fog=true;
    // Older titles' 404190 local cache starts true even with foreground writes off.
    for(auto& instance:resource.instances){auto& object=resource.objects[instance.object];if(object.layer!=layer)continue;
        if(!stage_visible(object,instance.position,camera,state.draw_distance_squared)){instance.flags&=~1; ++state.culled_objects;continue;}
        object.flags|=2;
        for(const auto& primitive:object.primitives){if(primitive.type!=0)continue;auto& vm=object_animations[primitive.animation];
            const u32 mode=vm.drawing_mode();
            if(mode>=4){vm.position={float(double(primitive.position.x)+instance.position.x),float(double(primitive.position.y)+instance.position.y),float(double(primitive.position.z)+instance.position.z)};
                if((primitive.size.x!=0||primitive.size.y!=0)&&!vm.sprite){error="stage primitive has no sprite";return false;}
                if(primitive.size.x!=0){vm.scale.x=float(double(primitive.size.x)/vm.sprite->width);vm.flags|=8;}
                if(primitive.size.y!=0){vm.scale.y=float(double(primitive.size.y)/vm.sprite->height);vm.flags|=8;}
            }
            auto& pipeline=renderer.pipeline();pipeline.fog=mode==8||(kPortTitle==14&&mode==23);
            const bool write=(vm.flags&0x800)==0;
#if PORT_TITLE == 14
            renderer.set_stage_depth_write(write);
#else
            if(write!=depth_write){pipeline.depthWrite=write;depth_write=write;}
#endif
            if(renderer.draw(vm)==-2){error="stage primitive draw failed";return false;}++state.drawn_primitives;
        }
        instance.flags|=1;++state.drawn_objects;
    }
    // 40f5cb..40f5f5 leaves each TH14 layer with depth writes disabled.
#if PORT_TITLE == 14
    renderer.set_stage_depth_write(false);
#else
    if(!depth_write)renderer.pipeline().depthWrite=true;
#endif
    return true;
}
}
