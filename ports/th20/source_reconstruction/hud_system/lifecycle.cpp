#include "hud.hpp"
#include "dialogue.hpp"
#ifdef TH_SDL3
#include "platform/Time.hpp"
#endif
#include "../program_entry/program_entry.hpp"
#include "../runtime_core/worker.hpp"
#include "../sprite_renderer/animation_file.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../archive/resource_manager.hpp"
#include "../gameplay/stage_data.hpp"
#include "../gameplay/player_state.hpp"
#include "../stone_menu/stone.hpp"
#include <atomic>
#include <cstring>
#include <new>
namespace th20::source::hud {
namespace pe=program_entry;
namespace {
runtime::Worker resource_worker; //5c4a04, default ctor40b780
std::uint8_t* cached_message=nullptr; //5c4a14
std::atomic<std::uint32_t> shared_ready{0}; //5c4a18
void load_error(){runtime::log_printf(pe::log_buffer,"\x83\x66\x81\x5b\x83\x5e\x82\xaa\x89\xf3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb7\r\n");}
sprite::AnimationFile* load(int slot,const char* path){return sprite::load_animation_file(environment::sprites(),slot,path,pe::log_buffer,pe::graphics_state.event_flags);}
void mark(int slot){auto& c=environment::sprites();sprite::mark_file_animations(c,c.files[slot],false);}
int __cdecl update_callback(void* value){return update(*static_cast<FrontInf*>(value));}
int __cdecl draw_callback(void* value){return draw(*static_cast<FrontInf*>(value));}
int __cdecl player_callback(void*){return draw_player();}
}
bool load_shared(){const auto* file=load(5,"front.anm");if(file)shared_ready.store(1,std::memory_order_relaxed);else load_error();return file==nullptr;}
void unload_shared(){runtime::join_worker(resource_worker);sprite::unload_animation_file(environment::sprites(),5);}
int initialize_stage(FrontInf& owner){
    owner.stage_file=load(6,gameplay::selected_stage->logo);if(!owner.stage_file){load_error();return -1;}
    if(!cached_message){
        const auto& selected=*game_session::context(0).current_player;
        const auto index=selected.fields_00[3]+selected.fields_00[2]*8;
        if(index>=16)throw std::out_of_range("FrontInf stage message index outside original table");
        char path[260];strcpy_s(path,gameplay::selected_stage->messages[index]); //4bd4a0 empty-prefix path buffer
        auto bytes=resources::read(path);if(!bytes){owner.message_data=nullptr;load_error();return -1;}
        owner.message_data=static_cast<std::uint8_t*>(runtime::allocate_bytes(bytes->size()));
        if(!owner.message_data){load_error();return -1;}std::memcpy(owner.message_data,bytes->data(),bytes->size());
    }else{owner.message_data=cached_message;cached_message=nullptr;}
    recovered::timer_set(owner.age,0);owner.score=gameplay::player_state::score(game_session::session.player_table.players[0]);owner.message_index=-1;owner.field_1cc=-1;return 0;
}
int initialize(FrontInf& owner){
#ifdef TH_SDL3
    while(shared_ready.load(std::memory_order_relaxed)==0)web::time::sleep(10);
#else
    while(shared_ready.load(std::memory_order_relaxed)==0)Sleep(10);
#endif
    owner.front_file=load(5,"fronttr.anm");if(!owner.front_file){load_error();return -1;}
    if(initialize_stage(owner)!=0)return -1;
    auto& c=*pe::function_controller;auto& e=pe::scheduler_environment;
    owner.update_node=scheduler::register_callback(c,e,42,update_callback,&owner,false,false);
    owner.draw_node=scheduler::register_callback(c,e,58,player_callback,&owner,true,false);
    owner.secondary_draw=scheduler::register_callback(c,e,52,draw_callback,&owner,true,false);return 0;
}
void clear(FrontInf& owner){
    if(game_session::flags()&0x11u)mark(6);else sprite::unload_animation_file(environment::sprites(),6);owner.stage_file=nullptr;
    if(owner.collecting){destroy_dialogue(owner.collecting);owner.collecting=nullptr;}
    if(game_session::flags()&0x11u)cached_message=owner.message_data;
    else{if(owner.message_data)runtime::release_bytes(owner.message_data);owner.message_data=nullptr;cached_message=nullptr;}
    if(owner.update_node)scheduler::disable(*owner.update_node);
    auto& c=environment::sprites();sprite::request_animation_deletion(c,owner.notice_handles[0]);sprite::request_animation_deletion(c,owner.notice_handles[1]);sprite::request_animation_deletion(c,owner.handle_cc);
    for(auto& handle:owner.handles_d0)handle=0;
    for(auto& handle:owner.score_handles)sprite::request_animation_deletion(c,handle);
    owner.fields_174[3]=0;for(auto& panel:owner.panels)panel.field_4c=0;owner.flags|=0xe0u;
}
void release_stage_resources(FrontInf& owner){
    if(owner.collecting){destroy_dialogue(owner.collecting);owner.collecting=nullptr;}
    if((game_session::flags()&0x11u)==0){
        sprite::unload_animation_file(environment::sprites(),6);owner.stage_file=nullptr;
        if(owner.message_data)runtime::release_bytes(owner.message_data);owner.message_data=nullptr;
    }
}
FrontInf::~FrontInf(){
    clear(*this);stone_menu::clear(*stone_menu::controller);
    auto& c=*pe::function_controller;auto& e=pe::scheduler_environment;
    scheduler::remove(c,e,update_node);scheduler::remove(c,e,draw_node);scheduler::remove(c,e,secondary_draw);update_node=nullptr;
    auto& sprites=environment::sprites();sprite::request_animation_deletion(sprites,handles_f8[0]);sprite::request_animation_deletion(sprites,handles_f8[6]);
    for(auto& handle:score_handles)sprite::request_animation_deletion(sprites,handle);
    for(auto& handle:number_handles)sprite::request_animation_deletion(sprites,handle);
    mark(5);controller=nullptr;
}
FrontInf* create(){auto* memory=::operator new(sizeof(FrontInf),std::nothrow);if(!memory)return nullptr;std::memset(memory,0,sizeof(FrontInf));auto* value=new(memory)FrontInf;if(initialize(*value)!=0){runtime::retire_callback_owner(value);return nullptr;}return value;}
}
