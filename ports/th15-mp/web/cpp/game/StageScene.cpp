#include "StageScene.hpp"
namespace th15 {
StageScene::StageScene(const StageResource& f,AnmManager& a,AnmEnvironment& env,StageSceneServices& world,i32 id):file(f),animations(a),environment(env),services(world),bank(id),primitives(f.animation_count),object_flags(f.objects.size()),instance_flags(f.instances.size()),script(f,*this){}
StageScene::~StageScene(){for(auto& vm:primitives)animations.registry.destroy_tree(vm);for(auto& vm:embedded)animations.registry.destroy_tree(vm);}
bool StageScene::fail(const std::string& message){error=message;return false;}
bool StageScene::bind(AnmVm& vm,i32 index){if(!animations.bind_template(vm,bank,index))return fail(animations.error);vm.rotation_parent=vm.creation_parent=nullptr;if(animations.tick_instance(vm)<0)return fail(animations.error);return true;}
bool StageScene::initialize(const StageCamera& initial_camera){
    if(initialized)return fail("Background scene already initialized");if(!file.error.empty())return fail(file.error);if(!animations.resource(bank))return fail("Background animation bank unavailable");
    script.state.camera=initial_camera;view_direction=initial_camera.direction;script.state.camera.position={0,0,-600};script.state.camera.direction={0,300,600};script.state.camera.up={0,1,0};script.state.camera.eye_offset={};script.state.camera.target_offset={};script.state.timer.set(0);script.state.culling_distance_squared=9999999.f;
    for(u32 i=0;i<file.objects.size();++i){object_flags[i]=1;for(const auto& p:file.objects[i].primitives){if(p.animation>=primitives.size())return fail("Background primitive index outside pool");if(!bind(primitives[p.animation],p.script))return false;}}
    for(u32 i=0;i<file.instances.size();++i)instance_flags[i]=file.instances[i].flags;initialized=true;return true;
}
bool StageScene::clear_color(u32 value){color=value;return true;}
bool StageScene::animation(u32 index,i32 value,i32 layer){if(index>=embedded.size())return fail("Background embedded slot outside pool");auto& vm=embedded[index];if(value<0){vm.visual.flags&=~1u;vm.visible=false;}else if(!bind(vm,value))return false;return true;}
bool StageScene::deformation(i32 mode,i32 index){if(!services.begin_deformation(mode,index))return fail("Background deformation creation unavailable");deforming=true;return true;}
bool StageScene::object_interrupt(i32 label){return interrupt_objects(label);}
bool StageScene::interrupt(i32 label){if(!script.interrupt(label))return fail(script.error);return true;}
bool StageScene::interrupt_objects(i32 label){for(auto& vm:primitives)vm.pending_interrupt=label;return true;}
bool StageScene::update(const StageSceneFrame& f){
    if(!error.empty())return false;if(!initialized)return fail("Background scene not initialized");if((f.flags&8)||((f.flags&4)&&f.transition_age>=60))return true;rate=f.rate;animations.rate=rate;script.state.camera.animation_delta={};
    // The original normalizes the prior frame's direction before executing STD.
    const auto& d=script.state.camera.direction;const auto& o=script.state.camera.target_offset;const auto reference=normalize_scene_vector({float(d.x+o.x),float(d.y+o.y),float(d.z+o.z)});view_direction=reference;
    if(!(f.flags&4)||f.transition_age<30){for(u32 i=0;i<file.objects.size();++i){if(!(object_flags[i]&1))continue;u32 active=0;for(const auto& p:file.objects[i].primitives){auto& vm=primitives[p.animation];if(animations.tick_instance(vm)<0)return fail(animations.error);if(vm.instruction_offset>=0)active++;}if(!active)object_flags[i]&=~1u;}if(!script.step(rate))return fail(script.error);}
    // STD camera delta belongs to the background camera snapshot (native
    // 0x4e7d94). It must not overwrite the independent world scroll vector
    // (0x51bc94) consumed by player, enemies, projectiles and ANM updates.
    environment.camera_origin=script.state.camera.eye_offset;environment.screen_translation=script.state.camera.position;environment.reference_position=view_direction;
    for(auto& vm:embedded)if(animations.tick_instance(vm)<0)return fail(animations.error);
    if(deforming&&!services.update_deformation(script.state,rate))return fail("Background deformation update unavailable");frames++;return true;
}
bool StageScene::draw(AnmRenderer& renderer,ZunGraphics& graphics,i32 layer,const GraphicsViewport& viewport,Vec2 origin){
    if(!error.empty())return false;if(!initialized)return fail("Background scene not initialized");auto camera_state=script.state.camera;camera_state.direction=view_direction;const auto camera=stage_camera(camera_state,viewport);renderer.set_viewport(viewport);renderer.set_camera(camera);graphics.presentation_camera(true);
    for(u32 i=0;i<embedded.size();++i)if(script.state.animation_layers[i]==layer){renderer.flush();graphics.set_fog(false);graphics.set_depth_mask(false);if(renderer.draw(embedded[i])==-2)return fail(renderer.error);}
    renderer.flush();if(!draw_objects){graphics.set_depth_mask(false);return true;}graphics.set_fog(true);
    for(u32 i=0;i<file.instances.size();++i){renderer.presentation_instance=i+1;const auto& instance=file.instances[i];const auto& object=file.objects[u32(instance.object)];if(i32(i8(object.layer))!=layer)continue;
        if(!stage_visible(object,instance.position,camera,viewport,origin,script.state.culling_distance_squared)){culled_instances++;instance_flags[i]&=~1;continue;}object_flags[u32(instance.object)]|=2;
        for(const auto& p:object.primitives){if(p.type!=0)continue;auto& vm=primitives[p.animation];auto& v=vm.visual;const u32 mode=v.draw_mode();if(mode>=4){v.translation={float(p.position.x+instance.position.x),float(p.position.y+instance.position.y),float(p.position.z+instance.position.z)};// Native 40f99c looks up the selected bank/index only for a nonzero
            // STD size. A delayed sprite instruction can leave vm.sprite null.
            if(p.size.x!=0||p.size.y!=0){auto* source=vm.sprite_resource?vm.sprite_resource:vm.resource;if(!source||v.sprite<0||u32(v.sprite)>=source->sprites.size())return fail("Background primitive sprite unavailable");const auto& sprite=source->sprites[u32(v.sprite)];if(p.size.x!=0){v.scale.x=float(p.size.x/sprite.width);v.flags|=8;}if(p.size.y!=0){v.scale.y=float(p.size.y/sprite.height);v.flags|=8;}}}
            const bool fog=mode==8||mode==24,depth_write=!(v.flags&0x2000);if(graphics.pipeline().fog!=fog||graphics.pipeline().depthWrite!=depth_write)renderer.flush();graphics.set_depth_mask(depth_write);graphics.set_fog(fog);graphics.set_fog_color(camera.fog_color);graphics.set_fog_range(camera.fog_near,camera.fog_far);if(renderer.draw(vm)==-2)return fail(renderer.error);drawn_primitives++;
        }instance_flags[i]|=1;drawn_instances++;
    }renderer.presentation_instance=0;renderer.flush();graphics.set_depth_mask(false);return true;
}
AnmVm* StageScene::primitive(u32 index)noexcept{return index<primitives.size()?&primitives[index]:nullptr;}
AnmVm* StageScene::slot(u32 index)noexcept{return index<embedded.size()?&embedded[index]:nullptr;}
u8 StageScene::object_state(u32 index)const noexcept{return index<object_flags.size()?object_flags[index]:0;}
i16 StageScene::instance_state(u32 index)const noexcept{return index<instance_flags.size()?instance_flags[index]:0;}
}
