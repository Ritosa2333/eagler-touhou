#include "item.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../program_entry/program_entry.hpp"
#include <cstring>
#include <new>
namespace th20::source::item {
void construct_item(Item& item){scheduler::initialize_link(item.link,reinterpret_cast<scheduler::Node*>(&item));item.free_list=nullptr;sprite::construct_animation(item.animation);sprite::construct_animation(item.secondary_animation);std::memset(&item.attachment,0,sizeof(Item)-offsetof(Item,attachment));}
void destroy_item(Item& item){sprite::destroy_animation_contents(item.secondary_animation);sprite::destroy_animation_contents(item.animation);}
ItemInf::ItemInf(){field_10=0;second_draw_node=nullptr;for(auto& item:pool)construct_item(item);scheduler::initialize_list(active);scheduler::initialize_list(ordinary_free);scheduler::initialize_list(special_free);std::memset(&speed_scale,0,sizeof(ItemInf)-offsetof(ItemInf,speed_scale));}
ItemInf::~ItemInf(){auto& scheduler=*program_entry::function_controller;auto& environment=program_entry::scheduler_environment;scheduler::remove(scheduler,environment,update_node);scheduler::remove(scheduler,environment,draw_node);scheduler::remove(scheduler,environment,second_draw_node);for(int i=1535;i>=0;--i)destroy_item(pool[i]);}
void ItemInf::enable_callbacks(){runtime::CallbackOwner::enable_callbacks();if(second_draw_node)scheduler::enable(*second_draw_node);}
void ItemInf::select_context(std::int32_t index) noexcept{view_index=index;context=&game_session::context(index);}
void select_context(Item& item,std::int32_t index) noexcept{item.view_index=index;item.context=&game_session::context(index);}
void ItemInf::initialize_pool(){
    spawn_counter=0;point_counter=0;generation=0;std::memset(pool,0,sizeof(pool));scheduler::initialize_list(active);scheduler::initialize_list(ordinary_free);scheduler::initialize_list(special_free);
    for(unsigned i=0;i<1536;++i){auto& item=pool[i];scheduler::initialize_link(item.link,reinterpret_cast<scheduler::Node*>(&item));item.free_list=i<512?&ordinary_free:&special_free;scheduler::append(*item.free_list,item.link);}
    speed_scale=1;field_49c880=0;field_49c87c=100;bonus_counter=0;
}
namespace {
int __cdecl update_thunk(void* value){return update_callback(*static_cast<ItemInf*>(value));}
int __cdecl draw_thunk(void* value){return draw_callback(*static_cast<ItemInf*>(value),1);}
int __cdecl second_draw_thunk(void* value){return draw_callback(*static_cast<ItemInf*>(value),0);}
}
int ItemInf::initialize(int index){
    game_session::context(index).objects_04[2]=this;select_context(index);auto& scheduler=*program_entry::function_controller;auto& environment=program_entry::scheduler_environment;
    update_node=scheduler::register_callback(scheduler,environment,39,update_thunk,this,false,false);draw_node=scheduler::register_callback(scheduler,environment,35,draw_thunk,this,true,false);second_draw_node=scheduler::register_callback(scheduler,environment,19,second_draw_thunk,this,true,false);initialize_pool();return 0;
}
ItemInf* controller(int index) noexcept{return static_cast<ItemInf*>(game_session::context(index).objects_04[2]);}
ItemInf* create_controller(int index){auto* storage=::operator new(sizeof(ItemInf),std::nothrow);if(!storage)return nullptr;std::memset(storage,0,sizeof(ItemInf));ItemInf* result;try{result=::new(storage)ItemInf;}catch(...){::operator delete(storage);throw;}if(result->initialize(index)!=0){runtime::retire_callback_owner(result);return nullptr;}return result;}
}
