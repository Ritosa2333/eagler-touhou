#include "ending.hpp"
#include "data_constants.hpp"
#include "../program_entry/program_entry.hpp"
#include "../platform_window/graphics_callbacks.hpp"
#include "../gameplay/gameplay.hpp"
#include "../gameplay/loading_dependencies.hpp"
#include "../sprite_renderer/animation_file.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../text_renderer/text.hpp"
#include <cstring>
#include <new>
namespace th20::source::platform_window::unrecovered {runtime::CallbackOwner* ending_scene=nullptr;void create_ending_scene(){ending::create();}}
namespace th20::source::ending {
EndingInf* controller() noexcept{return static_cast<EndingInf*>(platform_window::unrecovered::ending_scene);}
int selected_gallery_ending=-1;
EndingInf::EndingInf():field_10(0),message_data(nullptr),script(nullptr),ending_id(0),ending_flags(0),frames(0){}
EndingInf::~EndingInf(){
    scheduler::remove(*program_entry::function_controller,program_entry::scheduler_environment,update_node);scheduler::remove(*program_entry::function_controller,program_entry::scheduler_environment,draw_node);
    if(script){script->~Script();std::lock_guard lock(runtime::shared_locks().slot(1));::operator delete(script);}script=nullptr;
    for(int slot=15;slot<=18;++slot)sprite::unload_animation_file(*program_entry::sprite_controller,slot);
    runtime::release_bytes(message_data);message_data=nullptr;platform_window::unrecovered::ending_scene=nullptr;gameplay::slowdown_frames=0;
}
Script::Script(std::uint8_t* pc,int id):ending_id(0),elapsed{},script_time{},wait_time{},text_handles{},ruby_handles{},instruction(nullptr),positions{},pending_file(nullptr),flags(0),line(0),foreground(0xffffff),background(0xffffff),files{},handles{},pending_slot(0){
    auto& sprites=*program_entry::sprite_controller;
    for(unsigned row=0;row<2;++row)for(unsigned i=0;i<5;++i){
        auto& handle=(row?ruby_handles:text_handles)[i];handle=sprite::spawn_named_animation(sprites,*program_entry::graphics_state.surface_animation,data::s_0056fe1c,static_cast<int>(i)+(row?81:76));
        auto* animation=sprite::resolve_animation_handle(sprites,handle);animation->field_578=16;animation->field_579=16;animation->base.flags[1]&=~0x400u;
    }
    ending_id=id;instruction=pc;flags|=1;
}
Script::~Script(){runtime::join_worker(worker);auto& sprites=*program_entry::sprite_controller;for(auto& handle:text_handles)sprite::request_animation_deletion(sprites,handle);for(auto& handle:ruby_handles)sprite::request_animation_deletion(sprites,handle);}
EndingInf* create(){
    auto* memory=::operator new(sizeof(EndingInf),std::nothrow);if(!memory)return nullptr;std::memset(memory,0,sizeof(EndingInf));auto* owner=new(memory)EndingInf;
    if(initialize(*owner)==0)return owner;runtime::retire_callback_owner(owner);return nullptr;
}
}
