#include "score.hpp"
#include "../sprite_renderer/pool.hpp"
#include <cstring>
#include <emmintrin.h>
namespace th20::source::small_score {
namespace {
float a(float x,float y){return recovered::add32(x,y);}float m(float x,float y){return recovered::mul32(x,y);}
float s(float x,float y){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(x),_mm_set_ss(y)));}float d(float x,float y){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(x),_mm_set_ss(y)));}
sprite::Vec3 player_position(const SmallScoreInf& owner){sprite::Vec3 p;std::memcpy(&p,static_cast<std::uint8_t*>(owner.context->objects_04[0])+0x614,sizeof(p));return p;}
}
int update(SmallScoreInf& owner,Environment& host){
    host.select_view(owner.view_index);
    for(unsigned i=0;i<13;++i){auto& entry=owner.entries[i];if(!entry.active)continue;entry.position.y=s(entry.position.y,m(state::clock_scale,entry.speed));entry.speed=m(entry.speed,.95f);recovered::timer_tick(entry.age,state::timer_rate);if(entry.age.current>60)entry.active=0;}
    for(unsigned i=13;i<18;++i){auto& entry=owner.entries[i];if(!entry.active)continue;recovered::timer_tick(entry.age,state::timer_rate);if(entry.age.current>60){const auto alpha=static_cast<int>(entry.color>>24)-4;if(alpha<1)entry.active=0;else entry.color=(static_cast<unsigned>(alpha)<<24)|(entry.color&0xffffffu);}}
    return 1;
}
int draw(SmallScoreInf& owner,Environment& host){
    host.configure_layer(21,owner.view_index);if(!host.fog_configuration())host.disable_fog();auto& animation=owner.animation;
    for(unsigned i=0;i<13;++i){auto& entry=owner.entries[i];if(!entry.active)continue;
        const auto spacing=entry.age.current<8?d(8,entry.age.current_f):8.f;
        animation.vector_5bc.x=s(entry.position.x,d(m(recovered::int_float(entry.length),spacing),2));animation.vector_5bc.y=entry.position.y;sprite::set_animation_color(animation,entry.color);
        const auto player=player_position(owner);const auto dx=s(player.x,entry.position.x),dy=s(player.y,entry.position.y);const auto distance=recovered::truncate32(a(m(dx,dx),m(dy,dy)));
        const auto alpha=distance>16384?std::uint8_t(255):distance>4096?static_cast<std::uint8_t>(((distance-4096)*128)/12288+128):std::uint8_t(128);
        for(unsigned remaining=entry.length;remaining>0;--remaining){const auto digit=entry.digits[remaining-1];int index=-1;
            if(entry.age.current<52-static_cast<int>(remaining)*2||digit==10)index=digit+0x121;
            else if(entry.age.current<56-static_cast<int>(remaining)*2)index=digit+300;
            else if(entry.age.current<60-static_cast<int>(remaining)*2)index=digit+0x136;
            if(index>=0){host.set_sprite(animation,index);animation.base.field_490=(animation.base.field_490&0xffffffu)|(static_cast<unsigned>(alpha)<<24);animation.base.vector_70.x=host.sprite_height(animation);animation.base.flags[1]|=4;host.draw_sprite(animation);}
            animation.vector_5bc.x=a(animation.vector_5bc.x,spacing);
        }
    }
    for(unsigned i=13;i<18;++i){const auto& entry=owner.entries[i];if(!entry.active)continue;
        host.text_vertical_alignment(2);host.text_horizontal_alignment(2);host.text_color(entry.color);host.text_style(0,0);
        sprite::Vec3 position{a(a(192,32),entry.position.x),a(entry.position.y,16),entry.position.z};
        if(entry.bonus<0)host.write_text(TextLine::no_bonus,position,0,0);
        else {host.write_text(TextLine::multiplier,position,entry.multiplier,0);position.y=a(position.y,11);host.write_text(TextLine::integer,position,0,entry.bonus);}
        host.text_color(0xffffffff);host.text_horizontal_alignment(0);host.text_vertical_alignment(0);host.text_style(1,1);
    }
    return 1;
}
}
