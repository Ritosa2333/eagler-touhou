#pragma once
#include "../../cpp/game/StageScript.hpp"
#include "../../cpp/game/StageCamera.hpp"
#include "../../cpp/game/StageScene.hpp"
struct StageScriptFixture final:StageScriptWorld {
    StageScript script;std::vector<u32> events;std::array<u32,8> animation_flags{};std::array<i32,8> animation_cursors{};u32 clear=0;bool available=true;
    explicit StageScriptFixture(StageResource& file):script(file,*this){}
    bool clear_color(u32 color)override{clear=color;events.insert(events.end(),{0,color});return available;}
    bool animation(u32 index,i32 value,i32)override{if(value==-2||value==-1)animation_flags[index]&=~1u;if(value==-1)animation_cursors[index]=-1;if(value>=0){animation_flags[index]=1;events.insert(events.end(),{1,index,u32(value)});}return available;}
    bool deformation(i32,i32 id)override{events.insert(events.end(),{2,u32(id)});return available;}
    bool object_interrupt(i32 id)override{events.insert(events.end(),{3,u32(id)});return available;}
};
API(stage_resource_create) StageResource* stage_resource_create(){return new StageResource;}
API(stage_resource_delete) void stage_resource_delete(StageResource* p){delete p;}
API(stage_resource_open) u32 stage_resource_open(StageResource* p,const u8* bytes,u32 size){return p->open(bytes,size);}
API(stage_resource_error) const char* stage_resource_error(StageResource* p){return p->error.c_str();}
API(stage_resource_name) const char* stage_resource_name(StageResource* p){return p->animation_name.c_str();}
API(stage_resource_count) u32 stage_resource_count(StageResource* p,u32 field){switch(field){case 0:return p->objects.size();case 1:return p->animation_count;case 2:return p->instances.size();case 3:return p->instructions.size();default:return 0;}}
API(stage_script_create) StageScriptFixture* stage_script_create(StageResource* p){return new StageScriptFixture(*p);}
API(stage_script_delete) void stage_script_delete(StageScriptFixture* p){delete p;}
API(stage_script_step) u32 stage_script_step(StageScriptFixture* p,float rate){return p->script.step(rate);}
API(stage_script_interrupt) u32 stage_script_interrupt(StageScriptFixture* p,i32 id){return p->script.interrupt(id);}
API(stage_script_error) const char* stage_script_error(StageScriptFixture* p){return p->script.error.c_str();}
API(stage_script_available) void stage_script_available(StageScriptFixture* p,u32 value){p->available=value;}
API(stage_script_event_reset) void stage_script_event_reset(StageScriptFixture* p){p->events.clear();}
API(stage_script_event_count) u32 stage_script_event_count(StageScriptFixture* p){return p->events.size();}
API(stage_script_events) u32* stage_script_events(StageScriptFixture* p){return p->events.data();}
API(stage_script_clear) u32 stage_script_clear(StageScriptFixture* p){return p->clear;}
API(stage_script_snapshot) void stage_script_snapshot(StageScriptFixture* p,u8* out){
 const auto& s=p->script.state;std::memset(out,0,0x33a8);std::memcpy(out,&s.timer,20);std::memcpy(out+0x14,&s.instruction_offset,4);out[0x18]=s.camera_effect;std::memcpy(out+0x1c,&s.effect_timer,20);std::memcpy(out+0x30,&s.effect_fade,20);
 std::memcpy(out+0x44,&s.direction,88);std::memcpy(out+0x9c,&s.position,88);std::memcpy(out+0xf4,&s.up,88);std::memcpy(out+0x14c,&s.fog,168);std::memcpy(out+0x1f4,&s.fov,48);
 std::memcpy(out+0x224,&s.camera.position,12);std::memcpy(out+0x230,&s.camera.direction,12);std::memcpy(out+0x23c,&s.camera.up,12);std::memcpy(out+0x260,&s.camera.eye_offset,12);std::memcpy(out+0x26c,&s.camera.target_offset,12);std::memcpy(out+0x278,&s.camera.fov,4);std::memcpy(out+0x328,&s.camera.animation_delta,12);std::memcpy(out+0x334,&s.camera.fog,28);
 for(u32 i=0;i<8;++i){std::memcpy(out+0x354+i*0x600+0x530,&p->animation_flags[i],4);std::memcpy(out+0x354+i*0x600+0x28,&p->animation_cursors[i],4);}
 std::memcpy(out+0x3354,s.animation_layers.data(),32);std::memcpy(out+0x3374,&s.culling_distance_squared,4);std::memcpy(out+0x337c,&s.deformation_target,4);std::memcpy(out+0x3380,&s.deformation_radius,4);std::memcpy(out+0x3384,&s.deformation_color,4);std::memcpy(out+0x3388,&s.deformation_timer,20);std::memcpy(out+0x339c,&s.phase_x,4);std::memcpy(out+0x33a0,&s.phase_y,4);std::memcpy(out+0x33a4,&s.deformation_mode,4);
}
API(stage_camera_snapshot) void stage_camera_snapshot(const u8* data,const GraphicsViewport* vp,u8* out){StageCamera s;std::memcpy(&s.position,data,12);std::memcpy(&s.direction,data+0x24,12);std::memcpy(&s.up,data+0x18,12);std::memcpy(&s.eye_offset,data+0x3c,12);std::memcpy(&s.fov,data+0x54,4);const auto c=stage_camera(s,*vp);std::memcpy(out,&c.view,64);std::memcpy(out+64,&c.projection,64);std::memcpy(out+128,&c.reference,12);}
API(anm_camera_create) AnmCamera* anm_camera_create(){return new AnmCamera;}
API(anm_camera_delete) void anm_camera_delete(AnmCamera* p){delete p;}
API(stage_visibility) u32 stage_visibility(const u8* object,const Vec3* instance,AnmCamera* camera,const GraphicsViewport* viewport,float x,float y,float radius,Vec3* out){StageObject o;std::memcpy(&o.position,object+4,12);std::memcpy(&o.size,object+16,12);std::array<Vec3,16> p{};const bool visible=stage_visible(o,*instance,*camera,*viewport,{x,y},radius,&p);std::memcpy(out,p.data(),192);return visible;}
struct StageSceneFixture final:StageSceneServices {
    StageScene scene;bool available=false;StageSceneFixture(StageResource& f,AnmManagerFixture& a,i32 bank):scene(f,a.manager,a.environment,*this,bank){}
    bool begin_deformation(i32,i32)override{return available;}bool update_deformation(StageScriptState&,float)override{return available;}
};
API(stage_scene_create) StageSceneFixture* stage_scene_create(StageResource* f,AnmManagerFixture* a,i32 bank){return new StageSceneFixture(*f,*a,bank);}
API(stage_scene_delete) void stage_scene_delete(StageSceneFixture* p){delete p;}
API(stage_scene_initialize) u32 stage_scene_initialize(StageSceneFixture* p,float fov){StageCamera camera;camera.fov=fov;camera.direction={0,0,1};return p->scene.initialize(camera);}
API(stage_scene_error) const char* stage_scene_error(StageSceneFixture* p){return p->scene.error.c_str();}
API(stage_scene_update) u32 stage_scene_update(StageSceneFixture* p,float rate,u32 flags,i32 age){return p->scene.update({rate,flags,age});}
API(stage_scene_interrupt) u32 stage_scene_interrupt(StageSceneFixture* p,i32 label,u32 objects){return objects?p->scene.interrupt_objects(label):p->scene.interrupt(label);}
API(stage_scene_vm) AnmVm* stage_scene_vm(StageSceneFixture* p,u32 index,u32 slots){return slots?p->scene.slot(index):p->scene.primitive(index);}
API(stage_scene_object_flags) u32 stage_scene_object_flags(StageSceneFixture* p,u32 index){return p->scene.object_state(index);}
API(stage_scene_instance_flags) i32 stage_scene_instance_flags(StageSceneFixture* p,u32 index){return p->scene.instance_state(index);}
API(stage_scene_draw) u32 stage_scene_draw(StageSceneFixture* p,AnmRendererFixture* r,i32 layer,const GraphicsViewport* viewport,float x,float y){return p->scene.draw(r->renderer,r->graphics,layer,*viewport,{x,y});}
API(stage_scene_direction) const Vec3* stage_scene_direction(StageSceneFixture* p){return &p->scene.camera_direction();}
API(stage_scene_color) u32 stage_scene_color(StageSceneFixture* p){return p->scene.color;}
API(stage_scene_frames) u32 stage_scene_frames(StageSceneFixture* p){return p->scene.frames;}
API(stage_scene_draw_stats) void stage_scene_draw_stats(StageSceneFixture* p,u32* out,u32 reset){out[0]=p->scene.drawn_instances;out[1]=p->scene.culled_instances;out[2]=p->scene.drawn_primitives;if(reset)p->scene.drawn_instances=p->scene.culled_instances=p->scene.drawn_primitives=0;}
#include "../../cpp/game/StageDeformation.hpp"
struct StageDeformationFixture {ScreenGridViewport view;StageScriptState state;AnmManager& animations;StageDeformation effect;StageDeformationFixture(AnmManagerFixture& a,i32 bank):animations(a.manager),effect(a.manager,a.random,view,bank){}}
;
API(stage_deformation_create) StageDeformationFixture* stage_deformation_create(AnmManagerFixture* a,i32 bank){return new StageDeformationFixture(*a,bank);}
API(stage_deformation_delete) void stage_deformation_delete(StageDeformationFixture* p){delete p;}
API(stage_deformation_begin) u32 stage_deformation_begin(StageDeformationFixture* p,i32 mode){p->state.deformation_mode=mode;return p->effect.begin(mode);}
API(stage_deformation_field) void* stage_deformation_field(StageDeformationFixture* p,u32 kind){switch(kind){case 0:return &p->state.deformation_target;case 1:return &p->view;case 2:return p->effect.mesh.grid.vertices.data();case 3:return p->effect.mesh.grid.sampling.data();case 4:return &p->state.phase_x;case 5:return &p->state.deformation_mode;case 6:return &p->state.deformation_timer;default:return nullptr;}}
API(stage_deformation_handle) u32 stage_deformation_handle(StageDeformationFixture* p,u32 index){return index==0?p->effect.mesh.capture_handle:index<=16?p->effect.mesh.strip_handles[index-1]:0;}
API(stage_deformation_update) u32 stage_deformation_update(StageDeformationFixture* p,u32 spell,u32 draw,float rate){return p->effect.update(p->state,spell,rate)&&(!draw||p->effect.prepare_draw());}
API(stage_deformation_error) const char* stage_deformation_error(StageDeformationFixture* p){return p->effect.error.c_str();}
