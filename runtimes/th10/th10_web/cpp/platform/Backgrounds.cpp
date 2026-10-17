#include "../game/CallbackNames.hpp"
#include "Backgrounds.hpp"
#include "../game/HighRefresh.hpp"
#include <cstdlib>
#include <vector>
namespace th10::browser {
BackgroundScript::BackgroundScript(Backgrounds& o):owner(o){rate=&o.engine.speed;background_color=&o.state.background_color;world=&o.engine.world;}
i32 BackgroundScript::update_animation(AnmVm& vm){return owner.engine.update(vm);}
void BackgroundScript::initialize_animation(AnmFile& file,AnmVm& vm,i32 id){initialize_embedded_animation(file,vm,id,owner.engine,owner.engine.manager.started_scripts);}
void BackgroundScript::normalize(Vec3& output,const Vec3& input){GraphicsMath::normalize(output,input);}
BackgroundDraw::BackgroundDraw(Backgrounds& o):owner(o),renderer(o.engine.renderer()),graphics_camera(renderer){world=&o.engine.world;active=&o.engine.active;screen_space=&o.engine.screen_space;fog_enabled=&o.engine.fog_enabled;rate=&o.engine.speed;camera=&graphics_camera;}
void BackgroundDraw::translation(Matrix4& matrix,const Vec3& position){GraphicsMath::translation(matrix,position);}
void BackgroundDraw::project_points(Vec3* output,const Vec3* input,u32 count,const Camera& camera,const Matrix4& world){GraphicsMath::project_array(output,12,input,12,count,&camera.viewport,&camera.projection,&camera.view,&world);}
void BackgroundDraw::draw_animation(AnmVm& vm){owner.engine.draw(vm);}
void BackgroundDraw::draw_layer(u32 layer){owner.engine.manager.draw_layer(layer,owner.engine);}
void BackgroundDraw::clear(u32 flags,u32 color,const StageClearRect* rectangle){owner.engine.device.clear_target(flags,color,1.f,0,reinterpret_cast<const i32*>(rectangle),rectangle?1u:0u);}
void BackgroundDraw::fade(i32 kind,i32 duration){if(!high_refresh::render_only)ScreenEffect::create(static_cast<ScreenEffectKind>(kind),duration,0,0,0,15,owner.effects);}
Backgrounds::Backgrounds(GameState& s,AnimationEngine& e,ScreenEffects& fx,FileSystem& f):state(s),engine(e),effects(fx),files(f),script(*this){
    background=&current;overlay=&previous;stage_number=&s.game.stage;game_flags=reinterpret_cast<const u8*>(&s.game.flags);world=&e.world;rate=&e.speed;filename=source_name;animation_slots=e.manager.files;
    chain=&e.chain_value;callbacks=&e.callback_environment;update_callback=callback_id::StageUpdate;background_callback=callback_id::StageBackground;foreground_callback=callback_id::StageForeground;e.register_receiver(*this);
}
Backgrounds::~Backgrounds(){destroy(current);destroy(previous);engine.unregister_receiver(*this);}
Stage* Backgrounds::create(const char* name,i32 offset){if(offset?previous:current)__builtin_trap();return StageResources::create(name,offset,*this);}
void Backgrounds::destroy(Stage* stage){if(stage){for(auto& entry:presentation)if(entry.owner==stage)entry={};StageResources{*stage,*this}.release();std::free(stage);}}
void Backgrounds::snapshot(Stage& stage){
    PresentationStage* slot=nullptr;for(auto& entry:presentation)if(entry.owner==&stage){slot=&entry;break;}
    if(!slot)for(auto& entry:presentation)if(!entry.owner){slot=&entry;break;}if(!slot)slot=&presentation[0];
    slot->owner=&stage;slot->camera=stage.camera;slot->valid=true;
}
Camera Backgrounds::presentation_camera(const Stage& stage)const{
    Camera result=stage.camera;if(!high_refresh::active)return result;const PresentationStage* slot=nullptr;
    for(const auto& entry:presentation)if(entry.owner==&stage&&entry.valid){slot=&entry;break;}if(!slot)return result;
    const auto close=[](const Vec3& a,const Vec3& b){const float dx=a.x-b.x,dy=a.y-b.y,dz=a.z-b.z;return dx*dx+dy*dy+dz*dz<250000.0f;};
    if(!close(slot->camera.position,stage.camera.position)||!close(slot->camera.target_offset,stage.camera.target_offset)||!close(slot->camera.eye_offset,stage.camera.eye_offset))return result;
    const auto lerp3=[](const Vec3& a,const Vec3& b){return Vec3{high_refresh::lerp_world(a.x,b.x),high_refresh::lerp_world(a.y,b.y),high_refresh::lerp_world(a.z,b.z)};};
    result.position=lerp3(slot->camera.position,stage.camera.position);result.target_offset=lerp3(slot->camera.target_offset,stage.camera.target_offset);result.eye_offset=lerp3(slot->camera.eye_offset,stage.camera.eye_offset);result.up=lerp3(slot->camera.up,stage.camera.up);result.field_of_view=high_refresh::lerp_world(slot->camera.field_of_view,stage.camera.field_of_view);result.draw_offset={high_refresh::lerp_world(slot->camera.draw_offset.x,stage.camera.draw_offset.x),high_refresh::lerp_world(slot->camera.draw_offset.y,stage.camera.draw_offset.y)};return result;
}
i32 Backgrounds::update(Stage& stage){snapshot(stage);return stage.update(script);}
i32 Backgrounds::draw(Stage& stage,bool foreground){
    BackgroundDraw draw(*this);if(!high_refresh::render_only){StageRenderer renderer{stage,draw};return foreground?renderer.draw_foreground():renderer.draw_background();}
    Stage copy=stage;copy.camera=presentation_camera(stage);
    for(u32 i=0;i<8;++i)engine.present(copy.script_animations[i],stage.script_animations[i]);
    std::vector<AnmVm> animations;const i32 count=stage.file?stage.file->primitive_count:0;if(stage.object_animations&&count>0){animations.assign(stage.object_animations,stage.object_animations+count);for(i32 i=0;i<count;++i)engine.present(animations[i],stage.object_animations[i]);copy.object_animations=animations.data();}
    std::vector<u8> flags;if(stage.file&&stage.objects&&stage.file->object_count>0){flags.resize(stage.file->object_count);for(i32 i=0;i<stage.file->object_count;++i)flags[i]=stage.objects[i]->flags;}
    StageRenderer renderer{copy,draw};const i32 result=foreground?renderer.draw_foreground():renderer.draw_background();for(i32 i=0;i<i32(flags.size());++i)stage.objects[i]->flags=flags[i];return result;
}
#ifndef TH_NATIVE_PLATFORM
bool Backgrounds::invoke(CallbackToken token,void* object,i32& result){auto& stage=*static_cast<Stage*>(object);if(token==update_callback){result=update(stage);return true;}if(token==background_callback||token==foreground_callback){result=draw(stage,token==foreground_callback);return true;}return false;}
#endif
Stage* Backgrounds::allocate_stage(){return static_cast<Stage*>(std::malloc(sizeof(Stage)));}
void* Backgrounds::allocate_bytes(u32 size){return std::malloc(size);}
void Backgrounds::release_memory(void* bytes){std::free(bytes);}
u8* Backgrounds::read_file(const char* name,u32* size){return ResourceFiles{files}.load(name,size,false);}
AnmFile* Backgrounds::load_animations(i32 slot,const char* name){return engine.manager.load(slot,name,engine.resources);}
void Backgrounds::release_animations(AnmFile& file){file.release(engine.resources);}
void Backgrounds::report(StageResourceError){error=-1;}
}

namespace th10::browser {
void Backgrounds::bind_callbacks(Callbacks& b){callback_context=this;
 b.bind(update_callback,this,[](void* p,void* o,i32){return static_cast<Backgrounds*>(p)->update(*static_cast<Stage*>(o));});
 b.bind(background_callback,this,[](void* p,void* o,i32){return static_cast<Backgrounds*>(p)->draw(*static_cast<Stage*>(o),false);});
 b.bind(foreground_callback,this,[](void* p,void* o,i32){return static_cast<Backgrounds*>(p)->draw(*static_cast<Stage*>(o),true);});
}
}
