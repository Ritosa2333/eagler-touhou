#include "effect.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include <cstring>
#include <stdexcept>
namespace th20::source::effects {
Controller* controller(std::int32_t index) noexcept {return static_cast<Controller*>(environment::context(index).objects_04[7]);}
namespace s=sprite;namespace q=scheduler;namespace e=environment;
const Descriptor descriptors[16]{
    {2,0,unrecovered::initialize_0,{}},{0,0,unrecovered::initialize_1,{}},{0,0,unrecovered::initialize_2,{}},{0,0,unrecovered::initialize_3,{}},
    {0,0,unrecovered::initialize_4,{}},{0,0,unrecovered::initialize_5,{}},{0,0,unrecovered::initialize_6,{}},{0,0,unrecovered::initialize_7,{}},
    {0,0,unrecovered::initialize_8,{}},{0,0,unrecovered::initialize_9,{}},{0,0,unrecovered::initialize_10,{}},{0,0,unrecovered::initialize_11,{}},
    {0,0,unrecovered::initialize_12,{}},{0,0,unrecovered::initialize_13,{}},{0,0,unrecovered::initialize_14,{}},{-1,-1,nullptr,{}}
};
void enable_animation_tree(s::Animation& a){a.base.flags[1]|=1;auto* first=a.links[3].next;if(first){q::Iterator it(reinterpret_cast<q::Link*>(first));while(it.current){enable_animation_tree(*reinterpret_cast<s::Animation*>(it.current->value));it.advance();}}}
void Controller::select_context(std::int32_t index){view_index=index;context=&e::context(index);}
std::uint32_t Controller::spawn(std::int32_t type,const void* parameters,s::Animation* existing,bool secondary){
    if(type<0||type>15)throw std::out_of_range("EffectInf descriptor outside original15 entries and terminator");
    const auto& entry=descriptors[type];std::uint32_t handle=0;if(entry.script<0)return handle;
    if(!existing){auto& sprites=e::sprites();auto& file=*files[entry.file];if(secondary)s::spawn_named_animation(sprites,file,handle,nullptr,entry.script,nullptr,0.f,-1,4,nullptr);
        else handle=s::spawn_named_animation(sprites,file,nullptr,entry.script,view_index);
        existing=s::resolve_animation_handle(sprites,handle);if(!secondary)enable_animation_tree(*existing);
    }
    if(entry.initialize)entry.initialize(existing,parameters,view_index);return handle;
}
void* Controller::enqueue(std::int32_t delay,std::int32_t type,const Parameters* parameters,s::Animation* animation){
    for(auto& r:requests)if(r.type<0){r.type=type;r.delay=delay;r.original_parameters=parameters;r.animation=animation;std::memcpy(&r.parameters,parameters,sizeof(Parameters));return &r.parameters;}
    return requests;
}
int Controller::update(){auto& sprites=e::sprites();sprites.field_6c4=static_cast<std::uint32_t>(view_index);
    for(auto& handle:handles)if(!s::resolve_animation_handle(sprites,handle))handle=0;
    for(auto& r:requests)if(r.type>=0){if(r.delay<1){spawn(r.type,&r.parameters,r.animation);r.type=-1;}else --r.delay;}return 1;
}
void Controller::clear(){auto& sprites=e::sprites();s::mark_file_animations(sprites,sprites.files[8],true);s::mark_file_animations(sprites,sprites.files[7],true);
    // The original copies a stack-local request, including three indeterminate
    // padding bytes. Defined fields are exact; source chooses zero padding.
    Request empty{};construct_request(empty);for(auto& r:requests)r=empty;
}
}
