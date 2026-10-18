#include "effect.hpp"
#include <bit>
namespace th20::source::screen {
namespace n=th20::recovered;namespace e=environment;
namespace {
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
int signed_value(std::uint32_t v){return std::bit_cast<std::int32_t>(v);}
float unsigned_float(std::uint32_t value){return static_cast<float>(static_cast<double>(value));}
void tick(Effect& value){n::timer_tick(value.timer,e::timer_rate());}
void shake(Effect& value,int mode,float amplitude){for(unsigned axis=0;axis<2;++axis)e::apply_shake(mode,value.view_index,axis,e::random_direction(),amplitude);}
}
int update_fade_out(Effect& v){
    if(e::cancelled())return 7;
    if(v.duration){v.alpha=n::truncate32(sub(255.f,div(n::mul32(v.timer.current_f,255.f),n::int_float(v.duration))));if(v.alpha<0)v.alpha=0;}
    if(v.timer.current>=v.duration)return 7;tick(v);return 1;
}
int update_fade_in(Effect& v){
    if(e::cancelled())return 7;
    if(v.duration){v.alpha=v.timer.current<v.duration?n::truncate32(div(n::mul32(v.timer.current_f,255.f),n::int_float(v.duration))):255;if(v.alpha<0)v.alpha=0;}
    if(v.timer.current>=signed_value(static_cast<std::uint32_t>(v.duration)+2u))return 7;
    const auto* flags=e::game_flags();if(!flags||!(*flags&5))tick(v);return 1;
}
int update_hold(Effect& v){
    if(!v.phase){if(v.duration&&v.timer.current<=v.duration)v.alpha=n::truncate32(div(n::mul32(v.timer.current_f,128.f),n::int_float(v.duration)));}
    else{if(v.timer.current>8)return 7;v.alpha=signed_value(128u-static_cast<std::uint32_t>(n::truncate32(div(n::mul32(v.timer.current_f,128.f),8.f))));}
    tick(v);return 1;
}
int update_flashes(Effect& v){
    if(e::cancelled())return 7;const auto initial_alpha=v.argument_24>>24;
    if(v.timer.current<v.duration){v.alpha=signed_value(initial_alpha-static_cast<std::uint32_t>(n::truncate32(div(n::mul32(v.timer.current_f,unsigned_float(initial_alpha)),n::int_float(v.duration)))));if(v.alpha<0)v.alpha=0;}
    else{v.alpha=0;--v.argument_20;if(signed_value(v.argument_20)<1)return 7;n::timer_set(v.timer,0);}
    tick(v);return 1;
}
int update_solid(Effect& v){v.alpha=255;if(v.timer.current>=v.duration)return 7;tick(v);return 1;}
int update_linear_shake(Effect& v){
    if(e::cancelled())return 7;tick(v);if(v.timer.current>=v.duration)return 7;
    const float amplitude=n::add32(n::int_float(signed_value(v.argument_20)),div(n::mul32(v.timer.current_f,n::int_float(signed_value(v.argument_24-v.argument_20))),n::int_float(v.duration)));
    shake(v,1,amplitude);return 1;
}
int update_envelope_shake(Effect& v){
    if(e::cancelled())return 7;const auto* flags=e::game_flags();if(!flags||(*flags&0x77))return 1;
    tick(v);float weight;
    if(v.timer.current<signed_value(v.argument_20))weight=div(v.timer.current_f,n::int_float(signed_value(v.argument_20)));
    else if(v.timer.current<signed_value(v.argument_20+v.argument_24))weight=1.f;
    else{const auto total=v.argument_20+v.argument_24+v.argument_28;if(signed_value(total)<=v.timer.current)return 7;weight=div(unsigned_float(total-static_cast<std::uint32_t>(v.timer.current)),unsigned_float(v.argument_28));}
    shake(v,8,n::mul32(n::int_float(v.duration),weight));return 1;
}
}
