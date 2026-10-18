#include "score.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/pool.hpp"
#include <cstring>
#include <new>
namespace th20::source::small_score {
void construct_entry(Entry& entry) noexcept{
    std::memset(entry.digits,0,sizeof(entry.digits));entry.position={0,0,0};entry.speed=0;entry.color=0;entry.age={0,0,0,0};entry.field_30=entry.field_34=0;entry.active=entry.length=0;entry.bonus=0;entry.multiplier=0;
    // Original preserves padding bytes3a/3b even under an A5-filled constructor.
}
SmallScoreInf::SmallScoreInf(){file=nullptr;next_slot=0;field_18=0;sprite::construct_animation(animation);for(auto& entry:entries)construct_entry(entry);for(auto& entry:secondary_entries)construct_entry(entry);view_index=0;context=nullptr;}
SmallScoreInf::~SmallScoreInf(){scheduler::remove(*program_entry::function_controller,program_entry::scheduler_environment,update_node);scheduler::remove(*program_entry::function_controller,program_entry::scheduler_environment,draw_node);sprite::destroy_animation_contents(animation);}
void SmallScoreInf::select_context(int index) noexcept{view_index=index;context=&game_session::context(index);}
namespace {
int __cdecl update_thunk(void* owner){return update(*static_cast<SmallScoreInf*>(owner));}
int __cdecl draw_thunk(void* owner){return draw(*static_cast<SmallScoreInf*>(owner));}
}
int SmallScoreInf::initialize(int index,Environment& host){
    file=&host.text_file();auto& scheduler=*program_entry::function_controller;auto& scheduler_host=program_entry::scheduler_environment;
    update_node=scheduler::register_callback(scheduler,scheduler_host,25,update_thunk,this,false,false);draw_node=scheduler::register_callback(scheduler,scheduler_host,51,draw_thunk,this,true,false);
    host.initialize_sprite(*file,animation,0x121);
    // Original4bf/4c0 are Animation+4a3/+4a4, not the main flags word.
    auto* fields=reinterpret_cast<std::uint8_t*>(&animation);fields[0x4a3]=(fields[0x4a3]&0xfcu)|2u;fields[0x4a4]=3;
    select_context(index);game_session::context(index).objects_04[6]=this;return 0;
}
SmallScoreInf* controller(int index) noexcept{return static_cast<SmallScoreInf*>(game_session::context(index).objects_04[6]);}
SmallScoreInf* create_controller(int index){auto* storage=::operator new(sizeof(SmallScoreInf),std::nothrow);if(!storage)return nullptr;std::memset(storage,0,sizeof(SmallScoreInf));SmallScoreInf* result;try{result=::new(storage)SmallScoreInf;}catch(...){::operator delete(storage);throw;}if(result->initialize(index,environment())!=0){runtime::retire_callback_owner(result);return nullptr;}return result;}
void spawn(SmallScoreInf& owner,const sprite::Vec3& position,int value,std::uint32_t color) noexcept{
    if(owner.next_slot>9)owner.next_slot=0;auto& entry=owner.entries[owner.next_slot];entry.active=1;unsigned count=0;
    if(value<0){entry.digits[0]=10;count=1;}else while(value!=0){entry.digits[count++]=static_cast<std::uint8_t>(value%10);value/=10;}
    if(!count){entry.digits[0]=0;count=1;}entry.length=static_cast<std::uint8_t>(count);entry.color=color;recovered::timer_set(entry.age,0);entry.position=position;entry.speed=1;owner.next_slot=recovered::signed_bits(static_cast<unsigned>(owner.next_slot)+1);
}
}
