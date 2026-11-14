#include "Stage.hpp"
#include "GraphicsMath.hpp"
#include "Movement.hpp"
#include <cmath>
#include <type_traits>
#if PORT_TITLE == 14
#include "SceneCameraReadJobTh14.hpp"
#endif
#include <algorithm>
#include <type_traits>
namespace th11 {
#if PORT_TITLE == 14
bool Stage::prepare_object_storage(u32 count){
    if(th14_object_storage_prepared||!object_animations.empty())return false;
    // Native40f132..157 allocates and zeroes detached VM storage BEFORE
    // 40d445 copies the live camera. These VMs are private and unregistered.
    object_animations.resize(count);
    static_assert(std::is_standard_layout_v<AnmVm> && std::is_trivially_destructible_v<AnmVm>);
    static_assert(offsetof(AnmVm,flags2)==offsetof(AnmVm,flags)+4);
    static_assert(offsetof(AnmVm,th13_native_flags)==offsetof(AnmVm,flags)+8);
    for(auto& vm:object_animations){
        auto* bytes=reinterpret_cast<u8*>(&vm);
        // Clear private scalar representation around the three flag words;
        // keep their regular typed stores, including atomic worker builds.
        std::fill(bytes,bytes+offsetof(AnmVm,flags),u8(0));
        std::fill(bytes+offsetof(AnmVm,th13_native_flags)+4,bytes+sizeof(vm),u8(0));
        vm.flags=0;vm.flags2=0;vm.th13_native_flags=0;
    }
    th14_object_storage_prepared=true;return true;
}
#endif
Stage::~Stage(){for(auto& vm:object_animations)manager.release_geometry(vm);for(auto& vm:state.script_animations)manager.release_geometry(vm);for(auto& vm:state.effect_animations)manager.release_geometry(vm);}
bool Stage::initialize(const StageResource& file,i32 number,const SceneCamera& camera,bool start){
#if PORT_TITLE == 14
    if(file.instructions.empty())return false;
    if(!th14_object_storage_prepared&&!prepare_object_storage(file.animation_count))return false;
    if(object_animations.size()!=file.animation_count)return false;
#else
    if(!object_animations.empty()||file.instructions.empty())return false;
#endif
    resource=file;std::memset(&state,0,sizeof state);state.flags=2;state.stage_number=number;state.camera=camera;
    state.camera.position={0,0,-600};state.camera.direction={0,300,600};state.camera.up={0,1,0};state.camera.eye_offset={};
#if PORT_TITLE == 14
    // Retail40d4ef/40d4fc clears the copied previous-stage target offset.
    state.camera.target_offset={};
#endif
    state.draw_distance_squared=9610000;state.draw_flags=1;state.script_timer.set(0,&manager.rate);
    return !start||start_objects();
}
bool Stage::start_objects(){
#if PORT_TITLE == 14
    if(!th14_object_storage_prepared&&!prepare_object_storage(resource.animation_count))return false;
    if(object_animations.size()!=resource.animation_count)return false;
    th14_object_storage_prepared=false;
#else
    if(!object_animations.empty())return false;
    object_animations.resize(resource.animation_count);
#endif
    for(auto& object:resource.objects){object.flags=1;for(const auto& primitive:object.primitives)if(!stage_animation(object_animations[primitive.animation],primitive.script))return false;}
    return true;
}
bool Stage::stage_animation(AnmVm& vm,i32 script){
    manager.release_geometry(vm);
#if PORT_TITLE == 14
    const bool bound=vm.copy_script(animations,script,file_id,&manager.rate);
#else
    const bool bound=vm.bind_script(animations,script,file_id,&manager.rate);
#endif
    if(!bound){error="invalid stage animation script";return false;}
    // 405030 writes 16 before 44acd0, whose second initialization clears it.
    if(vm.update(manager)<0){error="stage animation execution failed";return false;}return true;
}
bool Stage::stage_deformation(i32 mode){
    mesh.reset();state.deformation=0;auto next=std::make_unique<ScreenDeformation>(manager);
    if(!next->initialize(text,17,mode==1?(kPortTitle==16?7:25):17,false)){error="stage deformation initialization failed";return false;}
    mesh=std::move(next);state.deformation=1;return true;
}
bool Stage::update_objects(
#if PORT_TITLE == 14
    const StageObjectVmBoundaryTh14& object_boundary
#endif
){
    for(u32 object_index=0;object_index<resource.objects.size();++object_index){
        auto& object=resource.objects[object_index];if(!(object.flags&1))continue;u32 active=0;
        for(u32 primitive_index=0;primitive_index<object.primitives.size();++primitive_index){
            const auto& primitive=object.primitives[primitive_index];auto& vm=object_animations[primitive.animation];
#if PORT_TITLE == 14
            // Retail40f2a2 calls this detached background VM;40f2a7 resumes.
            if(object_boundary&&!object_boundary(*this,vm,object_index,primitive_index,primitive.animation,AnmManager::VmBoundaryTh14::BeforeCall)){
                error="loading before stage object VM call";return false;
            }
#endif
            if(vm.update(manager)<0){error="stage object animation failed";return false;}
#if PORT_TITLE == 14
            if(object_boundary&&!object_boundary(*this,vm,object_index,primitive_index,primitive.animation,AnmManager::VmBoundaryTh14::AfterReturn)){
                error="loading after stage object VM return";return false;
            }
#endif
            if(vm.instruction)++active;
        }
        if(!active)object.flags&=~1u;
    }
    return true;
}
bool Stage::update(
#if PORT_TITLE == 14
    const StageObjectVmBoundaryTh14& object_boundary
    ,const StageCameraWordBoundaryTh14& camera_word_boundary
#endif
){
    if((state.draw_flags&8)||((state.draw_flags&4)&&state.fade_timer.current>=60))return true;
    state.camera.offset={};state.camera.animation_delta={};state.camera.reserved24=GraphicsMath::normalize(state.camera.direction);state.tint=0x00808080;
#if PORT_TITLE == 13 || PORT_TITLE == 14
    const auto& d=state.camera.direction;const auto& o=state.camera.target_offset;
    state.camera.reserved24=GraphicsMath::normalize({float(double(d.x)+o.x),float(double(d.y)+o.y),float(double(d.z)+o.z)});
#endif
    if(!(state.draw_flags&4)||state.fade_timer.current<30){
        if(!update_objects(
#if PORT_TITLE == 14
            object_boundary
#endif
        )||!update_script())return false;
        if constexpr(kPortTitle!=14)for(auto& vm:state.script_animations)if(vm.update(manager)<0){error="stage standalone animation failed";return false;}
        if(state.frame_effect){const float saved_rate=manager.rate;manager.rate=1;bool ok=true;for(auto& vm:state.effect_animations)if(vm.update(manager)<0)ok=false;manager.rate=saved_rate;if(!ok){error="stage effect animation failed";return false;}}
    }
    state.frame_effect=0;if(!publish_camera(
#if PORT_TITLE == 14
        camera_word_boundary
#endif
    ))return false;
    // Retail 40dae2 publishes the camera before 40daf8 updates all eight VMs.
    // The animation loop also runs while the fade clock is between 30 and 59.
    if constexpr(kPortTitle==14)for(auto& vm:state.script_animations)if(vm.update(manager)<0){error="stage standalone animation failed";return false;}
    if(mesh&&!update_deformation())return false;++state.frame_count;return true;
}
bool Stage::publish_camera(
#if PORT_TITLE == 14
    const StageCameraWordBoundaryTh14& camera_word_boundary
#endif
){
    if(!active_camera)return true;
    const auto refresh_camera_references=[this](){
#if PORT_TITLE == 14
    // Retail46f921..46f95c reads world-camera position plus eye_offset.
    manager.reference_positions[0]={(*active_camera).position.x+(*active_camera).eye_offset.x,
        (*active_camera).position.y+(*active_camera).eye_offset.y,(*active_camera).position.z+(*active_camera).eye_offset.z};
#else
    manager.reference_positions[0]=active_camera->position;
#endif
    manager.reference_positions[1]=active_camera->reserved24;
    manager.camera_delta=active_camera->animation_delta;    };
#if PORT_TITLE == 14
    static_assert(std::is_trivially_copyable<SceneCamera>::value);
    static_assert(sizeof(SceneCamera)%sizeof(u32)==0);
    // Retail40dae7 publishes with REP MOVSD. Only the mapped camera words
    // are modeled here, including the original render width and height.
    // An independent worker may read between retail REP MOVSD iterations
    // even with no observation callback installed. Never aggregate those
    // shared stores into a whole-structure copy in the worker configuration.
#if defined(TH14_CPP_STAGE_WORKER)
    constexpr bool shared_word_copy=true;
#else
    constexpr bool shared_word_copy=false;
#endif
    if(shared_word_copy||camera_word_boundary){
        if(camera_word_boundary){
            refresh_camera_references();
            if(!camera_word_boundary(*this,0)){error="loading before camera publication";return false;}
        }
        // Native target_offset and render dimensions precede view. C++
        // retains them at the end; publish every one of the 75 native words.
        static_assert(sizeof(SceneCamera)==300);
        static_assert(offsetof(SceneCamera,target_offset)==280);
        static_assert(offsetof(SceneCamera,fov)==72);
        static_assert(offsetof(SceneCamera,view)==76);
        for(u32 i=0;i<75;++i){
            const u32 offset=SceneCameraReadJobTh14::source_offset(i);
            u32 word;std::memcpy(&word,reinterpret_cast<const u8*>(&state.camera)+offset,4);
#if defined(TH14_CPP_STAGE_WORKER)
            __atomic_store_n(reinterpret_cast<u32*>(reinterpret_cast<u8*>(active_camera)+offset),word,__ATOMIC_SEQ_CST);
#else
            std::memcpy(reinterpret_cast<u8*>(active_camera)+offset,&word,4);
#endif
            if(camera_word_boundary){
                refresh_camera_references();
                if(!camera_word_boundary(*this,i+1)){error="loading during camera publication";return false;}
            }
        }
    }else *active_camera=state.camera;
#else
    *active_camera=state.camera;
#endif
    refresh_camera_references();return true;
}
bool Stage::update_deformation(){
    float phase_x=state.phase_x,phase_y=state.phase_y;
    constexpr double pi=3.1415927410125732421875;
    if(state.deformation_mode==1){
        mesh->rectangle(-192,0,384,448);
        for(u32 x=0;x<mesh->columns;++x){const float wave_y=float(double(float(std::sin(double(phase_y))))*12);
            for(u32 y=0;y<mesh->rows;++y){const u32 index=x*mesh->rows+y;auto& v=mesh->vertices[index];v.color=(v.color&0xffffff)|0xc0000000;
                const float wave_x=float(double(float(std::sin(double(phase_x))))*12);
                if(x&&y&&x+1<mesh->columns&&y+1<mesh->rows){v.position.x=float(double(v.position.x)+wave_x);v.position.y=float(double(v.position.y)+wave_y);v.position.z=mesh->positions[index].z=0;}
                phase_x=normalize_angle(float(double(phase_x)+.6981317400932312));
            }phase_y=normalize_angle(float(double(phase_y)-1.2566370964050293));
        }
        state.phase_x=normalize_angle(float(double(state.phase_x)+.04908738657832146));state.phase_y=normalize_angle(float(double(state.phase_y)+.039269909262657166));
    }else if(state.deformation_mode==2){
        const float radius=state.deformation_radius;if(state.deformation_target<radius)state.deformation_radius=float(double(radius)-2);
        mesh->rectangle(-radius,float(224.-radius),float(double(radius)*2),float(double(radius)*2));const double squared=double(radius)*radius;
        for(u32 i=0;i<mesh->vertices.size();++i){auto& v=mesh->vertices[i];const auto& p=mesh->positions[i];const Vec3 delta{float(double(p.x)-224),float(double(p.y)-240),p.z};
            const float distance=float(double(delta.x)*delta.x+double(delta.y)*delta.y),remaining=float(squared-distance);
            if(remaining<0)v.color&=0xffffff;
            else{const float amount=float(double(remaining)/squared),strength=float(double(amount)*32);v.color=0x60ffffff;auto direction=GraphicsMath::normalize(delta);
                direction.x=float(double(direction.x)*strength);direction.y=float(double(direction.y)*strength);
                direction.x=float(double(float(std::sin(double(phase_x))))*amount*8+direction.x);direction.y=float(double(float(std::sin(double(phase_y))))*amount*8+direction.y);
                v.position.x=float(double(v.position.x)+direction.x);v.position.y=float(double(v.position.y)+direction.y);v.position.z=mesh->positions[i].z=0;
            }
            phase_x=normalize_angle(float(double(phase_x)+pi*.5));phase_y=normalize_angle(float(double(phase_y)-.6981317400932312));
        }
        state.phase_x=normalize_angle(float(double(state.phase_x)+.04908738657832146));
        const float delta=float(double(manager.script_rng.unit())*pi/40+.039269909262657166);
        state.phase_y=normalize_angle(float(double(state.phase_y)+delta));
    }return true;
}
}
