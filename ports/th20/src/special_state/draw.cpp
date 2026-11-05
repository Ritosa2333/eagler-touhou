#include "environment.hpp"
namespace th20::source::special_state {
void draw(Controller& p,Environment& host){
    if(!p.active||!p.entries.sentinel.next||!(p.age.current<200))return;
    auto& entry=*reinterpret_cast<Entry*>(p.entries.sentinel.next->value);
    constexpr unsigned colors[]{0xffff4040,0xff4040ff,0xffffff00,0xff40ff40};
    host.text_style(0,0);host.text_font(12);host.text_color(p.age.current%8<6?colors[entry.color]:0xffffffff);
    const auto enemy=host.enemy_position(entry.enemy_handle);float x;
    if(enemy.x>=-112.f)x=enemy.x>112.f?112.f:enemy.x;else x=-112.f;
    sprite::Vec3 position{recovered::add32(x,224.f),160.f,0};
    auto lines=[&]{host.text_line(position,"Wonder Stone Appeared!");position.y=recovered::add32(position.y,16.f);host.text_level(position,unsigned(entry.level)+1);position.y-=16.f;};
    lines();const float expansion=evaluate(p.text_expansion);host.text_save(1);
    float scale=recovered::add32(expansion,1.f);host.text_scale(scale,scale);host.text_alpha(evaluate(p.text_alpha));lines();
    scale=recovered::add32(recovered::mul32(expansion,.66f),1.f);host.text_scale(scale,scale);lines();
    scale=recovered::add32(recovered::mul32(expansion,.33f),1.f);host.text_scale(scale,scale);lines();host.text_restore();
}
}
