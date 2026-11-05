#include "hud.hpp"
#include "dialogue.hpp"
#include "../gameplay/enemy_variables.hpp"
#include "../gameplay/enemy.hpp"
#include "../gameplay/stage_data.hpp"
#include "../program_entry/program_entry.hpp"
#include "../player_entity/player.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../effect_system/effect.hpp"
#include "../runtime_state/state.hpp"
#include "../audio_runtime/audio.hpp"
#include "../ecl_vm/math.hpp"
#include <algorithm>
#include <bit>
#include <cmath>
#include <cstring>
namespace th20::source::hud {
namespace s=sprite;
namespace {
template<class T>T read(const void* value,std::size_t offset){T result;std::memcpy(&result,static_cast<const std::uint8_t*>(value)+offset,sizeof(T));return result;}
float f(std::uint32_t value){return std::bit_cast<float>(value);}
void put(std::uint32_t& value,float v){value=std::bit_cast<std::uint32_t>(v);}
int integer(std::uint32_t value){return recovered::signed_bits(value);}
float add(float a,float b){return recovered::add32(a,b);}float mul(float a,float b){return recovered::mul32(a,b);}
constexpr float pi=3.1415927410125732421875f;
auto& sprites(){return environment::sprites();}
s::Animation* animation(std::uint32_t& handle){return s::resolve_animation_handle(sprites(),handle);}
void signal(std::uint32_t handle,int event){s::interrupt_animation_children(sprites(),handle,event);}
void remove(std::uint32_t& handle){s::request_animation_deletion(sprites(),handle);}
void sound(int id){program_entry::thread_registry.request_effect(id,0);}
void immediate(s::Animation& a,int event){a.base.field_438=event;s::execute_animation(a);} //477450 has no pre-dispatch callback
void alpha(s::Animation& a,unsigned value){a.base.field_490=(a.base.field_490&0xffffffu)|((value&255u)<<24);}
std::uint32_t spawn(FrontInf& o,int script,const char* name="front"){return s::spawn_named_animation(sprites(),*o.front_file,name,script);}
auto* enemy_controller(){return static_cast<gameplay::EnemyController*>(game_session::context(0).objects_04[1]);}
void* enemy(unsigned index){return gameplay::selected_enemy(enemy_controller(),index);}
std::uint32_t spell_flags(){return read<std::uint32_t>(game_session::context(0).objects_04[3],0x78);}
s::Vec3 player_position(){return player_entity::position(game_session::context(0).objects_04[0]);}
float distance_squared(const s::Vec3& a,const s::Vec3& b){const auto x=add(a.x,-b.x),y=add(a.y,-b.y);return add(mul(x,x),mul(y,y));}
void ensure_boss_label(FrontInf& o){ //4b8f60
    if(animation(o.handle_cc))return;
    auto& table=game_session::session.player_table;
    const auto value=std::clamp(gameplay::player_state::read<int>(table,0x1fc),0,999);gameplay::player_state::write(table,0x1fc,value);
    const int index=gameplay::selected_stage->fields_58[(value<41?0xc8-0x58:0x94-0x58)/4];
    if(index>=0)o.handle_cc=spawn(o,index+150,nullptr);
}
void timer_display(FrontInf& o){
    bool visible=false;
    if(enemy_controller()&&o.message_index>=0&&enemy(0)&&!(enemy_controller()->data.field_84&1u)&&!o.collecting&&!(gameplay::controller->game_flags&0x10000u)){
        visible=true;for(auto* a:o.number_animations)effects::enable_animation_tree(*a);
        const auto mode=(o.flags>>9)&3u;const auto y=player_position().y;const bool reversed=(spell_flags()&0x100u)!=0;
        if(mode==0){if((!reversed&&y<128)||(reversed&&y>448-128)){o.flags=(o.flags&~0x600u)|0x200u;for(auto* a:o.number_animations)s::set_animation_interrupt(*a,5);}}
        else if(mode==1){if((!reversed&&y<160)||(reversed&&y>448-160)){for(auto* a:o.number_animations)s::set_animation_interrupt(*a,4);o.flags&=~0x600u;}}
        else{const auto event=(spell_flags()&1u)?2:3;for(auto* a:o.number_animations)immediate(*a,event);for(auto* a:o.number_animations)immediate(*a,4);o.flags&=~0x600u;}
        if(o.message_index<o.field_1cc){if(o.message_index<2){for(auto* a:o.number_animations)s::set_animation_interrupt(*a,9);sound(12);}else if(o.message_index<5){for(auto* a:o.number_animations)s::set_animation_interrupt(*a,8);sound(11);}}
        else if(o.field_1cc<o.message_index)for(auto* a:o.number_animations)s::set_animation_interrupt(*a,7);
        if(o.message_index!=o.field_1cc){s::assign_animation_sprite(s::script_file(sprites(),*o.number_animations[0]),*o.number_animations[0],o.message_index/10+239);s::assign_animation_sprite(s::script_file(sprites(),*o.number_animations[1]),*o.number_animations[1],o.message_index%10+239);}
        o.field_1cc=o.message_index;
    }
    if(!visible){for(auto* a:o.number_animations)s::hide_animation_tree(*a);o.flags=(o.flags&~0x600u)|0x400u;}
}
void boss_panels(FrontInf& o){
    if(!enemy_controller()||(enemy_controller()->data.field_84&1u))return;
    for(unsigned i=0;i<2;++i){
        auto& panel=o.panels[i];auto* entity=enemy(i);
        if(!entity){panel.fields_00[0]=0;for(auto& pair:panel.pairs)pair.first=0;if(panel.field_4c){for(auto& handle:panel.handles)remove(handle);panel.field_4c=0;}if(i==0)remove(o.handle_cc);continue;}
        const auto hp=read<int>(entity,0x214),maximum=read<int>(entity,0x218);const bool excluded=(read<unsigned>(entity,0x350)&0x31u)!=0||read<int>(entity,0x314)>0;
        if(hp<100000&&!excluded&&!o.collecting){
            panel.fields_00[2]=hp;put(panel.fields_00[1],recovered::int_float(hp)/recovered::int_float(maximum));
            if(f(panel.fields_00[0])<f(panel.fields_00[1]))put(panel.fields_00[0],add(f(panel.fields_00[0]),0.02500000037252903f));
            if(f(panel.fields_00[1])<f(panel.fields_00[0]))panel.fields_00[0]=panel.fields_00[1];
            if(!panel.field_4c){for(unsigned j=0;j<7;++j)panel.handles[j]=spawn(o,j<3?374+j:377);panel.field_4c=1;}
            ensure_boss_label(o);
            auto& ring=*animation(panel.handles[0]);ring.base.vector_38.x=mul(-f(panel.fields_00[0]),mul(pi,2));ring.base.flags[1]|=2;
            auto location=gameplay::enemy_position(entity);location.x=mul(location.x,2);location.y=mul(location.y,2);
            for(unsigned j=0;j<3;++j)animation(panel.handles[j])->vector_5bc=location;
            s::Vec3 marker_position{}; //458fa0 writes XY only; Z accumulates through4296e0 for each visible marker.
            for(unsigned j=0;j<4;++j){auto& marker=*animation(panel.handles[j+3]);const float fraction=f(panel.pairs[j].first);
                if(fraction==0||f(panel.fields_00[0])<=fraction)s::hide_animation_tree(marker);
                else{effects::enable_animation_tree(marker);const auto angle=ecl::math::wrap_angle(add(-mul(pi,2)/2,-mul(mul(pi,2),fraction)));marker.base.vector_38.z=angle;marker.base.flags[1]|=2;
                    const float sine=static_cast<float>(std::sin(static_cast<double>(angle))),cosine=static_cast<float>(std::cos(static_cast<double>(angle)));
                    marker_position.x=add(-mul(112,sine),location.x);marker_position.y=add(mul(112,cosine),location.y);marker_position.z=add(marker_position.z,location.z);marker.vector_5bc=marker_position;
                }
            }
            const auto distance=distance_squared(gameplay::enemy_position(entity),player_position());
            if(!panel.field_50){if(distance<mul(80,80)){for(auto handle:panel.handles)signal(handle,3);panel.field_50=1;}}
            else if(mul(96,96)<=distance){for(auto handle:panel.handles)signal(handle,2);panel.field_50=0;}
        }else if(panel.field_4c){for(auto& handle:panel.handles)remove(handle);panel.field_4c=0;}
    }
}
void boss_pointer(FrontInf& o){
    if(!enemy_controller())return;
    auto* target=enemy(0);
    if(!target||(read<unsigned>(target,0x350)&0x21u)){if(auto* a=animation(o.handle_90))s::hide_animation_tree(*a);return;}
    auto& a=*animation(o.handle_90);effects::enable_animation_tree(a);
    const bool spell=(spell_flags()&1u)!=0;const auto hp=read<int>(target,0x21c);const auto mode=(o.flags>>1)&3u;
    if(mode==0&&hp<(spell?2000:700)){s::set_animation_interrupt(a,7);o.flags=(o.flags&~6u)|2u;}
    else if(mode==1&&hp<(spell?1000:400)){s::set_animation_interrupt(a,8);o.flags=(o.flags&~6u)|4u;}
    else if(mode==2&&hp<(spell?400:200)){s::set_animation_interrupt(a,9);o.flags|=6u;}
    else if(mode==3&&hp>(spell?400:200)){s::set_animation_interrupt(a,10);o.flags&=~6u;}
    const auto position=gameplay::enemy_position(target);a.vector_5bc.x=mul(add(add(position.x,32),192),2);a.vector_5bc.y=960;
    const auto distance=std::fabs(add(position.x,-player_position().x));if(static_cast<double>(distance)<64.0)alpha(a,static_cast<unsigned>(recovered::truncate32(mul(191,distance)/64))+64u);else alpha(a,255);
    if(position.x<-192||192<position.x)alpha(a,0);
}
}
int update(FrontInf& o){
    if(o.flags&0x100u)recovered::timer_tick(o.secondary_age,state::timer_rate);
    if((o.flags>>11)&3u){
        recovered::timer_tick(o.secondary_age,state::timer_rate);
        if(((o.flags>>11)&3u)==1&&o.secondary_age.current>=90){
            if(f(o.fields_11c[1])<=0){if(o.secondary_age.current!=90)sound(47);o.fields_11c[1]=o.fields_11c[2];o.fields_11c[5]=0;o.fields_11c[4]=o.fields_11c[3];o.flags=(o.flags&~0x1800u)|0x1000u;}
            else{if(o.secondary_age.current%4==0)sound(39);put(o.fields_11c[1],add(f(o.fields_11c[1]),-1));o.fields_11c[4]+=o.fields_11c[5];}
        }
        if(o.secondary_age.current>=integer(o.field_1b8)){signal(o.handles_f8[8],1);o.flags=(o.flags&~0x1800u)|0x800u;recovered::timer_set(o.secondary_age,0);o.flags&=~0x1800u;}
    }
    if(o.fields_11c[7]&&!animation(o.handles_f8[7]))o.fields_11c[7]=0;
    timer_display(o);boss_panels(o);
    for(unsigned i=0;i<10;++i){if(static_cast<int>(i)<integer(o.fields_174[3])){if(o.handles_d0[i]==0)o.handles_d0[i]=spawn(o,i+58);}else if(o.handles_d0[i]){signal(o.handles_d0[i],1);o.handles_d0[i]=0;}}
    if(o.collecting&&update_dialogue(*o.collecting)){destroy_dialogue(o.collecting);o.collecting=nullptr;}
    boss_pointer(o);recovered::timer_tick(o.age,state::timer_rate);return 1;
}
}
