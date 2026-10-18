#include "../update.hpp"
namespace {
using FrameEvent=std::array<unsigned,20>;std::vector<FrameEvent> frame_events;
th20::source::game_session::Player frame_player{};
std::array<unsigned,6> frame_buttons{};std::array<std::uint8_t,0x620> frame_entity{};std::array<std::uint8_t,0xc0> frame_special{},frame_overlay{};
bool frame_overlay_filling=false,frame_special_active=false;unsigned frame_special_color=0;
void frame_record(unsigned type,unsigned a=0,unsigned b=0){FrameEvent event{};event[0]=type;event[1]=a;event[2]=b;frame_events.push_back(event);}
void __fastcall frame_sound(void*,void*,int id,int){frame_record(1,id);}
void __fastcall frame_select(void*,void*,int slot,int,int index){selected[slot]=index;frame_record(2,slot,index);}
void __fastcall frame_set_used(void*,void*,unsigned index,unsigned count){used[index]=count;frame_record(3,index,count);}
void __fastcall frame_consume(void*,void*,unsigned index){if(used[index]<available[index])++used[index];frame_record(4,index);}
void __fastcall frame_refresh_overlay(void*,void*,int character){frame_record(5,character);}
unsigned __fastcall frame_color(void*,void*){return frame_special_color;}
void prepare_frame_animation(unsigned handle,int script){auto& a=frame_animations[handle];std::memset(&a,0,sizeof(a));a.handle=handle;a.index=handle;a.base.field_440=static_cast<unsigned short>(script);a.base.vector_2c={float(handle)*3.f,float(handle)*7.f,0};a.base.vector_484={float(handle)*5.f,float(handle)*11.f,0};a.base.field_490=0xa0123456;a.links[3].value=&a;
    if(script==31){for(unsigned i=0;i<5;++i){auto& child=frame_animations[128+handle*5+i];std::memset(&child,0,sizeof(child));child.handle=128+handle*5+i;child.base.field_440=static_cast<unsigned short>(32+i);child.links[2].value=&child;child.links[2].next=i==4?nullptr:&frame_animations[128+handle*5+i+1].links[2];}a.links[3].next=&frame_animations[128+handle*5].links[2];}
}
unsigned frame_spawn_named(int script,const sp::Vec3* position,unsigned flags){const unsigned handle=frame_next++;prepare_frame_animation(handle,script);frame_record(6,script,flags);auto& e=frame_events.back();e[3]=handle;e[4]=position!=nullptr;if(position){e[5]=bits(position->x);e[6]=bits(position->y);e[7]=bits(position->z);frame_animations[handle].vector_5bc=*position;}return handle;}
unsigned* __fastcall frame_named(void*,void*,unsigned* output,const char*,int script,int,void*){*output=frame_spawn_named(script,nullptr,0);return output;}
unsigned* __fastcall frame_named_position(void*,void*,unsigned* output,const char*,int script,const sp::Vec3* position,float,int,void*){*output=frame_spawn_named(script,position,0);return output;}
unsigned* __fastcall frame_named_secondary(void*,void*,unsigned* output,const char*,int script,const sp::Vec3* position,void*){*output=frame_spawn_named(script,position,4);return output;}
unsigned frame_spawn_effect(int type,const th20::source::effects::Parameters* p,bool secondary){const unsigned handle=frame_next++;prepare_frame_animation(handle,type);frame_record(7,type,secondary);auto& event=frame_events.back();event[3]=handle;std::memcpy(event.data()+4,p,sizeof(*p));event[14]&=255;frame_animations[handle].vector_5bc=p->vector_00;return handle;}
unsigned* __fastcall frame_effect(void*,void*,unsigned* output,int type,const th20::source::effects::Parameters* p,void*){*output=frame_spawn_effect(type,p,false);return output;}
unsigned* __fastcall frame_effect_secondary(void*,void*,unsigned* output,int type,const th20::source::effects::Parameters* p,void*){*output=frame_spawn_effect(type,p,true);return output;}
void __fastcall frame_immediate(unsigned* handle,void*,int event){frame_record(8,*handle,event);if(auto* animation=animation_boundary(*handle)){animation->base.field_438=event;animation->base.vector_484={float(event)*10.f,float(event)*13.f,0};animation->base.vector_2c={float(event)*17.f,float(event)*19.f,0};}}
struct UpdateFixture final:sm::UpdateEnvironment {
    sm::DrawEnvironment& choices() override{return draw_host;}
    th20::source::game_session::Player& player() override{return frame_player;}
    bool pressed(unsigned mask) override{return (frame_buttons[4]&mask)!=0;}
    bool repeated(unsigned mask) override{return ((frame_buttons[4]|frame_buttons[2])&mask)!=0;}
    void sound(int id) override{frame_sound(nullptr,nullptr,id,0);}
    void select_profile(int slot,int index) override{frame_select(nullptr,nullptr,slot,0,index);}
    void set_used_stones(unsigned index,unsigned value) override{frame_set_used(nullptr,nullptr,index,value);}
    void consume_stone(unsigned index) override{frame_consume(nullptr,nullptr,index);}
    void refresh_overlay(int character) override{frame_refresh_overlay(nullptr,nullptr,character);}
    sp::Animation* animation(unsigned& handle,bool fallback) override{auto* a=animation_boundary(handle);if(!a)handle=0;return !a&&fallback?&frame_animations[0]:a;}
    sp::Animation* child_animation(unsigned& handle,int script) override{auto* a=animation(handle,false);return a?sp::find_animation_child(*a,script,0):nullptr;}
    void texture_rectangle(sp::Animation& a,float x,float y,float w,float h) override{sp::set_animation_texture_rectangle(a,sprite_descriptor,x,y,w,h);}
    void immediate_interrupt(unsigned handle,int event) override{frame_immediate(&handle,nullptr,event);}
    unsigned spawn_named(sp::AnimationFile&,int script,const sp::Vec3* p,unsigned flags) override{return frame_spawn_named(script,p,flags);}
    unsigned spawn_effect(int type,const th20::source::effects::Parameters& p,bool secondary) override{return frame_spawn_effect(type,&p,secondary);}
    bool overlay_filling() override{return frame_overlay_filling;}
    bool special_active() override{return frame_special_active;}
    unsigned special_color() override{return frame_special_color;}
    sp::Vec3 player_position() override{sp::Vec3 p;std::memcpy(&p,frame_entity.data()+0x614,12);return p;}
} update_host;
}
namespace th20::source::stone_menu {UpdateEnvironment& update_environment(){return update_host;}}
