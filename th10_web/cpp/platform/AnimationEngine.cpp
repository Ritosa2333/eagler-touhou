#include "../game/CallbackNames.hpp"
#include "../game/HighRefresh.hpp"
#include "AnimationEngine.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "../multiplayer/RollbackState.hpp"
#endif
#include <algorithm>
#include <cmath>
#include <cstdlib>
namespace th10::browser {
namespace {
u32 pointer(const void* p){return static_cast<u32>(reinterpret_cast<uintptr_t>(p));}
constexpr CallbackToken animation_callbacks[]={callback_id::AnimationsWorld,callback_id::AnimationsUI,callback_id::AnimationLayer0,callback_id::AnimationLayer1,callback_id::AnimationLayer2,callback_id::AnimationLayer3,callback_id::AnimationLayer4,callback_id::AnimationLayer5,callback_id::AnimationLayer6,callback_id::AnimationLayer7,callback_id::AnimationLayer8,callback_id::AnimationLayer9,callback_id::AnimationLayer10,callback_id::AnimationLayer11,callback_id::AnimationLayer12,callback_id::AnimationLayer13,callback_id::AnimationLayer14,callback_id::AnimationLayer15,callback_id::AnimationLayer16,callback_id::AnimationLayer19};
constexpr CallbackToken layer_callbacks[]={callback_id::AnimationLayer0,callback_id::AnimationLayer1,callback_id::AnimationLayer2,callback_id::AnimationLayer3,callback_id::AnimationLayer4,callback_id::AnimationLayer5,callback_id::AnimationLayer6,callback_id::AnimationLayer7,callback_id::AnimationLayer8,callback_id::AnimationLayer9,callback_id::AnimationLayer10,callback_id::AnimationLayer11,callback_id::AnimationLayer12,callback_id::AnimationLayer13,callback_id::AnimationLayer14,callback_id::AnimationLayer15,callback_id::AnimationLayer16,0,0,callback_id::AnimationLayer19};
u16 continuous_fields(const AnmVm& vm){
    u16 fields=0;
    if(vm.rotation_interpolation.duration>0||vm.angular_velocity.x!=0||vm.angular_velocity.y!=0||vm.angular_velocity.z!=0)fields|=1;
    if(vm.scale_interpolation.duration>0||vm.scale_velocity.x!=0||vm.scale_velocity.y!=0)fields|=2;
    if(vm.color_interpolation.duration>0)fields|=4;if(vm.alpha_interpolation.duration>0)fields|=8;
    if(vm.color2_interpolation.duration>0)fields|=16;if(vm.alpha2_interpolation.duration>0)fields|=32;
    if(vm.uv_velocity.x!=0)fields|=64;if(vm.uv_velocity.y!=0)fields|=128;return fields;
}
float presentation_angle(float previous,float current){constexpr float pi=3.1415927410125732f,tau=6.2831854820251465f;float delta=current-previous;if(delta>pi)delta-=tau;else if(delta<-pi)delta+=tau;return previous+delta*high_refresh::alpha;}
float presentation_uv(float previous,float current){float delta=current-previous;if(delta>.5f)delta-=1;else if(delta<-.5f)delta+=1;float value=previous+delta*high_refresh::alpha;if(value>=1)value-=1;else if(value<0)value+=1;return value;}
u32 presentation_channel(u32 before,u32 current,u32 shift){const float value=high_refresh::lerp(float((before>>shift)&255),float((current>>shift)&255));return u32(std::clamp(value,0.0f,255.0f))<<shift;}
AnimationEngine::PresentationVmSample presentation_sample(const AnmVm& vm){
    return {vm.id,vm.script_index,vm.sprite_index,vm.animation_file,vm.position,vm.script_position,vm.child_position,vm.rotation,vm.scale,vm.uv_offset,vm.color,vm.secondary_color,vm.flags&3u,vm.script_timer.current,continuous_fields(vm)};
}
}
AnimationEngine::AnimationEngine(FileSystem& f,GraphicsDevice& d,Rng& script,Rng& visual,float& s):device(d),script_random(script),visual_random(visual),speed(s),callback_environment(*this),resources(f,d,manager){
    rate=&speed;script_rng=&script_random;visual_rng=&visual_random;reference_positions[0]=&world.position;reference_positions[1]=&world.reserved_024;camera_delta=&world.animation_delta;default_tangent=&tangent;
    initial_quad=initial_vertices;render_quad=vertices;model_quad=model_template;chain=&chain_value;callbacks=&callback_environment;
    std::memcpy(frame_callbacks,animation_callbacks,sizeof(frame_callbacks));allocation=this;
    AnmDistortionEnvironment::random=&script_random;update_callback=callback_id::DistortionUpdate;draw_callback=callback_id::DistortionDraw;

    callback_environment.bind(callback_id::AnimationsWorld,this,[](void* p,void* argument,i32){return static_cast<AnmManager*>(argument)->update_world(*static_cast<AnimationEngine*>(p));});
    callback_environment.bind(callback_id::AnimationsUI,this,[](void* p,void* argument,i32){return static_cast<AnmManager*>(argument)->update_ui(*static_cast<AnimationEngine*>(p));});
    for(i32 i=0;i<20;i++)if(layer_callbacks[i])callback_environment.bind(layer_callbacks[i],this,[](void* p,void*,i32 layer){return static_cast<AnimationEngine*>(p)->draw_layer(layer);},i);
    callback_environment.bind(callback_id::DistortionUpdate,this,[](void*,void* o,i32){auto& vm=*static_cast<AnmVm*>(o);return static_cast<AnmDistortion*>(vm.geometry)->update(vm);});
    callback_environment.bind(callback_id::DistortionDraw,this,[](void* p,void* o,i32){auto& s=*static_cast<AnimationEngine*>(p);auto& vm=*static_cast<AnmVm*>(o);auto env=s.renderer();AnmRenderer{s.manager,env}.draw_textured_fan(vm,static_cast<AnmDistortion*>(vm.geometry)->vertices,33);return 0;});
    chain_value.initialize();manager.initialize(*this);manager.initialize_model(*this);Camera::initialize(world,ui);configure_camera(true);
}
AnimationEngine::~AnimationEngine(){
    chain_value.clear_list(chain_value.update,callback_environment);chain_value.clear_list(chain_value.draw,callback_environment);manager.release(*this);
    for(i32 slot=0;slot<AnmManager::file_slot_count;++slot)manager.unload(slot,resources);
    if(manager.model_vertex_buffer)device.release_resource(manager.model_vertex_buffer);
}
GraphicsRenderer AnimationEngine::renderer(){return GraphicsRenderer(device,manager,*active,world,vertices);}
#ifndef TH_NATIVE_PLATFORM
bool AnimationEngine::invoke(CallbackToken token,void* object,i32& result){
    if(token==callback_id::AnimationsWorld){result=static_cast<AnmManager*>(object)->update_world(*this);return true;}
    if(token==callback_id::AnimationsUI){result=static_cast<AnmManager*>(object)->update_ui(*this);return true;}
    for(u32 i=0;i<20;++i)if(layer_callbacks[i]&&layer_callbacks[i]==token){result=draw_layer(i);return true;}
    if(token==callback_id::DistortionUpdate){auto& vm=*static_cast<AnmVm*>(object);result=static_cast<AnmDistortion*>(vm.geometry)->update(vm);return true;}
    if(token==callback_id::DistortionDraw){auto& vm=*static_cast<AnmVm*>(object);auto env=renderer();AnmRenderer{manager,env}.draw_textured_fan(vm,static_cast<AnmDistortion*>(vm.geometry)->vertices,33);result=0;return true;}
    const auto cached=resolved_callbacks.find(token);
    if(cached!=resolved_callbacks.end())return cached->second->invoke(token,object,result);
    for(auto* receiver:receivers)if(receiver&&receiver->invoke(token,object,result)){resolved_callbacks[token]=receiver;return true;}
    return application_callbacks&&application_callbacks->invoke(token,object,result);
}
#endif
void AnimationEngine::register_receiver(CallbackReceiver& receiver){receiver.bind_callbacks(callback_environment);
#ifndef TH_NATIVE_PLATFORM
resolved_callbacks.clear();for(auto*& slot:receivers)if(!slot){slot=&receiver;return;}__builtin_trap();
#endif
}
void AnimationEngine::unregister_receiver(CallbackReceiver& receiver){callback_environment.unbind(receiver.callback_context);
#ifndef TH_NATIVE_PLATFORM
resolved_callbacks.clear();for(auto*& slot:receivers)if(slot==&receiver)slot=nullptr;
#endif
}
void AnimationEngine::callback(u32 token,AnmVm& vm){callback_environment.invoke(token,&vm);}
i32 AnimationEngine::update(AnmVm& vm){
    // Registry-owned VMs are captured in snapshot_presentation() before the
    // fixed tick. Embedded VMs (HUD, spell digits, markers, etc.) are not in
    // that registry, so remember their first pre-update state here instead.
    presentation_previous.try_emplace(&vm,presentation_sample(vm));return vm.update(*this);
}
bool AnimationEngine::present(AnmVm& copy,const AnmVm& source) const{
    if(!high_refresh::render_only||!high_refresh::active)return false;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    const auto* sample=presentation_previous.find(&source);if(!sample)return false;const auto& before=*sample;
#else
    const auto found=presentation_previous.find(&source);if(found==presentation_previous.end())return false;const auto& before=found->second;
#endif
    // Visibility changes, timer rewinds and script/file changes are lifecycle
    // boundaries. Snap instead of blending from a stale incarnation.
    if(before.id!=source.id||before.script_index!=source.script_index||before.file!=source.animation_file||before.visible!=(source.flags&3u)||source.script_timer.current<before.script_time)return false;
    const auto near=[](const Vec3& a,const Vec3& b){const float dx=a.x-b.x,dy=a.y-b.y,dz=a.z-b.z;return dx*dx+dy*dy+dz*dz<16384.0f;};
    const auto mix=[](const Vec3& a,const Vec3& b){return Vec3{high_refresh::lerp(a.x,b.x),high_refresh::lerp(a.y,b.y),high_refresh::lerp(a.z,b.z)};};
    if(near(before.position,source.position))copy.position=mix(before.position,source.position);if(near(before.script_position,source.script_position))copy.script_position=mix(before.script_position,source.script_position);if(near(before.child_position,source.child_position))copy.child_position=mix(before.child_position,source.child_position);
    const u16 continuous=before.continuous|continuous_fields(source);
    if(continuous&1)copy.rotation={presentation_angle(before.rotation.x,source.rotation.x),presentation_angle(before.rotation.y,source.rotation.y),presentation_angle(before.rotation.z,source.rotation.z)};
    if((continuous&2)&&before.scale.x*source.scale.x>=0&&before.scale.y*source.scale.y>=0)copy.scale={high_refresh::lerp(before.scale.x,source.scale.x),high_refresh::lerp(before.scale.y,source.scale.y)};
    if(continuous&4)copy.color=(copy.color&0xff000000)|presentation_channel(before.color,source.color,0)|presentation_channel(before.color,source.color,8)|presentation_channel(before.color,source.color,16);
    if(continuous&8)copy.color=(copy.color&0x00ffffff)|presentation_channel(before.color,source.color,24);
    if(continuous&16)copy.secondary_color=(copy.secondary_color&0xff000000)|presentation_channel(before.secondary_color,source.secondary_color,0)|presentation_channel(before.secondary_color,source.secondary_color,8)|presentation_channel(before.secondary_color,source.secondary_color,16);
    if(continuous&32)copy.secondary_color=(copy.secondary_color&0x00ffffff)|presentation_channel(before.secondary_color,source.secondary_color,24);
    if(before.sprite_index==source.sprite_index){if(continuous&64)copy.uv_offset.x=presentation_uv(before.uv_offset.x,source.uv_offset.x);if(continuous&128)copy.uv_offset.y=presentation_uv(before.uv_offset.y,source.uv_offset.y);}
    return true;
}
void AnimationEngine::draw(AnmVm& vm){
    auto env=renderer();if(!high_refresh::render_only||!high_refresh::active){AnmRenderer{manager,env}.draw(vm);return;}
    auto copy=vm;present(copy,vm);
    AnmRenderer{manager,env}.draw(copy);
}
void AnimationEngine::bind_sprite(AnmVm& vm,i32 index){vm.animation_file->bind_sprite(vm,index);}
void AnimationEngine::change_draw_mode(AnmVm& vm){AnmDistortion::initialize(vm,*this);}
void* AnimationEngine::allocate_geometry(u32 bytes){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    return rollback_geometry.allocate(bytes,true);
#else
    return std::malloc(bytes);
#endif
}
AnmVm* AnimationEngine::spawn_child(AnmVm& parent,i32 script,u32 mode){
    const auto placement=mode==88?AnimationPlacement::WorldBack:mode==90?AnimationPlacement::UiBack:mode==91?AnimationPlacement::WorldFront:AnimationPlacement::UiFront;
    u32 id=manager.create(*parent.animation_file,script,parent.owner_tag,placement,*this,*this);return manager.registry.find_and_clear(id);
}
AnmVm* AnimationEngine::allocate_animation(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    return static_cast<AnmVm*>(rollback_animation_overflow.allocate(sizeof(AnmVm),true));
#else
    return static_cast<AnmVm*>(std::malloc(sizeof(AnmVm)));
#endif
}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
void AnimationEngine::preserve_animation_slot(AnmVm& vm){
    if(rollback_state)(void)rollback_state->Touch(&vm,sizeof(vm));
}
#endif
void AnimationEngine::release_memory(void* p){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(p&&rollback_animation_overflow.owns(p)){
        if(!rollback_animation_overflow.release(p))__builtin_trap();
        return;
    }
    if(p&&rollback_geometry.owns(p)){
        if(!rollback_geometry.release(p))__builtin_trap();
        return;
    }
#endif
    std::free(p);
}
void* AnimationEngine::allocate(u32 bytes){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    return rollback_geometry.allocate(bytes,true);
#else
    return std::malloc(bytes);
#endif
}
void AnimationEngine::release(void* p){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(p&&rollback_geometry.owns(p)){
        if(!rollback_geometry.release(p))__builtin_trap();
        return;
    }
#endif
    std::free(p);
}
void AnimationEngine::clear_pixel_shader(){device.clear_shader();}
void AnimationEngine::create_model_buffer(void*& buffer){device.create_vertices(80,Layouts::World,buffer);}
void* AnimationEngine::lock_model_buffer(void* buffer){return device.map_vertices(buffer);}
void AnimationEngine::unlock_model_buffer(void* buffer){device.unmap_vertices(buffer);}
void AnimationEngine::bind_model_buffer(void* buffer){device.vertex_buffer(buffer,20);}
void AnimationEngine::begin_frame(){auto env=renderer();AnmRenderer{manager,env}.begin_frame();}
void AnimationEngine::flush(){auto env=renderer();AnmRenderer{manager,env}.flush();}
i32 AnimationEngine::update_all(){return chain_value.run(false,callback_environment);}
i32 AnimationEngine::draw_all(){return chain_value.run(true,callback_environment);}
i32 AnimationEngine::draw_layer(u32 layer){auto env=renderer();GraphicsCamera camera(env);return AnmLayers{world,ui,active,screen_space,fog_enabled,camera,*this}.draw(manager,layer);}
void AnimationEngine::configure_camera(bool flat){auto env=renderer();GraphicsCamera camera(env);if(flat)active->configure_flat(camera);else active->configure_world(camera);camera.set_viewport(active->viewport);}
void AnimationEngine::snapshot_presentation(){
    presentation_previous.clear();
    for(auto* node: {manager.registry.world_head,manager.registry.ui_head})while(node){const auto* vm=node->value;node=node->next;if(vm){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        presentation_previous.try_emplace(vm,presentation_sample(*vm));
#else
        presentation_previous[vm]=presentation_sample(*vm);
#endif
    }}
}
}
