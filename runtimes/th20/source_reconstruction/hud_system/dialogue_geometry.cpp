#include "dialogue.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/menu_animation.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include <bit>
#include <stdexcept>
namespace th20::source::hud {
namespace {
char text_buffer[256]; //actual5c4a20, nextBSS5c4b20
bool lead(unsigned c){return (0x81<=c&&c<=0x9f)||(0xe0<=c&&c<=0xfc);}
bool box_position(Dialogue& d,sprite::Vec3& position,float spacing){
    auto* a=sprite::find_animation_child(environment::sprites(),d.handles[6],static_cast<int>(d.fields_138[1])+166,0);if(!a)return false;
    position=sprite::animation_position(*a);const float scale=2.f/program_entry::window_state.scale;position.x=recovered::mul32(scale,position.x);position.y=recovered::mul32(scale,position.y);
    const auto mode=d.fields_108[3];const auto xscale=a->base.vector_50.x;
    if(mode==1)position.x=recovered::add32(recovered::add32(recovered::mul32(xscale>=1?1:recovered::add32(xscale,.125f),32),-spacing),position.x);
    else if(mode==0)position.x=recovered::add32(position.x,-36);
    else if(mode==2)position.x=recovered::add32(recovered::add32(recovered::mul32(recovered::add32(xscale,.125f),16),-spacing),position.x);
    return true;
}
}
const char* decode_dialogue_text(const std::uint8_t* bytes){
    unsigned offset=0;std::uint8_t key=0x77,step=7;
    do{if(offset==sizeof(text_buffer))throw std::length_error("Dialogue text exceeds original 256-byte buffer");const auto value=static_cast<std::uint8_t>(*bytes++^key);text_buffer[offset++]=static_cast<char>(value);key+=step;step+=16;if(value==0)break;}while(true);
    for(unsigned i=0;text_buffer[i];++i){if(lead(static_cast<std::uint8_t>(text_buffer[i])))++i;else if(text_buffer[i]=='_')text_buffer[i]=' ';}return text_buffer;
}
void resize_dialogue_box(Dialogue& d,float width){for(int i=0;;++i){auto* a=sprite::find_animation_child(environment::sprites(),d.handles[6],-1,i);if(!a)break;a->base.fields_444[4]=std::bit_cast<unsigned>(recovered::add32(width,16));}}
void create_dialogue_box(Dialogue& d,float x,float y,float width,int type){
    auto& c=environment::sprites();sprite::request_animation_deletion(c,d.handles[6]);const sprite::Vec3 position{x,y,0};sprite::spawn_named_animation(c,*controller->front_file,d.handles[6],"front",type+270,&position,0,-1,0);
    for(int i=0;;++i){auto* a=sprite::find_animation_child(c,d.handles[6],-1,i);if(!a)break;a->base.fields_444[4]=std::bit_cast<unsigned>(width);}d.fields_138[1]=type;
}
void follow_dialogue_box(Dialogue& d,sprite::Animation& a){sprite::Vec3 position{};if(box_position(d,position,8))a.vector_5bc=position;}
void position_dialogue_text(Dialogue& d){sprite::Vec3 position{};if(box_position(d,position,6))for(unsigned i=1;i<=4;++i)if(auto* a=sprite::find_animation(environment::sprites(),d.handles[i]))a->vector_5bc=position;}
}
