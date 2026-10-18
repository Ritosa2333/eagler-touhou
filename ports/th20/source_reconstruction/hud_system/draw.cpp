#include "hud.hpp"
#include "draw_support.hpp"
#include "../gameplay/player_state.hpp"
#include "../gameplay/enemy.hpp"
#include "../gameplay/enemy_variables.hpp"
#include "../gameplay/subsystems.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include <algorithm>
#include <stdexcept>
namespace th20::source::hud {
namespace n=recovered;namespace d=draw_detail;namespace gs=game_session;namespace gp=gameplay;namespace ps=gp::player_state;namespace s=sprite;
namespace {
int clamp_field(gs::Player& player,unsigned offset,int low,int high){const auto value=std::clamp(ps::read<int>(player,offset),low,high);ps::write(player,offset,value);return value;}
int continuation_digit(){auto& value=gs::session.player_table.continue_count;value=std::clamp(value,0,9);return value;} //4b7fa0/4b7fc0
std::uint32_t life_threshold(gs::Player& player){ //4e15c0 and4e1570
    const int index=clamp_field(player,0xc4,0,100);
    // 31 normal entries followed by the ten Extra entries in the original data.
    // Original indices beyond 40 read unrelated adjacent objects, not a defined table element.
    if(index>40)throw std::out_of_range("HUD life threshold outside original 41-entry data span");
    return index==30||index==40?99999999u:3u;
}
template<class T>T owner_field(const void* owner,unsigned offset){T value;std::memcpy(&value,static_cast<const std::uint8_t*>(owner)+offset,sizeof(value));return value;}
void best_spell_time(const void* owner,int& seconds,int& hundredths){ //4b7ec0/4885e0
    const int encoded=owner_field<int>(owner,0xa8);seconds=((encoded/100)%1000+934)%1000;hundredths=(encoded%100+67)%100;
    if(encoded/100000-22!=seconds+hundredths){seconds=999;hundredths=99;}
}
}
int draw(FrontInf& hud){ //4b4730, actual return EAX=1 at4b576f
    auto& renderer=*text::renderer;auto& sprites=environment::sprites();auto& player=*gs::context(0).current_player;
    if(hud.fields_11c[7]){
        s::Vec3 position{224,144,0};auto* animation=s::resolve_animation_handle(sprites,hud.handles_f8[7]);const auto* spell=gs::context(0).objects_04[3];
        if(!spell){hud.fields_11c[7]=0;s::request_animation_deletion(sprites,hud.handles_f8[7]);}
        else if(!animation)hud.fields_11c[7]=0;
        else{
            d::color_alpha(renderer,std::uint8_t(animation->base.field_490>>24));d::layer(renderer,2);d::font(renderer,4);
            const int seconds=std::min(owner_field<int>(spell,0x90)/60,999);renderer.write_ascii_format(position,"%3d.",seconds);d::scale(renderer,.6f,.6f);position.x=n::add32(position.x,44);position.y=n::add32(position.y,6);
            const int hundredths=((owner_field<int>(spell,0x90)%60)*100)/60;renderer.write_ascii_format(position,"%.2ds",hundredths);
            position={224,160,0};renderer.color=0xff808080;d::color_alpha(renderer,std::uint8_t(animation->base.field_490>>24));d::scale(renderer,1,1);int best_seconds,best_hundredths;best_spell_time(spell,best_seconds,best_hundredths);renderer.write_ascii_format(position,"%3d.",best_seconds);d::scale(renderer,.6f,.6f);position.x=n::add32(position.x,44);position.y=n::add32(position.y,6);renderer.write_ascii_format(position,"%.2ds",best_hundredths);
            d::scale(renderer,1,1);d::color_alpha(renderer,255);d::font(renderer,0);d::layer(renderer,0);renderer.color=0xffffffff;
        }
    }
    const auto inherit_life_alpha=[&]{const auto alpha=std::uint8_t(hud.life_animations[0]->base.field_490>>24);d::color_alpha(renderer,alpha);d::shadow_alpha(renderer,alpha);};
    d::font(renderer,10);renderer.color=0xff000000;renderer.shadow_color=0xffffffff;d::layer(renderer,3);s::Vec3 position{620,42,0};renderer.color=0xff707070;renderer.shadow_color=0x80ffffff;inherit_life_alpha();d::alignment(renderer,2,1);
    // The original fetches a uint64 but deliberately sign-extends only its low word for this call.
    renderer.write_grouped_score(position,static_cast<std::uint64_t>(static_cast<std::int64_t>(n::signed_bits(gs::session.field_60))),n::signed_bits(gs::session.field_68));
    position.y=64;renderer.color=0xff001080;renderer.shadow_color=0xd0ffffff;inherit_life_alpha();const int digit=continuation_digit();renderer.write_grouped_score(position,static_cast<std::uint32_t>(hud.score),digit);d::alignment(renderer,1,1);
    position={576.f-(life_threshold(player)<100?0.f:7.f),120,0};d::scale(renderer,.6f,.6f);renderer.color=0xff000000;renderer.shadow_color=0xffffffff;inherit_life_alpha();
    if(life_threshold(player)<999999){renderer.write_padded_integer(position,clamp_field(player,0xc0,0,10),3,' ');position.x=n::add32(position.x,21);renderer.write_prefixed_integer(position,"/",n::signed_bits(life_threshold(player)));}
    else renderer.write_ascii_format(position,"-/-");
    position={576,158,0};inherit_life_alpha();d::scale(renderer,.6f,.6f);renderer.write_padded_integer(position,clamp_field(player,0xd0,0,10),3,' ');position.x=n::add32(position.x,21);renderer.write_prefixed_integer(position,"/",3);d::scale(renderer,1,1);
    position={540,182,0};renderer.color=0xff800000;renderer.shadow_color=0x80ffd0d0;inherit_life_alpha();
    const int power=clamp_field(player,0x30,0,400);const int divisor=clamp_field(player,0x38,100,400);renderer.write_integer_suffix(position,power/divisor,".");d::scale(renderer,.6f,.6f);position.x=n::add32(position.x,20);position.y=n::add32(position.y,7);
    const int power_remainder=clamp_field(player,0x30,0,400)%clamp_field(player,0x38,100,400);renderer.write_padded_integer(position,(power_remainder*100)/clamp_field(player,0x38,100,400),2,'0');d::scale(renderer,1,1);position.y-=7;position.x=n::add32(position.x,14);
    const int maximum=clamp_field(player,0x34,400,400),maximum_divisor=clamp_field(player,0x38,100,400);renderer.write_affixed_integer(position,"/",maximum/maximum_divisor,".");position.x=n::add32(position.x,32);position.y=n::add32(position.y,7);d::scale(renderer,.6f,.6f);renderer.write_ascii(position,"00");d::scale(renderer,1,1);
    d::alignment(renderer,2,1);renderer.color=0xff701070;renderer.shadow_color=0x80f0c0f0;inherit_life_alpha();position={608,204,0};const int resource=clamp_field(player,0x44,0,1000000),resource_divisor=clamp_field(player,0x48,5000,10000);renderer.write_integer_suffix(position,resource/resource_divisor,".");d::scale(renderer,.6f,.6f);position.x=n::add32(position.x,12);position.y=n::add32(position.y,7);
    const int resource_remainder=clamp_field(player,0x44,0,1000000)%clamp_field(player,0x48,5000,10000);renderer.write_padded_integer(position,(resource_remainder*100)/clamp_field(player,0x48,5000,10000),2,'0');d::scale(renderer,1,1);position.y-=7;position.x=n::add32(position.x,14);
    d::alignment(renderer,1,1);d::scale(renderer,1,1);renderer.color=0xffffffff;renderer.shadow_color=0xff000000;d::color_alpha(renderer,255);d::font(renderer,0);d::layer(renderer,0);
    auto* enemies=static_cast<gp::EnemyController*>(gs::context(0).objects_04[1]);
    if(enemies && hud.message_index>=0 && gp::find_enemy_in_list(enemies->enemies,enemies->data.handles_44[0]) && !(enemies->data.field_84&1) && !hud.collecting){
        auto* pause=gp::unrecovered::owner(gp::Owner::global_005c60bc);
        if(owner_field<int>(pause,0xd0)==0 && !(gp::controller->game_flags&0x10000)){
            auto& number=*hud.number_animations[0];position={n::add32(number.base.vector_2c.x,16),number.base.vector_2c.y-7,0};renderer.color=number.base.field_490;d::font(renderer,4);d::layer(renderer,2);renderer.write_ascii_format(position,".");position.x=n::add32(position.x,8);position.y=n::add32(position.y,6);d::scale(renderer,.6f,.6f);renderer.write_ascii_format(position,"%.2d",n::signed_bits(hud.field_1c8));d::scale(renderer,1,1);renderer.color=0xffffffff;d::layer(renderer,0);d::font(renderer,0);
        }
    }
    return 1;
}
}
