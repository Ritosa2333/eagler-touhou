#include "StageResources.hpp"
namespace th10 {
// 0x403850. Keep an unmodified source copy; every load relocates a fresh STD
// image and allocates one animation slot for each primitive in its header.
i32 StageResources::load(const char* name){
    if(!stage.source){environment.filename[0]=0;std::memcpy(environment.filename,name,std::strlen(name)+1);stage.source=environment.read_file(environment.filename,&stage.source_size);if(!stage.source)return -1;}
    stage.file=static_cast<StageHeader*>(environment.allocate_bytes(stage.source_size));std::memcpy(stage.file,stage.source,stage.source_size);
    stage.animation_file=environment.load_animations((static_cast<u32>(stage.stage_number)&1)+4,stage.file->animation_name);
    if(!stage.animation_file){environment.report(StageResourceError::Animation);return -1;}
    auto* base=reinterpret_cast<u8*>(stage.file);stage.objects=reinterpret_cast<StageObject**>(stage.file+1);stage.instances=reinterpret_cast<StageInstance*>(base+stage.file->instances_offset);stage.script_begin=reinterpret_cast<StageInstruction*>(base+stage.file->script_offset);
    for(i32 index=0;index<stage.file->object_count;++index){const auto offset=reinterpret_cast<uintptr_t>(stage.objects[index]);stage.objects[index]=reinterpret_cast<StageObject*>(base+offset);}
    stage.object_animations=static_cast<AnmVm*>(environment.allocate_bytes(static_cast<u32>(static_cast<i32>(stage.file->primitive_count))*static_cast<u32>(sizeof(AnmVm))));return 0;
}
// 0x402230. Priority offset selects the second background instance and shifts
// all three callbacks together. Fog and projection fields copy the world camera.
i32 StageResources::start(const char* name,i32 priority_offset){
    *(priority_offset?environment.overlay:environment.background)=&stage;stage.stage_number=*environment.stage_number;
    if(load(name)){environment.report(StageResourceError::StageStart);return -1;}
    stage.camera=*environment.world;stage.camera.position={0,0,-600};stage.camera.target_offset={0,300,600};stage.camera.up={0,1,0};stage.camera.eye_offset={0,0,0};stage.draw_distance_squared=0x1.25462p+23f;
    stage.update_entry=environment.chain->add(environment.update_callback,&stage,wrapping_add(priority_offset,12),false,false,*environment.callbacks);
    stage.draw_entry=environment.chain->add(environment.background_callback,&stage,wrapping_add(priority_offset,7),true,false,*environment.callbacks);
    stage.foreground_entry=environment.chain->add(environment.foreground_callback,&stage,wrapping_add(priority_offset,10),true,false,*environment.callbacks);
    stage.frame_count=0;if(!(stage.script_timer_flags&1)){stage.script_timer.rate=environment.rate;stage.script_timer_flags|=1;}stage.script_timer.initialize(-1);stage.draw_flags|=1;stage.target_interpolation.duration=stage.position_interpolation.duration=0;return 0;
}
// 0x402440. The primitive VM array is freed as a raw allocation; only the
// embedded effect/script VMs have individual geometry destructors here.
void StageResources::release(){
    environment.chain->remove_locked(stage.update_entry,*environment.callbacks);environment.chain->remove_locked(stage.draw_entry,*environment.callbacks);environment.chain->remove_locked(stage.foreground_entry,*environment.callbacks);
    if(stage.file){environment.release_memory(stage.file);stage.file=nullptr;}if(stage.source){environment.release_memory(stage.source);stage.source=nullptr;}if(stage.object_animations){environment.release_memory(stage.object_animations);stage.object_animations=nullptr;}
    if(!(*environment.game_flags&1)){auto*& file=environment.animation_slots[(static_cast<u32>(stage.stage_number)&1)+4];if(file){environment.release_animations(*file);environment.release_memory(file);file=nullptr;}}
    if(*environment.background==&stage)*environment.background=nullptr;if(*environment.overlay==&stage)*environment.overlay=nullptr;
    for(u32 i=3;i>0;--i)if(auto*& geometry=stage.effect_animations[i-1].geometry){environment.release_memory(geometry);geometry=nullptr;}
    for(u32 i=8;i>0;--i)if(auto*& geometry=stage.script_animations[i-1].geometry){environment.release_memory(geometry);geometry=nullptr;}
}
// 0x402640. Failed starts release the partially loaded stage and its owner.
Stage* StageResources::create(const char* name,i32 priority_offset,StageResourceEnvironment& environment){
    auto* stage=environment.allocate_stage();if(!stage)return nullptr;stage->initialize();StageResources resources{*stage,environment};if(resources.start(name,priority_offset)){resources.release();environment.release_memory(stage);return nullptr;}return stage;
}
}
