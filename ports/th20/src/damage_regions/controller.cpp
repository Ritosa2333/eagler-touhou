#include "regions.hpp"
#include "../program_entry/program_entry.hpp"
#include <cstring>
#include <new>
namespace th20::source::damage {
namespace pe=program_entry;
namespace {int __cdecl update_callback(void* object){return static_cast<HitCtrlInf*>(object)->update();}}
HitCtrlInf::HitCtrlInf(){for(auto& region:pool)construct_region(region);next_handle=0;scheduler::initialize_list(active);scheduler::initialize_list(free);visited_regions=0;age={};view_index=0;context=nullptr;}
HitCtrlInf::~HitCtrlInf(){scheduler::remove(*pe::function_controller,pe::scheduler_environment,update_node);scheduler::Iterator it(active.sentinel.next);while(it.current){auto* region=reinterpret_cast<Region*>(it.current->value);retire(*region);it.advance();}}
void HitCtrlInf::select_context(std::int32_t index) noexcept {view_index=index;context=&game_session::context(index);}
void HitCtrlInf::initialize_pool(std::int32_t index){
    scheduler::initialize_list(active);scheduler::initialize_list(free);
    for(auto& region:pool){scheduler::initialize_link(region.link,reinterpret_cast<scheduler::Node*>(&region));scheduler::append(free,region.link);}
    next_handle=(static_cast<std::uint32_t>(index)<<16)|1;recovered::timer_set(age,0);
}
int HitCtrlInf::initialize(std::int32_t index){select_context(index);update_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,28,update_callback,this,false,false);context->object_28=this;initialize_pool(index);return 0;}
void HitCtrlInf::advance_handle() noexcept {next_handle=(next_handle+1)&0xffff;if(!next_handle)next_handle=1;}
Region* HitCtrlInf::allocate(){
    Region* region;
    if(auto* link=free.sentinel.next){scheduler::unlink(*link);scheduler::append(active,*link);region=reinterpret_cast<Region*>(link->value);region->handle=next_handle;}
    else {void* memory=::operator new(sizeof(Region),std::nothrow);if(!memory)return nullptr;std::memset(memory,0,sizeof(Region));region=::new(memory)Region;construct_region(*region);scheduler::initialize_link(region->link,reinterpret_cast<scheduler::Node*>(region));scheduler::append(active,region->link);region->handle=next_handle|0x1000000u;damage::select_context(*region,0);}
    advance_handle();return region;
}
Region* HitCtrlInf::find(std::uint32_t handle) noexcept {
    if(!handle)return nullptr;
    // Lookup never mutates the list. An observing Iterator here would replace
    // the damage dispatcher's observer during a hit callback; retirement would
    // then leave that dispatcher pointing at a freed heap region.
    for(auto* link=active.sentinel.next;link;link=link->next){
        auto* region=reinterpret_cast<Region*>(link->value);
        if(region->handle==handle)return region;
    }
    return nullptr;
}
void HitCtrlInf::detach(Region& region) noexcept {scheduler::unlink(region.link);if(!(region.handle&0x1000000u)){scheduler::insert_after(free.sentinel,region.link);region.link.owner=&free;if(free.tail==&free.sentinel)free.tail=&region.link;}}
int HitCtrlInf::update(){std::uint32_t count=0;scheduler::Iterator it(active.sentinel.next);while(it.current){damage::update(*reinterpret_cast<Region*>(it.current->value));++count;it.advance();}visited_regions=count;recovered::timer_tick(age,state::timer_rate);return 1;}
HitCtrlInf* controller(std::int32_t index) noexcept {return static_cast<HitCtrlInf*>(game_session::context(index).object_28);}
HitCtrlInf* create_controller(std::int32_t index){void* memory=::operator new(sizeof(HitCtrlInf),std::nothrow);if(!memory)return nullptr;std::memset(memory,0,sizeof(HitCtrlInf));HitCtrlInf* owner;try{owner=::new(memory)HitCtrlInf;}catch(...){::operator delete(memory);throw;}if(owner->initialize(index)!=0){runtime::retire_callback_owner(owner);return nullptr;}return owner;}
}
