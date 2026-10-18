#include "Stage.hpp"
#include "GraphicsMath.hpp"
#include "Movement.hpp"
#include <cmath>
namespace th11 {
void Stage::stage_camera_position_interpolated(){
#ifdef TH_ENABLE_THPRAC
    // THPrac 404663, immediately after STD opcode 3. The portable battle
    // increments frame at the end of its tick; native priority-10 control
    // increments before priority-12 STD, hence native current = frame + 1.
    if(!practice_replay_has_stage5||state.stage_number!=6||state.script_timer.current||!practice_game_frame)return;
    const i32 offset=wrapping_add(i32(*practice_game_frame),-1);
    if(offset){state.script_timer.current=offset;state.script_timer.fractional=float(offset);state.position_interpolation.timer.current=offset;state.position_interpolation.timer.fractional=float(offset);}
#endif
}
Stage::~Stage(){for(auto& vm:object_animations)manager.release_geometry(vm);for(auto& vm:state.script_animations)manager.release_geometry(vm);for(auto& vm:state.effect_animations)manager.release_geometry(vm);}
bool Stage::initialize(const StageResource& file,i32 number,const SceneCamera& camera,bool start){
    if(!object_animations.empty()||file.instructions.empty())return false;
    resource=file;std::memset(&state,0,sizeof state);state.flags=2;state.stage_number=number;state.camera=camera;
    state.camera.position={0,0,-600};state.camera.direction={0,300,600};state.camera.up={0,1,0};state.camera.eye_offset={};
    state.draw_distance_squared=9610000;state.draw_flags=1;state.script_timer.set(0,&manager.rate);
    return !start||start_objects();
}
bool Stage::start_objects(){
    if(!object_animations.empty())return false;
    object_animations.resize(resource.animation_count);
    for(auto& object:resource.objects){object.flags=1;for(const auto& primitive:object.primitives)if(!stage_animation(object_animations[primitive.animation],primitive.script))return false;}
    return true;
}
bool Stage::stage_animation(AnmVm& vm,i32 script){
    manager.release_geometry(vm);if(!vm.bind_script(animations,script,file_id,&manager.rate)){error="invalid stage animation script";return false;}
    // 405030 writes 16 before 44acd0, whose second initialization clears it.
    if(vm.update(manager)==-2){error="stage animation execution failed";return false;}return true;
}
bool Stage::stage_deformation(i32 mode){
    mesh.reset();state.deformation=0;auto next=std::make_unique<ScreenDeformation>(manager);
    if(!next->initialize(text,17,mode==1?25:17,false)){error="stage deformation initialization failed";return false;}
    mesh=std::move(next);state.deformation=1;return true;
}
bool Stage::update_objects(){
    for(auto& object:resource.objects){if(!(object.flags&1))continue;u32 active=0;
        for(const auto& primitive:object.primitives){auto& vm=object_animations[primitive.animation];if(vm.update(manager)==-2){error="stage object animation failed";return false;}if(vm.instruction)++active;}
        if(!active)object.flags&=~1u;
    }return true;
}
bool Stage::update(){
    if((state.draw_flags&8)||((state.draw_flags&4)&&state.fade_timer.current>=60))return true;
    state.camera.offset={};state.camera.animation_delta={};state.camera.reserved24=GraphicsMath::normalize(state.camera.direction);state.tint=0x00808080;
    if(!(state.draw_flags&4)||state.fade_timer.current<30){
        if(!update_objects()||!update_script())return false;
        for(auto& vm:state.script_animations)if(vm.update(manager)==-2){error="stage standalone animation failed";return false;}
        if(state.frame_effect){const float saved_rate=manager.rate;manager.rate=1;bool ok=true;for(auto& vm:state.effect_animations)if(vm.update(manager)==-2)ok=false;manager.rate=saved_rate;if(!ok){error="stage effect animation failed";return false;}}
    }
    state.frame_effect=0;publish_camera();if(mesh&&!update_deformation())return false;++state.frame_count;return true;
}
void Stage::publish_camera(){
    if(!active_camera)return;
    *active_camera=state.camera;
    manager.reference_positions[0]=active_camera->position;
    manager.reference_positions[1]=active_camera->reserved24;
    manager.camera_delta=active_camera->animation_delta;
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
