#include "dialogue_text.hpp"
#include "../sprite_renderer/pool.hpp"
#include <stdexcept>
#include "../text_renderer/text.hpp"
#include "../text_renderer/raster.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/menu_animation.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include <algorithm>
#include <cstdlib>
#include <string>
namespace th20::source::hud {
namespace {
auto& sprites(){return environment::sprites();}
unsigned color(const Dialogue& d){if(d.fields_108[3]>=4)throw std::out_of_range("Dialogue text color index outside four original slots");return d.fields_108[4+d.fields_108[3]];}
int font(const Dialogue& d){return d.flags&2u?13:4;}
void position(Dialogue& d,unsigned slot){if(auto* a=sprite::find_animation(sprites(),d.handles[slot]))a->vector_5bc=d.vector_128;}
void text_complete(Dialogue& d,unsigned slot,bool immediate,bool reset){
    if(immediate)sprite::execute_animation_interrupt(sprites(),d.handles[slot],2);else sprite::interrupt_animation_children(sprites(),d.handles[slot],2);
    if(reset)d.fields_108[0]=d.fields_108[1]=0;
}
}
void clear_dialogue_text(Dialogue& d,bool final_clear){
    if(final_clear)sprite::request_animation_deletion(sprites(),d.handles[6]);
    for(unsigned i=1;i<=4;++i){auto* animation=sprite::resolve_animation_handle(sprites(),d.handles[i]);text::renderer->enqueue_task([&d,animation,i,final_clear]{
        // The eight source functors' empty completion closures resolve to40e5e0.
        text::write_animation_text(sprites(),*animation,final_clear?color(d):0,0x20ffffff,i<3?11:18,0,0,nullptr,[]{},"                                                                     ");
    });}
    for(unsigned i=1;i<=4;++i){if(final_clear)sprite::execute_animation_interrupt(sprites(),d.handles[i],3);else sprite::interrupt_animation_children(sprites(),d.handles[i],1);}
    if(!final_clear)sprite::request_animation_deletion(sprites(),d.handles[6]);
}
void queue_dialogue_text(Dialogue& d,bool second){
    const unsigned slot=second?2:1;auto* animation=sprite::resolve_animation_handle(sprites(),d.handles[slot]);
    text::renderer->enqueue_task([&d,animation,slot]{text::write_animation_text(sprites(),*animation,color(d),0,font(d),0,0,nullptr,[&d,slot]{text_complete(d,slot,false,false);},decode_dialogue_text(d.script+4));});
}
void queue_dialogue_line(Dialogue& d){
    const bool second=d.fields_108[0]!=0;
    if(!second&&d.fields_108[1]==0){d.field_134=0;for(unsigned i=1;i<=4;++i)sprite::execute_animation_interrupt(sprites(),d.handles[i],3);}
    std::string value(decode_dialogue_text(d.script+4));
    if(!value.empty()&&value.front()=='|'){
        const auto first=value.find(',',1),last=first==std::string::npos?std::string::npos:value.find(',',first+1);
        if(last==std::string::npos)throw std::invalid_argument("Dialogue ruby line lacks original two commas");
        const auto x=std::atoi(value.c_str()+1),spacing=std::atoi(value.c_str()+first+1);value.erase(0,last+1);
        const unsigned slot=second?4:3;auto* animation=sprite::resolve_animation_handle(sprites(),d.handles[slot]);
        text::renderer->enqueue_task([&d,animation,value=std::move(value),x,spacing,slot,second]{
            text::set_next_outline_scale(second?.6000000238418579f:.5f);
            text::write_animation_text(sprites(),*animation,0,0x20ffffff,18,x,spacing,nullptr,[&d,slot]{text_complete(d,slot,true,false);},value.c_str());
            std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(9));position(d,slot);
        });
    }else{
        const auto width=recovered::mul32(static_cast<float>((static_cast<std::uint32_t>(value.size())&0x1ffffffeu)*8u-24u),2);
        if(!(width<=d.field_134))d.field_134=width;
        const auto type=(d.flags>>2&15u)*3u+d.fields_108[3]+(second?24u:0u);create_dialogue_box(d,d.vector_128.x,d.vector_128.y,d.field_134,type);resize_dialogue_box(d,d.field_134);
        const unsigned slot=second?2:1;auto* animation=sprite::resolve_animation_handle(sprites(),d.handles[slot]);
        text::renderer->enqueue_task([&d,animation,value=std::move(value),slot,second]{text::set_next_outline_scale(.6000000238418579f);text::write_animation_text(sprites(),*animation,color(d),0x20000000,font(d),0,0,nullptr,[&d,slot,second]{text_complete(d,slot,true,second);},value.c_str());});
        if(d.fields_108[3]==0)position(d,1);else if(d.fields_108[3]==1||d.fields_108[3]==2)for(unsigned i:{1u,3u,2u,4u})position(d,i);
        if(!second)++d.fields_108[0];
    }
}
}
