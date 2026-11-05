#include "ending.hpp"
#include "data_constants.hpp"
#include "../program_entry/program_entry.hpp"
#include "../platform_window/graphics_callbacks.hpp"
#include "../archive/resource_manager.hpp"
#include "../sprite_renderer/animation_file.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../text_renderer/text.hpp"
#include "../progress_state/manager.hpp"
#include "../gameplay/player_state.hpp"
#include <atomic>
#include <cstring>
#include <new>
#include <stdexcept>
namespace th20::source::ending {
namespace {
std::uint8_t* read(const char* file){auto bytes=resources::read(file,false);if(!bytes)return nullptr;auto* value=static_cast<std::uint8_t*>(runtime::allocate_bytes(bytes->size()));if(!value)throw std::bad_alloc();std::memcpy(value,bytes->data(),bytes->size());return value;}
int __cdecl update_callback(void* o){return update(*static_cast<EndingInf*>(o));}
//49dea0->478bf0 has no rendering side effects, returns1.
int __cdecl draw_callback(void*){return 1;}
}
std::uint8_t* replace_script_data(EndingInf& o,const char* filename){auto* bytes=read(filename);runtime::release_bytes(o.message_data);o.message_data=bytes;return bytes;}
int initialize(EndingInf& o){
    platform_window::unrecovered::ending_scene=&o;
    o.update_node=scheduler::register_callback(*program_entry::function_controller,program_entry::scheduler_environment,47,update_callback,&o,false,true);
    o.draw_node=scheduler::register_callback(*program_entry::function_controller,program_entry::scheduler_environment,87,draw_callback,&o,true,true);
    text::renderer->create_loading_text(data::f_0056cda8,data::f_00570388);
    o.ending_id=select_ending();o.ending_flags&=~4u;
    if(selected_gallery_ending>=0){o.ending_id=selected_gallery_ending;selected_gallery_ending=-1;o.ending_flags|=4;}
    else{
        if(ending_record(o.ending_id)==0)o.ending_flags|=1;
        if(ending_record(18)==0||ending_record(19)==0||ending_record(20)==0||ending_record(21)==0)o.ending_flags|=2;
        auto& metadata=progress::manager->current.metadata;metadata.bytes[0x16+o.ending_id]|=1;
        const int difficulty=gameplay::player_state::difficulty(game_session::session.player_table);if(difficulty>=1&&difficulty<=3)metadata.bytes[0x16+o.ending_id]|=static_cast<unsigned char>(1u<<difficulty);
        for(unsigned i=18;i<22;++i)metadata.bytes[0x16+i]=1;
    }
    if(o.ending_id<0||o.ending_id>=static_cast<int>(std::size(data::endings)))throw std::out_of_range("Ending file index outside recovered table");
    o.message_data=read(data::endings[o.ending_id]);if(!o.message_data){runtime::log_error(program_entry::log_buffer,data::s_0056f610);return -1;}
    const auto offset=progress::read<unsigned>(o.message_data,4);auto* memory=::operator new(sizeof(Script),std::nothrow);if(!memory){o.script=nullptr;return 0;}std::memset(memory,0,sizeof(Script));o.script=new(memory)Script(o.message_data+offset,o.ending_id);
    if(o.ending_id>=18)o.script->flags|=2;return 0;
}
void load_pending_resource(Script& o){
    auto* file=sprite::load_animation_file(*program_entry::sprite_controller,o.pending_slot+15,o.pending_file,program_entry::log_buffer,program_entry::graphics_state.event_flags);
    if(o.pending_slot<0||o.pending_slot>=4)throw std::out_of_range("Ending ANM slot outside4 files");o.files[o.pending_slot]=file;std::atomic_ref(o.flags).fetch_and(~4u);
    auto& handle=text::renderer->loading_handle;
    sprite::interrupt_animation_children(*program_entry::sprite_controller,handle,1);
    if(auto* animation=sprite::find_animation(*program_entry::sprite_controller,handle))sprite::hide_animation_tree(*animation);
    sprite::request_animation_deletion(*program_entry::sprite_controller,handle);
}
void begin_resource_load(Script& o){
    text::renderer->create_loading_text(data::f_0056cda8,data::f_00570388);
    const auto slot=progress::read<int>(o.instruction,4);sprite::unload_animation_file(*program_entry::sprite_controller,slot+15);
    std::atomic_ref(o.flags).fetch_or(4u);o.pending_file=reinterpret_cast<const char*>(o.instruction+8);o.pending_slot=slot;
    std::lock_guard lock(runtime::shared_locks().slot(6));{std::lock_guard nested(runtime::shared_locks().slot(6));if(o.worker.thread.joinable())o.worker.thread.detach();}o.worker.close_requested.store(false);
    //Original4a0a00 resolves the active EndingInf at worker execution time.
    o.worker.thread=TH20_WORKER_THREAD("ending-resource",[]{load_pending_resource(*controller()->script);});
}
}
