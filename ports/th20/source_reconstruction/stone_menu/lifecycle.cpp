#include "stone.hpp"
#include "../program_entry/program_entry.hpp"
#include <cstring>
#include <new>
namespace th20::source::stone_menu {
namespace {
int __cdecl update_thunk(void* self){return update(*static_cast<StoneMenuInf*>(self));}
int __cdecl draw_thunk(void* self){return draw(*static_cast<StoneMenuInf*>(self));}
}
int initialize(StoneMenuInf& owner,int view,Environment& host){
    owner.select_context(view);host.select_view(view);owner.file=host.load_animation(24,"stone.anm");
    if(!owner.file){host.load_error();return -1;}
    auto& scheduler=*program_entry::function_controller;auto& scheduler_host=program_entry::scheduler_environment;
    owner.update_node=scheduler::register_callback(scheduler,scheduler_host,27,update_thunk,&owner,false,true);
    owner.draw_node=scheduler::register_callback(scheduler,scheduler_host,55,draw_thunk,&owner,true,true);
    parse_text(owner,host.text_resource("stonetext.txt"));return 0;
}
int initialize(StoneMenuInf& owner,int view){return initialize(owner,view,environment());}
void hide(StoneMenuInf& owner,Environment& host){
    for(unsigned index:{0u,2u,3u})host.interrupt(owner.animation_handles[index],1);
    for(unsigned index:{1u,5u,7u,8u})host.request_delete(owner.animation_handles[index]);
    owner.visible=0;owner.state=0;
}
void clear(StoneMenuInf& owner,Environment& host){
    for(unsigned index:{0u,2u,3u,1u,5u,7u,8u})host.request_delete(owner.animation_handles[index]);
    owner.visible=0;owner.state=0;
}
void hide(StoneMenuInf& owner){hide(owner,environment());}
void clear(StoneMenuInf& owner){clear(owner,environment());}
StoneMenuInf* create_controller(int index){
    auto* memory=::operator new(sizeof(StoneMenuInf),std::nothrow);if(!memory)return nullptr;
    std::memset(memory,0,sizeof(StoneMenuInf));StoneMenuInf* value;
    try{value=::new(memory)StoneMenuInf;}catch(...){::operator delete(memory);throw;}
    if(initialize(*value,index)!=0){runtime::retire_callback_owner(value);return nullptr;}
    controller=value;return value;
}
void release(){if(controller)runtime::retire_callback_owner(controller);controller=nullptr;}
}
