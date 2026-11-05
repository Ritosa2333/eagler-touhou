#include "background.hpp"
#include "../archive/resource_manager.hpp"
#include "../sprite_renderer/animation_file.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../game_session/session.hpp"
#include "../gameplay/player_state.hpp"
#include <cstring>
#include <new>
namespace th20::source::background {
namespace pe=program_entry;
Background* primary=nullptr;Background* secondary=nullptr;
namespace {
int __cdecl update_callback(void* self){return static_cast<Background*>(self)->update();}
int __cdecl draw_callback(void* self){return static_cast<Background*>(self)->draw_geometry();}
int __cdecl foreground_callback(void* self){return static_cast<Background*>(self)->draw_foreground();}
Primitive* first_primitive(Object& object){return reinterpret_cast<Primitive*>(reinterpret_cast<std::uint8_t*>(&object)+sizeof(Object));}
Primitive* next_primitive(Primitive& primitive){return reinterpret_cast<Primitive*>(reinterpret_cast<std::uint8_t*>(&primitive)+primitive.size);}
const char missing_animation[]="\x83\x58\x83\x65\x81\x5b\x83\x57\x83\x66\x81\x5b\x83\x5e\x82\xaa\x8c\xa9\x82\xc2\x82\xa9\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x81\x42\x83\x66\x81\x5b\x83\x5e\x82\xaa\x89\xf3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb7\r\n";
const char corrupt_stage[]="\x83\x58\x83\x65\x81\x5b\x83\x57\x83\x66\x81\x5b\x83\x5e\x82\xaa\x93\xc7\x82\xdd\x8d\x9e\x82\xdf\x82\xdc\x82\xb9\x82\xf1\x81\x42\x83\x66\x81\x5b\x83\x5e\x82\xaa\x89\xf3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb7\r\n";
}
Background::Background(){construct_background_members(*this);}
Background::~Background(){
    auto& callbacks=*pe::function_controller;auto& environment=pe::scheduler_environment;
    scheduler::remove(callbacks,environment,update_node);scheduler::remove(callbacks,environment,draw_node);scheduler::remove(callbacks,environment,additional_draw);
    if(primitive_animations){for(int i=0;i<file->animation_count;++i)sprite::destroy_animation_contents(primitive_animations[i]);runtime::release_bytes(primitive_animations);primitive_animations=nullptr;}
    for(auto& animation:state.animations)sprite::destroy_animation_contents(animation);
    if(file){runtime::release_bytes(file);file=nullptr;}
    if(original_data){runtime::release_bytes(original_data);original_data=nullptr;}
    for(unsigned index:{9u,10u})if(state.fields_3294[index]){unrecovered::destroy_render_mesh(reinterpret_cast<void*>(state.fields_3294[index]));state.fields_3294[index]=0;}
    if(!(game_session::flags()&1))sprite::unload_animation_file(*pe::sprite_controller,(static_cast<std::uint32_t>(stage_id)&1)+3);
    if(primary==this)primary=nullptr;if(secondary==this)secondary=nullptr;
    // 471c40 invokes all eight member destructors again in reverse order.
    for(std::size_t i=8;i>0;--i)sprite::destroy_animation_contents(state.animations[i-1]);
}
int Background::load(const char* name){
    if(!original_data){
        const auto data=resources::read(name);if(!data)return -1;
        data_size=static_cast<std::uint32_t>(data->size());original_data=static_cast<std::uint8_t*>(runtime::allocate_bytes(data_size));
        if(!original_data)throw std::bad_alloc();std::memcpy(original_data,data->data(),data_size);
    }
    file=static_cast<Header*>(runtime::allocate_bytes(data_size));if(!file)throw std::bad_alloc();std::memcpy(file,original_data,data_size);
    animation_file=sprite::load_animation_file(*pe::sprite_controller,(static_cast<std::uint32_t>(stage_id)&1)+3,file->animation_name,pe::log_buffer,pe::graphics_event_flags);
    if(!animation_file){runtime::log_printf(pe::log_buffer,missing_animation);return -1;}
    auto* bytes=reinterpret_cast<std::uint8_t*>(file);
    objects=reinterpret_cast<Object**>(bytes+sizeof(Header));instances=reinterpret_cast<Instance*>(bytes+file->instance_offset);instructions=reinterpret_cast<Instruction*>(bytes+file->script_offset);
    for(int i=0;i<file->object_count;++i)objects[i]=reinterpret_cast<Object*>(bytes+reinterpret_cast<std::uintptr_t>(objects[i]));
    const std::uint32_t size=static_cast<std::uint32_t>(file->animation_count)*sizeof(sprite::Animation);
    primitive_animations=static_cast<sprite::Animation*>(runtime::allocate_bytes(size));if(!primitive_animations)throw std::bad_alloc();std::memset(primitive_animations,0,size);return 0;
}
int Background::initialize(const char* name,int slot){
    if(slot==0)primary=this;else secondary=this;
    stage_id=gameplay::player_state::stage(game_session::session.player_table);
    if(load(name)!=0){runtime::log_error(pe::log_buffer,corrupt_stage);return -1;}
    state.owner=this;state.camera=pe::graphics_state.viewports[3];
    const sprite::Vec3 position{0,0,-600},direction{0,300,600},up{0,1,0},zero{};
    std::memcpy(state.camera.vectors[0],&position,12);std::memcpy(state.camera.vectors[1],&direction,12);std::memcpy(state.camera.vectors[2],&up,12);
    std::memcpy(state.camera.vectors[5],&zero,12);std::memcpy(state.camera.vectors[6],&zero,12);
    const auto limit=recovered::mul32(3100.f,3100.f);std::memcpy(&state.fields_3294[8],&limit,4);
    auto& callbacks=*pe::function_controller;auto& environment=pe::scheduler_environment;
    update_node=scheduler::register_callback(callbacks,environment,slot+22,&update_callback,this,false,false);
    draw_node=scheduler::register_callback(callbacks,environment,slot+3,&draw_callback,this,true,false);
    additional_draw=scheduler::register_callback(callbacks,environment,slot+6,&foreground_callback,this,true,false);
    frame_count=0;recovered::timer_set(state.timer,0);state.direction_interpolation.duration=state.position_interpolation.duration=0;state_flags|=1;return 0;
}
void Background::enable_callbacks(){
    scheduler::enable(*update_node);scheduler::enable(*draw_node);scheduler::enable(*additional_draw);
    int animation_index=0;
    for(int object_index=0;object_index<file->object_count;++object_index){auto& object=*objects[object_index];object.flags=1;
        for(auto* primitive=first_primitive(object);primitive->type>=0;primitive=next_primitive(*primitive)){
            sprite::bind_animation_script(*animation_file,primitive_animations[animation_index],primitive->script_index,nullptr);primitive->animation_index=static_cast<std::int16_t>(animation_index++);
        }
    }state.instruction_offset=0;
}
Background* create_background(const char* name,int slot){
    auto* memory=::operator new(sizeof(Background),std::nothrow);if(!memory)return nullptr;std::memset(memory,0,sizeof(Background));
    auto* background=new(memory)Background;if(background->initialize(name,slot)!=0){runtime::retire_callback_owner(background);return nullptr;}return background;
}
}
