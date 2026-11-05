#include "stone.hpp"
#include "../program_entry/program_entry.hpp"
#include <cstring>
namespace th20::source::stone_menu {
StoneMenuInf* controller=nullptr;
StoneMenuInf::StoneMenuInf():file(nullptr),visible(0),age{0,0,0,0},saved_selection(0),animation_handles{},selection_position{0,0,0},state(0),field_210f8(0),view_index(0),context(nullptr){std::memset(names,0,sizeof(names));std::memset(descriptions,0,sizeof(descriptions));}
StoneMenuInf::~StoneMenuInf(){
    scheduler::remove(*program_entry::function_controller,program_entry::scheduler_environment,update_node);scheduler::remove(*program_entry::function_controller,program_entry::scheduler_environment,draw_node);
    auto& host=environment();for(unsigned index:{0u,2u,3u,1u})host.request_delete(animation_handles[index]);host.unload_animation(24);
}
void StoneMenuInf::select_context(int index) noexcept{view_index=index;context=&game_session::context(index);}
}
