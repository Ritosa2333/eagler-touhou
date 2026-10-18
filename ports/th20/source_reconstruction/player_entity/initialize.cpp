#include "initialize.hpp"
#include "power.hpp"
#include "frame.hpp"
#include "../ecl_vm/math.hpp"
#include <bit>
#include <cstdio>
#include <cstring>
#include <new>
namespace th20::source::player_entity {
namespace {
template<class T>T read(const void* p,unsigned offset){T result;std::memcpy(&result,static_cast<const std::uint8_t*>(p)+offset,sizeof(T));return result;}
int __cdecl update_callback(void* p){return update_player(*static_cast<Player*>(p));} //4feda0 preserves 4f7430 EAX=1
int __cdecl draw_callback(void* p){return draw_player(*static_cast<Player*>(p));}
}
void select_context(Player& p,int index,game_session::Session& session) noexcept{p.view_index=index;p.context=&session.contexts[index];}
void select_context(Option& option,int index,game_session::Session& session) noexcept{option.fields_f4[12]=index;option.fields_f4[13]=reinterpret_cast<std::uint32_t>(&session.contexts[index]);}
void select_context(Feedback& feedback,int index,game_session::Session& session) noexcept{feedback.view_index=index;feedback.context=&session.contexts[index];}
void select_context(Shot& shot,int index,game_session::Session& session) noexcept{shot.view_index=index;shot.context=&session.contexts[index];shot.owner=&static_cast<Player*>(shot.context->objects_04[0])->shots;}
void initialize_shots(ShotController& owner,int index,game_session::Session& session){
    owner.view_index=index;owner.context=&session.contexts[index];owner.field_12558=reinterpret_cast<std::uint32_t>(static_cast<Player*>(owner.context->objects_04[0])->shot_data);
    scheduler::initialize_list(owner.active);scheduler::initialize_list(owner.free);
    for(auto& shot:owner.pool){select_context(shot,index,session);scheduler::initialize_link(shot.link,reinterpret_cast<scheduler::Node*>(&shot));scheduler::append(owner.free,shot.link);}
    recovered::timer_set(owner.timer_12400,-1);recovered::timer_set(owner.timer_12410,-1);recovered::timer_set(owner.timer_12420,0);
    owner.handle_12560=std::bit_cast<std::uint32_t>(ecl::math::wrap_angle(-0x1.921fb6p+0f));owner.field_12564=0;recovered::timer_set(owner.timer_12568,0);owner.field_12460=1;
}
void initialize_feedback(Feedback& feedback,int index,InitializationServices& host){
    recovered::timer_set(feedback.timers[0],0);feedback.fields_30[2]=feedback.fields_30[0]=0;
    recovered::timer_set(feedback.timers[2],0);recovered::timer_set(feedback.timers[1],0);select_context(feedback,index,host.session());
    feedback.handle_4c=host.create_feedback_animation();host.hide_feedback_animation(feedback.handle_4c);feedback.enabled=0;
}
int initialize(Player& p,int index,InitializationServices& host){
    host.session().contexts[index].objects_04[0]=&p;select_context(p,index,host.session());host.select_view(index);
    char filename[40];const auto character=static_cast<std::int32_t>(p.context->current_player->fields_00[2]);std::snprintf(filename,sizeof(filename),"pl%02d.anm",character);
    p.animation_file=host.load_animation(index+9,filename);
    if(p.animation_file){std::snprintf(filename,sizeof(filename),"pl%02d.sht",static_cast<std::int32_t>(p.context->current_player->fields_00[2]));p.shot_data=host.load_shots(filename);}
    if(!p.animation_file||!p.shot_data){host.resource_error();return -1;}
    p.field_24=reinterpret_cast<std::uint32_t>(p.animation_file);
    //Original assigns retained5c60f0 only after loading a fresh resource. It
    //does not free that displaced pointer here, a retained-mode ownership quirk.
    if(!retained_shot_data)initialize_shots(p.shots,p.view_index,host.session());else{p.shot_data=retained_shot_data;retained_shot_data=nullptr;}
    auto& services=*p.services;
    // 412310/4123b0 both call 4127f0: remain disabled while the loading
    // worker constructs dependent entities. Stage activation enables them.
    p.update_node=scheduler::register_callback(services.scheduler(),services.scheduler_environment(),0x1d,update_callback,&p,false,false);
    p.draw_node=scheduler::register_callback(services.scheduler(),services.scheduler_environment(),0x1e,draw_callback,&p,true,false);
    for(unsigned i=0;i<5;++i)p.animation_scripts[i]=i;
    host.bind_animation(*p.animation_file,p.animation,0);set_position(p,0,400);
    for(unsigned i=0;i<4;++i)p.speeds_20b4[i]=fixed_coordinates({read<float>(p.shot_data,0x10+i*4),0}).x;
    const std::int32_t unit=100;std::memcpy(static_cast<std::uint8_t*>(p.shot_data)+0x24,&unit,4);
    p.context->current_player->fields_30[1]=std::uint32_t(read<std::int32_t>(p.shot_data,0x20))*100u;(void)clamped_maximum_power(*p.context->current_player);
    p.context->current_player->fields_30[2]=100;(void)clamped_power_unit(*p.context->current_player);
    recovered::timer_set(p.timers_644[0],0);recovered::timer_set(p.timers_2050[0],0);recovered::timer_set(p.timers_2050[1],0);
    p.fields_20e4[0]=30;p.fields_674[3]=0;p.fields_20e4[2]=0x3f800000u;
    for(auto& option:p.options){option.vector_78.y=std::bit_cast<std::int32_t>(0xffff3800u);select_context(option,p.view_index,host.session());}
    for(auto& option:p.secondary_options){option.vector_78.y=std::bit_cast<std::int32_t>(0xffff3800u);select_context(option,p.view_index,host.session());}
    p.entity_flags&=~4u;p.interpolation_220c.duration=0;p.collision_expansion=1.0f;initialize_feedback(p.feedback,index,host);return 0;
}
Player* create_player(int index,PlayerServices& lifetime,InitializationServices& initialization){
    void* storage=nullptr;
    {std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(1));storage=::operator new(sizeof(Player),std::nothrow);}
    if(!storage)return nullptr;std::memset(storage,0,sizeof(Player));
    auto* player=new(storage)Player(lifetime);if(initialize(*player,index,initialization)==0){initialization.create_damage(index);return player;}
    runtime::retire_callback_owner(player);return nullptr;
}
}
